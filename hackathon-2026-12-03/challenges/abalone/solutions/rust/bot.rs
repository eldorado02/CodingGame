// Abalone — Rust port of cpp/bot.cpp (same simulation, same score, same tie-break).
// Build: cargo build --release
use std::io::{self, BufRead, Write};

const RADIUS: i32 = 4;
const ROWS: usize = 9;
const DIRECTIONS: [(i32, i32); 6] = [(1, 0), (1, -1), (0, -1), (-1, 0), (-1, 1), (0, 1)];
const CAPTURE_WEIGHT: i64 = 10000;
const CENTER_WEIGHT: i64 = 10;
const EDGE_WEIGHT: i64 = 5;
const COHESION_WEIGHT: i64 = 2;
const EMPTY: u8 = b'0';

type Board = [[u8; ROWS]; ROWS]; // [q + RADIUS][r + RADIUS]
type Hex = (i32, i32);

fn q_min(r: i32) -> i32 { (-RADIUS).max(-r - RADIUS) }
fn q_max(r: i32) -> i32 { RADIUS.min(-r + RADIUS) }
fn to_axial(x: i32, y: i32) -> Hex { let r = y - RADIUS; (x + q_min(r), r) }
fn on_board(h: Hex) -> bool { h.1 >= -RADIUS && h.1 <= RADIUS && h.0 >= q_min(h.1) && h.0 <= q_max(h.1) }
fn distance(q: i32, r: i32) -> i32 { (q.abs() + r.abs() + (q + r).abs()) / 2 }
fn get(b: &Board, h: Hex) -> u8 { b[(h.0 + RADIUS) as usize][(h.1 + RADIUS) as usize] }
fn set(b: &mut Board, h: Hex, v: u8) { b[(h.0 + RADIUS) as usize][(h.1 + RADIUS) as usize] = v; }

/// Plays our move on `b`; returns the enemy marbles pushed off.
fn play(b: &mut Board, me: u8, m: &[i32; 5]) -> i64 {
    let (first, last) = (to_axial(m[0], m[1]), to_axial(m[2], m[3]));
    let steps = distance(last.0 - first.0, last.1 - first.1);
    let axis = ((last.0 - first.0).signum(), (last.1 - first.1).signum());
    let line: Vec<Hex> = (0..=steps).map(|k| (first.0 + k * axis.0, first.1 + k * axis.1)).filter(|&h| on_board(h)).collect();
    if line.is_empty() { return 0; }
    let (dq, dr) = DIRECTIONS[m[4].rem_euclid(6) as usize];
    let in_line = steps == 0 || (dq, dr) == axis || (dq, dr) == (-axis.0, -axis.1);
    let before = *b;
    let mut captured = 0;
    if in_line {
        let mut head = line[0];
        for &h in &line[1..] {
            if h.0 * dq + h.1 * dr > head.0 * dq + head.1 * dr { head = h; }
        }
        let mut chain = Vec::new();
        let mut cursor = (head.0 + dq, head.1 + dr);
        while on_board(cursor) && get(&before, cursor) != EMPTY && get(&before, cursor) != me {
            chain.push(cursor);
            cursor = (cursor.0 + dq, cursor.1 + dr);
        }
        for &c in chain.iter().rev() {
            let target = (c.0 + dq, c.1 + dr);
            if on_board(target) { set(b, target, get(&before, c)); } else { captured += 1; }
            set(b, c, EMPTY);
        }
    }
    for &h in &line { set(b, h, EMPTY); }
    for &h in &line {
        let target = (h.0 + dq, h.1 + dr);
        if on_board(target) { set(b, target, me); }
    }
    captured
}

fn evaluate(b: &Board, me: u8) -> i64 {
    let mut score = 0;
    for r in -RADIUS..=RADIUS {
        for q in q_min(r)..=q_max(r) {
            let v = get(b, (q, r));
            if v == me {
                score -= CENTER_WEIGHT * distance(q, r) as i64;
                for &(dq, dr) in &DIRECTIONS {
                    let n = (q + dq, r + dr);
                    if on_board(n) && get(b, n) == me { score += COHESION_WEIGHT; }
                }
            } else if v != EMPTY {
                score += EDGE_WEIGHT * distance(q, r) as i64;
            }
        }
    }
    score
}

fn main() {
    let stdin = io::stdin();
    let mut words = stdin.lock().lines().map_while(Result::ok).flat_map(|l| l.split_whitespace().map(str::to_string).collect::<Vec<_>>());
    let Some(me) = words.next() else { return };
    let me = me.as_bytes()[0];
    let stdout = io::stdout();
    let mut out = stdout.lock();
    let mut next = move || words.next();
    let int = |w: Option<String>| w.and_then(|t| t.parse::<i32>().ok());
    loop {
        let (Some(_mine), Some(_theirs)) = (int(next()), int(next())) else { return };
        let mut board: Board = [[EMPTY; ROWS]; ROWS];
        for y in 0..ROWS as i32 {
            let row = next().unwrap_or_default(); // a word: keeps the leading zeros
            let r = y - RADIUS;
            for x in 0..q_max(r) - q_min(r) + 1 {
                let v = row.as_bytes().get(x as usize).copied().unwrap_or(EMPTY);
                set(&mut board, to_axial(x, y), v);
            }
        }
        for _ in 0..5 { next(); }
        let Some(count) = int(next()) else { return };
        let mut best: Option<([i32; 5], i64)> = None;
        for _ in 0..count.max(0) {
            let mut m = [0i32; 5];
            for v in m.iter_mut() { *v = int(next()).unwrap_or(0); }
            let mut after = board;
            let score = CAPTURE_WEIGHT * play(&mut after, me, &m) + evaluate(&after, me);
            if best.map_or(true, |(_, s)| score > s) { best = Some((m, score)); }
        }
        let text = best.map_or("0 0 0 0 0".to_string(), |(m, _)| format!("{} {} {} {} {}", m[0], m[1], m[2], m[3], m[4]));
        writeln!(out, "{}", text).unwrap();
        out.flush().unwrap();
    }
}
