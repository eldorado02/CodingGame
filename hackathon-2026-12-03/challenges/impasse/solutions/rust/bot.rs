// Impasse — Rust port of solutions/python/bot.py (v1 greedy).
use std::io::{self, BufRead, BufWriter, Write};

fn parse_coords(t: &str) -> Option<Vec<(usize, i32)>> {
    let b = t.as_bytes();
    if b.is_empty() || b.len() % 2 != 0 { return None; }
    let mut out = Vec::new();
    for ch in b.chunks(2) {
        if !(b'a'..=b'h').contains(&ch[0]) || !(b'1'..=b'8').contains(&ch[1]) { return None; }
        out.push(((ch[0] - b'a') as usize, (ch[1] - b'0') as i32));
    }
    Some(out)
}

fn score(board: &[Vec<u8>], me: u8, co: &[(usize, i32)]) -> i32 {
    if co.len() == 1 { return 50; }
    let up = me == b'w';
    let (near, far) = if up { (1, 8) } else { (8, 1) };
    let ((c1, r1), (c2, r2)) = (co[0], co[1]);
    let src = board[(8 - r1) as usize][c1];
    let dst = board[(8 - r2) as usize][c2];
    let me_u = if up { b'W' } else { b'B' };
    let rows = (r2 - r1).abs();
    let mut sc = if src == me_u {
        if dst == me { 12 + if r2 == near { 20 } else { 0 } } else { 3 * rows + if r2 == near { 25 } else { 0 } }
    } else {
        3 * rows + if r2 == far { 8 } else { 0 }
    };
    if co.len() >= 3 { sc += 2; }
    sc
}

fn main() {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();
    let mut out = BufWriter::new(io::stdout());
    let first = match lines.next() { Some(Ok(l)) => l, _ => return };
    let me = match first.trim().bytes().next() { Some(b'b') | Some(b'B') => b'b', _ => b'w' };
    loop {
        let mut board: Vec<Vec<u8>> = Vec::new();
        for _ in 0..8 {
            let r = match lines.next() { Some(Ok(r)) => r, _ => return };
            let mut row: Vec<u8> = r.trim_end_matches('\r').bytes().collect();
            row.resize(8.max(row.len()), b'.');
            row.truncate(8);
            board.push(row);
        }
        if lines.next().is_none() { return; }
        let n: usize = match lines.next() { Some(Ok(l)) => l.trim().parse().unwrap_or(0), _ => return };
        let (mut best, mut best_v) = (String::new(), -1);
        for _ in 0..n {
            let l = match lines.next() { Some(Ok(l)) => l, _ => return };
            if let Some(t) = l.split_whitespace().next() {
                if let Some(co) = parse_coords(t) {
                    let v = score(&board, me, &co);
                    if v > best_v { best_v = v; best = t.to_string(); }
                }
            }
        }
        writeln!(out, "{}", if best.is_empty() { "random" } else { &best }).ok();
        out.flush().ok();
    }
}
