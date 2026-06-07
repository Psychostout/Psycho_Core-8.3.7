/*
 * ===========================================================================
 *  Copyright (c) 2026 Psychostout. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Rotation tables for all 12 BfA 8.3 classes. Spell NAMES are resolved to the
 *  bot's known spells at runtime, so unknown/renamed spells are simply skipped
 *  (no crash, no hardcoded IDs). DK = full vertical slice; others seeded with
 *  signature abilities, to be tuned once the 8.3.7 client DB2 is available.
 *  Spec order follows ChrSpecialization OrderIndex (0,1,2[,3]).
 * ===========================================================================
 */

#include "PsychobotClassAI.h"
#include "SharedDefines.h"
#include <vector>

namespace psychobot
{
    namespace ClassRegistry
    {
        // m() = melee entry, r() = ranged/instant entry
        static RotationEntry m(std::string s) { return RotationEntry{ std::move(s), true  }; }
        static RotationEntry r(std::string s) { return RotationEntry{ std::move(s), false }; }

        static std::vector<SpecRotation> const& Table()
        {
            static std::vector<SpecRotation> const table =
            {
                // ---------------- WARRIOR (1) ----------------
                { CLASS_WARRIOR, 0, SpecRole::Dps,  "Arms",
                  { m("Mortal Strike"), m("Overpower"), m("Execute"), m("Slam"), m("Whirlwind") } },
                { CLASS_WARRIOR, 1, SpecRole::Dps,  "Fury",
                  { m("Rampage"), m("Raging Blow"), m("Bloodthirst"), m("Execute"), m("Whirlwind") } },
                { CLASS_WARRIOR, 2, SpecRole::Tank, "Protection",
                  { m("Shield Slam"), m("Thunder Clap"), m("Revenge"), m("Devastate"), m("Ignore Pain") } },

                // ---------------- PALADIN (2) ----------------
                { CLASS_PALADIN, 0, SpecRole::Healer, "Holy",
                  { r("Holy Light"), r("Flash of Light"), r("Holy Shock"), r("Word of Glory") } },
                { CLASS_PALADIN, 1, SpecRole::Tank,   "Protection",
                  { m("Avenger's Shield"), m("Judgment"), m("Hammer of the Righteous"), m("Consecration"), m("Shield of the Righteous") } },
                { CLASS_PALADIN, 2, SpecRole::Dps,    "Retribution",
                  { m("Templar's Verdict"), m("Blade of Justice"), m("Judgment"), m("Crusader Strike"), m("Wake of Ashes") } },

                // ---------------- HUNTER (3) ----------------
                { CLASS_HUNTER, 0, SpecRole::Dps, "Beast Mastery",
                  { r("Barbed Shot"), r("Kill Command"), r("Cobra Shot"), r("Multi-Shot") } },
                { CLASS_HUNTER, 1, SpecRole::Dps, "Marksmanship",
                  { r("Aimed Shot"), r("Rapid Fire"), r("Arcane Shot"), r("Steady Shot") } },
                { CLASS_HUNTER, 2, SpecRole::Dps, "Survival",
                  { m("Raptor Strike"), m("Wildfire Bomb"), m("Kill Command"), m("Carve") } },

                // ---------------- ROGUE (4) ----------------
                { CLASS_ROGUE, 0, SpecRole::Dps, "Assassination",
                  { m("Mutilate"), m("Garrote"), m("Rupture"), m("Envenom"), m("Fan of Knives") } },
                { CLASS_ROGUE, 1, SpecRole::Dps, "Outlaw",
                  { m("Sinister Strike"), m("Between the Eyes"), m("Dispatch"), m("Roll the Bones"), m("Blade Flurry") } },
                { CLASS_ROGUE, 2, SpecRole::Dps, "Subtlety",
                  { m("Backstab"), m("Shadowstrike"), m("Eviscerate"), m("Nightblade"), m("Shuriken Storm") } },

                // ---------------- PRIEST (5) ----------------
                { CLASS_PRIEST, 0, SpecRole::Healer, "Discipline",
                  { r("Power Word: Shield"), r("Penance"), r("Flash Heal"), r("Smite") } },
                { CLASS_PRIEST, 1, SpecRole::Healer, "Holy",
                  { r("Heal"), r("Flash Heal"), r("Prayer of Healing"), r("Renew") } },
                { CLASS_PRIEST, 2, SpecRole::Dps,    "Shadow",
                  { r("Vampiric Touch"), r("Shadow Word: Pain"), r("Mind Blast"), r("Mind Flay"), r("Shadow Word: Death") } },

                // ---------------- DEATH KNIGHT (6) - FULL VERTICAL SLICE ----------------
                { CLASS_DEATH_KNIGHT, 0, SpecRole::Tank, "Blood",
                  { m("Death Strike"), m("Marrowrend"), m("Heart Strike"), m("Blood Boil"),
                    m("Death and Decay"), m("Dancing Rune Weapon") } },
                { CLASS_DEATH_KNIGHT, 1, SpecRole::Dps,  "Frost",
                  { m("Obliterate"), m("Frost Strike"), m("Howling Blast"), m("Remorseless Winter"),
                    m("Pillar of Frost"), m("Frostscythe") } },
                { CLASS_DEATH_KNIGHT, 2, SpecRole::Dps,  "Unholy",
                  { m("Festering Strike"), m("Death Coil"), m("Scourge Strike"), m("Outbreak"),
                    m("Apocalypse"), m("Dark Transformation"), m("Death and Decay") } },

                // ---------------- SHAMAN (7) ----------------
                { CLASS_SHAMAN, 0, SpecRole::Dps,    "Elemental",
                  { r("Lava Burst"), r("Earth Shock"), r("Lightning Bolt"), r("Flame Shock"), r("Chain Lightning") } },
                { CLASS_SHAMAN, 1, SpecRole::Dps,    "Enhancement",
                  { m("Stormstrike"), m("Lava Lash"), m("Crash Lightning"), m("Lightning Bolt"), m("Flametongue") } },
                { CLASS_SHAMAN, 2, SpecRole::Healer, "Restoration",
                  { r("Healing Wave"), r("Healing Surge"), r("Riptide"), r("Chain Heal") } },

                // ---------------- MAGE (8) ----------------
                { CLASS_MAGE, 0, SpecRole::Dps, "Arcane",
                  { r("Arcane Blast"), r("Arcane Missiles"), r("Arcane Barrage"), r("Arcane Explosion") } },
                { CLASS_MAGE, 1, SpecRole::Dps, "Fire",
                  { r("Fireball"), r("Pyroblast"), r("Fire Blast"), r("Scorch"), r("Flamestrike") } },
                { CLASS_MAGE, 2, SpecRole::Dps, "Frost",
                  { r("Frostbolt"), r("Ice Lance"), r("Flurry"), r("Frozen Orb"), r("Blizzard") } },

                // ---------------- WARLOCK (9) ----------------
                { CLASS_WARLOCK, 0, SpecRole::Dps, "Affliction",
                  { r("Agony"), r("Corruption"), r("Unstable Affliction"), r("Malefic Rapture"), r("Drain Soul") } },
                { CLASS_WARLOCK, 1, SpecRole::Dps, "Demonology",
                  { r("Call Dreadstalkers"), r("Hand of Gul'dan"), r("Demonbolt"), r("Shadow Bolt"), r("Implosion") } },
                { CLASS_WARLOCK, 2, SpecRole::Dps, "Destruction",
                  { r("Immolate"), r("Conflagrate"), r("Incinerate"), r("Chaos Bolt"), r("Rain of Fire") } },

                // ---------------- MONK (10) - NET-NEW ----------------
                { CLASS_MONK, 0, SpecRole::Tank,   "Brewmaster",
                  { m("Keg Smash"), m("Breath of Fire"), m("Tiger Palm"), m("Blackout Strike"), m("Purifying Brew") } },
                { CLASS_MONK, 1, SpecRole::Healer, "Mistweaver",
                  { r("Vivify"), r("Renewing Mist"), r("Enveloping Mist"), r("Essence Font") } },
                { CLASS_MONK, 2, SpecRole::Dps,    "Windwalker",
                  { m("Rising Sun Kick"), m("Fists of Fury"), m("Blackout Kick"), m("Tiger Palm"), m("Spinning Crane Kick") } },

                // ---------------- DRUID (11) - 4 specs ----------------
                { CLASS_DRUID, 0, SpecRole::Dps,    "Balance",
                  { r("Moonfire"), r("Sunfire"), r("Starsurge"), r("Wrath"), r("Starfire") } },
                { CLASS_DRUID, 1, SpecRole::Dps,    "Feral",
                  { m("Rake"), m("Shred"), m("Rip"), m("Ferocious Bite"), m("Thrash") } },
                { CLASS_DRUID, 2, SpecRole::Tank,   "Guardian",
                  { m("Mangle"), m("Thrash"), m("Swipe"), m("Maul"), m("Ironfur") } },
                { CLASS_DRUID, 3, SpecRole::Healer, "Restoration",
                  { r("Rejuvenation"), r("Regrowth"), r("Wild Growth"), r("Lifebloom"), r("Swiftmend") } },

                // ---------------- DEMON HUNTER (12) - NET-NEW, 2 specs ----------------
                { CLASS_DEMON_HUNTER, 0, SpecRole::Dps,  "Havoc",
                  { m("Eye Beam"), m("Blade Dance"), m("Chaos Strike"), m("Immolation Aura"),
                    m("Demon's Bite"), m("Fel Rush") } },
                { CLASS_DEMON_HUNTER, 1, SpecRole::Tank, "Vengeance",
                  { m("Soul Cleave"), m("Immolation Aura"), m("Sigil of Flame"), m("Fracture"),
                    m("Spirit Bomb"), m("Demon Spikes") } },
            };
            return table;
        }

        SpecRotation const* GetRotation(uint8 classId, uint8 specIndex)
        {
            for (SpecRotation const& s : Table())
                if (s.classId == classId && s.specIndex == specIndex)
                    return &s;
            return nullptr;
        }

        uint8 GetDefaultSpecIndex(uint8 /*classId*/)
        {
            return 0;
        }
    }
}
