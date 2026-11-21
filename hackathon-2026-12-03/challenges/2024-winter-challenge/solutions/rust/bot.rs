// Cellularena (Wood 3+) — HARVESTER puis croissance greedy.
// Les sources déjà harvesées sont bloquées dans le BFS pour ne pas les absorber.
use std::collections::{HashSet, VecDeque};
use std::io::{self, BufRead, Write};

#[derive(Clone, Copy, Default)]
struct Cell {
    wall: bool,
    taken: bool,
    protein: u8,
}

#[derive(Clone, Copy)]
struct Step {
    parent: i32,
    fx: i32,
    fy: i32,
    dist: i32,
}

const DX: [i32; 4] = [0, 1, 0, -1];
const DY: [i32; 4] = [-1, 0, 1, 0];
const DIR_NAME: [&str; 4] = ["N", "E", "S", "W"];
// Ordre lexicographique des noms : E < N < S < W.
const DIR_RANK: [i32; 4] = [1, 0, 2, 3];

fn dir_index(s: &str) -> Option<usize> {
    DIR_NAME.iter().position(|&n| n == s)
}

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

    let stdout = io::stdout();
    let mut out = io::BufWriter::new(stdout.lock());
    while let Some(line) = lines.next() {
        let n: usize = match line.trim().parse() {
            Ok(v) => v,
            Err(_) => break,
        };
        let mut grid = vec![vec![Cell::default(); width]; height];
        let mut mine: Vec<(usize, usize, i32)> = Vec::new();
        let mut harvested: HashSet<(usize, usize)> = HashSet::new();
        for _ in 0..n {
            let f: Vec<String> = match lines.next() {
                Some(v) => v.split_whitespace().map(str::to_string).collect(),
                None => return,
            };
            if f.len() < 8 {
                continue;
            }
            let x: i32 = f[0].parse().unwrap_or(-1);
            let y: i32 = f[1].parse().unwrap_or(-1);
            if x < 0 || x >= width as i32 || y < 0 || y >= height as i32 {
                continue;
            }
            let (x, y) = (x as usize, y as usize);
            let owner: i32 = f[3].parse().unwrap_or(-1);
            let id: i32 = f[4].parse().unwrap_or(0);
            let cell = &mut grid[y][x];
            if f[2] == "WALL" {
                cell.wall = true;
            } else if f[2].len() == 1 && matches!(f[2].as_bytes()[0], b'A'..=b'D') {
                cell.protein = f[2].as_bytes()[0];
            } else {
                cell.taken = true;
                if owner == 1 {
                    mine.push((x, y, id));
                    if f[2] == "HARVESTER" {
                        if let Some(k) = dir_index(&f[5]) {
                            let sx = x as i32 + DX[k];
                            let sy = y as i32 + DY[k];
                            if sx >= 0 && sx < width as i32 && sy >= 0 && sy < height as i32 {
                                harvested.insert((sx as usize, sy as usize));
                            }
                        }
                    }
                }
            }
        }
        let stocks: Vec<i32> = lines
            .next()
            .unwrap_or_default()
            .split_whitespace()
            .filter_map(|s| s.parse().ok())
            .collect();
        lines.next();  // opposant
        let required: usize = lines
            .next()
            .unwrap_or_default()
            .trim()
            .parse()
            .unwrap_or(1);

        let mut info = vec![vec![None::<Step>; width]; height];
        if required > 0 && !mine.is_empty() {
            let mut queue = VecDeque::new();
            for (x, y, id) in &mine {
                for k in 0..4 {
                    let nx = *x as i32 + DX[k];
                    let ny = *y as i32 + DY[k];
                    if nx < 0 || nx >= width as i32 || ny < 0 || ny >= height as i32 {
                        continue;
                    }
                    let (nx, ny) = (nx as usize, ny as usize);
                    let cell = &grid[ny][nx];
                    if cell.wall || cell.taken || harvested.contains(&(nx, ny))
                        || info[ny][nx].is_some()
                    {
                        continue;
                    }
                    info[ny][nx] = Some(Step {
                        parent: *id,
                        fx: nx as i32,
                        fy: ny as i32,
                        dist: 1,
                    });
                    queue.push_back((nx, ny));
                }
            }
            while let Some((x, y)) = queue.pop_front() {
                for k in 0..4 {
                    let nx = x as i32 + DX[k];
                    let ny = y as i32 + DY[k];
                    if nx < 0 || nx >= width as i32 || ny < 0 || ny >= height as i32 {
                        continue;
                    }
                    let (nx, ny) = (nx as usize, ny as usize);
                    let cell = &grid[ny][nx];
                    if cell.wall || cell.taken || harvested.contains(&(nx, ny))
                        || info[ny][nx].is_some()
                    {
                        continue;
                    }
                    let from = info[y][x].unwrap();
                    info[ny][nx] = Some(Step {
                        parent: from.parent,
                        fx: from.fx,
                        fy: from.fy,
                        dist: from.dist + 1,
                    });
                    queue.push_back((nx, ny));
                }
            }
        }

        // 0=WAIT, 1=HARVESTER, 2=BASIC
        let mut answer = 0;
        let mut a_id = 0;
        let mut a_x = 0;
        let mut a_y = 0;
        let mut a_dir = 0;
        if stocks.get(2).copied().unwrap_or(0) > 0 && stocks.get(3).copied().unwrap_or(0) > 0 {
            let mut best: (i32, usize, usize, i32) = (9, 0, 0, 9);
            for y in 0..height {
                for x in 0..width {
                    let step = match info[y][x] {
                        Some(v) => v,
                        None => continue,
                    };
                    if step.dist != 1 {
                        continue;
                    }
                    for k in 0..4 {
                        let sx = x as i32 + DX[k];
                        let sy = y as i32 + DY[k];
                        if sx < 0 || sx >= width as i32 || sy < 0 || sy >= height as i32 {
                            continue;
                        }
                        let (sx, sy) = (sx as usize, sy as usize);
                        if grid[sy][sx].protein == 0 || harvested.contains(&(sx, sy)) {
                            continue;
                        }
                        let key = (
                            if grid[sy][sx].protein == b'A' { 0 } else { 1 },
                            y,
                            x,
                            DIR_RANK[k],
                        );
                        if key < best {
                            best = key;
                            a_id = step.parent;
                            a_x = x;
                            a_y = y;
                            a_dir = k;
                            answer = 1;
                        }
                    }
                }
            }
        }
        if answer == 0 && stocks.first().copied().unwrap_or(0) > 0 {
            let pick = |phase: i32| -> Option<Step> {
                let mut best: Option<(i32, usize, usize)> = None;
                let mut chosen = None;
                for y in 0..height {
                    for x in 0..width {
                        let step = match info[y][x] {
                            Some(v) => v,
                            None => continue,
                        };
                        if phase == 0 && grid[y][x].protein != b'A' {
                            continue;
                        }
                        if phase == 1 && grid[y][x].protein == 0 {
                            continue;
                        }
                        let key = (step.dist, y, x);
                        if best.map_or(true, |b| key < b) {
                            best = Some(key);
                            chosen = Some(step);
                        }
                    }
                }
                chosen
            };
            if let Some(s) = pick(0).or_else(|| pick(1)).or_else(|| pick(2)) {
                a_id = s.parent;
                a_x = s.fx as usize;
                a_y = s.fy as usize;
                answer = 2;
            }
        }

        match answer {
            1 => writeln!(
                out,
                "GROW {} {} {} HARVESTER {}",
                a_id, a_x, a_y, DIR_NAME[a_dir]
            )
            .unwrap(),
            2 => writeln!(out, "GROW {} {} {} BASIC", a_id, a_x, a_y).unwrap(),
            _ => writeln!(out, "WAIT").unwrap(),
        }
        for _ in 1..required {
            writeln!(out, "WAIT").unwrap();
        }
        out.flush().unwrap();
    }
}
