// Xmas Rush — BFS sur les tuiles, poussée qui minimise la distance à la quête.
use std::collections::VecDeque;
use std::io::{self, BufRead, Write};

const N: usize = 7;
const MAXSTEP: usize = 20;
const DX: [i32; 4] = [0, 1, 0, -1];
const DY: [i32; 4] = [-1, 0, 1, 0];
const DIRS: [&str; 4] = ["UP", "RIGHT", "DOWN", "LEFT"];

type Grid = [[i32; N]; N];

#[derive(Clone, Copy, PartialEq, Eq)]
struct Pos { x: usize, y: usize }

fn mask_of(s: &str) -> i32 {
    let b = s.as_bytes();
    (0..4).filter(|&d| b[d] == b'1').map(|d| 1 << d).sum()
}

fn wrap(v: i32) -> usize {
    ((v % N as i32 + N as i32) % N as i32) as usize
}

fn bfs(g: &Grid, s: Pos, t: Pos) -> Option<Vec<usize>> {
    if s == t { return Some(Vec::new()); }
    let mut prev = [[usize::MAX; 2]; N * N];
    let si = s.y * N + s.x;
    let ti = t.y * N + t.x;
    prev[si][0] = si;
    let mut q = VecDeque::new();
    q.push_back(si);
    while let Some(cur) = q.pop_front() {
        if cur == ti { break; }
        let x = cur % N;
        let y = cur / N;
        for d in 0..4 {
            if g[y][x] & (1 << d) == 0 { continue; }
            let nx = x as i32 + DX[d];
            let ny = y as i32 + DY[d];
            if nx < 0 || nx >= N as i32 || ny < 0 || ny >= N as i32 { continue; }
            let (nx, ny) = (nx as usize, ny as usize);
            if g[ny][nx] & (1 << ((d + 2) % 4)) == 0 { continue; }
            let ni = ny * N + nx;
            if prev[ni][0] != usize::MAX { continue; }
            prev[ni][0] = cur;
            prev[ni][1] = d;
            q.push_back(ni);
        }
    }
    if prev[ti][0] == usize::MAX { return None; }
    let mut rev = Vec::new();
    let mut cur = ti;
    while cur != si {
        rev.push(prev[cur][1]);
        cur = prev[cur][0];
    }
    rev.reverse();
    Some(rev)
}

struct Sim { g: Grid, p: Pos, item: Option<Pos> }

fn simulate(g: &Grid, tile: i32, p: Pos, item: Option<Pos>, line: usize, d: usize) -> Sim {
    let mut r = Sim { g: *g, p, item };
    let (ins, pop) = match d {
        1 => ((0, line), (N - 1, line)),
        3 => ((N - 1, line), (0, line)),
        0 => ((line, N - 1), (line, 0)),
        _ => ((line, 0), (line, N - 1)),
    };
    match d {
        1 => for x in (1..N).rev() { r.g[x][line] = r.g[x - 1][line]; },
        3 => for x in 0..N - 1 { r.g[x][line] = r.g[x + 1][line]; },
        0 => for y in 0..N - 1 { r.g[line][y] = r.g[line][y + 1]; },
        _ => for y in (1..N).rev() { r.g[line][y] = r.g[line][y - 1]; },
    }
    r.g[ins.1][ins.0] = tile;
    r.item = match item {
        None => Some(Pos { x: ins.0, y: ins.1 }),
        Some(it) if it.x == pop.0 && it.y == pop.1 => None,
        Some(it) if ((d == 1 || d == 3) && it.y == line)
                 || ((d == 0 || d == 2) && it.x == line) =>
            Some(Pos { x: wrap(it.x as i32 + DX[d]), y: wrap(it.y as i32 + DY[d]) }),
        other => other,
    };
    if ((d == 1 || d == 3) && p.y == line) || ((d == 0 || d == 2) && p.x == line) {
        r.p = Pos { x: wrap(p.x as i32 + DX[d]), y: wrap(p.y as i32 + DY[d]) };
    }
    r
}

fn solve(turn: usize, lines: &mut impl Iterator<Item = String>, out: &mut String) -> bool {
    let mut g = [[0i32; N]; N];
    for y in 0..N {
        let row = match lines.next() { Some(s) => s, None => return false };
        let cells: Vec<&str> = row.split_whitespace().collect();
        for (x, c) in cells.iter().enumerate() { g[y][x] = mask_of(c); }
    }
    let mut p = Pos { x: 0, y: 0 };
    let mut tile = 0;
    for i in 0..2 {
        let line = match lines.next() { Some(s) => s, None => return false };
        let f: Vec<&str> = line.split_whitespace().collect();
        if i == 0 {
            p = Pos { x: f[1].parse().unwrap(), y: f[2].parse().unwrap() };
            tile = mask_of(f[3]);
        }
    }
    let nitems: usize = match lines.next() { Some(s) => s.trim().parse(), None => return false }.unwrap();
    let mut myitem = String::new();
    let mut item: Option<Pos> = None;
    for _ in 0..nitems {
        let line = match lines.next() { Some(s) => s, None => return false };
        let f: Vec<&str> = line.split_whitespace().collect();
        let x: i32 = f[1].parse().unwrap();
        let y: i32 = f[2].parse().unwrap();
        let pid: usize = f[3].parse().unwrap();
        if pid == 0 {
            myitem = f[0].to_string();
            item = if x < 0 { None } else { Some(Pos { x: x as usize, y: y as usize }) };
        }
    }
    let nquests: usize = match lines.next() { Some(s) => s.trim().parse(), None => return false }.unwrap();
    let mut found = false;
    for _ in 0..nquests {
        let line = match lines.next() { Some(s) => s, None => return false };
        let f: Vec<&str> = line.split_whitespace().collect();
        if f[1] == "0" && f[0] == myitem { found = true; }
    }
    if !found { item = None; }

    if turn == 1 {
        match item.and_then(|t| bfs(&g, p, t)) {
            Some(path) if !path.is_empty() => {
                out.push_str("MOVE");
                for d in path.into_iter().take(MAXSTEP) {
                    out.push(' ');
                    out.push_str(DIRS[d]);
                }
            }
            _ => out.push_str("PASS"),
        }
        out.push('\n');
        return true;
    }

    let mut best = i32::MAX;
    let mut bestline = 0;
    let mut bestd = 0;
    for horizontal in [true, false] {
        for line in 0..N {
            let dirs: [usize; 2] = if horizontal { [1, 3] } else { [0, 2] };
            for d in dirs {
                let s = simulate(&g, tile, p, item, line, d);
                let dist = match s.item {
                    None => 1000,
                    Some(it) if it == s.p => -1,
                    Some(it) => match bfs(&s.g, s.p, it) {
                        Some(path) => path.len() as i32,
                        None => 100,
                    },
                };
                if dist < best || (dist == best &&
                    (line < bestline || (line == bestline && d < bestd))) {
                    best = dist; bestline = line; bestd = d;
                }
            }
        }
    }
    out.push_str(&format!("PUSH {} {}\n", bestline, DIRS[bestd]));
    true
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines().map(|l| l.unwrap());
    let stdout = io::stdout();
    let mut handle = stdout.lock();
    let mut out = String::new();
    while let Some(first) = lines.next() {
        if first.is_empty() { continue; }
        let turn: usize = first.trim().parse()?;
        if !solve(turn, &mut lines, &mut out) { break; }
        handle.write_all(out.as_bytes())?;
        handle.flush()?;
        out.clear();
    }
    Ok(())
}
