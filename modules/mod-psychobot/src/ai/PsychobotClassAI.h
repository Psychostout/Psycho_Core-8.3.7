/*
 * ===========================================================================
 *  Copyright (c) 2026 Psychostout. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  PsychobotClassAI - data-driven, name-resolved spell-priority rotation.
 *  Each spec provides an ordered list of spell NAMES; at runtime the names are
 *  resolved to the spells the bot actually knows (via the ServerFacade), so the
 *  rotation self-adapts to the client's DB2 (no hardcoded WotLK spell IDs).
 *
 *  DK is the fully-populated vertical slice; all other classes/specs are
 *  registered with their signature rotation names (refined once 8.3.7 client
 *  DB2 is loaded).
 * ===========================================================================
 */

#ifndef PSYCHOBOT_CLASS_AI_H
#define PSYCHOBOT_CLASS_AI_H

#include "Define.h"
#include <string>
#include <vector>

class Player;
class Unit;

namespace psychobot
{
    class PsychobotAI;

    enum class SpecRole : uint8
    {
        Dps = 0,
        Tank,
        Healer
    };

    // A single rotation entry: cast <spell> when its simple condition holds.
    struct RotationEntry
    {
        std::string spell;     // spell name (resolved to a known spell at runtime)
        bool        melee;     // true = requires melee range; false = ranged/instant
        // (Future: resource/aura conditions. Stage 2 keeps it priority + range.)
    };

    // A class' rotation table for a given spec (by ChrSpecialization OrderIndex).
    struct SpecRotation
    {
        uint8                       classId;     // CLASS_*
        uint8                       specIndex;   // ChrSpecialization OrderIndex (0..n)
        SpecRole                    role;
        std::string                 name;        // e.g. "Frost", "Havoc"
        std::vector<RotationEntry>  priority;    // highest priority first
    };

    namespace ClassRegistry
    {
        // Returns the rotation for a class+specIndex, or nullptr if none.
        SpecRotation const* GetRotation(uint8 classId, uint8 specIndex);

        // Returns the best-guess specIndex (0) if a precise one isn't known.
        uint8 GetDefaultSpecIndex(uint8 classId);
    }

    // The combat brain a bot uses: runs the spec rotation each combat tick.
    class ClassAI
    {
    public:
        explicit ClassAI(PsychobotAI* ai) : _ai(ai) { }

        // Run one combat decision; returns the name of the cast spell or "".
        std::string DoRotation(Unit* target);

    private:
        PsychobotAI* _ai;
    };
}

#endif // PSYCHOBOT_CLASS_AI_H
