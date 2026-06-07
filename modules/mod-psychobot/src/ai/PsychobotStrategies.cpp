/*
 * ===========================================================================
 *  Copyright (c) 2026 Psychostout. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "PsychobotStrategies.h"
#include "PsychobotAI.h"
#include "Player.h"
#include "Unit.h"
#include "MotionMaster.h"

namespace psychobot
{
    // ======================================================================
    // TRIGGERS
    // ======================================================================

    // Active when the bot has a valid hostile target available.
    class HasTargetTrigger : public Trigger
    {
    public:
        HasTargetTrigger(PsychobotAI* ai) : Trigger(ai, "has target") { }
        bool IsActive() override
        {
            Unit* target = _ai->GetCurrentTarget();
            Player* bot = _ai->GetBot();
            if (!target || !bot)
                return false;
            return _ai->IsAlive(target) && bot->IsValidAttackTarget(target);
        }
    };

    // Active when the bot is too far from its master (needs to catch up).
    class FarFromMasterTrigger : public Trigger
    {
    public:
        FarFromMasterTrigger(PsychobotAI* ai) : Trigger(ai, "far from master") { }
        bool IsActive() override
        {
            Player* master = _ai->GetMaster();
            Player* bot = _ai->GetBot();
            if (!master || !bot)
                return false;
            if (master->GetMap() != bot->GetMap())
                return false;
            return bot->GetDistance(master) > 5.0f;
        }
    };

    // Active when the bot has no master (an open-world / population bot).
    class NoMasterTrigger : public Trigger
    {
    public:
        NoMasterTrigger(PsychobotAI* ai) : Trigger(ai, "no master") { }
        bool IsActive() override { return _ai->GetMaster() == nullptr; }
    };

    // ======================================================================
    // ACTIONS
    // ======================================================================

    // Move to / follow the master.
    class FollowMasterAction : public Action
    {
    public:
        FollowMasterAction(PsychobotAI* ai) : Action(ai, "follow master") { }
        bool IsUseful() override { return _ai->GetMaster() != nullptr; }
        bool Execute(Event const& /*event*/) override
        {
            _ai->FollowMaster();
            return true;
        }
    };

    // Engage the current target: run the class rotation + keep autoattack going.
    class AttackTargetAction : public Action
    {
    public:
        AttackTargetAction(PsychobotAI* ai) : Action(ai, "attack target") { }

        bool IsPossible() override { return _ai->GetCurrentTarget() != nullptr; }

        bool Execute(Event const& /*event*/) override
        {
            Player* bot = _ai->GetBot();
            Unit* target = _ai->GetCurrentTarget();
            if (!bot || !target)
                return false;
            if (!_ai->IsAlive(target) || !bot->IsValidAttackTarget(target))
                return false;

            // Select + start autoattacking.
            bot->SetSelection(target->GetGUID());
            if (bot->GetVictim() != target)
                bot->Attack(target, true);

            // Positioning: melee specs chase; the rotation handles ranged casts.
            if (bot->GetDistance(target) > 5.0f)
                bot->GetMotionMaster()->MoveChase(target);

            // Stage 2: run the data-driven class rotation (casts a spell if one
            // is known + usable; otherwise autoattack carries the fight).
            _ai->DoClassRotation(target);
            return true;
        }
    };

    // World-behaviour: a masterless (population) bot seeks a nearby hostile to
    // fight, which drives leveling. Master-owned bots use follow instead.
    class GrindAction : public Action
    {
    public:
        GrindAction(PsychobotAI* ai) : Action(ai, "grind") { }
        bool IsUseful() override { return _ai->GetMaster() == nullptr; }
        bool Execute(Event const& /*event*/) override
        {
            Player* bot = _ai->GetBot();
            if (!bot)
                return false;

            // Find a nearby attackable hostile within a generous radius.
            Unit* enemy = bot->SelectNearbyTarget(nullptr, 40.0f);
            if (!enemy || !_ai->IsAlive(enemy) || !bot->IsValidAttackTarget(enemy))
                return false;

            bot->SetSelection(enemy->GetGUID());
            bot->Attack(enemy, true);
            bot->GetMotionMaster()->MoveChase(enemy);
            return true;
        }
    };

    // World-behaviour: rest when low and safe (placeholder - eat/drink wiring
    // arrives with item data; for now it simply stops to regen via standing).
    class RestAction : public Action
    {
    public:
        RestAction(PsychobotAI* ai) : Action(ai, "rest") { }
        bool IsUseful() override
        {
            Player* bot = _ai->GetBot();
            return bot && !bot->IsInCombat() && bot->GetHealthPct() < 50.0f;
        }
        bool Execute(Event const& /*event*/) override
        {
            // Stop and let natural out-of-combat regen tick. (Food/drink later.)
            _ai->StopMoving();
            return true;
        }
    };

    // ======================================================================
    // STRATEGIES
    // ======================================================================

    // Non-combat: follow the master if owned; rest if hurt; otherwise (no
    // master) grind nearby hostiles to level up.
    class FollowStrategy : public Strategy
    {
    public:
        FollowStrategy(PsychobotAI* ai) : Strategy(ai, "follow") { }
        void InitTriggers(std::vector<TriggerNode>& out) override
        {
            out.push_back({ "far from master", "follow master", 10.0f });
            out.push_back({ "no master",       "rest",           6.0f });
            out.push_back({ "no master",       "grind",          5.0f });
            // Low-relevance trailing follow when owned + close.
            out.push_back({ "",                "follow master",  1.0f });
        }
    };

    // Combat: attack whatever target is current.
    class BasicCombatStrategy : public Strategy
    {
    public:
        BasicCombatStrategy(PsychobotAI* ai) : Strategy(ai, "combat") { }
        void InitTriggers(std::vector<TriggerNode>& out) override
        {
            out.push_back({ "has target", "attack target", 20.0f });
            // Fall back to following the master if there's nothing to fight.
            out.push_back({ "",           "follow master",  1.0f });
        }
    };

    // ======================================================================
    // ENGINE BUILDERS (register actions/triggers + add strategies)
    // ======================================================================

    static void RegisterCommon(PsychobotAI* ai, Engine* engine)
    {
        engine->RegisterTrigger(new HasTargetTrigger(ai));
        engine->RegisterTrigger(new FarFromMasterTrigger(ai));
        engine->RegisterTrigger(new NoMasterTrigger(ai));
        engine->RegisterAction(new FollowMasterAction(ai));
        engine->RegisterAction(new AttackTargetAction(ai));
        engine->RegisterAction(new GrindAction(ai));
        engine->RegisterAction(new RestAction(ai));
    }

    void BuildNonCombatEngine(PsychobotAI* ai, Engine* engine)
    {
        RegisterCommon(ai, engine);
        engine->AddStrategy(new FollowStrategy(ai));
    }

    void BuildCombatEngine(PsychobotAI* ai, Engine* engine)
    {
        RegisterCommon(ai, engine);
        engine->AddStrategy(new BasicCombatStrategy(ai));
    }
}
