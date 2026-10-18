#!/usr/bin/env python3
"""Turn an objdiff report into progress numbers, badges and a web page.

Usage:
  progress.py build/report.json                  # print a summary
  progress.py build/report.json --markdown       # summary as Markdown (CI job summary)
  progress.py build/report.json --record progress
      saves the report in progress/, adds an entry to progress/history.json and
      rewrites the page (progress/README.md, history.svg) and badge files there
  progress.py build/report.json --changes changes.json
      Markdown list of what changed, from `objdiff-cli report changes`

The headline number is the share of the bytes of code in .text that objdiff
reports as matched, which needs a report from tools/report.py (it counts code
no unit covers yet).
"""

import argparse
import datetime
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


def bar(percent: float, width: int = 20) -> str:
    filled = round(percent / 100 * width)
    return "█" * filled + "░" * (width - filled)


def history_svg(history: list[dict]) -> str:
    w, h, left, bottom, pad = 640, 180, 44, 22, 8
    times = [datetime.datetime.fromisoformat(e["time"]).timestamp() for e in history]
    values = [e["percent"] for e in history]
    t0, t1 = times[0], max(times[-1], times[0] + 1)
    top = max(10.0, -(-max(values) * 1.2 // 10) * 10)
    x = lambda t: left + (t - t0) / (t1 - t0) * (w - left - pad)
    y = lambda v: pad + (1 - v / top) * (h - pad - bottom)
    grid = "".join(
        f'<line x1="{left}" x2="{w - pad}" y1="{y(v):.1f}" y2="{y(v):.1f}" stroke="#8884" />'
        f'<text x="{left - 6}" y="{y(v) + 4:.1f}" text-anchor="end">{v:g}%</text>'
        for v in (0, top / 2, top))
    pts = " ".join(f"{x(t):.1f},{y(v):.1f}" for t, v in zip(times, values))
    dots = "".join(f'<circle cx="{x(t):.1f}" cy="{y(v):.1f}" r="3" />' for t, v in zip(times, values))
    return (f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {w} {h}" width="{w}" height="{h}" '
            f'font-family="system-ui,sans-serif" font-size="11" fill="#888">{grid}'
            f'<polyline points="{pts}" fill="none" stroke="#2f9e5f" stroke-width="2" stroke-linejoin="round" />'
            f'<g fill="#2f9e5f">{dots}</g>'
            f'<text x="{left}" y="{h - 6}">{history[0]["time"][:10]}</text>'
            f'<text x="{w - pad}" y="{h - 6}" text-anchor="end">{history[-1]["time"][:10]}</text></svg>\n')


def markdown_page(report: dict, groups: dict, history: list[dict]) -> str:
    code = groups["code"]
    last = history[-1] if history else {}
    out = [
        "# Progress",
        "",
        f"**{code['percent']:.2f}%** of `Tornado.exe`'s code matches byte for byte: "
        f"{fmt_bytes(code['matched_code'])} of {fmt_bytes(code['total_code'])}, "
        f"{code['matched_functions']:,} of {code['total_functions']:,} functions, {code['complete_units']} units complete.",
        "",
        "| | | Matched | Code | Functions |",
        "|---|---|---:|---:|---:|",
    ]
    for g in groups.values():
        out.append(f"| {g['label']} | `{bar(g['percent'])}` | {g['percent']:.2f}% | "
                   f"{fmt_bytes(g['matched_code'])} / {fmt_bytes(g['total_code'])} | "
                   f"{g['matched_functions']:,} / {g['total_functions']:,} |")
    if len(history) > 1:
        out += ["", "## Over time", "", "![Code matched over time](history.svg)"]
    units = sorted((u for u in report.get("units", []) if not u.get("metadata", {}).get("auto_generated")),
                   key=lambda u: u["name"])
    out += ["", "## Units", "", "<details><summary>" f"{len(units)} units with source</summary>", "",
            "| Unit | Code | Matched | Complete |", "|---|---:|---:|---|"]
    out += [f"| `{u['name']}` | {fmt_bytes(number(u['measures'], 'total_code'))} | "
            f"{float(u['measures'].get('matched_code_percent') or 0):.2f}% | "
            f"{'yes' if u.get('metadata', {}).get('complete') else ''} |" for u in units]
    out += ["", "</details>", "",
            f"Measured {last.get('time', '')[:16].replace('T', ' ')} UTC at commit {last.get('commit', '')[:7]} "
            "with `tools/report.py`; see [docs/maintainers.md](../docs/maintainers.md). "
            "Generated by `tools/progress.py`; don't edit by hand.", ""]
    return "\n".join(out)


def now() -> str:
    return datetime.datetime.now(datetime.timezone.utc).replace(microsecond=0).isoformat()


def write_page(report: dict, groups: dict, history: list[dict], folder: Path) -> None:
    (folder / "README.md").write_text(markdown_page(report, groups, history))
    if len(history) > 1:
        (folder / "history.svg").write_text(history_svg(history))
    (folder / "badge.json").write_text(json.dumps(badge("decompiled", groups["code"]["percent"])) + "\n")
    for gid, g in groups.items():
        if gid != "code":
            (folder / f"badge-{gid}.json").write_text(json.dumps(badge(g["label"].lower(), g["percent"])) + "\n")


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
    write_page(report, groups, history, folder)
    print(f"Recorded {entry['percent']:.2f}% for {commit[:7]} in {folder}")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("report", type=Path, help="objdiff report (from tools/report.py)")
    parser.add_argument("--markdown", action="store_true", help="print the summary as Markdown")
    parser.add_argument("--record", type=Path, help="save the report to this folder and add it to history.json there")
    parser.add_argument("--changes", type=Path, help="also print this `objdiff-cli report changes` output")
    args = parser.parse_args()

    report = json.loads(args.report.read_text())
    groups = summarize(report)
    sys.stdout.write(text_summary(groups, args.markdown))
    if args.changes:
        sys.stdout.write("\n" + changes_markdown(json.loads(args.changes.read_text())))
    if args.record:
        record(args.report, groups, args.record)
    return 0


if __name__ == "__main__":
    sys.exit(main())
