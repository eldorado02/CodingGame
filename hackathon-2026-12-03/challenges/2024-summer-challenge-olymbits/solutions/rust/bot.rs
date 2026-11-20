// Olymbits — Hurdle Race : BFS sans jamais atterrir sur une haie.
use std::collections::VecDeque;
use std::io::{self, BufRead, Write};

const NAMES: [&str; 4] = ["LEFT", "DOWN", "RIGHT", "UP"];
const STEPS: [i32; 4] = [1, 2, 3, 2];
const JUMP: [bool; 4] = [false, false, false, true];

/// Position atteinte, ou None si le coureur heurte une haie.
fn move_pos(track: &[u8], pos: usize, steps: i32, mut jump: bool) -> Option<usize> {
    let last = track.len() - 1;
    let mut np = pos;
    for _ in 0..steps {
        if np < last {
            np += 1;
        }
        if track[np] == b'#' && !jump {
            return None;
        }
        if np == last {
            break;
        }
        jump = false; // UP n'ignore que la première case.
    }
    Some(np)
}

fn best_action(track: &[u8], pos: usize) -> &'static str {
    let last = track.len() - 1;
    if pos >= last {
        return "LEFT";
    }
    // parent[p] = Some(chemin depuis pos), None = non visité ; Some((p, a)) = visité.
    let mut path: Vec<Option<(usize, usize)>> = vec![None; last + 1];
    let mut queue = VecDeque::new();
    queue.push_back(pos);
    path[pos] = Some((pos, usize::MAX));
    while let Some(p) = queue.pop_front() {
        for (a, (&steps, &jump)) in STEPS.iter().zip(JUMP.iter()).enumerate() {
            let np = match move_pos(track, p, steps, jump) {
                Some(v) => v,
                None => continue,
            };
            if path[np].is_some() {
                continue;
            }
            let first = if p == pos { a } else { path[p].unwrap().1 };
            path[np] = Some((p, first));
            if np == last {
                return NAMES[first];
            }
            queue.push_back(np);
        }
    }
    "LEFT"
}

fn main() {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines().map_while(Result::ok);
    let player: usize = lines.next().and_then(|l| l.trim().parse().ok()).unwrap_or(0);
    let games: usize = lines.next().and_then(|l| l.trim().parse().ok()).unwrap_or(0);

    let stdout = io::stdout();
    let mut out = io::BufWriter::new(stdout.lock());
    while (0..3).all(|_| lines.next().is_some()) {
        let mut gpu = String::new();
        let mut reg = [0i32; 7];
        let mut count = 0;
        for _ in 0..games {
            let line = match lines.next() {
                Some(v) => v,
                None => return,
            };
            let mut parts = line.split_whitespace();
            if let Some(token) = parts.next() {
                gpu = token.to_string();
                for (i, value) in parts.by_ref().take(7).enumerate() {
                    reg[i] = value.parse().unwrap_or(0);
                }
                count += 1;
            }
        }
        let answer = if count == 0 || gpu == "GAME_OVER" {
            "LEFT"
        } else {
            best_action(gpu.as_bytes(), reg[player] as usize)
        };
        writeln!(out, "{}", answer).unwrap();
        out.flush().unwrap();
    }
}
