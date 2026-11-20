// Spring Challenge 2020 (Wood 4) — BFS + meilleur pellet, évitement collision.
use std::cmp::Ordering;
use std::io::{self, BufRead, Write};

const MAXC: usize = 64;
const MAXR: usize = 64;
const DX: [i32; 4] = [0, 1, 0, -1];
const DY: [i32; 4] = [-1, 0, 1, 0];

type Grid<T> = [[T; MAXC]; MAXR];

struct Bot {
    w: usize,
    h: usize,
    grid: Grid<char>,
    dist: Grid<i32>,
    prevx: Grid<i32>,
    prevy: Grid<i32>,
    target: Option<(usize, usize)>,
    last_sx: usize,
    last_sy: usize,
}

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
struct Key {
    penalty: i32,
    cost: i32,
    negval: i32,
    d: i32,
    tx: i32,
    ty: i32,
}

impl Ord for Key {
    fn cmp(&self, o: &Self) -> Ordering {
        (self.penalty, self.cost, self.negval, self.d, self.tx, self.ty)
            .cmp(&(o.penalty, o.cost, o.negval, o.d, o.tx, o.ty))
    }
}
impl PartialOrd for Key {
    fn partial_cmp(&self, o: &Self) -> Option<Ordering> { Some(self.cmp(o)) }
}

impl Bot {
    fn new(w: usize, h: usize, grid: Grid<char>) -> Self {
        Self {
            w, h, grid,
            dist: [[-1; MAXC]; MAXR],
            prevx: [[-1; MAXC]; MAXR],
            prevy: [[-1; MAXC]; MAXR],
            target: None,
            last_sx: 0,
            last_sy: 0,
        }
    }

    fn wrapx(&self, x: i32) -> i32 {
        let v = x.rem_euclid(self.w as i32);
        v
    }

    fn bfs(&mut self, sx: usize, sy: usize) {
        for y in 0..self.h {
            for x in 0..self.w {
                self.dist[y][x] = -1;
                self.prevx[y][x] = -1;
                self.prevy[y][x] = -1;
            }
        }
        let mut qx = vec![0usize; MAXR * MAXC];
        let mut qy = vec![0usize; MAXR * MAXC];
        let (mut head, mut tail) = (0usize, 0usize);
        self.dist[sy][sx] = 0;
        qx[tail] = sx; qy[tail] = sy; tail += 1;
        while head < tail {
            let x = qx[head]; let y = qy[head]; head += 1;
            for k in 0..4 {
                let nx = self.wrapx(x as i32 + DX[k]);
                let ny = y as i32 + DY[k];
                if nx < 0 || ny < 0 || ny >= self.h as i32 { continue; }
                let (nx, ny) = (nx as usize, ny as usize);
                if self.grid[ny][nx] == '#' || self.dist[ny][nx] != -1 { continue; }
                self.dist[ny][nx] = self.dist[y][x] + 1;
                self.prevx[ny][nx] = x as i32;
                self.prevy[ny][nx] = y as i32;
                qx[tail] = nx; qy[tail] = ny; tail += 1;
            }
        }
    }

    fn first_step(&self, sx: usize, sy: usize, tx: usize, ty: usize) -> Option<(usize, usize)> {
        if sx == tx && sy == ty { return None; }
        let (mut x, mut y) = (tx, ty);
        let mut guard = 0;
        while x != sx || y != sy {
            let px = self.prevx[y][x];
            let py = self.prevy[y][x];
            if px < 0 || py < 0 || guard > self.w * self.h { return None; }
            guard += 1;
            if px as usize == sx && py as usize == sy { return Some((x, y)); }
            x = px as usize; y = py as usize;
        }
        None
    }

    fn best_key(&self, pellets: &[(usize, usize, i32)], penalty_cost: bool,
                opp_step: Option<(usize, usize)>) -> Key {
        let mut best = Key { penalty: 0, cost: 0, negval: 0, d: 0, tx: -1, ty: -1 };
        for &(tx, ty, v) in pellets {
            if tx >= self.w || ty >= self.h || self.dist[ty][tx] < 0 { continue; }
            let penalty = if penalty_cost {
                let step = match self.first_step(self.last_sx, self.last_sy, tx, ty) {
                    Some(s) => s,
                    None => continue,
                };
                match opp_step {
                    Some(o) if step == o => 1,
                    _ => 0,
                }
            } else { 0 };
            let d = self.dist[ty][tx];
            let k = Key {
                penalty,
                cost: if penalty_cost { d * 10 / v } else { d },
                negval: -v,
                d,
                tx: tx as i32,
                ty: ty as i32,
            };
            if best.tx < 0 || k < best { best = k; }
        }
        best
    }

}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();
    let stdout = io::stdout();
    let mut out = stdout.lock();

    let header = match lines.next() { Some(l) => l?, None => return Ok(()) };
    let head: Vec<&str> = header.split_whitespace().collect();
    if head.len() < 2 { return Ok(()); }
    let w: usize = head[0].parse()?;
    let h: usize = head[1].parse()?;
    let mut grid: Grid<char> = [[' '; MAXC]; MAXR];
    for y in 0..h {
        let row = lines.next().unwrap()?;
        for (x, c) in row.chars().enumerate() {
            if x < MAXC { grid[y][x] = c; }
        }
    }
    let mut bot = Bot::new(w, h, grid);
    bot.last_sx = 0; bot.last_sy = 0;

    loop {
        let line = match lines.next() { Some(l) => l?, None => break };
        if line.trim().is_empty() { break; }
        if line.split_whitespace().count() < 2 { break; }
        let np: usize = lines.next().unwrap()?.parse()?;
        let (mut px, mut py, mut ox, mut oy) = (usize::MAX, usize::MAX, usize::MAX, usize::MAX);
        for _ in 0..np {
            let l = lines.next().unwrap()?;
            let f: Vec<&str> = l.split_whitespace().collect();
            if f.len() < 4 { continue; }
            let id: i32 = f[0].parse()?;
            let mine: i32 = f[1].parse()?;
            let x: usize = f[2].parse()?;
            let y: usize = f[3].parse()?;
            if mine == 1 { px = x; py = y; }
            else if ox == usize::MAX || id == 0 { ox = x; oy = y; }
        }
        let npel: usize = lines.next().unwrap()?.parse()?;
        let mut pellets = Vec::with_capacity(npel);
        for _ in 0..npel {
            let l = lines.next().unwrap()?;
            let f: Vec<&str> = l.split_whitespace().collect();
            if f.len() >= 3 {
                pellets.push((f[0].parse()?, f[1].parse()?, f[2].parse()?));
            }
        }
        if px == usize::MAX {
            writeln!(out, "MOVE 0 0 0")?; out.flush()?;
            continue;
        }

        bot.bfs(px, py);
        bot.last_sx = px; bot.last_sy = py;
        let mut target_ok = false;
        if let Some((tx, ty)) = bot.target {
            if tx == px && ty == py { bot.target = None; }
            else {
                for &(ax, ay, _) in &pellets {
                    if ax == tx && ay == ty && ty < bot.h && tx < bot.w && bot.dist[ty][tx] >= 0 {
                        target_ok = true; break;
                    }
                }
                if !target_ok { bot.target = None; }
            }
        }
        if let Some((tx, ty)) = bot.target {
            writeln!(out, "MOVE 0 {} {}", tx, ty)?; out.flush()?;
            continue;
        }
        if pellets.is_empty() {
            writeln!(out, "MOVE 0 {} {}", px, py)?; out.flush()?;
            continue;
        }

        let mut opp_step = None;
        if ox != usize::MAX && oy != usize::MAX && ox < bot.w && oy < bot.h {
            let sd = bot.dist; let sx_ = bot.prevx; let sy_ = bot.prevy;
            bot.bfs(ox, oy);
            let bo = bot.best_key(&pellets, false, None);
            if bo.tx >= 0 {
                opp_step = bot.first_step(ox, oy, bo.tx as usize, bo.ty as usize);
            }
            bot.dist = sd; bot.prevx = sx_; bot.prevy = sy_;
        }

        let best = bot.best_key(&pellets, true, opp_step);
        if best.tx < 0 {
            writeln!(out, "MOVE 0 {} {}", px, py)?; out.flush()?;
            continue;
        }
        let (tx, ty) = (best.tx as usize, best.ty as usize);
        bot.target = Some((tx, ty));
        writeln!(out, "MOVE 0 {} {}", tx, ty)?; out.flush()?;
    }
    Ok(())
}
