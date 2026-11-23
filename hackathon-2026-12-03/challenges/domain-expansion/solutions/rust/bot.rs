// Domain Expansion 7x7 (CodinGame) — Rust port of solutions/python/bot.py (v1).
// Same rules, action order, evaluation and tie-breaks.
use std::io::{self, BufRead, Write};
use std::time::Instant;

const SIZE: usize = 7;
const CELLS: usize = SIZE * SIZE;
const MAX_MOVE: usize = 3; // Manhattan reach of a move
const PATH_EXPANSION_DEPTH: i32 = 3; // referee BFS expands nodes up to this depth
const LETTERS: [char; 4] = ['U', 'R', 'D', 'L'];
const DX: [i32; 4] = [0, 1, 0, -1];
const DY: [i32; 4] = [-1, 0, 1, 0];
const OPPOSITE: [usize; 4] = [2, 3, 0, 1];
const WIN: i32 = 1_000_000;
const INF: i32 = 1 << 30;
const MAX_DEPTH: i32 = 64;
const DEFAULT_NODES: i64 = 10000; // CodinGame budget (100 ms per turn): ~15 ms local
const FIRST_TURN_NODES: i64 = 60000; // first turn has 1000 ms
const TIME_GUARD_MS: u128 = 40; // 40 % of the limits
const FIRST_TURN_GUARD_MS: u128 = 400;

type Neighbours = [[i32; 4]; CELLS];

fn build_neighbours() -> Neighbours {
    let mut t = [[-1; 4]; CELLS];
    for cell in 0..CELLS {
        for d in 0..4 {
            let (nx, ny) = ((cell % SIZE) as i32 + DX[d], (cell / SIZE) as i32 + DY[d]);
            if nx >= 0 && nx < SIZE as i32 && ny >= 0 && ny < SIZE as i32 {
                t[cell][d] = ny * SIZE as i32 + nx;
            }
        }
    }
    t
}

fn manhattan(a: usize, b: usize) -> usize {
    (a % SIZE).abs_diff(b % SIZE) + (a / SIZE).abs_diff(b / SIZE)
}

#[derive(Clone, Copy)]
struct Action {
    cell: usize,
    dir: usize,
}

#[derive(Clone, Copy)]
struct State {
    walls: [u8; CELLS], // bitmask over U R D L of sides already walled
    tokens: [usize; 2], // 0 = us, 1 = them
}

impl State {
    fn new(nb: &Neighbours, mine: usize, theirs: usize) -> State {
        let mut walls = [0u8; CELLS];
        for c in 0..CELLS {
            for d in 0..4 {
                if nb[c][d] < 0 {
                    walls[c] |= 1 << d;
                }
            }
        }
        State { walls, tokens: [mine, theirs] }
    }

    fn walled(&self, cell: usize, d: usize) -> bool {
        (self.walls[cell] >> d) & 1 == 1
    }

    fn apply(&mut self, nb: &Neighbours, player: usize, cell: usize, d: usize) {
        self.tokens[player] = cell;
        self.walls[cell] |= 1 << d;
        self.walls[nb[cell][d] as usize] |= 1 << OPPOSITE[d];
    }
}

/// Cells the token of `player` may move to, ascending (referee findPath).
fn destinations(nb: &Neighbours, s: &State, player: usize) -> Vec<usize> {
    let (start, blocker) = (s.tokens[player], s.tokens[1 - player]);
    let mut depth = [-1i32; CELLS];
    let mut queue = Vec::with_capacity(CELLS);
    depth[start] = 0;
    queue.push(start);
    let mut head = 0;
    while head < queue.len() {
        let cell = queue[head];
        head += 1;
        if depth[cell] > PATH_EXPANSION_DEPTH {
            continue;
        }
        for d in 0..4 {
            if s.walled(cell, d) {
                continue;
            }
            let next = nb[cell][d] as usize;
            if next == blocker || depth[next] >= 0 {
                continue;
            }
            depth[next] = depth[cell] + 1;
            queue.push(next);
        }
    }
    (0..CELLS).filter(|&c| depth[c] >= 0 && manhattan(start, c) <= MAX_MOVE).collect()
}

fn generate_actions(nb: &Neighbours, s: &State, player: usize) -> Vec<Action> {
    let mut out = Vec::new();
    for cell in destinations(nb, s, player) {
        for dir in 0..4 {
            if !s.walled(cell, dir) {
                out.push(Action { cell, dir });
            }
        }
    }
    out
}

/// BFS steps through walls only (tokens do not block territory); -1 = unreachable.
fn distances(nb: &Neighbours, s: &State, start: usize) -> [i32; CELLS] {
    let mut dist = [-1i32; CELLS];
    let mut queue = [0usize; CELLS];
    let (mut head, mut tail) = (0, 1);
    dist[start] = 0;
    queue[0] = start;
    while head < tail {
        let cell = queue[head];
        head += 1;
        for d in 0..4 {
            if s.walled(cell, d) {
                continue;
            }
            let next = nb[cell][d] as usize;
            if dist[next] < 0 {
                dist[next] = dist[cell] + 1;
                queue[tail] = next;
                tail += 1;
            }
        }
    }
    dist
}

fn reachable_count(dist: &[i32; CELLS]) -> i32 {
    dist.iter().filter(|&&v| v >= 0).count() as i32
}

/// None while the tokens are connected, else the outcome for `player`.
fn final_score(nb: &Neighbours, s: &State, player: usize) -> Option<i32> {
    let mine = distances(nb, s, s.tokens[player]);
    if mine[s.tokens[1 - player]] >= 0 {
        return None;
    }
    let theirs = distances(nb, s, s.tokens[1 - player]);
    let diff = reachable_count(&mine) - reachable_count(&theirs);
    Some(if diff > 0 { WIN + diff } else if diff < 0 { -WIN + diff } else { 0 })
}

/// Voronoi territory of `player` minus the opponent's.
fn evaluate(nb: &Neighbours, s: &State, player: usize) -> i32 {
    let mine = distances(nb, s, s.tokens[player]);
    let theirs = distances(nb, s, s.tokens[1 - player]);
    let mut score = 0;
    for c in 0..CELLS {
        let (a, b) = (mine[c], theirs[c]);
        if a < 0 && b < 0 {
            continue;
        }
        if b < 0 || (a >= 0 && a < b) {
            score += 1;
        } else if a < 0 || b < a {
            score -= 1;
        }
    }
    score
}

// ---------------------------------------------------------------- search
struct Search<'a> {
    nb: &'a Neighbours,
    node_limit: i64,
    nodes: i64,
    aborted: bool,
    last_depth: i32,
    t0: Instant,
    guard_ms: u128,
}

impl<'a> Search<'a> {
    /// Value of `a` for `player`, searching depth - 1 below it.
    fn action_value(&mut self, s: &State, player: usize, a: Action, depth: i32, alpha: i32, beta: i32) -> i32 {
        let mut child = *s;
        child.apply(self.nb, player, a.cell, a.dir);
        match final_score(self.nb, &child, player) {
            Some(v) => v,
            None => -self.negamax(&child, 1 - player, depth - 1, -beta, -alpha),
        }
    }

    fn negamax(&mut self, s: &State, player: usize, depth: i32, mut alpha: i32, beta: i32) -> i32 {
        self.nodes += 1;
        if self.nodes > self.node_limit {
            self.aborted = true;
            return 0;
        }
        if (self.nodes & 63) == 0 && self.t0.elapsed().as_millis() > self.guard_ms {
            eprintln!("TIMEGUARD");
            self.aborted = true;
            return 0;
        }
        if depth == 0 {
            return evaluate(self.nb, s, player);
        }
        let mut best = -INF;
        for a in generate_actions(self.nb, s, player) {
            let value = self.action_value(s, player, a, depth, alpha, beta);
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

    /// Root actions sorted by 1-ply value (stable insertion sort).
    fn ordered_root(&mut self, s: &State, player: usize) -> Vec<Action> {
        let mut acts = generate_actions(self.nb, s, player);
        let mut keys: Vec<i32> = Vec::with_capacity(acts.len());
        for i in 0..acts.len() {
            let a = acts[i];
            keys.push(self.action_value(s, player, a, 1, -INF, INF));
        }
        for i in 1..acts.len() {
            let mut j = i;
            while j > 0 && keys[j - 1] < keys[j] {
                acts.swap(j, j - 1);
                keys.swap(j, j - 1);
                j -= 1;
            }
        }
        acts
    }

    fn run(&mut self, s: &State, player: usize) -> Action {
        let mut acts = self.ordered_root(s, player);
        let mut best = acts[0];
        for depth in 2..=MAX_DEPTH {
            let (mut alpha, mut best_value, mut best_index) = (-INF, -INF, 0usize);
            for i in 0..acts.len() {
                let value = self.action_value(s, player, acts[i], depth, alpha, INF);
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
            best = acts[best_index];
            self.last_depth = depth;
            let pv = acts.remove(best_index);
            acts.insert(0, pv);
            if best_value >= WIN || best_value <= -WIN {
                break;
            }
        }
        best
    }
}

// ---------------------------------------------------------------- I/O
fn read_pair(line: &str) -> (usize, usize) {
    let v: Vec<usize> = line.split_whitespace().filter_map(|t| t.parse().ok()).collect();
    (v[0], v[1])
}

fn main() {
    let nb = build_neighbours();
    let cli_nodes: Option<i64> = std::env::var("CG_NODES").ok().and_then(|v| v.trim().parse().ok());
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines().map(|l| l.unwrap_or_default());
    if lines.next().is_none() {
        return;
    }
    let (mx, my) = read_pair(&lines.next().unwrap_or_default());
    let (ox, oy) = read_pair(&lines.next().unwrap_or_default());
    let mut s = State::new(&nb, my * SIZE + mx, oy * SIZE + ox);
    let out = io::stdout();
    let mut first_turn = true;
    for line in lines {
        let t0 = Instant::now();
        let parts: Vec<&str> = line.split_whitespace().collect();
        if parts.len() >= 3 {
            if let Some(d) = LETTERS.iter().position(|&c| parts[2] == c.to_string()) {
                let (ax, ay): (usize, usize) = (parts[0].parse().unwrap_or(0), parts[1].parse().unwrap_or(0));
                s.apply(&nb, 1, ay * SIZE + ax, d);
            }
        }
        let node_limit = cli_nodes.unwrap_or(if first_turn { FIRST_TURN_NODES } else { DEFAULT_NODES });
        let guard_ms = if first_turn { FIRST_TURN_GUARD_MS } else { TIME_GUARD_MS };
        let mut search = Search { nb: &nb, node_limit, nodes: 0, aborted: false, last_depth: 0, t0, guard_ms };
        let a = search.run(&s, 0);
        s.apply(&nb, 0, a.cell, a.dir);
        eprintln!("d={} n={} ms={}", search.last_depth, search.nodes, t0.elapsed().as_millis());
        let mut o = out.lock();
        writeln!(o, "{} {} {}", a.cell % SIZE, a.cell / SIZE, LETTERS[a.dir]).unwrap();
        o.flush().unwrap();
        first_turn = false;
    }
}
