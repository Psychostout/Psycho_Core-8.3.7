/*
 * ===========================================================================
 *  Copyright (c) 2026 Psychostout. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  PsychobotAI - per-bot brain. Wraps a real TrinityCore Player and drives it
 *  each tick via the Psychobot Engine. (Stage 1: foundation + main AI.)
 * ===========================================================================
 */

#ifndef PSYCHOBOT_AI_H
#define PSYCHOBOT_AI_H

#include "Define.h"
#include "ObjectGuid.h"
#include "PsychobotEngine.h"
#include <memory>

class Player;
class Unit;

namespace psychobot
{
    enum class BotState : uint8
    {
        NonCombat = 0,
        Combat    = 1
    };

    class PsychobotAI
    {
    public:
        PsychobotAI(Player* bot, ObjectGuid masterGuid);
        ~PsychobotAI();

        // Driven each world tick by PsychobotMgr.
        void UpdateAI(uint32 diff);

        Player* GetBot() const { return _bot; }
        Player* GetMaster() const;            // resolved live from guid (may be null)
        ObjectGuid GetMasterGuid() const { return _masterGuid; }

        // --- ServerFacade-style helpers (the TC 8.3 core-API seam) ---------
        // These are the only places that touch core combat/movement/spell APIs;
        // engine/strategy/action code calls THESE, not the core directly.
        bool   IsAlive(Unit* unit) const;
        bool   IsInCombat() const;
        Unit*  GetCurrentTarget() const;       // bot's victim / master's target
        float  GetDistance(Unit* to) const;
        bool   HasSpell(uint32 spellId) const;
        bool   CastSpell(uint32 spellId, Unit* target);
        bool   CastSpell(std::string const& name, Unit* target);
        uint32 GetSpellIdByName(std::string const& name) const;

        // Movement seam.
        void   FollowMaster();
        void   StopMoving();
        bool   IsMoving() const;

        BotState GetState() const { return _state; }

    private:
        void   UpdateState();                  // combat vs non-combat engine swap

        Player*    _bot;
        ObjectGuid _masterGuid;
        BotState   _state;
        uint32     _updateDelay;               // ms accumulator (throttle)

        std::unique_ptr<Engine> _nonCombatEngine;
        std::unique_ptr<Engine> _combatEngine;
        Engine*    _currentEngine;
    };
}

#endif // PSYCHOBOT_AI_H
