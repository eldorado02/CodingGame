// Xmas Rush (Silver) — port Rust du prototype Python v17 validé 3/5 contre 2215206.
use std::io::{self, Read, Write};

const N: i32 = 7;
const NZ: usize = N as usize;
const CELLS: usize = (N * N) as usize;
const MAXSTEP: usize = 20;
const STSZ: usize = CELLS * 8;
const DX: [i32; 4] = [0, 1, 0, -1];
const DY: [i32; 4] = [-1, 0, 1, 0];
const DIRS: [&str; 4] = ["UP", "RIGHT", "DOWN", "LEFT"];

type Grid = [[i32; NZ]; NZ];

#[derive(Clone, Copy, PartialEq, Eq)]
struct Pos { x: i32, y: i32 }

fn mask_of(s: &str) -> i32 {
    let b = s.as_bytes();
    (0..4).filter(|&d| b[d] == b'1').map(|d| 1 << d).sum()
}

fn wrap(v: i32) -> i32 {
    let r = v % N;
    if r < 0 { r + N } else { r }
}

fn connected(g: &Grid, x: i32, y: i32, d: usize) -> bool {
    if (g[y as usize][x as usize] >> d) & 1 == 0 { return false; }
    let nx = x + DX[d];
    let ny = y + DY[d];
    nx >= 0 && nx < N && ny >= 0 && ny < N &&
        ((g[ny as usize][nx as usize] >> ((d + 2) % 4)) & 1) != 0
}

fn bfs_dist(g: &Grid, sx: i32, sy: i32, tx: i32, ty: i32) -> i32 {
    if sx == tx && sy == ty { return 0; }
    let mut seen = vec![false; CELLS];
    let mut qx = vec![0i32; CELLS];
    let mut qy = vec![0i32; CELLS];
    let mut qs = vec![0i32; CELLS];
    let (mut head, mut tail) = (0usize, 0usize);
    qx[tail] = sx; qy[tail] = sy; qs[tail] = 0; tail += 1;
    seen[(sy * N + sx) as usize] = true;
    while head < tail {
        let x = qx[head]; let y = qy[head]; let s = qs[head]; head += 1;
        for d in 0..4 {
            if !connected(g, x, y, d) { continue; }
            let nx = x + DX[d]; let ny = y + DY[d];
            if nx == tx && ny == ty { return s + 1; }
            let id = (ny * N + nx) as usize;
            if !seen[id] {
                seen[id] = true;
                qx[tail] = nx; qy[tail] = ny; qs[tail] = s + 1; tail += 1;
            }
        }
    }
    999
}

fn dist_field(g: &Grid, tx: i32, ty: i32) -> [[i32; NZ]; NZ] {
    let mut dist = [[999i32; NZ]; NZ];
    dist[ty as usize][tx as usize] = 0;
    let mut qx = vec![0i32; CELLS];
    let mut qy = vec![0i32; CELLS];
    let (mut head, mut tail) = (0usize, 0usize);
    qx[tail] = tx; qy[tail] = ty; tail += 1;
    while head < tail {
        let x = qx[head]; let y = qy[head]; head += 1;
        let nd = dist[y as usize][x as usize] + 1;
        for d in 0..4 {
            if !connected(g, x, y, d) { continue; }
            let nx = x + DX[d]; let ny = y + DY[d];
            if dist[ny as usize][nx as usize] == 999 {
                dist[ny as usize][nx as usize] = nd;
                qx[tail] = nx; qy[tail] = ny; tail += 1;
            }
        }
    }
    dist
}

fn multi_bfs(g: &Grid, px: i32, py: i32, targets: &[Pos]) -> Vec<usize> {
    if targets.is_empty() { return Vec::new(); }
    let fields: Vec<[[i32; NZ]; NZ]> = targets.iter()
        .take(3).map(|t| dist_field(g, t.x, t.y)).collect();
    let exact = |x: i32, y: i32| {
        fields.iter().fold(999i32, |best, f| best.min(f[y as usize][x as usize]))
    };
    let mut idx = vec![-1i32; STSZ];
    let mut pre = vec![-2i32; STSZ];
    let mut pdir = vec![0usize; STSZ];
    let mut xx = vec![0i32; STSZ];
    let mut yy = vec![0i32; STSZ];
    let mut mm = vec![0i32; STSZ];
    let mut stp = vec![0i32; STSZ];
    let start = ((py * N + px) * 8) as usize;
    idx[start] = 0; pre[0] = -1;
    xx[0] = px; yy[0] = py; mm[0] = 0; stp[0] = 0;
    let mut q = vec![0usize; STSZ];
    let (mut head, mut tail) = (0usize, 0usize);
    q[tail] = 0; tail += 1;
    let mut best_got = 0i32;
    let mut best_steps = 0i32;
    let mut best_state = 0usize;
    let mut best_md = exact(px, py);
    while head < tail {
        let id = q[head]; head += 1;
        let x = xx[id]; let y = yy[id]; let mask = mm[id]; let st = stp[id];
        let got = (0..targets.len()).filter(|&i| (mask >> i) & 1 != 0).count() as i32;
        let md = if got < targets.len() as i32 { exact(x, y) } else { 0 };
        if got > best_got || (got == best_got &&
            (md < best_md || (md == best_md && st < best_steps))) {
            best_got = got; best_steps = st; best_state = id; best_md = md;
        }
        if st >= MAXSTEP as i32 { continue; }
        for d in 0..4 {
            if !connected(g, x, y, d) { continue; }
            let nx = x + DX[d]; let ny = y + DY[d];
            let mut nm = mask;
            for (i, t) in targets.iter().enumerate() {
                if t.x == nx && t.y == ny { nm |= 1 << i; }
            }
            let code = ((ny * N + nx) * 8 + nm) as usize;
            if idx[code] != -1 { continue; }
            let nid = tail;
            idx[code] = nid as i32; pre[nid] = id as i32; pdir[nid] = d;
            xx[nid] = nx; yy[nid] = ny; mm[nid] = nm; stp[nid] = st + 1;
            q[tail] = nid; tail += 1;
        }
    }
    let mut rev = Vec::new();
    let mut cur = best_state;
    while pre[cur] >= 0 {
        rev.push(pdir[cur]);
        cur = pre[cur] as usize;
    }
    rev.reverse();
    rev.truncate(MAXSTEP);
    rev
}

fn path_to_goal(g: &Grid, px: i32, py: i32, tx: i32, ty: i32) -> Vec<usize> {
    let mut prev = vec![-2i32; CELLS];
    let mut pdir = vec![0usize; CELLS];
    let mut q = vec![0usize; CELLS];
    let (mut head, mut tail) = (0usize, 0usize);
    let s = (py * N + px) as usize;
    prev[s] = -1; q[tail] = s; tail += 1;
    let target = (ty * N + tx) as usize;
    let mut found = false;
    while head < tail {
        let id = q[head]; head += 1;
        if id == target { found = true; break; }
        let x = (id % NZ as usize) as i32;
        let y = (id / NZ as usize) as i32;
        for d in 0..4 {
            if !connected(g, x, y, d) { continue; }
            let nx = x + DX[d]; let ny = y + DY[d];
            let nid = (ny * N + nx) as usize;
            if prev[nid] != -2 { continue; }
            prev[nid] = id as i32; pdir[nid] = d; q[tail] = nid; tail += 1;
        }
    }
    if !found { return Vec::new(); }
    let mut rev = Vec::new();
    let mut cur = target;
    while prev[cur] >= 0 {
        rev.push(pdir[cur]);
        cur = prev[cur] as usize;
    }
    rev.reverse();
    rev.truncate(MAXSTEP);
    rev
}

fn nearest_path(g: &Grid, px: i32, py: i32, goals: &[Pos]) -> Vec<usize> {
    let mut best = 1_000_000i32;
    let mut tx = -1; let mut ty = -1;
    for p in goals {
        let d = bfs_dist(g, px, py, p.x, p.y);
        if d < 999 && d < best { best = d; tx = p.x; ty = p.y; }
    }
    if tx < 0 || best == 0 { return Vec::new(); }
    path_to_goal(g, px, py, tx, ty)
}

struct SimResult {
    g: Grid,
    nx: i32,
    ny: i32,
    targets: Vec<Pos>,
    popped: i32,
    instant: i32,
}

fn simulate(grid: &Grid, tile: i32, px: i32, py: i32,
            board_targets: &[Pos], hand: i32, line: i32, d: usize) -> SimResult {
    let mut g = *grid;
    let horizontal = d == 1 || d == 3;
    let (insx, insy, popx, popy);
    if d == 1 {
        insx = 0; insy = line; popx = N - 1; popy = line;
        for x in (1..NZ).rev() { g[line as usize][x] = g[line as usize][x - 1]; }
    } else if d == 3 {
        insx = N - 1; insy = line; popx = 0; popy = line;
        for x in 0..NZ - 1 { g[line as usize][x] = g[line as usize][x + 1]; }
    } else if d == 0 {
        insx = line; insy = N - 1; popx = line; popy = 0;
        for y in 0..NZ - 1 { g[y][line as usize] = g[y + 1][line as usize]; }
    } else {
        insx = line; insy = 0; popx = line; popy = N - 1;
        for y in (1..NZ).rev() { g[y][line as usize] = g[y - 1][line as usize]; }
    }
    g[insy as usize][insx as usize] = tile;
    let on_line = if horizontal { py == line } else { px == line };
    let nx = if on_line { wrap(px + DX[d]) } else { px };
    let ny = if on_line { wrap(py + DY[d]) } else { py };
    let mut targets = Vec::new();
    let mut popped = 0;
    for p in board_targets {
        if p.x == popx && p.y == popy {
            popped += 1;
        } else if (horizontal && p.y == line) || (!horizontal && p.x == line) {
            targets.push(Pos { x: wrap(p.x + DX[d]), y: wrap(p.y + DY[d]) });
        } else {
            targets.push(*p);
        }
    }
    let instant = if on_line && nx == insx && ny == insy { hand } else { 0 };
    if hand != 0 && !on_line { targets.push(Pos { x: insx, y: insy }); }
    SimResult { g, nx, ny, targets, popped, instant }
}

fn reachable_count(g: &Grid, sx: i32, sy: i32) -> i32 {
    let mut seen = vec![false; CELLS];
    let mut qx = vec![0i32; CELLS];
    let mut qy = vec![0i32; CELLS];
    let (mut head, mut tail) = (0usize, 0usize);
    qx[tail] = sx; qy[tail] = sy; tail += 1;
    seen[(sy * N + sx) as usize] = true;
    let mut total = 1;
    while head < tail {
        let x = qx[head]; let y = qy[head]; head += 1;
        for d in 0..4 {
            if !connected(g, x, y, d) { continue; }
            let nx = x + DX[d]; let ny = y + DY[d];
            let id = (ny * N + nx) as usize;
            if !seen[id] { seen[id] = true; total += 1; qx[tail] = nx; qy[tail] = ny; tail += 1; }
        }
    }
    total
}

struct Input {
    turn_type: i32,
    px: i32, py: i32, tile: i32,
    ox: i32, oy: i32,
    hand: i32,
    grid: Grid,
    board_targets: Vec<Pos>,
    opp_targets: Vec<Pos>,
    our_all: Vec<Pos>,
    opp_all: Vec<Pos>,
}

fn read_input(tokens: &[&str], pos: &mut usize) -> Option<Input> {
    if *pos >= tokens.len() { return None; }
    let turn_type = tokens[*pos].parse().ok()?; *pos += 1;
    let mut grid = [[0i32; NZ]; NZ];
    for y in 0..NZ {
        for x in 0..NZ {
            grid[y][x] = mask_of(tokens[*pos]); *pos += 1;
        }
    }
    let mut coords = [0i32; 5];
    for p in 0..2 {
        let _count: i32 = tokens[*pos].parse().ok()?; *pos += 1;
        coords[p * 2] = tokens[*pos].parse().ok()?; *pos += 1;
        coords[p * 2 + 1] = tokens[*pos].parse().ok()?; *pos += 1;
        let tile = mask_of(tokens[*pos]); *pos += 1;
        if p == 0 { coords[4] = tile; }
    }
    let mut items: Vec<Vec<(String, i32, i32)>> = vec![Vec::new(), Vec::new()];
    let ni: usize = tokens[*pos].parse().ok()?; *pos += 1;
    for _ in 0..ni {
        let name = tokens[*pos].to_string(); *pos += 1;
        let x: i32 = tokens[*pos].parse().ok()?; *pos += 1;
        let y: i32 = tokens[*pos].parse().ok()?; *pos += 1;
        let pid: usize = tokens[*pos].parse().ok()?; *pos += 1;
        if pid <= 1 { items[pid].push((name, x, y)); }
    }
    let mut quests: Vec<Vec<String>> = vec![Vec::new(), Vec::new()];
    let nq: usize = tokens[*pos].parse().ok()?; *pos += 1;
    for _ in 0..nq {
        let name = tokens[*pos].to_string(); *pos += 1;
        let pid: usize = tokens[*pos].parse().ok()?; *pos += 1;
        if pid <= 1 { quests[pid].push(name); }
    }
    let mut inp = Input {
        turn_type, px: coords[0], py: coords[1], tile: coords[4],
        ox: coords[2], oy: coords[3], hand: 0, grid,
        board_targets: Vec::new(), opp_targets: Vec::new(),
        our_all: Vec::new(), opp_all: Vec::new(),
    };
    for pid in 0..2 {
        for (name, x, y) in &items[pid] {
            if *x == -1 { if pid == 0 { inp.hand += 1; } continue; }
            if *x == -2 { continue; }
            let revealed = quests[pid].iter().any(|q| q == name);
            if pid == 0 { inp.our_all.push(Pos { x: *x, y: *y }); }
            else { inp.opp_all.push(Pos { x: *x, y: *y }); }
            if revealed {
                if pid == 0 { inp.board_targets.push(Pos { x: *x, y: *y }); }
                else { inp.opp_targets.push(Pos { x: *x, y: *y }); }
            }
        }
    }
    Some(inp)
}

#[derive(Clone)]
struct Candidate {
    score: f64,
    instant: i32,
    popped: i32,
    conn: i32,
    line: i32,
    d: usize,
    cmd: String,
}

fn parse_last(cmd: &str) -> Option<(i32, bool)> {
    let parts: Vec<&str> = cmd.split_whitespace().collect();
    if parts.len() != 3 || parts[0] != "PUSH" { return None; }
    let line = parts[1].parse().ok()?;
    Some((line, parts[2] == "RIGHT" || parts[2] == "LEFT"))
}

fn best_push(inp: &Input, last_push: &str, stale: i32) -> String {
    let mut cands = Vec::new();
    let last = parse_last(last_push);
    for pass in 0..2 {
        let horizontal = pass == 0;
        for line in 0..N {
            for step in 0..2 {
                let d = if horizontal { if step == 0 { 1 } else { 3 } }
                        else { if step == 0 { 0 } else { 2 } };
                if let Some((ll, lh)) = last {
                    if ll == line && lh == horizontal { continue; }
                }
                let s = simulate(&inp.grid, inp.tile, inp.px, inp.py,
                                 &inp.board_targets, inp.hand, line, d);
                let o = simulate(&inp.grid, inp.tile, inp.ox, inp.oy,
                                 &inp.opp_targets, 0, line, d);
                let mut score = 1000.0 * s.instant as f64 - 100.0 * s.popped as f64;
                for t in &s.targets {
                    let dist = bfs_dist(&s.g, s.nx, s.ny, t.x, t.y);
                    if dist <= 20 { score += (1000 - 10 * dist) as f64; }
                    else if dist < 999 { score += 200.0 / (dist + 1) as f64; }
                    else {
                        score += 500.0 - 30.0 * ((s.nx - t.x).abs() + (s.ny - t.y).abs()) as f64;
                    }
                }
                score += 2.0 * reachable_count(&s.g, s.nx, s.ny) as f64;
                for t in &o.targets {
                    let dist = bfs_dist(&s.g, o.nx, o.ny, t.x, t.y);
                    if dist <= 20 { score -= 10.0; }
                }
                for t in &inp.our_all {
                    if inp.board_targets.iter().any(|q| q == t) { continue; }
                    let dist = bfs_dist(&s.g, s.nx, s.ny, t.x, t.y);
                    if dist <= 20 { score += 20.0; }
                    else if dist < 999 { score += 20.0 / (dist + 1) as f64; }
                }
                let mut conn = 0;
                for k in 0..4 { if connected(&s.g, s.nx, s.ny, k) { conn += 1; } }
                score += conn as f64;
                cands.push(Candidate {
                    score, instant: s.instant, popped: s.popped, conn,
                    line, d, cmd: format!("PUSH {} {}", line, DIRS[d]),
                });
            }
        }
    }
    cands.sort_by(|a, b| {
        if a.score != b.score { return b.score.partial_cmp(&a.score).unwrap(); }
        if a.instant != b.instant { return b.instant.cmp(&a.instant); }
        if a.popped != b.popped { return a.popped.cmp(&b.popped); }
        if a.conn != b.conn { return b.conn.cmp(&a.conn); }
        if a.line != b.line { return a.line.cmp(&b.line); }
        if a.d != b.d { return a.d.cmp(&b.d); }
        b.cmd.cmp(&a.cmd)
    });
    if stale >= 3 && cands.len() > 1 {
        if let Some(c) = cands.iter().find(|c| c.cmd != last_push) { return c.cmd.clone(); }
    }
    cands[0].cmd.clone()
}

fn main() {
    let mut raw = String::new();
    io::stdin().read_to_string(&mut raw).unwrap();
    let tokens: Vec<&str> = raw.split_whitespace().collect();
    let mut pos = 0usize;
    let mut last_push = String::new();
    let mut stale = 0i32;
    let stdout = io::stdout();
    let mut out = stdout.lock();
    while let Some(inp) = read_input(&tokens, &mut pos) {
        if inp.turn_type == 1 {
            let mut moves = multi_bfs(&inp.grid, inp.px, inp.py, &inp.board_targets);
            if moves.is_empty() && !inp.board_targets.is_empty() {
                moves = nearest_path(&inp.grid, inp.px, inp.py, &inp.board_targets);
            }
            if moves.is_empty() && inp.hand != 0 {
                let mut edge = Vec::new();
                for x in 0..N { edge.push(Pos { x, y: 0 }); edge.push(Pos { x, y: N - 1 }); }
                for y in 0..N { edge.push(Pos { x: 0, y }); edge.push(Pos { x: N - 1, y }); }
                moves = nearest_path(&inp.grid, inp.px, inp.py, &edge);
            }
            if moves.is_empty() {
                writeln!(out, "PASS").unwrap();
            } else {
                write!(out, "MOVE").unwrap();
                for m in moves { write!(out, " {}", DIRS[m]).unwrap(); }
                writeln!(out).unwrap();
            }
        } else {
            let cmd = best_push(&inp, &last_push, stale);
            if cmd == last_push { stale += 1; } else { stale = 0; }
            last_push = cmd.clone();
            writeln!(out, "{}", cmd).unwrap();
        }
        out.flush().unwrap();
    }
}
