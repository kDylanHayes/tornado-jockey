# Agent guide

This repository is a matching decompilation of **Tornado Jockey** with
Microsoft Visual C++ 7.1 (`tools/vc71`). The work is finished only when the
source compiles to the retail bytes, function by function, and eventually the
whole link hashes to `Tornado.exe`.

Read before touching a unit:

- [CONTRIBUTING.md](CONTRIBUTING.md) — ground rules, style, what counts as a match
- [docs/getting-started.md](docs/getting-started.md) — setup and the build loop
- [docs/delinking.md](docs/delinking.md) — target objects, symbols and sizes
- [docs/toolchain.md](docs/toolchain.md#matching-notes) — flags and compiler quirks
- [docs/agent-workflow.md](docs/agent-workflow.md) — working a unit in a worktree
- [docs/matching-levers.md](docs/matching-levers.md) — what to try when a function plateaus

## Project adapter

| verb | this project |
|---|---|
| `build` | `ninja -j 2` |
| `match <unit>` | `py tools\delink.py <unit>` then `ninja` then `tools\objdiff\objdiff-cli.exe diff -p . -u <unit> --format json -o build\<unit>.json` |
| `diff <function>` | `tools\objdiff\objdiff-cli.exe diff -1 build\target\<unit>.obj -2 build\src\<unit>.obj '<mangled>' --format json-pretty` |
| `report` | `py tools\report.py --objdiff tools\objdiff\objdiff-cli.exe`, then `py tools\progress.py build\report.json --record progress` when the progress page should move |
| `gate` | `py tools\unit_status.py <unit>` and `py tools\function_status.py <unit>`: 100% exact code and data, not fuzzy 100 |
| `decompile` | `py tools\decompile.py <unit>` (cached pseudocode in `build/decompile/`) |
| `disasm` | `build/functions.txt` for addresses and sizes, `config/symbols.txt` for names, objdiff JSON for the pair |
| `permute` | no source permuter is wired up; record a diagnosed register/schedule residue and stop |

Always delink only the units you changed; `build/target` is a shared baseline.
`tools/delink.py` is slow, so batch the units of one change.

## Loop

1. **Pick work.** Lowest-scoring or never-matched functions, a unit close to
   complete, or a lost match found in history. Read the unit's notes first.
2. **Classify the gap** with [docs/matching-levers.md](docs/matching-levers.md):
   referent, inline/call set, control flow, then register/schedule.
3. **Reconstruct** the original source shape, including inline helpers and
   macros. Decompiler output is a hint, not a submission.
4. **Add or rename symbols** in `config/symbols.txt` with their MSVC decorated
   names; put shared declarations in `include/`.
5. **Compile and compare** from the first real divergence, not just the score:
   instructions, constants, call and branch structure, ordered relocations.
6. **Gate** before committing.
7. **Commit** source, headers, `config/units.json`, `config/symbols.txt` and
   notes. Never commit `orig/`, `build/`, target objects, Ghidra projects or
   game files.

## The gate

There is no full-link hash gate yet. Until there is one, a function is done
when objdiff reports **100% exact** for it (fuzzy 100% is not enough), and a
unit is done when `unit_status.py` reports 100% code and data; only then set
`"complete": true` in `config/units.json`.

Before committing a change that adds symbols, compare the new
`build/report.json` with the previous measurement: no unit may lose exact
bytes. A score dip in an unchanged function caused by codegen shifting in its
translation unit is not a regression, but a lost match is a history
investigation.

## Stop signals

- A pure register-colour difference with identical instruction and call
  structure is compiler state; do not manufacture locals to request a register.
- A single scheduler slot (one `fcos` or `fmul`) is compiler state; bank the
  best source and record the residue.
- A code-shape difference that resists every expression form (for example the
  `lea` + `mov` getters) is a helper-boundary or inline-budget question; leave
  it at the best honest source rather than adding probes.
- Record bounded residue in the unit notes or the commit message.
