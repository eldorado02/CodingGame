// Yinsh — Rust port of cpp/bot.cpp (same simulation, same score, same tie-break).
// Build: cargo build --release
use std::io::{self, BufRead, Write};

const COLUMNS: i32 = 11;
const NUMBERS: i32 = 12; // numbers 1..11
const COLUMN_START: [i32; 11] = [2, 1, 1, 1, 1, 2, 2, 3, 4, 5, 7];
const COLUMN_SIZE: [i32; 11] = [4, 7, 8, 9, 10, 9, 10, 9, 8, 7, 4];
const EMPTY: u8 = b'.';
const MY_RING: u8 = b'R';
const MY_MARKER: u8 = b'S';
const THEIR_RING: u8 = b'r';
const THEIR_MARKER: u8 = b's';
const AXES: [(i32, i32); 3] = [(0, 1), (1, 0), (1, 1)];
const DIRECTIONS: [(i32, i32); 6] = [(0, 1), (1, 0), (1, 1), (0, -1), (-1, 0), (-1, -1)];
const ROW: i32 = 5; // markers in a winning row
const RING_VALUE: i64 = 1000;
const MAX_CELL_TEXT: usize = 3; // "k11"
const STEAL: &str = "STEAL";

type Cell = (i32, i32);
type Board = [[u8; NUMBERS as usize]; COLUMNS as usize];

fn valid(col: i32, num: i32) -> bool {
    (0..COLUMNS).contains(&col) && num >= COLUMN_START[col as usize] && num < COLUMN_START[col as usize] + COLUMN_SIZE[col as usize]
}

fn at(b: &Board, c: Cell) -> u8 { b[c.0 as usize][c.1 as usize] }
fn set(b: &mut Board, c: Cell, v: u8) { b[c.0 as usize][c.1 as usize] = v; }

/// Every line of ROW consecutive valid cells.
fn windows() -> Vec<Vec<Cell>> {
    let mut out = Vec::new();
    for col in 0..COLUMNS {
        for num in 1..NUMBERS {
            for &(dc, dn) in &AXES {
                let cells: Vec<Cell> = (0..ROW).map(|k| (col + k * dc, num + k * dn)).collect();
                if cells.iter().all(|&(c, n)| valid(c, n)) { out.push(cells); }
            }
        }
    }
    out
}

/// "f10" -> (5, 10); None when it is not a board cell.
fn parse_cell(text: &str) -> Option<Cell> {
    let bytes = text.as_bytes();
    if bytes.len() < 2 || bytes.len() > MAX_CELL_TEXT || !bytes[1..].iter().all(u8::is_ascii_digit) { return None; }
    let col = bytes[0].to_ascii_lowercase() as i32 - b'a' as i32;
    let num: i32 = text[1..].parse().ok()?;
    if valid(col, num) { Some((col, num)) } else { None }
}

/// Cells from a to b inclusive when they share an axis, else empty.
fn segment(a: Cell, b: Cell) -> Vec<Cell> {
    let (dc, dn) = (b.0 - a.0, b.1 - a.1);
    if !(dc == 0 || dn == 0 || dc == dn) { return Vec::new(); }
    let steps = dc.abs().max(dn.abs());
    (0..=steps).map(|k| (a.0 + k * dc.signum(), a.1 + k * dn.signum())).collect()
}

fn flipped(v: u8) -> u8 {
    match v {
        MY_MARKER => THEIR_MARKER,
        THEIR_MARKER => MY_MARKER,
        other => other,
    }
}

fn apply_move(b: &mut Board, src: Cell, dst: Cell) {
    let line = segment(src, dst);
    let ring = at(b, src);
    set(b, dst, ring);
    set(b, src, MY_MARKER);
    if line.len() > 2 {
        for &c in &line[1..line.len() - 1] {
            let v = flipped(at(b, c));
            set(b, c, v);
        }
    }
}

/// "xA-BxC": clears the markers A..B and the ring C; returns 1 if a ring went.
fn apply_removal(b: &mut Board, text: &str) -> i32 {
    let parts: Vec<&str> = text[1..].split('x').collect();
    let ends: Vec<&str> = parts[0].split('-').collect();
    if ends.len() == 2 {
        if let (Some(a), Some(z)) = (parse_cell(ends[0]), parse_cell(ends[1])) {
            for c in segment(a, z) {
                if at(b, c) == MY_MARKER || at(b, c) == THEIR_MARKER { set(b, c, EMPTY); }
            }
        }
    }
    match parts.get(1).and_then(|t| parse_cell(t)) {
        Some(ring) if at(b, ring) == MY_RING || at(b, ring) == THEIR_RING => {
            set(b, ring, EMPTY);
            1
        }
        _ => 0,
    }
}

/// Plays our action on the board; returns the rings it removed.
fn simulate(b: &mut Board, action: &str) -> i32 {
    let mut removed = 0;
    for step in action.split(';') {
        if step.starts_with('x') {
            removed += apply_removal(b, step);
        } else if let Some((src, dst)) = step.split_once('-') {
            if let (Some(s), Some(d)) = (parse_cell(src), parse_cell(dst)) { apply_move(b, s, d); }
        } else if let Some(c) = parse_cell(step) {
            if at(b, c) == EMPTY { set(b, c, MY_RING); }
        }
    }
    removed
}

/// Destinations of every ring of this colour.
fn mobility(b: &Board, ring: u8) -> i32 {
    let mut total = 0;
    for col in 0..COLUMNS {
        for num in 1..NUMBERS {
            if !valid(col, num) || at(b, (col, num)) != ring { continue; }
            for &(dc, dn) in &DIRECTIONS {
                let (mut c, mut n, mut jumped) = (col + dc, num + dn, false);
                while valid(c, n) && at(b, (c, n)) != MY_RING && at(b, (c, n)) != THEIR_RING {
                    if at(b, (c, n)) == EMPTY {
                        total += 1;
                        if jumped { break; }
                    } else {
                        jumped = true;
                    }
                    c += dc;
                    n += dn;
                }
            }
        }
    }
    total
}

fn evaluate(b: &Board, windows: &[Vec<Cell>]) -> i32 {
    let mut score = 0;
    for cells in windows {
        let mine = cells.iter().filter(|&&c| at(b, c) == MY_MARKER).count() as i32;
        let theirs = cells.iter().filter(|&&c| at(b, c) == THEIR_MARKER).count() as i32;
        if theirs == 0 {
            score += mine * mine;
        } else if mine == 0 {
            score -= theirs * theirs;
        }
    }
    for col in 0..COLUMNS {
        for num in (1..NUMBERS).filter(|&n| valid(col, n)) {
            score += (at(b, (col, num)) == MY_MARKER) as i32 - (at(b, (col, num)) == THEIR_MARKER) as i32;
        }
    }
    score + mobility(b, MY_RING) - mobility(b, THEIR_RING)
}

fn choose<'a>(board: &Board, actions: &'a [String], windows: &[Vec<Cell>]) -> &'a str {
    let mut best: Option<(usize, i64)> = None;
    for (i, action) in actions.iter().enumerate() {
        if action.eq_ignore_ascii_case(STEAL) { continue; }
        let mut after = *board;
        let score = RING_VALUE * simulate(&mut after, action) as i64 + evaluate(&after, windows) as i64;
        if best.map_or(true, |(_, s)| score > s) { best = Some((i, score)); }
    }
    match best {
        Some((i, _)) => &actions[i],
        None => actions.first().map_or(STEAL, |a| a.as_str()),
    }
}

/// Whitespace-separated words as they arrive (reading all stdin at once would block the game).
fn words() -> impl Iterator<Item = String> {
    io::stdin()
        .lock()
        .lines()
        .map_while(Result::ok)
        .flat_map(|line| line.split_ascii_whitespace().map(str::to_string).collect::<Vec<_>>())
}

fn main() {
    let windows = windows();
    let mut words = words();
    let stdout = io::stdout();
    let mut out = stdout.lock();
    if words.next().is_none() { return; }
    writeln!(out, "yes").unwrap();
    out.flush().unwrap();
    while let Some(count) = words.next().and_then(|t| t.parse::<i32>().ok()) {
        let mut board: Board = [[EMPTY; NUMBERS as usize]; COLUMNS as usize];
        for col in 0..count {
            let Some(row) = words.next() else { return };
            if col >= COLUMNS { continue; }
            for (k, &v) in row.as_bytes().iter().take(COLUMN_SIZE[col as usize] as usize).enumerate() {
                set(&mut board, (col, COLUMN_START[col as usize] + k as i32), v);
            }
        }
        let Some(action_count) = words.next().and_then(|t| t.parse::<i32>().ok()) else { return };
        let actions: Vec<String> = words.by_ref().take(action_count.max(0) as usize).collect();
        writeln!(out, "{}", choose(&board, &actions, &windows)).unwrap();
        out.flush().unwrap();
    }
}
