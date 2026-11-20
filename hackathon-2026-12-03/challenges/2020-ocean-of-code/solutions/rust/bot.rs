// Ocean of Code (Wood 3) — croyance adversaire + torpedo.
use std::error::Error;
use std::io::{self, BufRead, Write};

const N: usize = 15;
const DIR_NAME: [char; 4] = ['N', 'E', 'S', 'W'];
const DX: [i32; 4] = [0, 1, 0, -1];
const DY: [i32; 4] = [-1, 0, 1, 0];

type GridBool = [[bool; N]; N];
type GridInt = [[i32; N]; N];

struct Bot {
    water: GridBool,
    visited: GridBool,
    bel: GridBool,
    prev_opp_life: i32,
    last_x: i32,
    last_y: i32,
    cnt: i32,
    sumx: i32,
    sumy: i32,
}

impl Bot {
    fn new() -> Self {
        Self {
            water: [[false; N]; N],
            visited: [[false; N]; N],
            bel: [[false; N]; N],
            prev_opp_life: -1,
            last_x: -1,
            last_y: -1,
            cnt: 0,
            sumx: 0,
            sumy: 0,
        }
    }

    fn clear_bel(&mut self) {
        for row in &mut self.bel { row.fill(false); }
    }

    fn reset_bel(&mut self) {
        for y in 0..N {
            for x in 0..N { self.bel[y][x] = self.water[y][x]; }
        }
    }

    fn bel_empty(&self) -> bool {
        for row in &self.bel {
            if row.iter().any(|&v| v) { return false; }
        }
        true
    }

    fn reset_if_empty(&mut self) {
        if self.bel_empty() { self.reset_bel(); }
    }

    fn compute_centroid(&mut self) {
        self.cnt = 0; self.sumx = 0; self.sumy = 0;
        for y in 0..N {
            for x in 0..N {
                if self.bel[y][x] {
                    self.cnt += 1;
                    self.sumx += x as i32;
                    self.sumy += y as i32;
                }
            }
        }
        if self.cnt == 0 {
            self.reset_bel();
            for y in 0..N {
                for x in 0..N {
                    if self.bel[y][x] {
                        self.cnt += 1;
                        self.sumx += x as i32;
                        self.sumy += y as i32;
                    }
                }
            }
        }
    }

    fn best_dir(&self, px: usize, py: usize, ignore_visited: bool) -> Option<(usize, usize, usize)> {
        let mut best: Option<(i32, i32, usize, usize, usize)> = None;
        for k in 0..4 {
            let nx = px as i32 + DX[k];
            let ny = py as i32 + DY[k];
            if nx < 0 || nx >= N as i32 || ny < 0 || ny >= N as i32 { continue; }
            let (nx, ny) = (nx as usize, ny as usize);
            if !self.water[ny][nx] { continue; }
            if !ignore_visited && self.visited[ny][nx] { continue; }
            let mut free = 0;
            for j in 0..4 {
                let ax = nx as i32 + DX[j];
                let ay = ny as i32 + DY[j];
                if ax < 0 || ax >= N as i32 || ay < 0 || ay >= N as i32 { continue; }
                let (ax, ay) = (ax as usize, ay as usize);
                if !self.water[ay][ax] { continue; }
                let blocked = if ignore_visited { ax == px && ay == py } else { self.visited[ay][ax] };
                if !blocked { free += 1; }
            }
            let cdist = (2 * self.cnt * nx as i32 - self.sumx).abs()
                + (2 * self.cnt * ny as i32 - self.sumy).abs();
            let candidate = (-free, cdist, k, nx, ny);
            if best.map_or(true, |cur| candidate < (cur.0, cur.1, cur.2, cur.3, cur.4)) {
                best = Some(candidate);
            }
        }
        best.map(|(_, _, k, nx, ny)| (nx, ny, k))
    }
}

fn sector_of(x: usize, y: usize) -> i32 {
    (x / 5) as i32 + 1 + ((y / 5) as i32) * 3
}

fn bfs(water: &GridBool, sx: usize, sy: usize) -> GridInt {
    let mut dist = [[-1i32; N]; N];
    let mut queue = std::collections::VecDeque::new();
    dist[sy][sx] = 0;
    queue.push_back((sx, sy));
    while let Some((x, y)) = queue.pop_front() {
        for k in 0..4 {
            let nx = x as i32 + DX[k];
            let ny = y as i32 + DY[k];
            if nx < 0 || nx >= N as i32 || ny < 0 || ny >= N as i32 { continue; }
            let (nx, ny) = (nx as usize, ny as usize);
            if !water[ny][nx] || dist[ny][nx] != -1 { continue; }
            dist[ny][nx] = dist[y][x] + 1;
            queue.push_back((nx, ny));
        }
    }
    dist
}

fn choose_start(b: &Bot) -> (usize, usize) {
    let mut best = (usize::MAX, usize::MAX);
    let mut best_deg = -1i32;
    let mut best_center = 1_000_000i32;
    for y in 0..N {
        for x in 0..N {
            if !b.water[y][x] { continue; }
            let mut deg = 0;
            for k in 0..4 {
                let nx = x as i32 + DX[k];
                let ny = y as i32 + DY[k];
                if nx >= 0 && nx < N as i32 && ny >= 0 && ny < N as i32 && b.water[ny as usize][nx as usize] {
                    deg += 1;
                }
            }
            let center = (x as i32 - 7).abs() + (y as i32 - 7).abs();
            if deg > best_deg
                || (deg == best_deg && center < best_center)
                || (deg == best_deg && center == best_center && (x, y) < best)
            {
                best = (x, y);
                best_deg = deg;
                best_center = center;
            }
        }
    }
    best
}

fn main() -> Result<(), Box<dyn Error>> {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();
    let stdout = io::stdout();
    let mut out = stdout.lock();

    let header = match lines.next() {
        Some(l) => l?,
        None => return Ok(()),
    };
    let head: Vec<&str> = header.split_whitespace().collect();
    let mut bot = Bot::new();

    if head.len() == 3 {
        let hh: usize = head[1].parse()?;
        for _y in 0..hh {
            let row = lines.next().unwrap()?;
            for (x, c) in row.bytes().enumerate() {
                bot.water[_y][x] = c == b'.';
            }
        }
        bot.reset_bel();
        let (sx, sy) = choose_start(&bot);
        bot.visited[sy][sx] = true;
        writeln!(out, "{} {}", sx, sy)?;
        out.flush()?;
    }

    while let Some(result) = lines.next() {
        let line = result?;
        if line.is_empty() { break; }
        let f: Vec<&str> = line.split_whitespace().collect();
        if f.len() != 8 { break; }
        let px: usize = f[0].parse()?;
        let py: usize = f[1].parse()?;
        let opp_life: i32 = f[3].parse()?;
        let cooldown: i32 = f[4].parse()?;
        lines.next(); // sonar
        let orders = lines.next().unwrap()?;

        if bot.last_x >= 0 && bot.prev_opp_life >= 0 {
            let delta = bot.prev_opp_life - opp_life;
            let tx = bot.last_x as usize;
            let ty = bot.last_y as usize;
            if delta == 2 {
                bot.clear_bel();
                if bot.water[ty][tx] { bot.bel[ty][tx] = true; }
            } else if delta == 1 {
                bot.clear_bel();
                for dy in -1..=1 {
                    for dx in -1..=1 {
                        if dx == 0 && dy == 0 { continue; }
                        let nx = tx as i32 + dx;
                        let ny = ty as i32 + dy;
                        if nx >= 0 && nx < N as i32 && ny >= 0 && ny < N as i32 && bot.water[ny as usize][nx as usize] {
                            bot.bel[ny as usize][nx as usize] = true;
                        }
                    }
                }
            } else if delta == 0 {
                for dy in -1..=1 {
                    for dx in -1..=1 {
                        let nx = tx as i32 + dx;
                        let ny = ty as i32 + dy;
                        if nx >= 0 && nx < N as i32 && ny >= 0 && ny < N as i32 {
                            bot.bel[ny as usize][nx as usize] = false;
                        }
                    }
                }
            }
        }
        bot.last_x = -1; bot.last_y = -1;
        bot.reset_if_empty();

        if orders != "NA" {
            for raw in orders.split('|') {
                let tok: Vec<&str> = raw.split_whitespace().collect();
                if !tok.is_empty() {
                    if tok[0] == "MOVE" && tok.len() >= 2 {
                        let mut k_dir = None;
                        for j in 0..4 {
                            if tok[1].chars().next() == Some(DIR_NAME[j]) { k_dir = Some(j); break; }
                        }
                        if let Some(k) = k_dir {
                            let mut nb = [[false; N]; N];
                            for y in 0..N {
                                for x in 0..N {
                                    if !bot.bel[y][x] { continue; }
                                    let nx = x as i32 + DX[k];
                                    let ny = y as i32 + DY[k];
                                    if nx >= 0 && nx < N as i32 && ny >= 0 && ny < N as i32 && bot.water[ny as usize][nx as usize] {
                                        nb[ny as usize][nx as usize] = true;
                                    }
                                }
                            }
                            bot.bel = nb;
                        }
                    } else if tok[0] == "SURFACE" && tok.len() >= 2 {
                        let s: i32 = tok[1].parse()?;
                        for y in 0..N {
                            for x in 0..N {
                                if bot.bel[y][x] && sector_of(x, y) != s { bot.bel[y][x] = false; }
                            }
                        }
                    } else if tok[0] == "TORPEDO" && tok.len() >= 3 {
                        let ax: usize = tok[1].parse()?;
                        let ay: usize = tok[2].parse()?;
                        let d = bfs(&bot.water, ax, ay);
                        for y in 0..N {
                            for x in 0..N {
                                if bot.bel[y][x] && (d[y][x] < 0 || d[y][x] > 4) {
                                    bot.bel[y][x] = false;
                                }
                            }
                        }
                    }
                }
                bot.reset_if_empty();
            }
        }

        bot.compute_centroid();
        bot.visited[py][px] = true;
        let mut actions: Vec<String> = Vec::new();
        let (mut final_x, mut final_y) = (px, py);

        if let Some((nx, ny, k)) = bot.best_dir(px, py, false) {
            actions.push(format!("MOVE {} TORPEDO", DIR_NAME[k]));
            final_x = nx; final_y = ny;
            bot.visited[ny][nx] = true;
        } else {
            for row in &mut bot.visited { row.fill(false); }
            bot.visited[py][px] = true;
            actions.push("SURFACE".to_string());
            if let Some((nx, ny, k)) = bot.best_dir(px, py, true) {
                actions.push(format!("MOVE {} TORPEDO", DIR_NAME[k]));
                final_x = nx; final_y = ny;
                bot.visited[py][px] = true;
                bot.visited[ny][nx] = true;
            }
        }

        if cooldown == 0 {
            let dstart = bfs(&bot.water, px, py);
            let mut best_score = -1;
            let mut best_target = (0usize, 0usize);
            for y in 0..N {
                for x in 0..N {
                    if !bot.water[y][x] || dstart[y][x] < 0 || dstart[y][x] > 4 { continue; }
                    if (x as i32 - final_x as i32).abs() <= 1 && (y as i32 - final_y as i32).abs() <= 1 { continue; }
                    let mut score = if bot.bel[y][x] { 2 } else { 0 };
                    for dy in -1..=1 {
                        for dx in -1..=1 {
                            if dx == 0 && dy == 0 { continue; }
                            let nx = x as i32 + dx;
                            let ny = y as i32 + dy;
                            if nx >= 0 && nx < N as i32 && ny >= 0 && ny < N as i32 && bot.bel[ny as usize][nx as usize] {
                                score += 1;
                            }
                        }
                    }
                    if score > best_score || (score == best_score && (x, y) < best_target) {
                        best_score = score;
                        best_target = (x, y);
                    }
                }
            }
            if best_score > 0 {
                actions.push(format!("TORPEDO {} {}", best_target.0, best_target.1));
                bot.last_x = best_target.0 as i32;
                bot.last_y = best_target.1 as i32;
            }
        }

        writeln!(out, "{}", actions.join("|"))?;
        out.flush()?;
        bot.prev_opp_life = opp_life;
    }
    Ok(())
}
