// Hypersonic Wood — BFS vers les boîtes, bombes sur cibles rentables.
use std::collections::VecDeque;
use std::io::{self, BufRead, Write};

const W: usize = 13;
const H: usize = 11;
const RANGE: i32 = 3;
const DX: [i32; 4] = [1, -1, 0, 0];
const DY: [i32; 4] = [0, 0, 1, -1];

fn blast_boxes(grid: &[String], x: usize, y: usize) -> usize {
    let mut total = 0;
    for d in 0..4 {
        for s in 1..=RANGE {
            let nx = x as i32 + DX[d] * s;
            let ny = y as i32 + DY[d] * s;
            if nx < 0 || nx >= W as i32 || ny < 0 || ny >= H as i32 {
                break;
            }
            if grid[ny as usize].as_bytes()[nx as usize] == b'0' {
                total += 1;
                break;
            }
        }
    }
    total
}

// mode 0=boîte, 1=centre, 2=fuite hors portée 3.
fn bfs_next(grid: &[String], sx: usize, sy: usize, blocked: &[Vec<bool>; H], mode: i32) -> Option<(usize, usize)> {
    let mut parent = vec![usize::MAX; H * W];
    let mut seen = vec![vec![false; W]; H];
    let mut q = VecDeque::new();
    q.push_back((sx, sy));
    seen[sy][sx] = true;
    while let Some((x, y)) = q.pop_front() {
        for d in 0..4 {
            let nx = x as i32 + DX[d];
            let ny = y as i32 + DY[d];
            if nx < 0 || nx >= W as i32 || ny < 0 || ny >= H as i32 {
                continue;
            }
            let (nx, ny) = (nx as usize, ny as usize);
            if seen[ny][nx] || blocked[ny][nx] || grid[ny].as_bytes()[nx] == b'0' {
                continue;
            }
            seen[ny][nx] = true;
            parent[ny * W + nx] = y * W + x;
            let done = match mode {
                0 => (0..4).any(|k| {
                    let ax = nx as i32 + DX[k];
                    let ay = ny as i32 + DY[k];
                    ax >= 0 && ax < W as i32 && ay >= 0 && ay < H as i32
                        && grid[ay as usize].as_bytes()[ax as usize] == b'0'
                }),
                1 => nx == W / 2 && ny == H / 2,
                _ => (nx as i32 - sx as i32).abs() + (ny as i32 - sy as i32).abs() > 3,
            };
            if done {
                let mut idx = ny * W + nx;
                while parent[idx] != sy * W + sx && parent[idx] != usize::MAX {
                    idx = parent[idx];
                }
                return Some((idx % W, idx / W));
            }
            q.push_back((nx, ny));
        }
    }
    None
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();
    let mut next = || -> Result<Option<String>, Box<dyn std::error::Error>> {
        match lines.next() {
            Some(line) => Ok(Some(line?)),
            None => Ok(None),
        }
    };
    let header = match next()? {
        Some(line) => line,
        None => return Ok(()),
    };
    let h: Vec<i32> = header.split_whitespace().map(|v| v.parse()).collect::<Result<_, _>>()?;
    let my_id = h[2];

    let stdout = io::stdout();
    let mut out = stdout.lock();
    loop {
        let mut grid = Vec::with_capacity(H);
        for _ in 0..H {
            match next()? {
                Some(line) => grid.push(line),
                None => return Ok(()),
            }
        }
        let n: usize = match next()? {
            Some(line) => line.trim().parse()?,
            None => return Ok(()),
        };
        let mut me: Option<(usize, usize)> = None;
        let mut bombs = 0;
        let mut blocked: [Vec<bool>; H] = std::array::from_fn(|_| vec![false; W]);
        for _ in 0..n {
            let line = match next()? {
                Some(line) => line,
                None => return Ok(()),
            };
            let v: Vec<i32> = line.split_whitespace().map(|x| x.parse()).collect::<Result<_, _>>()?;
            let (kind, owner, x, y) = (v[0], v[1], v[2], v[3]);
            if kind == 0 && owner == my_id {
                me = Some((x as usize, y as usize));
                bombs = v[4];
            } else if kind == 1 && x >= 0 && x < W as i32 && y >= 0 && y < H as i32 {
                blocked[y as usize][x as usize] = true;
            }
        }
        match me {
            None => writeln!(out, "MOVE {} {}", W / 2, H / 2)?,
            Some((mx, my)) => {
                if bombs > 0 && blast_boxes(&grid, mx, my) > 0 {
                    let mut b2 = blocked.clone();
                    b2[my][mx] = true;
                    let (nx, ny) = bfs_next(&grid, mx, my, &b2, 2).unwrap_or((mx, my));
                    writeln!(out, "BOMB {nx} {ny}")?;
                } else {
                    let (nx, ny) = bfs_next(&grid, mx, my, &blocked, 0)
                        .or_else(|| bfs_next(&grid, mx, my, &blocked, 1))
                        .unwrap_or((W / 2, H / 2));
                    writeln!(out, "MOVE {nx} {ny}")?;
                }
            }
        }
        out.flush()?;
    }
}
