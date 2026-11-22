// Elemental Wars — v3 : commande toutes les elementaux, fuit le predateur,
// poursuit la proie. Distances BFS precalculees par ennemi (port de bot.cpp).
use std::collections::VecDeque;
use std::io::{self, Read, Write};

const NMAX: usize = 16;
const DX: [i32; 4] = [0, 1, 0, -1];
const DY: [i32; 4] = [-1, 0, 1, 0];

type Grid = [[u8; NMAX]; NMAX];
type Dist = [[i32; NMAX]; NMAX];

fn prey_of(t: u8) -> u8 {
    match t {
        b'W' => b'F',
        b'F' => b'P',
        _ => b'W',
    }
}

fn pred_of(t: u8) -> u8 {
    match t {
        b'W' => b'P',
        b'F' => b'W',
        _ => b'F',
    }
}

fn bfs(g: &Grid, n: i32, sx: i32, sy: i32) -> Dist {
    let mut d = [[-1i32; NMAX]; NMAX];
    if sx < 0 || sx >= n || sy < 0 || sy >= n {
        return d;
    }
    let (sx, sy) = (sx as usize, sy as usize);
    if g[sy][sx] == b'#' {
        return d;
    }
    d[sy][sx] = 0;
    let mut q = VecDeque::new();
    q.push_back((sx, sy));
    while let Some((x, y)) = q.pop_front() {
        for k in 0..4 {
            let nx = x as i32 + DX[k];
            let ny = y as i32 + DY[k];
            if nx < 0 || nx >= n || ny < 0 || ny >= n {
                continue;
            }
            let (nx, ny) = (nx as usize, ny as usize);
            if g[ny][nx] == b'#' || d[ny][nx] >= 0 {
                continue;
            }
            d[ny][nx] = d[y][x] + 1;
            q.push_back((nx, ny));
        }
    }
    d
}

struct El {
    id: i32,
    tribe: u8,
    x: i32,
    y: i32,
    prisoner: i32,
}

fn next_token<'a>(it: &mut impl Iterator<Item = &'a str>) -> Option<&'a str> {
    it.next()
}

fn next_i32<'a>(it: &mut impl Iterator<Item = &'a str>) -> Option<i32> {
    it.next()?.parse().ok()
}

fn main() {
    let mut input = String::new();
    if io::stdin().read_to_string(&mut input).is_err() {
        return;
    }
    let mut it = input.split_whitespace();

    let me = match next_token(&mut it) {
        Some(t) => t.as_bytes()[0],
        None => return,
    };
    let n: i32 = match next_i32(&mut it) {
        Some(v) => v,
        None => return,
    };

    let mut grid = [[b'.'; NMAX]; NMAX];
    for y in 0..n {
        let row = match next_token(&mut it) {
            Some(t) => t,
            None => return,
        };
        let bytes = row.as_bytes();
        for x in 0..n {
            grid[y as usize][x as usize] = if (x as usize) < bytes.len() {
                bytes[x as usize]
            } else {
                b'.'
            };
        }
    }

    let stdout = io::stdout();
    let mut out = stdout.lock();
    loop {
        // `while (cin >> sw >> sf >> sp)` : trois entiers, puis le compteur.
        if next_i32(&mut it).is_none() {
            break;
        }
        if next_i32(&mut it).is_none() {
            break;
        }
        if next_i32(&mut it).is_none() {
            break;
        }
        let count: i32 = match next_i32(&mut it) {
            Some(v) => v,
            None => break,
        };

        let mut els: Vec<El> = Vec::with_capacity(count.max(0) as usize);
        for _ in 0..count {
            let id = match next_i32(&mut it) {
                Some(v) => v,
                None => return,
            };
            let tribe = match next_token(&mut it) {
                Some(t) => t.as_bytes()[0],
                None => return,
            };
            let x = match next_i32(&mut it) {
                Some(v) => v,
                None => return,
            };
            let y = match next_i32(&mut it) {
                Some(v) => v,
                None => return,
            };
            let prisoner = match next_i32(&mut it) {
                Some(v) => v,
                None => return,
            };
            els.push(El {
                id,
                tribe,
                x,
                y,
                prisoner,
            });
        }

        let prey = prey_of(me);
        let pred = pred_of(me);
        let mut prey_dist: Vec<Dist> = Vec::new();
        let mut pred_dist: Vec<Dist> = Vec::new();
        for e in &els {
            if e.prisoner != 0 || e.tribe == me {
                continue;
            }
            let d = bfs(&grid, n, e.x, e.y);
            if e.tribe == prey {
                prey_dist.push(d);
            } else if e.tribe == pred {
                pred_dist.push(d);
            }
        }

        let mut line = String::new();
        for e in &els {
            if e.tribe != me || e.prisoner != 0 {
                continue;
            }
            let mut bx = e.x;
            let mut by = e.y;
            let mut best = -1_000_000_000i32;
            for k in -1..4i32 {
                let mut x = e.x;
                let mut y = e.y;
                if k >= 0 {
                    x += DX[k as usize];
                    y += DY[k as usize];
                }
                if x < 0 || x >= n || y < 0 || y >= n || grid[y as usize][x as usize] == b'#' {
                    continue;
                }
                let (ux, uy) = (x as usize, y as usize);
                let mut dp = 100i32;
                let mut dq = 100i32;
                for d in &prey_dist {
                    let v = d[uy][ux];
                    if v >= 0 && v < dp {
                        dp = v;
                    }
                }
                for d in &pred_dist {
                    let v = d[uy][ux];
                    if v >= 0 && v < dq {
                        dq = v;
                    }
                }
                let mut score = -100 * dp.min(20) + 15 * dq.min(8);
                if dp == 0 {
                    score += 10000;
                }
                if dq == 0 {
                    score -= 12000;
                }
                if dq <= 1 && dp > 1 {
                    score -= 2000;
                }
                if score > best || (score == best && (x < bx || (x == bx && y < by))) {
                    best = score;
                    bx = x;
                    by = y;
                }
            }
            if !line.is_empty() {
                line.push(';');
            }
            if bx == e.x && by == e.y {
                line.push_str(&format!("{} WAIT", e.id));
            } else {
                line.push_str(&format!("{} MOVE {} {}", e.id, bx, by));
            }
        }
        if line.is_empty() {
            line.push_str("MSG");
        }
        writeln!(out, "{}", line).ok();
        out.flush().ok();
    }
}
