// LOCM Constructed — Rust port of cpp/bot.cpp (same deck, same battle rules).
// Build: cargo build --release
use std::io::{self, BufRead, Write};

const CREATURE: i32 = 0;
const GREEN: i32 = 1;
const RED: i32 = 2;
const BLUE: i32 = 3;
const HAND: i32 = 0;
const MY_BOARD: i32 = 1;
const LANES: i32 = 2;
const LANE_SIZE: usize = 3;
const DECK_SIZE: usize = 30;
const COPIES: usize = 2;
const ITEM_KINDS: i32 = 3;
const EXPENSIVE_COST: i32 = 7;
const EXPENSIVE_KINDS: i32 = 2;
const AREA_TARGET: i32 = 0;
const EFFICIENCY_SCALE: i32 = 100; // integer value / mana ratios
const ABILITY_LETTERS: &[u8] = b"BCDGLW";
const ABILITY_VALUE: [i32; 6] = [1, 1, 1, 3, 4, 3];
const CHARGE: i32 = 1 << 1;
const GUARD: i32 = 1 << 3;
const LETHAL: i32 = 1 << 4;
const WARD: i32 = 1 << 5;

#[derive(Clone)]
struct Card {
    number: i32, id: i32, location: i32, kind: i32, cost: i32, attack: i32, defense: i32, abilities: i32,
    my_hp: i32, opp_hp: i32, draw: i32, area: i32, lane: i32, can_attack: bool, dead: bool,
}

fn parse_abilities(text: &str) -> i32 {
    text.bytes().filter_map(|ch| ABILITY_LETTERS.iter().position(|&l| l == ch)).fold(0, |m, i| m | 1 << i)
}

/// Rough strength of a card (bodies and effects), whatever its cost.
fn value(c: &Card) -> i32 {
    let abilities: i32 = (0..6).filter(|&i| c.abilities >> i & 1 == 1).map(|i| ABILITY_VALUE[i]).sum();
    let effects = 3 * c.draw + c.my_hp - c.opp_hp;
    let body = if c.kind == CREATURE {
        2 * c.attack + 2 * c.defense + abilities
    } else {
        2 * (c.attack.abs() + c.defense.abs()) + 2 * abilities
    };
    let total = body + effects;
    if c.area == AREA_TARGET { total } else { 2 * total }
}

/// Value per mana, as a floored integer (identical in every language).
fn efficiency(c: &Card) -> i32 { (value(c) * EFFICIENCY_SCALE).div_euclid(c.cost + 1) }

fn construct(cards: &[Card]) -> Vec<String> {
    let mut ranked: Vec<&Card> = cards.iter().collect();
    ranked.sort_by_key(|c| (-efficiency(c), c.number));
    let (mut chosen, mut items, mut expensive) = (Vec::new(), 0, 0);
    for c in ranked {
        if chosen.len() * COPIES >= DECK_SIZE { break; }
        if chosen.contains(&c.number) { continue; }
        if c.kind != CREATURE && items >= ITEM_KINDS { continue; }
        if c.cost >= EXPENSIVE_COST && expensive >= EXPENSIVE_KINDS { continue; }
        chosen.push(c.number);
        items += (c.kind != CREATURE) as i32;
        expensive += (c.cost >= EXPENSIVE_COST) as i32;
    }
    chosen.iter().flat_map(|n| std::iter::repeat(format!("CHOOSE {}", n)).take(COPIES)).collect()
}

struct Battle { cards: Vec<Card>, mana: i32, hand: Vec<usize>, mine: Vec<usize>, enemy: Vec<usize>, actions: Vec<String> }

impl Battle {
    fn new(mut cards: Vec<Card>, mana: i32) -> Self {
        let mut order: Vec<usize> = (0..cards.len()).collect();
        order.sort_by_key(|&i| cards[i].id);
        let (mut hand, mut mine, mut enemy) = (Vec::new(), Vec::new(), Vec::new());
        for i in order {
            let c = &mut cards[i];
            c.can_attack = c.location == MY_BOARD;
            c.dead = false;
            match c.location {
                HAND => hand.push(i),
                MY_BOARD => mine.push(i),
                _ => enemy.push(i),
            }
        }
        Battle { cards, mana, hand, mine, enemy, actions: Vec::new() }
    }

    fn alive<'a>(&'a self, side: &'a [usize]) -> impl Iterator<Item = usize> + 'a {
        side.iter().copied().filter(move |&i| !self.cards[i].dead)
    }

    fn lane_count(&self, lane: i32) -> usize { self.alive(&self.mine).filter(|&i| self.cards[i].lane == lane).count() }

    fn summon(&mut self) {
        let creatures: Vec<usize> =
            self.hand.iter().copied().filter(|&i| self.cards[i].kind == CREATURE && self.cards[i].cost <= self.mana).collect();
        let free_slots = LANES as usize * LANE_SIZE - self.alive(&self.mine).count();
        let (mut best_mask, mut best_value) = (0usize, 0);
        for mask in 1usize..1 << creatures.len() {
            let picked: Vec<&Card> = (0..creatures.len()).filter(|k| mask >> k & 1 == 1).map(|k| &self.cards[creatures[k]]).collect();
            let cost: i32 = picked.iter().map(|c| c.cost).sum();
            if picked.len() > free_slots || cost > self.mana { continue; }
            let total: i32 = picked.iter().map(|c| value(c)).sum();
            if total > best_value {
                best_mask = mask;
                best_value = total;
            }
        }
        for (k, &i) in creatures.iter().enumerate() {
            if best_mask >> k & 1 == 0 { continue; }
            let lane = if self.lane_count(0) <= self.lane_count(1) { 0 } else { 1 };
            if self.lane_count(lane) >= LANE_SIZE { continue; }
            let c = &mut self.cards[i];
            c.lane = lane;
            c.location = MY_BOARD;
            c.can_attack = c.abilities & CHARGE != 0;
            self.mana -= c.cost;
            self.actions.push(format!("SUMMON {} {}", c.id, lane));
            self.mine.push(i);
        }
    }

    /// Deals damage (Ward absorbs it); the creature dies at 0 defense.
    fn damage(&mut self, target: usize, amount: i32) {
        if amount <= 0 { return; }
        let t = &mut self.cards[target];
        if t.abilities & WARD != 0 {
            t.abilities &= !WARD;
            return;
        }
        t.defense -= amount;
        if t.defense <= 0 { t.dead = true; }
    }

    fn killable_or_strongest(&self, item_defense: i32) -> Option<usize> {
        let kills = |c: &Card| c.abilities & WARD == 0 && c.defense <= -item_defense;
        let any_kill = self.alive(&self.enemy).any(|i| kills(&self.cards[i]));
        let mut best: Option<usize> = None;
        for i in self.alive(&self.enemy) {
            let c = &self.cards[i];
            if any_kill && !kills(c) { continue; }
            if best.map_or(true, |b| c.attack + c.defense > self.cards[b].attack + self.cards[b].defense) { best = Some(i); }
        }
        best
    }

    fn strongest_mine(&self) -> Option<usize> {
        let mut best: Option<usize> = None;
        for i in self.alive(&self.mine) {
            if best.map_or(true, |b| self.cards[i].attack > self.cards[b].attack) { best = Some(i); }
        }
        best
    }

    fn use_items(&mut self) {
        let mut items: Vec<usize> = self.hand.iter().copied().filter(|&i| self.cards[i].kind != CREATURE).collect();
        items.sort_by_key(|&i| (-value(&self.cards[i]), self.cards[i].id));
        for i in items {
            let item = self.cards[i].clone();
            if item.cost > self.mana { continue; }
            let mut target = None;
            if item.kind == GREEN {
                let Some(t) = self.strongest_mine() else { continue };
                let c = &mut self.cards[t];
                c.attack += item.attack;
                c.defense += item.defense;
                c.abilities |= item.abilities;
                target = Some(t);
            } else if item.kind == RED || (item.kind == BLUE && item.defense < 0 && self.alive(&self.enemy).next().is_some()) {
                let Some(t) = self.killable_or_strongest(item.defense) else { continue };
                let c = &mut self.cards[t];
                c.attack = (c.attack + item.attack).max(0);
                c.abilities &= !item.abilities;
                self.damage(t, -item.defense);
                target = Some(t);
            }
            self.mana -= item.cost;
            let target_id = target.map_or(-1, |t| self.cards[t].id);
            self.actions.push(format!("USE {} {}", item.id, target_id));
        }
    }

    fn attack_lane(&mut self, lane: i32) {
        let mut attackers: Vec<usize> = self
            .alive(&self.mine)
            .filter(|&i| self.cards[i].lane == lane && self.cards[i].can_attack && self.cards[i].attack > 0)
            .collect();
        while !attackers.is_empty() {
            let guard = self
                .alive(&self.enemy)
                .filter(|&i| self.cards[i].lane == lane && self.cards[i].abilities & GUARD != 0)
                .min_by_key(|&i| (self.cards[i].defense, self.cards[i].id));
            let Some(g) = guard else { break };
            let g_defense = self.cards[g].defense;
            let kills = |i: &usize| self.cards[*i].attack >= g_defense || self.cards[*i].abilities & LETHAL != 0;
            // smallest attacker that kills it, else the strongest one
            let pick = attackers
                .iter()
                .copied()
                .filter(kills)
                .min_by_key(|&i| (self.cards[i].attack, self.cards[i].id))
                .or_else(|| attackers.iter().copied().min_by_key(|&i| (-self.cards[i].attack, self.cards[i].id)))
                .unwrap();
            attackers.retain(|&i| i != pick);
            let (a_id, a_attack, a_lethal) = (self.cards[pick].id, self.cards[pick].attack, self.cards[pick].abilities & LETHAL != 0);
            self.actions.push(format!("ATTACK {} {}", a_id, self.cards[g].id));
            let lethal = a_lethal && self.cards[g].abilities & WARD == 0;
            self.damage(g, if lethal { g_defense } else { a_attack });
        }
        for i in attackers {
            self.actions.push(format!("ATTACK {} -1", self.cards[i].id));
        }
    }

    fn play(mut self) -> Vec<String> {
        self.summon();
        self.use_items();
        for lane in 0..LANES { self.attack_lane(lane); }
        self.actions
    }
}

fn ints(line: &str) -> Vec<i32> { line.split_whitespace().filter_map(|t| t.parse().ok()).collect() }

fn main() {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines().map_while(Result::ok);
    let stdout = io::stdout();
    let mut out = stdout.lock();
    loop {
        let (Some(me), Some(_opponent)) = (lines.next(), lines.next()) else { return };
        let mana = ints(&me).get(1).copied().unwrap_or(0);
        let Some(counts) = lines.next() else { return };
        let action_count = ints(&counts).get(1).copied().unwrap_or(0);
        for _ in 0..action_count { lines.next(); }
        let card_count = lines.next().map(|l| ints(&l).first().copied().unwrap_or(0)).unwrap_or(0);
        let mut cards = Vec::new();
        for _ in 0..card_count {
            let Some(line) = lines.next() else { return };
            let f: Vec<&str> = line.split_whitespace().collect();
            if f.len() < 13 { return; }
            let n = |k: usize| f[k].parse::<i32>().unwrap_or(0);
            cards.push(Card {
                number: n(0), id: n(1), location: n(2), kind: n(3), cost: n(4), attack: n(5), defense: n(6),
                abilities: parse_abilities(f[7]), my_hp: n(8), opp_hp: n(9), draw: n(10), area: n(11), lane: n(12),
                can_attack: false, dead: false,
            });
        }
        let actions = if mana == 0 { construct(&cards) } else { Battle::new(cards, mana).play() };
        let text = if actions.is_empty() { "PASS".to_string() } else { actions.join(";") };
        writeln!(out, "{}", text).unwrap();
        out.flush().unwrap();
    }
}
