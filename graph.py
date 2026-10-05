#!/usr/bin/env python3
"""Turn the Big O demo's CSV output into charts -- no installs needed.

    python3 graph.py            # time charts   (results.csv       -> charts.html)
    python3 graph.py --space    # space charts  (results_space.csv -> charts_space.html)

The C++ programs call this for you when they finish, so normally you just run
the program and open the HTML file it names.

Why not Plotly, like the later Big O demos? Plotly has to be pip-installed, and
inside the Dev Container (Ubuntu 24.04) a plain `pip install` is refused. This
script uses only the Python standard library and writes plain SVG, so it works
the same in the container, on Windows, on a Mac, and offline.
"""

import csv
import html
import math
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent

# Accessible palette from the course CLAUDE.md -- every one is >= 4.5:1 on white.
COLORS = ["#2573a7", "#c0392b", "#1c7e45", "#a95c19", "#8e44ad", "#00695c"]

W, H = 720, 360                 # chart size in SVG units
L, R, T, B = 80, 230, 40, 56    # margins: left, right (legend), top, bottom


def nice_ticks(hi, count=5):
    """Round tick values from 0 to a little above hi."""
    if hi <= 0:
        return [0, 1]
    raw = hi / count
    mag = 10 ** math.floor(math.log10(raw))
    step = min((m * mag for m in (1, 2, 2.5, 5, 10) if m * mag >= raw))
    top = step * math.ceil(hi / step)
    n = int(round(top / step))
    return [i * step for i in range(n + 1)]


def fmt(v):
    if v == 0:
        return "0"
    if abs(v) >= 1000:
        return f"{v:,.0f}"
    if abs(v) >= 10:
        return f"{v:.0f}"
    if abs(v) >= 1:
        return f"{v:.1f}"
    return f"{v:.3f}"


def svg_chart(title, subtitle, unit, series):
    """series: list of (label, [(x, y), ...]). Returns an <svg> string."""
    xs = sorted({x for _, pts in series for x, _ in pts})
    ymax = max((y for _, pts in series for _, y in pts), default=1)
    yt = nice_ticks(ymax)
    xmin, xmax = min(xs), max(xs)
    pw, ph = W - L - R, H - T - B

    def px(x):
        return L + (0 if xmax == xmin else (x - xmin) / (xmax - xmin) * pw)

    def py(y):
        return T + ph - (y / yt[-1]) * ph

    out = [f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {W} {H}" '
           f'role="img" aria-labelledby="t-{id(series)}" '
           f'font-family="Segoe UI, system-ui, sans-serif">',
           f'<title id="t-{id(series)}">{html.escape(title)}: {html.escape(subtitle)}</title>',
           f'<rect width="100%" height="100%" fill="white"/>',
           f'<text x="{L}" y="22" font-size="15" font-weight="600" fill="#2c3e50">{html.escape(title)}</text>']
    for v in yt:
        y = py(v)
        out.append(f'<line x1="{L}" x2="{L + pw}" y1="{y:.1f}" y2="{y:.1f}" stroke="#e5e7eb"/>')
        out.append(f'<text x="{L - 8}" y="{y + 4:.1f}" font-size="11" text-anchor="end" fill="#555555">{fmt(v)}</text>')
    last_label = -1e9
    for x in reversed(xs):          # right to left, so the biggest n always shows
        if last_label - px(x) < 44:     # would overlap the label to its right
            if x != xs[-1]:
                continue
        out.append(f'<text x="{px(x):.1f}" y="{T + ph + 18}" font-size="11" text-anchor="middle" fill="#555555">{x:,}</text>')
        last_label = px(x)
    out.append(f'<line x1="{L}" x2="{L + pw}" y1="{T + ph}" y2="{T + ph}" stroke="#2c3e50"/>')
    out.append(f'<line x1="{L}" x2="{L}" y1="{T}" y2="{T + ph}" stroke="#2c3e50"/>')
    out.append(f'<text x="{L + pw / 2}" y="{H - 12}" font-size="12" text-anchor="middle" fill="#2c3e50">n (number of elements)</text>')
    out.append(f'<text transform="translate(18 {T + ph / 2}) rotate(-90)" font-size="12" text-anchor="middle" fill="#2c3e50">{html.escape(unit)}</text>')
    for i, (label, pts) in enumerate(series):
        c = COLORS[i % len(COLORS)]
        pts = sorted(pts)
        d = " ".join(f"{px(x):.1f},{py(y):.1f}" for x, y in pts)
        # Earlier series are drawn wider, so two series with identical values
        # show as a line inside an outline instead of one hiding the other.
        width = max(2.5, 6.5 - 2 * i)
        out.append(f'<polyline points="{d}" fill="none" stroke="{c}" stroke-width="{width}"/>')
        for x, y in pts:
            out.append(f'<circle cx="{px(x):.1f}" cy="{py(y):.1f}" r="3.5" fill="white" stroke="{c}" stroke-width="2"/>')
        ly = T + 8 + i * 20
        out.append(f'<line x1="{L + pw + 16}" x2="{L + pw + 36}" y1="{ly}" y2="{ly}" stroke="{c}" stroke-width="3"/>')
        out.append(f'<text x="{L + pw + 42}" y="{ly + 4}" font-size="12" fill="#2c3e50">{html.escape(label)}</text>')
    out.append("</svg>")
    return "\n".join(out)


def table(unit, series):
    xs = sorted({x for _, pts in series for x, _ in pts})
    head = "".join(f"<th scope='col'>{x:,}</th>" for x in xs)
    rows = []
    for label, pts in series:
        d = dict(pts)
        cells = "".join(f"<td>{fmt(d[x]) if x in d else ''}</td>" for x in xs)
        rows.append(f"<tr><th scope='row'>{html.escape(label)}</th>{cells}</tr>")
    return (f"<table><caption>{html.escape(unit)} at each n</caption>"
            f"<thead><tr><th scope='col'>n</th>{head}</tr></thead>"
            f"<tbody>{''.join(rows)}</tbody></table>")


def load(path, space):
    """Group rows into charts: {chart title: (subtitle, unit, {series: [(n, y)]})}."""
    charts = {}
    with open(path, newline="", encoding="utf-8") as fh:
        for row in csv.DictReader(fh):
            n = int(row["n"])
            if space:
                title, unit = row["metric"], row["unit"]
                label, sub = row["structure"], row.get("note", "")
                y = float(row["value"])
            else:
                title, unit = row["operation"], "microseconds per operation"
                label = f'{row["structure"]}  {row["complexity"]}'
                sub = row.get("note", "")
                y = float(row["time_us"])
            sub0, unit0, ser = charts.setdefault(title, (sub, unit, {}))
            ser.setdefault(label, []).append((n, y))
    return charts


def main():
    space = "--space" in sys.argv
    src = HERE / ("results_space.csv" if space else "results.csv")
    dst = HERE / ("charts_space.html" if space else "charts.html")
    if not src.exists():
        sys.exit(f"{src.name} not found -- run the demo program first; it writes this file.")
    charts = load(src, space)
    name = HERE.name
    kind = "Space" if space else "Time"
    parts = [f"<!doctype html><html lang='en'><head><meta charset='utf-8'>",
             f"<meta name='viewport' content='width=device-width, initial-scale=1'>",
             f"<title>{html.escape(name)} &middot; {kind}</title>",
             "<style>body{font-family:'Segoe UI',system-ui,sans-serif;color:#2c3e50;max-width:900px;"
             "margin:2rem auto;padding:0 1rem;background:#fff}h1{font-size:1.5rem}"
             "figure{margin:2rem 0}figcaption{color:#555;font-size:.95rem;margin:.3rem 0 .6rem}"
             "svg{width:100%;height:auto;border:1px solid #cbd5e1;border-radius:8px}"
             "table{border-collapse:collapse;font-size:.85rem;margin-top:.6rem}"
             "th,td{border:1px solid #cbd5e1;padding:.25rem .6rem;text-align:right}"
             "th[scope=row]{text-align:left}caption{text-align:left;color:#555;padding-bottom:.3rem}"
             "</style></head><body>",
             f"<h1>{html.escape(name)} &mdash; {kind}</h1>",
             "<p>Each chart is one operation, measured at growing sizes. A flat line is O(1); "
             "a straight line rising with n is O(n); a line that curves upward is O(n&sup2;).</p>"
             if not space else
             "<p>Each chart is one measurement of memory or work, at growing sizes.</p>"]
    for title, (sub, unit, ser) in charts.items():
        series = list(ser.items())
        parts.append("<figure>")
        parts.append(svg_chart(title, sub or unit, unit, series))
        if sub:
            parts.append(f"<figcaption>{html.escape(sub)}</figcaption>")
        parts.append(table(unit, series))
        parts.append("</figure>")
    parts.append("</body></html>")
    dst.write_text("\n".join(parts), encoding="utf-8")
    print(f"  Charts written to {dst}")
    print("  Open that file in a browser to see them.")


if __name__ == "__main__":
    main()
