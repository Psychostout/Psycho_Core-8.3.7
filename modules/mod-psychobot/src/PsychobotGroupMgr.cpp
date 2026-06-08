/*
 * ===========================================================================
 *  Copyright (c) 2026 Psychostout. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "PsychobotGroupMgr.h"
#include "ai/PsychobotSpecRoles.h"
#include "Player.h"
#include "Unit.h"
#include "Group.h"
#include "GroupMgr.h"
#include "ObjectAccessor.h"
#include "DB2Stores.h"
#include "DB2Structure.h"
#include "DungeonFinding/LFG.h"
#include "Log.h"

namespace psychobot
{
    namespace GroupMgr
    {
        bool InviteToGroup(Player* master, Player* bot)
        {
            if (!master || !bot || master == bot)
                return false;

            Group* group = master->GetGroup();
            if (!group)
            {
                // Create a new group led by the master.
                group = new Group();
                if (!group->Create(master))
                {
                    delete group;
                    return false;
                }
                sGroupMgr->AddGroup(group);
            }

            if (group->IsFull())
                return false;
            if (bot->GetGroup())
                return false; // already grouped elsewhere

            if (!group->AddMember(bot))
                return false;

            AssignRole(bot);
            return true;
        }

        void LeaveGroup(Player* bot)
        {
            if (!bot)
                return;
            if (Group* group = bot->GetGroup())
                group->RemoveMember(bot->GetGUID());
        }

        SpecRole GetBotRole(Player* bot)
        {
            return SpecRoles::GetBotRole(bot);
        }

        void AssignRole(Player* bot)
        {
            if (!bot)
                return;
            Group* group = bot->GetGroup();
            if (!group)
                return;

            uint8 roleFlags = lfg::PLAYER_ROLE_DAMAGE;
            switch (GetBotRole(bot))
            {
                case SpecRole::Tank:   roleFlags = lfg::PLAYER_ROLE_TANK;   break;
                case SpecRole::Healer: roleFlags = lfg::PLAYER_ROLE_HEALER; break;
                default:               roleFlags = lfg::PLAYER_ROLE_DAMAGE; break;
            }
            group->SetLfgRoles(bot->GetGUID(), roleFlags);
        }

        Unit* GetGroupAssistTarget(Player* bot)
        {
            if (!bot)
                return nullptr;

            Group* group = bot->GetGroup();
            if (!group)
                return nullptr;

            // Prefer what the group's tank is fighting; else the leader's target.
            Unit* tankTarget = nullptr;
            Unit* leaderTarget = nullptr;
            ObjectGuid leaderGuid = group->GetLeaderGUID();

            for (Group::MemberSlot const& slot : group->GetMemberSlots())
            {
                Player* member = ObjectAccessor::GetPlayer(*bot, slot.guid);
                if (!member || member == bot || !member->IsInWorld() || !member->IsInMap(bot))
                    continue;

                if (Unit* victim = member->GetVictim())
                {
                    if (member->GetGUID() == leaderGuid && !leaderTarget)
                        leaderTarget = victim;
                    if (GetBotRole(member) == SpecRole::Tank && !tankTarget)
                        tankTarget = victim;
                }
            }
            return tankTarget ? tankTarget : leaderTarget;
        }

        Unit* GetGroupHealTarget(Player* bot, float belowPct)
        {
            if (!bot)
                return nullptr;
            Group* group = bot->GetGroup();
            if (!group)
            {
                // Solo healer: heal self if hurt.
                return (bot->GetHealthPct() < belowPct) ? bot : nullptr;
            }

            Unit* worst = nullptr;
            float worstPct = belowPct;

            // Consider self.
            if (bot->GetHealthPct() < worstPct)
            {
                worst = bot;
                worstPct = bot->GetHealthPct();
            }

            for (Group::MemberSlot const& slot : group->GetMemberSlots())
            {
                Player* member = ObjectAccessor::GetPlayer(*bot, slot.guid);
                if (!member || !member->IsInWorld() || !member->IsAlive() || !member->IsInMap(bot))
                    continue;
                float pct = member->GetHealthPct();
                if (pct < worstPct)
                {
                    worst = member;
                    worstPct = pct;
                }
            }
            return worst;
        }

        bool InBattleground(Player* bot)
        {
            return bot && bot->InBattleground();
        }

        bool InDungeon(Player* bot)
        {
            if (!bot)
                return false;
            if (Group* group = bot->GetGroup())
                return group->isLFGGroup();
            return false;
        }
    }
}
