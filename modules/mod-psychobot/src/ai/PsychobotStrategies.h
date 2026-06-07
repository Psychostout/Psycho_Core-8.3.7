/*
 * ===========================================================================
 *  Copyright (c) 2026 Psychostout. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Stage 1 generic strategies: follow-master (non-combat) + basic attack
 *  (combat). Class-specific rotations arrive in Stage 2.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_STRATEGIES_H
#define PSYCHOBOT_STRATEGIES_H

namespace psychobot
{
    class PsychobotAI;
    class Engine;

    // Installs the non-combat strategy set (follow master, stay close).
    void BuildNonCombatEngine(PsychobotAI* ai, Engine* engine);

    // Installs the combat strategy set (acquire target + basic melee/attack).
    void BuildCombatEngine(PsychobotAI* ai, Engine* engine);
}

#endif // PSYCHOBOT_STRATEGIES_H
