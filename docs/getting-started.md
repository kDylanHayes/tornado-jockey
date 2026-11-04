# Getting started

This walks you from a fresh clone to checking a function in objdiff. It takes about half an hour, most of it downloads. Everything runs on Windows natively; Linux and macOS run the 32-bit compiler through Wine.

## 1. What you need to bring

The repository contains no game code and no Microsoft tools. You supply two files yourself:

| File | What it is | SHA-1 |
|---|---|---|
| `Tornado.exe` | The game, version 1.0.0.67, complete (code sections filled in) | `20bf7200effe49d2dff295a3b6c546a5804e06bc` |
| `VCToolkitSetup.exe` | Microsoft Visual C++ Toolkit 2003, the free compiler the game was built with | `50dc6cca1503f08a41d56816f3f1abcaf347b0bd` |

The target is the complete image extracted from the installer in the game's ISO (its payload is `Tornado-WT.exe`). The `Tornado.exe` that the retail installer puts on disk is a shell: its headers are right but its code sections are zeros, and WildTangent's launcher fills them in at runtime. `tools/check_target.py` tells you if that's what you have. The ISO image differs from the earlier retail image (`71be8eb9…`, which is no longer available) in 18 bytes at VA `0x59188f`, in `lib/ode/joint`. This project does not provide the complete exe or any way to remove copy protection, and issues or pull requests asking for one will be closed.

**Never commit either file**, or anything derived from them: delinked objects, Ghidra projects, dumps of disassembly. `.gitignore` and CI reject the common cases, but it's your responsibility.

## 2. Install the tools

| Tool | Version | Why |
|---|---|---|
| Python | 3.10+ | build scripts |
| [ninja](https://ninja-build.org/) | any | builds the source |
| JDK | 21+ (tested Temurin 25) | runs Ghidra |
| [Ghidra](https://github.com/NationalSecurityAgency/ghidra/releases) | 12.2 | analyzes the exe so units can be cut out of it |
| [ghidra-delinker-extension](https://github.com/boricj/ghidra-delinker-extension/releases/tag/v0.9.3) | 0.9.3, the build for Ghidra 12.2 | exports those units as object files |
| [objdiff](https://github.com/encounter/objdiff/releases) | 3.x, GUI and `objdiff-cli` | compares our objects with the game's |
| Wine | 8+ (Linux and macOS only) | runs the 32-bit compiler |
| msitools | any (Linux and macOS only) | unpacks the toolkit installer |
| 7-Zip | any (Windows only) | unpacks the toolkit installer |

**Windows**

Install Python 3.10+, ninja and a JDK 21+. The compiler, Ghidra and the toolkit extractor all run natively; Windows needs no Wine or msitools. The extractor's only extra is 7-Zip.

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

Then unzip Ghidra somewhere and put the delinker in its `Extensions` folder (the release zip is already built for Ghidra 12.2, no Gradle needed). On Windows:

```powershell
Expand-Archive .\ghidra_12.2_PUBLIC_20261030.zip -DestinationPath C:\
Expand-Archive .\ghidra_12.2_PUBLIC_20261030_ghidra-delinker-extension.zip -DestinationPath C:\ghidra_12.2_PUBLIC\Ghidra\Extensions
$env:GHIDRA_INSTALL_DIR = "C:\ghidra_12.2_PUBLIC"
```

On Linux or macOS:

```sh
unzip ghidra_12.2_PUBLIC_20261030.zip -d ~/tools
unzip ghidra_12.2_PUBLIC_20261030_ghidra-delinker-extension.zip -d ~/tools/ghidra_12.2_PUBLIC/Ghidra/Extensions
export GHIDRA_INSTALL_DIR=~/tools/ghidra_12.2_PUBLIC
```

## 3. Set up the repository

On Windows:

```bat
git clone https://github.com/kDylanHayes/tornado-jockey && cd tornado-jockey
copy C:\path\to\Tornado.exe orig\
py tools\check_target.py
py tools\extract_vctoolkit.py C:\path\to\VCToolkitSetup.exe tools\vc71
tools\vc71\bin\cl.exe
```

`check_target.py` should print `OK: Tornado.exe 1.0.0.67 (20bf7200…)`, and `cl.exe` should report version 13.10.3077.

`extract_vctoolkit.py` unpacks the installer without running it, using 7-Zip and the MSI tables on Windows and `msiextract` on Linux/macOS. You can also run the toolkit installer normally and point `--msvc` at its `bin` folder's parent.

On Linux or macOS, the same steps with `python3` and Wine:

```sh
git clone https://github.com/kDylanHayes/tornado-jockey && cd tornado-jockey
cp /path/to/Tornado.exe orig/
python3 tools/check_target.py                                   # OK: Tornado.exe 1.0.0.67 (20bf7200…)
python3 tools/extract_vctoolkit.py /path/to/VCToolkitSetup.exe tools/vc71
wine tools/vc71/bin/cl.exe                                      # ...Version 13.10.3077 for 80x86
```

## 4. Build and compare

On Windows:

```bat
rem Ghidra project + auto-analysis, ~3 min, once
py tools\analyze.py
rem target objects into build\target\, ~1 min
py tools\delink.py
py configure.py --msvc tools\vc71
rem compiles src\ into build\src\
ninja
rem open this folder; pick a unit on the left
objdiff
```

On Linux or macOS, use `python3 configure.py --msvc tools/vc71` and the other commands with `python3` and `/` paths; `--wine` defaults to `wine` there.

Every unit already in the repository should show 100%. Run `tools/delink.py` again when `config/symbols.txt` or `config/units.json` changes (after a `git pull`, say), and `tools/analyze.py` only when `symbols.txt` changes a lot. `ninja` re-runs `configure.py` by itself.

## 5. Check overall progress

On Windows:

```bat
py tools\report.py --objdiff C:\path\to\objdiff-cli
py tools\progress.py build\report.json
```

```
All code          16.71%      397.6 KB / 2.32 MB   1,311 / 13,241 functions
Game and engine    0.33%        4.0 KB / 1.17 MB       31 / 4,453 functions
...
```

objdiff only counts code in units we've defined, so `tools/report.py` adds a placeholder unit for every stretch of `.text` that no unit covers yet (in its own objdiff project under `build/report/`, so the GUI isn't cluttered). The percentage is matched bytes of code out of all of the exe's code. See [Progress tracking](#progress-tracking) for how the published numbers are updated.

## Next

- [CONTRIBUTING.md](../CONTRIBUTING.md): picking a unit, matching it, opening a pull request
- [toolchain.md](toolchain.md): compiler flags and matching tricks learned so far
- [delinking.md](delinking.md): how target objects are made, naming symbols, placing library code
- [regions.md](regions.md): what's where in the exe
- [rea.md](rea.md): agent-assisted analysis

## Progress tracking

The game exe and the compiler can't be on GitHub, so CI doesn't build anything. Progress is measured on a machine that has them and committed to `progress/`: the objdiff report, a history entry per measurement, the [progress page](../progress/README.md) with its chart, and the shields.io badge files the README badge reads. Contributors don't commit `progress/`; see [maintainers.md](maintainers.md) for how it's updated.

To see what your branch changes, measure `main` and your branch and compare them:

```sh
objdiff-cli report changes main-report.json build/report.json -o changes.json
py tools\progress.py build/report.json --changes changes.json
```
