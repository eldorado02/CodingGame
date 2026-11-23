// Mad Knights — Rust port of solutions/python/bot.py (v1, paranoid alpha-beta).
use std::io::{self, BufRead, BufWriter, Write};
use std::time::Instant;

const WIN: i32 = 1_000_000;
const INF: i32 = 1 << 30;
const MAX_DEPTH: i32 = 64;
const KX: [i32; 8] = [1, 2, 2, 1, -1, -2, -2, -1];
const KY: [i32; 8] = [2, 1, -1, -2, -2, -1, 1, 2];

#[derive(Clone, Copy)]
struct State { blocked: u64, pos: [usize; 3], alive: [bool; 3], turn: usize }

fn targets() -> Vec<Vec<usize>> {
    (0..64).map(|i| {
        let (x, y) = ((i % 8) as i32, (i / 8) as i32);
        (0..8).filter_map(|k| {
            let (nx, ny) = (x + KX[k], y + KY[k]);
            if (0..8).contains(&nx) && (0..8).contains(&ny) { Some((ny * 8 + nx) as usize) } else { None }
        }).collect()
    }).collect()
}

struct Ctx { tg: Vec<Vec<usize>>, me: usize }

impl Ctx {
    fn moves_of(&self, b: u64, sq: usize) -> Vec<usize> { self.tg[sq].iter().copied().filter(|&t| (b >> t) & 1 == 0).collect() }
    fn mob(&self, b: u64, sq: usize) -> i32 { self.tg[sq].iter().filter(|&&t| (b >> t) & 1 == 0).count() as i32 }
    fn evaluate(&self, s: &State) -> i32 {
        let mut sc = 0;
        for t in self.moves_of(s.blocked, s.pos[self.me]) { sc += 10 + self.mob(s.blocked | (1u64 << t), t); }
        for j in 0..3 { if j != self.me && s.alive[j] { sc -= 6 * self.mob(s.blocked, s.pos[j]); } }
        sc
    }
    fn order(&self, s: &State, mv: &mut Vec<usize>) {
        mv.sort_by_key(|&t| -self.mob(s.blocked | (1u64 << t), t)); // stable
    }
}

fn next_alive(alive: &[bool; 3], turn: usize) -> usize {
    for k in 1..4 { let j = (turn + k) % 3; if alive[j] { return j; } }
    turn
}

fn apply_move(s: &State, t: usize) -> State {
    let mut r = *s;
    r.pos[s.turn] = t;
    r.blocked = s.blocked | (1u64 << t);
    r.turn = next_alive(&s.alive, s.turn);
    r
}

fn eliminate(s: &State) -> State {
    let mut r = *s;
    r.alive[s.turn] = false;
    r.turn = next_alive(&r.alive, s.turn);
    r
}

struct Search<'a> { cx: &'a Ctx, nodes: i64, limit: i64, time_ms: u128, aborted: bool, depth: i32, t0: Instant }

impl<'a> Search<'a> {
    fn minimax(&mut self, s: &State, depth: i32, mut alpha: i32, mut beta: i32, ply: i32) -> i32 {
        self.nodes += 1;
        if self.nodes > self.limit { self.aborted = true; return 0; }
        if self.nodes & 1023 == 0 && self.t0.elapsed().as_millis() > self.time_ms {
            eprintln!("TIMEGUARD"); self.aborted = true; return 0;
        }
        let me = self.cx.me;
        if !s.alive[me] { return -(WIN - ply); }
        if s.alive.iter().filter(|&&a| a).count() == 1 { return WIN - ply; }
        let mut mv = self.cx.moves_of(s.blocked, s.pos[s.turn]);
        if mv.is_empty() { return self.minimax(&eliminate(s), depth, alpha, beta, ply); }
        if depth == 0 { return self.cx.evaluate(s); }
        self.cx.order(s, &mut mv);
        if s.turn == me {
            let mut best = -INF;
            for t in mv {
                let v = self.minimax(&apply_move(s, t), depth - 1, alpha, beta, ply + 1);
                if self.aborted { return 0; }
                if v > best { best = v; }
                if v > alpha { alpha = v; }
                if alpha >= beta { break; }
            }
            return best;
        }
        let mut best = INF;
        for t in mv {
            let v = self.minimax(&apply_move(s, t), depth - 1, alpha, beta, ply + 1);
            if self.aborted { return 0; }
            if v < best { best = v; }
            if v < beta { beta = v; }
            if alpha >= beta { break; }
        }
        best
    }

    fn run(&mut self, root: &State, mut mv: Vec<usize>) -> usize {
        self.cx.order(root, &mut mv);
        let mut best = mv[0];
        self.nodes = 0;
        self.aborted = false;
        for depth in 1..=MAX_DEPTH {
            let (mut alpha, mut best_v, mut cur, mut cur_idx) = (-INF, -INF, mv[0], 0usize);
            for (i, &t) in mv.iter().enumerate() {
                let v = self.minimax(&apply_move(root, t), depth - 1, alpha, INF, 1);
                if self.aborted { break; }
                if v > best_v { best_v = v; cur = t; cur_idx = i; }
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

fn color_idx(c: u8) -> Option<usize> { match c { b'r' => Some(0), b'g' => Some(1), b'b' => Some(2), _ => None } }

fn main() {
    let cli: Option<i64> = std::env::var("CG_NODES").ok().and_then(|v| v.parse().ok());
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();
    let mut out = BufWriter::new(io::stdout());
    let first_line = match lines.next() { Some(Ok(l)) => l, _ => return };
    let me = first_line.trim().bytes().next().and_then(color_idx).unwrap_or(0);
    let cx = Ctx { tg: targets(), me };
    let mut first = true;
    loop {
        let mut alive = [true; 3];
        let mut t0 = Instant::now();
        for k in 0..3 {
            let l = match lines.next() { Some(Ok(l)) => l, _ => return };
            if k == 0 { t0 = Instant::now(); }
            let parts: Vec<&str> = l.split_whitespace().collect();
            if parts.len() >= 2 && parts[0].len() == 1 {
                if let Some(c) = color_idx(parts[0].as_bytes()[0]) {
                    if let Ok(st) = parts[1].parse::<i32>() { alive[c] = st == 1; }
                }
            }
        }
        let mut blocked = 0u64;
        let mut pos = [0usize; 3];
        for y in 0..8 {
            let row = match lines.next() { Some(Ok(r)) => r, _ => String::new() };
            let b = row.as_bytes();
            for x in 0..8 {
                let ch = if x < b.len() { b[x] } else { b'#' };
                if ch != b'.' { blocked |= 1u64 << (y * 8 + x); }
                if let Some(c) = color_idx(ch) { pos[c] = y * 8 + x; }
            }
        }
        let n: usize = match lines.next() { Some(Ok(l)) => l.trim().parse().unwrap_or(0), _ => return };
        let mut legal = Vec::new();
        for _ in 0..n {
            if let Some(Ok(l)) = lines.next() {
                if let Some(tok) = l.split_whitespace().next() {
                    let b = tok.as_bytes();
                    if b.len() >= 2 && (b'a'..=b'h').contains(&b[0]) && (b'1'..=b'8').contains(&b[1]) {
                        legal.push((8 - (b[1] - b'0') as usize) * 8 + (b[0] - b'a') as usize);
                    }
                }
            }
        }
        alive[me] = true;
        let s = State { blocked, pos, alive, turn: me };
        let limit = cli.unwrap_or(if first { 400_000 } else { 60_000 });
        let time_ms = if first { 700 } else { 70 };
        first = false;
        if legal.is_empty() {
            writeln!(out, "random").ok();
            out.flush().ok();
            continue;
        }
        let mut se = Search { cx: &cx, nodes: 0, limit, time_ms, aborted: false, depth: 0, t0 };
        let m = se.run(&s, legal);
        eprintln!("d={} n={} ms={}", se.depth, se.nodes, t0.elapsed().as_millis());
        writeln!(out, "{}{}", (b'a' + (m % 8) as u8) as char, 8 - m / 8).ok();
        out.flush().ok();
    }
}
