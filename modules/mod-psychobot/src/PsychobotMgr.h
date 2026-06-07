/*
 * ===========================================================================
 *  Copyright (c) 2026 Psychostout. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  PsychobotMgr - global singleton that owns every active bot AI and ticks
 *  them each world update. Stage 1: bots are existing characters managed in
 *  memory; full socketless login arrives with later stages.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_MGR_H
#define PSYCHOBOT_MGR_H

#include "Define.h"
#include "ObjectGuid.h"
#include <unordered_map>
#include <memory>
#include <string>

class Player;

namespace psychobot
{
    class PsychobotAI;

    class PsychobotMgr
    {
    public:
        static PsychobotMgr* instance();

        // Master-facing controls (driven by the .psychobot command).
        // Returns a human-readable status string.
        std::string AddBot(Player* master, std::string const& charName);
        std::string RemoveBot(Player* master, std::string const& charName);
        std::string ListBots(Player* master);

        // Called when a connected player whom we manage as a bot is detected,
        // or when a master logs out (cleanup).
        void OnPlayerLogout(Player* player);

        // World tick entry point (from the module WorldScript::OnUpdate).
        void UpdateAI(uint32 diff);

        bool IsBot(ObjectGuid guid) const;
        uint32 GetBotCount() const { return static_cast<uint32>(_bots.size()); }

    private:
        PsychobotMgr() = default;

        // Attach AI to an already in-world player (Stage 1 path).
        bool AttachAI(Player* bot, ObjectGuid masterGuid);
        void DetachAI(ObjectGuid botGuid);

        std::unordered_map<ObjectGuid, std::unique_ptr<PsychobotAI>> _bots;
    };
}

#define sPsychobotMgr psychobot::PsychobotMgr::instance()

#endif // PSYCHOBOT_MGR_H
