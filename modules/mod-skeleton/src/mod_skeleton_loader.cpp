/*
 * ===========================================================================
 *  Copyright (c) 2026 Psychostout. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

/*
 * Psycho_Core 8.3.7 - Skeleton Module loader
 * ------------------------------------------
 * The CMake-generated ModulesLoader.cpp calls Add<ModuleName>Scripts() for each
 * module. Hyphens in the directory name are converted to underscores, so the
 * folder "mod-skeleton" expects the function "Addmod_skeletonScripts".
 *
 * Inside it, call every AddSC_* registrator your module defines.
 *
 * License: GPL-2.0-or-later.
 */

// Forward declarations of this module's per-file registrators.
void AddSC_mod_skeleton();

// Entry point invoked by the module loader (name derived from the folder name).
void Addmod_skeletonScripts()
{
    AddSC_mod_skeleton();
}
