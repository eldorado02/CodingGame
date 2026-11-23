// Nine Men's Morris — Rust port of solutions/python/bot.py (v1 alpha-beta).
// Mills = aligned triples derived from the adjacency given at init.
use std::collections::{BTreeSet, HashMap};
use std::io::{self, BufRead, BufWriter, Write};
use std::time::Instant;

const STONES: i32 = 9;
const TURN_NODES: i64 = 15_000;
const TURN_MS: u128 = 35;
const STONE_VALUE: i32 = 100;
const MILL_VALUE: i32 = 10;
const EMPTY: i32 = -1;
const WIN: i32 = 1_000_000;
const INF: i32 = 1 << 30;
const MAX_DEPTH: i32 = 64;

#[derive(Clone, Copy, PartialEq, Eq, Hash)]
struct Move { src: i32, dst: i32, take: i32 } // src = -1 placement, take = -1 no mill

#[derive(Clone)]
struct State { owner: Vec<i32>, hand: [i32; 2], side: i32 }

struct Board { adj: Vec<Vec<usize>>, mills: Vec<[usize; 3]>, mills_of: Vec<Vec<usize>> }

impl Board {
    fn new(names: &[String], adjacency: &[Vec<usize>]) -> Board {
        let aligned = |a: usize, b: usize, c: usize| {
            let (x, y, z) = (&names[a], &names[b], &names[c]);
            (x[..1] == y[..1] && y[..1] == z[..1]) || (x[1..] == y[1..] && y[1..] == z[1..])
        };
        let mut set = BTreeSet::new();
        for b in 0..names.len() {
            for &a in adjacency[b].iter() {
                for &c in adjacency[b].iter() {
                    if a < c && aligned(a, b, c) { set.insert([a, b, c]); }
                }
            }
        }
        let mills: Vec<[usize; 3]> = set.into_iter().collect();
        let mut mills_of = vec![Vec::new(); names.len()];
        for (k, m) in mills.iter().enumerate() { for &i in m.iter() { mills_of[i].push(k); } }
        Board { adj: adjacency.to_vec(), mills, mills_of }
    }

    fn in_mill(&self, owner: &[i32], i: usize) -> bool {
        let p = owner[i];
        self.mills_of[i].iter().any(|&k| self.mills[k].iter().all(|&j| owner[j] == p))
    }

    fn take_targets(&self, owner: &[i32], enemy: i32) -> Vec<usize> {
        let stones: Vec<usize> = (0..owner.len()).filter(|&i| owner[i] == enemy).collect();
        let free: Vec<usize> = stones.iter().copied().filter(|&i| !self.in_mill(owner, i)).collect();
        if free.is_empty() { stones } else { free }
    }

    fn gen_moves(&self, s: &State) -> Vec<Move> {
        let p = s.side;
        let n = s.owner.len();
        let mut steps: Vec<(i32, usize)> = Vec::new();
        if s.hand[p as usize] > 0 {
            steps.extend((0..n).filter(|&d| s.owner[d] == EMPTY).map(|d| (-1, d)));
        } else {
            let mine: Vec<usize> = (0..n).filter(|&i| s.owner[i] == p).collect();
            for &src in mine.iter() {
                if mine.len() == 3 { steps.extend((0..n).filter(|&d| s.owner[d] == EMPTY).map(|d| (src as i32, d))); }
                else { steps.extend(self.adj[src].iter().filter(|&&d| s.owner[d] == EMPTY).map(|&d| (src as i32, d))); }
            }
        }
        let mut out = Vec::new();
        for (src, dst) in steps {
            let mut after = s.owner.clone();
            if src >= 0 { after[src as usize] = EMPTY; }
            after[dst] = p;
            if self.in_mill(&after, dst) {
                for t in self.take_targets(&after, 1 - p) { out.push(Move { src, dst: dst as i32, take: t as i32 }); }
            } else {
                out.push(Move { src, dst: dst as i32, take: -1 });
            }
        }
        out
    }

    fn side_score(&self, s: &State, p: i32) -> i32 {
        let mills = self.mills.iter().filter(|m| m.iter().all(|&j| s.owner[j] == p)).count() as i32;
        let mobility: i32 = (0..s.owner.len()).filter(|&i| s.owner[i] == p)
            .map(|i| self.adj[i].iter().filter(|&&d| s.owner[d] == EMPTY).count() as i32).sum();
        STONE_VALUE * material(s, p) + MILL_VALUE * mills + mobility
    }

    fn evaluate(&self, s: &State) -> i32 { self.side_score(s, s.side) - self.side_score(s, 1 - s.side) }
}

fn material(s: &State, p: i32) -> i32 { s.hand[p as usize] + s.owner.iter().filter(|&&o| o == p).count() as i32 }

fn apply_move(s: &State, m: &Move) -> State {
    let mut t = s.clone();
    if m.src >= 0 { t.owner[m.src as usize] = EMPTY; } else { t.hand[s.side as usize] -= 1; }
    t.owner[m.dst as usize] = s.side;
    if m.take >= 0 { t.owner[m.take as usize] = EMPTY; }
    t.side = 1 - s.side;
    t
}

fn order_key(m: &Move) -> i32 { if m.take >= 0 { 1 } else { 0 } }

struct Search<'a> { board: &'a Board, nodes: i64, limit: i64, aborted: bool, t0: Instant }

impl<'a> Search<'a> {
    fn negamax(&mut self, s: &State, depth: i32, mut alpha: i32, beta: i32, ply: i32) -> i32 {
        self.nodes += 1;
        if self.nodes > self.limit { self.aborted = true; return 0; }
        if self.nodes & 255 == 0 && self.t0.elapsed().as_millis() > TURN_MS { eprintln!("TIMEGUARD"); self.aborted = true; return 0; }
        if material(s, s.side) < 3 { return -(WIN - ply); }
        let mut mv = self.board.gen_moves(s);
        if mv.is_empty() { return -(WIN - ply); }
        if depth == 0 { return self.board.evaluate(s); }
        mv.sort_by_key(|m| -order_key(m)); // stable
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
        mv.sort_by_key(|m| -order_key(m));
        let mut best = mv[0];
        for depth in 1..=MAX_DEPTH {
            let (mut alpha, mut best_v, mut cur) = (-INF, -INF, 0usize);
            for (i, m) in mv.iter().enumerate() {
                let v = -self.negamax(&apply_move(root, m), depth - 1, -INF, -alpha, 1);
                if self.aborted { break; }
                if v > best_v { best_v = v; cur = i; }
                if v > alpha { alpha = v; }
            }
            if self.aborted { break; }
            best = mv[cur];
            let pv = mv.remove(cur); // PV first next iteration
            mv.insert(0, pv);
            if best_v >= WIN - MAX_DEPTH || best_v <= -(WIN - MAX_DEPTH) { break; }
        }
        best
    }
}

fn parse_command(index: &HashMap<String, usize>, text: &str) -> Option<Move> {
    let parts: Vec<&str> = text.split(';').map(|p| p.trim()).collect();
    if parts.len() < 2 { return None; }
    let f: Option<Vec<i32>> = parts[1..].iter().map(|p| index.get(*p).map(|&i| i as i32)).collect();
    let f = f?;
    match (parts[0], f.len()) {
        ("PLACE", 1) => Some(Move { src: -1, dst: f[0], take: -1 }),
        ("PLACE&TAKE", 2) => Some(Move { src: -1, dst: f[0], take: f[1] }),
        ("MOVE", 2) => Some(Move { src: f[0], dst: f[1], take: -1 }),
        ("MOVE&TAKE", 3) => Some(Move { src: f[0], dst: f[1], take: f[2] }),
        _ => None,
    }
}

fn main() {
    let limit: i64 = std::env::var("CG_NODES").ok().and_then(|v| v.parse().ok()).unwrap_or(TURN_NODES);
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();
    let mut out = BufWriter::new(io::stdout());
    let me: i32 = match lines.next() { Some(Ok(l)) => l.trim().parse().unwrap_or(0), _ => return };
    let count: usize = match lines.next() { Some(Ok(l)) => l.trim().parse().unwrap_or(0), _ => return };
    let mut names: Vec<String> = Vec::new();
    let mut raw_adj: Vec<Vec<String>> = Vec::new();
    for _ in 0..count {
        let l = match lines.next() { Some(Ok(l)) => l, _ => return };
        let (name, rest) = match l.split_once(':') { Some(p) => p, None => continue };
        names.push(name.trim().to_string());
        raw_adj.push(rest.split(';').map(|x| x.trim().to_string()).filter(|x| !x.is_empty()).collect());
    }
    let index: HashMap<String, usize> = names.iter().enumerate().map(|(i, n)| (n.clone(), i)).collect();
    let adjacency: Vec<Vec<usize>> = raw_adj.iter().map(|v| {
        let mut a: Vec<usize> = v.iter().filter_map(|x| index.get(x).copied()).collect();
        a.sort();
        a
    }).collect();
    let board = Board::new(&names, &adjacency);
    let mut placed = [0i32; 2];
    while let Some(Ok(op_move)) = lines.next() {
        let t0 = Instant::now();
        if op_move.trim().starts_with("PLACE") { placed[(1 - me) as usize] += 1; }
        let mut owner = vec![EMPTY; names.len()];
        let board_line = match lines.next() { Some(Ok(l)) => l, _ => return };
        for item in board_line.split(';') {
            if let Some((name, state)) = item.split_once(':') {
                if let (Some(&i), Ok(v)) = (index.get(name.trim()), state.trim().parse::<i32>()) {
                    if v == 0 || v == 1 { owner[i] = v; }
                }
            }
        }
        let n: usize = match lines.next() { Some(Ok(l)) => l.trim().parse().unwrap_or(0), _ => return };
        let mut commands = Vec::new();
        let mut legal = Vec::new();
        let mut texts: HashMap<Move, String> = HashMap::new();
        for _ in 0..n {
            let c = match lines.next() { Some(Ok(l)) => l.trim().to_string(), _ => return };
            if let Some(m) = parse_command(&index, &c) {
                if !texts.contains_key(&m) { legal.push(m); texts.insert(m, c.clone()); }
            }
            commands.push(c);
        }
        if legal.is_empty() {
            writeln!(out, "{}", commands.first().map(|s| s.as_str()).unwrap_or("PLACE;A1")).ok();
            out.flush().ok();
            continue;
        }
        let s = State { owner, hand: [STONES - placed[0], STONES - placed[1]], side: me };
        let mut search = Search { board: &board, nodes: 0, limit, aborted: false, t0 };
        let m = search.run(&s, legal);
        if m.src < 0 { placed[me as usize] += 1; }
        writeln!(out, "{}", texts[&m]).ok();
        out.flush().ok();
    }
}
