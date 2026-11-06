# Working a unit in an agent worktree

This page is for an automated session that owns one unit (or a small, related
set) in its own git worktree. The goal is always the same: make objdiff show
the unit's code and data at 100%, commit it, and hand the branch back. It is
not enough for the code to "look right"; the built object must match the
delinked target byte for byte.

Read [getting-started.md](getting-started.md), [delinking.md](delinking.md)
and the matching notes in [toolchain.md](toolchain.md#matching-notes) first.

## Set up the worktree

The worktree is a fresh checkout: `orig/`, `build/` and `tools/vc71/` are
git-ignored and do not exist in it. Copy the inputs from the main checkout
(`$MAIN` below is the absolute path of the main checkout, usually
`C:\Users\dill\ao\projects\tornado-jockey`):

```powershell
$MAIN = 'C:\Users\dill\ao\projects\tornado-jockey'
Copy-Item "$MAIN\orig\Tornado.exe" orig\ -Force
Copy-Item "$MAIN\build\target" build\target -Recurse -Force
Copy-Item "$MAIN\build\functions.txt" build\ -Force
py tools\check_target.py
py configure.py --msvc "$MAIN\tools\vc71"
ninja
```

`build/target` is the delinked baseline shared by every branch. Only delink
the unit you are changing; do not re-delink the whole tree.

Ghidra refuses project folders under a path element that starts with `.`
(worktrees live under `~/.ao/...`), so `tools/delink.py` falls back to a
shared project in `%LOCALAPPDATA%\tornado-jockey\ghidra`. Give your session
its own copy so parallel agents cannot lock the same project:

```powershell
$env:GHIDRA_INSTALL_DIR = 'C:\ghidra_12.2_PUBLIC'
Copy-Item "$env:LOCALAPPDATA\tornado-jockey\ghidra" "$env:LOCALAPPDATA\tornado-jockey\ghidra-<task>" -Recurse -Force
py tools\delink.py --project "$env:LOCALAPPDATA\tornado-jockey\ghidra-<task>" Tongas2/YOURUNIT
```

## Match and measure

```powershell
py tools\decompile.py Tongas2/YOURUNIT     # cached pseudocode in build/decompile
ninja
$env:GHIDRA_INSTALL_DIR = 'C:\ghidra_12.2_PUBLIC'
py tools\report.py --objdiff tools\objdiff\objdiff-cli.exe
py tools\unit_status.py Tongas2/YOURUNIT --functions
```

`unit_status.py` prints the unit's exact and fuzzy percentages and its
unmatched functions, worst first. `function_status.py` gives the exact
per-function percentages from a one-shot objdiff diff; use it because a
function can be fuzzy 100% and still not byte-exact:

```powershell
py tools\function_status.py Tongas2/YOURUNIT
```

Fuzzy 100% is not the same as exact: a unit is complete only when objdiff
reports 100% matched code and data. For a one-function look, `objdiff-cli
diff` accepts the target and base objects:

```powershell
tools\objdiff\objdiff-cli.exe diff -1 build\target\Tongas2\YOURUNIT.obj -2 build\src\Tongas2\YOURUNIT.obj '?Symbol@@...' --format json-pretty
```

Iterate function by function. Add or rename symbols in `config/symbols.txt`
with their MSVC decorated names. Put shared declarations in `include/`; keep
changes to other units' files to what is strictly needed. Set
`"complete": true` in `config/units.json` only at 100%, and leave a partial
unit incomplete.

## Delinking pitfalls

The delinker takes a function's size from `config/symbols.txt` when it has
one, otherwise from our compiled object. A stub function that is shorter than
the target truncates the delinked target and objdiff can crash on the
resulting one-byte section. Before re-delinking after adding a stub, give
every defined function whose target body is longer its exact size from
`build/functions.txt`:

```
0x004e68e0 ?Validate@INDEXBUFFER@@QAEXXZ function 0xcc
```

A quick check for truncation:

```powershell
py tools\unit_status.py Tongas2/YOURUNIT
```

If objdiff panics with `range start index N out of range for slice of length
1`, a target function was cut by a short stub; add its size and delink again.

## Commit and report

- One unit per branch. Commit source, headers, `config/units.json`,
  `config/symbols.txt`, and notes in `docs/`; never commit `orig/`, `build/`,
  target objects, Ghidra projects or `progress/`.
- Do not merge into `main`; the integrator does that.
- End with: the unit name, exact percentage and matched function count from
  `unit_status.py`, the commit SHAs, which functions remain unmatched, and
  any quirk worth adding to `docs/toolchain.md`.
