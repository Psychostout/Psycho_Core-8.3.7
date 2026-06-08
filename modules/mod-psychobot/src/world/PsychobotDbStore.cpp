/*
 * ===========================================================================
 *  Copyright (c) 2026 Psychostout. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "PsychobotDbStore.h"

namespace psychobot
{
    PsychobotDbStore* PsychobotDbStore::instance()
    {
        static PsychobotDbStore instance;
        return &instance;
    }

    std::set<std::string> const& PsychobotDbStore::GetStrategies(ObjectGuid bot) const
    {
        auto it = _saved.find(bot);
        return it != _saved.end() ? it->second : _empty;
    }

    bool PsychobotDbStore::HasStrategy(ObjectGuid bot, std::string const& name) const
    {
        auto it = _saved.find(bot);
        return it != _saved.end() && it->second.count(name) != 0;
    }

    void PsychobotDbStore::SetStrategy(ObjectGuid bot, std::string const& name, bool on)
    {
        if (on)
            _saved[bot].insert(name);
        else
        {
            auto it = _saved.find(bot);
            if (it != _saved.end())
                it->second.erase(name);
        }
        Save(bot);   // persist (hook)
    }

    bool PsychobotDbStore::ToggleStrategy(ObjectGuid bot, std::string const& name)
    {
        bool nowOn = !HasStrategy(bot, name);
        SetStrategy(bot, name, nowOn);
        return nowOn;
    }

    void PsychobotDbStore::Clear(ObjectGuid bot)
    {
        _saved.erase(bot);
    }

    // --- persistence hooks (on-server pass) -------------------------------
    void PsychobotDbStore::Save(ObjectGuid /*bot*/)
    {
        // On-server: DELETE then INSERT the bot's saved strategy rows into a
        // characters-DB table (e.g. psychobot_strategies(guid, name)) via a
        // CharacterDatabase prepared statement. The in-memory set above already
        // holds them for the running session, so this is a documented no-op
        // until the table + statements are added on a live server.
    }

    void PsychobotDbStore::Load(ObjectGuid /*bot*/)
    {
        // On-server: SELECT the bot's saved strategy rows and populate _saved
        // (called on bot login). No-op until the DB table exists.
    }
}
