// Lines of Action 8x8 — Rust port of solutions/python/bot.py (v1 alpha-beta).
// Index = y * 8 + x, y = 0 is the first input line (rank 8).
use std::io::{self, BufRead, BufWriter, Write};
use std::time::Instant;

const N: i32 = 8;
const CELLS: usize = 64;
const FIRST_TURN_NODES: i64 = 100_000;
const TURN_NODES: i64 = 20_000;
const FIRST_TURN_MS: u128 = 700;
const TURN_MS: u128 = 100;
const GROUP_WEIGHT: i32 = 10;
const DIRS: [(i32, i32); 8] = [(0, -1), (0, 1), (-1, 0), (1, 0), (-1, -1), (1, 1), (-1, 1), (1, -1)];
const EMPTY: u8 = b'.';
const WIN: i32 = 1_000_000;
const INF: i32 = 1 << 30;
const MAX_DEPTH: i32 = 64;

#[derive(Clone, Copy)]
struct Move { src: usize, dst: usize }

#[derive(Clone, Copy)]
struct State { cell: [u8; CELLS], me: u8, op: u8 }

fn inb(x: i32, y: i32) -> bool { (0..N).contains(&x) && (0..N).contains(&y) }

fn line_count(c: &[u8; CELLS], x: i32, y: i32, dx: i32, dy: i32) -> i32 {
    let mut n = 1; // the moving checker itself
    for sgn in [1, -1] {
        let (mut cx, mut cy) = (x + sgn * dx, y + sgn * dy);
        while inb(cx, cy) {
            if c[(cy * N + cx) as usize] != EMPTY { n += 1; }
            cx += sgn * dx;
            cy += sgn * dy;
        }
    }
    n
}

fn gen_moves(s: &State) -> Vec<Move> {
    let mut out = Vec::new();
    for i in 0..CELLS {
        if s.cell[i] != s.me { continue; }
        let (x, y) = (i as i32 % N, i as i32 / N);
        for &(dx, dy) in DIRS.iter() {
            let k = line_count(&s.cell, x, y, dx, dy);
            let (tx, ty) = (x + k * dx, y + k * dy);
            if !inb(tx, ty) { continue; }
            let t = (ty * N + tx) as usize;
            if s.cell[t] == s.me { continue; }
            let blocked = (1..k).any(|j| s.cell[((y + j * dy) * N + x + j * dx) as usize] == s.op);
            if !blocked { out.push(Move { src: i, dst: t }); }
        }
    }
    out
}

fn apply_move(s: &State, m: &Move) -> State {
    let mut t = *s;
    t.cell[m.dst] = s.cell[m.src];
    t.cell[m.src] = EMPTY;
    t.me = s.op;
    t.op = s.me;
    t
}

fn groups(c: &[u8; CELLS], color: u8) -> i32 {
    let mut seen = [false; CELLS];
    let mut count = 0;
    for i in 0..CELLS {
        if c[i] != color || seen[i] { continue; }
        count += 1;
        let mut stack = vec![i];
        seen[i] = true;
        while let Some(j) = stack.pop() {
            let (x, y) = (j as i32 % N, j as i32 / N);
            for dx in -1..=1 {
                for dy in -1..=1 {
                    let (nx, ny) = (x + dx, y + dy);
                    if !inb(nx, ny) { continue; }
                    let k = (ny * N + nx) as usize;
                    if !seen[k] && c[k] == color { seen[k] = true; stack.push(k); }
                }
            }
        }
    }
    count
}

/// s is the position after a move by s.op: the mover wins ties.
fn winner_after_move(s: &State) -> Option<u8> {
    if groups(&s.cell, s.op) <= 1 { return Some(s.op); }
    if groups(&s.cell, s.me) <= 1 { return Some(s.me); }
    None
}

fn side_score(c: &[u8; CELLS], color: u8) -> i32 {
    let cells: Vec<i32> = (0..CELLS as i32).filter(|&i| c[i as usize] == color).collect();
    if cells.is_empty() { return 0; }
    let n = cells.len() as i32;
    let cx = cells.iter().map(|i| i % N).sum::<i32>() / n;
    let cy = cells.iter().map(|i| i / N).sum::<i32>() / n;
    let spread: i32 = cells.iter().map(|i| (i % N - cx).abs().max((i / N - cy).abs())).sum();
    -spread - GROUP_WEIGHT * (groups(c, color) - 1)
}

fn evaluate(s: &State) -> i32 { side_score(&s.cell, s.me) - side_score(&s.cell, s.op) }

fn order_key(s: &State, m: &Move) -> i32 { if s.cell[m.dst] == s.op { 1 } else { 0 } }

struct Search { nodes: i64, limit: i64, time_ms: u128, aborted: bool, depth: i32, t0: Instant }

impl Search {
    fn negamax(&mut self, s: &State, depth: i32, mut alpha: i32, beta: i32, ply: i32) -> i32 {
        self.nodes += 1;
        if self.nodes > self.limit { self.aborted = true; return 0; }
        if self.nodes & 255 == 0 && self.t0.elapsed().as_millis() > self.time_ms {
            eprintln!("TIMEGUARD"); self.aborted = true; return 0;
        }
        if let Some(w) = winner_after_move(s) { return if w == s.me { WIN - ply } else { -(WIN - ply) }; }
        if depth == 0 { return evaluate(s); }
        let mut mv = gen_moves(s);
        if mv.is_empty() {
            let passed = State { cell: s.cell, me: s.op, op: s.me };
            return -self.negamax(&passed, depth - 1, -beta, -alpha, ply + 1);
        }
        mv.sort_by_key(|m| -order_key(s, m)); // stable
        let mut best = -INF;
        for m in mv.iter() {
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
        for depth in 1..=MAX_DEPTH {
            let (mut alpha, mut best_v, mut cur_idx) = (-INF, -INF, 0usize);
            for (i, m) in mv.iter().enumerate() {
                let v = -self.negamax(&apply_move(root, m), depth - 1, -INF, -alpha, 1);
                if self.aborted { break; }
                if v > best_v { best_v = v; cur_idx = i; }
                if v > alpha { alpha = v; }
            }
            if self.aborted { break; }
            best = mv[cur_idx];
            self.depth = depth;
            let pv = mv.remove(cur_idx); // PV first next iteration
            mv.insert(0, pv);
            if best_v >= WIN - MAX_DEPTH || best_v <= -(WIN - MAX_DEPTH) { break; }
        }
        best
    }
}

fn square(t: &[u8]) -> Option<usize> {
    if !(b'a'..=b'h').contains(&t[0]) || !(b'1'..=b'8').contains(&t[1]) { return None; }
    Some(((N - (t[1] - b'0') as i32) * N + (t[0] - b'a') as i32) as usize)
}

fn name_of(i: usize) -> String { format!("{}{}", (b'a' + (i % 8) as u8) as char, 8 - i / 8) }

fn main() {
    let cli: Option<i64> = std::env::var("CG_NODES").ok().and_then(|v| v.parse().ok());
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();
    let mut out = BufWriter::new(io::stdout());
    let me = match lines.next() { Some(Ok(l)) => if l.trim().starts_with('w') { b'w' } else { b'b' }, _ => return };
    let op = if me == b'b' { b'w' } else { b'b' };
    let mut first = true;
    loop {
        let mut cell = [EMPTY; CELLS];
        let mut t0 = Instant::now();
        for y in 0..8 {
            let row = match lines.next() { Some(Ok(r)) => r, _ => return };
            if y == 0 { t0 = Instant::now(); }
            for (x, ch) in row.trim_end_matches('\r').bytes().take(8).enumerate() { cell[y * 8 + x] = ch; }
        }
        if lines.next().is_none() { return; }
        let n: usize = match lines.next() { Some(Ok(l)) => l.trim().parse().unwrap_or(0), _ => return };
        let mut legal = Vec::new();
        for _ in 0..n {
            let l = match lines.next() { Some(Ok(l)) => l, _ => return };
            if let Some(t) = l.split_whitespace().next() {
                let b = t.as_bytes();
                if b.len() == 4 {
                    if let (Some(a), Some(d)) = (square(&b[0..2]), square(&b[2..4])) { legal.push(Move { src: a, dst: d }); }
                }
            }
        }
        let limit = cli.unwrap_or(if first { FIRST_TURN_NODES } else { TURN_NODES });
        let time_ms = if first { FIRST_TURN_MS } else { TURN_MS };
        first = false;
        if legal.is_empty() {
            writeln!(out, "pass").ok();
            out.flush().ok();
            continue;
        }
        let mut search = Search { nodes: 0, limit, time_ms, aborted: false, depth: 0, t0 };
        let m = search.run(&State { cell, me, op }, legal);
        eprintln!("d={} n={} ms={}", search.depth, search.nodes, t0.elapsed().as_millis());
        writeln!(out, "{}{}", name_of(m.src), name_of(m.dst)).ok();
        out.flush().ok();
    }
}
