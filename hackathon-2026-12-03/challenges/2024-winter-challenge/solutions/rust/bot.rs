// Cellularena (Wood) — BFS multi-source vers la protéine A la plus proche.
use std::collections::VecDeque;
use std::io::{self, BufRead, Write};

#[derive(Clone, Copy, Default)]
struct Cell {
    wall: bool,
    taken: bool,
    protein: bool,
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
                cell.protein = true;
            } else {
                cell.taken = true;
                if owner == 1 {
                    mine.push((x, y, id));
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
        if stocks.first().copied().unwrap_or(0) > 0 && !mine.is_empty() {
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
                    if cell.wall || cell.taken || info[ny][nx].is_some() {
                        continue;
                    }
                    info[ny][nx] = Some(Step { parent: *id, fx: nx as i32, fy: ny as i32, dist: 1 });
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
                    if cell.wall || cell.taken || info[ny][nx].is_some() {
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

        // Une source de protéines reste prioritaire ; à défaut, la case libre
        // la plus proche. Deux passes évitent de mélanger les deux objectifs.
        let pick = |want_source: bool| -> Option<Step> {
            let mut best: Option<(i32, usize, usize)> = None;
            let mut chosen = None;
            for y in 0..height {
                for x in 0..width {
                    let step = match info[y][x] {
                        Some(v) => v,
                        None => continue,
                    };
                    if grid[y][x].protein != want_source {
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
        let chosen = pick(true).or_else(|| pick(false));
        match chosen {
            Some(s) => writeln!(out, "GROW {} {} {} BASIC", s.parent, s.fx, s.fy).unwrap(),
            None => writeln!(out, "WAIT").unwrap(),
        }
        for _ in 1..required {
            writeln!(out, "WAIT").unwrap();
        }
        out.flush().unwrap();
    }
}
