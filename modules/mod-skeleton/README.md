# mod-skeleton

A minimal, working **example module** for the Psycho_Core 8.3.7 module system.
Use it as the starting template for your own modules.

## What it does
- Reads config keys via `sConfigMgr` (`conf/mod_skeleton.conf.dist`).
- `PlayerScript`: sends a welcome system message on login (when enabled) and
  logs level-ups.
- `WorldScript`: logs when its config is (re)loaded.

## Folder layout
```
mod-skeleton/
├── README.md                 This file
├── mod-skeleton.cmake        OPTIONAL extra CMake (deps/includes) - reference only
├── conf/
│   └── mod_skeleton.conf.dist   Config (copied to configs/modules on build)
├── sql/
│   └── world/
│       └── mod_skeleton_example.sql   Example world SQL (no-op)
└── src/
    ├── mod_skeleton.cpp          The scripts + AddSC_mod_skeleton()
    └── mod_skeleton_loader.cpp   Addmod_skeletonScripts() -> calls AddSC_*
```

## Enable it
1. Build the core (the module is picked up automatically; default linkage =
   `static`). See `modules/README.md` and `docs/HOW_TO_BUILD_A_MODULE.md`.
2. Copy `conf/mod_skeleton.conf.dist` → your server's `configs/modules/mod_skeleton.conf`
   (the build also copies it there automatically).
3. Set `Skeleton.Enable = 1`.
4. (Optional) apply `sql/world/mod_skeleton_example.sql` — see the header of that
   file for the two supported methods.
5. Start worldserver, log in, and you should see the welcome message.

## Rename it for your own module
- Copy the whole folder to `modules/mod-<yourname>/`.
- Rename `src/mod_skeleton_loader.cpp`'s function to `Addmod_<yourname>Scripts()`
  (folder name with `-` → `_`).
- Rename `AddSC_mod_skeleton` and the script names accordingly.
- Rename `conf/mod_<yourname>.conf.dist` and `mod-<yourname>.cmake`.
- Reconfigure CMake and rebuild.
