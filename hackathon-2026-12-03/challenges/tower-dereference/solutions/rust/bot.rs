// Tower Dereference (CodinGame) — Rust port of solutions/python/bot.py (v1c).
// Same path model, value model, option order and tie-breaks (integer math).
use std::io::{self, BufRead, Write};

const GUN: &str = "GUNTOWER";
const GLUE: &str = "GLUETOWER";
const GUN_COST: i64 = 100;
const GLUE_COST: i64 = 70;
const GLUE_RANGE: [i32; 4] = [3, 4, 5, 6];
const GLUE_BONUS: i64 = 3; // slowed (speed / 4) attackers stay ~4x longer in range
const GUN_DAMAGE: [i32; 4] = [5, 8, 15, 30];
const GUN_RANGE: [i32; 4] = [3, 4, 5, 6];
const GUN_RELOAD: [i32; 4] = [5, 4, 3, 2];
const UPGRADE_COST: [i64; 3] = [50, 100, 150];
const MAX_LEVEL: usize = 3;
const RATE_SCALE: i64 = 60; // lcm of reload times: damage rate = dmg * 60 / reload
const DAMAGE_CAP: i32 = 15; // wave HP is 12..18: damage above this is mostly overkill
const CONTESTED_PERCENT: i64 = 90; // value kept for cells on the opponent half (lost build conflicts)
const PROPERTIES: [&str; 3] = ["DAMAGE", "RANGE", "RELOAD"];

type Cell = (i32, i32);

/// Canyon cells lying on a shortest path between the left and right ends.
fn path_cells(grid: &[Vec<u8>], width: usize, height: usize) -> Vec<Cell> {
    let bfs = |source_x: usize| -> Vec<i32> {
        let mut dist = vec![-1i32; width * height];
        let mut queue = Vec::new();
        for y in 0..height {
            if grid[y][source_x] == b'.' {
                dist[y * width + source_x] = 0;
                queue.push(y * width + source_x);
            }
        }
        let mut head = 0;
        while head < queue.len() {
            let c = queue[head];
            head += 1;
            let (x, y) = ((c % width) as i32, (c / width) as i32);
            for (nx, ny) in [(x + 1, y), (x - 1, y), (x, y + 1), (x, y - 1)] {
                if nx < 0 || nx >= width as i32 || ny < 0 || ny >= height as i32 {
                    continue;
                }
                let n = ny as usize * width + nx as usize;
                if grid[ny as usize][nx as usize] == b'.' && dist[n] < 0 {
                    dist[n] = dist[c] + 1;
                    queue.push(n);
                }
            }
        }
        dist
    };
    let (left, right) = (bfs(0), bfs(width - 1));
    let total = (0..height)
        .filter(|&y| grid[y][width - 1] == b'.' && left[y * width + width - 1] >= 0)
        .map(|y| left[y * width + width - 1])
        .min();
    let mut path = Vec::new();
    for x in 0..width {
        for y in 0..height {
            if grid[y][x] != b'.' {
                continue;
            }
            let c = y * width + x;
            let keep = match total {
                None => true,
                Some(t) => left[c] >= 0 && right[c] >= 0 && left[c] + right[c] == t,
            };
            if keep {
                path.push((x as i32, y as i32));
            }
        }
    }
    path
}

fn in_range(c: Cell, x: i32, y: i32, radius: i32) -> bool {
    (c.0 - x) * (c.0 - x) + (c.1 - y) * (c.1 - y) <= radius * radius
}

fn damage_rate(damage_level: usize, reload_level: usize) -> i64 {
    GUN_DAMAGE[damage_level].min(DAMAGE_CAP) as i64 * RATE_SCALE / GUN_RELOAD[reload_level] as i64
}

/// Simultaneous builds on one cell go to the player whose base is closer.
fn on_opponent_half(me: i32, x: i32, width: i32) -> bool {
    let middle = width / 2;
    if me == 0 { x > middle } else { x < middle }
}

fn side_adjusted(value: i64, me: i32, x: i32, width: i32) -> i64 {
    if on_opponent_half(me, x, width) { value * CONTESTED_PERCENT / 100 } else { value }
}

/// One of our towers (or one planned this turn: id None, not upgradable).
#[derive(Clone)]
struct Tower {
    id: Option<i32>,
    is_gun: bool,
    x: i32,
    y: i32,
    levels: [usize; 3], // damage, range, reload
}

impl Tower {
    fn covers(&self, c: Cell, lv: &[usize; 3]) -> bool {
        let radius = if self.is_gun { GUN_RANGE[lv[1]] } else { GLUE_RANGE[lv[1]] };
        in_range(c, self.x, self.y, radius)
    }
}

/// Value model with per-cell weights and gun rates, refreshed after every purchase.
struct Defence {
    path: Vec<Cell>,
    glued: Vec<bool>,
    weight: Vec<i64>,
    rate: Vec<i64>,
}

impl Defence {
    fn new(path: Vec<Cell>) -> Defence {
        let n = path.len();
        Defence { path, glued: vec![false; n], weight: vec![1; n], rate: vec![0; n] }
    }

    fn refresh(&mut self, towers: &[Tower]) {
        for (i, &c) in self.path.iter().enumerate() {
            self.glued[i] = towers.iter().any(|t| !t.is_gun && t.covers(c, &t.levels));
            self.weight[i] = if self.glued[i] { 1 + GLUE_BONUS } else { 1 };
            self.rate[i] = towers
                .iter()
                .filter(|t| t.is_gun && t.covers(c, &t.levels))
                .map(|t| damage_rate(t.levels[0], t.levels[2]))
                .sum();
        }
    }

    fn gun_value(&self, gun: &Tower, lv: &[usize; 3]) -> i64 {
        let covered: i64 = self.path.iter().zip(&self.weight).filter(|(c, _)| gun.covers(**c, lv)).map(|(_, w)| *w).sum();
        damage_rate(lv[0], lv[2]) * covered
    }

    /// Extra value of a level-0 glue tower at (x, y).
    fn glue_value(&self, x: i32, y: i32) -> i64 {
        let sum: i64 = (0..self.path.len())
            .filter(|&i| !self.glued[i] && in_range(self.path[i], x, y, GLUE_RANGE[0]))
            .map(|i| self.rate[i])
            .sum();
        GLUE_BONUS * sum
    }

    fn build_gun_value(&self, x: i32, y: i32) -> i64 {
        let covered: i64 = self.path.iter().zip(&self.weight).filter(|(c, _)| in_range(**c, x, y, GUN_RANGE[0])).map(|(_, w)| *w).sum();
        damage_rate(0, 0) * covered
    }
}

/// Upgrade level of a printed property value, None if not in the table.
fn level_of(table: &[i32; 4], text: &str) -> Option<usize> {
    let value: f64 = text.replace(',', ".").parse().ok()?;
    table.iter().position(|&r| (r as f64 - value).abs() < 1e-9)
}

enum Effect {
    Build(bool, i32, i32), // is_gun, x, y
    Upgrade(usize, usize), // tower index, property
}

struct Option_ {
    gain: i64,
    cost: i64,
    command: String,
    effect: Effect,
}

/// True when option a has a strictly higher gain per coin than b.
fn better(a: &Option_, b: &Option_) -> bool {
    a.gain * b.cost > b.gain * a.cost
}

fn best_free_cell(grid: &[Vec<u8>], taken: &[Vec<bool>], me: i32, value_of: impl Fn(i32, i32) -> i64) -> Option<(i32, i32, i64)> {
    let mut best: Option<(i32, i32, i64)> = None;
    for (y, row) in grid.iter().enumerate() {
        for (x, &ch) in row.iter().enumerate() {
            if ch != b'#' || taken[y][x] {
                continue;
            }
            let value = side_adjusted(value_of(x as i32, y as i32), me, x as i32, row.len() as i32);
            if value > best.map_or(0, |b| b.2) {
                best = Some((x as i32, y as i32, value));
            }
        }
    }
    best
}

fn collect_options(grid: &[Vec<u8>], taken: &[Vec<bool>], me: i32, defence: &Defence, towers: &[Tower]) -> Vec<Option_> {
    let mut out = Vec::new();
    if let Some((x, y, v)) = best_free_cell(grid, taken, me, |a, b| defence.build_gun_value(a, b)) {
        out.push(Option_ { gain: v, cost: GUN_COST, command: format!("BUILD {} {} GUNTOWER", x, y), effect: Effect::Build(true, x, y) });
    }
    if let Some((x, y, v)) = best_free_cell(grid, taken, me, |a, b| defence.glue_value(a, b)) {
        out.push(Option_ { gain: v, cost: GLUE_COST, command: format!("BUILD {} {} GLUETOWER", x, y), effect: Effect::Build(false, x, y) });
    }
    for (t, gun) in towers.iter().enumerate() {
        let id = match gun.id {
            Some(id) if gun.is_gun => id,
            _ => continue,
        };
        for prop in 0..3 {
            let level = gun.levels[prop];
            if level >= MAX_LEVEL {
                continue;
            }
            let mut upgraded = gun.levels;
            upgraded[prop] += 1;
            let gain = defence.gun_value(gun, &upgraded) - defence.gun_value(gun, &gun.levels);
            out.push(Option_ { gain, cost: UPGRADE_COST[level], command: format!("UPGRADE {} {}", id, PROPERTIES[prop]), effect: Effect::Upgrade(t, prop) });
        }
    }
    out
}

fn plan_turn(grid: &[Vec<u8>], me: i32, defence: &mut Defence, mut money: i64, mut towers: Vec<Tower>, mut taken: Vec<Vec<bool>>) -> Vec<String> {
    let mut commands = Vec::new();
    defence.refresh(&towers);
    loop {
        let options = collect_options(grid, &taken, me, defence, &towers);
        let mut best: Option<&Option_> = None;
        for o in &options {
            if o.gain > 0 && o.cost <= money && best.map_or(true, |b| better(o, b)) {
                best = Some(o);
            }
        }
        let best = match best {
            Some(b) => b,
            None => return commands,
        };
        money -= best.cost;
        commands.push(best.command.clone());
        match best.effect {
            Effect::Upgrade(t, prop) => towers[t].levels[prop] += 1,
            Effect::Build(is_gun, x, y) => {
                taken[y as usize][x as usize] = true;
                towers.push(Tower { id: None, is_gun, x, y, levels: [0, 0, 0] });
            }
        }
        defence.refresh(&towers);
    }
}

fn main() {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines().map(|l| l.unwrap_or_default());
    let me: i32 = lines.next().unwrap_or_default().trim().parse().unwrap_or(0);
    let dims: Vec<usize> = lines.next().unwrap_or_default().split_whitespace().filter_map(|t| t.parse().ok()).collect();
    let (width, height) = (dims[0], dims[1]);
    let grid: Vec<Vec<u8>> = (0..height)
        .map(|_| {
            let row = lines.next().unwrap_or_default();
            let b = row.trim_end_matches('\r').as_bytes();
            (0..width).map(|x| if x < b.len() { b[x] } else { b'#' }).collect()
        })
        .collect();
    let mut defence = Defence::new(path_cells(&grid, width, height));
    let out = io::stdout();
    while let Some(line) = lines.next() {
        if line.trim().is_empty() {
            break;
        }
        let money: i64 = line.split_whitespace().next().and_then(|t| t.parse().ok()).unwrap_or(0);
        lines.next(); // opponent money and lives
        let mut towers = Vec::new();
        let mut taken = vec![vec![false; width]; height];
        let tower_count: usize = lines.next().unwrap_or_default().trim().parse().unwrap_or(0);
        for _ in 0..tower_count {
            let l = lines.next().unwrap_or_default();
            let t: Vec<&str> = l.split_whitespace().collect();
            if t.len() < 8 {
                continue;
            }
            let (x, y): (i32, i32) = (t[3].parse().unwrap_or(-1), t[4].parse().unwrap_or(-1));
            if x >= 0 && (x as usize) < width && y >= 0 && (y as usize) < height {
                taken[y as usize][x as usize] = true;
            }
            let owner: i32 = t[2].parse().unwrap_or(-1);
            if (t[0] != GUN && t[0] != GLUE) || owner != me {
                continue;
            }
            let is_gun = t[0] == GUN;
            let levels = if is_gun {
                [level_of(&GUN_DAMAGE, t[5]), level_of(&GUN_RANGE, t[6]), level_of(&GUN_RELOAD, t[7])]
            } else {
                [Some(0), level_of(&GLUE_RANGE, t[6]), Some(0)]
            };
            if let [Some(d), Some(r), Some(l)] = levels {
                towers.push(Tower { id: t[1].parse().ok(), is_gun, x, y, levels: [d, r, l] });
            }
        }
        let attacker_count: usize = lines.next().unwrap_or_default().trim().parse().unwrap_or(0);
        for _ in 0..attacker_count {
            lines.next(); // attackers are not used by v1b
        }
        let commands = plan_turn(&grid, me, &mut defence, money, towers, taken);
        let mut o = out.lock();
        writeln!(o, "{}", if commands.is_empty() { "PASS".to_string() } else { commands.join(";") }).unwrap();
        o.flush().unwrap();
    }
}
