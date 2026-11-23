// Yavalath (CodinGame) — Rust port of solutions/python/bot.py (v1).
// Same geometry, move order, tie-breaks and node budget semantics.
use std::io::{self, BufRead, Write};
use std::time::Instant;

const NC: usize = 61;
const WIN: i32 = 1_000_000;
const INF: i32 = 1 << 30;
const MAX_DEPTH: i32 = 64;
const W_TWO: i32 = 10;
const W_THREE: i32 = 40;
const W_SAFE: i32 = 3;
const OPENING: usize = 7;

struct Geo {
    rows: [usize; 9],
    cx: [usize; NC],
    cy: [usize; NC],
    idx: [[i32; 9]; 9],
    ray: [[[usize; 3]; 6]; NC],
    rayn: [[usize; 6]; NC],
    windows: Vec<[usize; 4]>,
    win_of: Vec<Vec<usize>>,
}

fn qof(x: usize, y: usize) -> usize { x + 4 - y.min(4) }

fn init_geo() -> Geo {
    let mut g = Geo {
        rows: [0; 9], cx: [0; NC], cy: [0; NC], idx: [[-1; 9]; 9],
        ray: [[[0; 3]; 6]; NC], rayn: [[0; 6]; NC], windows: Vec::new(), win_of: vec![Vec::new(); NC],
    };
    let mut n = 0;
    for y in 0..9 {
        g.rows[y] = 9 - (4i32 - y as i32).unsigned_abs() as usize;
        for x in 0..g.rows[y] {
            g.cx[n] = x;
            g.cy[n] = y;
            g.idx[qof(x, y)][y] = n as i32;
            n += 1;
        }
    }
    let dq = [1i32, -1, 0, 0, -1, 1];
    let dr = [0i32, 0, 1, -1, 1, -1];
    for i in 0..NC {
        let (q, r) = (qof(g.cx[i], g.cy[i]) as i32, g.cy[i] as i32);
        for d in 0..6 {
            let mut k = 0;
            for s in 1..4 {
                let (a, b) = (q + dq[d] * s, r + dr[d] * s);
                if a < 0 || a > 8 || b < 0 || b > 8 || g.idx[a as usize][b as usize] < 0 { break; }
                g.ray[i][d][k] = g.idx[a as usize][b as usize] as usize;
                k += 1;
            }
            g.rayn[i][d] = k;
        }
    }
    for i in 0..NC {
        for d in [0usize, 2, 4] {
            if g.rayn[i][d] == 3 {
                g.windows.push([i, g.ray[i][d][0], g.ray[i][d][1], g.ray[i][d][2]]);
            }
        }
    }
    for w in 0..g.windows.len() {
        for k in 0..4 {
            let c = g.windows[w][k];
            g.win_of[c].push(w);
        }
    }
    g
}

fn place_result(g: &Geo, b: &[i8; NC], c: usize, p: i8) -> i32 {
    let mut best = 1;
    for a in [0usize, 2, 4] {
        let mut n = 1;
        for k in 0..g.rayn[c][a] { if b[g.ray[c][a][k]] != p { break; } n += 1; }
        for k in 0..g.rayn[c][a + 1] { if b[g.ray[c][a + 1][k]] != p { break; } n += 1; }
        if n > best { best = n; }
    }
    if best >= 4 { 2 } else if best == 3 { 1 } else { 0 }
}

fn evaluate(g: &Geo, b: &[i8; NC], p: i8, safe_p: i32, safe_o: i32) -> i32 {
    let o = 3 - p;
    let mut s = 0;
    for w in &g.windows {
        let (mut a, mut n) = (0, 0);
        for &c in w.iter() {
            let v = b[c];
            if v == p { a += 1; } else if v == o { n += 1; }
        }
        if n == 0 {
            if a == 2 { s += W_TWO; } else if a == 3 { s += W_THREE; }
        } else if a == 0 {
            if n == 2 { s -= W_TWO; } else if n == 3 { s -= W_THREE; }
        }
    }
    s + W_SAFE * (safe_p - safe_o)
}

fn order_key(g: &Geo, b: &[i8; NC], c: usize, p: i8) -> i32 {
    const OWN: [i32; 4] = [0, 1, 4, 8];
    const OPP: [i32; 4] = [0, 0, 3, 6];
    let o = 3 - p;
    let mut k = 0;
    for &w in &g.win_of[c] {
        let (mut a, mut n) = (0usize, 0usize);
        for &t in g.windows[w].iter() {
            let v = b[t];
            if v == p { a += 1; } else if v == o { n += 1; }
        }
        if n == 0 { k += OWN[a]; } else if a == 0 { k += OPP[n]; }
    }
    k
}

fn sort_moves(g: &Geo, b: &[i8; NC], mv: &mut [usize], p: i8) {
    let n = mv.len();
    let mut keys = [0i32; NC];
    for i in 0..n { keys[i] = order_key(g, b, mv[i], p); }
    for i in 1..n {
        let (x, k) = (mv[i], keys[i]);
        let mut j = i as i32 - 1;
        while j >= 0 && keys[j as usize] < k {
            mv[(j + 1) as usize] = mv[j as usize];
            keys[(j + 1) as usize] = keys[j as usize];
            j -= 1;
        }
        mv[(j + 1) as usize] = x;
        keys[(j + 1) as usize] = k;
    }
}

#[derive(PartialEq, Clone, Copy)]
enum Kind { Win, Draw, Forced, Free, Lost }

struct Analysis { kind: Kind, n: usize, safe_p: i32, safe_o: i32, mv: [usize; NC], loses: [bool; NC] }

fn analyse(g: &Geo, b: &[i8; NC], p: i8) -> Analysis {
    let o = 3 - p;
    let mut a = Analysis { kind: Kind::Free, n: 0, safe_p: 0, safe_o: 0, mv: [0; NC], loses: [false; NC] };
    let mut forced = [0usize; NC];
    let mut fl = [false; NC];
    let mut free = [0usize; NC];
    let (mut nf, mut nfree, mut safe_o, mut empty) = (0, 0, 0, 0);
    for c in 0..NC {
        if b[c] != 0 { continue; }
        empty += 1;
        let rp = place_result(g, b, c, p);
        if rp == 2 { a.kind = Kind::Win; a.n = 1; a.mv[0] = c; return a; }
        let ro = place_result(g, b, c, o);
        if ro == 2 { forced[nf] = c; fl[nf] = rp == 1; nf += 1; } else if ro == 0 { safe_o += 1; }
        if rp == 0 { free[nfree] = c; nfree += 1; }
    }
    a.safe_p = nfree as i32;
    a.safe_o = safe_o;
    if empty == 0 { a.kind = Kind::Draw; return a; }
    if nf > 0 {
        a.kind = Kind::Forced; a.n = nf;
        for i in 0..nf { a.mv[i] = forced[i]; a.loses[i] = fl[i]; }
        return a;
    }
    if nfree == 0 { a.kind = Kind::Lost; return a; }
    a.kind = Kind::Free; a.n = nfree;
    for i in 0..nfree { a.mv[i] = free[i]; }
    a
}

struct Search<'a> {
    g: &'a Geo,
    limit: i64,
    nodes: i64,
    aborted: bool,
    last_depth: i32,
    t0: Instant,
    time_ms: u128,
}

impl<'a> Search<'a> {
    fn negamax(&mut self, b: &mut [i8; NC], p: i8, depth: i32, mut alpha: i32, beta: i32, ply: i32) -> i32 {
        self.nodes += 1;
        if self.nodes > self.limit { self.aborted = true; return 0; }
        if (self.nodes & 255) == 0 && self.t0.elapsed().as_millis() > self.time_ms {
            eprintln!("TIMEGUARD");
            self.aborted = true;
            return 0;
        }
        let mut a = analyse(self.g, b, p);
        match a.kind {
            Kind::Win => return WIN - ply,
            Kind::Draw => return 0,
            Kind::Lost => return -(WIN - ply - 1),
            _ => {}
        }
        let mut best = -INF;
        if a.kind == Kind::Forced {
            for i in 0..a.n {
                let c = a.mv[i];
                let v;
                if a.loses[i] {
                    v = -(WIN - ply - 1);
                } else {
                    b[c] = p;
                    v = -self.negamax(b, 3 - p, depth, -beta, -alpha, ply + 1);
                    b[c] = 0;
                    if self.aborted { return 0; }
                }
                if v > best { best = v; }
                if v > alpha { alpha = v; }
                if alpha >= beta { break; }
            }
            return best;
        }
        if depth <= 0 { return evaluate(self.g, b, p, a.safe_p, a.safe_o); }
        let n = a.n;
        sort_moves(self.g, b, &mut a.mv[..n], p);
        for i in 0..n {
            let c = a.mv[i];
            b[c] = p;
            let v = -self.negamax(b, 3 - p, depth - 1, -beta, -alpha, ply + 1);
            b[c] = 0;
            if self.aborted { return 0; }
            if v > best { best = v; }
            if v > alpha { alpha = v; }
            if alpha >= beta { break; }
        }
        best
    }

    fn run(&mut self, b: &mut [i8; NC], p: i8, mv: &mut Vec<usize>) -> usize {
        sort_moves(self.g, b, &mut mv[..], p);
        let mut best = mv[0];
        for depth in 1..=MAX_DEPTH {
            let (mut alpha, mut best_v, mut cur_i) = (-INF, -INF, 0usize);
            for i in 0..mv.len() {
                b[mv[i]] = p;
                let v = -self.negamax(b, 3 - p, depth - 1, -INF, -alpha, 1);
                b[mv[i]] = 0;
                if self.aborted { break; }
                if v > best_v { best_v = v; cur_i = i; }
                if v > alpha { alpha = v; }
            }
            if self.aborted { break; }
            best = mv[cur_i];
            self.last_depth = depth;
            let pv = mv.remove(cur_i);
            mv.insert(0, pv);
            if best_v >= WIN - MAX_DEPTH || best_v <= -(WIN - MAX_DEPTH) { break; }
        }
        best
    }
}

// returns Some(cell), or None = steal the opponent's first stone
fn choose(g: &Geo, b: &mut [i8; NC], my_id: i32, first_turn: bool, last: i32, limit: i64, t0: Instant, time_ms: u128) -> (Option<usize>, String) {
    let stones = b.iter().filter(|&&v| v != 0).count();
    if stones == 0 { return (Some(OPENING), "opening".into()); }
    if my_id == 2 && first_turn && stones == 1 && last >= 0 {
        let l = last as usize;
        let (q, r) = (qof(g.cx[l], g.cy[l]) as i32, g.cy[l] as i32);
        let dist = (q - 4).abs().max((r - 4).abs()).max((q + r - 8).abs());
        if dist <= 2 { return (None, "steal".into()); }
    }
    let a = analyse(g, b, 1);
    let mut cand: Vec<usize> = Vec::new();
    match a.kind {
        Kind::Win => return (Some(a.mv[0]), "win".into()),
        Kind::Forced => {
            for i in 0..a.n { if !a.loses[i] { cand.push(a.mv[i]); } }
            if cand.len() <= 1 {
                let c = if cand.is_empty() { a.mv[0] } else { cand[0] };
                return (Some(c), "forced".into());
            }
        }
        Kind::Free => { for i in 0..a.n { cand.push(a.mv[i]); } }
        _ => {
            let c = (0..NC).find(|&c| b[c] == 0).unwrap();
            return (Some(c), "lost".into());
        }
    }
    let mut s = Search { g, limit, nodes: 0, aborted: false, last_depth: 0, t0, time_ms };
    let c = s.run(b, 1, &mut cand);
    (Some(c), format!("d={} n={}", s.last_depth, s.nodes))
}

fn main() {
    let g = init_geo();
    let cli: i64 = std::env::var("CG_NODES").ok().and_then(|v| v.trim().parse().ok()).unwrap_or(-1);
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();
    let out = io::stdout();
    let (mut my_id, mut turn, mut first_line) = (0i32, 0, true);
    while let Some(Ok(mut line)) = lines.next() {
        if line.trim().is_empty() { continue; }
        let t0 = Instant::now();
        if first_line {
            my_id = line.trim().split_whitespace().next().unwrap_or("0").parse().unwrap_or(0);
            first_line = false;
            line = match lines.next() { Some(Ok(l)) => l, _ => break };
        }
        let count: usize = line.trim().split_whitespace().next().unwrap_or("0").parse().unwrap_or(0);
        let mut b = [0i8; NC];
        let mut k = 0;
        for _ in 0..count {
            let row = match lines.next() { Some(Ok(l)) => l, _ => String::new() };
            for ch in row.bytes() {
                if ch == b' ' || ch == b'\r' || ch == b'\t' || ch == b'\n' { continue; }
                if k < NC { b[k] = if ch == b'1' { 1 } else if ch == b'2' { 2 } else { 0 }; }
                k += 1;
            }
        }
        let ol = match lines.next() { Some(Ok(l)) => l, _ => String::new() };
        let v: Vec<i32> = ol.split_whitespace().filter_map(|t| t.parse().ok()).collect();
        let (ox, oy) = (*v.get(0).unwrap_or(&-1), *v.get(1).unwrap_or(&-1));
        let mut last = -1i32;
        if oy >= 0 && oy < 9 && ox >= 0 && (ox as usize) < g.rows[oy as usize] {
            last = g.idx[qof(ox as usize, oy as usize)][oy as usize];
        }
        let limit = if cli > 0 { cli } else if turn == 0 { 140000 } else { 14000 };
        let time_ms: u128 = if turn == 0 { 700 } else { 70 };
        let (c, info) = choose(&g, &mut b, my_id, turn == 0, last, limit, t0, time_ms);
        let c = c.unwrap_or(last as usize);
        eprintln!("{} ms={}", info, t0.elapsed().as_millis());
        let mut o = out.lock();
        writeln!(o, "{} {}", g.cx[c], g.cy[c]).unwrap();
        o.flush().unwrap();
        turn += 1;
    }
}
