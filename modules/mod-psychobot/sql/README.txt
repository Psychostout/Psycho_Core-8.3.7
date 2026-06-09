================================================================================
 mod-psychobot - SQL
================================================================================

WHAT TO APPLY
-------------
characters/psychobot_strategies.sql   -> apply to the CHARACTERS database.
    Creates `psychobot_strategies` (guid, name). Used by PsychobotDbStore (S27)
    to persist each bot's master-toggled extra combat strategies across relogs.
    Idempotent (CREATE TABLE IF NOT EXISTS) - safe to re-run.

HOW TO APPLY (manually, until auto-SQL wiring is enabled on your build)
----------------------------------------------------------------------
    mysql -u <user> -p <characters_db> < characters/psychobot_strategies.sql

WHAT IS *NOT* NEEDED (and why) - honest notes
---------------------------------------------
The reference cmangos/ike3 playerbots ships several SQL tables. Our clean-room
re-implementation does NOT need most of them because the equivalent feature is
either consolidated, deterministic, or in-memory:

  ai_playerbot_db_store        -> REPLACED by characters/psychobot_strategies.sql
                                  (simpler 2-column schema for our DbStore).
  ai_playerbot_random_bots     -> NOT NEEDED. PsychobotPopulationMgr selects from
                                  EXISTING characters and tracks the active roster
                                  in memory; it does not generate/track random bots
                                  in the DB.
  ahbot_history/category/price -> NOT NEEDED. PsychobotAhBot is config-driven and
                                  deterministic (no price-history persistence).
  ai_playerbot_names           -> NOT NEEDED. We do not auto-create bot characters
                                  (bots are existing characters logged in socketlessly).
  ai_playerbot_texts / rpg     -> NOT NEEDED. No RPG/broadcast text system (deferred).
  ai_playerbot_cache           -> NOT NEEDED. Caches are in-memory per run.

If/when a future stage adds: random-bot generation, ahbot price history, or a
bot-name pool, add the matching table here and wire its prepared statements.
================================================================================
