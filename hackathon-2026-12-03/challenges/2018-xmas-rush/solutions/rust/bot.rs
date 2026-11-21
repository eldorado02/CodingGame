// Xmas Rush Bronze — poussées locales, distance de Manhattan atteignable, anti-stale.
// Port de xmas_v5.py.
use std::cmp::Ordering;
use std::collections::VecDeque;
use std::io::{self, BufRead, Write};

const N: usize = 7;
const CELLS: usize = N * N;
const MAXSTEP: usize = 20;
const MAXT: usize = 12;
const MASKS: usize = 1 << MAXT;
const STATES: usize = CELLS * MASKS;
const DX: [i32; 4] = [0, 1, 0, -1];
const DY: [i32; 4] = [-1, 0, 1, 0];
const DIRS: [&str; 4] = ["UP", "RIGHT", "DOWN", "LEFT"];

type Grid = [[i32; N]; N];

#[derive(Clone, Copy, PartialEq, Eq)]
struct Pos { x: i32, y: i32 }

fn mask_of(s: &str) -> i32 {
    let b = s.as_bytes();
    (0..4).filter(|&d| b[d] == b'1').map(|d| 1 << d).sum()
}

fn wrap(v: i32) -> i32 {
    let r = v % (N as i32);
    if r < 0 { r + N as i32 } else { r }
}

fn connected(g: &Grid, x: i32, y: i32, d: usize) -> bool {
    if (g[y as usize][x as usize] >> d) & 1 == 0 { return false; }
    let nx = x + DX[d];
    let ny = y + DY[d];
    nx >= 0 && nx < N as i32 && ny >= 0 && ny < N as i32 &&
        ((g[ny as usize][nx as usize] >> ((d + 2) % 4)) & 1) != 0
}

fn bfs_dist(g: &Grid, s: Pos, t: Pos) -> i32 {
    if s == t { return 0; }
    let mut dist = vec![-1i32; CELLS];
    let si = (s.y as usize) * N + s.x as usize;
    dist[si] = 0;
    let mut q = VecDeque::new();
    q.push_back(s);
    while let Some(cur) = q.pop_front() {
        for d in 0..4 {
            if !connected(g, cur.x, cur.y, d) { continue; }
            let nx = cur.x + DX[d];
            let ny = cur.y + DY[d];
            if nx == t.x && ny == t.y { return dist[(cur.y as usize) * N + cur.x as usize] + 1; }
            let ni = ny as usize * N + nx as usize;
            if dist[ni] == -1 {
                dist[ni] = dist[(cur.y as usize) * N + cur.x as usize] + 1;
                q.push_back(Pos { x: nx, y: ny });
            }
        }
    }
    999
}

fn manhattan(p: Pos, targets: &[Pos]) -> i32 {
    targets.iter().map(|t| (p.x - t.x).abs() + (p.y - t.y).abs()).min().unwrap_or(1_000_000)
}

#[derive(Clone, Copy)]
struct Par { px: i32, py: i32, pm: u16, d: i8 }

fn multi_bfs(g: &Grid, p: Pos, targets: &[Pos]) -> Vec<usize> {
    let nt = targets.len();
    if nt == 0 { return Vec::new(); }
    let mut parent = vec![Par { px: -1, py: -1, pm: 0, d: -1 }; STATES];
    let si = (p.y as usize * N + p.x as usize) * MASKS;
    parent[si] = Par { px: p.x, py: p.y, pm: 0, d: -2 }; // racine
    #[derive(Clone, Copy)]
    struct State { x: i32, y: i32, mask: u16, steps: usize }
    let mut q = VecDeque::new();
    q.push_back(State { x: p.x, y: p.y, mask: 0, steps: 0 });
    let mut best_got = 0usize;
    let mut best_steps = 0usize;
    let mut best_md = manhattan(p, targets);
    let mut bx = p.x;
    let mut by = p.y;
    let mut bm = 0u16;
    while let Some(st) = q.pop_front() {
        let md = manhattan(Pos { x: st.x, y: st.y }, targets);
        let got = st.mask.count_ones() as usize;
        if got > best_got || (got == best_got && md < best_md) ||
           (got == best_got && md == best_md && st.steps < best_steps) {
            best_got = got; best_steps = st.steps; best_md = md;
            bx = st.x; by = st.y; bm = st.mask;
        }
        if st.steps >= MAXSTEP { continue; }
        for d in 0..4 {
            if !connected(g, st.x, st.y, d) { continue; }
            let nx = st.x + DX[d];
            let ny = st.y + DY[d];
            let mut nm = st.mask;
            for (i, t) in targets.iter().enumerate().rev() {
                if t.x == nx && t.y == ny { nm |= 1u16 << i; break; }
            }
            let idx = (ny as usize * N + nx as usize) * MASKS + nm as usize;
            if parent[idx].d != -1 { continue; }
            parent[idx] = Par { px: st.x, py: st.y, pm: st.mask, d: d as i8 };
            q.push_back(State { x: nx, y: ny, mask: nm, steps: st.steps + 1 });
        }
    }
    let mut rev = Vec::new();
    let (mut x, mut y, mut m) = (bx, by, bm);
    loop {
        let par = parent[(y as usize * N + x as usize) * MASKS + m as usize];
        if par.d < 0 { break; }
        rev.push(par.d as usize);
        x = par.px; y = par.py; m = par.pm;
    }
    rev.reverse();
    rev
}

fn reachable_manhattan(g: &Grid, p: Pos, targets: &[Pos]) -> i32 {
    if targets.is_empty() { return 0; }
    let mut best = manhattan(p, targets);
    if best == 0 { return 0; }
    let mut dist = vec![-1i32; CELLS];
    let si = p.y as usize * N + p.x as usize;
    dist[si] = 0;
    let mut q = VecDeque::new();
    q.push_back(p);
    while let Some(cur) = q.pop_front() {
        let steps = dist[cur.y as usize * N + cur.x as usize] as usize;
        if steps >= MAXSTEP { continue; }
        for d in 0..4 {
            if !connected(g, cur.x, cur.y, d) { continue; }
            let nx = cur.x + DX[d];
            let ny = cur.y + DY[d];
            let ni = ny as usize * N + nx as usize;
            if dist[ni] != -1 { continue; }
            dist[ni] = steps as i32 + 1;
            let md = manhattan(Pos { x: nx, y: ny }, targets);
            if md < best { best = md; }
            if best == 0 { return 0; }
            q.push_back(Pos { x: nx, y: ny });
        }
    }
    best
}

fn target_path(g: &Grid, p: Pos, targets: &[Pos]) -> Vec<usize> {
    let mut best: Option<(i32, Pos)> = None;
    for t in targets {
        let d = bfs_dist(g, p, *t);
        if d < 999 && (best.is_none() || d < best.unwrap().0) { best = Some((d, *t)); }
    }
    let (_bd, t) = match best { Some(v) => v, None => return Vec::new() };
    if p == t { return Vec::new(); }
    path_to(g, p, t)
}

fn path_to(g: &Grid, p: Pos, t: Pos) -> Vec<usize> {
    let si = p.y as usize * N + p.x as usize;
    let ti = t.y as usize * N + t.x as usize;
    let mut prev = vec![usize::MAX; CELLS];
    prev[si] = si;
    let mut q = VecDeque::new();
    q.push_back(si);
    while let Some(cur) = q.pop_front() {
        if cur == ti { break; }
        let x = (cur % N) as i32;
        let y = (cur / N) as i32;
        for d in 0..4 {
            if !connected(g, x, y, d) { continue; }
            let nx = x + DX[d];
            let ny = y + DY[d];
            let ni = ny as usize * N + nx as usize;
            if prev[ni] != usize::MAX { continue; }
            prev[ni] = cur;
            q.push_back(ni);
        }
    }
    if prev[ti] == usize::MAX { return Vec::new(); }
    let mut rev = Vec::new();
    let mut cur = ti;
    while cur != si && rev.len() < MAXSTEP {
        let par = prev[cur];
        let x = (cur % N) as i32;
        let y = (cur / N) as i32;
        let px = (par % N) as i32;
        let py = (par / N) as i32;
        for d in 0..4 {
            if px + DX[d] == x && py + DY[d] == y { rev.push(d); break; }
        }
        cur = par;
    }
    rev.reverse();
    rev
}

fn edge_path(g: &Grid, p: Pos) -> Vec<usize> {
    let mut goals = Vec::new();
    for x in 0..N as i32 {
        goals.push(Pos { x, y: 0 });
        goals.push(Pos { x, y: (N - 1) as i32 });
    }
    for y in 0..N as i32 {
        goals.push(Pos { x: 0, y });
        goals.push(Pos { x: (N - 1) as i32, y });
    }
    let mut best: Option<(i32, Pos)> = None;
    for t in &goals {
        let d = bfs_dist(g, p, *t);
        if d < 999 && (best.is_none() || d < best.unwrap().0) { best = Some((d, *t)); }
    }
    let (_bd, t) = match best { Some(v) => v, None => return Vec::new() };
    if p == t { return Vec::new(); }
    let si = p.y as usize * N + p.x as usize;
    let ti = t.y as usize * N + t.x as usize;
    let mut prev = vec![usize::MAX; CELLS];
    prev[si] = si;
    let mut q = VecDeque::new();
    q.push_back(si);
    while let Some(cur) = q.pop_front() {
        if cur == ti { break; }
        let x = (cur % N) as i32;
        let y = (cur / N) as i32;
        for d in 0..4 {
            if !connected(g, x, y, d) { continue; }
            let nx = x + DX[d];
            let ny = y + DY[d];
            let ni = ny as usize * N + nx as usize;
            if prev[ni] != usize::MAX { continue; }
            prev[ni] = cur;
            q.push_back(ni);
        }
    }
    if prev[ti] == usize::MAX { return Vec::new(); }
    let mut rev = Vec::new();
    let mut cur = ti;
    while cur != si && rev.len() < MAXSTEP {
        let par = prev[cur];
        let x = (cur % N) as i32;
        let y = (cur / N) as i32;
        let px = (par % N) as i32;
        let py = (par / N) as i32;
        for d in 0..4 {
            if px + DX[d] == x && py + DY[d] == y { rev.push(d); break; }
        }
        cur = par;
    }
    rev.reverse();
    rev
}

struct Sim {
    g: Grid,
    p: Pos,
    targets: Vec<Pos>,
    popped: i32,
    instant: i32,
}

fn simulate(g: &Grid, tile: i32, p: Pos, targets: &[Pos], hand: i32, line: i32, d: usize) -> Sim {
    let mut r = Sim { g: *g, p, targets: Vec::new(), popped: 0, instant: 0 };
    let horizontal = d == 1 || d == 3;
    let (ins, pop) = match d {
        1 => (Pos { x: 0, y: line }, Pos { x: (N - 1) as i32, y: line }),
        3 => (Pos { x: (N - 1) as i32, y: line }, Pos { x: 0, y: line }),
        0 => (Pos { x: line, y: (N - 1) as i32 }, Pos { x: line, y: 0 }),
        _ => (Pos { x: line, y: 0 }, Pos { x: line, y: (N - 1) as i32 }),
    };
    let l = line as usize;
    match d {
        1 => for x in (1..N).rev() { r.g[x][l] = r.g[x - 1][l]; },
        3 => for x in 0..N - 1 { r.g[x][l] = r.g[x + 1][l]; },
        0 => for y in 0..N - 1 { r.g[l][y] = r.g[l][y + 1]; },
        _ => for y in (1..N).rev() { r.g[l][y] = r.g[l][y - 1]; },
    }
    r.g[ins.y as usize][ins.x as usize] = tile;
    let on_line = if horizontal { p.y == line } else { p.x == line };
    if on_line {
        r.p = Pos { x: wrap(p.x + DX[d]), y: wrap(p.y + DY[d]) };
    }
    for &t in targets {
        if t == pop { r.popped += 1; }
        else if (horizontal && t.y == line) || (!horizontal && t.x == line) {
            r.targets.push(Pos { x: wrap(t.x + DX[d]), y: wrap(t.y + DY[d]) });
        } else {
            r.targets.push(t);
        }
    }
    r.instant = if on_line { hand } else { 0 };
    if hand != 0 && !on_line { r.targets.push(ins); }
    r
}

#[derive(Clone)]
struct Candidate { score: [i32; 8], cmd: String }

impl Candidate {
    fn better(&self, other: &Candidate) -> bool {
        for i in 0..8 {
            if self.score[i] != other.score[i] { return self.score[i] > other.score[i]; }
        }
        self.cmd > other.cmd
    }
}

fn best_push(g: &Grid, p: Pos, tile: i32, targets: &[Pos], hand: i32,
             last_push: &str, stale: i32) -> String {
    let mut cand: Vec<Candidate> = Vec::new();
    let lines_y = [p.y - 1, p.y, p.y + 1];
    let lines_x = [p.x - 1, p.x, p.x + 1];
    for hor in [true, false] {
        let lines = if hor { &lines_y } else { &lines_x };
        for &line in lines {
            if line < 0 || line >= N as i32 { continue; }
            for di in 0..2 {
                let d = if hor { if di == 0 { 1 } else { 3 } } else { if di == 0 { 0 } else { 2 } };
                let r = simulate(g, tile, p, targets, hand, line, d);
                let md = if r.targets.is_empty() { 0 } else { reachable_manhattan(&r.g, r.p, &r.targets) };
                let collectable = r.targets.iter()
                    .filter(|t| bfs_dist(&r.g, r.p, **t) <= 20).count() as i32;
                let conn = (0..4).filter(|&k| connected(&r.g, r.p.x, r.p.y, k)).count() as i32;
                cand.push(Candidate {
                    score: [r.instant, -r.popped, -md, collectable, conn, -hand, -line, -(d as i32)],
                    cmd: format!("PUSH {} {}", line, DIRS[d]),
                });
            }
        }
    }
    cand.sort_by(|a, b| {
        if a.better(b) { Ordering::Less } else if b.better(a) { Ordering::Greater } else { Ordering::Equal }
    });
    if stale >= 3 && cand.len() > 1 {
        for c in &cand { if c.cmd != last_push { return c.cmd.clone(); } }
    }
    cand[0].cmd.clone()
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines().map(|l| l.unwrap());
    let stdout = io::stdout();
    let mut handle = stdout.lock();
    let mut last_push = String::new();
    let mut stale = 0i32;
    while let Some(first) = lines.next() {
        if first.is_empty() { continue; }
        let turn: usize = first.trim().parse()?;
        let mut out = String::new();
        let mut g = [[0i32; N]; N];
        for y in 0..N {
            let row = match lines.next() { Some(s) => s, None => return Ok(()) };
            for (x, c) in row.split_whitespace().enumerate() { g[y][x] = mask_of(c); }
        }
        let mut p = Pos { x: 0, y: 0 };
        let mut tile = 0;
        for i in 0..2 {
            let line = match lines.next() { Some(s) => s, None => return Ok(()) };
            let f: Vec<&str> = line.split_whitespace().collect();
            if i == 0 {
                p = Pos { x: f[1].parse()?, y: f[2].parse()? };
                tile = mask_of(f[3]);
            }
        }
        let nitems: usize = match lines.next() { Some(s) => s.trim().parse()?, None => return Ok(()) };
        let mut items: Vec<(String, i32, i32)> = Vec::new();
        for _ in 0..nitems {
            let line = match lines.next() { Some(s) => s, None => return Ok(()) };
            let f: Vec<&str> = line.split_whitespace().collect();
            let x: i32 = f[1].parse()?;
            let y: i32 = f[2].parse()?;
            let pid: usize = f[3].parse()?;
            if pid == 0 { items.push((f[0].to_string(), x, y)); }
        }
        let nquests: usize = match lines.next() { Some(s) => s.trim().parse()?, None => return Ok(()) };
        let mut quests: Vec<String> = Vec::new();
        for _ in 0..nquests {
            let line = match lines.next() { Some(s) => s, None => return Ok(()) };
            let f: Vec<&str> = line.split_whitespace().collect();
            if f[1] == "0" { quests.push(f[0].to_string()); }
        }
        let mut targets: Vec<Pos> = Vec::new();
        let mut hand = 0i32;
        for q in &quests {
            if let Some((_nm, x, y)) = items.iter().find(|(nm, _, _)| nm == q) {
                if *x == -1 { hand += 1; }
                else if *x != -2 { targets.push(Pos { x: *x, y: *y }); }
            }
        }
        if turn == 1 {
            let mut moves = multi_bfs(&g, p, &targets);
            if moves.is_empty() && !targets.is_empty() { moves = target_path(&g, p, &targets); }
            if moves.is_empty() && hand > 0 { moves = edge_path(&g, p); }
            if moves.is_empty() { out.push_str("PASS"); }
            else {
                out.push_str("MOVE");
                for d in moves { out.push(' '); out.push_str(DIRS[d]); }
            }
            out.push('\n');
        } else {
            let cmd = best_push(&g, p, tile, &targets, hand, &last_push, stale);
            if cmd == last_push { stale += 1; }
            else { stale = 0; last_push = cmd.clone(); }
            out.push_str(&cmd);
            out.push('\n');
        }
        handle.write_all(out.as_bytes())?;
        handle.flush()?;
    }
    Ok(())
}
