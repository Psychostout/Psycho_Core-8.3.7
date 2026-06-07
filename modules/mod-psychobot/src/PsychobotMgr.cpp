/*
 * ===========================================================================
 *  Copyright (c) 2026 Psychostout. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "PsychobotMgr.h"
#include "ai/PsychobotAI.h"
#include "Player.h"
#include "ObjectAccessor.h"
#include "Config.h"
#include "Log.h"

namespace psychobot
{
    PsychobotMgr* PsychobotMgr::instance()
    {
        static PsychobotMgr instance;
        return &instance;
    }

    bool PsychobotMgr::IsBot(ObjectGuid guid) const
    {
        return _bots.find(guid) != _bots.end();
    }

    bool PsychobotMgr::AttachAI(Player* bot, ObjectGuid masterGuid)
    {
        if (!bot)
            return false;
        if (IsBot(bot->GetGUID()))
            return false;
        _bots[bot->GetGUID()] = std::make_unique<PsychobotAI>(bot, masterGuid);
        return true;
    }

    void PsychobotMgr::DetachAI(ObjectGuid botGuid)
    {
        _bots.erase(botGuid);
    }

    std::string PsychobotMgr::AddBot(Player* master, std::string const& charName)
    {
        if (!sConfigMgr->GetBoolDefault("Psychobot.Enable", false))
            return "Psychobot is disabled (set Psychobot.Enable = 1).";

        if (!master)
            return "No master.";

        if (charName.empty())
            return "Usage: .psychobot add <charactername>";

        // Stage 1 (V1 = alts-as-bots): the target character must already be
        // connected/in-world. Socketless auto-login arrives in a later stage.
        Player* bot = ObjectAccessor::FindConnectedPlayerByName(charName);
        if (!bot)
            return "Character '" + charName + "' is not online. (Stage 1 manages "
                   "already-logged-in characters; auto-login is a later stage.)";

        if (bot->GetGUID() == master->GetGUID())
            return "You cannot add yourself as a bot.";

        if (IsBot(bot->GetGUID()))
            return "'" + charName + "' is already a Psychobot.";

        if (AttachAI(bot, master->GetGUID()))
            return "Added Psychobot: " + bot->GetName() + ". It will follow you.";

        return "Failed to add Psychobot.";
    }

    std::string PsychobotMgr::RemoveBot(Player* master, std::string const& charName)
    {
        if (!master)
            return "No master.";

        if (charName.empty())
            return "Usage: .psychobot remove <charactername>";

        Player* bot = ObjectAccessor::FindConnectedPlayerByName(charName);
        if (bot && IsBot(bot->GetGUID()))
        {
            DetachAI(bot->GetGUID());
            return "Removed Psychobot: " + bot->GetName() + ".";
        }
        return "'" + charName + "' is not an active Psychobot.";
    }

    std::string PsychobotMgr::ListBots(Player* master)
    {
        if (_bots.empty())
            return "No active Psychobots.";

        std::string out = "Active Psychobots (" + std::to_string(_bots.size()) + "):";
        for (auto const& pair : _bots)
        {
            if (PsychobotAI* ai = pair.second.get())
                if (Player* bot = ai->GetBot())
                    out += "\n  - " + bot->GetName();
        }
        return out;
    }

    void PsychobotMgr::OnPlayerLogout(Player* player)
    {
        if (!player)
            return;

        // If a bot logs out, drop its AI.
        DetachAI(player->GetGUID());

        // If a master logs out, drop the bots it owns.
        ObjectGuid masterGuid = player->GetGUID();
        for (auto it = _bots.begin(); it != _bots.end(); )
        {
            if (it->second && it->second->GetMasterGuid() == masterGuid)
                it = _bots.erase(it);
            else
                ++it;
        }
    }

    void PsychobotMgr::UpdateAI(uint32 diff)
    {
        if (_bots.empty())
            return;

        for (auto it = _bots.begin(); it != _bots.end(); )
        {
            PsychobotAI* ai = it->second.get();
            // Drop AI whose bot left the world (logged out / deleted).
            if (!ai || !ai->GetBot() || !ai->GetBot()->IsInWorld())
            {
                it = _bots.erase(it);
                continue;
            }
            ai->UpdateAI(diff);
            ++it;
        }
    }
}
