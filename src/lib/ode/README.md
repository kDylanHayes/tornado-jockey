# ODE 0.5

Sources from the ODE 0.5 release (tag `0.5.0` of https://github.com/odedevs/ode):
`ode/src`, `include/ode` and the bundled OPCODE collision library. ODE is
dual-licensed under the LGPL and a BSD-style license (`LICENSE.TXT`,
`LICENSE-BSD.TXT`); OPCODE's terms are in `OPCODE/ReadMe.txt`.

Tornado.exe links ODE 0.5 built in single precision with trimesh collision
enabled. Wanako compiled it with the engine's flags (`/O2 /GX`) plus
`/DdTRIMESH_ENABLED`; later ODE revisions match fewer functions.

Added files, not part of the release:

- `include/ode/config.h`: what ODE's `configurator` writes for MSVC with
  `dSINGLE`.
- `stub/windows.h`: the few declarations `ode/src/error.cpp` needs
  (`MessageBoxA` and two `MB_` flags), since the VC Toolkit has no Platform SDK.

Files not linked into the game (`stepfast`, `mat`, `timer`, `testing`,
`scrapbook`, `export-dif` and most of `OPCODE/Ice`) are kept for reference
but have no unit.
