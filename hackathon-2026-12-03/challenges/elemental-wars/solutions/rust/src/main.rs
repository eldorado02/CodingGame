// Elemental Wars — fuir le prédateur, poursuivre la proie.
use std::collections::VecDeque;
use std::io::{self, BufRead, Write};

const DX: [i32; 4] = [0, 1, 0, -1];
const DY: [i32; 4] = [-1, 0, 1, 0];

fn prey_of(t: u8) -> u8 {
    match t { b'W' => b'F', b'F' => b'P', _ => b'W' }
}
fn pred_of(t: u8) -> u8 {
    match t { b'W' => b'P', b'F' => b'W', _ => b'F' }
}

fn bfs(grid: &[Vec<u8>], sx: i32, sy: i32) -> Vec<Vec<i32>> {
    let n = grid.len();
    let mut dist = vec![vec![-1; n]; n];
    if sx < 0 || sy < 0 || sx >= n as i32 || sy >= n as i32 {
        return dist;
    }
    let (sx, sy) = (sx as usize, sy as usize);
    if grid[sy][sx] == b'#' { return dist; }
    dist[sy][sx] = 0;
    let mut q = VecDeque::new();
    q.push_back((sx, sy));
    while let Some((x, y)) = q.pop_front() {
        for d in 0..4 {
            let nx = x as i32 + DX[d];
            let ny = y as i32 + DY[d];
            if nx < 0 || ny < 0 || nx >= n as i32 || ny >= n as i32 { continue; }
            let (nx, ny) = (nx as usize, ny as usize);
            if grid[ny][nx] == b'#' || dist[ny][nx] >= 0 { continue; }
            dist[ny][nx] = dist[y][x] + 1;
            q.push_back((nx, ny));
        }
    }
    dist
}

fn main() {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines().map(|l| l.unwrap());
    let tribe = lines.next().unwrap().trim().as_bytes()[0];
    let n: usize = lines.next().unwrap().trim().parse().unwrap();
    let grid: Vec<Vec<u8>> = (0..n)
        .map(|_| lines.next().unwrap().trim().as_bytes().to_vec())
        .collect();
    let stdout = io::stdout();
    let mut out = stdout.lock();
    while let Some(line) = lines.next() {
        if line.trim().is_empty() { continue; }
        let count: usize = match lines.next() {
            Some(l) => l.trim().parse().unwrap(),
            None => break,
        };
        let mut mine: Option<(i32, i32, i32)> = None; // id, x, y
        let mut mine_prisoner = false;
        let mut prey: Option<(i32, i32)> = None;
        let mut pred: Option<(i32, i32)> = None;
        let mut pred_prisoner = false;
        let mut prey_prisoner = false;
        for _ in 0..count {
            let unit_line = lines.next().unwrap();
            let parts: Vec<&str> = unit_line.split_whitespace().collect();
            let id: i32 = parts[0].parse().unwrap();
            let t = parts[1].as_bytes()[0];
            let x: i32 = parts[2].parse().unwrap();
            let y: i32 = parts[3].parse().unwrap();
            let prisoner: i32 = parts[4].parse().unwrap();
            if t == tribe { mine = Some((id, x, y)); mine_prisoner = prisoner == 1; }
            if t == prey_of(tribe) { prey = Some((x, y)); prey_prisoner = prisoner == 1; }
            if t == pred_of(tribe) { pred = Some((x, y)); pred_prisoner = prisoner == 1; }
        }
        let (id, mx, my) = match mine { Some(v) => v, None => { writeln!(out, "0 WAIT").unwrap(); out.flush().unwrap(); continue; } };
        let (px, py) = match prey { Some(v) => v, None => { writeln!(out, "0 WAIT").unwrap(); out.flush().unwrap(); continue; } };
        if mine_prisoner || prey_prisoner {
            writeln!(out, "{} WAIT", id).unwrap(); out.flush().unwrap(); continue;
        }
        let (qx, qy) = if pred_prisoner { (100, 100) } else { pred.unwrap_or((100, 100)) };
        let dprey = bfs(&grid, px, py);
        let dpred = bfs(&grid, qx, qy);
        let mut best = (mx, my);
        let mut best_score = i32::MIN;
        let mut options = vec![(mx, my)];
        for d in 0..4 {
            let x = mx + DX[d];
            let y = my + DY[d];
            if x >= 0 && y >= 0 && (x as usize) < n && (y as usize) < n
                && grid[y as usize][x as usize] != b'#' {
                options.push((x, y));
            }
        }
        for (x, y) in options {
            let dp = dprey[y as usize][x as usize];
            let dq = dpred[y as usize][x as usize];
            let dp = if dp < 0 { 100 } else { dp };
            let dq = if dq < 0 { 100 } else { dq };
            let mut score = -100 * dp + 20 * dq.min(6);
            if x == px && y == py && !(x == qx && y == qy) { score += 10000; }
            if x == qx && y == qy { score -= 10000; }
            if dq <= 1 && dp > 1 { score -= 2000; }
            if score > best_score || (score == best_score && (x < best.0 || (x == best.0 && y < best.1))) {
                best_score = score;
                best = (x, y);
            }
        }
        writeln!(out, "{} MOVE {} {}", id, best.0, best.1).unwrap();
        out.flush().unwrap();
    }
}
