// Penguins — Rust port of solutions/python/bot.py (v2 greedy + mobility).
// Rows 1, 3, ... (index 0, 2, ...) are the narrow shifted rows; see step().
use std::collections::{HashMap, HashSet};
use std::io::{self, BufRead, BufWriter, Write};

const SOURCE_WEIGHT: i32 = 10; // fish eaten now outweigh fish reachable later
const MOBILITY_WEIGHT: i32 = 1; // a penguin with few rays left gets trapped
type Cell = (i32, i32); // (row, x)

/// Neighbour in direction d (0 E, 1 W, 2 NE, 3 NW, 4 SE, 5 SW).
fn step((r, x): Cell, d: usize) -> Cell {
    let narrow = r % 2 == 0;
    match d {
        0 => (r, x + 1),
        1 => (r, x - 1),
        _ => {
            let dr = if d == 2 || d == 3 { 1 } else { -1 };
            let right = d == 2 || d == 4;
            if narrow { (r + dr, if right { x + 1 } else { x }) } else { (r + dr, if right { x } else { x - 1 }) }
        }
    }
}

fn cell_char(b: &[Vec<u8>], (r, x): Cell) -> Option<u8> {
    if r < 0 || x < 0 { return None; }
    b.get(r as usize).and_then(|row| row.get(x as usize)).copied()
}

fn is_ice(b: &[Vec<u8>], blocked: &HashSet<Cell>, c: Cell) -> bool {
    matches!(cell_char(b, c), Some(ch) if ch.is_ascii_digit() && ch != b'0') && !blocked.contains(&c)
}

fn mobility(b: &[Vec<u8>], blocked: &HashSet<Cell>, from: Cell) -> i32 {
    let mut n = 0;
    for d in 0..6 {
        let mut c = step(from, d);
        while is_ice(b, blocked, c) { n += 1; c = step(c, d); }
    }
    n
}

fn parse_pos(pos: &str) -> Option<Cell> {
    let b = pos.as_bytes();
    if b.len() < 2 || !b[0].is_ascii_alphabetic() || !b[1..].iter().all(|c| c.is_ascii_digit()) { return None; }
    Some((pos[1..].parse::<i32>().ok()? - 1, (b[0].to_ascii_uppercase() - b'A') as i32))
}

fn fish_at(b: &[Vec<u8>], c: Cell) -> i32 {
    match cell_char(b, c) { Some(ch) if ch.is_ascii_digit() => (ch - b'0') as i32, _ => -1 }
}

fn score(board: &[Vec<u8>], mine: &HashMap<String, Cell>, occupied: &[Cell], action: &str) -> i32 {
    let parts: Vec<&str> = action.split_whitespace().collect();
    let dst = match parts.last().and_then(|p| parse_pos(p)) { Some(c) => c, None => return -1 };
    let mut b: Vec<Vec<u8>> = board.to_vec();
    let mut value = fish_at(&b, dst);
    let mut blocked: HashSet<Cell> = occupied.iter().copied().collect();
    if parts.len() >= 3 {
        if let Some(&src) = mine.get(parts[1]) {
            value += SOURCE_WEIGHT * fish_at(&b, src).max(0);
            blocked.remove(&src);
            if fish_at(&b, src) >= 0 { b[src.0 as usize][src.1 as usize] = b'0'; }
        }
    }
    blocked.remove(&dst);
    value + MOBILITY_WEIGHT * mobility(&b, &blocked, dst)
}

fn main() {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();
    let mut out = BufWriter::new(io::stdout());
    let players: usize = match lines.next() { Some(Ok(l)) => l.trim().parse().unwrap_or(2), _ => return };
    let me = match lines.next() { Some(Ok(l)) => l.trim().to_string(), _ => return };
    if lines.next().is_none() { return; }
    while let Some(Ok(l)) = lines.next() {
        let rows: usize = l.trim().parse().unwrap_or(0);
        let mut board: Vec<Vec<u8>> = Vec::new();
        for _ in 0..rows {
            match lines.next() { Some(Ok(r)) => board.push(r.trim().bytes().collect()), _ => return }
        }
        for _ in 0..players { if lines.next().is_none() { return; } }
        let count: usize = match lines.next() { Some(Ok(l)) => l.trim().parse().unwrap_or(0), _ => return };
        let mut mine: HashMap<String, Cell> = HashMap::new(); // penguin ids are unique per player only
        let mut occupied: Vec<Cell> = Vec::new();
        for _ in 0..count {
            let l = match lines.next() { Some(Ok(l)) => l, _ => return };
            let p: Vec<&str> = l.split_whitespace().collect();
            if p.len() >= 3 {
                if let Some(c) = parse_pos(p[2]) {
                    occupied.push(c);
                    if p[1] == me { mine.insert(p[0].to_string(), c); }
                }
            }
        }
        let actions: usize = match lines.next() { Some(Ok(l)) => l.trim().parse().unwrap_or(0), _ => return };
        let (mut best, mut best_v) = (String::new(), -2);
        for _ in 0..actions {
            let l = match lines.next() { Some(Ok(l)) => l, _ => return };
            let action = l.trim().to_string();
            let v = if action.is_empty() { -2 } else { score(&board, &mine, &occupied, &action) };
            if v > best_v { best_v = v; best = action; }
        }
        writeln!(out, "{}", if best.is_empty() { "MOVE 0 A1" } else { &best }).ok();
        out.flush().ok();
    }
}
