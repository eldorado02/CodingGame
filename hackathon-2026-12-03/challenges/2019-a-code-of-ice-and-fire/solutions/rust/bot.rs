// A Code of Ice and Fire (Wood 3) — rush du QG adverse.
use std::collections::VecDeque;
use std::error::Error;
use std::io::{self, BufRead, Write};

const W: usize = 12;
const H: usize = 12;
const INF: i32 = 1_000_000;
const DX: [i32; 4] = [0, 1, 0, -1];
const DY: [i32; 4] = [-1, 0, 1, 0];

fn main() -> Result<(), Box<dyn Error>> {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();

    let first = match lines.next() {
        Some(line) => line?,
        None => return Ok(()),
    };
    let mines: usize = first.trim().parse()?;
    for _ in 0..mines {
        lines.next();
    }

    let stdout = io::stdout();
    let mut out = stdout.lock();

    while let Some(result) = lines.next() {
        let line = result?;
        if line.is_empty() {
            break;
        }
        let gold: i32 = line.trim().parse()?;
        lines.next(); // income
        lines.next(); // opponent gold
        lines.next(); // opponent income

        let mut grid = Vec::with_capacity(H);
        for _ in 0..H {
            grid.push(lines.next().unwrap()?);
        }

        let nb: usize = lines.next().unwrap()?.trim().parse()?;
        let mut my_hq: Option<(usize, usize)> = None;
        let mut enemy_hq: Option<(usize, usize)> = None;
        for _ in 0..nb {
            let building_line = lines.next().unwrap()?;
            let fields: Vec<&str> = building_line.split_whitespace().collect();
            let owner: i32 = fields[0].parse()?;
            let kind: i32 = fields[1].parse()?;
            let x: usize = fields[2].parse()?;
            let y: usize = fields[3].parse()?;
            if kind == 0 {
                if owner == 0 { my_hq = Some((x, y)); }
                else { enemy_hq = Some((x, y)); }
            }
        }

        let nu: usize = lines.next().unwrap()?.trim().parse()?;
        let mut units: Vec<(i32, usize, usize)> = Vec::new();
        for _ in 0..nu {
            let unit_line = lines.next().unwrap()?;
            let fields: Vec<&str> = unit_line.split_whitespace().collect();
            let owner: i32 = fields[0].parse()?;
            let id: i32 = fields[1].parse()?;
            let x: usize = fields[3].parse()?;
            let y: usize = fields[4].parse()?;
            if owner == 0 {
                units.push((id, x, y));
            }
        }

        let (ex, ey) = match enemy_hq {
            Some(p) => p,
            None => {
                writeln!(out, "WAIT")?;
                out.flush()?;
                continue;
            }
        };

        let mut dist = vec![vec![INF; W]; H];
        let mut queue = VecDeque::new();
        dist[ey][ex] = 0;
        queue.push_back((ex, ey));
        while let Some((x, y)) = queue.pop_front() {
            let nd = dist[y][x] + 1;
            for k in 0..4 {
                let nx = x as i32 + DX[k];
                let ny = y as i32 + DY[k];
                if nx < 0 || nx >= W as i32 || ny < 0 || ny >= H as i32 {
                    continue;
                }
                let (nx, ny) = (nx as usize, ny as usize);
                if grid[ny].as_bytes()[nx] == b'#' || dist[ny][nx] != INF {
                    continue;
                }
                dist[ny][nx] = nd;
                queue.push_back((nx, ny));
            }
        }

        let mut actions: Vec<String> = Vec::new();
        for (id, _x, _y) in &units {
            actions.push(format!("MOVE {} {} {}", id, ex, ey));
        }

        let mut candidates: Vec<(i32, usize, usize)> = Vec::new();
        for y in 0..H {
            for x in 0..W {
                if grid[y].as_bytes()[x] == b'#' {
                    continue;
                }
                let mut playable = grid[y].as_bytes()[x] == b'O';
                for k in 0..4 {
                    let nx = x as i32 + DX[k];
                    let ny = y as i32 + DY[k];
                    if nx < 0 || nx >= W as i32 || ny < 0 || ny >= H as i32 {
                        continue;
                    }
                    if grid[ny as usize].as_bytes()[nx as usize] == b'O' {
                        playable = true;
                    }
                }
                if !playable {
                    continue;
                }
                if my_hq == Some((x, y)) || dist[y][x] == INF {
                    continue;
                }
                candidates.push((dist[y][x], x, y));
            }
        }
        candidates.sort_unstable();

        let ntrain = if gold >= 10 { (gold / 10) as usize } else { 0 };
        for &(d, x, y) in candidates.iter().take(ntrain) {
            let _ = d;
            actions.push(format!("TRAIN 1 {} {}", x, y));
        }

        if actions.is_empty() {
            writeln!(out, "WAIT")?;
        } else {
            writeln!(out, "{}", actions.join(";"))?;
        }
        out.flush()?;
    }
    Ok(())
}
