# Maintainer notes

## Updating progress

CI can't measure progress: it would need `Tornado.exe` and the Visual C++ Toolkit, which can't be on GitHub. So the numbers are measured on a machine that has them (a maintainer's, or a Claude session with the project files) and committed. After merging matching work:

```sh
git checkout main && git pull
python3 tools/analyze.py            # only if config/symbols.txt changed a lot
python3 tools/delink.py
python3 configure.py --msvc tools/vc71 && ninja
python3 tools/report.py
python3 tools/progress.py build/report.json --record progress
git checkout -b update-progress && git add progress && git commit -m "Update progress"
```

`--record` saves the report to `progress/report.json` (without the per-function lists of placeholder units, to keep it small), adds an entry for the current commit to `progress/history.json`, and rewrites the page and badges. Merge that as a pull request.

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

The pipeline has been run with Ghidra 12.1.4 (`ghidra_12.1.4_PUBLIC_20260921.zip`), ghidra-delinker-extension 0.9.2 (`ghidra_12.1.4_PUBLIC_20260926_ghidra-delinker-extension.zip`) and objdiff-cli 3.8.2.
