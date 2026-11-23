// Counting TicTacToe 10x10 — Rust port of solutions/python/bot.py (v2).
// Score = sliding windows of 3 same-colour cells; incremental pattern score;
// selective alpha-beta (inner nodes keep the BEAM best moves by immediate gain).
use std::io::{self, BufWriter, Read, Write};
use std::time::Instant;

const N: usize = 10;
const CELLS: usize = N * N;
const TURN_NODES: i64 = 2500;
const TURN_MS: u128 = 70;
const BEAM: usize = 10;
const WEIGHTS: [i32; 4] = [0, 1, 10, 100]; // window value by own stones when unopposed
const MINE: u8 = 1;
const THEIRS: u8 = 2;
const WIN: i32 = 1_000_000;
const INF: i32 = 1 << 30;
const MAX_DEPTH: i32 = 64;

#[derive(Clone, Copy)]
struct State { cell: [u8; CELLS], me: u8, score: i32 } // score for player 1

struct Windows { list: Vec<[usize; 3]>, of: Vec<Vec<usize>> }

fn build_windows() -> Windows {
    let mut w = Windows { list: Vec::new(), of: vec![Vec::new(); CELLS] };
    for r in 0..N as i32 {
        for c in 0..N as i32 {
            for (dr, dc) in [(1, 0), (0, 1), (1, 1), (1, -1)] {
                let (r2, c2) = (r + 2 * dr, c + 2 * dc);
                if !(0..N as i32).contains(&r2) || !(0..N as i32).contains(&c2) { continue; }
                let cells = [(r * 10 + c) as usize, ((r + dr) * 10 + c + dc) as usize, (r2 * 10 + c2) as usize];
                let k = w.list.len();
                for &x in cells.iter() { w.of[x].push(k); }
                w.list.push(cells);
            }
        }
    }
    w
}

impl Windows {
    fn value(&self, c: &[u8; CELLS], k: usize) -> i32 {
        let w = self.list[k];
        let ones = w.iter().filter(|&&i| c[i] == 1).count();
        let twos = w.iter().filter(|&&i| c[i] == 2).count();
        if twos == 0 { WEIGHTS[ones] } else if ones == 0 { -WEIGHTS[twos] } else { 0 }
    }

    fn full_score(&self, c: &[u8; CELLS]) -> i32 { (0..self.list.len()).map(|k| self.value(c, k)).sum() }

    /// Only the windows through cell i change: update the score by delta.
    fn apply_move(&self, s: &State, i: usize) -> State {
        let mut t = *s;
        let before: i32 = self.of[i].iter().map(|&k| self.value(&t.cell, k)).sum();
        t.cell[i] = s.me;
        let after: i32 = self.of[i].iter().map(|&k| self.value(&t.cell, k)).sum();
        t.score = s.score + after - before;
        t.me = 3 - s.me;
        t
    }

    fn final_triples(&self, s: &State) -> i32 {
        let mut diff = 0;
        for w in self.list.iter() {
            let a = s.cell[w[0]];
            if a != 0 && a == s.cell[w[1]] && a == s.cell[w[2]] { diff += if a == s.me { 1 } else { -1 }; }
        }
        diff
    }

    /// Moves sorted by the mover's immediate gain (stable), at most `limit`.
    fn ordered_moves(&self, s: &State, limit: usize) -> Vec<usize> {
        let mut scored: Vec<(i32, usize)> = (0..CELLS).filter(|&i| s.cell[i] == 0)
            .map(|i| (-evaluate(&self.apply_move(s, i)), i)).collect();
        scored.sort_by_key(|&(g, _)| -g);
        scored.into_iter().take(limit).map(|(_, i)| i).collect()
    }
}

fn evaluate(s: &State) -> i32 { if s.me == 1 { s.score } else { -s.score } }

struct Search<'a> { w: &'a Windows, nodes: i64, limit: i64, aborted: bool, depth: i32, t0: Instant }

impl<'a> Search<'a> {
    fn negamax(&mut self, s: &State, depth: i32, mut alpha: i32, beta: i32, ply: i32) -> i32 {
        self.nodes += 1;
        if self.nodes > self.limit { self.aborted = true; return 0; }
        if self.nodes & 255 == 0 && self.t0.elapsed().as_millis() > TURN_MS {
            eprintln!("TIMEGUARD"); self.aborted = true; return 0;
        }
        if !s.cell.iter().any(|&v| v == 0) {
            let diff = self.w.final_triples(s);
            return if diff > 0 { WIN - ply } else if diff < 0 { -(WIN - ply) } else { 0 };
        }
        if depth == 0 { return evaluate(s); }
        let mut best = -INF;
        for m in self.w.ordered_moves(s, BEAM) {
            let v = -self.negamax(&self.w.apply_move(s, m), depth - 1, -beta, -alpha, ply + 1);
            if self.aborted { return 0; }
            if v > best { best = v; }
            if v > alpha { alpha = v; }
            if alpha >= beta { break; }
        }
        best
    }

    fn run(&mut self, root: &State) -> usize {
        let mut mv = self.w.ordered_moves(root, CELLS);
        let mut best = mv[0];
        for depth in 1..=MAX_DEPTH {
            let (mut alpha, mut best_v, mut cur_idx) = (-INF, -INF, 0usize);
            for (i, &m) in mv.iter().enumerate() {
                let v = -self.negamax(&self.w.apply_move(root, m), depth - 1, -INF, -alpha, 1);
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

fn main() {
    let windows = build_windows();
    let cli: Option<i64> = std::env::var("CG_NODES").ok().and_then(|v| v.parse().ok());
    let mut input = String::new();
    io::stdin().read_to_string(&mut input).ok();
    let mut vals = input.split_whitespace().map(|t| t.parse::<i32>().unwrap_or(-1));
    let mut out = BufWriter::new(io::stdout());
    let mut my_cells = [false; CELLS];
    loop {
        let t0 = Instant::now();
        if vals.next().is_none() || vals.next().is_none() { return; }
        let k = match vals.next() { Some(v) => v.max(0) as usize, None => return };
        let mut empty = [false; CELLS];
        for _ in 0..k {
            let (r, c) = match (vals.next(), vals.next()) { (Some(r), Some(c)) => (r, c), _ => return };
            if (0..N as i32).contains(&r) && (0..N as i32).contains(&c) { empty[r as usize * N + c as usize] = true; }
        }
        if (0..CELLS).any(|i| my_cells[i] && empty[i]) { my_cells = [false; CELLS]; } // board was cleared
        let mut cell = [0u8; CELLS];
        for i in 0..CELLS { cell[i] = if empty[i] { 0 } else if my_cells[i] { MINE } else { THEIRS }; }
        if !empty.iter().any(|&e| e) {
            writeln!(out, "0 0").ok();
            out.flush().ok();
            continue;
        }
        let s = State { cell, me: MINE, score: windows.full_score(&cell) };
        let mut search = Search { w: &windows, nodes: 0, limit: cli.unwrap_or(TURN_NODES), aborted: false, depth: 0, t0 };
        let m = search.run(&s);
        my_cells[m] = true;
        eprintln!("d={} n={} ms={}", search.depth, search.nodes, t0.elapsed().as_millis());
        writeln!(out, "{} {}", m / N, m % N).ok();
        out.flush().ok();
    }
}
