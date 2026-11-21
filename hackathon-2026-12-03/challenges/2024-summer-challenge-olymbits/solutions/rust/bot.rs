// Olymbits (Wood 1, 4 courses) — énumération des 4^5 séquences d'actions.
// Toutes les machines reçoivent la même action ; on minimise la somme des
// tours restants estimés sur toutes les courses actives.
use std::io::{self, BufRead, Write};

const HORIZON: usize = 5;
const POW4: [usize; 5] = [1, 4, 16, 64, 256];
const NAMES: [&str; 4] = ["LEFT", "DOWN", "RIGHT", "UP"];
const STEPS: [usize; 4] = [1, 2, 3, 2];
const JUMP: [bool; 4] = [false, false, false, true];

struct State {
    track: Vec<u8>,
    pos: usize,
    stun: usize,
    last: usize,
}

fn step(s: &State, pos: &mut usize, stun: &mut usize, m: usize) {
    if *stun > 0 {
        *stun -= 1;
        return;
    }
    if JUMP[m] {
        let mut np = *pos + 2;
        if np > s.last {
            np = s.last;
        }
        *stun = if *pos + 2 < s.last && s.track[*pos + 2] == b'#' {
            3
        } else {
            0
        };
        *pos = np;
        return;
    }
    for k in 1..=STEPS[m] {
        if *pos + k > s.last {
            *pos = s.last;
            *stun = 0;
            return;
        }
        if s.track[*pos + k] == b'#' {
            *pos += k;
            *stun = 3;
            return;
        }
    }
    *pos += STEPS[m];
    *stun = 0;
}

fn main() {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines().map_while(Result::ok);
    let player: usize = lines
        .next()
        .and_then(|l| l.trim().parse().ok())
        .unwrap_or(0);
    let games: usize = lines
        .next()
        .and_then(|l| l.trim().parse().ok())
        .unwrap_or(0);

    let stdout = io::stdout();
    let mut out = io::BufWriter::new(stdout.lock());
    while (0..3).all(|_| lines.next().is_some()) {
        let mut states = Vec::new();
        for _ in 0..games {
            let line = match lines.next() {
                Some(v) => v,
                None => break,
            };
            let f: Vec<&str> = line.split_whitespace().collect();
            if f.len() < 8 || f[0] == "GAME_OVER" {
                continue;
            }
            let pos: usize = f[1 + player].parse().unwrap_or(0);
            let stun: usize = f[4 + player].parse().unwrap_or(0);
            let track = f[0].as_bytes().to_vec();
            let last = track.len() - 1;
            states.push(State {
                track,
                pos,
                stun,
                last,
            });
        }

        let mut best: i64 = -1;
        let mut best_first = 0usize;
        for code in 0..1024usize {
            let mut total: i64 = 0;
            for s in &states {
                let mut pos = s.pos;
                let mut stun = s.stun;
                let mut done: i64 = -1;
                for t in 0..HORIZON {
                    let m = (code / POW4[HORIZON - 1 - t]) % 4;
                    step(s, &mut pos, &mut stun, m);
                    if pos >= s.last {
                        done = (t + 1) as i64;
                        break;
                    }
                }
                if done < 0 {
                    done = HORIZON as i64 + (s.last as i64 - pos as i64);
                }
                total += done;
            }
            if best < 0 || total < best {
                best = total;
                best_first = (code / POW4[HORIZON - 1]) % 4;
                if best == 0 {
                    break;
                }
            }
        }
        writeln!(out, "{}", NAMES[best_first]).unwrap();
        out.flush().unwrap();
    }
}
