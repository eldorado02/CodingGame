// 6 nimmt! — Rust port of cpp/bot.cpp (same cost model, same tie-breaks).
// Build: cargo build --release
use std::io::{self, BufRead, Write};

const PHASE_PLAY: &str = "CHOOSE_CARD_TO_PLAY";
const DECK_SIZE: i32 = 104;
const HAND_SIZE: usize = 10; // a full hand means a new round: forget the seen cards
const ROW_LIMIT: i32 = 5; // the sixth card of a row takes the five others
const ROWS: usize = 4;
const DISQUALIFIED_SCORE: i32 = -999;

fn cows(card: i32) -> i32 {
    if card == 55 { 7 } else if card % 11 == 0 { 5 } else if card % 10 == 0 { 3 } else if card % 5 == 0 { 2 } else { 1 }
}

fn row_cows(row: &[i32]) -> i32 { row.iter().map(|&c| cows(c)).sum() }

fn last_of(row: &[i32]) -> i32 { *row.last().unwrap() }

/// Index of the row the card goes on, or None when it is below every row.
fn target_row(rows: &[Vec<i32>], card: i32) -> Option<usize> {
    let mut best: Option<usize> = None;
    for (i, row) in rows.iter().enumerate() {
        if last_of(row) < card && best.map_or(true, |b| last_of(row) > last_of(&rows[b])) { best = Some(i); }
    }
    best
}

fn binomial(n: i32, k: i32) -> i64 {
    let mut value: i64 = 1;
    for i in 0..k { value = value * (n - i) as i64 / (i + 1) as i64; }
    value
}

/// P(Binomial(draws, p) >= count), products written out for identical floats.
fn at_least(count: i32, draws: i32, p: f64) -> f64 {
    if count <= 0 { return 1.0; }
    let mut total = 0.0;
    for j in count..=draws {
        let mut term = 1.0;
        for _ in 0..j { term *= p; }
        for _ in 0..draws - j { term *= 1.0 - p; }
        total += binomial(draws, j) as f64 * term;
    }
    total
}

fn gap_unseen(seen: &[bool], low: i32, high: i32) -> i32 { (low + 1..high).filter(|&c| !seen[c as usize]).count() as i32 }

/// (expected cows, gap) of playing this card now.
fn play_cost(rows: &[Vec<i32>], seen: &[bool], unseen: i32, others: i32, card: i32) -> (f64, i32) {
    match target_row(rows, card) {
        None => (rows.iter().map(|r| row_cows(r)).min().unwrap() as f64, 0),
        Some(r) => {
            let gap = gap_unseen(seen, last_of(&rows[r]), card);
            let p = if unseen > 0 { gap as f64 / unseen as f64 } else { 0.0 };
            let slots = ROW_LIMIT - rows[r].len() as i32;
            (at_least(slots, others, p) * row_cows(&rows[r]) as f64, gap)
        }
    }
}

fn choose_card(rows: &[Vec<i32>], hand: &[i32], seen: &[bool], others: i32) -> i32 {
    let unseen = (1..=DECK_SIZE).filter(|&c| !seen[c as usize]).count() as i32;
    let mut sorted = hand.to_vec();
    sorted.sort();
    let mut best = sorted[0];
    let mut best_key: Option<(f64, i32)> = None;
    for &card in &sorted {
        let key = play_cost(rows, seen, unseen, others, card);
        let better = match best_key {
            None => true,
            Some((cost, gap)) => key.0 < cost || (key.0 == cost && key.1 < gap),
        };
        if better {
            best = card;
            best_key = Some(key);
        }
    }
    best
}

fn choose_row(rows: &[Vec<i32>]) -> usize {
    let mut best = 0;
    for i in 1..rows.len() {
        if row_cows(&rows[i]) < row_cows(&rows[best]) { best = i; }
    }
    best
}

fn ints(line: &str) -> Vec<i32> { line.split_whitespace().filter_map(|t| t.parse().ok()).collect() }

fn main() {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines().map(|l| l.unwrap_or_default());
    let mut next = move || lines.next();
    let header = match next() { Some(l) => ints(&l), None => return };
    if header.is_empty() { return; }
    let player_count = header[0] as usize;
    let mut seen = vec![false; DECK_SIZE as usize + 2];
    let stdout = io::stdout();
    let mut out = stdout.lock();
    // The real referee sends the phase as the LAST line of a turn (the statement
    // lists it first): last cards, rows, scores, hand size, hand, phase.
    while let Some(last_line) = next() {
        let last_played = ints(&last_line);
        let mut rows = Vec::with_capacity(ROWS);
        for _ in 0..ROWS {
            next();
            let row = ints(&next().unwrap_or_default());
            rows.push(if row.is_empty() { vec![0] } else { row });
        }
        let scores = ints(&next().unwrap_or_default());
        next();
        let hand = ints(&next().unwrap_or_default());
        let phase = next().unwrap_or_default().trim().to_string();
        let play = phase == PHASE_PLAY;
        if play && hand.len() == HAND_SIZE { seen = vec![false; DECK_SIZE as usize + 2]; }
        for &card in last_played.iter().chain(hand.iter()).chain(rows.iter().flatten()) {
            if (1..=DECK_SIZE).contains(&card) { seen[card as usize] = true; }
        }
        let active = scores.iter().take(player_count).filter(|&&s| s != DISQUALIFIED_SCORE).count() as i32;
        let others = (active - 1).max(0);
        if play && !hand.is_empty() {
            writeln!(out, "PLAY {}", choose_card(&rows, &hand, &seen, others)).unwrap();
        } else {
            writeln!(out, "PICK {}", choose_row(&rows)).unwrap();
        }
        out.flush().unwrap();
    }
}
