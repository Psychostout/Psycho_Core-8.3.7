/*
 * ===========================================================================
 *  Copyright (c) 2026 Psychostout. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "PsychobotAhBot.h"
#include "Config.h"
#include "Log.h"

namespace psychobot
{
    PsychobotAhBot* PsychobotAhBot::instance()
    {
        static PsychobotAhBot instance;
        return &instance;
    }

    void PsychobotAhBot::LoadConfig()
    {
        _enable          = sConfigMgr->GetBoolDefault("Psychobot.AhBot.Enable", false);
        _intervalSeconds = sConfigMgr->GetIntDefault ("Psychobot.AhBot.IntervalSeconds", 3600);
    }

    void PsychobotAhBot::Update(uint32 diff)
    {
        if (!_enable)
            return;

        _accum += diff;
        if (_accum < _intervalSeconds * 1000)
            return;
        _accum = 0;

        // TODO: post/buy auctions. Requires the item DB2 (to choose sellable
        // items + price baselines) and an AH-owner faction account. Until that
        // data is present this is intentionally a safe no-op so the build and
        // runtime are stable. Marked for completion alongside client data.
    }
}
