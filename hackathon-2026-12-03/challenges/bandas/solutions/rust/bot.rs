// Bandas 8x8 (CodinGame) — Rust port of solutions/python/bot.py (v1).
// Same referee rules (scan order, recursive pushes, border trimming), search and eval.
use std::io::{self, BufRead, Write};
use std::time::Instant;

const MAX_CELLS: usize = 64;
const EMPTY: u8 = 0;
const PAWN0: u8 = 1;
const PAWN1: u8 = 2;
const HOLE: u8 = 3;
const DIRECTIONS: [&str; 4] = ["UP", "RIGHT", "DOWN", "LEFT"];
const STEP_Y: [i32; 4] = [-1, 0, 1, 0];
const STEP_X: [i32; 4] = [0, 1, 0, -1];
const WIN: i32 = 1_000_000;
const INF: i32 = 1 << 30;
const MAX_DEPTH: i32 = 64;
const PAWN_WEIGHT: i32 = 100; // material dominates
const SAFETY_WEIGHT: i32 = 3; // per step between a pawn and the nearest drop
const SAFETY_CAP: i32 = 3; // beyond this distance a pawn is considered safe
const DEFAULT_NODES: i64 = 10000; // CodinGame budget: ~1.8 us/node local, ~40 ms on CG
const TIME_GUARD_MS: u128 = 40; // 40 % of the limit

fn is_pawn(v: u8) -> bool {
    v == PAWN0 || v == PAWN1
}

#[derive(Clone, Copy)]
struct Grid {
    h: i32,
    w: i32,
    cells: [u8; MAX_CELLS], // row major, h x w
}

impl Grid {
    fn at(&self, y: i32, x: i32) -> u8 {
        self.cells[(y * self.w + x) as usize]
    }

    fn put(&mut self, y: i32, x: i32, v: u8) {
        self.cells[(y * self.w + x) as usize] = v;
    }

    fn inside(&self, y: i32, x: i32) -> bool {
        y >= 0 && y < self.h && x >= 0 && x < self.w
    }

    /// referee singleMove*: push what is in front, then move or fall.
    fn step_pawn(&mut self, pawn: u8, y: i32, x: i32, dy: i32, dx: i32) {
        let (ny, nx) = (y + dy, x + dx);
        if !self.inside(ny, nx) {
            self.put(y, x, EMPTY);
            return;
        }
        let front = self.at(ny, nx);
        if is_pawn(front) {
            self.step_pawn(front, ny, nx, dy, dx);
            self.step_pawn(pawn, y, x, dy, dx);
        } else if front == HOLE {
            self.put(y, x, EMPTY);
        } else {
            self.put(ny, nx, pawn);
            self.put(y, x, EMPTY);
        }
    }

    /// Cells in the order the referee scans them for this direction.
    fn scan_order(&self, dir: usize) -> Vec<(i32, i32)> {
        let (h, w) = (self.h, self.w);
        match dir {
            0 => (0..w).flat_map(|x| (0..h).map(move |y| (y, x))).collect(),
            2 => (0..w).flat_map(|x| (0..h).rev().map(move |y| (y, x))).collect(),
            1 => (0..h).flat_map(|y| (0..w).rev().map(move |x| (y, x))).collect(),
            _ => (0..h).flat_map(|y| (0..w).map(move |x| (y, x))).collect(),
        }
    }

    fn move_pawns(&mut self, player: usize, dir: usize) {
        let pawn = PAWN0 + player as u8;
        for (y, x) in self.scan_order(dir) {
            if self.at(y, x) == pawn {
                self.step_pawn(pawn, y, x, STEP_Y[dir], STEP_X[dir]);
            }
        }
    }

    /// Cells of a row (column = false) or a column.
    fn line(&self, index: i32, column: bool) -> Vec<(i32, i32)> {
        if column {
            (0..self.h).map(|k| (k, index)).collect()
        } else {
            (0..self.w).map(|k| (index, k)).collect()
        }
    }

    /// Pawn-free lines become holes from one side until a line with a pawn.
    fn trim(&mut self, column: bool, reverse: bool) {
        let n = if column { self.w } else { self.h };
        for i in 0..n {
            let index = if reverse { n - 1 - i } else { i };
            let cells = self.line(index, column);
            if cells.iter().any(|&(y, x)| is_pawn(self.at(y, x))) {
                return;
            }
            for (y, x) in cells {
                self.put(y, x, HOLE);
            }
        }
    }

    fn play(&mut self, player: usize, dir: usize) {
        self.move_pawns(player, dir);
        self.trim(false, false);
        self.trim(false, true);
        self.trim(true, false);
        self.trim(true, true);
    }

    fn count(&self, player: usize) -> usize {
        let n = (self.h * self.w) as usize;
        self.cells[..n].iter().filter(|&&v| v == PAWN0 + player as u8).count()
    }

    /// None while both sides have pawns (referee checkWinner order).
    fn winner(&self) -> Option<usize> {
        if self.count(0) == 0 {
            Some(1)
        } else if self.count(1) == 0 {
            Some(0)
        } else {
            None
        }
    }

    /// Steps before this pawn would fall, in its most exposed direction.
    fn drop_distance(&self, y: i32, x: i32) -> i32 {
        let mut best = SAFETY_CAP;
        for d in 0..4 {
            let (mut steps, mut ny, mut nx) = (0, y + STEP_Y[d], x + STEP_X[d]);
            while steps < best && self.inside(ny, nx) && self.at(ny, nx) != HOLE {
                steps += 1;
                ny += STEP_Y[d];
                nx += STEP_X[d];
            }
            best = best.min(steps);
        }
        best
    }
}

fn evaluate(g: &Grid, player: usize) -> i32 {
    let mut score = 0;
    for y in 0..g.h {
        for x in 0..g.w {
            let v = g.at(y, x);
            if !is_pawn(v) {
                continue;
            }
            let sign = if v == PAWN0 + player as u8 { 1 } else { -1 };
            score += sign * (PAWN_WEIGHT + SAFETY_WEIGHT * g.drop_distance(y, x));
        }
    }
    score
}

// ---------------------------------------------------------------- search
struct Search {
    node_limit: i64,
    nodes: i64,
    aborted: bool,
    last_depth: i32,
    t0: Instant,
}

impl Search {
    fn move_value(&mut self, g: &Grid, player: usize, dir: usize, depth: i32, alpha: i32, beta: i32, ply: i32) -> i32 {
        let mut child = *g;
        child.play(player, dir);
        match child.winner() {
            Some(w) if w == player => WIN - ply,
            Some(_) => -(WIN - ply),
            None => -self.negamax(&child, 1 - player, depth - 1, -beta, -alpha, ply + 1),
        }
    }

    fn negamax(&mut self, g: &Grid, player: usize, depth: i32, mut alpha: i32, beta: i32, ply: i32) -> i32 {
        self.nodes += 1;
        if self.nodes > self.node_limit {
            self.aborted = true;
            return 0;
        }
        if (self.nodes & 255) == 0 && self.t0.elapsed().as_millis() > TIME_GUARD_MS {
            eprintln!("TIMEGUARD");
            self.aborted = true;
            return 0;
        }
        if depth == 0 {
            return evaluate(g, player);
        }
        let mut best = -INF;
        for dir in 0..4 {
            let value = self.move_value(g, player, dir, depth, alpha, beta, ply);
            if self.aborted {
                return 0;
            }
            best = best.max(value);
            alpha = alpha.max(value);
            if alpha >= beta {
                break;
            }
        }
        best
    }

    fn run(&mut self, g: &Grid, player: usize) -> usize {
        let mut order = vec![0usize, 1, 2, 3];
        let mut best = order[0];
        for depth in 1..=MAX_DEPTH {
            let (mut alpha, mut best_value, mut best_index) = (-INF, -INF, 0usize);
            for i in 0..4 {
                let value = self.move_value(g, player, order[i], depth, alpha, INF, 1);
                if self.aborted {
                    break;
                }
                if value > best_value {
                    best_value = value;
                    best_index = i;
                }
                alpha = alpha.max(value);
            }
            if self.aborted {
                break;
            }
            best = order[best_index];
            self.last_depth = depth;
            let pv = order.remove(best_index);
            order.insert(0, pv);
            if best_value >= WIN - MAX_DEPTH || best_value <= -(WIN - MAX_DEPTH) {
                break;
            }
        }
        best
    }
}

// ---------------------------------------------------------------- I/O
fn cell_code(tok: &str) -> u8 {
    match tok {
        "-" => EMPTY,
        "0" => PAWN0,
        "1" => PAWN1,
        _ => HOLE,
    }
}

fn main() {
    let node_limit: i64 = std::env::var("CG_NODES").ok().and_then(|v| v.trim().parse().ok()).unwrap_or(DEFAULT_NODES);
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines().map(|l| l.unwrap_or_default());
    let mut header = [0i32; 3]; // myId, height, width
    for value in header.iter_mut() {
        *value = lines.next().and_then(|l| l.trim().parse().ok()).unwrap_or(0);
    }
    let (me, h, w) = (header[0] as usize, header[1], header[2]);
    let out = io::stdout();
    loop {
        let mut g = Grid { h, w, cells: [HOLE; MAX_CELLS] };
        let mut t0 = Instant::now();
        for y in 0..h {
            let line = match lines.next() {
                Some(l) => l,
                None => return,
            };
            if y == 0 {
                t0 = Instant::now();
            }
            let toks: Vec<&str> = line.split_whitespace().collect();
            for x in 0..w {
                g.put(y, x, toks.get(x as usize).map_or(HOLE, |t| cell_code(t)));
            }
        }
        let mut search = Search { node_limit, nodes: 0, aborted: false, last_depth: 0, t0 };
        let dir = search.run(&g, me);
        eprintln!("d={} n={} ms={}", search.last_depth, search.nodes, t0.elapsed().as_millis());
        let mut o = out.lock();
        writeln!(o, "{}", DIRECTIONS[dir]).unwrap();
        o.flush().unwrap();
    }
}
