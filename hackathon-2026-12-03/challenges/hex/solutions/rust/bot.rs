// Hex 11x11 (CodinGame) — Rust port of solutions/python/bot.py (v1).
// Cell = row*11+col. Neighbours (-1,0)(-1,1)(0,-1)(0,1)(1,-1)(1,0).
// MCTS + RAVE, random-fill playouts, iteration budget (env CG_NODES).
use std::io::{self, BufRead, Write};
use std::time::Instant;

const N: usize = 11;
const NC: usize = N * N;
const RED: i8 = 1;
const BLUE: i8 = 2;
const RAVE_B: f64 = 0.04;
const MAX_NODES: usize = 2_000_000;
const EXPAND_N: i32 = 3;
const OPENING: usize = 1 * N + 2;

struct Nb {
    nb: [[usize; 6]; NC],
    n: [usize; NC],
}

fn init_nb() -> Nb {
    let dr = [-1i32, -1, 0, 0, 1, 1];
    let dc = [0i32, 1, -1, 1, -1, 0];
    let mut g = Nb { nb: [[0; 6]; NC], n: [0; NC] };
    for c in 0..NC {
        let (r, q) = ((c / N) as i32, (c % N) as i32);
        let mut k = 0;
        for i in 0..6 {
            let (a, b) = (r + dr[i], q + dc[i]);
            if a >= 0 && a < N as i32 && b >= 0 && b < N as i32 {
                g.nb[c][k] = (a as usize) * N + b as usize;
                k += 1;
            }
        }
        g.n[c] = k;
    }
    g
}

fn connected(g: &Nb, board: &[i8; NC], p: i8) -> bool {
    let mut stack = [0usize; NC];
    let mut sp = 0;
    let mut seen = [false; NC];
    if p == RED {
        for c in 0..N {
            if board[c] == RED { stack[sp] = c; sp += 1; seen[c] = true; }
        }
    } else {
        for r in 0..N {
            if board[r * N] == BLUE { stack[sp] = r * N; sp += 1; seen[r * N] = true; }
        }
    }
    while sp > 0 {
        sp -= 1;
        let c = stack[sp];
        if (p == RED && c / N == N - 1) || (p == BLUE && c % N == N - 1) {
            return true;
        }
        for i in 0..g.n[c] {
            let d = g.nb[c][i];
            if !seen[d] && board[d] == p { seen[d] = true; stack[sp] = d; sp += 1; }
        }
    }
    false
}

struct Tree {
    cell: Vec<i32>,
    first: Vec<i32>,
    nch: Vec<i32>,
    n: Vec<i32>,
    w: Vec<i32>,
    an: Vec<i32>,
    aw: Vec<i32>,
    size: usize,
}

impl Tree {
    fn new() -> Tree {
        Tree {
            cell: vec![0; MAX_NODES], first: vec![0; MAX_NODES], nch: vec![0; MAX_NODES],
            n: vec![0; MAX_NODES], w: vec![0; MAX_NODES], an: vec![0; MAX_NODES], aw: vec![0; MAX_NODES],
            size: 0,
        }
    }
    fn new_node(&mut self, c: i32) -> usize {
        let i = self.size;
        self.size += 1;
        self.cell[i] = c; self.first[i] = -1; self.nch[i] = 0;
        self.n[i] = 0; self.w[i] = 0; self.an[i] = 0; self.aw[i] = 0;
        i
    }
    fn expand(&mut self, node: usize, board: &[i8; NC]) {
        if self.size + NC > MAX_NODES { return; }
        self.first[node] = self.size as i32;
        let mut k = 0;
        for c in 0..NC {
            if board[c] == 0 { self.new_node(c as i32); k += 1; }
        }
        self.nch[node] = k;
    }
    fn select(&self, node: usize) -> usize {
        let mut best = 0usize;
        let mut bestv = -1.0f64;
        let f = self.first[node] as usize;
        for i in f..f + self.nch[node] as usize {
            let (ni, ai) = (self.n[i], self.an[i]);
            let v = if ai == 0 {
                if ni > 0 { self.w[i] as f64 / ni as f64 } else { 0.5 }
            } else if ni == 0 {
                self.aw[i] as f64 / ai as f64
            } else {
                let beta = ai as f64 / (ai as f64 + ni as f64 + RAVE_B * ai as f64 * ni as f64);
                (1.0 - beta) * (self.w[i] as f64 / ni as f64) + beta * (self.aw[i] as f64 / ai as f64)
            };
            if v > bestv { bestv = v; best = i; }
        }
        best
    }
}

fn mcts(g: &Nb, t: &mut Tree, root: &[i8; NC], me: i8, iters: i64, t0: &Instant, time_ms: u128) -> (usize, i64) {
    t.size = 0;
    let mut rng: u64 = 0x9E3779B97F4A7C15;
    t.new_node(-1);
    t.expand(0, root);
    let mut path = [0usize; NC + 2];
    let mut empt = [0usize; NC];
    let mut done = 0i64;
    for it in 0..iters {
        if (it & 255) == 0 && it > 0 && t0.elapsed().as_millis() > time_ms {
            eprintln!("TIMEGUARD");
            break;
        }
        let mut board = *root;
        let mut node = 0usize;
        let mut p = me;
        let mut plen = 0;
        path[plen] = 0; plen += 1;
        while t.first[node] >= 0 && t.nch[node] > 0 {
            node = t.select(node);
            board[t.cell[node] as usize] = p;
            p = 3 - p;
            path[plen] = node; plen += 1;
        }
        if t.first[node] < 0 && t.n[node] >= EXPAND_N {
            t.expand(node, &board);
            if t.first[node] >= 0 && t.nch[node] > 0 {
                node = t.first[node] as usize;
                board[t.cell[node] as usize] = p;
                p = 3 - p;
                path[plen] = node; plen += 1;
            }
        }
        let mut k = 0;
        for c in 0..NC {
            if board[c] == 0 { empt[k] = c; k += 1; }
        }
        let mut i = k;
        while i > 1 {
            i -= 1;
            rng ^= rng << 13; rng ^= rng >> 7; rng ^= rng << 17;
            let j = (((rng >> 32) * (i as u64 + 1)) >> 32) as usize;
            empt.swap(i, j);
        }
        let mut q = p;
        for i in 0..k { board[empt[i]] = q; q = 3 - q; }
        let winner = if connected(g, &board, RED) { RED } else { BLUE };
        let mut mover = me;
        let mut pl = me;
        for d in 0..plen {
            let nd = path[d];
            t.n[nd] += 1;
            if d > 0 {
                if winner == mover { t.w[nd] += 1; }
                mover = 3 - mover;
            }
            let f = t.first[nd];
            if f >= 0 {
                let win = if winner == pl { 1 } else { 0 };
                let f = f as usize;
                for i in f..f + t.nch[nd] as usize {
                    if board[t.cell[i] as usize] == pl { t.an[i] += 1; t.aw[i] += win; }
                }
            }
            pl = 3 - pl;
        }
        done += 1;
    }
    let f = t.first[0] as usize;
    let (mut best, mut bn) = (f, -1i32);
    for i in f..f + t.nch[0] as usize {
        if t.n[i] > bn { bn = t.n[i]; best = i; }
    }
    (t.cell[best] as usize, done)
}

fn name_of(c: usize) -> String {
    format!("{}{}", (b'a' + (c % N) as u8) as char, c / N + 1)
}

fn choose(g: &Nb, t: &mut Tree, board: &mut [i8; NC], me: i8, last: &str, iters: i64, t0: &Instant, time_ms: u128) -> String {
    let reds = board.iter().filter(|&&x| x == RED).count();
    let blues = board.iter().filter(|&&x| x == BLUE).count();
    if reds == 0 && blues == 0 {
        eprintln!("opening");
        return name_of(OPENING);
    }
    if me == BLUE && reds == 1 && blues == 0 && last != "empty" {
        let c = board.iter().position(|&x| x == RED).unwrap();
        let (r, q) = (c / N, c % N);
        if r >= 1 && r <= N - 2 && q >= 1 && q <= N - 2 {
            eprintln!("swap");
            return "swap-pieces".to_string();
        }
    }
    let opp = 3 - me;
    for c in 0..NC {
        if board[c] == 0 {
            board[c] = me;
            let ok = connected(g, board, me);
            board[c] = 0;
            if ok { eprintln!("win"); return name_of(c); }
        }
    }
    for c in 0..NC {
        if board[c] == 0 {
            board[c] = opp;
            let ok = connected(g, board, opp);
            board[c] = 0;
            if ok { eprintln!("block"); return name_of(c); }
        }
    }
    let (c, done) = mcts(g, t, board, me, iters, t0, time_ms);
    eprintln!("mcts it={} nodes={} ms={}", done, t.size, t0.elapsed().as_millis());
    name_of(c)
}

fn main() {
    let g = init_nb();
    let mut t = Tree::new();
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();
    let first = match lines.next() { Some(Ok(l)) => l, _ => return };
    let me = if first.contains("red") { RED } else { BLUE };
    let cli: i64 = std::env::var("CG_NODES").ok().and_then(|v| v.trim().parse().ok()).unwrap_or(-1);
    let out = io::stdout();
    let mut turn = 0;
    while let Some(Ok(line)) = lines.next() {
        let t0 = Instant::now();
        let last = line.split_whitespace().next().unwrap_or("empty").to_string();
        let mut board = [0i8; NC];
        for r in 0..N {
            let raw = match lines.next() { Some(Ok(l)) => l, _ => String::new() };
            let row: Vec<u8> = raw.bytes().filter(|&b| b != b' ' && b != b'\r' && b != b'\n' && b != b'\t').collect();
            for q in 0..N {
                let ch = if q < row.len() { row[q] } else { b'.' };
                board[r * N + q] = if ch == b'r' { RED } else if ch == b'b' { BLUE } else { 0 };
            }
        }
        let iters = if cli > 0 { cli } else if turn == 0 { 30000 } else { 2600 };
        let time_ms: u128 = if turn == 0 { 700 } else { 70 };
        let mv = choose(&g, &mut t, &mut board, me, &last, iters, &t0, time_ms);
        let mut o = out.lock();
        writeln!(o, "{}", mv).unwrap();
        o.flush().unwrap();
        turn += 1;
    }
}
