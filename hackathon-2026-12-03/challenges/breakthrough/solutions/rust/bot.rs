// Breakthrough 8x8 — Rust port of solutions/python/bot.py (v3 bitboards).
// Same move order, same tie-breaks, same node budget semantics.
use std::collections::HashSet;
use std::io::{self, BufRead, BufWriter, Write};
use std::time::Instant;

const WHITE: i32 = 1;
const BLACK: i32 = 2;
const FILE_A: u64 = 0x0101_0101_0101_0101;
const FILE_H: u64 = 0x8080_8080_8080_8080;
const ADV: [i32; 8] = [0, 4, 10, 20, 36, 60, 110, 0];
const PIECE: i32 = 1000;
const DEFENDED: i32 = 12;
const HOME_GUARD: i32 = 18;
const WIN: i32 = 1_000_000;
const INF: i32 = 1 << 30;
const MAX_DEPTH: i32 = 64;

fn rank(r: u32) -> u64 { 0xFFu64 << (8 * r) }

#[derive(Clone, Copy, PartialEq, Eq)]
struct Move { from: u32, to: u32 }

#[derive(Clone, Copy)]
struct State { w: u64, b: u64, to_move: i32 }

impl State {
    fn own(&self, p: i32) -> u64 { if p == WHITE { self.w } else { self.b } }
}

fn popc(x: u64) -> i32 { x.count_ones() as i32 }

fn attacks(p: i32, x: u64) -> u64 {
    if p == WHITE { ((x << 7) & !FILE_H) | ((x << 9) & !FILE_A) }
    else { ((x >> 9) & !FILE_H) | ((x >> 7) & !FILE_A) }
}

fn runners(p: i32, x: u64) -> u64 { x & if p == WHITE { rank(6) } else { rank(1) } }

fn winner(s: &State) -> i32 {
    if s.w & rank(7) != 0 { return WHITE; }
    if s.b & rank(0) != 0 { return BLACK; }
    if s.w == 0 { return BLACK; }
    if s.b == 0 { return WHITE; }
    0
}

fn gen_moves(s: &State) -> Vec<Move> {
    let p = s.to_move;
    let d: i32 = if p == WHITE { 1 } else { -1 };
    let mine = s.own(p);
    let occ = s.w | s.b;
    let mut out = Vec::with_capacity(48);
    let mut x = mine;
    while x != 0 {
        let sq = x.trailing_zeros();
        x &= x - 1;
        let (r, f) = ((sq >> 3) as i32, (sq & 7) as i32);
        let nr = r + d;
        if !(0..=7).contains(&nr) { continue; }
        for df in -1..=1 {
            let nf = f + df;
            if !(0..=7).contains(&nf) { continue; }
            let t = (nr * 8 + nf) as u32;
            let tb = 1u64 << t;
            if tb & mine != 0 { continue; }
            if df == 0 && tb & occ != 0 { continue; }
            out.push(Move { from: sq, to: t });
        }
    }
    out
}

fn apply_move(s: &State, m: Move) -> State {
    let (fb, tb) = (1u64 << m.from, 1u64 << m.to);
    if s.to_move == WHITE {
        State { w: (s.w ^ fb) | tb, b: s.b & !tb, to_move: BLACK }
    } else {
        State { w: s.w & !tb, b: (s.b ^ fb) | tb, to_move: WHITE }
    }
}

fn side_score(p: i32, x: u64) -> i32 {
    let mut sc = PIECE * popc(x);
    for r in 0..8u32 {
        let c = popc(x & rank(r));
        if c != 0 { sc += c * ADV[if p == WHITE { r as usize } else { 7 - r as usize }]; }
    }
    sc += HOME_GUARD * popc(x & if p == WHITE { rank(0) } else { rank(7) });
    sc += DEFENDED * popc(x & attacks(p, x));
    sc
}

fn evaluate(s: &State) -> i32 {
    let p = s.to_move;
    let o = 3 - p;
    side_score(p, s.own(p)) - side_score(o, s.own(o))
}

fn order_key(s: &State, m: &Move) -> i32 {
    let mut k = 0;
    let goal = if s.to_move == WHITE { 7 } else { 0 };
    if (m.to >> 3) == goal { k += 4; }
    if (1u64 << m.to) & (s.w | s.b) != 0 { k += 2; }
    k
}

fn runner_rule(s: &State, ply: i32) -> i32 {
    let p = s.to_move;
    let o = 3 - p;
    if runners(p, s.own(p)) != 0 { return WIN - (ply + 1); }
    let r = runners(o, s.own(o));
    if r != 0 && ((r & (r - 1)) != 0 || r & attacks(p, s.own(p)) == 0) { return -(WIN - (ply + 2)); }
    0
}

struct Search { nodes: i64, limit: i64, time_ms: u128, aborted: bool, depth: i32, t0: Instant }

impl Search {
    fn negamax(&mut self, s: &State, depth: i32, mut alpha: i32, beta: i32, ply: i32) -> i32 {
        self.nodes += 1;
        if self.nodes > self.limit { self.aborted = true; return 0; }
        if self.nodes & 1023 == 0 && self.t0.elapsed().as_millis() > self.time_ms {
            eprintln!("TIMEGUARD"); self.aborted = true; return 0;
        }
        let w = winner(s);
        if w != 0 { return if w == s.to_move { WIN - ply } else { -(WIN - ply) }; }
        let q = runner_rule(s, ply);
        if q != 0 { return q; }
        if depth == 0 { return evaluate(s); }
        let mut mv = gen_moves(s);
        if mv.is_empty() { return -(WIN - ply); }
        mv.sort_by_key(|m| -order_key(s, m)); // stable
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

fn sq_of(t: &[u8]) -> u32 { ((t[1] - b'1') as u32) * 8 + (t[0] - b'a') as u32 }

fn name_of(m: Move) -> String {
    let mut s = String::with_capacity(4);
    s.push((b'a' + (m.from & 7) as u8) as char);
    s.push((b'1' + (m.from >> 3) as u8) as char);
    s.push((b'a' + (m.to & 7) as u8) as char);
    s.push((b'1' + (m.to >> 3) as u8) as char);
    s
}

fn valid_move_text(t: &str) -> bool {
    let b = t.as_bytes();
    b.len() >= 4 && (b'a'..=b'h').contains(&b[0]) && (b'1'..=b'8').contains(&b[1])
        && (b'a'..=b'h').contains(&b[2]) && (b'1'..=b'8').contains(&b[3])
}

fn main() {
    let cli: Option<i64> = std::env::var("CG_NODES").ok().and_then(|v| v.parse().ok());
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();
    let mut out = BufWriter::new(io::stdout());
    let mut s = State { w: 0xFFFF, b: 0xFFFFu64 << 48, to_move: WHITE };
    let mut me = 0;
    let mut first = true;
    while let Some(Ok(line)) = lines.next() {
        let t0 = Instant::now();
        let opp = line.split_whitespace().next().unwrap_or("None").to_string();
        let n: usize = match lines.next() { Some(Ok(l)) => l.trim().parse().unwrap_or(0), _ => break };
        let mut legal: Vec<String> = Vec::new();
        for _ in 0..n {
            match lines.next() {
                Some(Ok(l)) => {
                    if let Some(t) = l.split_whitespace().next() {
                        if valid_move_text(t) { legal.push(t[..4].to_string()); }
                    }
                }
                _ => break,
            }
        }
        if me == 0 { me = if opp == "None" { WHITE } else { BLACK }; }
        if valid_move_text(&opp) {
            let b = opp.as_bytes();
            s.to_move = 3 - me;
            s = apply_move(&s, Move { from: sq_of(&b[0..2]), to: sq_of(&b[2..4]) });
        }
        s.to_move = me;
        let limit = cli.unwrap_or(if first { 600_000 } else { 60_000 });
        let time_ms = if first { 700 } else { 75 };
        first = false;
        let set: HashSet<&str> = legal.iter().map(|x| x.as_str()).collect();
        let mine: Vec<Move> = gen_moves(&s).into_iter().filter(|m| set.contains(name_of(*m).as_str())).collect();
        let mut se = Search { nodes: 0, limit, time_ms, aborted: false, depth: 0, t0 };
        let answer = if mine.is_empty() {
            match legal.first() {
                Some(l) => {
                    let b = l.as_bytes();
                    s = apply_move(&s, Move { from: sq_of(&b[0..2]), to: sq_of(&b[2..4]) });
                    l.clone()
                }
                None => "a1a2".to_string(),
            }
        } else {
            let m = se.run(&s, mine);
            s = apply_move(&s, m);
            name_of(m)
        };
        eprintln!("d={} n={} ms={}", se.depth, se.nodes, t0.elapsed().as_millis());
        writeln!(out, "{}", answer).ok();
        out.flush().ok();
    }
}
