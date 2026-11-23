// Clobber — Rust port of solutions/python/bot.py (v1 alpha-beta, mobility eval).
// Bit index = y * 8 + x, y = 0 is the first input line (top row).
use std::io::{self, BufRead, BufWriter, Write};
use std::time::Instant;

const STRIDE: usize = 8;
const FIRST_TURN_NODES: i64 = 400_000;
const TURN_NODES: i64 = 60_000;
const FIRST_TURN_MS: u128 = 700;
const TURN_MS: u128 = 100;
const DIRS: [(i32, i32); 4] = [(0, -1), (0, 1), (-1, 0), (1, 0)]; // same order as Python
const COL0: u64 = 0x0101_0101_0101_0101;
const COL7: u64 = 0x8080_8080_8080_8080;
const WIN: i32 = 1_000_000;
const INF: i32 = 1 << 30;
const MAX_DEPTH: i32 = 64;

#[derive(Clone, Copy)]
struct Move { src: usize, dst: usize }

#[derive(Clone, Copy)]
struct State { mine: u64, theirs: u64 } // mine = side to move

struct Board { size: usize, cells: Vec<usize>, nb: Vec<Vec<usize>>, mask: u64 }

impl Board {
    fn new(n: usize) -> Board {
        let mut b = Board { size: n, cells: Vec::new(), nb: vec![Vec::new(); 64], mask: 0 };
        for y in 0..n {
            for x in 0..n {
                let i = y * STRIDE + x;
                b.cells.push(i);
                b.mask |= 1u64 << i;
                for (dx, dy) in DIRS {
                    let (nx, ny) = (x as i32 + dx, y as i32 + dy);
                    if (0..n as i32).contains(&nx) && (0..n as i32).contains(&ny) {
                        b.nb[i].push(ny as usize * STRIDE + nx as usize);
                    }
                }
            }
        }
        b
    }

    fn gen_moves(&self, s: &State) -> Vec<Move> {
        let mut out = Vec::new();
        for &i in self.cells.iter() {
            if (s.mine >> i) & 1 == 0 { continue; }
            for &t in self.nb[i].iter() {
                if (s.theirs >> t) & 1 == 1 { out.push(Move { src: i, dst: t }); }
            }
        }
        out
    }

    /// One shift per direction, then popcount; column masks remove row wrap.
    fn count_moves(&self, own: u64, other: u64) -> i32 {
        let up = own >> STRIDE;
        let down = (own << STRIDE) & self.mask;
        let left = (own >> 1) & !COL7 & self.mask;
        let right = (own << 1) & !COL0 & self.mask;
        ((up & other).count_ones() + (down & other).count_ones()
            + (left & other).count_ones() + (right & other).count_ones()) as i32
    }

    fn evaluate(&self, s: &State) -> i32 { self.count_moves(s.mine, s.theirs) - self.count_moves(s.theirs, s.mine) }

    fn square_name(&self, i: usize) -> String { format!("{}{}", (b'a' + (i % STRIDE) as u8) as char, self.size - i / STRIDE) }
}

fn apply_move(s: &State, m: Move) -> State {
    let mine = (s.mine & !(1u64 << m.src)) | (1u64 << m.dst);
    State { mine: s.theirs & !(1u64 << m.dst), theirs: mine } // side to move switches
}

struct Search<'a> { board: &'a Board, nodes: i64, limit: i64, time_ms: u128, aborted: bool, depth: i32, t0: Instant }

impl<'a> Search<'a> {
    fn negamax(&mut self, s: &State, depth: i32, mut alpha: i32, beta: i32, ply: i32) -> i32 {
        self.nodes += 1;
        if self.nodes > self.limit { self.aborted = true; return 0; }
        if self.nodes & 1023 == 0 && self.t0.elapsed().as_millis() > self.time_ms {
            eprintln!("TIMEGUARD"); self.aborted = true; return 0;
        }
        let mv = self.board.gen_moves(s);
        if mv.is_empty() { return -(WIN - ply); }
        if depth == 0 { return self.board.evaluate(s); }
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

    fn run(&mut self, root: &State) -> Move {
        let mut mv = self.board.gen_moves(root);
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
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();
    let mut out = BufWriter::new(io::stdout());
    let n: usize = match lines.next() { Some(Ok(l)) => l.trim().parse().unwrap_or(8), _ => return };
    let n = n.clamp(1, STRIDE);
    let board = Board::new(n);
    let me = match lines.next() { Some(Ok(l)) => l.trim().bytes().next().unwrap_or(b'w'), _ => return };
    let mut first = true;
    loop {
        let mut s = State { mine: 0, theirs: 0 };
        let mut t0 = Instant::now();
        for y in 0..n {
            let row = match lines.next() { Some(Ok(r)) => r, _ => return };
            if y == 0 { t0 = Instant::now(); }
            for (x, ch) in row.bytes().take(n).enumerate() {
                let bit = 1u64 << (y * STRIDE + x);
                if ch == me { s.mine |= bit; } else if ch == b'w' || ch == b'b' { s.theirs |= bit; }
            }
        }
        if lines.next().is_none() || lines.next().is_none() { return; }
        let limit = cli.unwrap_or(if first { FIRST_TURN_NODES } else { TURN_NODES });
        let time_ms = if first { FIRST_TURN_MS } else { TURN_MS };
        first = false;
        if board.gen_moves(&s).is_empty() {
            writeln!(out, "random").ok();
            out.flush().ok();
            continue;
        }
        let mut search = Search { board: &board, nodes: 0, limit, time_ms, aborted: false, depth: 0, t0 };
        let m = search.run(&s);
        eprintln!("d={} n={} ms={}", search.depth, search.nodes, t0.elapsed().as_millis());
        writeln!(out, "{}{}", board.square_name(m.src), board.square_name(m.dst)).ok();
        out.flush().ok();
    }
}
