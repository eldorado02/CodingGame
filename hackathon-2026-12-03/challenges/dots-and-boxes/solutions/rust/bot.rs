// Dots and Boxes — Rust port of solutions/python/bot.py (v1 alpha-beta).
// Edges: horizontal h(x, line) = line * n + x, vertical H + y * (n + 1) + line.
// Closing a box scores it and the same player moves again (no sign flip).
use std::collections::HashMap;
use std::io::{self, BufRead, BufWriter, Write};
use std::time::Instant;

const FIRST_TURN_NODES: i64 = 200_000;
const TURN_NODES: i64 = 20_000;
const FIRST_TURN_MS: u128 = 700;
const TURN_MS: u128 = 70;
const CAPTURABLE_WEIGHT: i32 = 1; // a box with one free side is ours to take
const SIDE_LETTERS: &str = "LTRB";
const WIN: i32 = 1_000_000;
const INF: i32 = 1 << 30;
const MAX_DEPTH: i32 = 64;

struct Board { n: usize, edges: usize, box_edges: Vec<[usize; 4]>, edge_boxes: Vec<Vec<usize>> }

#[derive(Clone)]
struct State { drawn: Vec<bool>, diff: i32 } // diff = side to move - other

impl Board {
    fn new(n: usize) -> Board {
        let h = n * (n + 1);
        let edges = 2 * n * (n + 1);
        let mut box_edges = Vec::new();
        for y in 0..n {
            for x in 0..n {
                let left = h + y * (n + 1) + x;
                box_edges.push([left, (y + 1) * n + x, left + 1, y * n + x]);
            }
        }
        let mut edge_boxes = vec![Vec::new(); edges];
        for (b, es) in box_edges.iter().enumerate() { for &e in es.iter() { edge_boxes[e].push(b); } }
        Board { n, edges, box_edges, edge_boxes }
    }

    fn free_sides(&self, drawn: &[bool], b: usize) -> i32 { self.box_edges[b].iter().filter(|&&e| !drawn[e]).count() as i32 }

    fn gen_moves(&self, s: &State) -> Vec<usize> { (0..self.edges).filter(|&e| !s.drawn[e]).collect() }

    /// Returns (child, same_player).
    fn apply_move(&self, s: &State, e: usize) -> (State, bool) {
        let mut drawn = s.drawn.clone();
        drawn[e] = true;
        let closed = self.edge_boxes[e].iter().filter(|&&b| self.free_sides(&drawn, b) == 0).count() as i32;
        if closed > 0 { (State { drawn, diff: s.diff + closed }, true) } else { (State { drawn, diff: -s.diff }, false) }
    }

    fn evaluate(&self, s: &State) -> i32 {
        let capturable = (0..self.n * self.n).filter(|&b| self.free_sides(&s.drawn, b) == 1).count() as i32;
        s.diff + CAPTURABLE_WEIGHT * capturable
    }

    /// 2 = closes a box, 1 = safe (gives no third side), 0 = gift.
    fn order_key(&self, s: &State, e: usize) -> i32 {
        let (mut closes, mut gift) = (false, false);
        for &b in self.edge_boxes[e].iter() {
            match self.free_sides(&s.drawn, b) { 1 => closes = true, 2 => gift = true, _ => {} }
        }
        if closes { 2 } else if gift { 0 } else { 1 }
    }
}

struct Search<'a> { board: &'a Board, nodes: i64, limit: i64, time_ms: u128, aborted: bool, depth: i32, t0: Instant }

impl<'a> Search<'a> {
    fn value(&mut self, s: &State, depth: i32, mut alpha: i32, beta: i32) -> i32 {
        self.nodes += 1;
        if self.nodes > self.limit { self.aborted = true; return 0; }
        if self.nodes & 1023 == 0 && self.t0.elapsed().as_millis() > self.time_ms {
            eprintln!("TIMEGUARD"); self.aborted = true; return 0;
        }
        let mut mv = self.board.gen_moves(s);
        if mv.is_empty() { return if s.diff > 0 { WIN + s.diff } else if s.diff < 0 { -WIN + s.diff } else { 0 }; }
        if depth == 0 { return self.board.evaluate(s); }
        let board = self.board;
        mv.sort_by_key(|&e| -board.order_key(s, e)); // stable
        let mut best = -INF;
        for e in mv {
            let (child, again) = board.apply_move(s, e);
            let v = if again { self.value(&child, depth - 1, alpha, beta) } else { -self.value(&child, depth - 1, -beta, -alpha) };
            if self.aborted { return 0; }
            if v > best { best = v; }
            if v > alpha { alpha = v; }
            if alpha >= beta { break; }
        }
        best
    }

    fn run(&mut self, root: &State) -> usize {
        let board = self.board;
        let mut mv = board.gen_moves(root);
        mv.sort_by_key(|&e| -board.order_key(root, e));
        let mut best = mv[0];
        let k = mv.len() as i32;
        for depth in 1..=MAX_DEPTH {
            let (mut alpha, mut best_v, mut cur_idx) = (-INF, -INF, 0usize);
            for (i, &e) in mv.iter().enumerate() {
                let (child, again) = board.apply_move(root, e);
                let v = if again { self.value(&child, depth - 1, alpha, INF) } else { -self.value(&child, depth - 1, -INF, -alpha) };
                if self.aborted { break; }
                if v > best_v { best_v = v; cur_idx = i; }
                if v > alpha { alpha = v; }
            }
            if self.aborted { break; }
            best = mv[cur_idx];
            self.depth = depth;
            let pv = mv.remove(cur_idx); // PV first next iteration
            mv.insert(0, pv);
            if depth >= k + 1 { break; }
        }
        best
    }
}

fn main() {
    let cli: Option<i64> = std::env::var("CG_NODES").ok().and_then(|v| v.parse().ok());
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();
    let mut out = BufWriter::new(io::stdout());
    let n: usize = match lines.next() { Some(Ok(l)) => l.trim().parse().unwrap_or(2), _ => return };
    let n = if (1..=10).contains(&n) { n } else { 2 };
    let board = Board::new(n);
    if lines.next().is_none() { return; } // our id: the state is given in our point of view
    let mut first = true;
    while let Some(Ok(score_line)) = lines.next() {
        let t0 = Instant::now();
        let sc: Vec<i32> = score_line.split_whitespace().map(|t| t.parse().unwrap_or(0)).collect();
        let diff = sc.first().copied().unwrap_or(0) - sc.get(1).copied().unwrap_or(0);
        let k: usize = match lines.next() { Some(Ok(l)) => l.trim().parse().unwrap_or(0), _ => return };
        let mut drawn = vec![true; board.edges];
        let mut owner: HashMap<usize, String> = HashMap::new();
        for _ in 0..k {
            let l = match lines.next() { Some(Ok(l)) => l, _ => return };
            let parts: Vec<&str> = l.split_whitespace().collect();
            if parts.len() < 2 || parts[0].len() < 2 { continue; }
            let name = parts[0];
            let x = name.as_bytes()[0] as i32 - b'A' as i32;
            let y = name[1..].parse::<i32>().unwrap_or(0) - 1;
            if !(0..n as i32).contains(&x) || !(0..n as i32).contains(&y) { continue; }
            for ch in parts[1].chars() {
                if let Some(idx) = SIDE_LETTERS.find(ch) {
                    let e = board.box_edges[y as usize * n + x as usize][idx];
                    drawn[e] = false;
                    owner.entry(e).or_insert_with(|| format!("{} {}", name, ch));
                }
            }
        }
        let limit = cli.unwrap_or(if first { FIRST_TURN_NODES } else { TURN_NODES });
        let time_ms = if first { FIRST_TURN_MS } else { TURN_MS };
        first = false;
        if owner.is_empty() {
            writeln!(out, "A1 L").ok();
            out.flush().ok();
            continue;
        }
        let s = State { drawn, diff };
        let mut search = Search { board: &board, nodes: 0, limit, time_ms, aborted: false, depth: 0, t0 };
        let e = search.run(&s);
        eprintln!("d={} n={} ms={}", search.depth, search.nodes, t0.elapsed().as_millis());
        writeln!(out, "{}", owner[&e]).ok();
        out.flush().ok();
    }
}
