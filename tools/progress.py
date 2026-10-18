#!/usr/bin/env python3
"""Turn an objdiff report into progress numbers, badges and a web page.

Usage:
  progress.py build/report.json                  # print a summary
  progress.py build/report.json --markdown       # summary as Markdown (CI job summary)
  progress.py build/report.json --record progress
      saves the report in progress/ and adds an entry to progress/history.json
  progress.py progress/report.json --site build/site --history progress/history.json
      writes index.html, progress.json, history.json and shields.io badge files
  progress.py build/report.json --changes changes.json
      Markdown list of what changed, from `objdiff-cli report changes`

The headline number is the share of the bytes of code in .text that objdiff
reports as matched, which needs a report from tools/report.py (it counts code
no unit covers yet).
"""

import argparse
import datetime
import html
import json
import os
import subprocess
import sys
from pathlib import Path

GROUPS = [
    ("code", "All code", None),
    ("game", "Game and engine", ("game", "engine")),
    ("lib", "Libraries", ("lib",)),
    ("crt", "C/C++ runtime", ("crt",)),
    ("sdk", "SDK libraries", ("sdk",)),
]


def number(measures: dict, key: str) -> int:
    return int(measures.get(key) or 0)


def summarize(report: dict) -> dict:
    categories = {c["id"]: c["measures"] for c in report.get("categories", [])}
    groups = {}
    for gid, label, members in GROUPS:
        sources = [report["measures"]] if members is None else [categories.get(m, {}) for m in members]
        g = {"label": label}
        for key in ("total_code", "matched_code", "total_functions", "matched_functions"):
            g[key] = sum(number(m, key) for m in sources)
        g["percent"] = 100 * g["matched_code"] / g["total_code"] if g["total_code"] else 0.0
        groups[gid] = g
    units = [u for u in report.get("units", []) if not u.get("metadata", {}).get("auto_generated")]
    groups["code"]["complete_units"] = sum(1 for u in units if u.get("metadata", {}).get("complete"))
    groups["code"]["units"] = len(units)
    return groups


def fmt_bytes(n: int) -> str:
    return f"{n / 1024:,.1f} KB" if n < 1024 * 1024 else f"{n / 1024 / 1024:,.2f} MB"


def text_summary(groups: dict, markdown: bool) -> str:
    rows = [(g["label"], f"{g['percent']:.2f}%",
             f"{fmt_bytes(g['matched_code'])} / {fmt_bytes(g['total_code'])}",
             f"{g['matched_functions']:,} / {g['total_functions']:,}") for g in groups.values()]
    if markdown:
        out = ["### Match progress", "", "| | Code matched | Bytes | Functions |", "|---|---:|---:|---:|"]
        out += [f"| {a} | {b} | {c} | {d} |" for a, b, c, d in rows]
        return "\n".join(out) + "\n"
    width = max(len(r[0]) for r in rows)
    return "\n".join(f"{a:<{width}}  {b:>7}  {c:>22}  {d:>15} functions" for a, b, c, d in rows) + "\n"


def changes_markdown(changes: dict) -> str:
    def pct(m: dict) -> float:
        return float((m or {}).get("matched_code_percent") or 0)

    before, after = pct(changes.get("from")), pct(changes.get("to"))
    out = ["### Changes from main", "",
           f"All code: {before:.2f}% → {after:.2f}% ({after - before:+.3f} points)", ""]
    matched, regressed = [], []
    for unit in changes.get("units", []):
        for f in unit.get("functions", []):
            a = float((f.get("from") or {}).get("fuzzy_match_percent") or 0)
            b = float((f.get("to") or {}).get("fuzzy_match_percent") or 0)
            size = int((f.get("to") or f.get("from") or {}).get("size") or 0)
            line = f"| `{unit['name']}` | `{f['name']}` | {size} | {a:.1f}% → {b:.1f}% |"
            if b >= 100 > a:
                matched.append(line)
            elif b < a:
                regressed.append(line)
    for title, rows in (("Newly matched", matched), ("Regressed", regressed)):
        if rows:
            out += [f"**{title}** ({len(rows)})", "", "| Unit | Function | Bytes | Match |", "|---|---|---:|---:|"]
            out += rows[:200] + ([f"| | {len(rows) - 200} more | | |"] if len(rows) > 200 else []) + [""]
    if not matched and not regressed:
        out.append("No functions changed.")
    return "\n".join(out) + "\n"


def badge(label: str, percent: float) -> dict:
    color = "brightgreen" if percent >= 99.995 else "green" if percent >= 50 else "yellowgreen" if percent >= 10 else "orange"
    return {"schemaVersion": 1, "label": label, "message": f"{percent:.2f}%", "color": color}


def chart(history: list[dict]) -> str:
    if len(history) < 2:
        return ""
    w, h, pad = 640, 160, 4
    times = [datetime.datetime.fromisoformat(e["time"]).timestamp() for e in history]
    values = [e["percent"] for e in history]
    t0, t1 = times[0], max(times[-1], times[0] + 1)
    top = max(max(values) * 1.15, 1.0)
    pts = " ".join(
        f"{pad + (t - t0) / (t1 - t0) * (w - 2 * pad):.1f},{h - pad - v / top * (h - 2 * pad):.1f}"
        for t, v in zip(times, values))
    return (f'<svg class="chart" viewBox="0 0 {w} {h}" role="img" aria-label="Code matched over time">'
            f'<polyline points="{pts}" fill="none" stroke="var(--accent)" stroke-width="2" '
            f'stroke-linejoin="round" vector-effect="non-scaling-stroke"/></svg>'
            f'<p class="axis"><span>{history[0]["time"][:10]}</span><span>{history[-1]["time"][:10]}</span></p>')


def page(report: dict, groups: dict, history: list[dict], meta: dict) -> str:
    code = groups["code"]
    bars = []
    for gid, g in groups.items():
        if gid == "code":
            continue
        bars.append(
            f'<div class="row"><div class="label">{html.escape(g["label"])}</div>'
            f'<div class="bar"><span style="width:{g["percent"]:.3f}%"></span></div>'
            f'<div class="pct">{g["percent"]:.2f}%</div>'
            f'<div class="detail">{fmt_bytes(g["matched_code"])} of {fmt_bytes(g["total_code"])}, '
            f'{g["matched_functions"]:,} of {g["total_functions"]:,} functions</div></div>')

    units = sorted((u for u in report.get("units", []) if not u.get("metadata", {}).get("auto_generated")),
                   key=lambda u: u["name"])
    unit_rows = "".join(
        f'<tr><td>{html.escape(u["name"])}</td>'
        f'<td>{fmt_bytes(number(u["measures"], "total_code"))}</td>'
        f'<td>{float(u["measures"].get("matched_code_percent") or 0):.2f}%</td>'
        f'<td>{"yes" if u.get("metadata", {}).get("complete") else ""}</td></tr>'
        for u in units)

    commit = meta.get("commit", "")
    repo = meta.get("repo", "")
    commit_html = (f'<a href="https://github.com/{html.escape(repo)}/commit/{html.escape(commit)}">{html.escape(commit[:7])}</a>'
                   if commit and repo else html.escape(commit[:7]))
    return f"""<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Tornado Jockey decomp progress</title>
<style>
:root {{ --bg:#fbfaf8; --fg:#1d1d1f; --muted:#6b6b70; --line:#e4e2dd; --track:#ecebe6; --accent:#2f7d4f; }}
@media (prefers-color-scheme: dark) {{ :root {{ --bg:#16171a; --fg:#ececee; --muted:#9a9aa2; --line:#2c2d32; --track:#26272c; --accent:#5cc28a; }} }}
* {{ box-sizing:border-box; }}
body {{ margin:0; background:var(--bg); color:var(--fg); font:15px/1.5 system-ui,-apple-system,"Segoe UI",sans-serif; }}
main {{ max-width:760px; margin:0 auto; padding:40px 16px 64px; }}
h1 {{ font-size:22px; margin:0 0 4px; }}
.sub {{ color:var(--muted); margin:0 0 32px; }}
.hero {{ display:flex; align-items:baseline; gap:12px; flex-wrap:wrap; margin-bottom:8px; }}
.hero .big {{ font-size:48px; font-weight:650; letter-spacing:-0.02em; font-variant-numeric:tabular-nums; }}
.hero .of {{ color:var(--muted); }}
.bar {{ height:10px; background:var(--track); border-radius:5px; overflow:hidden; }}
.bar span {{ display:block; height:100%; background:var(--accent); min-width:2px; }}
.hero-bar {{ margin-bottom:36px; }}
.hero-bar .bar {{ height:14px; border-radius:7px; }}
.row {{ display:grid; grid-template-columns:150px 1fr 72px; gap:4px 16px; align-items:center; padding:12px 0; border-top:1px solid var(--line); }}
.row .pct {{ text-align:right; font-variant-numeric:tabular-nums; }}
.row .detail {{ grid-column:2 / 4; color:var(--muted); font-size:13px; }}
@media (max-width:520px) {{ .row {{ grid-template-columns:1fr 64px; }} .row .bar {{ grid-column:1 / 3; grid-row:2; }} .row .detail {{ grid-column:1 / 3; }} }}
h2 {{ font-size:16px; margin:40px 0 12px; }}
.chart {{ width:100%; height:160px; display:block; border-bottom:1px solid var(--line); }}
.axis {{ display:flex; justify-content:space-between; color:var(--muted); font-size:12px; margin:4px 0 0; }}
details summary {{ cursor:pointer; color:var(--muted); }}
table {{ width:100%; border-collapse:collapse; font-size:13px; margin-top:12px; }}
th, td {{ text-align:left; padding:6px 8px 6px 0; border-bottom:1px solid var(--line); }}
td:nth-child(n+2), th:nth-child(n+2) {{ text-align:right; font-variant-numeric:tabular-nums; }}
td:first-child {{ word-break:break-all; }}
footer {{ margin-top:40px; color:var(--muted); font-size:13px; }}
a {{ color:var(--accent); }}
</style>
</head>
<body>
<main>
<h1>Tornado Jockey decompilation</h1>
<p class="sub">Matching C++ for <code>Tornado.exe</code> 1.0.0.67, measured with objdiff. <a href="https://github.com/{html.escape(repo)}">Source on GitHub</a></p>
<div class="hero"><span class="big">{code["percent"]:.2f}%</span><span class="of">of the game's code matches byte for byte</span></div>
<div class="hero-bar"><div class="bar"><span style="width:{code["percent"]:.3f}%"></span></div>
<p class="sub" style="margin:8px 0 0">{fmt_bytes(code["matched_code"])} of {fmt_bytes(code["total_code"])}, {code["matched_functions"]:,} of {code["total_functions"]:,} functions, {code["complete_units"]} units complete</p></div>
{"".join(bars)}
{"<h2>Over time</h2>" + chart(history) if len(history) > 1 else ""}
<h2>Units</h2>
<details><summary>{len(units)} units with source</summary>
<table><thead><tr><th>Unit</th><th>Code</th><th>Matched</th><th>Complete</th></tr></thead><tbody>{unit_rows}</tbody></table>
</details>
<footer>Updated {html.escape(meta.get("time", "")[:16].replace("T", " "))} UTC from commit {commit_html}. Badge data: <a href="badge.json">badge.json</a>, full report: <a href="report.json">report.json</a>.</footer>
</main>
</body>
</html>
"""


def now() -> str:
    return datetime.datetime.now(datetime.timezone.utc).replace(microsecond=0).isoformat()


def record(report_path: Path, groups: dict, folder: Path) -> None:
    """Keep the report in the repository, with one history entry per commit measured."""
    commit = subprocess.run(["git", "rev-parse", "HEAD"], capture_output=True, text=True,
                            cwd=Path(__file__).resolve().parent).stdout.strip()
    folder.mkdir(parents=True, exist_ok=True)
    history_path = folder / "history.json"
    history = json.loads(history_path.read_text()) if history_path.is_file() else []
    entry = {"time": now(), "commit": commit, "percent": round(groups["code"]["percent"], 4),
             **{gid: {"matched_code": g["matched_code"], "total_code": g["total_code"]} for gid, g in groups.items()}}
    history = [e for e in history if e.get("commit") != commit] + [entry]
    history_path.write_text(json.dumps(history, indent=1) + "\n")
    # Placeholder units' per-function lists are just Ghidra's FUN_ names; their
    # totals are enough, and dropping them keeps the committed file small.
    report = json.loads(report_path.read_text())
    for unit in report.get("units", []):
        if unit.get("metadata", {}).get("auto_generated"):
            unit.pop("functions", None)
    (folder / "report.json").write_text(json.dumps(report, separators=(",", ":")) + "\n")
    print(f"Recorded {entry['percent']:.2f}% for {commit[:7]} in {folder}")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("report", type=Path, help="objdiff report (from tools/report.py)")
    parser.add_argument("--markdown", action="store_true", help="print the summary as Markdown")
    parser.add_argument("--site", type=Path, help="write the progress page and badges to this folder")
    parser.add_argument("--record", type=Path, help="save the report to this folder and add it to history.json there")
    parser.add_argument("--history", type=Path, help="history.json to chart on the page")
    parser.add_argument("--changes", type=Path, help="also print this `objdiff-cli report changes` output")
    args = parser.parse_args()

    report = json.loads(args.report.read_text())
    groups = summarize(report)
    sys.stdout.write(text_summary(groups, args.markdown))
    if args.changes:
        sys.stdout.write("\n" + changes_markdown(json.loads(args.changes.read_text())))
    if args.record:
        record(args.report, groups, args.record)
    if not args.site:
        return 0

    history = []
    if args.history and args.history.is_file():
        history = json.loads(args.history.read_text())
    last = history[-1] if history else {}
    meta = {
        "time": last.get("time") or now(),
        "commit": last.get("commit", ""),
        "repo": os.environ.get("GITHUB_REPOSITORY", "kDylanHayes/tornado-jockey"),
    }

    site = args.site
    site.mkdir(parents=True, exist_ok=True)
    (site / "index.html").write_text(page(report, groups, history, meta))
    (site / "progress.json").write_text(json.dumps({**meta, "groups": groups}, indent=2) + "\n")
    (site / "history.json").write_text(json.dumps(history, indent=1) + "\n")
    (site / "report.json").write_text(args.report.read_text())
    (site / "badge.json").write_text(json.dumps(badge("decompiled", groups["code"]["percent"])) + "\n")
    for gid, g in groups.items():
        if gid != "code":
            (site / f"badge-{gid}.json").write_text(json.dumps(badge(g["label"].lower(), g["percent"])) + "\n")
    print(f"Wrote progress page to {site}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
