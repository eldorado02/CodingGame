// Amazons — Rust port of solutions/python/bot.py (v1 two-stage territory search).
// Eval = queen-distance territory + MOBILITY_WEIGHT * mobility difference.
use std::io::{self, BufRead, BufWriter, Write};

const TOP_MOVES: usize = 10; // moves kept for the arrow stage (env CG_NODES overrides)
const MOBILITY_WEIGHT: i32 = 1;
const DIRS: [(i32, i32); 8] = [(-1, -1), (0, -1), (1, -1), (-1, 0), (1, 0), (-1, 1), (0, 1), (1, 1)];
const EMPTY: u8 = b'.';
const WALL: u8 = b'-';

struct Board { n: usize, rays: Vec<Vec<Vec<usize>>> }

impl Board {
    fn new(n: usize) -> Board {
        let rays = (0..n * n).map(|i| {
            let (x, y) = ((i % n) as i32, (i / n) as i32);
            DIRS.iter().map(|&(dx, dy)| {
                let mut ray = Vec::new();
                let (mut cx, mut cy) = (x + dx, y + dy);
                while (0..n as i32).contains(&cx) && (0..n as i32).contains(&cy) {
                    ray.push(cy as usize * n + cx as usize);
                    cx += dx;
                    cy += dy;
                }
                ray
            }).collect()
        }).collect();
        Board { n, rays }
    }

    fn reach(&self, cells: &[u8], start: usize) -> Vec<usize> {
        let mut out = Vec::new();
        for ray in self.rays[start].iter() {
            for &t in ray.iter() {
                if cells[t] != EMPTY { break; }
                out.push(t);
            }
        }
        out
    }

    fn queen_distance(&self, cells: &[u8], sources: &[usize]) -> Vec<i32> {
        let mut dist = vec![-1; self.n * self.n];
        let mut frontier: Vec<usize> = sources.to_vec();
        for &s in sources { dist[s] = 0; }
        let mut d = 0;
        while !frontier.is_empty() {
            d += 1;
            let mut next = Vec::new();
            for &c in frontier.iter() {
                for t in self.reach(cells, c) {
                    if dist[t] < 0 { dist[t] = d; next.push(t); }
                }
            }
            frontier = next;
        }
        dist
    }

    fn evaluate(&self, cells: &[u8], me: u8, op: u8) -> i32 {
        let mine = pieces(cells, me);
        let theirs = pieces(cells, op);
        let (dm, dt) = (self.queen_distance(cells, &mine), self.queen_distance(cells, &theirs));
        let mut territory = 0;
        for i in 0..cells.len() {
            if cells[i] != EMPTY { continue; }
            let (a, b) = (dm[i], dt[i]);
            if a >= 0 && (b < 0 || a < b) { territory += 1; } else if b >= 0 && (a < 0 || b < a) { territory -= 1; }
        }
        let mobility: i32 = mine.iter().map(|&p| self.reach(cells, p).len() as i32).sum::<i32>()
            - theirs.iter().map(|&p| self.reach(cells, p).len() as i32).sum::<i32>();
        territory + MOBILITY_WEIGHT * mobility
    }

    /// Two stages: rank every queen move without arrow, keep the best `top`
    /// (stable sort), then score every arrow of those moves.
    fn best_action(&self, cells: &mut Vec<u8>, me: u8, op: u8, top: usize) -> Option<(usize, usize, usize)> {
        let mut ranked: Vec<(i32, usize, usize)> = Vec::new();
        for src in pieces(cells, me) {
            for dst in self.reach(cells, src) {
                cells[src] = EMPTY;
                cells[dst] = me;
                ranked.push((self.evaluate(cells, me, op), src, dst));
                cells[src] = me;
                cells[dst] = EMPTY;
            }
        }
        ranked.sort_by_key(|t| -t.0);
        let mut best: Option<((usize, usize, usize), i32)> = None;
        for &(_, src, dst) in ranked.iter().take(top) {
            cells[src] = EMPTY;
            cells[dst] = me;
            for arrow in self.reach(cells, dst) {
                cells[arrow] = WALL;
                let v = self.evaluate(cells, me, op);
                cells[arrow] = EMPTY;
                if best.map_or(true, |(_, bv)| v > bv) { best = Some(((src, dst, arrow), v)); }
            }
            cells[src] = me;
            cells[dst] = EMPTY;
        }
        best.map(|(a, _)| a)
    }

    fn square_name(&self, i: usize) -> String { format!("{}{}", (b'a' + (i % self.n) as u8) as char, self.n - i / self.n) }
}

fn pieces(cells: &[u8], color: u8) -> Vec<usize> { (0..cells.len()).filter(|&i| cells[i] == color).collect() }

fn main() {
    let top: usize = std::env::var("CG_NODES").ok().and_then(|v| v.parse().ok()).unwrap_or(TOP_MOVES);
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();
    let mut out = BufWriter::new(io::stdout());
    let n: usize = match lines.next() { Some(Ok(l)) => l.trim().parse().unwrap_or(8), _ => return };
    let n = if (1..=10).contains(&n) { n } else { 8 };
    let board = Board::new(n);
    while let Some(Ok(color)) = lines.next() {
        let me = if color.trim().starts_with('b') { b'b' } else { b'w' };
        let op = if me == b'w' { b'b' } else { b'w' };
        let mut cells: Vec<u8> = Vec::with_capacity(n * n);
        for _ in 0..n {
            let row = match lines.next() { Some(Ok(r)) => r, _ => return };
            let b = row.trim_end_matches('\r').as_bytes();
            for x in 0..n { cells.push(if x < b.len() { b[x] } else { WALL }); }
        }
        if lines.next().is_none() || lines.next().is_none() { return; }
        match board.best_action(&mut cells, me, op, top) {
            Some((a, b, c)) => writeln!(out, "{}{}{}", board.square_name(a), board.square_name(b), board.square_name(c)).ok(),
            None => writeln!(out, "random").ok(),
        };
        out.flush().ok();
    }
}
