// Checkers (English draughts) — Rust port of solutions/python/bot.py (v1).
use std::io::{self, BufRead, BufWriter, Write};
use std::time::Instant;

const EMPTY: u8 = 0;
const BM: u8 = 1;
const BK: u8 = 2;
const RM: u8 = 3;
const RK: u8 = 4;
const SIDE_B: i32 = 0;
const SIDE_R: i32 = 1;
const MAN_V: i32 = 100;
const KING_V: i32 = 160;
const DDX: [i32; 4] = [-1, 1, -1, 1];
const DDY: [i32; 4] = [-1, -1, 1, 1];
const WIN: i32 = 1_000_000;
const INF: i32 = 1 << 30;
const MAX_DEPTH: i32 = 64;

#[derive(Clone, Copy)]
struct State { cell: [u8; 64], side: i32 }

type Path = Vec<usize>;

fn side_of(v: u8) -> i32 { if v == BM || v == BK { SIDE_B } else { SIDE_R } }
fn is_king(v: u8) -> bool { v == BK || v == RK }
fn crown_row(v: u8) -> i32 { if side_of(v) == SIDE_B { 0 } else { 7 } }
fn dir_range(v: u8) -> std::ops::Range<usize> {
    if is_king(v) { 0..4 } else if side_of(v) == SIDE_B { 0..2 } else { 2..4 }
}

fn jumps_from(c: &mut [u8; 64], sq: usize, v: u8, path: &mut Path, out: &mut Vec<Path>) {
    let (x0, y0) = ((sq % 8) as i32, (sq / 8) as i32);
    let mut found = false;
    for d in dir_range(v) {
        let (mx, my) = (x0 + DDX[d], y0 + DDY[d]);
        let (tx, ty) = (x0 + 2 * DDX[d], y0 + 2 * DDY[d]);
        if !(0..8).contains(&tx) || !(0..8).contains(&ty) { continue; }
        let mid = (my * 8 + mx) as usize;
        let to = (ty * 8 + tx) as usize;
        let mv = c[mid];
        if mv == EMPTY || side_of(mv) == side_of(v) || c[to] != EMPTY { continue; }
        found = true;
        c[mid] = EMPTY;
        path.push(to);
        if !is_king(v) && ty == crown_row(v) {
            out.push(path.clone());
        } else {
            c[to] = v;
            jumps_from(c, to, v, path, out);
            c[to] = EMPTY;
        }
        path.pop();
        c[mid] = mv;
    }
    if !found && path.len() > 1 { out.push(path.clone()); }
}

fn gen_moves(s: &State) -> Vec<Path> {
    let mut c = s.cell;
    let mut jumps = Vec::new();
    for sq in 0..64 {
        let v = c[sq];
        if v == EMPTY || side_of(v) != s.side { continue; }
        c[sq] = EMPTY;
        let mut p = vec![sq];
        jumps_from(&mut c, sq, v, &mut p, &mut jumps);
        c[sq] = v;
    }
    if !jumps.is_empty() { return jumps; }
    let mut out = Vec::new();
    for sq in 0..64 {
        let v = c[sq];
        if v == EMPTY || side_of(v) != s.side { continue; }
        let (x0, y0) = ((sq % 8) as i32, (sq / 8) as i32);
        for d in dir_range(v) {
            let (x, y) = (x0 + DDX[d], y0 + DDY[d]);
            if (0..8).contains(&x) && (0..8).contains(&y) && c[(y * 8 + x) as usize] == EMPTY {
                out.push(vec![sq, (y * 8 + x) as usize]);
            }
        }
    }
    out
}

fn apply_move(s: &State, m: &Path) -> State {
    let mut t = *s;
    let mut v = t.cell[m[0]];
    t.cell[m[0]] = EMPTY;
    for w in m.windows(2) {
        let (a, b) = (w[0] as i32, w[1] as i32);
        let (ax, ay, bx, by) = (a % 8, a / 8, b % 8, b / 8);
        if (bx - ax).abs() == 2 { t.cell[(((ay + by) / 2) * 8 + (ax + bx) / 2) as usize] = EMPTY; }
    }
    let end = *m.last().unwrap_or(&m[0]);
    if !is_king(v) && (end / 8) as i32 == crown_row(v) { v = if v == BM { BK } else { RK }; }
    t.cell[end] = v;
    t.side = 1 - s.side;
    t
}

fn evaluate(s: &State) -> i32 {
    let mut sc = 0;
    for sq in 0..64 {
        let v = s.cell[sq];
        if v == EMPTY { continue; }
        let y = (sq / 8) as i32;
        let val = if is_king(v) { KING_V } else {
            let adv = if side_of(v) == SIDE_B { 7 - y } else { y };
            MAN_V + 2 * adv + if adv == 0 { 6 } else { 0 }
        };
        sc += if side_of(v) == SIDE_B { val } else { -val };
    }
    if s.side == SIDE_B { sc } else { -sc }
}

struct Search { nodes: i64, limit: i64, aborted: bool, depth: i32, t0: Instant }

impl Search {
    fn negamax(&mut self, s: &State, depth: i32, mut alpha: i32, beta: i32, ply: i32) -> i32 {
        self.nodes += 1;
        if self.nodes > self.limit { self.aborted = true; return 0; }
        if self.nodes & 1023 == 0 && self.t0.elapsed().as_millis() > 70 {
            eprintln!("TIMEGUARD"); self.aborted = true; return 0;
        }
        let mut mv = gen_moves(s);
        if mv.is_empty() { return -(WIN - ply); }
        if depth == 0 { return evaluate(s); }
        mv.sort_by_key(|m| -(m.len() as i32)); // stable
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

    fn run(&mut self, root: &State, mut mv: Vec<Path>) -> Path {
        mv.sort_by_key(|m| -(m.len() as i32));
        let mut best = mv[0].clone();
        self.nodes = 0;
        self.aborted = false;
        for depth in 1..=MAX_DEPTH {
            let (mut alpha, mut best_v, mut cur_idx) = (-INF, -INF, 0usize);
            for (i, m) in mv.iter().enumerate() {
                let v = -self.negamax(&apply_move(root, m), depth - 1, -INF, -alpha, 1);
                if self.aborted { break; }
                if v > best_v { best_v = v; cur_idx = i; }
                if v > alpha { alpha = v; }
            }
            if self.aborted { break; }
            best = mv[cur_idx].clone();
            self.depth = depth;
            let pv = mv.remove(cur_idx);
            mv.insert(0, pv);
            if best_v >= WIN - MAX_DEPTH || best_v <= -(WIN - MAX_DEPTH) { break; }
        }
        best
    }
}

fn parse_path(txt: &str) -> Option<Path> {
    let t: Vec<u8> = txt.bytes().filter(|b| !b.is_ascii_whitespace()).map(|b| b.to_ascii_uppercase()).collect();
    if t.len() < 4 || t.len() % 2 != 0 || t.len() / 2 > 16 { return None; }
    let mut out = Vec::new();
    for ch in t.chunks(2) {
        let (col, row) = (ch[0], ch[1]);
        if !(b'A'..=b'H').contains(&col) || !(b'1'..=b'8').contains(&row) { return None; }
        out.push((8 - (row - b'0') as usize) * 8 + (col - b'A') as usize);
    }
    Some(out)
}

fn name_of(m: &Path) -> String {
    m.iter().map(|&sq| format!("{}{}", (b'A' + (sq % 8) as u8) as char, 8 - sq / 8)).collect()
}

fn main() {
    let cli: Option<i64> = std::env::var("CG_NODES").ok().and_then(|v| v.parse().ok());
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();
    let mut out = BufWriter::new(io::stdout());
    let first = match lines.next() { Some(Ok(l)) => l, _ => return };
    let side = match first.trim().chars().next() { Some(ch) if ch.to_ascii_lowercase() == 'b' => SIDE_B, _ => SIDE_R };
    loop {
        let mut cell = [EMPTY; 64];
        for y in 0..8 {
            let row = match lines.next() { Some(Ok(r)) => r, _ => return };
            let b = row.as_bytes();
            for x in 0..8 {
                let ch = if x < b.len() { b[x] } else { b'.' };
                cell[y * 8 + x] = match ch { b'b' => BM, b'B' => BK, b'r' => RM, b'R' => RK, _ => EMPTY };
            }
        }
        let t0 = Instant::now();
        let n: usize = match lines.next() { Some(Ok(l)) => l.trim().parse().unwrap_or(0), _ => return };
        let mut given: Vec<Path> = Vec::new();
        for _ in 0..n {
            match lines.next() {
                Some(Ok(l)) => {
                    if let Some(tok) = l.split_whitespace().next() {
                        if let Some(p) = parse_path(tok) { given.push(p); }
                    }
                }
                _ => break,
            }
        }
        given.truncate(96);
        let s = State { cell, side };
        if given.is_empty() {
            writeln!(out, "A1B2").ok();
            out.flush().ok();
            continue;
        }
        let mut se = Search { nodes: 0, limit: cli.unwrap_or(25_000), aborted: false, depth: 0, t0 };
        let best = se.run(&s, given);
        eprintln!("d={} n={} ms={}", se.depth, se.nodes, t0.elapsed().as_millis());
        writeln!(out, "{}", name_of(&best)).ok();
        out.flush().ok();
    }
}
