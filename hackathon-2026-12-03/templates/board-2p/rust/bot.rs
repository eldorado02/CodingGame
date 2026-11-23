// TEMPLATE board-2p — Rust port of cpp/bot.cpp (same engine, same tie-breaks).
// Replace the "GAME:" blocks only. Build: cargo build --release
use std::io::{self, BufRead, BufWriter, Write};
use std::time::Instant;

// ------------------------------------------------------------ GAME: state
const LINES: [[usize; 3]; 8] = [[0,1,2],[3,4,5],[6,7,8],[0,3,6],[1,4,7],[2,5,8],[0,4,8],[2,4,6]];
const WEIGHT: [i32; 9] = [3, 2, 3, 2, 4, 2, 3, 2, 3];

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
struct Move { r: i32, c: i32 }

#[derive(Clone, Copy)]
struct State { cell: [i32; 9], to_move: i32 }

fn winner(s: &State) -> i32 {
    for l in LINES.iter() {
        let a = s.cell[l[0]];
        if a != 0 && a == s.cell[l[1]] && a == s.cell[l[2]] { return a; }
    }
    if s.cell.iter().any(|&v| v == 0) { 0 } else { 3 }
}

fn gen_moves(s: &State) -> Vec<Move> {
    (0..9).filter(|&i| s.cell[i] == 0).map(|i| Move { r: (i / 3) as i32, c: (i % 3) as i32 }).collect()
}

fn apply_move(s: &State, m: Move) -> State {
    let mut t = *s;
    t.cell[(m.r * 3 + m.c) as usize] = s.to_move;
    t.to_move = 3 - s.to_move;
    t
}

fn evaluate(s: &State) -> i32 {
    let mut sc = 0;
    for i in 0..9 {
        if s.cell[i] == s.to_move { sc += WEIGHT[i]; } else if s.cell[i] != 0 { sc -= WEIGHT[i]; }
    }
    sc
}

fn order_key(_s: &State, m: &Move) -> i32 { if m.r == 1 && m.c == 1 { 1 } else { 0 } }

// ------------------------------------------------------------ ENGINE
const WIN: i32 = 1_000_000;
const INF: i32 = 1 << 30;
const MAX_DEPTH: i32 = 64;
const TIME_MS: u128 = 80;

struct Search { nodes: i64, limit: i64, aborted: bool, t0: Instant }

impl Search {
    fn negamax(&mut self, s: &State, depth: i32, mut alpha: i32, beta: i32, ply: i32) -> i32 {
        self.nodes += 1;
        if self.nodes > self.limit { self.aborted = true; return 0; }
        if self.nodes & 1023 == 0 && self.t0.elapsed().as_millis() > TIME_MS {
            eprintln!("TIMEGUARD"); self.aborted = true; return 0;
        }
        let w = winner(s);
        if w == 3 { return 0; }
        if w != 0 { return if w == s.to_move { WIN - ply } else { -(WIN - ply) }; }
        if depth == 0 { return evaluate(s); }
        let mut mv = gen_moves(s);
        if mv.is_empty() { return evaluate(s); }
        mv.sort_by_key(|m| -order_key(s, m)); // sort_by_key is stable
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

    fn run(&mut self, root: &State) -> Move {
        let mut mv = gen_moves(root);
        mv.sort_by_key(|m| -order_key(root, m));
        let mut best = mv[0];
        self.nodes = 0;
        self.aborted = false;
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
            let pv = mv.remove(cur_idx);
            mv.insert(0, pv);
            if best_v >= WIN - MAX_DEPTH || best_v <= -(WIN - MAX_DEPTH) { break; }
            if depth >= 9 { break; }
        }
        best
    }
}

// ------------------------------------------------------------ GAME: I/O
fn main() {
    let limit: i64 = std::env::var("CG_NODES").ok().and_then(|v| v.parse().ok()).unwrap_or(200_000);
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();
    let mut out = BufWriter::new(io::stdout());
    let mut s = State { cell: [0; 9], to_move: 1 };
    while let Some(Ok(line)) = lines.next() {
        let t0 = Instant::now();
        let p: Vec<i32> = line.split_whitespace().filter_map(|x| x.parse().ok()).collect();
        if p.len() < 2 { break; }
        if (0..3).contains(&p[0]) && (0..3).contains(&p[1]) { s.cell[(p[0] * 3 + p[1]) as usize] = 2; }
        let k: usize = match lines.next() { Some(Ok(l)) => l.trim().parse().unwrap_or(0), _ => break };
        for _ in 0..k { if lines.next().is_none() { return; } }
        s.to_move = 1;
        let mut se = Search { nodes: 0, limit, aborted: false, t0 };
        let m = se.run(&s);
        s.cell[(m.r * 3 + m.c) as usize] = 1;
        writeln!(out, "{} {}", m.r, m.c).ok();
        out.flush().ok();
    }
}
