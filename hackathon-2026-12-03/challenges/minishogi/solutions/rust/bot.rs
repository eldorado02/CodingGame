// Minishogi 5x5 — Rust port of solutions/python/bot.py (v1 greedy).
use std::io::{self, BufRead, BufWriter, Write};

const DICE: usize = 0;
const ORTH: [(i32, i32); 4] = [(0, -1), (0, 1), (-1, 0), (1, 0)];
const DIAG: [(i32, i32); 4] = [(-1, -1), (1, -1), (-1, 1), (1, 1)];
const GOLD: [(i32, i32); 6] = [(0, -1), (-1, -1), (1, -1), (-1, 0), (1, 0), (0, 1)];
const SILVER: [(i32, i32); 5] = [(0, -1), (-1, -1), (1, -1), (-1, 1), (1, 1)];

#[derive(Clone, Copy)]
struct Cell { k: u8, prom: bool, own: i32 }

fn val(k: u8, prom: bool) -> i32 {
    if prom { return match k { b'R' => 11, b'B' => 10, b'S' | b'P' => 6, _ => 0 }; }
    match k { b'R' => 9, b'B' => 8, b'G' => 6, b'S' => 5, b'P' => 1, _ => 0 }
}

fn parse_row(s: &str) -> Vec<Cell> {
    let b = s.as_bytes();
    let mut out = Vec::new();
    let mut i = 0;
    while i < b.len() && out.len() < 5 {
        let (mut prom, mut ch) = (false, b[i]);
        if ch == b'+' && i + 1 < b.len() { prom = true; ch = b[i + 1]; i += 2; } else { i += 1; }
        if ch == b'.' { out.push(Cell { k: b'.', prom: false, own: -1 }); }
        else { out.push(Cell { k: ch.to_ascii_uppercase(), prom, own: if ch.is_ascii_uppercase() { 0 } else { 1 } }); }
    }
    while out.len() < 5 { out.push(Cell { k: b'.', prom: false, own: -1 }); }
    out
}

fn attacks(b: &[Vec<Cell>], side: i32) -> [[bool; 5]; 5] {
    let mut att = [[false; 5]; 5];
    let fwd = if side == 0 { -1 } else { 1 };
    for y in 0..5 {
        for x in 0..5 {
            let c = b[y][x];
            if c.own != side { continue; }
            let mut steps: Vec<(i32, i32)> = Vec::new();
            let mut slides: Vec<(i32, i32)> = Vec::new();
            match (c.k, c.prom) {
                (b'K', _) => { steps.extend_from_slice(&ORTH); steps.extend_from_slice(&DIAG); }
                (b'G', _) | (b'S', true) | (b'P', true) => steps.extend_from_slice(&GOLD),
                (b'S', false) => steps.extend_from_slice(&SILVER),
                (b'P', false) => steps.push((0, -1)),
                (b'R', p) => { slides.extend_from_slice(&ORTH); if p { steps.extend_from_slice(&DIAG); } }
                (b'B', p) => { slides.extend_from_slice(&DIAG); if p { steps.extend_from_slice(&ORTH); } }
                _ => {}
            }
            for (dx, dy) in steps {
                let (nx, ny) = (x as i32 + dx, y as i32 + dy * fwd);
                if (0..5).contains(&nx) && (0..5).contains(&ny) { att[ny as usize][nx as usize] = true; }
            }
            for (dx, dy) in slides {
                let (mut nx, mut ny) = (x as i32 + dx, y as i32 + dy);
                while (0..5).contains(&nx) && (0..5).contains(&ny) {
                    att[ny as usize][nx as usize] = true;
                    if b[ny as usize][nx as usize].k != b'.' { break; }
                    nx += dx;
                    ny += dy;
                }
            }
        }
    }
    att
}

fn sq(c: u8, r: u8) -> Option<(usize, usize)> {
    if !(b'1'..=b'5').contains(&c) || !(b'1'..=b'5').contains(&r) { return None; }
    Some((5 - (c - b'0') as usize, (r - b'1') as usize))
}

fn score(b: &[Vec<Cell>], me: i32, att: &[[bool; 5]; 5], a: &str) -> Option<i32> {
    let s = a.as_bytes();
    if s.len() >= 4 && s[1] == b'*' {
        let (x, y) = sq(s[2], s[3])?;
        return Some(if att[y][x] { -10 * val(s[0].to_ascii_uppercase(), false) } else { 0 });
    }
    if s.len() < 4 { return None; }
    let (sx, sy) = sq(s[0], s[1])?;
    let (x, y) = sq(s[2], s[3])?;
    let (mover, target) = (b[sy][sx], b[y][x]);
    let mut sc = 0;
    if target.k != b'.' && target.own != me { sc += 10 * val(target.k, target.prom); }
    if s[s.len() - 1] == b'+' { sc += 4; }
    if mover.k == b'K' && !mover.prom { sc -= 1; } else if att[y][x] { sc -= 10 * val(mover.k, mover.prom); }
    Some(sc)
}

fn main() {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();
    let mut out = BufWriter::new(io::stdout());
    let me = match lines.next() { Some(Ok(l)) => if l.trim() == "1" { 1 } else { 0 }, _ => return };
    let n: usize = match lines.next() { Some(Ok(l)) => l.trim().parse().unwrap_or(5).min(5), _ => return };
    loop {
        let mut b: Vec<Vec<Cell>> = Vec::new();
        for _ in 0..n {
            let r = match lines.next() { Some(Ok(r)) => r, _ => return };
            b.push(parse_row(r.trim()));
        }
        for _ in 0..3 + DICE { if lines.next().is_none() { return; } }
        let c: usize = match lines.next() { Some(Ok(l)) => l.trim().parse().unwrap_or(0), _ => return };
        let att = attacks(&b, 1 - me);
        let mut best: Option<(String, i32)> = None;
        for _ in 0..c {
            let l = match lines.next() { Some(Ok(l)) => l, _ => return };
            if let Some(t) = l.split_whitespace().next() {
                if let Some(v) = score(&b, me, &att, t) {
                    if best.as_ref().map_or(true, |(_, bv)| v > *bv) { best = Some((t.to_string(), v)); }
                }
            }
        }
        writeln!(out, "{}", best.map(|(s, _)| s).unwrap_or_else(|| "random".to_string())).ok();
        out.flush().ok();
    }
}
