// Isola 9x9 — Rust port of solutions/python/bot.py (v1).
// Negamax alpha-beta over (move, removal) pairs; removals limited to the
// opponent's liberties. Eval = Voronoi + MOBILITY_WEIGHT * mobility diff.
use std::io::{self, BufWriter, Read, Write};
use std::time::Instant;

const W: usize = 9;
const N: usize = 81;
const FIRST_TURN_NODES: i64 = 60_000;
const TURN_NODES: i64 = 10_000;
const FIRST_TURN_MS: u128 = 700;
const TURN_MS: u128 = 70;
const MOBILITY_WEIGHT: i32 = 3;
const WIN: i32 = 1_000_000;
const INF: i32 = 1 << 30;
const MAX_DEPTH: i32 = 64;

#[derive(Clone, Copy)]
struct State { gone: [bool; N], pos: [usize; 2], side: usize }

#[derive(Clone, Copy)]
struct Action { mv: usize, rm: usize }

fn neighbor_table() -> Vec<Vec<usize>> {
    (0..N).map(|i| {
        let (x, y) = ((i % W) as i32, (i / W) as i32);
        let mut out = Vec::new();
        for dy in -1..=1 {
            for dx in -1..=1 {
                let (nx, ny) = (x + dx, y + dy);
                if (dx != 0 || dy != 0) && (0..W as i32).contains(&nx) && (0..W as i32).contains(&ny) {
                    out.push(ny as usize * W + nx as usize);
                }
            }
        }
        out
    }).collect()
}

struct Game { nb: Vec<Vec<usize>> }

impl Game {
    fn is_free(s: &State, t: usize) -> bool { !s.gone[t] && t != s.pos[0] && t != s.pos[1] }

    fn mobility(&self, s: &State, p: usize) -> i32 {
        self.nb[s.pos[p]].iter().filter(|&&t| Self::is_free(s, t)).count() as i32
    }

    fn gen_actions(&self, s: &State) -> Vec<Action> {
        let (me, op) = (s.side, 1 - s.side);
        let mut work = *s;
        let mut out = Vec::new();
        for &t in self.nb[s.pos[me]].iter() {
            if !Self::is_free(s, t) { continue; }
            work.pos[me] = t;
            let before = out.len();
            for &r in self.nb[work.pos[op]].iter() {
                if Self::is_free(&work, r) { out.push(Action { mv: t, rm: r }); }
            }
            if out.len() == before {
                if let Some(r) = (0..N).find(|&r| Self::is_free(&work, r)) { out.push(Action { mv: t, rm: r }); }
            }
        }
        out
    }

    fn bfs(&self, s: &State, start: usize) -> [i32; N] {
        let mut dist = [-1i32; N];
        let mut queue = Vec::with_capacity(N);
        dist[start] = 0;
        queue.push(start);
        let mut head = 0;
        while head < queue.len() {
            let i = queue[head];
            head += 1;
            for &t in self.nb[i].iter() {
                if dist[t] < 0 && Self::is_free(s, t) { dist[t] = dist[i] + 1; queue.push(t); }
            }
        }
        dist
    }

    /// Tiles reached strictly first by the side to move minus by the other.
    fn voronoi(&self, s: &State) -> i32 {
        let mine = self.bfs(s, s.pos[s.side]);
        let theirs = self.bfs(s, s.pos[1 - s.side]);
        let mut score = 0;
        for i in 0..N {
            let (a, b) = (mine[i], theirs[i]);
            if a > 0 && (b < 0 || a < b) { score += 1; } else if b > 0 && (a < 0 || b < a) { score -= 1; }
        }
        score
    }

    fn evaluate(&self, s: &State) -> i32 {
        self.voronoi(s) + MOBILITY_WEIGHT * (self.mobility(s, s.side) - self.mobility(s, 1 - s.side))
    }
}

fn apply_action(s: &State, a: Action) -> State {
    let mut t = *s;
    t.pos[s.side] = a.mv;
    t.gone[a.rm] = true;
    t.side = 1 - s.side;
    t
}

struct Search<'a> { game: &'a Game, nodes: i64, limit: i64, time_ms: u128, aborted: bool, depth: i32, t0: Instant }

impl<'a> Search<'a> {
    fn negamax(&mut self, s: &State, depth: i32, mut alpha: i32, beta: i32, ply: i32) -> i32 {
        self.nodes += 1;
        if self.nodes > self.limit { self.aborted = true; return 0; }
        if self.nodes & 255 == 0 && self.t0.elapsed().as_millis() > self.time_ms {
            eprintln!("TIMEGUARD"); self.aborted = true; return 0;
        }
        if self.game.mobility(s, s.side) == 0 { return -(WIN - ply); }
        if depth == 0 { return self.game.evaluate(s); }
        let mut best = -INF;
        for a in self.game.gen_actions(s) {
            let v = -self.negamax(&apply_action(s, a), depth - 1, -beta, -alpha, ply + 1);
            if self.aborted { return 0; }
            if v > best { best = v; }
            if v > alpha { alpha = v; }
            if alpha >= beta { break; }
        }
        best
    }

    fn run(&mut self, root: &State) -> Action {
        let mut mv = self.game.gen_actions(root);
        let mut best = mv[0];
        for depth in 1..=MAX_DEPTH {
            let (mut alpha, mut best_v, mut cur_idx) = (-INF, -INF, 0usize);
            for (i, &a) in mv.iter().enumerate() {
                let v = -self.negamax(&apply_action(root, a), depth - 1, -INF, -alpha, 1);
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
    let game = Game { nb: neighbor_table() };
    let mut out = BufWriter::new(io::stdout());
    let mut input = String::new();
    // whole-token reader: the protocol is integers only, one per line
    let stdin = io::stdin();
    let mut handle = stdin.lock();
    let mut read_int = move || -> Option<i32> {
        loop {
            if let Some(pos) = input.find(|c: char| !c.is_whitespace()) {
                let rest = &input[pos..];
                if let Some(end) = rest.find(char::is_whitespace) {
                    let tok = rest[..end].to_string();
                    input = rest[end..].to_string();
                    return tok.parse().ok();
                }
            }
            let mut buf = [0u8; 256];
            match handle.read(&mut buf) {
                Ok(0) | Err(_) => {
                    let tok = input.trim().to_string();
                    input.clear();
                    return if tok.is_empty() { None } else { tok.parse().ok() };
                }
                Ok(n) => input.push_str(&String::from_utf8_lossy(&buf[..n])),
            }
        }
    };
    let (mx, my) = match (read_int(), read_int()) { (Some(a), Some(b)) => (a as usize, b as usize), _ => return };
    let me = if mx == 0 { 0 } else { 1 };
    let mut my_pos = my * W + mx;
    let mut s = State { gone: [false; N], pos: [0, 0], side: me };
    let mut first = true;
    loop {
        let vals: Vec<Option<i32>> = (0..4).map(|_| read_int()).collect();
        if vals.iter().any(|v| v.is_none()) { return; }
        let t0 = Instant::now();
        let (ox, oy, rx, ry) = (vals[0].unwrap_or(0), vals[1].unwrap_or(0), vals[2].unwrap_or(-1), vals[3].unwrap_or(-1));
        if (0..W as i32).contains(&rx) && (0..W as i32).contains(&ry) { s.gone[ry as usize * W + rx as usize] = true; }
        s.pos[me] = my_pos;
        s.pos[1 - me] = oy as usize * W + ox as usize;
        s.side = me;
        let limit = cli.unwrap_or(if first { FIRST_TURN_NODES } else { TURN_NODES });
        let time_ms = if first { FIRST_TURN_MS } else { TURN_MS };
        first = false;
        if game.mobility(&s, me) == 0 {
            writeln!(out, "RANDOM").ok();
            out.flush().ok();
            continue;
        }
        let mut search = Search { game: &game, nodes: 0, limit, time_ms, aborted: false, depth: 0, t0 };
        let a = search.run(&s);
        my_pos = a.mv;
        s.gone[a.rm] = true;
        eprintln!("d={} n={} ms={}", search.depth, search.nodes, t0.elapsed().as_millis());
        writeln!(out, "{} {} {} {}", a.mv % W, a.mv / W, a.rm % W, a.rm / W).ok();
        out.flush().ok();
    }
}
