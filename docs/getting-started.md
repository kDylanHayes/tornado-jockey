# Getting started

This walks you from a fresh clone to checking a function in objdiff. It takes about half an hour, most of it downloads. Everything runs on Linux, macOS (through Wine) or Windows.

## 1. What you need to bring

The repository contains no game code and no Microsoft tools. You supply two files yourself:

| File | What it is | SHA-1 |
|---|---|---|
| `Tornado.exe` | The game, version 1.0.0.67, complete (code sections filled in) | `71be8eb97a8fb0aa161af7a2af14bed63af2c6de` |
| `VCToolkitSetup.exe` | Microsoft Visual C++ Toolkit 2003, the free compiler the game was built with | `50dc6cca1503f08a41d56816f3f1abcaf347b0bd` |

The `Tornado.exe` that the retail installer puts on disk is a shell: its headers are right but its code sections are zeros, and WildTangent's launcher fills them in at runtime. `tools/check_target.py` tells you if that's what you have. This project does not provide the complete exe or any way to remove copy protection, and issues or pull requests asking for one will be closed.

**Never commit either file**, or anything derived from them: delinked objects, Ghidra projects, dumps of disassembly. `.gitignore` and CI reject the common cases, but it's your responsibility.

## 2. Install the tools

| Tool | Version | Why |
|---|---|---|
| Python | 3.10+ | build scripts |
| [ninja](https://ninja-build.org/) | any | builds the source |
| Wine | 8+ (not needed on Windows) | runs the 32-bit compiler |
| msitools | any (not needed on Windows) | unpacks the toolkit installer without running it |
| JDK | 21 | runs Ghidra |
| [Ghidra](https://github.com/NationalSecurityAgency/ghidra/releases/tag/Ghidra_12.1.4_build) | 12.1.4 | analyzes the exe so units can be cut out of it |
| [ghidra-delinker-extension](https://github.com/boricj/ghidra-delinker-extension/releases/tag/v0.9.2) | 0.9.2, the build for Ghidra 12.1.4 | exports those units as object files |
| [objdiff](https://github.com/encounter/objdiff/releases) | 3.x, GUI and `objdiff-cli` | compares our objects with the game's |

**Ubuntu / Debian**

```sh
sudo dpkg --add-architecture i386 && sudo apt update
sudo apt install python3 ninja-build wine wine32:i386 msitools openjdk-21-jdk unzip
```

**macOS** (Homebrew)

```sh
brew install python ninja msitools openjdk@21
brew install --cask --no-quarantine wine-stable
```

**Windows**: install Python, ninja and a JDK 21; skip Wine and msitools, and run the toolkit installer normally instead of step 3's extractor.

Then unzip Ghidra somewhere and put the delinker in its `Extensions` folder (the release zip is already built for Ghidra 12.1.4, no Gradle needed):

```sh
unzip ghidra_12.1.4_PUBLIC_20260921.zip -d ~/tools
unzip ghidra_12.1.4_PUBLIC_20260926_ghidra-delinker-extension.zip -d ~/tools/ghidra_12.1.4_PUBLIC/Ghidra/Extensions
export GHIDRA_INSTALL_DIR=~/tools/ghidra_12.1.4_PUBLIC
```

## 3. Set up the repository

```sh
git clone https://github.com/kDylanHayes/tornado-jockey && cd tornado-jockey
cp /path/to/Tornado.exe orig/
python3 tools/check_target.py                                   # OK: Tornado.exe 1.0.0.67 (71be8eb9…)
python3 tools/extract_vctoolkit.py /path/to/VCToolkitSetup.exe tools/vc71
wine tools/vc71/bin/cl.exe                                      # ...Version 13.10.3077 for 80x86
```

## 4. Build and compare

```sh
python3 tools/analyze.py            # Ghidra project + auto-analysis, ~3 min, once
python3 tools/delink.py             # target objects into build/target/, ~1 min
python3 configure.py --msvc tools/vc71
ninja                               # compiles src/ into build/src/
objdiff                             # open this folder; pick a unit on the left
```

Every unit already in the repository should show 100%. Run `tools/delink.py` again when `config/symbols.txt` or `config/units.json` changes (after a `git pull`, say), and `tools/analyze.py` only when `symbols.txt` changes a lot. `ninja` re-runs `configure.py` by itself.

## 5. Check overall progress

```sh
python3 tools/report.py --objdiff /path/to/objdiff-cli    # build/report.json
python3 tools/progress.py build/report.json
```

```
All code           7.57%      180.3 KB / 2.33 MB     760 / 13,165 functions
Game and engine    0.13%        1.6 KB / 1.17 MB       19 / 4,427 functions
...
```

objdiff only counts code in units we've defined, so `tools/report.py` adds a placeholder unit for every stretch of `.text` that no unit covers yet (in its own objdiff project under `build/report/`, so the GUI isn't cluttered). The percentage is matched bytes of code out of all of the exe's code. CI runs the same two commands on every push; see [Progress tracking](#progress-tracking).

## Next

- [CONTRIBUTING.md](../CONTRIBUTING.md): picking a unit, matching it, opening a pull request
- [toolchain.md](toolchain.md): compiler flags and matching tricks learned so far
- [delinking.md](delinking.md): how target objects are made, naming symbols, placing library code
- [regions.md](regions.md): what's where in the exe
- [rea.md](rea.md): agent-assisted analysis

## Progress tracking

Every push to `main` and every pull request from this repository runs the [Progress workflow](../.github/workflows/progress.yml): it builds everything, generates the objdiff report and writes a summary into the run. On a pull request the summary also lists the functions that became matched or regressed compared with `main`. On `main` it publishes the [progress page](https://kdylanhayes.github.io/tornado-jockey/) and the README badge. The report is uploaded as the `Tornado_report` artifact in the format [decomp.dev](https://decomp.dev) reads.

Pull requests from forks don't get access to the game files, so the workflow skips the build there; run steps 4 and 5 locally and paste the numbers into your pull request.
