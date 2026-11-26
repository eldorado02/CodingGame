// Back to the Code — Rust port of cpp/bot.cpp (same rectangles, same tie-breaks).
// Build: cargo build --release
use std::io::{self, Write};

const WIDTH: i32 = 35;
const HEIGHT: i32 = 20;
const MAX_SIDE: i32 = 12; // bigger loops get cut by the opponents
const NEUTRAL: u8 = b'.';
const MINE: u8 = b'0';

#[derive(Clone, Copy)]
struct Point { x: i32, y: i32 }

#[derive(Clone, Copy)]
struct Rect { x0: i32, y0: i32, x1: i32, y1: i32 }

/// Count of cells matching a predicate in any rectangle, O(1).
struct Prefix { sum: Vec<Vec<i32>> }

impl Prefix {
    fn new(grid: &[Vec<u8>], matches: impl Fn(u8) -> bool) -> Self {
        let (w, h) = (WIDTH as usize, HEIGHT as usize);
        let mut sum = vec![vec![0; w + 1]; h + 1];
        for y in 0..h {
            for x in 0..w {
                sum[y + 1][x + 1] = sum[y][x + 1] + sum[y + 1][x] - sum[y][x] + matches(grid[y][x]) as i32;
            }
        }
        Prefix { sum }
    }

    fn count(&self, x0: i32, y0: i32, x1: i32, y1: i32) -> i32 {
        if x0 > x1 || y0 > y1 { return 0; }
        let (x0, y0, x1, y1) = (x0 as usize, y0 as usize, x1 as usize + 1, y1 as usize + 1);
        self.sum[y1][x1] - self.sum[y0][x1] - self.sum[y1][x0] + self.sum[y0][x0]
    }
}

struct Counts { neutral: Prefix, mine: Prefix, foe: Prefix }

fn distance_to_rect(p: Point, r: Rect) -> i32 { (r.x0 - p.x).max(0).max(p.x - r.x1) + (r.y0 - p.y).max(0).max(p.y - r.y1) }

/// (gain, cost) of claiming this rectangle, or None when it is not worth it.
fn rect_value(r: Rect, c: &Counts, opponents: &[Point]) -> Option<(i32, i32)> {
    if c.foe.count(r.x0, r.y0, r.x1, r.y1) > 0 { return None; }
    let (w, h) = (r.x1 - r.x0 + 1, r.y1 - r.y0 + 1);
    let gain = c.neutral.count(r.x0, r.y0, r.x1, r.y1);
    let border = if w > 1 && h > 1 { w * h - (w - 2) * (h - 2) } else { w * h };
    let inner_mine = c.mine.count(r.x0 + 1, r.y0 + 1, r.x1 - 1, r.y1 - 1);
    let cost = border - (c.mine.count(r.x0, r.y0, r.x1, r.y1) - inner_mine);
    if gain == 0 || cost == 0 { return None; }
    if opponents.iter().any(|&o| 2 * distance_to_rect(o, r) < cost) { return None; } // they could cut the loop
    Some((gain, cost))
}

/// Rectangle with us on its border maximising gain / cost; ties: bigger gain.
fn best_rect(me: Point, c: &Counts, opponents: &[Point]) -> Option<Rect> {
    let mut best: Option<(Rect, i32, i32)> = None;
    for y0 in (me.y - MAX_SIDE + 1).max(0)..=me.y {
        for y1 in me.y..HEIGHT.min(y0 + MAX_SIDE) {
            for x0 in (me.x - MAX_SIDE + 1).max(0)..=me.x {
                for x1 in me.x..WIDTH.min(x0 + MAX_SIDE) {
                    if me.x != x0 && me.x != x1 && me.y != y0 && me.y != y1 { continue; }
                    let r = Rect { x0, y0, x1, y1 };
                    let Some((gain, cost)) = rect_value(r, c, opponents) else { continue };
                    let better = match best {
                        None => true,
                        Some((_, bg, bc)) => {
                            let (lhs, rhs) = (gain as i64 * bc as i64, bg as i64 * cost as i64);
                            lhs > rhs || (lhs == rhs && gain > bg)
                        }
                    };
                    if better { best = Some((r, gain, cost)); }
                }
            }
        }
    }
    best.map(|(r, _, _)| r)
}

/// Clockwise corner to aim at from our cell on the border.
fn next_corner(me: Point, r: Rect) -> Point {
    if me.y == r.y0 && me.x < r.x1 { return Point { x: r.x1, y: r.y0 }; }
    if me.x == r.x1 && me.y < r.y1 { return Point { x: r.x1, y: r.y1 }; }
    if me.y == r.y1 && me.x > r.x0 { return Point { x: r.x0, y: r.y1 }; }
    Point { x: r.x0, y: r.y0 }
}

fn on_border(me: Point, r: Rect) -> bool {
    r.x0 <= me.x && me.x <= r.x1 && r.y0 <= me.y && me.y <= r.y1 && (me.x == r.x0 || me.x == r.x1 || me.y == r.y0 || me.y == r.y1)
}

fn nearest_neutral(me: Point, grid: &[Vec<u8>]) -> Point {
    let mut best: Option<(Point, i32)> = None;
    for y in 0..HEIGHT {
        for x in 0..WIDTH {
            let d = (x - me.x).abs() + (y - me.y).abs();
            if grid[y as usize][x as usize] == NEUTRAL && best.map_or(true, |(_, bd)| d < bd) { best = Some((Point { x, y }, d)); }
        }
    }
    best.map_or(me, |(p, _)| p)
}

/// The rectangle followed across turns lives in `plan`.
fn decide(me: Point, opponents: &[Point], grid: &[Vec<u8>], plan: &mut Option<Rect>) -> Point {
    let c = Counts {
        neutral: Prefix::new(grid, |ch| ch == NEUTRAL),
        mine: Prefix::new(grid, |ch| ch == MINE),
        foe: Prefix::new(grid, |ch| ch != NEUTRAL && ch != MINE),
    };
    let keep = plan.map_or(false, |r| on_border(me, r) && rect_value(r, &c, opponents).is_some());
    if !keep { *plan = best_rect(me, &c, opponents); }
    match *plan {
        Some(r) => next_corner(me, r),
        None => nearest_neutral(me, grid),
    }
}

/// Whitespace-separated words as they arrive (reading all stdin at once would block the game).
fn words() -> impl Iterator<Item = String> {
    io::stdin()
        .lines()
        .map_while(Result::ok)
        .flat_map(|line| line.split_ascii_whitespace().map(str::to_string).collect::<Vec<_>>())
}

fn main() {
    let mut words = words();
    let int = |w: &mut dyn Iterator<Item = String>| w.next().and_then(|t| t.parse::<i32>().ok());
    let Some(opponent_count) = int(&mut words) else { return };
    let stdout = io::stdout();
    let mut out = stdout.lock();
    let mut plan: Option<Rect> = None;
    while int(&mut words).is_some() {
        let mut head = [0; 3];
        for v in head.iter_mut() {
            let Some(n) = int(&mut words) else { return };
            *v = n;
        }
        let me = Point { x: head[0], y: head[1] };
        let mut opponents = Vec::new();
        for _ in 0..opponent_count {
            let mut o = [0; 3];
            for v in o.iter_mut() {
                let Some(n) = int(&mut words) else { return };
                *v = n;
            }
            if o[0] >= 0 { opponents.push(Point { x: o[0], y: o[1] }); }
        }
        let mut grid = Vec::with_capacity(HEIGHT as usize);
        for _ in 0..HEIGHT {
            let Some(row) = words.next() else { return };
            let mut cells = row.into_bytes();
            cells.resize(WIDTH as usize, NEUTRAL);
            grid.push(cells);
        }
        let target = decide(me, &opponents, &grid, &mut plan);
        writeln!(out, "{} {}", target.x, target.y).unwrap();
        out.flush().unwrap();
    }
}
