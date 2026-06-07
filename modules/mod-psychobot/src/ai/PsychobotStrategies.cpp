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

    // Engage the current target with melee + autoattack.
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

            // Select + start attacking (autoattack); move into melee.
            bot->SetSelection(target->GetGUID());

            if (bot->GetVictim() != target)
                bot->Attack(target, true);

            // Close to melee range if needed.
            if (bot->GetDistance(target) > 5.0f)
                bot->GetMotionMaster()->MoveChase(target);

            return true;
        }
    };

    // ======================================================================
    // STRATEGIES
    // ======================================================================

    // Non-combat: keep up with the master.
    class FollowStrategy : public Strategy
    {
    public:
        FollowStrategy(PsychobotAI* ai) : Strategy(ai, "follow") { }
        void InitTriggers(std::vector<TriggerNode>& out) override
        {
            // If far from master -> follow (high relevance). Always allow a
            // low-relevance follow so the bot trails even when close.
            out.push_back({ "far from master", "follow master", 10.0f });
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
        engine->RegisterAction(new FollowMasterAction(ai));
        engine->RegisterAction(new AttackTargetAction(ai));
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
