// Ataxx bot — deterministic 1-ply greedy (port of solutions/python/bot.rs).
// Build: cargo build --release --manifest-path solutions/rust/Cargo.toml
// The referee provides the legal actions; we only simulate and score them.
use std::io::{self, BufRead, Write};

const N: usize = 49;
const WIN_SCORE: i32 = 1_000_000;
const PIECE_WEIGHT: i32 = 20;

// Center-control weights (doubly symmetric matrix, row 0 = bottom).
const WEIGHTS: [i32; N] = [
    1, 2, 2, 3, 2, 2, 1,
    2, 3, 4, 4, 4, 3, 2,
    2, 4, 5, 5, 5, 4, 2,
    3, 4, 5, 6, 5, 4, 3,
    2, 4, 5, 5, 5, 4, 2,
    2, 3, 4, 4, 4, 3, 2,
    1, 2, 2, 3, 2, 2, 1,
];

fn build_neighbors() -> Vec<Vec<usize>> {
    let mut nb = vec![Vec::<usize>::new(); N];
    for y in 0..7usize {
        for x in 0..7usize {
            let src = y * 7 + x;
            for dy in -1isize..=1 {
                for dx in -1isize..=1 {
                    if dx == 0 && dy == 0 {
                        continue;
                    }
                    let nx = x as isize + dx;
                    let ny = y as isize + dy;
                    if (0..7).contains(&nx) && (0..7).contains(&ny) {
                        nb[src].push((ny as usize) * 7 + nx as usize);
                    }
                }
            }
        }
    }
    nb
}

fn clamp7(v: i32) -> usize {
    v.clamp(0, 6) as usize
}

fn char_to_cell(ch: u8) -> i32 {
    match ch {
        b'1' => 2,
        b'0' => 1,
        b'#' => 3,
        _ => 0,
    }
}

struct Parsed {
    jump: bool,
    src: usize,
    dst: usize,
}

fn parse_action(action: &str) -> Parsed {
    let s = action.as_bytes();
    if s.len() < 2 {
        return Parsed { jump: false, src: 0, dst: 0 };
    }
    let x0 = clamp7(s[0] as i32 - 'a' as i32);
    let y0 = clamp7(s[1] as i32 - '1' as i32);
    let src = y0 * 7 + x0;
    if s.len() >= 4 {
        let x1 = clamp7(s[2] as i32 - 'a' as i32);
        let y1 = clamp7(s[3] as i32 - '1' as i32);
        return Parsed { jump: true, src, dst: y1 * 7 + x1 };
    }
    Parsed { jump: false, src, dst: src }
}

fn simulate(cells: &[i32], action: &str, me: i32, opp: i32, nb: &[Vec<usize>]) -> Vec<i32> {
    let mut board = cells.to_vec();
    let p = parse_action(action);
    if p.jump {
        board[p.src] = 0; // source vacated by a jump
    }
    board[p.dst] = me;
    for &idx in &nb[p.dst] {
        if board[idx] == opp {
            board[idx] = me;
        }
    }
    board
}

fn evaluate(board: &[i32], me: i32, opp: i32, nb: &[Vec<usize>]) -> i32 {
    let mut my_count = 0;
    let mut opp_count = 0;
    let mut score = 0;
    for i in 0..N {
        let cell = board[i];
        if cell == me {
            my_count += 1;
            let mut s = WEIGHTS[i];
            for &idx in &nb[i] {
                if board[idx] == opp {
                    s += 1;
                }
            }
            score += s;
        } else if cell == opp {
            opp_count += 1;
            let mut s = WEIGHTS[i];
            for &idx in &nb[i] {
                if board[idx] == me {
                    s += 1;
                }
            }
            score -= s;
        }
    }
    if my_count == 0 {
        return -WIN_SCORE;
    }
    if opp_count == 0 {
        return WIN_SCORE;
    }
    score + PIECE_WEIGHT * (my_count - opp_count)
}

fn choose(cells: &[i32], me: i32, actions: &[String], nb: &[Vec<usize>]) -> String {
    let opp = 3 - me;
    let mut best: Option<String> = None;
    let mut best_eval = 0;
    for action in actions {
        if action.is_empty() {
            continue;
        }
        let next = simulate(cells, action, me, opp, nb);
        let val = evaluate(&next, me, opp, nb);
        let better = match &best {
            None => true,
            Some(b) => val > best_eval || (val == best_eval && action.as_str() < b.as_str()),
        };
        if better {
            best_eval = val;
            best = Some(action.clone());
        }
    }
    best.unwrap_or_else(|| "random".to_string())
}

/// Read one whitespace-trimmed line. Each protocol token sits on its own line.
fn next_line<R: BufRead>(reader: &mut R, buf: &mut String) -> Option<String> {
    buf.clear();
    let n = reader.read_line(buf).ok()?;
    if n == 0 {
        return None;
    }
    Some(buf.trim().to_string())
}

fn main() {
    let stdin = io::stdin();
    let mut reader = stdin.lock();
    let mut line = String::new();
    let mut out = io::BufWriter::new(io::stdout().lock());
    let nb = build_neighbors();

    let first = match next_line(&mut reader, &mut line) {
        Some(s) => s,
        None => return,
    };
    let id: i32 = first.parse().unwrap_or(0);
    let mut me = id + 1;
    if me != 1 && me != 2 {
        me = 1;
    }

    loop {
        let mut cells = vec![0i32; N];
        let mut complete = true;
        for y in 0..7usize {
            let row = match next_line(&mut reader, &mut line) {
                Some(s) => s,
                None => {
                    complete = false;
                    break;
                }
            };
            let bytes = row.as_bytes();
            for x in 0..7usize {
                let ch = if x < bytes.len() { bytes[x] } else { b'.' };
                cells[y * 7 + x] = char_to_cell(ch);
            }
        }
        if !complete {
            break;
        }
        let _last_action = match next_line(&mut reader, &mut line) {
            Some(s) => s,
            None => break,
        };
        let count: usize = match next_line(&mut reader, &mut line) {
            Some(s) => s.parse().unwrap_or(0),
            None => break,
        };
        let mut actions: Vec<String> = Vec::with_capacity(count.min(1024));
        for _ in 0..count {
            match next_line(&mut reader, &mut line) {
                Some(s) => actions.push(s),
                None => break,
            }
        }
        let best = choose(&cells, me, &actions, &nb);
        let _ = writeln!(out, "{}", best);
        let _ = out.flush();
    }
}
