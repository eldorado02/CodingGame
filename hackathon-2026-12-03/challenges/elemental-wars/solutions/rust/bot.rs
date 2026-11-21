// Elemental Wars (Wood) — BFS multi-source : poursuivre la proie, fuir le prédateur.
use std::collections::VecDeque;
use std::io::{self, BufRead, Write};

const DX: [i32; 4] = [-1, 0, 1, 0];
const DY: [i32; 4] = [0, 1, 0, -1];

fn prey_of(t: u8) -> u8 {
    match t { b'W' => b'F', b'F' => b'P', _ => b'W' }
}
fn pred_of(t: u8) -> u8 {
    match t { b'W' => b'P', b'F' => b'W', _ => b'F' }
}

fn distances(sources: &[(usize, usize)], grid: &[Vec<u8>]) -> Vec<Vec<i32>> {
    let n = grid.len();
    let mut dist = vec![vec![-1i32; n]; n];
    let mut q = VecDeque::new();
    for &(x, y) in sources {
        if x < n && y < n && dist[y][x] < 0 {
            dist[y][x] = 0;
            q.push_back((x, y));
        }
    }
    while let Some((x, y)) = q.pop_front() {
        for d in 0..4 {
            let nx = x as i32 + DX[d];
            let ny = y as i32 + DY[d];
            if nx < 0 || ny < 0 || nx >= n as i32 || ny >= n as i32 { continue; }
            let (nx, ny) = (nx as usize, ny as usize);
            if grid[ny][nx] != b'#' && dist[ny][nx] < 0 {
                dist[ny][nx] = dist[y][x] + 1;
                q.push_back((nx, ny));
            }
        }
    }
    dist
}

fn main() {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();
    let tribe: Vec<u8> = match lines.next() {
        Some(Ok(s)) => s.trim().bytes().collect(),
        _ => return,
    };
    if tribe.is_empty() { return; }
    let n: usize = match lines.next() {
        Some(Ok(s)) => s.trim().parse().unwrap_or(0),
        _ => return,
    };
    let mut grid = Vec::with_capacity(n);
    for _ in 0..n {
        if let Some(Ok(s)) = lines.next() { grid.push(s.trim().bytes().collect()); }
        else { return; }
    }
    let stdout = io::stdout();
    let mut out = stdout.lock();
    loop {
        let _scores: String = match lines.next() { Some(Ok(s)) => s, _ => return };
        let count: usize = match lines.next() {
            Some(Ok(s)) => s.trim().parse().unwrap_or(0),
            _ => return,
        };
        let mut mine: Vec<(usize, usize, usize)> = Vec::new();
        let mut prey: Vec<(usize, usize)> = Vec::new();
        let mut predator: Vec<(usize, usize)> = Vec::new();
        for _ in 0..count {
            let line = match lines.next() { Some(Ok(s)) => s, _ => return };
            let p: Vec<&str> = line.split_whitespace().collect();
            if p.len() < 5 { return; }
            let id: usize = p[0].parse().unwrap_or(0);
            let t = p[1].as_bytes()[0];
            let x: usize = p[2].parse().unwrap_or(0);
            let y: usize = p[3].parse().unwrap_or(0);
            if p[4] == "1" { continue; }
            if t == tribe[0] { mine.push((id, x, y)); }
            else if t == prey_of(tribe[0]) { prey.push((x, y)); }
            else if t == pred_of(tribe[0]) { predator.push((x, y)); }
        }
        let dp = distances(&prey, &grid);
        let dd = distances(&predator, &grid);
        let mut actions: Vec<String> = Vec::new();
        for (id, x, y) in mine {
            let mut best: Option<(i32, i32, i32, i32, usize)> = None; // key,order,nx,ny,-
            for order in 0..5usize {
                let (ax, ay) = if order == 0 { (0, 0) } else { (DX[order - 1], DY[order - 1]) };
                let nx = x as i32 + ax;
                let ny = y as i32 + ay;
                if nx < 0 || ny < 0 || nx >= n as i32 || ny >= n as i32 { continue; }
                let (nx, ny) = (nx as usize, ny as usize);
                if order != 0 && grid[ny][nx] == b'#' { continue; }
                let mut score = 0i32;
                if prey.iter().any(|&(px, py)| px == nx && py == ny) { score += 1000; }
                if dp[ny][nx] >= 0 { score -= 2 * dp[ny][nx]; }
                if dd[ny][nx] >= 0 {
                    score += 3 * dd[ny][nx];
                    if dd[ny][nx] <= 1 { score -= 50; }
                }
                let key = (score, -(order as i32), -(ny as i32), -(nx as i32));
                if best.as_ref().map_or(true, |b| key > (b.0, b.1, b.2, b.3)) {
                    best = Some((key.0, key.1, key.2, key.3, order));
                }
            }
            match best {
                Some((_, _, nx, ny, order)) if order != 0 => {
                    actions.push(format!("{} MOVE {} {}", id, -ny, -nx));
                }
                _ => actions.push(format!("{} WAIT", id)),
            }
        }
        let line = if actions.is_empty() { "WAIT".to_string() } else { actions.join(";") };
        writeln!(out, "{}", line).ok();
        out.flush().ok();
    }
}
