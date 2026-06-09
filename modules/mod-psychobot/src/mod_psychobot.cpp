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
#include "PsychobotPopulationMgr.h"
#include "PsychobotAhBot.h"
#include "dungeon/PsychobotDungeonMgr.h"

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
        // Stage 3/4: (re)load population/scaling + ahbot config too.
        sPsychobotPopulation->LoadConfig();
        sPsychobotAhBot->LoadConfig();
        // S25: seed the dungeon/raid encounter-script registry (idempotent).
        psychobot::DungeonMgr::InitEncounters();
        TC_LOG_INFO("module.psychobot", "mod-psychobot config %s (Psychobot.Enable = %u, RandomBots = %u).",
            reload ? "reloaded" : "loaded", enabled ? 1 : 0,
            sPsychobotPopulation->Config().enable ? 1 : 0);
    }

    void OnUpdate(uint32 diff) override
    {
        if (!sConfigMgr->GetBoolDefault("Psychobot.Enable", false))
            return;
        sPsychobotMgr->UpdateAI(diff);
        sPsychobotAhBot->Update(diff);
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

    // S28: when a socketless bot finishes loading into the world, attach its AI.
    void OnLogin(Player* player, bool /*firstLogin*/) override
    {
        sPsychobotMgr->OnPlayerLogin(player);
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
            { "spec",   rbac::RBAC_PERM_COMMAND_GPS, false, &HandleSpecCommand,   "" },
            { "group",  rbac::RBAC_PERM_COMMAND_GPS, false, &HandleGroupCommand,  "" },
            { "follow", rbac::RBAC_PERM_COMMAND_GPS, false, &HandleFollowCommand,   "" },
            { "stay",   rbac::RBAC_PERM_COMMAND_GPS, false, &HandleStayCommand,     "" },
            { "attack", rbac::RBAC_PERM_COMMAND_GPS, false, &HandleAttackCommand,   "" },
            { "cast",   rbac::RBAC_PERM_COMMAND_GPS, false, &HandleCastCommand,     "" },
            { "strategy", rbac::RBAC_PERM_COMMAND_GPS, false, &HandleStrategyCommand, "" },
            { "help",   rbac::RBAC_PERM_COMMAND_GPS, false, &HandleHelpCommand,     "" },
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

    static bool HandleSpecCommand(ChatHandler* handler, char const* args)
    {
        Player* master = handler->GetSession() ? handler->GetSession()->GetPlayer() : nullptr;
        std::string a = args ? args : "";
        handler->SendSysMessage(sPsychobotMgr->SetSpec(master, a).c_str());
        return true;
    }

    static bool HandleGroupCommand(ChatHandler* handler, char const* args)
    {
        Player* master = handler->GetSession() ? handler->GetSession()->GetPlayer() : nullptr;
        std::string name = args ? args : "";
        handler->SendSysMessage(sPsychobotMgr->GroupBot(master, name).c_str());
        return true;
    }

    // --- S27 order/grammar commands ---------------------------------------
    static bool HandleFollowCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* m = handler->GetSession() ? handler->GetSession()->GetPlayer() : nullptr;
        handler->SendSysMessage(sPsychobotMgr->OrderFollow(m).c_str());
        return true;
    }

    static bool HandleStayCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* m = handler->GetSession() ? handler->GetSession()->GetPlayer() : nullptr;
        handler->SendSysMessage(sPsychobotMgr->OrderStay(m).c_str());
        return true;
    }

    static bool HandleAttackCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* m = handler->GetSession() ? handler->GetSession()->GetPlayer() : nullptr;
        handler->SendSysMessage(sPsychobotMgr->OrderAttack(m).c_str());
        return true;
    }

    static bool HandleCastCommand(ChatHandler* handler, char const* args)
    {
        Player* m = handler->GetSession() ? handler->GetSession()->GetPlayer() : nullptr;
        std::string spell = args ? args : "";
        handler->SendSysMessage(sPsychobotMgr->OrderCast(m, spell).c_str());
        return true;
    }

    static bool HandleStrategyCommand(ChatHandler* handler, char const* args)
    {
        Player* m = handler->GetSession() ? handler->GetSession()->GetPlayer() : nullptr;
        std::string a = args ? args : "";
        handler->SendSysMessage(sPsychobotMgr->ToggleStrategy(m, a).c_str());
        return true;
    }

    static bool HandleHelpCommand(ChatHandler* handler, char const* /*args*/)
    {
        handler->SendSysMessage("Psychobot commands:");
        handler->SendSysMessage("  .psychobot add|remove|list|group <name>   - manage bots");
        handler->SendSysMessage("  .psychobot spec <name> <0-3>              - set a bot's spec");
        handler->SendSysMessage("  .psychobot follow|stay|attack            - order all your bots");
        handler->SendSysMessage("  .psychobot cast <spell name>             - all bots cast a spell");
        handler->SendSysMessage("  .psychobot strategy <name>               - toggle a combat strategy");
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
