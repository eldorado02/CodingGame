// TwixT-PP 12x12 (CodinGame) — Rust port of solutions/python/bot.py (v1).
// Same geometry, evaluation, move order and tie-breaks.
use std::io::{self, BufRead, Write};
use std::time::Instant;

const SIZE: usize = 12;
const CELLS: usize = SIZE * SIZE;
const EMPTY: i8 = 0;
const ME: usize = 1;
const OPP: usize = 2;
const TOP_BOTTOM: usize = 0;
const LEFT_RIGHT: usize = 1;
const KNIGHT_DELTAS: [(i32, i32); 8] = [(1, 2), (2, 1), (2, -1), (1, -2), (-1, -2), (-2, -1), (-2, 1), (-1, 2)];
const MAX_SEGMENTS: usize = 440;
const UNREACHABLE: i32 = 99;
const WIN_SCORE: i32 = 1_000_000;
const DISTANCE_WEIGHT: i32 = 100; // one peg of path length outweighs every link count
const LINK_WEIGHT: i32 = 3; // more links = more alternative routes
const DEFAULT_TOP_K: usize = 16; // 2-ply candidates (300 ms per turn on CodinGame)
const SWAP_LOW: usize = 3; // swap an opening peg inside [SWAP_LOW, SWAP_HIGH]^2
const SWAP_HIGH: usize = 8;
const OPENING_CELL: usize = 3 * SIZE + 5; // "F4"

fn x_of(cell: usize) -> usize { cell % SIZE }
fn y_of(cell: usize) -> usize { cell / SIZE }
fn cell_of(x: usize, y: usize) -> usize { y * SIZE + x }
fn name_of(cell: usize) -> String { format!("{}{}", (b'A' + x_of(cell) as u8) as char, y_of(cell) + 1) }
fn parse_cell(t: &str) -> usize {
    let b = t.as_bytes();
    cell_of((b[0] - b'A') as usize, t[1..].trim().parse::<usize>().unwrap_or(1) - 1)
}

// ---------------------------------------------------------------- geometry
struct Geometry {
    segments: Vec<(usize, usize)>,
    seg_id: Vec<Vec<i32>>,
    neighbours: Vec<Vec<(usize, usize)>>, // (other cell, segment)
    crosses: Vec<Vec<usize>>,
}

fn orientation(a: usize, b: usize, c: usize) -> i32 {
    let v = (x_of(b) as i64 - x_of(a) as i64) * (y_of(c) as i64 - y_of(a) as i64)
        - (y_of(b) as i64 - y_of(a) as i64) * (x_of(c) as i64 - x_of(a) as i64);
    v.signum() as i32
}

fn bounding_boxes_overlap(s: (usize, usize), t: (usize, usize)) -> bool {
    let span = |p: (usize, usize), f: fn(usize) -> usize| (f(p.0).min(f(p.1)), f(p.0).max(f(p.1)));
    let (sx, tx, sy, ty) = (span(s, x_of), span(t, x_of), span(s, y_of), span(t, y_of));
    sx.0 <= tx.1 && tx.0 <= sx.1 && sy.0 <= ty.1 && ty.0 <= sy.1
}

/// Proper crossing of two segments that share no endpoint.
fn segments_cross(s: (usize, usize), t: (usize, usize)) -> bool {
    if s.0 == t.0 || s.0 == t.1 || s.1 == t.0 || s.1 == t.1 {
        return false;
    }
    orientation(s.0, s.1, t.0) * orientation(s.0, s.1, t.1) < 0
        && orientation(t.0, t.1, s.0) * orientation(t.0, t.1, s.1) < 0
}

fn build_geometry() -> Geometry {
    let mut g = Geometry {
        segments: Vec::new(),
        seg_id: vec![vec![-1; CELLS]; CELLS],
        neighbours: vec![Vec::new(); CELLS],
        crosses: Vec::new(),
    };
    for cell in 0..CELLS {
        for &(dx, dy) in KNIGHT_DELTAS.iter() {
            let (nx, ny) = (x_of(cell) as i32 + dx, y_of(cell) as i32 + dy);
            if nx < 0 || nx >= SIZE as i32 || ny < 0 || ny >= SIZE as i32 {
                continue;
            }
            let other = cell_of(nx as usize, ny as usize);
            let (lo, hi) = (cell.min(other), cell.max(other));
            if g.seg_id[lo][hi] < 0 {
                let id = g.segments.len() as i32;
                g.seg_id[lo][hi] = id;
                g.seg_id[hi][lo] = id;
                g.segments.push((lo, hi));
            }
            g.neighbours[cell].push((other, g.seg_id[lo][hi] as usize));
        }
    }
    g.crosses = vec![Vec::new(); g.segments.len()];
    for i in 0..g.segments.len() {
        for j in i + 1..g.segments.len() {
            let (s, t) = (g.segments[i], g.segments[j]);
            if bounding_boxes_overlap(s, t) && segments_cross(s, t) {
                g.crosses[i].push(j);
                g.crosses[j].push(i);
            }
        }
    }
    g
}

// ---------------------------------------------------------------- state
/// board[cell] in {EMPTY, ME, OPP}; linked[p][seg] = p owns that link;
/// cut[p][seg] = number of opponent links crossing seg (unusable when > 0).
#[derive(Clone)]
struct State {
    side: [usize; 3],
    board: [i8; CELLS],
    linked: [[bool; MAX_SEGMENTS]; 3],
    cut: [[i16; MAX_SEGMENTS]; 3],
    link_count: [i32; 3],
}

impl State {
    fn new(my_side: usize) -> State {
        State {
            side: [0, my_side, 1 - my_side],
            board: [EMPTY; CELLS],
            linked: [[false; MAX_SEGMENTS]; 3],
            cut: [[0; MAX_SEGMENTS]; 3],
            link_count: [0; 3],
        }
    }

    fn add_link(&mut self, g: &Geometry, player: usize, seg: usize) {
        if self.linked[player][seg] {
            return;
        }
        self.linked[player][seg] = true;
        self.link_count[player] += 1;
        let other = if player == ME { OPP } else { ME };
        for &crossed in &g.crosses[seg] {
            self.cut[other][crossed] += 1;
        }
    }

    /// Drop a peg and create every legal link, as the referee does.
    fn place(&mut self, g: &Geometry, player: usize, cell: usize) {
        self.board[cell] = player as i8;
        for &(other, seg) in &g.neighbours[cell] {
            if self.board[other] == player as i8 && self.cut[player][seg] == 0 {
                self.add_link(g, player, seg);
            }
        }
    }

    fn can_play(&self, player: usize, cell: usize) -> bool {
        if self.board[cell] != EMPTY {
            return false;
        }
        let (x, y) = (x_of(cell), y_of(cell));
        let x_edge = x == 0 || x == SIZE - 1;
        let y_edge = y == 0 || y == SIZE - 1;
        if x_edge && y_edge {
            return false; // corners are not holes
        }
        if self.side[player] == TOP_BOTTOM { !x_edge } else { !y_edge }
    }

    fn legal_moves(&self, player: usize) -> Vec<usize> {
        (0..CELLS).filter(|&c| self.can_play(player, c)).collect()
    }
}

// ---------------------------------------------------------------- evaluation
fn on_start_border(side: usize, cell: usize) -> bool {
    if side == TOP_BOTTOM { y_of(cell) == 0 } else { x_of(cell) == 0 }
}

fn on_goal_border(side: usize, cell: usize) -> bool {
    if side == TOP_BOTTOM { y_of(cell) == SIZE - 1 } else { x_of(cell) == SIZE - 1 }
}

/// Fewest pegs `player` still needs to connect his borders (0-1 BFS).
fn path_distance(g: &Geometry, s: &State, player: usize) -> i32 {
    let side = s.side[player];
    let me = player as i8;
    let mut dist = [UNREACHABLE; CELLS];
    let mut queue = std::collections::VecDeque::with_capacity(CELLS * 4);
    for c in 0..CELLS {
        if !on_start_border(side, c) {
            continue;
        }
        if s.board[c] == me {
            dist[c] = 0;
            queue.push_front(c);
        } else if s.can_play(player, c) {
            dist[c] = 1;
            queue.push_back(c);
        }
    }
    while let Some(cell) = queue.pop_front() {
        let d = dist[cell];
        if on_goal_border(side, cell) {
            return d;
        }
        for &(other, seg) in &g.neighbours[cell] {
            if s.cut[player][seg] != 0 {
                continue;
            }
            let owner = s.board[other];
            if owner == me {
                // two own pegs not linked: an older opponent link separates them
                if s.board[cell] == me && !s.linked[player][seg] {
                    continue;
                }
                if d < dist[other] {
                    dist[other] = d;
                    queue.push_front(other);
                }
            } else if owner == EMPTY && s.can_play(player, other) && d + 1 < dist[other] {
                dist[other] = d + 1;
                queue.push_back(other);
            }
        }
    }
    UNREACHABLE
}

fn evaluate(g: &Geometry, s: &State) -> i32 {
    let mine = path_distance(g, s, ME);
    if mine == 0 {
        return WIN_SCORE;
    }
    let theirs = path_distance(g, s, OPP);
    if theirs == 0 {
        return -WIN_SCORE;
    }
    let links = s.link_count[ME] - s.link_count[OPP];
    DISTANCE_WEIGHT * (theirs - mine) + LINK_WEIGHT * links
}

// ---------------------------------------------------------------- search
/// Insertion sort on integer keys, descending, stable (same in 4 languages).
fn stable_sort_desc(items: &mut [usize], keys: &mut [i32]) {
    for i in 1..items.len() {
        let (item, key) = (items[i], keys[i]);
        let mut j = i;
        while j > 0 && keys[j - 1] < key {
            items[j] = items[j - 1];
            keys[j] = keys[j - 1];
            j -= 1;
        }
        items[j] = item;
        keys[j] = key;
    }
}

/// Lowest score the opponent can force; stops once it is <= floor.
fn worst_reply(g: &Geometry, s: &State, floor: i32) -> i32 {
    let mut worst = WIN_SCORE;
    for cell in s.legal_moves(OPP) {
        let mut child = s.clone();
        child.place(g, OPP, cell);
        let score = evaluate(g, &child);
        if score < worst {
            worst = score;
            if worst <= floor {
                break;
            }
        }
    }
    worst
}

fn choose_move(g: &Geometry, s: &State, top_k: usize) -> (usize, String) {
    let mut moves = s.legal_moves(ME);
    let mut scores: Vec<i32> = moves
        .iter()
        .map(|&cell| {
            let mut child = s.clone();
            child.place(g, ME, cell);
            evaluate(g, &child)
        })
        .collect();
    stable_sort_desc(&mut moves, &mut scores);
    if scores[0] == WIN_SCORE {
        return (moves[0], "win".to_string());
    }
    let (mut best_cell, mut best_score) = (moves[0], -WIN_SCORE - 1);
    for &cell in moves.iter().take(top_k) {
        let mut child = s.clone();
        child.place(g, ME, cell);
        let score = worst_reply(g, &child, best_score);
        if score > best_score {
            best_cell = cell;
            best_score = score;
        }
    }
    (best_cell, format!("score={}", best_score))
}

// ---------------------------------------------------------------- I/O
fn should_swap(cell: usize) -> bool {
    let in_zone = |v: usize| (SWAP_LOW..=SWAP_HIGH).contains(&v);
    in_zone(x_of(cell)) && in_zone(y_of(cell))
}

/// Rebuild the state from one turn of input (pegs, then links, for each player).
fn read_state(g: &Geometry, s: &mut State, read: &mut dyn FnMut() -> String) {
    for player in [ME, OPP] {
        let pegs: usize = read().trim().parse().unwrap_or(0);
        for _ in 0..pegs {
            let line = read();
            s.board[parse_cell(line.split_whitespace().next().unwrap_or("A1"))] = player as i8;
        }
        let links: usize = read().trim().parse().unwrap_or(0);
        for _ in 0..links {
            let line = read();
            let mut it = line.split_whitespace();
            let a = parse_cell(it.next().unwrap_or("A1"));
            let b = parse_cell(it.next().unwrap_or("A1"));
            s.add_link(g, player, g.seg_id[a][b] as usize);
        }
    }
}

fn decide(g: &Geometry, s: &State, last: &str, first_turn: bool, top_k: usize) -> (String, String) {
    let my_pegs = s.board.iter().filter(|&&v| v == ME as i8).count();
    let their_pegs = s.board.iter().filter(|&&v| v == OPP as i8).count();
    if last == "FIRST" {
        return (name_of(OPENING_CELL), "opening".into());
    }
    if first_turn && my_pegs == 0 && their_pegs == 1 {
        let their_cell = s.board.iter().position(|&v| v == OPP as i8).unwrap();
        if should_swap(their_cell) {
            return ("SWAP".into(), "swap".into());
        }
    }
    if s.legal_moves(ME).is_empty() {
        return (name_of(0), "no move".into());
    }
    let (cell, info) = choose_move(g, s, top_k);
    (name_of(cell), info)
}

fn main() {
    let g = build_geometry();
    let top_k = std::env::var("CG_NODES").ok().and_then(|v| v.trim().parse().ok()).unwrap_or(DEFAULT_TOP_K);
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();
    let mut read = move || -> String {
        match lines.next() {
            Some(Ok(l)) => l.trim_end().to_string(),
            _ => String::new(),
        }
    };
    let out = io::stdout();
    let mut my_side: Option<usize> = None;
    loop {
        let last = read();
        if last.is_empty() {
            break;
        }
        let t0 = Instant::now();
        let first_turn = my_side.is_none();
        let side = *my_side.get_or_insert(if last == "FIRST" { TOP_BOTTOM } else { LEFT_RIGHT });
        let mut s = State::new(side);
        read_state(&g, &mut s, &mut read);
        let (mv, info) = decide(&g, &s, &last, first_turn, top_k);
        eprintln!("{} ms={}", info, t0.elapsed().as_millis());
        let mut o = out.lock();
        writeln!(o, "{}", mv).unwrap();
        o.flush().unwrap();
    }
}
