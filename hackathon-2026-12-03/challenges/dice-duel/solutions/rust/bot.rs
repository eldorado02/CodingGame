// Dice Duel — Rust port of cpp/bot.cpp (same moves, same search, same tie-breaks).
// Build: cargo build --release
use std::collections::HashSet;
use std::io::{self, BufRead, Write};
use std::time::Instant;

const SIZE: i32 = 8;
const SUM_RULE: bool = false; // true from Wood 1 on (capture only when tops sum to 7)
const CAPTURE_SUM: i32 = 7;
const DIE_VALUE: i32 = 100;
const TURN_NODES: i64 = 4000; // node cost varies x4 with the dice count
const TURN_MS: u128 = 30;
/// (name, dx, dy, roll permutation), DFS order.
const STEPS: [(char, i32, i32, [usize; 6]); 4] = [
    ('U', 0, 1, [1, 2, 3, 0, 4, 5]),
    ('D', 0, -1, [3, 0, 1, 2, 4, 5]),
    ('R', 1, 0, [4, 1, 5, 3, 2, 0]),
    ('L', -1, 0, [5, 1, 4, 3, 0, 2]),
];

type Faces = [i32; 6]; // top, front, bottom, back, left, right

#[derive(Clone, Copy)]
struct Die { owner: i32, x: i32, y: i32, faces: Faces }

#[derive(Clone)]
struct Move { die: usize, path: String, x: i32, y: i32, faces: Faces, captured: Option<usize> }

/// Self-avoiding rolls of every die of one side (DFS order U, D, R, L).
struct Generator<'a> {
    dice: &'a [Die],
    side: i32,
    at: [[Option<usize>; 8]; 8],
    visited: [[bool; 8]; 8],
    seen: HashSet<(usize, i32, i32, Faces)>,
    moves: Vec<Move>,
}

impl<'a> Generator<'a> {
    fn new(dice: &'a [Die], side: i32) -> Self {
        let mut at = [[None; 8]; 8];
        for (i, d) in dice.iter().enumerate() { at[d.x as usize][d.y as usize] = Some(i); }
        Generator { dice, side, at, visited: [[false; 8]; 8], seen: HashSet::new(), moves: Vec::new() }
    }

    fn run(mut self) -> Vec<Move> {
        for (i, d) in self.dice.iter().enumerate() {
            if d.owner != self.side { continue; }
            self.visited[d.x as usize][d.y as usize] = true;
            let mut path = String::new();
            self.walk(i, d.x, d.y, d.faces, &mut path);
            self.visited[d.x as usize][d.y as usize] = false;
        }
        let (captures, quiet): (Vec<Move>, Vec<Move>) = self.moves.into_iter().partition(|m| m.captured.is_some());
        captures.into_iter().chain(quiet).collect()
    }

    fn walk(&mut self, i: usize, x: i32, y: i32, faces: Faces, path: &mut String) {
        let top = self.dice[i].faces[0] as usize;
        if path.len() == top {
            self.record(i, x, y, faces, path);
            return;
        }
        for &(name, dx, dy, roll) in &STEPS {
            let (nx, ny) = (x + dx, y + dy);
            if !(0..SIZE).contains(&nx) || !(0..SIZE).contains(&ny) || self.visited[nx as usize][ny as usize] { continue; }
            if let Some(j) = self.at[nx as usize][ny as usize] {
                if self.dice[j].owner == self.side || path.len() + 1 < top { continue; } // only the last step captures
            }
            let next: Faces = std::array::from_fn(|k| faces[roll[k]]);
            self.visited[nx as usize][ny as usize] = true;
            path.push(name);
            self.walk(i, nx, ny, next, path);
            path.pop();
            self.visited[nx as usize][ny as usize] = false;
        }
    }

    fn record(&mut self, i: usize, x: i32, y: i32, faces: Faces, path: &str) {
        let captured = self.at[x as usize][y as usize];
        if let Some(j) = captured {
            if SUM_RULE && faces[0] + self.dice[j].faces[0] != CAPTURE_SUM { return; }
        }
        if !self.seen.insert((i, x, y, faces)) { return; }
        self.moves.push(Move { die: i, path: path.to_string(), x, y, faces, captured });
    }
}

fn apply_move(dice: &[Die], m: &Move) -> Vec<Die> {
    let mut out = dice.to_vec();
    out[m.die] = Die { owner: dice[m.die].owner, x: m.x, y: m.y, faces: m.faces };
    if let Some(j) = m.captured { out.remove(j); }
    out
}

fn evaluate(dice: &[Die], side: i32) -> i32 {
    let mine = dice.iter().filter(|d| d.owner == side).count() as i32;
    DIE_VALUE * (mine - (dice.len() as i32 - mine))
}

// ---------------------------------------------------------------- ENGINE
const WIN: i32 = 1_000_000;
const INF: i32 = 1 << 30;
const MAX_DEPTH: i32 = 64;

struct Search { nodes: i64, limit: i64, aborted: bool, start: Instant }

impl Search {
    fn negamax(&mut self, dice: &[Die], side: i32, depth: i32, mut alpha: i32, beta: i32, ply: i32) -> i32 {
        self.nodes += 1;
        if self.nodes > self.limit {
            self.aborted = true;
            return 0;
        }
        if self.start.elapsed().as_millis() > TURN_MS { // every node: one move generation can cost 0.2 ms
            eprintln!("TIMEGUARD");
            self.aborted = true;
            return 0;
        }
        if depth == 0 { return evaluate(dice, side); }
        let moves = Generator::new(dice, side).run();
        if moves.is_empty() { return -(WIN - ply); }
        let mut best = -INF;
        for m in &moves {
            let v = -self.negamax(&apply_move(dice, m), 1 - side, depth - 1, -beta, -alpha, ply + 1);
            if self.aborted { return 0; }
            best = best.max(v);
            alpha = alpha.max(v);
            if alpha >= beta { break; }
        }
        best
    }

    fn run(&mut self, dice: &[Die], mut moves: Vec<Move>) -> Move {
        let mut best = moves[0].clone();
        for depth in 1..=MAX_DEPTH {
            let (mut alpha, mut best_v, mut cur) = (-INF, -INF, 0);
            for (k, m) in moves.iter().enumerate() {
                let v = -self.negamax(&apply_move(dice, m), 1, depth - 1, -INF, -alpha, 1);
                if self.aborted { break; }
                if v > best_v {
                    best_v = v;
                    cur = k;
                }
                alpha = alpha.max(v);
            }
            if self.aborted { break; }
            best = moves[cur].clone();
            let pv = moves.remove(cur); // PV first
            moves.insert(0, pv);
            if best_v.abs() >= WIN - MAX_DEPTH { break; }
        }
        best
    }
}

fn main() {
    let limit = std::env::var("CG_NODES").ok().and_then(|v| v.parse::<i64>().ok()).filter(|&v| v > 0).unwrap_or(TURN_NODES);
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines().map_while(Result::ok);
    let stdout = io::stdout();
    let mut out = stdout.lock();
    while let Some(line) = lines.next() {
        let Ok(count) = line.trim().parse::<usize>() else { return };
        let start = Instant::now();
        let mut dice = Vec::with_capacity(count);
        for _ in 0..count {
            let Some(row) = lines.next() else { return };
            let p: Vec<&str> = row.split_whitespace().collect();
            if p.len() < 8 { return; }
            let cell = p[1].as_bytes();
            let faces: Faces = std::array::from_fn(|k| p[2 + k].parse().unwrap_or(0));
            dice.push(Die {
                owner: p[0].parse().unwrap_or(0),
                x: cell[0] as i32 - 'A' as i32,
                y: p[1][1..].parse::<i32>().unwrap_or(1) - 1,
                faces,
            });
        }
        let moves = Generator::new(&dice, 0).run();
        if moves.is_empty() {
            writeln!(out, "A1 U").unwrap();
        } else {
            let mut search = Search { nodes: 0, limit, aborted: false, start };
            let m = search.run(&dice, moves);
            let d = dice[m.die];
            writeln!(out, "{}{} {}", (b'A' + d.x as u8) as char, d.y + 1, m.path).unwrap();
        }
        out.flush().unwrap();
    }
}
