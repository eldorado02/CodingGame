// Poker (Texas hold'em) — Rust port of cpp/bot.cpp (same equity, same decisions).
// Build: cargo build --release
use std::io::{self, BufRead, Write};

const RANKS: &str = "23456789TJQKA";
const SUITS: &str = "HDSC";
const HIGH_CARD: i64 = 0;
const PAIR: i64 = 1;
const TWO_PAIR: i64 = 2;
const TRIPS: i64 = 3;
const STRAIGHT: i64 = 4;
const FLUSH: i64 = 5;
const FULL_HOUSE: i64 = 6;
const QUADS: i64 = 7;
const STRAIGHT_FLUSH: i64 = 8;
const ALL_IN_PERCENT: i64 = 80;
const RAISE_PERCENT: i64 = 62;
const PUSH_PERCENT: i64 = 50;
const SHORT_STACK_BLINDS: i64 = 10;
const BOARD_SIZE: usize = 5;
const SEED_SALT: u64 = 2654435761;
const BLIND_CAP: i64 = 1_000_000_000; // blinds keep doubling: stay in 64 bits

/// xorshift32: identical sequence in every language.
struct Rng(u32);

impl Rng {
    fn new(seed: u64) -> Self { Rng(if seed as u32 == 0 { 1 } else { seed as u32 }) }
    fn next(&mut self) -> u32 {
        let mut x = self.0;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        self.0 = x;
        x
    }
}

/// Card index = rank * 4 + suit, unknown cards skipped.
fn parse(text: &str) -> Vec<usize> {
    text.split('_')
        .filter_map(|c| {
            let b = c.as_bytes();
            if b.len() != 2 { return None; }
            let r = RANKS.find(b[0] as char)?;
            let s = SUITS.find(b[1] as char)?;
            Some(r * 4 + s)
        })
        .collect()
}

/// Highest straight top rank (wheel = 3), or None.
fn straight_top(present: &[bool; 13]) -> Option<i64> {
    for top in (4..13).rev() {
        if (0..5).all(|k| present[top - k]) { return Some(top as i64); }
    }
    if present[12] && (0..4).all(|k| present[k]) { Some(3) } else { None }
}

fn encode(category: i64, kickers: &[i64]) -> i64 { (0..5).fold(category, |v, k| v * 16 + kickers.get(k).copied().unwrap_or(0)) }

/// Comparable value of the best 5-card hand among the cards.
fn evaluate(cards: &[usize]) -> i64 {
    let mut counts = [0usize; 13];
    let mut by_suit = [[false; 13]; 4];
    for &c in cards {
        counts[c / 4] += 1;
        by_suit[c % 4][c / 4] = true;
    }
    let suit_count = |s: usize| by_suit[s].iter().filter(|&&b| b).count();
    for s in 0..4 {
        if suit_count(s) >= 5 {
            if let Some(top) = straight_top(&by_suit[s]) { return encode(STRAIGHT_FLUSH, &[top]); }
        }
    }
    let ranks: Vec<i64> = (0..13).rev().filter(|&r| counts[r] > 0).map(|r| r as i64).collect();
    let with = |n: usize| -> Vec<i64> { ranks.iter().copied().filter(|&r| counts[r as usize] == n).collect() };
    let (quads, trips, pairs) = (with(4), with(3), with(2));
    let others = |skip: &[i64], n: usize| -> Vec<i64> { ranks.iter().copied().filter(|r| !skip.contains(r)).take(n).collect() };
    let join = |head: &[i64], tail: Vec<i64>| -> Vec<i64> { head.iter().copied().chain(tail).collect() };
    if let Some(&q) = quads.first() { return encode(QUADS, &join(&[q], others(&[q], 1))); }
    if !trips.is_empty() && (trips.len() > 1 || !pairs.is_empty()) {
        let second = trips[1..].iter().chain(pairs.iter()).copied().max().unwrap_or(-1);
        return encode(FULL_HOUSE, &[trips[0], second]);
    }
    for s in 0..4 {
        if suit_count(s) >= 5 {
            let k: Vec<i64> = (0..13).rev().filter(|&r| by_suit[s][r]).take(5).map(|r| r as i64).collect();
            return encode(FLUSH, &k);
        }
    }
    let present: [bool; 13] = std::array::from_fn(|r| counts[r] > 0);
    if let Some(top) = straight_top(&present) { return encode(STRAIGHT, &[top]); }
    if let Some(&t) = trips.first() { return encode(TRIPS, &join(&[t], others(&[t], 2))); }
    if pairs.len() >= 2 { return encode(TWO_PAIR, &join(&pairs[..2], others(&pairs[..2], 1))); }
    if let Some(&p) = pairs.first() { return encode(PAIR, &join(&[p], others(&[p], 3))); }
    encode(HIGH_CARD, &others(&[], 5))
}

/// (twice the wins + ties, twice the simulations).
fn equity(hole: &[usize], board: &[usize], opponents: usize, seed: u64, simulations: i64) -> (i64, i64) {
    let mut rng = Rng::new(seed);
    let mut deck: Vec<usize> = (0..52).filter(|c| !hole.contains(c) && !board.contains(c)).collect();
    let missing = BOARD_SIZE - board.len();
    let need = missing + 2 * opponents;
    let mut score = 0;
    for _ in 0..simulations {
        for k in 0..need {
            let j = k + (rng.next() % (deck.len() - k) as u32) as usize;
            deck.swap(k, j);
        }
        let full: Vec<usize> = board.iter().chain(&deck[..missing]).copied().collect();
        let ours = evaluate(&[hole, &full[..]].concat());
        let best = (0..opponents)
            .map(|o| evaluate(&[&deck[missing + 2 * o..missing + 2 * o + 2], &full[..]].concat()))
            .max()
            .unwrap_or(0);
        score += if ours > best { 2 } else if ours == best { 1 } else { 0 };
    }
    (score, 2 * simulations)
}

fn big_blind(start: i64, multiplier: i64, per_level: i64, hand: i64) -> i64 {
    (0..(hand - 1) / per_level.max(1)).fold(start, |blind, _| (blind * multiplier).min(BLIND_CAP))
}

#[allow(clippy::too_many_arguments)]
fn decide(score: i64, total: i64, possible: &[String], to_call: i64, pot: i64, stack: i64, blind: i64) -> String {
    let find = |head: &str| possible.iter().find(|p| p.split('_').next() == Some(head));
    let spaced = |s: &str| s.replace('_', " ");
    let push = stack <= SHORT_STACK_BLINDS * blind && score * 100 >= PUSH_PERCENT * total;
    if find("ALL-IN").is_some() && (score * 100 >= ALL_IN_PERCENT * total || push) { return "ALL-IN".to_string(); }
    if score * 100 >= RAISE_PERCENT * total {
        if let Some(bet) = find("BET") { return spaced(bet); }
    }
    if find("CHECK").is_some() { return "CHECK".to_string(); }
    if find("CALL").is_some() && score * (pot + to_call) > total * to_call { return "CALL".to_string(); }
    if find("FOLD").is_some() { return "FOLD".to_string(); }
    possible.first().map_or("FOLD".to_string(), |p| spaced(p))
}

fn main() {
    let simulations: i64 = std::env::var("CG_NODES").ok().and_then(|v| v.parse().ok()).unwrap_or(400);
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines().map_while(Result::ok);
    let stdout = io::stdout();
    let mut out = stdout.lock();
    let mut init = Vec::new();
    for _ in 0..8 {
        let Some(l) = lines.next() else { return };
        init.push(l.trim().parse::<i64>().unwrap_or(0));
    }
    let (big, per_level, multiplier) = (init[1], init[2], init[3]);
    let (players, me) = (init[6].max(0) as usize, init[7] as usize);
    while let Some(round_line) = lines.next() {
        let Ok(round) = round_line.trim().parse::<i64>() else { return };
        let hand: i64 = lines.next().unwrap_or_default().trim().parse().unwrap_or(0);
        let seats: Vec<(i64, i64)> = (0..players)
            .map(|_| {
                let v: Vec<i64> = lines.next().unwrap_or_default().split_whitespace().filter_map(|t| t.parse().ok()).collect();
                (v.first().copied().unwrap_or(0), v.get(1).copied().unwrap_or(0))
            })
            .collect();
        let board = parse(lines.next().unwrap_or_default().trim());
        let hole = parse(lines.next().unwrap_or_default().trim());
        for _ in 0..2 {
            let count: usize = lines.next().unwrap_or_default().trim().parse().unwrap_or(0); // past actions, then showdowns
            for _ in 0..count { lines.next(); }
        }
        let count: usize = lines.next().unwrap_or_default().trim().parse().unwrap_or(0);
        let possible: Vec<String> = (0..count).map(|_| lines.next().unwrap_or_default().trim().to_string()).collect();
        let top = seats.iter().map(|s| s.1).max().unwrap_or(0);
        let pot: i64 = seats.iter().map(|s| s.1).sum();
        let opponents = seats.iter().enumerate().filter(|(k, s)| *k != me && s.0 + s.1 > 0).count().max(1);
        let (score, total) =
            if hole.len() == 2 { equity(&hole, &board, opponents, round as u64 * SEED_SALT + hand as u64, simulations) } else { (0, 1) };
        let answer = decide(score, total, &possible, top - seats[me].1, pot, seats[me].0, big_blind(big, multiplier, per_level, hand));
        writeln!(out, "{}", answer).unwrap();
        out.flush().unwrap();
    }
}
