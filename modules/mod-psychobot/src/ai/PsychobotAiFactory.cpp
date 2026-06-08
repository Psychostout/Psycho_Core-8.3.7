/*
 * ===========================================================================
 *  Copyright (c) 2026 Psychostout. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "PsychobotAiFactory.h"
#include "PsychobotSpecRoles.h"
#include "../engine/AiObjectContext.h"
#include "../engine/Engine.h"
#include "../engine/Strategy.h"
#include "../values/CoreValues.h"
#include "../triggers/CoreTriggers.h"
#include "../actions/GenericSpellActions.h"
#include "../actions/MovementActions.h"
#include "../strategies/GenericStrategies.h"
#include "../classes/deathknight/DKAiObjectContext.h"
#include "../classes/warrior/WarriorAiObjectContext.h"
#include "../classes/paladin/PaladinAiObjectContext.h"
#include "../classes/hunter/HunterAiObjectContext.h"
#include "../classes/rogue/RogueAiObjectContext.h"
#include "../classes/priest/PriestAiObjectContext.h"
#include "../classes/shaman/ShamanAiObjectContext.h"
#include "../classes/mage/MageAiObjectContext.h"
#include "../PsychobotAIFwd.h"
#include "Player.h"
#include "SharedDefines.h"   // CLASS_*

namespace psychobot
{
    namespace AiFactory
    {
        AiObjectContext* CreateContext(PsychobotAI* ai)
        {
            AiObjectContext* context = new AiObjectContext(ai);

            // Base context: generic vocabulary + behaviour (S4/S5/S6).
            RegisterCoreValues(context);
            RegisterCoreTriggers(context);
            RegisterGenericSpellActions(context);
            RegisterMovementActions(context);
            RegisterGenericStrategies(context);

            // Per-class contexts (S7+) layered on top of the base for this bot.
            Player* bot = PsychobotAIBridge::GetBot(ai);
            uint8 classId = bot ? bot->getClass() : 0;
            switch (classId)
            {
                case CLASS_DEATH_KNIGHT: DeathKnight::RegisterContext(context); break;
                case CLASS_WARRIOR:      Warrior::RegisterContext(context);    break;
                case CLASS_PALADIN:      Paladin::RegisterContext(context);    break;
                case CLASS_HUNTER:       Hunter::RegisterContext(context);     break;
                case CLASS_ROGUE:        Rogue::RegisterContext(context);      break;
                case CLASS_PRIEST:       Priest::RegisterContext(context);     break;
                case CLASS_SHAMAN:       Shaman::RegisterContext(context);     break;
                case CLASS_MAGE:         Mage::RegisterContext(context);       break;
                // S15+ : other classes register their contexts here.
                default: break;
            }
            return context;
        }

        void InitNonCombatEngine(PsychobotAI* ai, Engine* engine)
        {
            if (!engine)
                return;

            engine->AddStrategy("follow");

            // Class-specific non-combat upkeep.
            Player* bot = PsychobotAIBridge::GetBot(ai);
            uint8 classId = bot ? bot->getClass() : 0;
            if (classId == CLASS_DEATH_KNIGHT)
                engine->AddStrategy("dk nc");
            else if (classId == CLASS_WARRIOR)
                engine->AddStrategy("warrior nc");
            else if (classId == CLASS_PALADIN)
                engine->AddStrategy("paladin nc");
            else if (classId == CLASS_HUNTER)
                engine->AddStrategy("hunter nc");
            else if (classId == CLASS_ROGUE)
                engine->AddStrategy("rogue nc");
            else if (classId == CLASS_PRIEST)
                engine->AddStrategy("priest nc");
            else if (classId == CLASS_SHAMAN)
                engine->AddStrategy("shaman nc");
            else if (classId == CLASS_MAGE)
                engine->AddStrategy("mage nc");
        }

        void InitCombatEngine(PsychobotAI* ai, Engine* engine)
        {
            if (!engine)
                return;

            Player* bot = PsychobotAIBridge::GetBot(ai);
            uint8 classId = bot ? bot->getClass() : 0;
            SpecRole role = SpecRoles::GetBotRole(bot);
            uint8 specIndex = SpecRoles::GetBotSpecIndex(bot);

            // --- class-specific combat strategies (S7+) ----------------------
            if (classId == CLASS_DEATH_KNIGHT)
            {
                // Spec rotation (blood/frost/unholy) on top of the shared DK base.
                engine->AddStrategy("dk");
                engine->AddStrategy(DeathKnight::CombatStrategyForSpec(specIndex));
                return;
            }
            if (classId == CLASS_WARRIOR)
            {
                // Spec rotation (arms/fury/protection) on the shared warrior base.
                engine->AddStrategy("warrior");
                engine->AddStrategy(Warrior::CombatStrategyForSpec(specIndex));
                return;
            }
            if (classId == CLASS_PALADIN)
            {
                // Spec rotation (holy/protection/retribution) on the paladin base.
                engine->AddStrategy("paladin");
                engine->AddStrategy(Paladin::CombatStrategyForSpec(specIndex));
                return;
            }
            if (classId == CLASS_HUNTER)
            {
                // Spec rotation (beast mastery/marksmanship/survival) + hunter base.
                engine->AddStrategy("hunter");
                engine->AddStrategy(Hunter::CombatStrategyForSpec(specIndex));
                return;
            }
            if (classId == CLASS_ROGUE)
            {
                // Spec rotation (assassination/outlaw/subtlety) + rogue base.
                engine->AddStrategy("rogue");
                engine->AddStrategy(Rogue::CombatStrategyForSpec(specIndex));
                return;
            }
            if (classId == CLASS_PRIEST)
            {
                // Spec rotation (discipline/holy/shadow) + priest base.
                engine->AddStrategy("priest");
                engine->AddStrategy(Priest::CombatStrategyForSpec(specIndex));
                return;
            }
            if (classId == CLASS_SHAMAN)
            {
                // Spec rotation (elemental/enhancement/restoration) + shaman base.
                engine->AddStrategy("shaman");
                engine->AddStrategy(Shaman::CombatStrategyForSpec(specIndex));
                return;
            }
            if (classId == CLASS_MAGE)
            {
                // Spec rotation (arcane/fire/frost) + mage base.
                engine->AddStrategy("mage");
                engine->AddStrategy(Mage::CombatStrategyForSpec(specIndex));
                return;
            }

            // --- generic fallback for not-yet-ported classes -----------------
            switch (role)
            {
                case SpecRole::Tank:
                    engine->AddStrategy("tank");
                    break;
                case SpecRole::Healer:
                    engine->AddStrategy("heal");
                    // Healers still auto-attack when nobody needs healing.
                    engine->AddStrategy(SpecRoles::IsMeleeClass(classId) ? "melee" : "ranged");
                    break;
                case SpecRole::Dps:
                default:
                    engine->AddStrategy(SpecRoles::IsMeleeClass(classId) ? "melee" : "ranged");
                    break;
            }
        }

        void InitDeadEngine(PsychobotAI* /*ai*/, Engine* /*engine*/)
        {
            // Release/resurrect handled in a later step; dead engine is idle now.
        }
    }
}
