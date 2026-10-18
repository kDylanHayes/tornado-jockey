# Maintainer setup

One-time settings that make the [Progress workflow](../.github/workflows/progress.yml) measure progress and publish the page. Contributors don't need any of this.

## Game files for CI

The workflow needs `Tornado.exe` and `VCToolkitSetup.exe`, which can't be in this public repository. It checks them out from a **private** repository instead.

1. Create a private repository, by default `<owner>/tornado-jockey-assets` (to use another name, set the repository variable `ASSETS_REPO` to `owner/name` under Settings → Secrets and variables → Actions → Variables).
2. Commit the two files to its root, with exactly those names. The workflow verifies `Tornado.exe`'s SHA-1.
3. Create a [fine-grained personal access token](https://github.com/settings/personal-access-tokens/new) with access to only that repository and the permission **Contents: Read-only**.
4. In this repository, add it as the Actions secret `ASSETS_TOKEN` (Settings → Secrets and variables → Actions → New repository secret).

Without the secret, the workflow passes and writes a note saying progress wasn't measured. GitHub never passes secrets to pull requests from forks, so the game files stay out of reach of outside code, and those pull requests are measured once merged.

## Progress page and badge

Under Settings → Pages, set **Source** to **GitHub Actions**. The next push to `main` publishes:

| URL | Contents |
|---|---|
| `https://<owner>.github.io/tornado-jockey/` | progress page with per-category bars and history |
| `.../badge.json` | [shields.io endpoint](https://shields.io/badges/endpoint-badge) for the README badge; `badge-game.json`, `badge-lib.json`, `badge-crt.json`, `badge-sdk.json` per category |
| `.../progress.json`, `.../history.json` | current numbers and one entry per push to `main` |
| `.../report.json` | the full objdiff report |

History is kept by reading the previously published `history.json` before each deploy.

## decomp.dev

[decomp.dev](https://decomp.dev) lists decompilation projects with progress from their CI. Each run uploads `build/report.json` as the artifact `Tornado_report`, the objdiff report format decomp.dev reads. Sign in there with GitHub and add the repository to list it.

## Pinned tool versions

Ghidra, the delinker and objdiff versions are pinned at the top of the workflow (`GHIDRA_VERSION`, `GHIDRA_DATE`, …). Release file names carry a build date, so update the date along with the version. Downloads are cached between runs.
