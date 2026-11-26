// Code Keeper - The Hero — Rust port of cpp/bot.cpp (same rules, same tie-breaks).
// Build: cargo build --release
use std::collections::VecDeque;
use std::io::{self, BufWriter, Read, Write};

const WIDTH: i32 = 16;
const HEIGHT: i32 = 12;
const CELLS: usize = (WIDTH * HEIGHT) as usize;
const TURN_LIMIT: i32 = 150;
const HERO_FOV: i32 = 3;
const UNKNOWN: u8 = 0;
const FLOOR: u8 = 1;
const WALL: u8 = 2;
const EXIT: i32 = 0;
const OBSTACLE: i32 = 1;
const TREASURE: i32 = 2;
const POTION: i32 = 3;
const FIRST_CHEST: i32 = 4;
const LAST_CHEST: i32 = 6;
const BOX: i32 = 7;
const FIRST_MONSTER: i32 = 8;
const LAST_MONSTER: i32 = 11;
const VIEW_RANGE: [i32; 12] = [0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 3];
const ATTACK_RANGE: [i32; 12] = [0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 1];
const DAMAGE: [i32; 12] = [0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 3];
const SWORD: usize = 0;
const HAMMER: usize = 1;
const SCYTHE: usize = 2;
const BOW: usize = 3;
const WEAPON_DAMAGE: [i32; 4] = [10, 6, 7, 8];
const BOW_RANGE: i32 = 3;
const EXIT_MARGIN: i32 = 6; // spare turns kept when heading for the exit
const LOW_HEALTH: i32 = 10; // a potion is worth a detour below this
const CRITICAL_HEALTH: i32 = 6; // leave through the exit if it is known
const CAUTION_HEALTH: i32 = 10; // below this, walk around cells monsters can see
const CARDINAL: [(i32, i32); 4] = [(-1, 0), (1, 0), (0, -1), (0, 1)];
const NONE: i32 = -1;

fn cell_of(x: i32, y: i32) -> i32 { y * WIDTH + x }
fn x_of(c: i32) -> i32 { c % WIDTH }
fn y_of(c: i32) -> i32 { c / WIDTH }
fn inside(x: i32, y: i32) -> bool { (0..WIDTH).contains(&x) && (0..HEIGHT).contains(&y) }
fn is_monster_type(t: i32) -> bool { (FIRST_MONSTER..=LAST_MONSTER).contains(&t) }
fn is_chest(t: i32) -> bool { (FIRST_CHEST..=LAST_CHEST).contains(&t) }
fn chebyshev(a: i32, b: i32) -> i32 { (x_of(a) - x_of(b)).abs().max((y_of(a) - y_of(b)).abs()) }
fn is_cardinal_neighbour(a: i32, b: i32) -> bool { (x_of(a) - x_of(b)).abs() + (y_of(a) - y_of(b)).abs() == 1 }

#[derive(Clone, Copy)]
struct Monster { cell: i32, kind: i32, hp: i32 }

/// Monsters move only when they see the hero, i.e. inside our view: remember them.
struct Memory { known: [u8; CELLS], items: [i32; CELLS], monster_type: [i32; CELLS], monster_hp: [i32; CELLS], exit: i32, turn: i32 }

impl Memory {
    fn new() -> Self { Memory { known: [UNKNOWN; CELLS], items: [NONE; CELLS], monster_type: [NONE; CELLS], monster_hp: [0; CELLS], exit: NONE, turn: 0 } }

    /// Every in-bounds cell within the field of view is now known; returns the
    /// remembered monsters by increasing cell.
    fn update(&mut self, hero: i32, entities: &[[i32; 4]]) -> Vec<Monster> {
        let (hx, hy) = (x_of(hero), y_of(hero));
        for y in (hy - HERO_FOV).max(0)..(hy + HERO_FOV + 1).min(HEIGHT) {
            for x in (hx - HERO_FOV).max(0)..(hx + HERO_FOV + 1).min(WIDTH) {
                self.known[cell_of(x, y) as usize] = FLOOR;
                self.items[cell_of(x, y) as usize] = NONE;
                self.monster_type[cell_of(x, y) as usize] = NONE;
            }
        }
        for e in entities {
            if !inside(e[0], e[1]) { continue; }
            let (c, t) = (cell_of(e[0], e[1]), e[2]);
            if t == OBSTACLE {
                self.known[c as usize] = WALL;
            } else if t == EXIT || t == TREASURE || t == POTION || is_chest(t) {
                self.items[c as usize] = t;
                if t == EXIT { self.exit = c; }
            } else if t == BOX || is_monster_type(t) {
                self.monster_type[c as usize] = t;
                self.monster_hp[c as usize] = e[3];
            }
        }
        (0..CELLS)
            .filter(|&c| self.monster_type[c] != NONE)
            .map(|c| Monster { cell: c as i32, kind: self.monster_type[c], hp: self.monster_hp[c] })
            .collect()
    }
}

struct Paths { dist: [i32; CELLS], first: [i32; CELLS] }

/// Distances and first steps over known floor, avoiding blocked cells;
/// dead-end cells can be reached but the path never goes through them.
fn bfs(m: &Memory, start: i32, blocked: &[bool; CELLS], dead_end: &[bool; CELLS]) -> Paths {
    let mut p = Paths { dist: [NONE; CELLS], first: [NONE; CELLS] };
    p.dist[start as usize] = 0;
    p.first[start as usize] = start;
    let mut queue = VecDeque::from([start]);
    while let Some(c) = queue.pop_front() {
        for (dx, dy) in CARDINAL {
            let (nx, ny) = (x_of(c) + dx, y_of(c) + dy);
            if !inside(nx, ny) { continue; }
            let n = cell_of(nx, ny) as usize;
            if p.dist[n] != NONE || m.known[n] != FLOOR || blocked[n] { continue; }
            p.dist[n] = p.dist[c as usize] + 1;
            p.first[n] = if c == start { n as i32 } else { p.first[c as usize] };
            if !dead_end[n] { queue.push_back(n as i32); }
        }
    }
    p
}

fn is_frontier(m: &Memory, c: i32) -> bool {
    (-1..=1).any(|dy| {
        (-1..=1).any(|dx| {
            let (nx, ny) = (x_of(c) + dx, y_of(c) + dy);
            inside(nx, ny) && m.known[cell_of(nx, ny) as usize] == UNKNOWN
        })
    })
}

/// Higher is better: kills first, then the most dangerous, then the weakest.
fn better_target(a: &Monster, b: &Monster, damage: i32) -> bool {
    let (ka, kb) = (a.hp <= damage, b.hp <= damage);
    if ka != kb { return ka; }
    if DAMAGE[a.kind as usize] != DAMAGE[b.kind as usize] {
        return DAMAGE[a.kind as usize] > DAMAGE[b.kind as usize];
    }
    a.hp < b.hp
}

fn pick(candidates: &[Monster], damage: i32) -> Monster {
    let mut best = candidates[0];
    for m in &candidates[1..] {
        if better_target(m, &best, damage) { best = *m; }
    }
    best
}

fn attack(weapon: usize, cell: i32) -> String { format!("ATTACK {} {} {}", weapon, x_of(cell), y_of(cell)) }
fn move_to(cell: i32) -> String { format!("MOVE {} {}", x_of(cell), y_of(cell)) }

enum Fight { Act(String), Chase(i32), Nothing }

fn combat(hero: i32, charges: &[i32; 4], monsters: &[Monster]) -> Fight {
    let threats: Vec<Monster> = monsters
        .iter()
        .filter(|m| is_monster_type(m.kind) && chebyshev(m.cell, hero) <= VIEW_RANGE[m.kind as usize])
        .copied()
        .collect();
    if threats.is_empty() { return Fight::Nothing; }
    let select = |keep: &dyn Fn(&Monster) -> bool| -> Vec<Monster> { threats.iter().filter(|m| keep(m)).copied().collect() };
    let adjacent = select(&|m| is_cardinal_neighbour(m.cell, hero));
    let killable = select(&|m| m.hp <= WEAPON_DAMAGE[BOW] && chebyshev(m.cell, hero) <= BOW_RANGE);
    let diagonal = select(&|m| chebyshev(m.cell, hero) == 1);
    let hitting = select(&|m| chebyshev(m.cell, hero) <= ATTACK_RANGE[m.kind as usize]);
    if !adjacent.is_empty() { return Fight::Act(attack(SWORD, pick(&adjacent, WEAPON_DAMAGE[SWORD]).cell)); }
    if charges[BOW] > 0 && !killable.is_empty() {
        return Fight::Act(attack(BOW, pick(&killable, WEAPON_DAMAGE[BOW]).cell));
    }
    if !diagonal.is_empty() {
        let target = pick(&diagonal, WEAPON_DAMAGE[SCYTHE]);
        if charges[SCYTHE] > 0 && target.hp <= WEAPON_DAMAGE[SCYTHE] { return Fight::Act(attack(SCYTHE, target.cell)); }
        if charges[HAMMER] > 0 { return Fight::Act(attack(HAMMER, target.cell)); }
    }
    if !hitting.is_empty() { return Fight::Chase(pick(&hitting, WEAPON_DAMAGE[SWORD]).cell); } // reach a cardinal side
    Fight::Nothing
}

/// Cell to walk to, or NONE.
fn choose_target(m: &Memory, hero: i32, health: i32, p: &Paths) -> i32 {
    let remaining = TURN_LIMIT - m.turn;
    let exit_reachable = m.exit != NONE && p.dist[m.exit as usize] != NONE;
    if exit_reachable && (remaining <= p.dist[m.exit as usize] + EXIT_MARGIN || health <= CRITICAL_HEALTH) {
        return m.exit;
    }
    let mut best = NONE;
    for c in 0..CELLS as i32 {
        let cu = c as usize;
        if p.dist[cu] == NONE || c == m.exit { continue; }
        let t = m.items[cu];
        let wanted = t == TREASURE || is_chest(t) || (t == POTION && health <= LOW_HEALTH) || (c != hero && is_frontier(m, c));
        if wanted && (best == NONE || p.dist[cu] < p.dist[best as usize]) { best = c; }
    }
    if best != NONE { best } else if exit_reachable { m.exit } else { NONE }
}

/// Floor cells inside the view range of a visible monster.
fn watched_cells(monsters: &[Monster]) -> [bool; CELLS] {
    let mut watched = [false; CELLS];
    for mo in monsters.iter().filter(|mo| is_monster_type(mo.kind)) {
        let (mx, my, r) = (x_of(mo.cell), y_of(mo.cell), VIEW_RANGE[mo.kind as usize]);
        for y in (my - r).max(0)..(my + r + 1).min(HEIGHT) {
            for x in (mx - r).max(0)..(mx + r + 1).min(WIDTH) {
                watched[cell_of(x, y) as usize] = true;
            }
        }
    }
    watched
}

/// First step towards the chase cell or the chosen target, or NONE.
/// With through_monsters, monster cells are walkable (the step is a sword hit).
fn plan_step(m: &Memory, hero: i32, health: i32, monsters: &[Monster], chase: i32, extra_blocked: &[bool; CELLS], through_monsters: bool) -> i32 {
    let mut blocked = *extra_blocked;
    if !through_monsters {
        for mo in monsters.iter().filter(|mo| mo.kind != BOX) { blocked[mo.cell as usize] = true; }
    }
    blocked[hero as usize] = false;
    let mut dead_end = blocked;
    if m.exit != NONE { dead_end[m.exit as usize] = true; }
    if chase != NONE {
        blocked[chase as usize] = false;
        dead_end[chase as usize] = true;
    }
    let p = bfs(m, hero, &blocked, &dead_end);
    let target = if chase != NONE && p.dist[chase as usize] != NONE { chase } else { choose_target(m, hero, health, &p) };
    if target == NONE || target == hero { NONE } else { p.first[target as usize] }
}

fn decide(m: &Memory, hero: i32, health: i32, charges: &[i32; 4], monsters: &[Monster]) -> String {
    let chase = match combat(hero, charges, monsters) {
        Fight::Act(command) => return command,
        Fight::Chase(cell) => cell,
        Fight::Nothing => NONE,
    };
    let none_blocked = [false; CELLS];
    let mut step = NONE;
    if health < CAUTION_HEALTH && chase == NONE {
        step = plan_step(m, hero, health, monsters, chase, &watched_cells(monsters), false);
    }
    if step == NONE { step = plan_step(m, hero, health, monsters, chase, &none_blocked, false); }
    if step == NONE {
        // a monster that never moves may block the only corridor
        step = plan_step(m, hero, health, monsters, chase, &none_blocked, true);
    }
    move_to(if step != NONE { step } else { hero })
}

fn main() {
    let mut input = String::new();
    io::stdin().read_to_string(&mut input).unwrap();
    let mut tokens = input.split_ascii_whitespace().map(|t| t.parse::<i32>().unwrap_or(0));
    let mut next = move || tokens.next();
    let stdout = io::stdout();
    let mut out = BufWriter::new(stdout.lock());
    let mut memory = Memory::new();
    while let Some(x) = next() {
        let head: Vec<i32> = (0..6).filter_map(|_| next()).collect();
        if head.len() < 6 { break; }
        let (y, health) = (head[0], head[1]);
        let charges = [0, head[3], head[4], head[5]];
        let count = next().unwrap_or(0).max(0) as usize;
        let entities: Vec<[i32; 4]> =
            (0..count).map(|_| [next().unwrap_or(0), next().unwrap_or(0), next().unwrap_or(0), next().unwrap_or(0)]).collect();
        let hero = cell_of(x, y);
        let monsters = memory.update(hero, &entities);
        let action = decide(&memory, hero, health, &charges, &monsters);
        eprintln!("t={} hp={} exit={}", memory.turn, health, memory.exit);
        memory.turn += 1;
        writeln!(out, "{}", action).unwrap();
        out.flush().unwrap();
    }
}
