/*
 * ===========================================================================
 *  Copyright (c) 2026 Psychostout. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  mod-psychobot - Stage 1 scripts: world tick, logout cleanup, and the
 *  ".psychobot" command. Bot AI lives in PsychobotMgr / PsychobotAI.
 * ===========================================================================
 */

#include "ScriptMgr.h"
#include "Chat.h"
#include "Player.h"
#include "WorldSession.h"
#include "Config.h"
#include "Log.h"
#include "RBAC.h"
#include "PsychobotMgr.h"

using namespace psychobot;

// ---------------------------------------------------------------------------
// WorldScript - drives the bot manager tick and reports config state.
// ---------------------------------------------------------------------------
class psychobot_WorldScript : public WorldScript
{
public:
    psychobot_WorldScript() : WorldScript("psychobot_WorldScript") { }

    void OnConfigLoad(bool reload) override
    {
        bool enabled = sConfigMgr->GetBoolDefault("Psychobot.Enable", false);
        TC_LOG_INFO("module.psychobot", "mod-psychobot config %s (Psychobot.Enable = %u).",
            reload ? "reloaded" : "loaded", enabled ? 1 : 0);
    }

    void OnUpdate(uint32 diff) override
    {
        if (!sConfigMgr->GetBoolDefault("Psychobot.Enable", false))
            return;
        sPsychobotMgr->UpdateAI(diff);
    }
};

// ---------------------------------------------------------------------------
// PlayerScript - cleanup when a master or bot logs out.
// ---------------------------------------------------------------------------
class psychobot_PlayerScript : public PlayerScript
{
public:
    psychobot_PlayerScript() : PlayerScript("psychobot_PlayerScript") { }

    void OnLogout(Player* player) override
    {
        sPsychobotMgr->OnPlayerLogout(player);
    }
};

// ---------------------------------------------------------------------------
// CommandScript - ".psychobot add|remove|list"
// ---------------------------------------------------------------------------
class psychobot_CommandScript : public CommandScript
{
public:
    psychobot_CommandScript() : CommandScript("psychobot_CommandScript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> psychobotCommandTable =
        {
            { "add",    rbac::RBAC_PERM_COMMAND_GPS, false, &HandleAddCommand,    "" },
            { "remove", rbac::RBAC_PERM_COMMAND_GPS, false, &HandleRemoveCommand, "" },
            { "list",   rbac::RBAC_PERM_COMMAND_GPS, false, &HandleListCommand,   "" },
        };
        static std::vector<ChatCommand> commandTable =
        {
            { "psychobot", rbac::RBAC_PERM_COMMAND_GPS, false, nullptr, "", psychobotCommandTable },
        };
        return commandTable;
    }

    static bool HandleAddCommand(ChatHandler* handler, char const* args)
    {
        Player* master = handler->GetSession() ? handler->GetSession()->GetPlayer() : nullptr;
        std::string name = args ? args : "";
        handler->SendSysMessage(sPsychobotMgr->AddBot(master, name).c_str());
        return true;
    }

    static bool HandleRemoveCommand(ChatHandler* handler, char const* args)
    {
        Player* master = handler->GetSession() ? handler->GetSession()->GetPlayer() : nullptr;
        std::string name = args ? args : "";
        handler->SendSysMessage(sPsychobotMgr->RemoveBot(master, name).c_str());
        return true;
    }

    static bool HandleListCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* master = handler->GetSession() ? handler->GetSession()->GetPlayer() : nullptr;
        handler->SendSysMessage(sPsychobotMgr->ListBots(master).c_str());
        return true;
    }
};

// ---------------------------------------------------------------------------
// Registrator called by the module loader.
// ---------------------------------------------------------------------------
void AddSC_mod_psychobot()
{
    new psychobot_WorldScript();
    new psychobot_PlayerScript();
    new psychobot_CommandScript();
}
