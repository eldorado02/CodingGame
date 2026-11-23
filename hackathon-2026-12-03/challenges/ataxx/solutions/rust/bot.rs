// Ataxx 7x7 — Rust port of solutions/python/bot.py (v2 alpha-beta, bitboards).
// Bit index = y * 7 + x, y = 0 is the first input line (row 7).
use std::io::{self, BufRead, BufWriter, Write};
use std::time::Instant;

const SIZE: usize = 7;
const CELLS: usize = SIZE * SIZE;
const ALL: u64 = (1u64 << CELLS) - 1;
const FIRST_TURN_NODES: i64 = 600_000;
const TURN_NODES: i64 = 120_000;
const FIRST_TURN_MS: u128 = 700;
const TURN_MS: u128 = 70;
const SPAWN_ORDER_BONUS: i32 = 1; // spawns add a piece: try them before equal jumps
const WIN: i32 = 1_000_000;
const INF: i32 = 1 << 30;
const MAX_DEPTH: i32 = 64;

#[derive(Clone, Copy)]
struct Move { src: i32, dst: usize } // src = -1 for a spawn

#[derive(Clone, Copy)]
struct State { mine: u64, theirs: u64, walls: u64 } // mine = side to move

struct Tables { adj: [u64; CELLS], jump: [u64; CELLS] }

fn tables() -> Tables {
    let mut t = Tables { adj: [0; CELLS], jump: [0; CELLS] };
    for i in 0..CELLS {
        for j in 0..CELLS {
            let dx = (j as i32 % 7 - i as i32 % 7).abs();
            let dy = (j as i32 / 7 - i as i32 / 7).abs();
            match dx.max(dy) {
                1 => t.adj[i] |= 1u64 << j,
                2 => t.jump[i] |= 1u64 << j,
                _ => {}
            }
        }
    }
    t
}

fn bits(mut x: u64) -> impl Iterator<Item = usize> {
    std::iter::from_fn(move || {
        if x == 0 { return None; }
        let b = x.trailing_zeros() as usize;
        x &= x - 1;
        Some(b)
    })
}

impl Tables {
    fn gen_moves(&self, s: &State) -> Vec<Move> {
        let empty = ALL & !(s.mine | s.theirs | s.walls);
        let spawn_targets = bits(s.mine).fold(0u64, |acc, p| acc | self.adj[p]);
        let mut out: Vec<Move> = bits(spawn_targets & empty).map(|t| Move { src: -1, dst: t }).collect();
        for p in bits(s.mine) {
            out.extend(bits(self.jump[p] & empty).map(|t| Move { src: p as i32, dst: t }));
        }
        out
    }

    fn apply_move(&self, s: &State, m: Move) -> State {
        let mut mine = s.mine | (1u64 << m.dst);
        if m.src >= 0 { mine &= !(1u64 << m.src); }
        let flipped = self.adj[m.dst] & s.theirs;
        State { mine: s.theirs & !flipped, theirs: mine | flipped, walls: s.walls } // point of view switches
    }

    fn order_key(&self, s: &State, m: &Move) -> i32 {
        2 * (self.adj[m.dst] & s.theirs).count_ones() as i32 + if m.src < 0 { SPAWN_ORDER_BONUS } else { 0 }
    }
}

fn evaluate(s: &State) -> i32 { s.mine.count_ones() as i32 - s.theirs.count_ones() as i32 }

fn final_score(s: &State, ply: i32) -> i32 {
    let diff = evaluate(s);
    if diff > 0 { WIN - ply } else if diff < 0 { -(WIN - ply) } else { 0 }
}

struct Search<'a> { tb: &'a Tables, nodes: i64, limit: i64, time_ms: u128, aborted: bool, depth: i32, t0: Instant }

impl<'a> Search<'a> {
    fn negamax(&mut self, s: &State, depth: i32, mut alpha: i32, beta: i32, ply: i32) -> i32 {
        self.nodes += 1;
        if self.nodes > self.limit { self.aborted = true; return 0; }
        if self.nodes & 1023 == 0 && self.t0.elapsed().as_millis() > self.time_ms {
            eprintln!("TIMEGUARD"); self.aborted = true; return 0;
        }
        if s.mine == 0 || s.theirs == 0 { return final_score(s, ply); }
        if depth == 0 { return evaluate(s); }
        let mut mv = self.tb.gen_moves(s);
        if mv.is_empty() {
            let passed = State { mine: s.theirs, theirs: s.mine, walls: s.walls };
            if self.tb.gen_moves(&passed).is_empty() { return final_score(s, ply); }
            return -self.negamax(&passed, depth - 1, -beta, -alpha, ply + 1);
        }
        let tb = self.tb;
        mv.sort_by_key(|m| -tb.order_key(s, m)); // stable
        let mut best = -INF;
        for m in mv {
            let v = -self.negamax(&tb.apply_move(s, m), depth - 1, -beta, -alpha, ply + 1);
            if self.aborted { return 0; }
            if v > best { best = v; }
            if v > alpha { alpha = v; }
            if alpha >= beta { break; }
        }
        best
    }

    fn run(&mut self, root: &State, mut mv: Vec<Move>) -> Move {
        let tb = self.tb;
        mv.sort_by_key(|m| -tb.order_key(root, m));
        let mut best = mv[0];
        for depth in 1..=MAX_DEPTH {
            let (mut alpha, mut best_v, mut cur_idx) = (-INF, -INF, 0usize);
            for (i, &m) in mv.iter().enumerate() {
                let v = -self.negamax(&tb.apply_move(root, m), depth - 1, -INF, -alpha, 1);
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
    if !(b'a'..=b'g').contains(&t[0]) || !(b'1'..=b'7').contains(&t[1]) { return None; }
    Some((7 - (t[1] - b'0') as usize) * SIZE + (t[0] - b'a') as usize)
}

fn parse_action(t: &str) -> Option<Move> {
    let b = t.as_bytes();
    match b.len() {
        2 => square(b).map(|d| Move { src: -1, dst: d }),
        4 => match (square(&b[..2]), square(&b[2..])) {
            (Some(s), Some(d)) => Some(Move { src: s as i32, dst: d }),
            _ => None,
        },
        _ => None,
    }
}

fn name_of(i: usize) -> String { format!("{}{}", (b'a' + (i % SIZE) as u8) as char, 7 - i / SIZE) }

fn main() {
    let tb = tables();
    let cli: Option<i64> = std::env::var("CG_NODES").ok().and_then(|v| v.parse().ok());
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();
    let mut out = BufWriter::new(io::stdout());
    let me = match lines.next() { Some(Ok(l)) => l.bytes().next().unwrap_or(b'0'), _ => return };
    let mut first = true;
    loop {
        let mut s = State { mine: 0, theirs: 0, walls: 0 };
        let mut t0 = Instant::now();
        for y in 0..SIZE {
            let row = match lines.next() { Some(Ok(r)) => r, _ => return };
            if y == 0 { t0 = Instant::now(); }
            let b = row.trim_end_matches('\r').as_bytes();
            for x in 0..SIZE {
                let ch = if x < b.len() { b[x] } else { b'#' };
                let bit = 1u64 << (y * SIZE + x);
                if ch == b'#' { s.walls |= bit; } else if ch == me { s.mine |= bit; } else if ch == b'0' || ch == b'1' { s.theirs |= bit; }
            }
        }
        if lines.next().is_none() { return; }
        let n: usize = match lines.next() { Some(Ok(l)) => l.trim().parse().unwrap_or(0), _ => return };
        let mut legal = Vec::new();
        for _ in 0..n {
            let l = match lines.next() { Some(Ok(l)) => l, _ => return };
            if let Some(m) = l.split_whitespace().next().and_then(parse_action) { legal.push(m); }
        }
        let limit = cli.unwrap_or(if first { FIRST_TURN_NODES } else { TURN_NODES });
        let time_ms = if first { FIRST_TURN_MS } else { TURN_MS };
        first = false;
        if legal.is_empty() {
            writeln!(out, "random").ok();
            out.flush().ok();
            continue;
        }
        let mut search = Search { tb: &tb, nodes: 0, limit, time_ms, aborted: false, depth: 0, t0 };
        let best = search.run(&s, legal);
        eprintln!("d={} n={} ms={}", search.depth, search.nodes, t0.elapsed().as_millis());
        let text = if best.src < 0 { name_of(best.dst) } else { name_of(best.src as usize) + &name_of(best.dst) };
        writeln!(out, "{}", text).ok();
        out.flush().ok();
    }
}
