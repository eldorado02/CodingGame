// Chain Reaction 6x6 — Rust port of solutions/python/bot.py (v1).
use std::io::{self, BufRead, BufWriter, Write};
use std::time::Instant;

const N: usize = 36;
const MAX_ROUNDS: usize = 200;
const WIN: i32 = 1_000_000;
const INF: i32 = 1 << 30;
const MAX_DEPTH: i32 = 64;

#[derive(Clone, Copy)]
struct State { own: [u8; N], orb: [i32; N], side: u8 }

struct Tables { nb: Vec<Vec<usize>>, crit: Vec<i32> }

fn tables() -> Tables {
    let mut nb = Vec::new();
    for i in 0..N {
        let (x, y) = ((i % 6) as i32, (i / 6) as i32);
        let mut l = Vec::new();
        for (dx, dy) in [(1, 0), (-1, 0), (0, 1), (0, -1)] {
            let (nx, ny) = (x + dx, y + dy);
            if (0..6).contains(&nx) && (0..6).contains(&ny) { l.push((ny * 6 + nx) as usize); }
        }
        nb.push(l);
    }
    let crit = nb.iter().map(|l| l.len() as i32).collect();
    Tables { nb, crit }
}

fn gen_moves(s: &State) -> Vec<usize> { (0..N).filter(|&i| s.own[i] == 0 || s.own[i] == s.side).collect() }

fn apply_move(tb: &Tables, s: &State, i: usize) -> State {
    let mut t = *s;
    let p = s.side;
    let o = 3 - p;
    t.own[i] = p;
    t.orb[i] += 1;
    for _ in 0..MAX_ROUNDS {
        let crit: Vec<usize> = (0..N).filter(|&j| t.orb[j] >= tb.crit[j]).collect();
        if crit.is_empty() { break; }
        for &j in crit.iter() {
            t.orb[j] -= tb.crit[j];
            if t.orb[j] == 0 { t.own[j] = 0; }
            for &k in tb.nb[j].iter() { t.orb[k] += 1; t.own[k] = p; }
        }
        if !t.own.iter().any(|&v| v == o) { break; }
    }
    t.side = o;
    t
}

fn winner(s: &State) -> u8 {
    let total: i32 = s.orb.iter().sum();
    if total < 2 { return 0; }
    let r = s.own.iter().filter(|&&v| v == 1).count();
    let b = s.own.iter().filter(|&&v| v == 2).count();
    if r == 0 && b > 0 { return 2; }
    if b == 0 && r > 0 { return 1; }
    0
}

fn side_score(tb: &Tables, s: &State, p: u8) -> i32 {
    let o = 3 - p;
    let mut sc = 0;
    for i in 0..N {
        if s.own[i] != p { continue; }
        sc += s.orb[i];
        let mut vulnerable = false;
        for &q in tb.nb[i].iter() {
            if s.own[q] == o && s.orb[q] == tb.crit[q] - 1 { vulnerable = true; sc -= 5 - tb.crit[i]; }
        }
        if !vulnerable {
            if tb.crit[i] == 2 { sc += 3; } else if tb.crit[i] == 3 { sc += 2; }
            if s.orb[i] == tb.crit[i] - 1 { sc += 2; }
        }
    }
    sc
}

fn evaluate(tb: &Tables, s: &State) -> i32 { side_score(tb, s, s.side) - side_score(tb, s, 3 - s.side) }

fn order_key(tb: &Tables, s: &State, i: usize) -> i32 {
    let mut k = 0;
    if s.own[i] == s.side && s.orb[i] == tb.crit[i] - 1 { k += 4; }
    k + 4 - tb.crit[i]
}

struct Search<'a> { tb: &'a Tables, nodes: i64, limit: i64, time_ms: u128, aborted: bool, depth: i32, t0: Instant }

impl<'a> Search<'a> {
    fn negamax(&mut self, s: &State, depth: i32, mut alpha: i32, beta: i32, ply: i32) -> i32 {
        self.nodes += 1;
        if self.nodes > self.limit { self.aborted = true; return 0; }
        if self.nodes & 255 == 0 && self.t0.elapsed().as_millis() > self.time_ms {
            eprintln!("TIMEGUARD"); self.aborted = true; return 0;
        }
        let w = winner(s);
        if w != 0 { return if w == s.side { WIN - ply } else { -(WIN - ply) }; }
        if depth == 0 { return evaluate(self.tb, s); }
        let mut mv = gen_moves(s);
        if mv.is_empty() { return -(WIN - ply); }
        let tb = self.tb;
        mv.sort_by_key(|&m| -order_key(tb, s, m));
        let mut best = -INF;
        for m in mv {
            let v = -self.negamax(&apply_move(tb, s, m), depth - 1, -beta, -alpha, ply + 1);
            if self.aborted { return 0; }
            if v > best { best = v; }
            if v > alpha { alpha = v; }
            if alpha >= beta { break; }
        }
        best
    }

    fn run(&mut self, root: &State, mut mv: Vec<usize>) -> usize {
        let tb = self.tb;
        mv.sort_by_key(|&m| -order_key(tb, root, m));
        let mut best = mv[0];
        self.nodes = 0;
        self.aborted = false;
        for depth in 1..=MAX_DEPTH {
            let (mut alpha, mut best_v, mut cur, mut cur_idx) = (-INF, -INF, mv[0], 0usize);
            for (i, &m) in mv.iter().enumerate() {
                let v = -self.negamax(&apply_move(tb, root, m), depth - 1, -INF, -alpha, 1);
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

fn main() {
    let tb = tables();
    let cli: Option<i64> = std::env::var("CG_NODES").ok().and_then(|v| v.parse().ok());
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();
    let mut out = BufWriter::new(io::stdout());
    let first_line = match lines.next() { Some(Ok(l)) => l, _ => return };
    let me: u8 = match first_line.trim().chars().next() { Some('b') | Some('B') => 2, _ => 1 };
    let mut first = true;
    while let Some(Ok(_last)) = lines.next() {
        let t0 = Instant::now();
        let mut s = State { own: [0; N], orb: [0; N], side: me };
        for y in 0..6 {
            let row = match lines.next() { Some(Ok(r)) => r, _ => return };
            let b = row.as_bytes();
            for x in 0..6 {
                if b.len() >= 2 * x + 2 {
                    let (a, c) = (b[2 * x], b[2 * x + 1]);
                    if (a == b'r' || a == b'b') && (b'1'..=b'3').contains(&c) {
                        s.own[y * 6 + x] = if a == b'r' { 1 } else { 2 };
                        s.orb[y * 6 + x] = (c - b'0') as i32;
                    }
                }
            }
        }
        let limit = cli.unwrap_or(if first { 250_000 } else { 50_000 });
        let time_ms = if first { 700 } else { 70 };
        first = false;
        let mv = gen_moves(&s);
        if mv.is_empty() {
            writeln!(out, "random").ok();
            out.flush().ok();
            continue;
        }
        let mut se = Search { tb: &tb, nodes: 0, limit, time_ms, aborted: false, depth: 0, t0 };
        let m = se.run(&s, mv);
        eprintln!("d={} n={} ms={}", se.depth, se.nodes, t0.elapsed().as_millis());
        writeln!(out, "{}{}", (b'a' + (m % 6) as u8) as char, 6 - m / 6).ok();
        out.flush().ok();
    }
}
