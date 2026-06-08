/*
 * ===========================================================================
 *  Copyright (c) 2026 Psychostout. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "CoreValues.h"
#include "../engine/AiObjectContext.h"
#include "../engine/ServerFacade.h"
#include "../PsychobotAIFwd.h"
#include "../PsychobotGroupMgr.h"
#include "Player.h"
#include "Unit.h"
#include <cstdlib>
#include <cctype>

namespace psychobot
{
    // Resolve a value/trigger qualifier to a spell id. Accepts either a numeric
    // spell id (e.g. "12345") OR a spell NAME (e.g. "frost fever"), resolving
    // names through the bot so class wiring stays DB2-agnostic.
    static uint32 ResolveQualifierSpell(PsychobotAI* ai, std::string const& qualifier)
    {
        if (qualifier.empty())
            return 0;
        bool numeric = true;
        for (unsigned char c : qualifier)
            if (!std::isdigit(c)) { numeric = false; break; }
        if (numeric)
            return static_cast<uint32>(std::atoi(qualifier.c_str()));
        return PsychobotAIBridge::GetSpellId(ai, qualifier);
    }

    // --- target values ----------------------------------------------------
    Unit* CurrentTargetValue::Calculate()
    {
        return PsychobotAIBridge::GetCurrentTarget(GetAI());
    }

    Unit* SelfTargetValue::Calculate()
    {
        return GetBot();   // Player* -> Unit* implicit upcast
    }

    Unit* MasterTargetValue::Calculate()
    {
        Player* master = GetMaster();
        return master ? master->GetSelectedUnit() : nullptr;
    }

    // --- resource values --------------------------------------------------
    float HealthValue::Calculate()
    {
        return ServerFacade::GetHealthPct(GetBot());
    }

    float PowerPctValue::Calculate()
    {
        // qualifier holds the Powers enum value as a string (e.g. "3" = energy)
        uint32 powerType = static_cast<uint32>(std::atoi(GetQualifier().c_str()));
        return ServerFacade::GetPowerPct(GetBot(), powerType);
    }

    // --- state values -----------------------------------------------------
    float DistanceToTargetValue::Calculate()
    {
        Unit* target = PsychobotAIBridge::GetCurrentTarget(GetAI());
        if (!target)
            return 99999.0f;
        return ServerFacade::GetDistance2d(GetBot(), target);
    }

    bool InCombatValue::Calculate()
    {
        return ServerFacade::IsInCombat(GetBot());
    }

    bool HasAuraValue::Calculate()
    {
        uint32 spellId = ResolveQualifierSpell(GetAI(), GetQualifier());
        return ServerFacade::HasAura(GetBot(), spellId);
    }

    bool TargetHasAuraValue::Calculate()
    {
        uint32 spellId = ResolveQualifierSpell(GetAI(), GetQualifier());
        Unit* target = PsychobotAIBridge::GetCurrentTarget(GetAI());
        return ServerFacade::HasAura(target, spellId);
    }

    bool SpellReadyValue::Calculate()
    {
        uint32 spellId = ResolveQualifierSpell(GetAI(), GetQualifier());
        return ServerFacade::IsSpellReady(GetBot(), spellId);
    }

    // --- group values -----------------------------------------------------
    Unit* PartyMemberToHealValue::Calculate()
    {
        Player* bot = GetBot();
        if (!bot)
            return nullptr;
        return GroupMgr::GetGroupHealTarget(bot, 80.0f);
    }

    // ----------------------------------------------------------------------
    // Registration - one cached context holding every core value creator.
    // ----------------------------------------------------------------------
    namespace
    {
        class CoreValueContext : public NamedObjectContext<UntypedValue>
        {
        public:
            CoreValueContext() : NamedObjectContext<UntypedValue>(/*shared*/ false)
            {
                _creators["current target"]      = [](PsychobotAI* ai) -> UntypedValue* { return new CurrentTargetValue(ai); };
                _creators["self target"]         = [](PsychobotAI* ai) -> UntypedValue* { return new SelfTargetValue(ai); };
                _creators["master target"]       = [](PsychobotAI* ai) -> UntypedValue* { return new MasterTargetValue(ai); };
                _creators["health"]              = [](PsychobotAI* ai) -> UntypedValue* { return new HealthValue(ai); };
                _creators["power"]               = [](PsychobotAI* ai) -> UntypedValue* { return new PowerPctValue(ai); };
                _creators["distance to target"]  = [](PsychobotAI* ai) -> UntypedValue* { return new DistanceToTargetValue(ai); };
                _creators["in combat"]           = [](PsychobotAI* ai) -> UntypedValue* { return new InCombatValue(ai); };
                _creators["has aura"]            = [](PsychobotAI* ai) -> UntypedValue* { return new HasAuraValue(ai); };
                _creators["target has aura"]     = [](PsychobotAI* ai) -> UntypedValue* { return new TargetHasAuraValue(ai); };
                _creators["spell ready"]         = [](PsychobotAI* ai) -> UntypedValue* { return new SpellReadyValue(ai); };
                _creators["party member to heal"]= [](PsychobotAI* ai) -> UntypedValue* { return new PartyMemberToHealValue(ai); };
            }
        };
    }

    void RegisterCoreValues(AiObjectContext* context)
    {
        if (context)
            context->AddValueContext(new CoreValueContext());
    }
}
