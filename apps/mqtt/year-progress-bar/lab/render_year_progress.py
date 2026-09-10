#!/usr/bin/env python3
"""Render the year progress bar as a 52x16 GIF for publishing to the TC002 over MQTT.

Usage:
  python3 lab/render_year_progress.py [--date YYYY-MM-DD] [--output PATH]

Output:
  the base64-encoded GIF on stdout
  --output optionally also writes it to a file
"""

import base64
import sys
from datetime import date, datetime
from io import BytesIO
from pathlib import Path

try:
    from PIL import Image, ImageDraw
except ImportError:
    print("Pillow is required: pip install pillow", file=sys.stderr)
    sys.exit(1)

W, H = 52, 16
BG = (0, 0, 0)
DARK_GREEN = (0, 62, 26)
BRIGHT_GREEN = (0, 214, 120)
GRID_Y = 9
GRID_ROWS = 7

TINY_FONT = {
    "0": ["111", "101", "101", "101", "111"],
    "1": ["010", "110", "010", "010", "111"],
    "2": ["110", "001", "010", "100", "111"],
    "3": ["110", "001", "010", "001", "110"],
    "4": ["101", "101", "111", "001", "001"],
    "5": ["111", "100", "110", "001", "110"],
    "6": ["011", "100", "111", "101", "111"],
    "7": ["111", "001", "010", "010", "010"],
    "8": ["111", "101", "111", "101", "111"],
    "9": ["111", "101", "111", "001", "110"],
    "%": ["101", "001", "010", "100", "101"],
    "Y": ["101", "101", "010", "010", "010"],
    "W": ["10001", "10001", "10101", "10101", "01010"],
    " ": ["000", "000", "000", "000", "000"],
}


def tiny_text_width(text):
    total = 0
    for ch in text:
        glyph = TINY_FONT.get(ch.upper(), TINY_FONT[" "])
        total += len(glyph[0]) + 1
    return max(0, total - 1)


def draw_tiny_text(img, text, x, y, fill=BRIGHT_GREEN):
    draw = ImageDraw.Draw(img)
    cursor = x
    for ch in text:
        glyph = TINY_FONT.get(ch.upper(), TINY_FONT[" "])
        for gy, row in enumerate(glyph):
            for gx, val in enumerate(row):
                if val == "1":
                    px, py = cursor + gx, y + gy
                    if 0 <= px < W and 0 <= py < H:
                        draw.point((px, py), fill=fill)
        cursor += len(glyph[0]) + 1


def year_stats(today):
    start = date(today.year, 1, 1)
    end = date(today.year + 1, 1, 1)
    days_total = (end - start).days
    day_index = max(0, min(days_total - 1, (today - start).days))
    days_done = day_index + 1
    progress = max(0, min(1, days_done / days_total))
    # Account for the weekday: 1 January 2026 is a Thursday (weekday=3)
    start_weekday = start.weekday()  # 0=Monday, 3=Thursday, 6=Sunday
    # Monday is the first day, so 1 January belongs in row `weekday`
    cell_index = min(W * GRID_ROWS - 1, day_index + start_weekday)
    week_index = cell_index // GRID_ROWS
    day_row = cell_index % GRID_ROWS
    return progress, week_index, day_row, cell_index, days_done, days_total


def render(progress, current_cell, week_index, year=2026):
    img = Image.new("RGB", (W, H), BG)
    draw = ImageDraw.Draw(img)
    GRAY = (128, 128, 128)
    WHITE = (255, 255, 255)
    BRIGHT = (0, 255, 136)
    
    # Work out the width of the three text groups
    year_label = f"Y{year}"
    year_w = tiny_text_width(year_label)
    week_num = str(week_index + 1)
    week_label = f"W{week_num}"
    week_w = tiny_text_width(week_label)
    pct_label = f"{round(progress * 100)}%"
    pct_w = tiny_text_width(pct_label)
    
    # Even distribution: Y2026 on the left, W26 in the middle, 48% on the right
    gap = 4  # spacing between groups
    total = year_w + week_w + pct_w + gap * 2
    x_year = 1
    x_week = x_year + year_w + gap
    x_pct = W - pct_w - 1
    
    # Draw the text
    draw_tiny_text(img, "Y", x_year, 1, fill=GRAY)
    draw_tiny_text(img, str(year), x_year + tiny_text_width("Y") + 1, 1, fill=WHITE)
    draw_tiny_text(img, "W", x_week, 1, fill=GRAY)
    draw_tiny_text(img, week_num, x_week + tiny_text_width("W") + 1, 1, fill=WHITE)
    draw_tiny_text(img, pct_label, x_pct, 1, fill=WHITE)
    
    # Draw the calendar dot matrix
    for cell in range(current_cell + 1):
        x = cell // GRID_ROWS
        y = GRID_Y + (cell % GRID_ROWS)
        color = BRIGHT if cell == current_cell else DARK_GREEN
        draw.point((x, y), fill=color)
    return img


def render_gif(progress, current_cell, week_index):
    # No longer generates a multi-frame animation, only a static GIF
    img = render(progress, current_cell, week_index)
    buf = BytesIO()
    img.save(buf, format="GIF", loop=0)
    return base64.b64encode(buf.getvalue()).decode("ascii")


def main():
    import argparse
    parser = argparse.ArgumentParser(description="Render the year progress bar GIF")
    parser.add_argument("--date", default=None, help="use a specific date, YYYY-MM-DD")
    parser.add_argument("--output", default=None, help="also write it to a file")
    args = parser.parse_args()

    today = datetime.strptime(args.date, "%Y-%m-%d").date() if args.date else date.today()
    progress, week_index, day_row, current_cell, days_done, days_total = year_stats(today)
    b64 = render_gif(progress, current_cell, week_index)
    print(b64, end="")

    if args.output:
        raw = base64.b64decode(b64)
        Path(args.output).parent.mkdir(parents=True, exist_ok=True)
        Path(args.output).write_bytes(raw)
        print(f"\n# written to {args.output}", file=sys.stderr)

    print(f"\n# date: {today} progress: {round(progress*100)}% ({days_done}/{days_total} days) week {week_index+1}, row {day_row+1}", file=sys.stderr)


if __name__ == "__main__":
    main()
