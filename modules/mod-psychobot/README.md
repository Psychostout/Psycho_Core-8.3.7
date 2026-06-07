<!--
===========================================================================
 Copyright (c) 2026 Psychostout. All rights reserved.
 Original work authored 100% from scratch for Psycho_Core.
 Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 NOT covered by the base GPL framework license. Development/evaluation only.

 NOTE: the AI architecture (Strategy/Action/Value/Trigger engine) is a
 clean-room re-implementation inspired by ike3/cmangos playerbots (GPL-2.0).
===========================================================================
-->

# mod-psychobot

Player-driven combat bots for **Psycho_Core 8.3.7** (TrinityCore BfA).
Built on the module system; AI uses a clean-room ike3-style
Strategy/Action/Value/Trigger engine.

## Status: Stages 1–4 implemented
- ✅ **Stage 1 — Foundation + Main AI:** module + AI engine (Engine/Action/Trigger/
  Strategy), `PsychobotMgr` ticked from `WorldScript::OnUpdate`, `PsychobotAI`
  per-bot brain with a TC-8.3 ServerFacade seam, follow + basic combat.
- ✅ **Stage 2 — Classes + BfA talents:** generic `PsychobotTalentMgr` (spec +
  7×3 talent grid), data-driven name-resolved spell-priority rotations for all
  12 classes / 36 specs (DK full slice; Monk + Demon Hunter included).
- ✅ **Stage 3 — World behaviour + scaling:** `PsychobotPopulationMgr`
  (BotActiveAlone / SmartScale / DisabledWithoutRealPlayer), `PsychobotGearMgr`,
  grind/rest world behaviour.
- ✅ **Stage 4 — Group content + extras:** `PsychobotGroupMgr` (party invite +
  LFG role assignment from spec, group assist target, healer heal-target),
  group-aware combat, BG/dungeon awareness, `PsychobotAhBot` scaffold.
- ⏳ Pending: **socketless auto-login (Phase A)** for offline alts + open-world
  population spawning; exact spell/gear/AH item data (needs 8.3.7 client DB2);
  per-spec rotation tuning; BG/dungeon tactic maps.

> **V1 scope = "alts as bots":** Stage 1 manages a character that is **already
> online**. Auto-login of offline alts (socketless `WorldSession`) is a later stage.

## Build & enable
1. Build the core with modules (default linkage = static):
   `cmake -S . -B build -DMODULES=static && cmake --build build -j`
2. Copy `configs/modules/mod_psychobot.conf.dist` → `mod_psychobot.conf` and set
   `Psychobot.Enable = 1`.
3. Start worldserver.

## Commands
| Command | Description |
|---|---|
| `.psychobot add <charname>` | Take control of an online character as your bot (it follows + fights). |
| `.psychobot remove <charname>` | Release a bot. |
| `.psychobot list` | List your active bots. |
| `.psychobot spec <charname> <0-3>` | Set the bot's specialization + talents. |
| `.psychobot group <charname>` | Add the bot to your party (role auto-assigned). |

## Layout
```
mod-psychobot/
├── conf/mod_psychobot.conf.dist
├── mod-psychobot.cmake
├── src/
│   ├── mod_psychobot.cpp          WorldScript + PlayerScript + CommandScript
│   ├── mod_psychobot_loader.cpp   Addmod_psychobotScripts()
│   ├── PsychobotMgr.{h,cpp}       global bot registry, ticked each world update
│   └── ai/
│       ├── PsychobotEngine.{h,cpp}     Strategy/Action/Trigger engine
│       ├── PsychobotAI.{h,cpp}         per-bot brain + TC-8.3 ServerFacade seam
│       └── PsychobotStrategies.{h,cpp} Stage-1 follow + basic combat
└── README.md
```
