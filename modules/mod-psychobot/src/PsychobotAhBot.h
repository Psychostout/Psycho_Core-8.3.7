/*
 * ===========================================================================
 *  Copyright (c) 2026 Psychostout. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  PsychobotAhBot - auction-house bot scaffold (Stage 4). Periodically posts/
 *  buys auctions to keep the AH populated. Config-gated and a safe no-op until
 *  the item DB2 + AH data are available to choose real items/prices.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_AHBOT_H
#define PSYCHOBOT_AHBOT_H

#include "Define.h"

namespace psychobot
{
    class PsychobotAhBot
    {
    public:
        static PsychobotAhBot* instance();

        void LoadConfig();
        void Update(uint32 diff);   // ticked from the world update

    private:
        PsychobotAhBot() { LoadConfig(); }

        bool   _enable = false;
        uint32 _intervalSeconds = 3600;   // post cycle period
        uint32 _accum = 0;                // ms accumulator
    };
}

#define sPsychobotAhBot psychobot::PsychobotAhBot::instance()

#endif // PSYCHOBOT_AHBOT_H
