// Tron Battle — Voronoi 1-ply + greedy primé. Même logique que Python/C/C++.
use std::collections::VecDeque;
use std::io::{self, BufRead, Write};

const W: usize = 30;
const H: usize = 20;
const INF: i32 = 99;
const DX: [i32; 4] = [0, 0, -1, 1];
const DY: [i32; 4] = [-1, 1, 0, 0];
const DIR_NAME: [&str; 4] = ["UP", "DOWN", "LEFT", "RIGHT"];

type Grid = [[bool; W]; H];
type Dist = [[i32; W]; H];

fn neigh() -> Vec<Vec<Vec<usize>>> {
    let mut nb = vec![vec![Vec::new(); W]; H];
    for y in 0..H {
        for x in 0..W {
            for d in 0..4 {
                let nx = x as i32 + DX[d];
                let ny = y as i32 + DY[d];
                if nx >= 0 && nx < W as i32 && ny >= 0 && ny < H as i32 {
                    nb[y][x].push(nx as usize + ny as usize * W);
                }
            }
        }
    }
    nb
}

fn bfs(sx: usize, sy: usize, blocked: &Grid, nb: &[Vec<Vec<usize>>]) -> Dist {
    let mut dist = [[INF; W]; H];
    // La tête est occupée mais reste l'origine légitime du BFS.
    let mut q = VecDeque::new();
    q.push_back(sx + sy * W);
    dist[sy][sx] = 0;
    while let Some(cell) = q.pop_front() {
        let (x, y) = (cell % W, cell / W);
        for &v in &nb[y][x] {
            let (nx, ny) = (v % W, v / W);
            if !blocked[ny][nx] && dist[ny][nx] == INF {
                dist[ny][nx] = dist[y][x] + 1;
                q.push_back(v);
            }
        }
    }
    dist
}

fn greedy_fill(sx: usize, sy: usize, blocked: &Grid, limit: usize, nb: &[Vec<Vec<usize>>]) -> i32 {
    let mut local = *blocked;
    let (mut x, mut y) = (sx, sy);
    let mut count = 0;
    while count < limit {
        let mut best = (i32::MAX, 0usize, 0usize);
        let mut found = false;
        for &v in &nb[y][x] {
            let (nx, ny) = (v % W, v / W);
            if local[ny][nx] { continue; }
            let mut free_next = 0;
            for &w in &nb[ny][nx] {
                let (ax, ay) = (w % W, w / W);
                if !local[ay][ax] { free_next += 1; }
            }
            let key = (free_next, -(ny as i32), -(nx as i32));
            if !found || key < (best.0, -(best.2 as i32), -(best.1 as i32)) {
                best = (free_next, nx, ny);
                found = true;
            }
        }
        if !found { break; }
        x = best.1; y = best.2; local[y][x] = true;
        count += 1;
    }
    count as i32
}

fn main() {
    let nb = neigh();
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();
    let first = match lines.next() { Some(Ok(l)) => l, _ => return };
    let mut line = first;
    let mut grid = [[false; W]; H];
    let mut heads: Vec<(i32,i32)> = Vec::new();
    let mut seen: Vec<Grid> = Vec::new();
    let mut initialized = false;
    let stdout = io::stdout();
    let mut out = stdout.lock();

    loop {
        let parts: Vec<i32> = line.split_whitespace().filter_map(|s| s.parse().ok()).collect();
        if parts.len() >= 2 {
            let n = parts[0] as usize;
            let p = parts[1] as usize;
            heads.clear();
            for _ in 0..n {
                let l = match lines.next() { Some(Ok(l)) => l, _ => return };
                let f: Vec<i32> = l.split_whitespace().filter_map(|s| s.parse().ok()).collect();
                if f.len() < 4 { return; }
                heads.push((f[2], f[3]));
            }
            if !initialized {
                seen = vec![[[false; W]; H]; n];
                initialized = true;
            }
            for row in grid.iter_mut() { row.fill(false); }
            for i in 0..n {
                if heads[i].0 < 0 { seen[i] = [[false; W]; H]; continue; }
                let (hx, hy) = (heads[i].0 as usize, heads[i].1 as usize);
                seen[i][hy][hx] = true;
                for y in 0..H {
                    for x in 0..W {
                        if seen[i][y][x] { grid[y][x] = true; }
                    }
                }
            }
            let (hx, hy) = (heads[p].0, heads[p].1);
            if hx < 0 {
                writeln!(out, "UP").ok(); out.flush().ok();
                line = match lines.next() { Some(Ok(l)) => l, _ => break };
                continue;
            }
            let me = (hx as usize, hy as usize);
            let mut blocked = grid;
            for &(x, y) in &heads {
                if x >= 0 { blocked[y as usize][x as usize] = true; }
            }
            blocked[me.1][me.0] = false;

            let mut best: Option<(i32,i32,i32,i32)> = None; // (score, -ny, -nx, dir)
            for d in 0..4 {
                let nx = me.0 as i32 + DX[d];
                let ny = me.1 as i32 + DY[d];
                if nx < 0 || nx >= W as i32 || ny < 0 || ny >= H as i32 { continue; }
                let (nx, ny) = (nx as usize, ny as usize);
                if blocked[ny][nx] { continue; }
                let mut trial = blocked;
                trial[me.1][me.0] = true;
                trial[ny][nx] = true;
                let dm = bfs(nx, ny, &trial, &nb);
                let mut ds = Vec::new();
                for i in 0..n {
                    if i != p && heads[i].0 >= 0 {
                        ds.push(bfs(heads[i].0 as usize, heads[i].1 as usize, &trial, &nb));
                    }
                }
                let mut voronoi = 0i32;
                let mut reach = 0i32;
                for y in 0..H {
                    for x in 0..W {
                        if trial[y][x] || dm[y][x] == INF { continue; }
                        reach += 1;
                        let mut b = INF;
                        for dd in &ds { b = b.min(dd[y][x]); }
                        if dm[y][x] < b { voronoi += 1; }
                        else if dm[y][x] > b { voronoi -= 1; }
                    }
                }
                let mut mob = 0;
                for &v in &nb[ny][nx] {
                    if !trial[v / W][v % W] { mob += 1; }
                }
                let fill = greedy_fill(nx, ny, &trial, 600, &nb);
                let score = 100000 * voronoi + 100 * reach + 10 * mob + fill;
                let key = (score, -(ny as i32), -(nx as i32), d as i32);
                if best.map_or(true, |b| key > b) { best = Some(key); }
            }
            let dir = best.map(|b| b.3 as usize).unwrap_or(0);
            writeln!(out, "{}", DIR_NAME[dir]).ok();
            out.flush().ok();
        }
        line = match lines.next() { Some(Ok(l)) => l, _ => break };
    }
}
