/*
 * ===========================================================================
 *  Copyright (c) 2026 Psychostout. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

/*
 * Psycho_Core 8.3.7 - Skeleton Module
 * -----------------------------------
 * A minimal, working example module for the Psycho_Core (TrinityCore BfA 8.3.x)
 * module system. Copy this folder, rename it (mod-<yourname>), and build.
 *
 * License: GPL-2.0-or-later (inherited from TrinityCore - see COPYING).
 */

#include "ScriptMgr.h"
#include "Player.h"
#include "Chat.h"
#include "Config.h"
#include "Log.h"

// ---------------------------------------------------------------------------
// Config helpers
// ---------------------------------------------------------------------------
// Module config keys live in modules/mod-skeleton/conf/mod_skeleton.conf.dist
// and are read via sConfigMgr (the same manager used by worldserver.conf).
namespace
{
    bool SkeletonEnabled()
    {
        return sConfigMgr->GetBoolDefault("Skeleton.Enable", false);
    }

    std::string SkeletonWelcome()
    {
        return sConfigMgr->GetStringDefault("Skeleton.WelcomeMessage",
            "Welcome to Psycho_Core! (mod-skeleton example)");
    }
}

// ---------------------------------------------------------------------------
// PlayerScript - hooks into player lifecycle events.
// See src/server/game/Scripting/ScriptMgr.h (class PlayerScript) for the full
// list of overridable hooks (OnLogin, OnLogout, OnLevelChanged, OnGiveXP, ...).
// ---------------------------------------------------------------------------
class mod_skeleton_PlayerScript : public PlayerScript
{
public:
    // The string passed to the base ctor is the unique script name; it must be
    // unique across the whole core. Prefix with the module name to be safe.
    mod_skeleton_PlayerScript() : PlayerScript("mod_skeleton_PlayerScript") { }

    void OnLogin(Player* player, bool /*firstLogin*/) override
    {
        if (!SkeletonEnabled())
            return;

        if (player)
            ChatHandler(player->GetSession()).SendSysMessage(SkeletonWelcome().c_str());
    }

    void OnLevelChanged(Player* player, uint8 oldLevel) override
    {
        if (!SkeletonEnabled() || !player)
            return;

        // NOTE: this 8.3 tree uses getLevel() (lowercase 'g'), not GetLevel().
        TC_LOG_INFO("module.skeleton", "Player %s leveled up from %u to %u.",
            player->GetName().c_str(), oldLevel, player->getLevel());
    }
};

// ---------------------------------------------------------------------------
// WorldScript - hooks into world/config lifecycle (e.g. config (re)load).
// ---------------------------------------------------------------------------
class mod_skeleton_WorldScript : public WorldScript
{
public:
    mod_skeleton_WorldScript() : WorldScript("mod_skeleton_WorldScript") { }

    // Called on startup and on ".reload config".
    void OnConfigLoad(bool reload) override
    {
        TC_LOG_INFO("module.skeleton", "mod-skeleton config %s (Skeleton.Enable = %u).",
            reload ? "reloaded" : "loaded", SkeletonEnabled() ? 1 : 0);
    }
};

// ---------------------------------------------------------------------------
// Registrator. Every script source provides a void AddSC_<name>() that creates
// its script objects. The module loader (below) calls these.
// ---------------------------------------------------------------------------
void AddSC_mod_skeleton()
{
    new mod_skeleton_PlayerScript();
    new mod_skeleton_WorldScript();
}
