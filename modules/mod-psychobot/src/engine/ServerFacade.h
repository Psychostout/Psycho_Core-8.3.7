/*
 * ===========================================================================
 *  Copyright (c) 2026 Psychostout. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  ServerFacade - the single seam between the engine/strategy/action layer and
 *  the TrinityCore 8.3 core API. Every core call the AI makes (combat, casting,
 *  movement, resources, auras, distance) goes through here, so a future core
 *  rev only needs edits in one place. The reference engine #ifdef's MaNGOS vs
 *  cMaNGOS here; our build always targets TrinityCore BfA 8.3.
 *
 *  Static, stateless helpers (no per-bot state) - actions/values pass the Unit*.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_ENGINE_SERVERFACADE_H
#define PSYCHOBOT_ENGINE_SERVERFACADE_H

#include "Define.h"
#include <string>

class Unit;
class Player;

namespace psychobot
{
    namespace ServerFacade
    {
        // --- liveness / combat ----------------------------------------------
        bool   IsAlive(Unit* unit);
        bool   IsInCombat(Unit* unit);
        bool   IsFriendly(Unit* a, Unit* b);
        bool   IsHostile(Unit* a, Unit* b);
        bool   IsValidAttackTarget(Unit* attacker, Unit* target);
        Unit*  GetVictim(Unit* unit);

        // --- distance / range / facing --------------------------------------
        float  GetDistance2d(Unit* from, Unit* to);
        bool   IsInRange(Unit* from, Unit* to, float range);
        bool   IsInMeleeRange(Unit* from, Unit* to);
        bool   IsInFront(Unit* from, Unit* to);

        // --- health / resources (percent 0..100) ----------------------------
        float  GetHealthPct(Unit* unit);
        float  GetPowerPct(Unit* unit, uint32 powerType);   // Powers cast to uint32
        uint32 GetPower(Unit* unit, uint32 powerType);

        // --- auras ----------------------------------------------------------
        bool   HasAura(Unit* unit, uint32 spellId);
        bool   IsCasting(Unit* unit);

        // --- spells ---------------------------------------------------------
        bool   IsSpellReady(Player* bot, uint32 spellId);   // known + not on CD
        bool   KnowsSpell(Player* bot, uint32 spellId);

        // --- movement -------------------------------------------------------
        bool   IsMoving(Unit* unit);

        // --- pet (hunter / warlock / DK ghoul) ------------------------------
        // The bot's living pet as a Unit*, or nullptr if it has none / it's dead.
        Unit*  GetPet(Player* bot);
        bool   HasLivingPet(Player* bot);
        float  GetPetHealthPct(Player* bot);   // 0 if no living pet
    }
}

#endif // PSYCHOBOT_ENGINE_SERVERFACADE_H
