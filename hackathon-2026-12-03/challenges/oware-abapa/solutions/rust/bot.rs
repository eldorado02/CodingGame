// Oware Abapa — Rust port of solutions/python/bot.py (v1 alpha-beta).
// Rules from github.com/MaximeCheramy/cg-oware (see the Python header).
use std::io::{self, BufWriter, Read, Write};
use std::time::Instant;

const HOUSES: usize = 12;
const SIDE: usize = 6;
const WIN_SCORE: i32 = 24;
const MAX_TURNS: i32 = 200;
const TOTAL_SEEDS: i32 = 48;
const SCORE_WEIGHT: i32 = 4; // captured seed vs seed still on our side
const FIRST_TURN_NODES: i64 = 600_000;
const TURN_NODES: i64 = 60_000;
const FIRST_TURN_MS: u128 = 700;
const TURN_MS: u128 = 35;
const WIN: i32 = 1_000_000;
const INF: i32 = 1 << 30;
const MAX_DEPTH: i32 = 64;

#[derive(Clone, Copy)]
struct State { h: [i32; HOUSES], mine: i32, theirs: i32, turn: i32 } // h[0..6] = side to move

fn legal_moves(s: &State) -> Vec<usize> {
    let opponent_has_seeds = s.h[SIDE..].iter().any(|&v| v > 0);
    (0..SIDE).filter(|&i| s.h[i] > 0 && (opponent_has_seeds || s.h[i] >= (SIDE - i) as i32)).collect()
}

/// Next state from the opponent's point of view.
fn apply_move(s: &State, i: usize) -> State {
    let mut h = s.h;
    let mut seeds = h[i];
    let mut pos = i;
    h[i] = 0;
    while seeds > 0 {
        pos = (pos + 1) % HOUSES;
        if pos == i { continue; }
        h[pos] += 1;
        seeds -= 1;
    }
    let mut captured = [false; HOUSES];
    let mut cur = pos;
    while cur >= SIDE && (h[cur] == 2 || h[cur] == 3) {
        captured[cur] = true;
        cur -= 1;
    }
    let grand_slam = (SIDE..HOUSES).all(|j| h[j] == 0 || captured[j]);
    let mut gain = 0;
    if !grand_slam {
        for j in SIDE..HOUSES {
            if captured[j] { gain += h[j]; h[j] = 0; }
        }
    }
    let mut rotated = [0; HOUSES];
    for j in 0..HOUSES { rotated[j] = h[(j + SIDE) % HOUSES]; }
    State { h: rotated, mine: s.theirs, theirs: s.mine + gain, turn: s.turn + 1 }
}

fn side_seeds(s: &State, from: usize) -> i32 { s.h[from..from + SIDE].iter().sum() }

fn evaluate(s: &State) -> i32 { SCORE_WEIGHT * (s.mine - s.theirs) + side_seeds(s, 0) - side_seeds(s, SIDE) }

fn final_score(mine: i32, theirs: i32, ply: i32) -> i32 {
    if mine > theirs { WIN - ply } else if mine < theirs { -(WIN - ply) } else { 0 }
}

struct Search { nodes: i64, limit: i64, time_ms: u128, aborted: bool, depth: i32, t0: Instant }

impl Search {
    fn negamax(&mut self, s: &State, depth: i32, mut alpha: i32, beta: i32, ply: i32) -> i32 {
        self.nodes += 1;
        if self.nodes > self.limit { self.aborted = true; return 0; }
        if self.nodes & 1023 == 0 && self.t0.elapsed().as_millis() > self.time_ms {
            eprintln!("TIMEGUARD"); self.aborted = true; return 0;
        }
        if s.theirs > WIN_SCORE { return -(WIN - ply); }
        if s.mine > WIN_SCORE { return WIN - ply; }
        let mv = legal_moves(s);
        if mv.is_empty() { return final_score(s.mine + side_seeds(s, 0) + side_seeds(s, SIDE), s.theirs, ply); }
        if s.turn >= MAX_TURNS { return final_score(s.mine, s.theirs, ply); }
        if depth == 0 { return evaluate(s); }
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

    fn run(&mut self, root: &State) -> usize {
        let mut mv = legal_moves(root);
        let mut best = mv[0];
        for depth in 1..=MAX_DEPTH {
            let (mut alpha, mut best_v, mut cur_idx) = (-INF, -INF, 0usize);
            for (i, &m) in mv.iter().enumerate() {
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

fn main() {
    let cli: Option<i64> = std::env::var("CG_NODES").ok().and_then(|v| v.parse().ok());
    let mut input = String::new();
    io::stdin().read_to_string(&mut input).ok();
    let mut values = input.split_whitespace().map(|t| t.parse::<i32>().unwrap_or(0));
    let mut out = BufWriter::new(io::stdout());
    let (mut me_id, mut my_score, mut their_score, mut seeds_after_my_move, mut my_turns) = (-1i32, 0, 0, TOTAL_SEEDS, 0);
    loop {
        let mut h = [0; HOUSES];
        for slot in h.iter_mut() {
            match values.next() { Some(v) => *slot = v, None => return }
        }
        let t0 = Instant::now();
        if me_id < 0 { me_id = if h.iter().all(|&v| v == 4) { 0 } else { 1 }; }
        // the opponent's captures are the seeds that vanished since our move
        their_score += seeds_after_my_move - h.iter().sum::<i32>();
        let s = State { h, mine: my_score, theirs: their_score, turn: 2 * my_turns + me_id };
        if legal_moves(&s).is_empty() {
            writeln!(out, "0").ok();
            out.flush().ok();
            continue;
        }
        let limit = cli.unwrap_or(if my_turns == 0 { FIRST_TURN_NODES } else { TURN_NODES });
        let time_ms = if my_turns == 0 { FIRST_TURN_MS } else { TURN_MS };
        let mut search = Search { nodes: 0, limit, time_ms, aborted: false, depth: 0, t0 };
        let m = search.run(&s);
        let next = apply_move(&s, m);
        my_score = next.theirs;
        seeds_after_my_move = next.h.iter().sum();
        my_turns += 1;
        eprintln!("d={} n={} ms={}", search.depth, search.nodes, t0.elapsed().as_millis());
        writeln!(out, "{}", m).ok(); // relative house: the referee adds 6 for player 1
        out.flush().ok();
    }
}
