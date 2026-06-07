/*
 * ===========================================================================
 *  Copyright (c) 2026 Psychostout. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "PsychobotAI.h"
#include "PsychobotStrategies.h"
#include "PsychobotClassAI.h"
#include "PsychobotTalentMgr.h"
#include "PsychobotGearMgr.h"
#include "PsychobotPopulationMgr.h"
#include "PsychobotGroupMgr.h"
#include "Player.h"
#include "Unit.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "SpellMgr.h"
#include "SpellInfo.h"
#include "DB2Stores.h"
#include "DBCEnums.h"
#include "Log.h"
#include <cctype>
#include <algorithm>
#include <cmath>

namespace psychobot
{
    // Throttle: run the decision engine ~ every 250ms (4x/sec) to stay cheap.
    static uint32 const PSYCHOBOT_AI_TICK_MS = 250;

    PsychobotAI::PsychobotAI(Player* bot, ObjectGuid masterGuid)
        : _bot(bot), _masterGuid(masterGuid), _state(BotState::NonCombat),
          _updateDelay(0), _specApplied(false), _currentEngine(nullptr)
    {
        _nonCombatEngine = std::make_unique<Engine>(this);
        _combatEngine    = std::make_unique<Engine>(this);
        _classAI         = std::make_unique<ClassAI>(this);

        // Install the generic strategies (follow / class-rotation combat).
        BuildNonCombatEngine(this, _nonCombatEngine.get());
        BuildCombatEngine(this, _combatEngine.get());

        _currentEngine = _nonCombatEngine.get();
    }

    void PsychobotAI::EnsureSpecAndTalents()
    {
        if (_specApplied || !_bot)
            return;
        // Stage 2: give the bot its class default spec + a basic talent build.
        TalentMgr::SetupSpec(_bot, /*specIndex*/ -1, /*preferColumn*/ 0);
        _specApplied = true;
    }

    std::string PsychobotAI::DoClassRotation(Unit* target)
    {
        return _classAI ? _classAI->DoRotation(target) : std::string();
    }

    PsychobotAI::~PsychobotAI() = default;

    Player* PsychobotAI::GetMaster() const
    {
        if (_masterGuid.IsEmpty())
            return nullptr;
        return ObjectAccessor::FindConnectedPlayer(_masterGuid);
    }

    void PsychobotAI::UpdateState()
    {
        BotState desired = IsInCombat() ? BotState::Combat : BotState::NonCombat;
        if (desired != _state)
        {
            _state = desired;
            _currentEngine = (_state == BotState::Combat)
                ? _combatEngine.get() : _nonCombatEngine.get();
        }
    }

    void PsychobotAI::UpdateAI(uint32 diff)
    {
        if (!_bot || !_bot->IsInWorld())
            return;

        // Dead bots do nothing in Stage 1 (rez handling is a later stage).
        if (!_bot->IsAlive())
            return;

        _updateDelay += diff;
        if (_updateDelay < PSYCHOBOT_AI_TICK_MS)
            return;
        _updateDelay = 0;

        // Stage 3 scaling gate: SmartScale / BotActiveAlone /
        // DisabledWithoutRealPlayer may pause this bot's AI to save CPU.
        // (Master-owned alts stay active when random-bot scaling is disabled.)
        if (!sPsychobotPopulation->ShouldBotBeActive(_bot))
            return;

        // One-time: ensure the bot has a spec + talents (Stage 2).
        EnsureSpecAndTalents();

        // Stage 3: keep gear roughly level-appropriate (cheap; persistence-gated).
        GearMgr::GearUp(_bot);

        UpdateState();

        if (_currentEngine)
            _currentEngine->DoNextAction();
    }

    // ----------------------------------------------------------------------
    // ServerFacade-style seam (TrinityCore BfA 8.3 core API lives ONLY here)
    // ----------------------------------------------------------------------
    bool PsychobotAI::IsAlive(Unit* unit) const
    {
        return unit && unit->IsAlive();
    }

    bool PsychobotAI::IsInCombat() const
    {
        return _bot && _bot->IsInCombat();
    }

    Unit* PsychobotAI::GetCurrentTarget() const
    {
        if (!_bot)
            return nullptr;

        // Prefer the bot's own victim if it's still valid.
        if (Unit* victim = _bot->GetVictim())
            return victim;

        // In a group: assist the tank/leader (Stage 4 group play).
        if (Unit* assist = GroupMgr::GetGroupAssistTarget(_bot))
            return assist;

        // Else mirror the master's selection.
        if (Player* master = GetMaster())
            if (Unit* sel = master->GetSelectedUnit())
                return sel;

        return nullptr;
    }

    float PsychobotAI::GetDistance(Unit* to) const
    {
        if (!_bot || !to)
            return 99999.0f;
        return _bot->GetDistance(to);
    }

    bool PsychobotAI::HasSpell(uint32 spellId) const
    {
        return _bot && spellId && _bot->HasSpell(spellId);
    }

    uint32 PsychobotAI::GetSpellIdByName(std::string const& name) const
    {
        if (name.empty())
            return 0;

        // Lowercase the query once.
        std::string lower = name;
        std::transform(lower.begin(), lower.end(), lower.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

        // Scan the DB2 SpellName store for a known spell whose name matches.
        // (Stage 1: simple linear scan; a cached name->id map is a later optimization.)
        for (SpellNameEntry const* entry : sSpellNameStore)
        {
            if (!entry)
                continue;

            char const* spellName = entry->Name[LOCALE_enUS];
            if (!spellName || !*spellName)
                continue;

            std::string sn = spellName;
            std::transform(sn.begin(), sn.end(), sn.begin(),
                [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

            if (sn == lower && _bot && _bot->HasSpell(entry->ID))
                return entry->ID;
        }
        return 0;
    }

    bool PsychobotAI::CastSpell(uint32 spellId, Unit* target)
    {
        if (!_bot || !spellId || !target)
            return false;

        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId, DIFFICULTY_NONE);
        if (!spellInfo)
            return false;

        if (!_bot->HasSpell(spellId))
            return false;

        // Don't double-cast while already casting.
        if (_bot->IsNonMeleeSpellCast(false))
            return false;

        _bot->CastSpell(target, spellId, false);
        return true;
    }

    bool PsychobotAI::CastSpell(std::string const& name, Unit* target)
    {
        uint32 id = GetSpellIdByName(name);
        if (!id)
            return false;
        return CastSpell(id, target);
    }

    // ----------------------------------------------------------------------
    // Movement seam
    // ----------------------------------------------------------------------
    void PsychobotAI::FollowMaster()
    {
        Player* master = GetMaster();
        if (!_bot || !master || !master->IsInWorld())
            return;

        if (master->GetMap() != _bot->GetMap())
            return; // cross-map teleport handled in a later stage

        // Standard follow: 2.0 yd behind, slightly to the side.
        _bot->GetMotionMaster()->MoveFollow(master, 2.0f, static_cast<float>(M_PI));
    }

    void PsychobotAI::StopMoving()
    {
        if (_bot)
            _bot->GetMotionMaster()->Clear();
    }

    bool PsychobotAI::IsMoving() const
    {
        if (!_bot || _bot->GetMotionMaster()->empty())
            return false;
        return _bot->GetMotionMaster()->GetCurrentMovementGeneratorType() != IDLE_MOTION_TYPE;
    }
}
