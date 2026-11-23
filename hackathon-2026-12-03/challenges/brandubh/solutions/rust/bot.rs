// Brandubh 7x7 — Rust port of solutions/python/bot.py (v9).
// Same move order, captures, tie-breaks and node budget semantics.
use std::collections::HashMap;
use std::io::{self, BufRead, BufWriter, Write};
use std::time::Instant;

const EMPTY: u8 = 0;
const ATT: u8 = 1;
const DEF: u8 = 2;
const KING: u8 = 3;
const SIDE_A: i32 = 0;
const SIDE_D: i32 = 1;
const THRONE: usize = 24;
const CORNER_DIAG: [usize; 4] = [8, 12, 36, 40];
const DX: [i32; 4] = [1, -1, 0, 0];
const DY: [i32; 4] = [0, 0, 1, -1];
const KDIST: [i32; 5] = [0, -600, -250, -80, 0];
const WIN: i32 = 1_000_000;
const INF: i32 = 1 << 30;
const MAX_DEPTH: i32 = 64;

#[derive(Clone, Copy, PartialEq, Eq)]
struct Move { from: usize, to: usize }

#[derive(Clone, Copy)]
struct State { cell: [u8; 49], side: i32 }

fn is_corner(i: usize) -> bool { i == 0 || i == 6 || i == 42 || i == 48 }
fn side_of(v: u8) -> i32 { if v == ATT { SIDE_A } else { SIDE_D } }
fn inb(x: i32, y: i32) -> bool { (0..7).contains(&x) && (0..7).contains(&y) }

fn gen_moves(s: &State) -> Vec<Move> {
    let mut out = Vec::with_capacity(64);
    for i in 0..49 {
        let v = s.cell[i];
        if v == EMPTY || side_of(v) != s.side { continue; }
        let (x0, y0) = ((i % 7) as i32, (i / 7) as i32);
        for d in 0..4 {
            let (mut x, mut y) = (x0 + DX[d], y0 + DY[d]);
            while inb(x, y) {
                let j = (y * 7 + x) as usize;
                if s.cell[j] != EMPTY { break; }
                if v == KING || !(j == THRONE || is_corner(j)) { out.push(Move { from: i, to: j }); }
                x += DX[d];
                y += DY[d];
            }
        }
    }
    out
}

fn hostile(c: &[u8; 49], x: i32, y: i32, cap_type: u8, cap_king: bool) -> bool {
    if !inb(x, y) { return false; }
    let j = (y * 7 + x) as usize;
    if is_corner(j) { return true; }
    let target_att = cap_type != ATT;
    if j == THRONE {
        if c[THRONE] == EMPTY { return !cap_king; }
        return target_att;
    }
    let p = c[j];
    if p != EMPTY { return if target_att { p != ATT } else { p == ATT }; }
    false
}

fn is_enemy(a: u8, b: u8) -> bool { if a == ATT { b != ATT } else { b == ATT } }

fn apply_move(s: &State, m: Move) -> State {
    let mut t = *s;
    let v = t.cell[m.from];
    t.cell[m.from] = EMPTY;
    t.cell[m.to] = v;
    let (ex, ey) = ((m.to % 7) as i32, (m.to / 7) as i32);
    for d in 0..4 {
        let (ax, ay) = (ex + DX[d], ey + DY[d]);
        if !inb(ax, ay) { continue; }
        let a = (ay * 7 + ax) as usize;
        if t.cell[a] == EMPTY || !is_enemy(v, t.cell[a]) { continue; }
        if t.cell[a] == KING && a == THRONE {
            let all = (0..4).all(|e| hostile(&t.cell, 3 + DX[e], 3 + DY[e], ATT, true));
            if all { t.cell[a] = EMPTY; }
        } else if hostile(&t.cell, ax + DX[d], ay + DY[d], v, t.cell[a] == KING) {
            t.cell[a] = EMPTY;
        }
    }
    t.side = 1 - s.side;
    t
}

fn encircled(c: &[u8; 49]) -> bool {
    let mut seen = [false; 49];
    let mut queue: Vec<usize> = Vec::with_capacity(49);
    for i in 0..49 {
        let (x, y) = (i % 7, i / 7);
        if x == 0 || x == 6 || y == 0 || y == 6 {
            if c[i] == KING || c[i] == DEF { return false; }
            if c[i] != ATT { seen[i] = true; queue.push(i); }
        }
    }
    let mut h = 0;
    while h < queue.len() {
        let i = queue[h];
        h += 1;
        if c[i] == KING || c[i] == DEF { return false; }
        let (x, y) = ((i % 7) as i32, (i / 7) as i32);
        for d in 0..4 {
            let (nx, ny) = (x + DX[d], y + DY[d]);
            if inb(nx, ny) {
                let j = (ny * 7 + nx) as usize;
                if !seen[j] && c[j] != ATT { seen[j] = true; queue.push(j); }
            }
        }
    }
    true
}

fn king_pos(c: &[u8; 49]) -> Option<usize> { (0..49).find(|&i| c[i] == KING) }

fn winner(s: &State) -> i32 {
    match king_pos(&s.cell) {
        None => SIDE_A,
        Some(k) if is_corner(k) => SIDE_D,
        Some(_) => if encircled(&s.cell) { SIDE_A } else { -1 },
    }
}

fn king_lines(c: &[u8; 49], k: usize) -> i32 {
    let mut n = 0;
    let (x0, y0) = ((k % 7) as i32, (k / 7) as i32);
    for d in 0..4 {
        let (mut x, mut y) = (x0 + DX[d], y0 + DY[d]);
        while inb(x, y) {
            let j = (y * 7 + x) as usize;
            if c[j] != EMPTY { break; }
            if is_corner(j) { n += 1; }
            x += DX[d];
            y += DY[d];
        }
    }
    n
}

fn king_dist(c: &[u8; 49], k: usize) -> usize {
    let mut dist = [9usize; 49];
    let mut queue: Vec<usize> = Vec::with_capacity(49);
    dist[k] = 0;
    queue.push(k);
    let mut h = 0;
    while h < queue.len() {
        let i = queue[h];
        h += 1;
        if is_corner(i) { return dist[i].min(4); }
        if dist[i] >= 3 { continue; }
        let (x0, y0) = ((i % 7) as i32, (i / 7) as i32);
        for d in 0..4 {
            let (mut x, mut y) = (x0 + DX[d], y0 + DY[d]);
            while inb(x, y) {
                let j = (y * 7 + x) as usize;
                if c[j] != EMPTY { break; }
                if dist[j] == 9 { dist[j] = dist[i] + 1; queue.push(j); }
                x += DX[d];
                y += DY[d];
            }
        }
    }
    4
}

fn evaluate(s: &State) -> i32 {
    let c = &s.cell;
    let k = king_pos(c).unwrap_or(THRONE);
    let a = c.iter().filter(|&&v| v == ATT).count() as i32;
    let d = c.iter().filter(|&&v| v == DEF).count() as i32;
    let mut sc = 100 * a - 150 * d;
    let (kx, ky) = ((k % 7) as i32, (k / 7) as i32);
    for e in 0..4 {
        let (x, y) = (kx + DX[e], ky + DY[e]);
        if inb(x, y) && c[(y * 7 + x) as usize] == ATT { sc += 60; }
    }
    for &i in CORNER_DIAG.iter() { if c[i] == ATT { sc += 60; } }
    sc += KDIST[king_dist(c, k)];
    if s.side == SIDE_A && king_lines(c, k) >= 2 { sc -= 3000; }
    if s.side == SIDE_A { sc } else { -sc }
}

fn order_key(s: &State, m: &Move) -> i32 {
    let v = s.cell[m.from];
    if v == KING { return if is_corner(m.to) { 10 } else { 1 }; }
    if v == ATT {
        let k = king_pos(&s.cell).unwrap_or(THRONE) as i32;
        let (t, kk) = (m.to as i32, k);
        let dx = (t % 7 - kk % 7).abs();
        let dy = (t / 7 - kk / 7).abs();
        return if dx + dy == 1 { 2 } else { 0 };
    }
    0
}

struct Search { nodes: i64, limit: i64, time_ms: u128, aborted: bool, depth: i32, t0: Instant }

impl Search {
    fn negamax(&mut self, s: &State, depth: i32, mut alpha: i32, beta: i32, ply: i32) -> i32 {
        self.nodes += 1;
        if self.nodes > self.limit { self.aborted = true; return 0; }
        if self.nodes & 255 == 0 && self.t0.elapsed().as_millis() > self.time_ms {
            eprintln!("TIMEGUARD"); self.aborted = true; return 0;
        }
        let w = winner(s);
        if w >= 0 { return if w == s.side { WIN - ply } else { -(WIN - ply) }; }
        if s.side == SIDE_D {
            if let Some(k) = king_pos(&s.cell) {
                if king_lines(&s.cell, k) != 0 { return WIN - (ply + 1); }
            }
        }
        if depth == 0 { return evaluate(s); }
        let mut mv = gen_moves(s);
        if mv.is_empty() { return -(WIN - ply); }
        mv.sort_by_key(|m| -order_key(s, m));
        let mut best = -INF;
        for m in mv {
            let v = -self.negamax(&apply_move(s, m), depth - 1, -beta, -alpha, ply + 1);
            if self.aborted { return 0; }
            if v > best { best = v; }
            if v > alpha { alpha = v; }
            if alpha >= beta { break; }
        }
        best
    }

    fn run(&mut self, root: &State, mut mv: Vec<Move>) -> Move {
        mv.sort_by_key(|m| -order_key(root, m));
        let mut best = mv[0];
        self.nodes = 0;
        self.aborted = false;
        self.depth = 0;
        for depth in 1..=MAX_DEPTH {
            let (mut alpha, mut best_v, mut cur, mut cur_idx) = (-INF, -INF, mv[0], 0usize);
            for (i, &m) in mv.iter().enumerate() {
                let v = -self.negamax(&apply_move(root, m), depth - 1, -INF, -alpha, 1);
                if self.aborted { break; }
                if v > best_v { best_v = v; cur = m; cur_idx = i; }
                if v > alpha { alpha = v; }
            }
            if self.aborted { break; }
            best = cur;
            self.depth = depth;
            let pv = mv.remove(cur_idx);
            mv.insert(0, pv);
            if best_v >= WIN - MAX_DEPTH || best_v <= -(WIN - MAX_DEPTH) { break; }
        }
        best
    }
}

fn key_of(s: &State) -> String {
    let mut k: String = s.cell.iter().map(|&v| b".ADK"[v as usize] as char).collect();
    k.push(if s.side == SIDE_A { '0' } else { '1' });
    k
}

fn name_of(m: Move) -> String {
    format!("{}{}{}{}", (b'a' + (m.from % 7) as u8) as char, (b'7' - (m.from / 7) as u8) as char,
            (b'a' + (m.to % 7) as u8) as char, (b'7' - (m.to / 7) as u8) as char)
}

fn main() {
    let cli: Option<i64> = std::env::var("CG_NODES").ok().and_then(|v| v.parse().ok());
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();
    let mut out = BufWriter::new(io::stdout());
    let mut history: HashMap<String, i32> = HashMap::new();
    let mut first = true;
    while let Some(Ok(line)) = lines.next() {
        let t0 = Instant::now();
        let role: i32 = match line.split_whitespace().next().and_then(|t| t.parse().ok()) { Some(r) => r, None => break };
        if lines.next().is_none() { break; }
        let mut cell = [EMPTY; 49];
        for y in 0..7 {
            let row = match lines.next() { Some(Ok(r)) => r, _ => String::new() };
            let b = row.as_bytes();
            for x in 0..7 {
                let ch = if x < b.len() { b[x] } else { b'.' };
                cell[y * 7 + x] = match ch { b'A' => ATT, b'D' => DEF, b'K' => KING, _ => EMPTY };
            }
        }
        let s = State { cell, side: if role == 0 { SIDE_A } else { SIDE_D } };
        let limit = cli.unwrap_or(if first { 200_000 } else { 12_000 });
        let time_ms = if first { 700 } else { 35 };
        first = false;
        *history.entry(key_of(&s)).or_insert(0) += 1;
        let all = gen_moves(&s);
        let safe: Vec<Move> = all.iter().copied()
            .filter(|&m| *history.get(&key_of(&apply_move(&s, m))).unwrap_or(&0) < 2).collect();
        let pool = if safe.is_empty() { all } else { safe };
        if pool.is_empty() {
            writeln!(out, "a1a1").ok();
            out.flush().ok();
            continue;
        }
        let mut se = Search { nodes: 0, limit, time_ms, aborted: false, depth: 0, t0 };
        let m = se.run(&s, pool);
        *history.entry(key_of(&apply_move(&s, m))).or_insert(0) += 1;
        eprintln!("d={} n={} ms={}", se.depth, se.nodes, t0.elapsed().as_millis());
        writeln!(out, "{}", name_of(m)).ok();
        out.flush().ok();
    }
}
