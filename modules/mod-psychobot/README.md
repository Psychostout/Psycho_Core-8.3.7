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
| `.psychobot add <charname>` | Take control of a character as your bot. If it's **offline**, it is logged in **socketlessly** (S28) and starts following once loaded. |
| `.psychobot remove <charname>` | Release a bot (socketless bots are logged back out). |
| `.psychobot list` | List your active bots. |
| `.psychobot spec <charname> <0-3>` | Set the bot's specialization + talents. |
| `.psychobot group <charname>` | Add the bot to your party (LFG role auto-assigned from spec). |
| `.psychobot follow` / `stay` / `attack` | Order all your bots (follow you / hold position / attack your target). |
| `.psychobot cast <spell name>` | All your bots cast a spell (by name) on your target. |
| `.psychobot strategy <name>` | Toggle an extra combat strategy on a bot (persisted in the characters DB). |
| `.psychobot help` | Print the full command grammar. |

## SQL
Apply once to the **characters** database (see `sql/README.txt`):
```
mysql -u <user> -p <characters_db> < sql/characters/psychobot_strategies.sql
```
This creates `psychobot_strategies` (used by `PsychobotDbStore` to persist each
bot's master-toggled combat strategies across relogs). No other tables are needed.

## Layout
```
mod-psychobot/
├── conf/mod_psychobot.conf.dist
├── mod-psychobot.cmake
├── sql/
│   ├── README.txt
│   └── characters/psychobot_strategies.sql
└── src/
    ├── mod_psychobot.cpp          WorldScript + PlayerScript + CommandScript
    ├── mod_psychobot_loader.cpp   Addmod_psychobotScripts()
    ├── PsychobotMgr.{h,cpp}       global bot registry, ticked each world update
    ├── PsychobotLoginMgr.{h,cpp}  socketless bot login/logout (S28)
    ├── PsychobotPopulationMgr.{h,cpp}  population/scaling (active-alone, smart-scale)
    ├── PsychobotGroupMgr.{h,cpp}  party invite + role + group assist/heal/dispel target
    ├── PsychobotAhBot.{h,cpp}     auction-house seller (config-driven)
    ├── PsychobotAIFwd.h           engine<->AI bridge declarations
    ├── engine/                    Strategy/Action/Trigger/Value/Engine/Queue/Context
    ├── ai/                        PsychobotAI, AiFactory, Gear/Talent/SpecRoles
    ├── actions/                   GenericSpell + Movement + World actions
    ├── triggers/  values/  strategies/   shared trigger/value/generic-strategy kits
    ├── classes/<class>/           12 classes x (Triggers + Strategies + Context)
    ├── pets/                      pet actions + pet strategy
    ├── pvp/  dungeon/  travel/  world/   Phase-D systems
    └── ...
```

## Honest limitations (framework hooks, not yet live)
The following are present as documented hooks (real entry points, gated until a
live server / client DB2 pass): vendor-sell, loot pickup, quest accept/turn-in,
gathering, AH `AddAuction` (live posting), per-BG objectives, per-encounter
boss reactions, chat hyperlink (spell/item LINK) parsing. There is **no** flee/
retreat behaviour yet. First real compile + in-world test happen on the desktop.
