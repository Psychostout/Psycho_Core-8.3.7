/*
 * ===========================================================================
 *  Copyright (c) 2026 Psychostout. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "PsychobotClassAI.h"
#include "PsychobotAI.h"
#include "PsychobotGroupMgr.h"
#include "Player.h"
#include "Unit.h"
#include "DB2Stores.h"
#include "DB2Structure.h"

namespace psychobot
{
    std::string ClassAI::DoRotation(Unit* target)
    {
        Player* bot = _ai ? _ai->GetBot() : nullptr;
        if (!bot)
            return std::string();

        uint8 classId = bot->getClass();

        // Resolve the bot's current spec -> OrderIndex (0..n). Fall back to 0.
        uint8 specIndex = ClassRegistry::GetDefaultSpecIndex(classId);
        if (uint32 specId = bot->GetPrimarySpecialization())
            if (ChrSpecializationEntry const* spec = sChrSpecializationStore.LookupEntry(specId))
                specIndex = static_cast<uint8>(spec->OrderIndex);

        SpecRotation const* rotation = ClassRegistry::GetRotation(classId, specIndex);
        if (!rotation)
            return std::string();

        // Healers cast on the most-injured friendly group member, not the enemy.
        if (rotation->role == SpecRole::Healer)
        {
            Unit* healTarget = GroupMgr::GetGroupHealTarget(bot, 80.0f);
            if (!healTarget)
                return std::string();   // nobody needs healing right now
            for (RotationEntry const& entry : rotation->priority)
            {
                uint32 spellId = _ai->GetSpellIdByName(entry.spell);
                if (!spellId)
                    continue;
                if (_ai->CastSpell(spellId, healTarget))
                    return entry.spell;
            }
            return std::string();
        }

        // DPS / Tank need an enemy target.
        if (!target)
            return std::string();

        float const meleeRange = 5.0f;
        bool inMelee = _ai->GetDistance(target) <= meleeRange;

        // Highest priority first: cast the first spell the bot KNOWS and can use.
        for (RotationEntry const& entry : rotation->priority)
        {
            // Melee spells require melee range; ranged/instant always allowed.
            if (entry.melee && !inMelee)
                continue;

            // Resolve name -> a spell the bot actually knows; skip if unknown.
            uint32 spellId = _ai->GetSpellIdByName(entry.spell);
            if (!spellId)
                continue;

            if (_ai->CastSpell(spellId, target))
                return entry.spell;
        }

        return std::string();
    }
}
