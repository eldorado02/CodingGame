// Clash of Bots — Rust port of cpp/bot.cpp (same rules, same tie-breaks).
// Build: cargo build --release
use std::io::{self, Write};

const SIZE: usize = 5;
const CENTER: i32 = 2;
const EXPLOSION_DAMAGE: i32 = 4;
const KILLS_TO_EXPLODE: i32 = 2;
/// (name, row delta, column delta), in tie-break order.
const DIRECTIONS: [(&str, i32, i32); 4] = [("UP", -1, 0), ("DOWN", 1, 0), ("LEFT", 0, -1), ("RIGHT", 0, 1)];
const DRIFT: usize = 0; // UP: the whole army keeps its shape

type Grid = [[i32; SIZE]; SIZE];

fn manhattan(r1: i32, c1: i32, r2: i32, c2: i32) -> i32 { (r1 - r2).abs() + (c1 - c2).abs() }
fn at(g: &Grid, r: i32, c: i32) -> i32 { g[r as usize][c as usize] }
fn all_cells() -> impl Iterator<Item = (i32, i32)> { (0..SIZE as i32).flat_map(|r| (0..SIZE as i32).map(move |c| (r, c))) }

fn explosion_pays(g: &Grid) -> bool {
    let mut kills = 0;
    for r in CENTER - 1..=CENTER + 1 {
        for c in CENTER - 1..=CENTER + 1 {
            if r == CENTER && c == CENTER { continue; }
            let v = at(g, r, c);
            if v > 0 { return false; }
            if v < 0 && -v <= EXPLOSION_DAMAGE { kills += 1; }
        }
    }
    kills >= KILLS_TO_EXPLODE
}

fn adjacent_attack(g: &Grid) -> Option<usize> {
    let mut best: Option<(usize, i32)> = None;
    for (d, &(_, dr, dc)) in DIRECTIONS.iter().enumerate() {
        let v = at(g, CENTER + dr, CENTER + dc);
        if v < 0 && best.map_or(true, |(_, hp)| -v < hp) { best = Some((d, -v)); }
    }
    best.map(|(d, _)| d)
}

/// Side cell (empty) that the most enemies at distance 2 can move into.
fn ambush_attack(g: &Grid) -> Option<usize> {
    let mut best: Option<usize> = None;
    let mut best_count = 0;
    for (d, &(_, dr, dc)) in DIRECTIONS.iter().enumerate() {
        let (r, c) = (CENTER + dr, CENTER + dc);
        if at(g, r, c) != 0 { continue; }
        let count = all_cells()
            .filter(|&(er, ec)| at(g, er, ec) < 0 && manhattan(er, ec, CENTER, CENTER) == 2 && manhattan(er, ec, r, c) == 1)
            .count();
        if count > best_count {
            best = Some(d);
            best_count = count;
        }
    }
    best
}

/// First free side cell that brings us closer to the nearest enemy.
fn approach(g: &Grid) -> Option<usize> {
    // nearest, then smallest row, then column
    let (er, ec) = all_cells().filter(|&(r, c)| at(g, r, c) < 0).min_by_key(|&(r, c)| manhattan(r, c, CENTER, CENTER))?;
    DIRECTIONS.iter().position(|&(_, dr, dc)| {
        let (r, c) = (CENTER + dr, CENTER + dc);
        at(g, r, c) == 0 && manhattan(er, ec, r, c) < manhattan(er, ec, CENTER, CENTER)
    })
}

fn decide(g: &Grid) -> String {
    if explosion_pays(g) { return "SELFDESTRUCTION".to_string(); }
    if let Some(d) = adjacent_attack(g).or_else(|| ambush_attack(g)) { return format!("ATTACK {}", DIRECTIONS[d].0); }
    if let Some(d) = approach(g) { return format!("MOVE {}", DIRECTIONS[d].0); }
    let (name, dr, dc) = DIRECTIONS[DRIFT];
    let any_enemy = all_cells().any(|(r, c)| at(g, r, c) < 0);
    if !any_enemy && at(g, CENTER + dr, CENTER + dc) == 0 { return format!("MOVE {}", name); }
    "GUARD".to_string()
}

/// Integers as they arrive, line by line (reading all stdin at once would block the game).
fn tokens() -> impl Iterator<Item = i32> {
    io::stdin()
        .lines()
        .map_while(Result::ok)
        .flat_map(|line| line.split_ascii_whitespace().map(|t| t.parse::<i32>().unwrap_or(0)).collect::<Vec<_>>())
}

fn main() {
    let mut tokens = tokens();
    let stdout = io::stdout();
    let mut out = stdout.lock();
    while let Some(count) = tokens.next() {
        let mut text = String::new();
        for _ in 0..count {
            let mut g: Grid = [[0; SIZE]; SIZE];
            for row in g.iter_mut() {
                for v in row.iter_mut() { *v = tokens.next().unwrap_or(0); }
            }
            text += &decide(&g);
            text.push('\n');
        }
        out.write_all(text.as_bytes()).unwrap();
        out.flush().unwrap();
    }
}
