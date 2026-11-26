// Tulips and Daisies — Rust port of cpp/bot.cpp (same score, same tie-breaks).
// Build: cargo build --release
use std::io::{self, BufRead, Write};

const DIRECTIONS: [(i32, i32); 4] = [(0, 1), (1, 0), (1, 1), (1, -1)];
const ROW: usize = 4; // flowers in a line to harvest
const NET_WEIGHT: i64 = 10;
const THREAT_WEIGHT: i64 = 10;
const ATTACK: [i64; ROW + 1] = [0, 1, 4, 16, 64]; // by our flowers in a clean window
const DEFENCE: [i64; ROW + 1] = [0, 0, 2, 12, 48]; // by their flowers in a window we block
const RESERVE_TURNS: i64 = 10;
const GAP_WEIGHT: i64 = 5; // a gap harvest is worth half of a sure one
const SOIL: u8 = b'S';
const GRASS: u8 = b'G';
const ROCKS: u8 = b'R';

fn fib_sum(n: usize) -> i64 {
    let (mut total, mut a, mut b) = (0i64, 1i64, 1i64);
    for _ in 0..n {
        total += a;
        let next = a + b;
        a = b;
        b = next;
    }
    total
}

struct Field {
    width: i32,
    height: i32,
    costs: [i64; 4], // soil, grass, rocks, enemy flower
    mine: u8,
    theirs: u8,
    grid: Vec<Vec<u8>>,
    placed: Option<(i32, i32)>, // cell holding our flower in the current hypothesis
}

impl Field {
    fn inside(&self, r: i32, c: i32) -> bool { r >= 0 && r < self.height && c >= 0 && c < self.width }

    fn at(&self, r: i32, c: i32) -> u8 { if self.placed == Some((r, c)) { self.mine } else { self.grid[r as usize][c as usize] } }

    /// Planting cost of `flower` on `tile`, None on its own flower.
    fn cost(&self, tile: u8, flower: u8) -> Option<i64> {
        match tile {
            t if t == flower => None,
            SOIL => Some(self.costs[0]),
            GRASS => Some(self.costs[1]),
            ROCKS => Some(self.costs[2]),
            _ => Some(self.costs[3]),
        }
    }

    /// Gold earned by planting `flower` at (r, c).
    fn harvest(&self, r: i32, c: i32, flower: u8) -> i64 {
        let (mut count, mut harvested) = (1usize, false);
        for &(dr, dc) in &DIRECTIONS {
            let mut length = 1usize;
            for sign in [1, -1] {
                let (mut nr, mut nc) = (r + sign * dr, c + sign * dc);
                while self.inside(nr, nc) && self.at(nr, nc) == flower {
                    length += 1;
                    nr += sign * dr;
                    nc += sign * dc;
                }
            }
            if length >= ROW {
                count += length - 1;
                harvested = true;
            }
        }
        if harvested { fib_sum(count) } else { 0 }
    }

    /// Our flowers after (r, c) along (dr, dc) and the first other cell.
    fn run_end(&self, mut r: i32, mut c: i32, dr: i32, dc: i32) -> (usize, i32, i32) {
        let mut count = 0;
        r += dr;
        c += dc;
        while self.inside(r, c) && self.at(r, c) == self.mine {
            count += 1;
            r += dr;
            c += dc;
        }
        (count, r, c)
    }

    /// Best harvest - cost of filling one cell next to our run through (r, c).
    fn gap_potential(&self, r: i32, c: i32) -> i64 {
        let mut best = 0;
        for &(dr, dc) in &DIRECTIONS {
            let (forward, fr, fc) = self.run_end(r, c, dr, dc);
            let (backward, br, bc) = self.run_end(r, c, -dr, -dc);
            let length = 1 + forward + backward;
            for (er, ec, sr, sc) in [(fr, fc, dr, dc), (br, bc, -dr, -dc)] {
                if !self.inside(er, ec) { continue; }
                let total = length + self.run_end(er, ec, sr, sc).0 + 1;
                if total >= ROW {
                    let price = self.cost(self.at(er, ec), self.mine).unwrap_or(0);
                    best = best.max(fib_sum(total) - price);
                }
            }
        }
        best
    }

    /// Best opponent harvest - cost among the threat cells (0 when none pays).
    fn reply(&self, threats: &[(i32, i32)], gold: i64) -> i64 {
        let mut best = 0;
        for &(r, c) in threats {
            let Some(price) = self.cost(self.at(r, c), self.theirs) else { continue };
            if price > gold { continue; }
            let earned = self.harvest(r, c, self.theirs);
            if earned > 0 && earned - price > best { best = earned - price; }
        }
        best
    }

    fn windows(&self, r: i32, c: i32) -> i64 {
        let mut score = 0;
        for &(dr, dc) in &DIRECTIONS {
            for shift in 0..ROW as i32 {
                let cells: Vec<(i32, i32)> = (0..ROW as i32).map(|k| (r + (k - shift) * dr, c + (k - shift) * dc)).collect();
                if !cells.iter().all(|&(cr, cc)| self.inside(cr, cc)) { continue; }
                let ours = cells.iter().filter(|&&(cr, cc)| self.at(cr, cc) == self.mine).count();
                let theirs_now = cells.iter().filter(|&&(cr, cc)| self.at(cr, cc) == self.theirs).count();
                let ours_before =
                    cells.iter().filter(|&&(cr, cc)| (cr, cc) != (r, c) && self.grid[cr as usize][cc as usize] == self.mine).count();
                let theirs_before = cells.iter().filter(|&&(cr, cc)| self.grid[cr as usize][cc as usize] == self.theirs).count();
                if theirs_now == 0 { score += ATTACK[ours]; }
                if ours_before == 0 { score += DEFENCE[theirs_before]; }
            }
        }
        score
    }

    fn threats(&mut self, gold: i64) -> Vec<(i32, i32)> {
        self.placed = None;
        let mut out = Vec::new();
        for r in 0..self.height {
            for c in 0..self.width {
                let affordable = self.cost(self.grid[r as usize][c as usize], self.theirs).map_or(false, |p| p <= gold);
                if affordable && self.harvest(r, c, self.theirs) > 0 { out.push((r, c)); }
            }
        }
        out
    }

    fn choose(&mut self, gold: i64, opponent_gold: i64) -> (i32, i32) {
        let danger = self.threats(opponent_gold);
        let reserve = gold.min(RESERVE_TURNS * self.costs[1]);
        let mut best: Option<((i32, i32), i64)> = None;
        let mut cheap: Option<((i32, i32), i64, i64)> = None;
        for r in 0..self.height {
            for c in 0..self.width {
                let Some(price) = self.cost(self.grid[r as usize][c as usize], self.mine) else { continue };
                if price > gold { continue; }
                self.placed = None;
                let earned = self.harvest(r, c, self.mine);
                self.placed = Some((r, c));
                let gap = if earned > 0 { 0 } else { GAP_WEIGHT * self.gap_potential(r, c) };
                let score = NET_WEIGHT * (earned - price) - THREAT_WEIGHT * self.reply(&danger, opponent_gold) + self.windows(r, c) + gap;
                if (price == 0 || gold - price + earned >= reserve) && best.map_or(true, |(_, s)| score > s) {
                    best = Some(((r, c), score));
                }
                if cheap.map_or(true, |(_, cp, cs)| price < cp || (price == cp && score > cs)) {
                    cheap = Some(((r, c), price, score));
                }
            }
        }
        best.map(|(cell, _)| cell).or(cheap.map(|(cell, _, _)| cell)).unwrap_or((0, 0))
    }
}

fn main() {
    let stdin = io::stdin();
    let mut words = stdin.lock().lines().map_while(Result::ok).flat_map(|l| l.split_whitespace().map(str::to_string).collect::<Vec<_>>());
    let int = |w: &mut dyn Iterator<Item = String>| w.next().and_then(|t| t.parse::<i64>().ok());
    let (Some(width), Some(height)) = (int(&mut words), int(&mut words)) else { return };
    let mut costs = [0i64; 4];
    for v in costs.iter_mut() {
        let Some(n) = int(&mut words) else { return };
        *v = n;
    }
    let (Some(mine), Some(theirs)) = (words.next(), words.next()) else { return };
    let mut field = Field {
        width: width as i32,
        height: height as i32,
        costs,
        mine: mine.as_bytes()[0].to_ascii_uppercase(),
        theirs: theirs.as_bytes()[0].to_ascii_uppercase(),
        grid: Vec::new(),
        placed: None,
    };
    let stdout = io::stdout();
    let mut out = stdout.lock();
    while int(&mut words).is_some() {
        let (Some(gold), Some(opponent_gold)) = (int(&mut words), int(&mut words)) else { return };
        let mut grid = Vec::with_capacity(height as usize);
        for _ in 0..height {
            let Some(row) = words.next() else { return };
            let mut cells = row.into_bytes();
            cells.resize(width as usize, SOIL);
            grid.push(cells);
        }
        field.grid = grid;
        let (r, c) = field.choose(gold, opponent_gold);
        writeln!(out, "{} {}", r, c).unwrap();
        out.flush().unwrap();
    }
}
