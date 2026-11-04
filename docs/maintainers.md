# Maintainer notes

## Updating progress

CI can't measure progress: it would need `Tornado.exe` and the Visual C++ Toolkit, which can't be on GitHub. So the numbers are measured on a machine that has them (a maintainer's, or a Claude session with the project files) and committed. After merging matching work:

```bat
git checkout main && git pull
rem only if config/symbols.txt changed a lot
py tools\analyze.py
py tools\delink.py
py configure.py --msvc tools\vc71 && ninja
py tools\report.py
py tools\progress.py build\report.json --record progress
git checkout -b update-progress && git add progress && git commit -m "Update progress"
```

On Linux and macOS, use `python3` with `/` paths. `--record` saves the report to `progress/report.json` (without the per-function lists of placeholder units, to keep it small), adds an entry for the current commit to `progress/history.json`, and rewrites the page and badges. Merge that as a pull request.

## Progress page and badge

Everything lives in the repository, so nothing needs GitHub Pages or Actions:

| File | Contents |
|---|---|
| `progress/README.md` | the progress page: per-category table, chart, unit list |
| `progress/history.svg` | the chart |
| `progress/badge.json` | [shields.io endpoint](https://shields.io/badges/endpoint-badge) for the README badge (read from raw.githubusercontent.com); `badge-game.json`, `badge-lib.json`, `badge-crt.json`, `badge-sdk.json` per category |
| `progress/history.json` | one entry per measurement |
| `progress/report.json` | the objdiff report |

## decomp.dev

[decomp.dev](https://decomp.dev) reads objdiff reports from a project's GitHub Actions artifacts, which this project can't produce because CI doesn't build. `progress/report.json` is in the same format if a listing is wanted later.

## Tool versions

The pipeline has been run with Ghidra 12.2 (`ghidra_12.2_PUBLIC_20261030.zip`), ghidra-delinker-extension 0.9.3 (`ghidra_12.2_PUBLIC_20261030_ghidra-delinker-extension.zip`), JDK 25 (Temurin) and objdiff-cli 3.8.2.
