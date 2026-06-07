/*
 * ===========================================================================
 *  Copyright (c) 2026 Psychostout. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "PsychobotEngine.h"
#include <algorithm>

namespace psychobot
{
    Engine::~Engine() = default;

    void Engine::AddStrategy(Strategy* strategy)
    {
        if (!strategy)
            return;
        if (HasStrategy(strategy->GetName()))
        {
            delete strategy;
            return;
        }
        _strategies.emplace_back(strategy);
        _dirty = true;
    }

    void Engine::RemoveStrategy(std::string const& name)
    {
        auto it = std::remove_if(_strategies.begin(), _strategies.end(),
            [&](std::unique_ptr<Strategy> const& s) { return s->GetName() == name; });
        if (it != _strategies.end())
        {
            _strategies.erase(it, _strategies.end());
            _dirty = true;
        }
    }

    bool Engine::HasStrategy(std::string const& name) const
    {
        for (auto const& s : _strategies)
            if (s->GetName() == name)
                return true;
        return false;
    }

    void Engine::RegisterAction(Action* action)
    {
        if (action)
            _actions[action->GetName()].reset(action);
    }

    void Engine::RegisterTrigger(Trigger* trigger)
    {
        if (trigger)
            _triggers[trigger->GetName()].reset(trigger);
    }

    Action* Engine::FindAction(std::string const& name) const
    {
        auto it = _actions.find(name);
        return it != _actions.end() ? it->second.get() : nullptr;
    }

    Trigger* Engine::FindTrigger(std::string const& name) const
    {
        auto it = _triggers.find(name);
        return it != _triggers.end() ? it->second.get() : nullptr;
    }

    void Engine::RebuildBindings()
    {
        _bindings.clear();
        for (auto const& s : _strategies)
            s->InitTriggers(_bindings);
        // Highest relevance first.
        std::stable_sort(_bindings.begin(), _bindings.end(),
            [](TriggerNode const& a, TriggerNode const& b) { return a.relevance > b.relevance; });
        _dirty = false;
    }

    std::string Engine::DoNextAction(Event const& event)
    {
        if (_dirty)
            RebuildBindings();

        for (TriggerNode const& node : _bindings)
        {
            Trigger* trigger = FindTrigger(node.trigger);
            // An empty trigger name means "always active" (passive bindings).
            if (!node.trigger.empty())
            {
                if (!trigger || !trigger->IsActive())
                    continue;
            }

            Action* action = FindAction(node.action);
            if (!action)
                continue;
            if (!action->IsUseful() || !action->IsPossible())
                continue;

            if (action->Execute(event))
                return action->GetName();
        }
        return std::string();
    }
}
