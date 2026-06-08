/*
 * ===========================================================================
 *  Copyright (c) 2026 Psychostout. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  PsychobotGroupMgr - grouping + role helpers (Stage 4). Lets a master pull a
 *  bot into the party, assigns the bot's LFG role from its spec, and provides
 *  group-context queries the strategies use (assist target, group leader,
 *  is-in-battleground/dungeon).
 * ===========================================================================
 */

#ifndef PSYCHOBOT_GROUP_MGR_H
#define PSYCHOBOT_GROUP_MGR_H

#include "Define.h"

class Player;
class Unit;
class Group;

namespace psychobot
{
    enum class SpecRole : uint8;  // defined in ai/PsychobotSpecRoles.h

    namespace GroupMgr
    {
        // Invite + accept the bot into the master's group (creating one if
        // needed). Returns a status string.
        bool InviteToGroup(Player* master, Player* bot);

        // Remove the bot from its current group.
        void LeaveGroup(Player* bot);

        // Set the bot's LFG role flags from its spec role (tank/heal/dps).
        void AssignRole(Player* bot);

        // Role of a bot derived from its current spec (via SpecRoles).
        SpecRole GetBotRole(Player* bot);

        // The unit a DPS/tank bot should be hitting: the group's main-assist /
        // tank target if available, else the bot's master's target.
        Unit* GetGroupAssistTarget(Player* bot);

        // The most-injured friendly group member a healer bot should heal, or
        // nullptr if none need healing past the threshold.
        Unit* GetGroupHealTarget(Player* bot, float belowPct = 80.0f);

        bool InBattleground(Player* bot);
        bool InDungeon(Player* bot);
    }
}

#endif // PSYCHOBOT_GROUP_MGR_H
