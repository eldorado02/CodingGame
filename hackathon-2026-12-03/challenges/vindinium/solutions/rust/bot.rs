// Vindinium — Rust port of cpp/bot.cpp (same rules, same tie-breaks).
// Build: cargo build --release
use std::collections::{BTreeMap, HashMap, HashSet, VecDeque};
use std::io::{self, BufRead, Write};

const KILL_LIFE: i64 = 20; // one hit kills
const KILL_REACH: i64 = 2; // moves to stand next to the prey
const LOW_LIFE: i64 = 50;
const TOP_UP: i64 = 80;
const MINE_FIGHT: i64 = 20;
const SAFETY: i64 = 15;
const BEER_PRICE: i64 = 2;
const WALKABLE: &[u8] = b".0123";
const TAVERN: u8 = b'T';
const MOVES: [(&str, i64, i64); 4] = [("NORTH", -1, 0), ("EAST", 0, 1), ("SOUTH", 1, 0), ("WEST", 0, -1)];

type Cell = (i64, i64);

#[derive(Clone, Copy)]
struct Entity { owner: i64, x: i64, y: i64, life: i64, gold: i64 }

struct Game { grid: Vec<Vec<u8>>, size: i64, me: i64, x_is_column: Option<bool> }

impl Game {
    fn inside(&self, (r, c): Cell) -> bool { r >= 0 && r < self.size && c >= 0 && c < self.size }
    fn at(&self, (r, c): Cell) -> u8 { self.grid[r as usize][c as usize] }
    fn cell(&self, x: i64, y: i64) -> Cell { if self.x_is_column == Some(true) { (y, x) } else { (x, y) } }

    fn detect_axes(&mut self, x: i64, y: i64) {
        if self.x_is_column.is_some() { return; }
        let spawn = b'0' + self.me as u8;
        let row_first = self.inside((x, y)) && self.at((x, y)) == spawn && !(self.inside((y, x)) && self.at((y, x)) == spawn);
        self.x_is_column = Some(!row_first);
    }

    /// Distance and first move of every walkable cell reachable from start.
    fn bfs(&self, start: Cell, blocked: &HashSet<Cell>) -> HashMap<Cell, (i64, Option<usize>)> {
        let mut reach = HashMap::new();
        if !self.inside(start) { return reach; }
        reach.insert(start, (0, None));
        let mut queue = VecDeque::from([start]);
        while let Some(cur) = queue.pop_front() {
            let (dist, first) = reach[&cur];
            for (m, &(_, dr, dc)) in MOVES.iter().enumerate() {
                let n = (cur.0 + dr, cur.1 + dc);
                if reach.contains_key(&n) || !self.inside(n) || !WALKABLE.contains(&self.at(n)) || blocked.contains(&n) { continue; }
                reach.insert(n, (dist + 1, first.or(Some(m))));
                queue.push_back(n);
            }
        }
        reach
    }

    /// (cost, move) to step into the best target cell.
    fn closest(&self, reach: &HashMap<Cell, (i64, Option<usize>)>, mut targets: Vec<Cell>) -> Option<(i64, usize)> {
        targets.sort();
        let mut best: Option<((i64, Cell), usize)> = None;
        for t in targets {
            for (m, &(_, dr, dc)) in MOVES.iter().enumerate() {
                let Some(&(dist, first)) = reach.get(&(t.0 - dr, t.1 - dc)) else { continue };
                let key = (dist + 1, t);
                if best.map_or(true, |(k, _)| key < k) { best = Some((key, first.unwrap_or(m))); }
            }
        }
        best.map(|((cost, _), m)| (cost, m))
    }

    fn decide(&self, heroes: &BTreeMap<i64, Entity>, mines: &[Entity]) -> &'static str {
        let hero = heroes[&self.me];
        let pos = self.cell(hero.x, hero.y);
        let mut blocked: HashSet<Cell> = heroes.iter().filter(|(&i, _)| i != self.me).map(|(_, h)| self.cell(h.x, h.y)).collect();
        let reach = self.bfs(pos, &blocked);
        let owners: HashSet<i64> = mines.iter().map(|m| m.owner).collect();
        let mut prey: Option<(i64, &'static str)> = None;
        for (&i, h) in heroes {
            if i == self.me || h.life > KILL_LIFE || !owners.contains(&i) { continue; }
            let c = self.cell(h.x, h.y);
            for &(_, dr, dc) in &MOVES {
                let Some(&(dist, first)) = reach.get(&(c.0 + dr, c.1 + dc)) else { continue };
                if dist <= KILL_REACH && prey.map_or(true, |(d, _)| dist < d) {
                    prey = Some((dist, first.map_or("WAIT", |m| MOVES[m].0)));
                }
            }
        }
        if let Some((_, m)) = prey { return m; }
        for (&i, h) in heroes {
            if i == self.me || h.life < hero.life { continue; } // enemies at least as healthy as us
            let c = self.cell(h.x, h.y);
            for &(_, dr, dc) in &MOVES {
                let n = (c.0 + dr, c.1 + dc);
                if n != pos { blocked.insert(n); }
            }
        }
        let reach = self.bfs(pos, &blocked);
        let taverns: Vec<Cell> = (0..self.size).flat_map(|r| (0..self.size).map(move |c| (r, c))).filter(|&p| self.at(p) == TAVERN).collect();
        let to_tavern = self.closest(&reach, taverns);
        let can_pay = hero.gold >= BEER_PRICE;
        if let Some((cost, m)) = to_tavern {
            if can_pay && (hero.life <= LOW_LIFE || (cost == 1 && hero.life <= TOP_UP)) { return MOVES[m].0; }
        }
        let targets: Vec<Cell> = mines.iter().filter(|m| m.owner != self.me).map(|m| self.cell(m.x, m.y)).collect();
        let to_mine = self.closest(&reach, targets);
        if let Some((cost, m)) = to_mine {
            if hero.life - cost - MINE_FIGHT > SAFETY { return MOVES[m].0; }
        }
        if let (Some((_, m)), true) = (to_tavern, can_pay) { return MOVES[m].0; }
        to_mine.map_or("WAIT", |(_, m)| MOVES[m].0)
    }
}

fn main() {
    let stdin = io::stdin();
    let mut words = stdin.lock().lines().map_while(Result::ok).flat_map(|l| l.split_whitespace().map(str::to_string).collect::<Vec<_>>());
    let mut next = move || words.next();
    let int = |w: Option<String>| w.and_then(|t| t.parse::<i64>().ok());
    let Some(size) = int(next()) else { return };
    let mut grid = Vec::new();
    for _ in 0..size {
        let mut row = next().unwrap_or_default().into_bytes();
        row.resize(size as usize, b'#');
        grid.push(row);
    }
    let Some(me) = int(next()) else { return };
    let mut game = Game { grid, size, me, x_is_column: None };
    let stdout = io::stdout();
    let mut out = stdout.lock();
    while let Some(count) = int(next()) {
        let mut heroes = BTreeMap::new();
        let mut mines = Vec::new();
        for _ in 0..count {
            let kind = next().unwrap_or_default();
            let v: Vec<i64> = (0..5).map(|_| int(next()).unwrap_or(0)).collect();
            let e = Entity { owner: v[0], x: v[1], y: v[2], life: v[3], gold: v[4] };
            if kind == "HERO" { heroes.insert(e.owner, e); } else { mines.push(e); }
        }
        let answer = match heroes.get(&game.me).copied() {
            None => "WAIT",
            Some(h) => {
                game.detect_axes(h.x, h.y);
                game.decide(&heroes, &mines)
            }
        };
        writeln!(out, "{}", answer).unwrap();
        out.flush().unwrap();
    }
}
