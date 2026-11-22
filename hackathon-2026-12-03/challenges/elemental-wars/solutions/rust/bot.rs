// Elemental Wars — v6 : predicted opponent moves + intercept + evade.
// Strict Rust port of /tmp/opencode/ew_v6.cpp (identical algorithm and tie-breaks).
use std::collections::VecDeque;
use std::io::{self, Read, Write};

const NMAX: usize = 16;
const DX: [i32; 4] = [0, 1, 0, -1];
const DY: [i32; 4] = [-1, 0, 1, 0];

type Grid = [[u8; NMAX]; NMAX];
type Dist = [[i32; NMAX]; NMAX];

fn prey_of(t: u8) -> u8 {
    if t == b'W' {
        b'F'
    } else if t == b'F' {
        b'P'
    } else {
        b'W'
    }
}

fn pred_of(t: u8) -> u8 {
    if t == b'W' {
        b'P'
    } else if t == b'F' {
        b'W'
    } else {
        b'F'
    }
}

fn empty_dist() -> Dist {
    [[-1i32; NMAX]; NMAX]
}

fn bfs(g: &Grid, n: i32, sx: i32, sy: i32) -> Dist {
    let mut d = empty_dist();
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

#[derive(Clone, Copy)]
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

    let pred = pred_of(me);
    let prey = prey_of(me);

    // Camp of the predator tribe: last matching cell in row-major order.
    let mut camp_x: i32 = -1;
    let mut camp_y: i32 = -1;
    for y in 0..n {
        for x in 0..n {
            if grid[y as usize][x as usize] == pred {
                camp_x = x;
                camp_y = y;
            }
        }
    }

    let stdout = io::stdout();
    let mut out = stdout.lock();

    // while (cin >> sw >> sf >> sp)
    loop {
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

        let mut prisoners = 0i32;
        let mut mine: Vec<usize> = Vec::new();
        for i in 0..els.len() {
            if els[i].tribe != me {
                continue;
            }
            if els[i].prisoner != 0 {
                prisoners += 1;
            } else {
                mine.push(i);
            }
        }

        // Distances from each free enemy elemental.
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

        // Predict each enemy's next cell: step toward its nearest free prey.
        let mut pred_next: Vec<(i32, i32)> = Vec::new();
        let mut prey_next: Vec<(i32, i32)> = Vec::new();
        for e in &els {
            if e.prisoner != 0 || e.tribe == me {
                continue;
            }
            let et = prey_of(e.tribe);
            let mut tx: i32 = -1;
            let mut ty: i32 = -1;
            let mut best: i32 = 1 << 28;
            for q in &els {
                if q.prisoner != 0 || q.tribe != et {
                    continue;
                }
                let d = (q.x - e.x).abs() + (q.y - e.y).abs();
                if d < best {
                    best = d;
                    tx = q.x;
                    ty = q.y;
                }
            }
            let mut nx = e.x;
            let mut ny = e.y;
            if tx >= 0 {
                let dt = bfs(&grid, n, tx, ty);
                let mut cur = -1i32;
                if e.x >= 0 && e.x < n && e.y >= 0 && e.y < n {
                    cur = dt[e.y as usize][e.x as usize];
                }
                if cur > 0 {
                    for k in 0..4 {
                        let ax = e.x + DX[k];
                        let ay = e.y + DY[k];
                        if ax < 0 || ax >= n || ay < 0 || ay >= n {
                            continue;
                        }
                        let (axu, ayu) = (ax as usize, ay as usize);
                        if grid[ayu][axu] == b'#' || dt[ayu][axu] != cur - 1 {
                            continue;
                        }
                        nx = ax;
                        ny = ay;
                        break;
                    }
                }
            }
            if e.tribe == prey {
                prey_next.push((nx, ny));
            } else {
                pred_next.push((nx, ny));
            }
        }

        // Liberator logic.
        let have_camp = prisoners > 0 && camp_x >= 0;
        let mut dcamp = empty_dist();
        if have_camp {
            dcamp = bfs(&grid, n, camp_x, camp_y);
        }
        let mut liberator: isize = -1;
        if have_camp {
            let mut best: i32 = 1 << 28;
            for &i in &mine {
                let (ux, uy) = (els[i].x as usize, els[i].y as usize);
                let d = dcamp[uy][ux];
                if d >= 0 && d < best {
                    best = d;
                    liberator = i as isize;
                }
            }
            if best > 14 {
                liberator = -1;
            }
        }

        let mut assigned: Vec<(i32, i32)> = Vec::new();
        let mut line = String::new();
        for &idx in &mine {
            let e = els[idx];
            let lib = liberator == idx as isize;
            let (mut bx, mut by) = (e.x, e.y);
            let mut best_score: i64 = -(1i64 << 50);
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

                let mut score: i64;
                if lib {
                    let dc = if dcamp[uy][ux] >= 0 { dcamp[uy][ux] } else { 100 };
                    score = -100i64 * (dc.min(20) as i64) + 20i64 * (dq.min(8) as i64);
                    if dc == 0 {
                        score += 3000i64 * (prisoners as i64);
                    }
                    if dq == 0 {
                        score -= 20000;
                    } else if dq == 1 {
                        score -= 5000;
                    }
                } else {
                    score = -100i64 * (dp.min(20) as i64) + 20i64 * (dq.min(8) as i64);
                    // Predicted interception / evasion.
                    for p in &prey_next {
                        if p.0 == x && p.1 == y {
                            score += 12000;
                        }
                    }
                    for p in &pred_next {
                        if p.0 == x && p.1 == y {
                            score -= 18000;
                        }
                    }
                    if dq == 0 {
                        score -= 20000;
                    } else if dq == 1 {
                        score -= 4000;
                    }
                }
                for c in &assigned {
                    if c.0 == x && c.1 == y {
                        score -= 5000;
                    }
                }
                if score > best_score || (score == best_score && (x < bx || (x == bx && y < by))) {
                    best_score = score;
                    bx = x;
                    by = y;
                }
            }
            assigned.push((bx, by));
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
