/*
 * ===========================================================================
 *  Copyright (c) 2026 Psychostout. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Psychobot AI engine - a clean-room re-implementation of the ike3-style
 *  Strategy/Action/Value/Trigger decision framework, authored for Psycho_Core
 *  (TrinityCore BfA 8.3). Architecture inspired by cmangos/playerbots (GPL-2.0);
 *  this is original code written against TrinityCore APIs.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_ENGINE_H
#define PSYCHOBOT_ENGINE_H

#include "Define.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

class Player;
class Unit;

namespace psychobot
{
    class PsychobotAI;

    // ----------------------------------------------------------------------
    // Event - carries optional context into an action/trigger evaluation.
    // ----------------------------------------------------------------------
    struct Event
    {
        Event() : source(), param() { }
        explicit Event(std::string src, std::string p = "") : source(std::move(src)), param(std::move(p)) { }
        std::string source;
        std::string param;
    };

    // ----------------------------------------------------------------------
    // Action - a single unit of bot behaviour. Subclasses override Execute().
    // ----------------------------------------------------------------------
    class Action
    {
    public:
        Action(PsychobotAI* ai, std::string name) : _ai(ai), _name(std::move(name)) { }
        virtual ~Action() { }

        // Returns true if the action did something this tick.
        virtual bool Execute(Event const& event) = 0;
        // Cheap feasibility gate (resources/range/known).
        virtual bool IsPossible() { return true; }
        // Should this action even be considered right now?
        virtual bool IsUseful() { return true; }

        std::string const& GetName() const { return _name; }

    protected:
        PsychobotAI* _ai;
        std::string  _name;
    };

    // ----------------------------------------------------------------------
    // Trigger - a condition that, when active, requests an action by name
    // with a relevance weight.
    // ----------------------------------------------------------------------
    class Trigger
    {
    public:
        Trigger(PsychobotAI* ai, std::string name) : _ai(ai), _name(std::move(name)) { }
        virtual ~Trigger() { }

        virtual bool IsActive() = 0;
        std::string const& GetName() const { return _name; }

    protected:
        PsychobotAI* _ai;
        std::string  _name;
    };

    // A trigger->action binding inside a strategy.
    struct TriggerNode
    {
        std::string trigger;
        std::string action;
        float       relevance;
    };

    // ----------------------------------------------------------------------
    // Strategy - a named bundle of trigger->action bindings (dps/tank/follow..)
    // ----------------------------------------------------------------------
    class Strategy
    {
    public:
        Strategy(PsychobotAI* ai, std::string name) : _ai(ai), _name(std::move(name)) { }
        virtual ~Strategy() { }

        // Fill the bindings this strategy contributes.
        virtual void InitTriggers(std::vector<TriggerNode>& out) = 0;
        std::string const& GetName() const { return _name; }

    protected:
        PsychobotAI* _ai;
        std::string  _name;
    };

    // ----------------------------------------------------------------------
    // Engine - per-bot brain. Holds active strategies; each tick it evaluates
    // triggers and executes the highest-relevance possible+useful action.
    // ----------------------------------------------------------------------
    class Engine
    {
    public:
        explicit Engine(PsychobotAI* ai) : _ai(ai) { }
        ~Engine();

        void AddStrategy(Strategy* strategy);          // takes ownership
        void RemoveStrategy(std::string const& name);
        bool HasStrategy(std::string const& name) const;

        void RegisterAction(Action* action);           // takes ownership
        void RegisterTrigger(Trigger* trigger);        // takes ownership

        // Run one decision tick. Returns name of executed action (or empty).
        std::string DoNextAction(Event const& event = Event());

    private:
        Action*  FindAction(std::string const& name) const;
        Trigger* FindTrigger(std::string const& name) const;
        void     RebuildBindings();

        PsychobotAI* _ai;
        std::vector<std::unique_ptr<Strategy>> _strategies;
        std::map<std::string, std::unique_ptr<Action>>  _actions;
        std::map<std::string, std::unique_ptr<Trigger>> _triggers;
        std::vector<TriggerNode> _bindings;   // flattened from strategies
        bool _dirty = true;
    };
}

#endif // PSYCHOBOT_ENGINE_H
