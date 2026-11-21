// Troll Farm (Wood) — BFS depuis le troll, arbre fruitier le plus proche,
// puis retour au plus près de la cabane.
use std::collections::VecDeque;
use std::io::{self, BufRead, Write};

const INF: i32 = 1_000_000_000;
const DX: [i32; 4] = [0, 1, 0, -1];
const DY: [i32; 4] = [-1, 0, 1, 0];

fn main() {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines().map_while(Result::ok);
    let dims: Vec<usize> = lines
        .next()
        .unwrap_or_default()
        .split_whitespace()
        .filter_map(|s| s.parse().ok())
        .collect();
    let (width, height) = (dims[0], dims[1]);
    let grid: Vec<Vec<u8>> = (0..height)
        .map(|_| lines.next().unwrap_or_default().into_bytes())
        .collect();
    let mut shack = (0usize, 0usize);
    for (y, row) in grid.iter().enumerate() {
        for (x, &ch) in row.iter().enumerate() {
            if ch == b'0' {
                shack = (x, y);
            }
        }
    }
    let walkable = |x: i32, y: i32| -> bool {
        x >= 0
            && x < width as i32
            && y >= 0
            && y < height as i32
            && grid[y as usize][x as usize] == b'.'
    };

    let stdout = io::stdout();
    let mut out = io::BufWriter::new(stdout.lock());
    while let Some(line) = lines.next() {
        if line.split_whitespace().count() != 6 {
            break;
        }
        lines.next(); // inventaire adverse
        let tree_count: usize = lines
            .next()
            .unwrap_or_default()
            .trim()
            .parse()
            .unwrap_or(0);
        let mut trees: Vec<(i32, i32, i32)> = Vec::new(); // x, y, fruits
        for _ in 0..tree_count {
            let f: Vec<i32> = lines
                .next()
                .unwrap_or_default()
                .split_whitespace()
                .skip(1)
                .filter_map(|s| s.parse().ok())
                .collect();
            if f.len() >= 5 {
                trees.push((f[0], f[1], f[4]));
            }
        }
        let troll_count: usize = lines
            .next()
            .unwrap_or_default()
            .trim()
            .parse()
            .unwrap_or(0);
        let mut me: Option<(i32, i32, i32, i32)> = None; // id, x, y, carry
        for _ in 0..troll_count {
            let f: Vec<i32> = lines
                .next()
                .unwrap_or_default()
                .split_whitespace()
                .filter_map(|s| s.parse().ok())
                .collect();
            if f.len() >= 14 && f[1] == 0 {
                let carry: i32 = f[8..12].iter().sum();
                me = Some((f[0], f[2], f[3], carry));
            }
        }
        let (id, px, py, carrying) = match me {
            Some(v) => v,
            None => {
                writeln!(out, "WAIT").unwrap();
                out.flush().unwrap();
                continue;
            }
        };

        let mut dist = vec![vec![-1i32; width]; height];
        let mut first = vec![vec![(-1i32, -1i32); width]; height];
        let mut queue = VecDeque::new();
        dist[py as usize][px as usize] = 0;
        first[py as usize][px as usize] = (px, py);
        queue.push_back((px, py));
        while let Some((x, y)) = queue.pop_front() {
            for k in 0..4 {
                let nx = x + DX[k];
                let ny = y + DY[k];
                if !walkable(nx, ny) || dist[ny as usize][nx as usize] >= 0 {
                    continue;
                }
                dist[ny as usize][nx as usize] = dist[y as usize][x as usize] + 1;
                first[ny as usize][nx as usize] =
                    if dist[y as usize][x as usize] == 0 {
                        (nx, ny)
                    } else {
                        first[y as usize][x as usize]
                    };
                queue.push_back((nx, ny));
            }
        }

        let at = |x: i32, y: i32| x >= 0 && x < width as i32 && y >= 0 && y < height as i32;
        let mut best: (i32, i32, i32, i32) = (INF, INF, INF, INF);
        let mut chosen = (-1i32, -1i32);
        let mut harvest = false;
        if carrying > 0 {
            if (px - shack.0 as i32).abs() + (py - shack.1 as i32).abs() == 1 {
                writeln!(out, "DROP {}", id).unwrap();
                out.flush().unwrap();
                continue;
            }
            for k in 0..4 {
                let nx = shack.0 as i32 + DX[k];
                let ny = shack.1 as i32 + DY[k];
                if !at(nx, ny) || dist[ny as usize][nx as usize] < 0 {
                    continue;
                }
                let key = (dist[ny as usize][nx as usize], 0, ny, nx);
                if key < best {
                    best = key;
                    chosen = (nx, ny);
                }
            }
        } else {
            for &(tx, ty, fruits) in &trees {
                if fruits <= 0 {
                    continue;
                }
                if tx == px && ty == py {
                    harvest = true;
                    break;
                }
                let d = if at(tx, ty) {
                    dist[ty as usize][tx as usize]
                } else {
                    INF
                };
                let d = if d < 0 { INF } else { d };
                let key = (d, -fruits, ty, tx);
                if key < best {
                    best = key;
                    chosen = (tx, ty);
                }
            }
        }
        if harvest {
            writeln!(out, "HARVEST {}", id).unwrap();
        } else if at(chosen.0, chosen.1) && dist[chosen.1 as usize][chosen.0 as usize] >= 0 {
            let (fx, fy) = first[chosen.1 as usize][chosen.0 as usize];
            writeln!(out, "MOVE {} {} {}", id, fx, fy).unwrap();
        } else {
            writeln!(out, "WAIT").unwrap();
        }
        out.flush().unwrap();
    }
}
