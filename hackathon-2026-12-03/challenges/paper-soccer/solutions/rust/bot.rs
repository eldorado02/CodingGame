// Paper Soccer 8x10 (CodinGame) — Rust port of solutions/python/bot.py (v1).
// Same geometry, turn generation, evaluation, move order and tie-breaks.
use std::io::{self, BufRead, Write};
use std::time::Instant;

const WIDTH: i32 = 8;
const HEIGHT: i32 = 10;
const COLS: i32 = WIDTH + 1;
const ROWS: i32 = HEIGHT + 1;
const FIELD_POINTS: usize = (COLS * ROWS) as usize;
const TOP_GOAL: usize = FIELD_POINTS;
const BOTTOM_GOAL: usize = FIELD_POINTS + 3;
const POINTS: usize = FIELD_POINTS + 6;
const GOAL_X0: i32 = 3;
const GOAL_WIDTH: i32 = 3;
const CENTER_X: i32 = WIDTH / 2;
const START_BALL: usize = ((HEIGHT / 2) * COLS + CENTER_X) as usize;
const DX: [i32; 8] = [0, 1, 1, 1, 0, -1, -1, -1];
const DY: [i32; 8] = [-1, -1, 0, 1, 1, 1, 0, -1];
const PLAYER_TOP: usize = 0;
const PLAYER_BOTTOM: usize = 1;
const NORMAL: usize = 0;
const WON: usize = 1;
const LOST: usize = 2;
const OUTCOMES: usize = 3;

const WIN: i32 = 1_000_000;
const INF: i32 = 1 << 30;
const MAX_DEPTH: i32 = 64;
const ROW_WEIGHT: i32 = 80; // one row of vertical progress
const GOAL_DISTANCE_WEIGHT: i32 = 120; // unused-edge path to goal (Lecorbio evaluator)
const NO_PATH: i32 = 40; // goal distance when every route is drawn
const CENTRE_WEIGHT: i32 = 15; // per column away from the goal mouth
const CENTRE_ZONE_ROWS: i32 = 3; // centring matters this close to a goal
const MAX_DFS_STEPS: i32 = 20000; // bounce-sequence enumeration cap per node
const DEFAULT_NODES: i64 = 3000; // CodinGame budget (200 ms per turn)
const FIRST_TURN_NODES: i64 = 15000; // first turn has 1000 ms
const TIME_GUARD_MS: u128 = 80; // 40 % of 200 ms: late nodes are slow
const FIRST_TURN_GUARD_MS: u128 = 400;

// ---------------------------------------------------------------- geometry
struct Geometry {
    neighbour: [[i32; 8]; POINTS],
    degree: [i32; POINTS],
}

fn point_index(x: i32, y: i32) -> i32 {
    if x >= 0 && x < COLS && y >= 0 && y < ROWS {
        return y * COLS + x;
    }
    let in_mouth = x >= GOAL_X0 && x < GOAL_X0 + GOAL_WIDTH;
    if in_mouth && y == -1 {
        return TOP_GOAL as i32 + x - GOAL_X0;
    }
    if in_mouth && y == ROWS {
        return BOTTOM_GOAL as i32 + x - GOAL_X0;
    }
    -1
}

fn point_xy(p: usize) -> (i32, i32) {
    if p < FIELD_POINTS {
        ((p as i32) % COLS, (p as i32) / COLS)
    } else if p < BOTTOM_GOAL {
        (GOAL_X0 + (p - TOP_GOAL) as i32, -1)
    } else {
        (GOAL_X0 + (p - BOTTOM_GOAL) as i32, ROWS)
    }
}

/// Goal posts: the ball cannot enter the goal diagonally from outside the mouth.
fn is_removed_pair(p: i32, q: i32) -> bool {
    const PAIRS: [[i32; 4]; 4] = [[2, 0, 3, -1], [6, 0, 5, -1], [2, 10, 3, 11], [6, 10, 5, 11]];
    PAIRS.iter().any(|r| {
        let (a, b) = (point_index(r[0], r[1]), point_index(r[2], r[3]));
        (p == a && q == b) || (p == b && q == a)
    })
}

fn build_geometry() -> Geometry {
    let mut g = Geometry { neighbour: [[-1; 8]; POINTS], degree: [0; POINTS] };
    for p in 0..POINTS {
        let (x, y) = point_xy(p);
        for d in 0..8 {
            let q = point_index(x + DX[d], y + DY[d]);
            if q >= 0 && !is_removed_pair(p as i32, q) {
                g.neighbour[p][d] = q;
                g.degree[p] += 1;
            }
        }
    }
    g
}

fn goal_owner(p: usize) -> Option<usize> {
    if p >= TOP_GOAL && p < BOTTOM_GOAL {
        Some(PLAYER_TOP)
    } else if p >= BOTTOM_GOAL {
        Some(PLAYER_BOTTOM)
    } else {
        None
    }
}

// ---------------------------------------------------------------- pitch
#[derive(Clone)]
struct Pitch {
    drawn: [u8; POINTS], // bitmask of directions already drawn from each point
    free_edges: [i32; POINTS],
    ball: usize,
}

impl Pitch {
    /// Border lines and goal frames, as Pitch() draws them.
    fn new(g: &Geometry) -> Pitch {
        let mut s = Pitch { drawn: [0; POINTS], free_edges: g.degree, ball: START_BALL };
        let mut pairs: Vec<(i32, i32, i32, i32)> = Vec::new();
        for y in 0..HEIGHT {
            pairs.push((0, y, 0, y + 1));
            pairs.push((WIDTH, y, WIDTH, y + 1));
        }
        for x in 0..WIDTH {
            if x < GOAL_X0 || x >= GOAL_X0 + GOAL_WIDTH - 1 {
                pairs.push((x, 0, x + 1, 0));
                pairs.push((x, HEIGHT, x + 1, HEIGHT));
            }
        }
        for (gy, fy) in [(-1, 0), (ROWS, HEIGHT)] {
            pairs.extend([(3, gy, 4, gy), (4, gy, 5, gy), (3, gy, 3, fy), (5, gy, 5, fy)]);
        }
        for (x1, y1, x2, y2) in pairs {
            let (a, b) = (point_index(x1, y1) as usize, point_index(x2, y2));
            let d = (0..8).find(|&d| g.neighbour[a][d] == b).unwrap();
            s.draw(g, a, d);
        }
        s
    }

    fn draw(&mut self, g: &Geometry, p: usize, d: usize) -> usize {
        let q = g.neighbour[p][d] as usize;
        self.drawn[p] |= 1 << d;
        self.drawn[q] |= 1 << ((d + 4) % 8);
        self.free_edges[p] -= 1;
        self.free_edges[q] -= 1;
        q
    }

    fn undraw(&mut self, g: &Geometry, p: usize, d: usize) {
        let q = g.neighbour[p][d] as usize;
        self.drawn[p] &= !(1 << d);
        self.drawn[q] &= !(1 << ((d + 4) % 8));
        self.free_edges[p] += 1;
        self.free_edges[q] += 1;
    }

    fn can_move(&self, g: &Geometry, d: usize) -> bool {
        g.neighbour[self.ball][d] >= 0 && (self.drawn[self.ball] >> d) & 1 == 0
    }

    fn apply(&mut self, g: &Geometry, digits: &str) {
        for ch in digits.bytes() {
            let d = ch.wrapping_sub(b'0') as usize;
            if d < 8 && self.can_move(g, d) {
                self.ball = self.draw(g, self.ball, d);
            }
        }
    }
}

// ---------------------------------------------------------------- turn generation
#[derive(Clone)]
struct Turn {
    digits: String,
    end: usize,
    outcome: usize,
}

struct TurnGenerator<'a> {
    g: &'a Geometry,
    mover: usize,
    steps: i32,
    path: Vec<u8>,
    seen: [[bool; OUTCOMES]; POINTS],
    out: Vec<Turn>,
}

impl<'a> TurnGenerator<'a> {
    fn emit(&mut self, q: usize, outcome: usize) {
        if self.seen[q][outcome] {
            return;
        }
        self.seen[q][outcome] = true;
        let digits = String::from_utf8(self.path.clone()).unwrap();
        self.out.push(Turn { digits, end: q, outcome });
    }

    fn dfs(&mut self, pitch: &mut Pitch) {
        let p = pitch.ball;
        for d in 0..8 {
            if self.steps >= MAX_DFS_STEPS {
                return;
            }
            if !pitch.can_move(self.g, d) {
                continue;
            }
            self.steps += 1;
            let q = pitch.draw(self.g, p, d);
            pitch.ball = q;
            self.path.push(b'0' + d as u8);
            let bounce = self.g.degree[q] - pitch.free_edges[q] > 1;
            if let Some(owner) = goal_owner(q) {
                self.emit(q, if owner == self.mover { WON } else { LOST });
            } else if pitch.free_edges[q] == 0 {
                self.emit(q, LOST);
            } else if bounce {
                self.dfs(pitch);
            } else {
                self.emit(q, NORMAL);
            }
            self.path.pop();
            pitch.ball = p;
            pitch.undraw(self.g, p, d);
        }
    }
}

/// Complete turns from pitch.ball, first sequence per (end point, outcome).
fn generate_turns(g: &Geometry, pitch: &mut Pitch, mover: usize) -> Vec<Turn> {
    let mut gen = TurnGenerator { g, mover, steps: 0, path: Vec::new(), seen: [[false; OUTCOMES]; POINTS], out: Vec::new() };
    gen.dfs(pitch);
    gen.out
}

// ---------------------------------------------------------------- evaluation
fn rows_to_goal(player: usize, p: usize) -> i32 {
    let (_, y) = point_xy(p);
    if player == PLAYER_TOP { y + 1 } else { ROWS - y }
}

/// Edges the ball needs, over unused edges only, to reach the goal `player` attacks.
fn goal_distance(g: &Geometry, pitch: &Pitch, player: usize) -> i32 {
    let target_lo = if player == PLAYER_TOP { TOP_GOAL } else { BOTTOM_GOAL };
    let mut dist = [NO_PATH; POINTS];
    let mut queue = Vec::with_capacity(POINTS);
    dist[pitch.ball] = 0;
    queue.push(pitch.ball);
    let mut head = 0;
    while head < queue.len() {
        let p = queue[head];
        head += 1;
        if p >= target_lo && p < target_lo + GOAL_WIDTH as usize {
            return dist[p];
        }
        if goal_owner(p).is_some() {
            continue; // the ball stops in any goal
        }
        for d in 0..8 {
            let q = g.neighbour[p][d];
            if q >= 0 && (pitch.drawn[p] >> d) & 1 == 0 && dist[q as usize] == NO_PATH {
                dist[q as usize] = dist[p] + 1;
                queue.push(q as usize);
            }
        }
    }
    NO_PATH
}

/// Score for `player`, who is about to move with the ball at pitch.ball.
fn evaluate(g: &Geometry, pitch: &Pitch, player: usize) -> i32 {
    let (x, _) = point_xy(pitch.ball);
    let mine = rows_to_goal(player, pitch.ball);
    let theirs = rows_to_goal(1 - player, pitch.ball);
    let mut score = ROW_WEIGHT * (theirs - mine);
    score += GOAL_DISTANCE_WEIGHT * (goal_distance(g, pitch, 1 - player) - goal_distance(g, pitch, player));
    let off_centre = (x - CENTER_X).abs();
    if mine <= CENTRE_ZONE_ROWS {
        score -= CENTRE_WEIGHT * off_centre;
    }
    if theirs <= CENTRE_ZONE_ROWS {
        score += CENTRE_WEIGHT * off_centre;
    }
    score
}

fn order_key(t: &Turn, mover: usize) -> i32 {
    match t.outcome {
        WON => WIN,
        LOST => -WIN,
        _ => -rows_to_goal(mover, t.end),
    }
}

/// Stable insertion sort, best first (identical in the 4 languages).
fn sort_turns(turns: &mut Vec<Turn>, mover: usize) {
    let mut keys: Vec<i32> = turns.iter().map(|t| order_key(t, mover)).collect();
    for i in 1..turns.len() {
        let key = keys[i];
        let mut j = i;
        while j > 0 && keys[j - 1] < key {
            turns.swap(j, j - 1);
            keys.swap(j, j - 1);
            j -= 1;
        }
    }
}

// ---------------------------------------------------------------- search
struct Search<'a> {
    g: &'a Geometry,
    node_limit: i64,
    nodes: i64,
    aborted: bool,
    last_depth: i32,
    t0: Instant,
    guard_ms: u128,
}

impl<'a> Search<'a> {
    fn child(&self, pitch: &Pitch, t: &Turn) -> Pitch {
        let mut next = pitch.clone();
        next.apply(self.g, &t.digits);
        next
    }

    fn negamax(&mut self, pitch: &mut Pitch, player: usize, depth: i32, mut alpha: i32, beta: i32, ply: i32) -> i32 {
        self.nodes += 1;
        if self.nodes > self.node_limit {
            self.aborted = true;
            return 0;
        }
        if self.t0.elapsed().as_millis() > self.guard_ms {
            eprintln!("TIMEGUARD");
            self.aborted = true;
            return 0;
        }
        if depth == 0 {
            return evaluate(self.g, pitch, player);
        }
        let mut turns = generate_turns(self.g, pitch, player);
        if turns.is_empty() {
            return -(WIN - ply);
        }
        sort_turns(&mut turns, player);
        let mut best = -INF;
        for t in &turns {
            if t.outcome == WON {
                return WIN - ply;
            }
            let value = if t.outcome == LOST {
                -(WIN - ply)
            } else {
                let mut next = self.child(pitch, t);
                let v = -self.negamax(&mut next, 1 - player, depth - 1, -beta, -alpha, ply + 1);
                if self.aborted {
                    return 0;
                }
                v
            };
            best = best.max(value);
            alpha = alpha.max(value);
            if alpha >= beta {
                break;
            }
        }
        best
    }

    fn run(&mut self, pitch: &mut Pitch, player: usize) -> String {
        let mut turns = generate_turns(self.g, pitch, player);
        sort_turns(&mut turns, player);
        let mut best = turns[0].clone();
        if best.outcome == WON {
            return best.digits;
        }
        for depth in 1..=MAX_DEPTH {
            let (mut alpha, mut best_value, mut best_index) = (-INF, -INF, 0usize);
            for i in 0..turns.len() {
                let value = if turns[i].outcome == LOST {
                    -WIN
                } else {
                    let mut next = self.child(pitch, &turns[i]);
                    -self.negamax(&mut next, 1 - player, depth - 1, -INF, -alpha, 1)
                };
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
            best = turns[best_index].clone();
            self.last_depth = depth;
            let pv = turns.remove(best_index);
            turns.insert(0, pv);
            if best_value >= WIN - MAX_DEPTH || best_value <= -(WIN - MAX_DEPTH) {
                break;
            }
        }
        best.digits
    }
}

// ---------------------------------------------------------------- I/O
fn main() {
    let g = build_geometry();
    let cli_nodes: Option<i64> = std::env::var("CG_NODES").ok().and_then(|v| v.trim().parse().ok());
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();
    let me: usize = match lines.next() {
        Some(Ok(l)) => l.trim().parse().unwrap_or(0),
        _ => return,
    };
    let out = io::stdout();
    let mut pitch = Pitch::new(&g);
    let mut first_turn = true;
    while let Some(Ok(_length)) = lines.next() {
        let opponent_move = match lines.next() {
            Some(Ok(l)) => l.trim_end().to_string(),
            _ => break,
        };
        let t0 = Instant::now();
        if opponent_move != "-" {
            pitch.apply(&g, &opponent_move);
        }
        let node_limit = cli_nodes.unwrap_or(if first_turn { FIRST_TURN_NODES } else { DEFAULT_NODES });
        let guard_ms = if first_turn { FIRST_TURN_GUARD_MS } else { TIME_GUARD_MS };
        let mut search = Search { g: &g, node_limit, nodes: 0, aborted: false, last_depth: 0, t0, guard_ms };
        let digits = search.run(&mut pitch, me);
        pitch.apply(&g, &digits);
        eprintln!("d={} n={} ms={}", search.last_depth, search.nodes, t0.elapsed().as_millis());
        let mut o = out.lock();
        writeln!(o, "{}", digits).unwrap();
        o.flush().unwrap();
        first_turn = false;
    }
}
