// Onitama 5x5 — Rust port of solutions/python/bot.py (v1 alpha-beta).
// Cells: index = (row - 1) * 5 + x, row 1 = last input line; cards stored in
// player 0's frame (player 1 negates).
use std::collections::HashMap;
use std::io::{self, BufRead, BufWriter, Write};
use std::time::Instant;

const SIZE: i32 = 5;
const CELLS: usize = 25;
const TURN_NODES: i64 = 30_000;
const TURN_MS: u128 = 35;
const STUDENT_VALUE: i32 = 100;
const SHRINE_DISTANCE_WEIGHT: i32 = 4;
const SHRINE: [usize; 2] = [22, 2]; // player 0 targets C5, player 1 targets C1
const EMPTY: u8 = 0;
const MASTER: u8 = 1;
const STUDENT: u8 = 2;
const WIN: i32 = 1_000_000;
const INF: i32 = 1 << 30;
const MAX_DEPTH: i32 = 64;

#[derive(Clone, Copy, PartialEq)]
struct Move { card: i32, src: i32, dst: i32 } // src = -1 for PASS

#[derive(Clone, Copy)]
struct State { kind: [u8; CELLS], owner: [u8; CELLS], hands: [[i32; 2]; 2], center: i32, side: usize }

fn cell_index(x: i32, row: i32) -> i32 { (row - 1) * SIZE + x }

struct Game { cards: HashMap<i32, Vec<(i32, i32)>> } // (dx, dy) in player 0's frame

impl Game {
    fn gen_moves(&self, s: &State) -> Vec<Move> {
        let sign = if s.side == 0 { 1 } else { -1 };
        let mut out = Vec::new();
        for &card in s.hands[s.side].iter() {
            let empty = Vec::new();
            let moves = self.cards.get(&card).unwrap_or(&empty);
            for src in 0..CELLS {
                if s.kind[src] == EMPTY || s.owner[src] as usize != s.side { continue; }
                let (x, row) = (src as i32 % SIZE, src as i32 / SIZE + 1);
                for &(dx, dy) in moves.iter() {
                    let (nx, nrow) = (x + sign * dx, row + sign * dy);
                    if !(0..SIZE).contains(&nx) || !(1..=SIZE).contains(&nrow) { continue; }
                    let dst = cell_index(nx, nrow) as usize;
                    if s.kind[dst] != EMPTY && s.owner[dst] as usize == s.side { continue; }
                    out.push(Move { card, src: src as i32, dst: dst as i32 });
                }
            }
        }
        if out.is_empty() {
            out = s.hands[s.side].iter().map(|&card| Move { card, src: -1, dst: -1 }).collect();
        }
        out
    }
}

fn apply_move(s: &State, m: &Move) -> State {
    let mut t = *s;
    if m.src >= 0 {
        let (src, dst) = (m.src as usize, m.dst as usize);
        t.kind[dst] = s.kind[src];
        t.owner[dst] = s.owner[src];
        t.kind[src] = EMPTY;
    }
    if let Some(h) = t.hands[s.side].iter().position(|&c| c == m.card) { t.hands[s.side][h] = s.center; }
    t.center = m.card;
    t.side = 1 - s.side;
    t
}

fn master_of(s: &State, p: usize) -> Option<usize> { (0..CELLS).find(|&i| s.kind[i] == MASTER && s.owner[i] as usize == p) }

fn winner(s: &State) -> i32 {
    for p in 0..2 {
        match master_of(s, p) {
            None => return 1 - p as i32,
            Some(m) if m == SHRINE[p] => return p as i32,
            _ => {}
        }
    }
    -1
}

fn distance(a: usize, b: usize) -> i32 {
    let (a, b) = (a as i32, b as i32);
    (a % SIZE - b % SIZE).abs().max((a / SIZE - b / SIZE).abs())
}

fn side_score(s: &State, p: usize) -> i32 {
    let students = (0..CELLS).filter(|&i| s.kind[i] == STUDENT && s.owner[i] as usize == p).count() as i32;
    STUDENT_VALUE * students - SHRINE_DISTANCE_WEIGHT * distance(master_of(s, p).unwrap_or(SHRINE[p]), SHRINE[p])
}

fn evaluate(s: &State) -> i32 { side_score(s, s.side) - side_score(s, 1 - s.side) }

fn order_key(s: &State, m: &Move) -> i32 { if m.src >= 0 && s.kind[m.dst as usize] != EMPTY { 1 } else { 0 } }

struct Search<'a> { game: &'a Game, nodes: i64, limit: i64, aborted: bool, depth: i32, t0: Instant }

impl<'a> Search<'a> {
    fn negamax(&mut self, s: &State, depth: i32, mut alpha: i32, beta: i32, ply: i32) -> i32 {
        self.nodes += 1;
        if self.nodes > self.limit { self.aborted = true; return 0; }
        if self.nodes & 1023 == 0 && self.t0.elapsed().as_millis() > TURN_MS {
            eprintln!("TIMEGUARD"); self.aborted = true; return 0;
        }
        let w = winner(s);
        if w >= 0 { return if w as usize == s.side { WIN - ply } else { -(WIN - ply) }; }
        if depth == 0 { return evaluate(s); }
        let mut mv = self.game.gen_moves(s);
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

fn square(t: &[u8]) -> Option<i32> {
    if t.len() < 2 || !(b'A'..=b'E').contains(&t[0]) || !(b'1'..=b'5').contains(&t[1]) { return None; }
    Some(cell_index((t[0] - b'A') as i32, (t[1] - b'0') as i32))
}

fn name_of(i: i32) -> String { format!("{}{}", (b'A' + (i % SIZE) as u8) as char, i / SIZE + 1) }

fn main() {
    let cli: Option<i64> = std::env::var("CG_NODES").ok().and_then(|v| v.parse().ok());
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();
    let mut out = BufWriter::new(io::stdout());
    let me: usize = match lines.next() { Some(Ok(l)) => l.trim().parse().unwrap_or(0), _ => return };
    let mut game = Game { cards: HashMap::new() };
    loop {
        let mut s = State { kind: [EMPTY; CELLS], owner: [0; CELLS], hands: [[0; 2]; 2], center: -1, side: me };
        let mut t0 = Instant::now();
        for li in 0..SIZE {
            let r = match lines.next() { Some(Ok(r)) => r, _ => return };
            if li == 0 { t0 = Instant::now(); }
            let b = r.as_bytes();
            for x in 0..SIZE {
                let ch = if (x as usize) < b.len() { b[x as usize] } else { b'.' };
                let i = cell_index(x, SIZE - li) as usize;
                match ch {
                    b'W' | b'w' => { s.kind[i] = if ch == b'W' { MASTER } else { STUDENT }; s.owner[i] = 0; }
                    b'B' | b'b' => { s.kind[i] = if ch == b'B' { MASTER } else { STUDENT }; s.owner[i] = 1; }
                    _ => {}
                }
            }
        }
        let mut hand_count = [0usize; 2];
        for _ in 0..5 {
            let l = match lines.next() { Some(Ok(l)) => l, _ => return };
            let v: Vec<i32> = l.split_whitespace().map(|t| t.parse().unwrap_or(0)).collect();
            if v.len() < 10 { continue; }
            let (owner, id) = (v[0], v[1]);
            let user = if owner == -1 { me as i32 } else { owner }; // the centre card's next user is us
            let sign = if user == 0 { 1 } else { -1 };
            let moves: Vec<(i32, i32)> = (2..10).step_by(2).filter(|&j| v[j] != 0 || v[j + 1] != 0)
                .map(|j| (sign * v[j], sign * v[j + 1])).collect();
            game.cards.insert(id, moves);
            if owner == -1 { s.center = id; } else if (owner == 0 || owner == 1) && hand_count[owner as usize] < 2 {
                s.hands[owner as usize][hand_count[owner as usize]] = id;
                hand_count[owner as usize] += 1;
            }
        }
        let n: usize = match lines.next() { Some(Ok(l)) => l.trim().parse().unwrap_or(0), _ => return };
        let mut legal = Vec::new();
        for _ in 0..n {
            let l = match lines.next() { Some(Ok(l)) => l, _ => return };
            let parts: Vec<&str> = l.split_whitespace().collect();
            if parts.len() < 2 { continue; }
            let card: i32 = match parts[0].parse() { Ok(c) => c, Err(_) => continue };
            if parts[1] == "PASS" { legal.push(Move { card, src: -1, dst: -1 }); continue; }
            let b = parts[1].as_bytes();
            if b.len() < 4 { continue; }
            if let (Some(a), Some(d)) = (square(&b[0..2]), square(&b[2..4])) { legal.push(Move { card, src: a, dst: d }); }
        }
        if legal.is_empty() {
            writeln!(out, "{} PASS", s.hands[me][0]).ok();
            out.flush().ok();
            continue;
        }
        let mut search = Search { game: &game, nodes: 0, limit: cli.unwrap_or(TURN_NODES), aborted: false, depth: 0, t0 };
        let m = search.run(&s, legal);
        eprintln!("d={} n={} ms={}", search.depth, search.nodes, t0.elapsed().as_millis());
        if m.src < 0 { writeln!(out, "{} PASS", m.card).ok(); } else { writeln!(out, "{} {}{}", m.card, name_of(m.src), name_of(m.dst)).ok(); }
        out.flush().ok();
    }
}
