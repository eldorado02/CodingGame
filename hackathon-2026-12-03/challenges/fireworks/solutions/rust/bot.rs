// Fireworks (Hanabi) — Rust port of cpp/bot.cpp (same conventions, same tie-breaks).
// Build: cargo build --release
use std::collections::BTreeMap;
use std::io::{self, BufRead, Write};

const COLORS: [&str; 5] = ["WHITE", "RED", "BLUE", "GREEN", "YELLOW"];
const SLOTS: [&str; 5] = ["A", "B", "C", "D", "E"];
const TEAM: i64 = 3; // players per round
const MAX_INFO: i64 = 12;
const DECK: i64 = 50;
const HAND: i64 = 5;
const UNKNOWN: &str = "?";

#[derive(Clone, Copy)]
struct Card { colour: usize, level: i64 }

#[derive(Clone, Copy, Default)]
struct Knowledge { colour: Option<usize>, level: Option<i64> }

fn is_digits(s: &str) -> bool { !s.is_empty() && s.bytes().all(|b| b.is_ascii_digit()) }
fn number(s: &str) -> i64 { s.parse().unwrap_or(0) }
fn colour_index(name: &str) -> Option<usize> { COLORS.iter().position(|&c| c == name) }
/// "COLOR-N" -> (colour, level text) split at the first '-'.
fn partition(value: &str) -> (&str, &str) { value.split_once('-').unwrap_or((value, "")) }

struct Round {
    me: i64,
    deck: i64,
    fireworks: [i64; 5],
    my_slots: Vec<String>,
    knowledge: BTreeMap<(i64, String), Knowledge>,
    hands: BTreeMap<i64, BTreeMap<String, Card>>,
}

impl Round {
    fn new(me: i64) -> Self {
        Round {
            me,
            deck: DECK - TEAM * HAND,
            fireworks: [0; 5],
            my_slots: SLOTS.iter().map(|s| s.to_string()).collect(),
            knowledge: BTreeMap::new(),
            hands: BTreeMap::new(),
        }
    }

    fn forget(&mut self, player: i64, slot: &str) {
        self.knowledge.remove(&(player, slot.to_string()));
        if self.deck > 0 {
            self.deck -= 1;
        } else if player == self.me {
            self.my_slots.retain(|s| s != slot);
        }
    }

    fn learn(&mut self, player: i64, slot: &str, value: &str) {
        let (colour, level) = partition(value);
        let k = self.knowledge.entry((player, slot.to_string())).or_default();
        if colour != UNKNOWN {
            if let Some(c) = colour_index(colour) { k.colour = Some(c); }
        }
        if level != UNKNOWN && is_digits(level) { k.level = Some(number(level)); }
    }

    /// Full card line of a teammate: (player, slot, card).
    fn visible_card(&self, line: &str) -> Option<(i64, String, Card)> {
        let parts: Vec<&str> = line.split(':').collect();
        if parts.len() != 4 || !is_digits(parts[0]) || parts[1] != "CARD" || number(parts[0]) == self.me { return None; }
        let (colour, level) = partition(parts[3]);
        let c = colour_index(colour)?;
        if !is_digits(level) { return None; }
        Some((number(parts[0]), parts[2].to_string(), Card { colour: c, level: number(level) }))
    }

    fn apply(&mut self, events: &[String]) {
        let is_new = |line: &String| {
            let parts: Vec<&str> = line.split(':').collect();
            parts.len() >= 2 && parts[1] == "NEWGAME" && is_digits(parts[0])
        };
        let mut events = events;
        if let Some(last) = events.iter().rposition(is_new) {
            *self = Round::new(number(events[last].split(':').next().unwrap_or("0")));
            events = &events[last + 1..];
        }
        let mut tail = events.len();
        while tail > 0 && self.visible_card(&events[tail - 1]).is_some() { tail -= 1; }
        self.hands.clear();
        for line in &events[tail..] {
            if let Some((player, slot, card)) = self.visible_card(line) {
                self.hands.entry(player).or_default().insert(slot, card);
            }
        }
        let mut hinted: Option<i64> = None;
        for line in &events[..tail] {
            let parts: Vec<&str> = line.split(':').collect();
            if parts.len() < 2 || !is_digits(parts[0]) { continue; }
            let (who, kind) = (number(parts[0]), parts[1]);
            if matches!(kind, "PLAY" | "ERROR" | "DISCARD") && parts.len() >= 3 {
                if kind == "PLAY" && parts.len() >= 4 {
                    let (colour, level) = partition(parts[3]);
                    if let (Some(c), true) = (colour_index(colour), is_digits(level)) {
                        self.fireworks[c] = self.fireworks[c].max(number(level));
                    }
                }
                self.forget(who, parts[2]);
            } else if matches!(kind, "SAYLEVEL" | "SAYCOLOR") && parts.len() >= 3 && is_digits(parts[2]) {
                hinted = Some(number(parts[2]));
            } else if kind == "CARD" && parts.len() >= 4 && hinted == Some(who) {
                self.learn(who, parts[2], parts[3]);
            }
        }
    }

    fn known(&self, player: i64, slot: &str) -> Knowledge { self.knowledge.get(&(player, slot.to_string())).copied().unwrap_or_default() }

    fn possible_colours(&self, slot: &str) -> Vec<usize> { self.known(self.me, slot).colour.map_or((0..5).collect(), |c| vec![c]) }

    fn known_playable(&self, slot: &str) -> bool {
        self.known(self.me, slot).level.map_or(false, |l| self.possible_colours(slot).iter().all(|&c| self.fireworks[c] == l - 1))
    }

    fn known_useless(&self, slot: &str) -> bool {
        self.known(self.me, slot).level.map_or(false, |l| self.possible_colours(slot).iter().all(|&c| self.fireworks[c] >= l))
    }

    fn playable(&self, c: &Card) -> bool { self.fireworks[c.colour] == c.level - 1 }

    /// Level then colour hint flagging only playable cards; never repeats an
    /// attribute the player already knows (the boss plays a card it knows fully).
    fn hint_for(&self, player: i64) -> Option<String> {
        let hand = self.hands.get(&player)?;
        for (slot, card) in hand {
            let k = self.known(player, slot);
            if !self.playable(card) || (k.colour.is_some() && k.level.is_some()) { continue; }
            if k.level.is_none() && hand.values().filter(|o| o.level == card.level).all(|o| self.playable(o)) {
                return Some(format!("SAY:{}:{}", player, card.level));
            }
            if k.colour.is_none() && hand.values().filter(|o| o.colour == card.colour).all(|o| self.playable(o)) {
                return Some(format!("SAY:{}:{}", player, COLORS[card.colour]));
            }
        }
        None
    }

    fn teammates(&self) -> Vec<i64> { (1..TEAM).map(|k| (self.me + k).rem_euclid(TEAM)).collect() }

    fn harmless_hint(&self) -> String {
        for player in self.teammates() {
            if let Some(hand) = self.hands.get(&player) {
                if let Some(card) = hand.values().find(|c| self.fireworks[c.colour] >= c.level) {
                    return format!("SAY:{}:{}", player, COLORS[card.colour]);
                }
            }
        }
        for player in self.teammates() {
            if let Some(card) = self.hands.get(&player).and_then(|h| h.values().next()) {
                return format!("SAY:{}:{}", player, card.level);
            }
        }
        format!("SAY:{}:1", self.teammates()[0])
    }

    fn decide(&self, info: i64) -> String {
        if let Some(slot) = self.my_slots.iter().find(|s| self.known_playable(s)) { return format!("PLAY:{}", slot); }
        if info > 0 {
            if let Some(hint) = self.teammates().into_iter().find_map(|p| self.hint_for(p)) { return hint; }
        }
        if info < MAX_INFO && !self.my_slots.is_empty() {
            let pick = self
                .my_slots
                .iter()
                .find(|s| self.known_useless(s))
                .or_else(|| self.my_slots.iter().find(|s| !self.knowledge.contains_key(&(self.me, s.to_string()))))
                .unwrap_or(&self.my_slots[0]);
            return format!("DISCARD:{}", pick);
        }
        self.harmless_hint()
    }
}

fn main() {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines().map_while(Result::ok);
    let stdout = io::stdout();
    let mut out = stdout.lock();
    let mut game = Round::new(0);
    while let Some(head) = lines.next() {
        let values: Vec<i64> = head.split_whitespace().filter_map(|t| t.parse().ok()).collect();
        if values.len() < 2 { return; }
        let Some(count_line) = lines.next() else { return };
        let count = count_line.trim().parse::<i64>().unwrap_or(0).max(0);
        let events: Vec<String> = (0..count).map(|_| lines.next().unwrap_or_default().trim().to_string()).collect();
        game.apply(&events);
        writeln!(out, "{}", game.decide(values[1])).unwrap();
        out.flush().unwrap();
    }
}
