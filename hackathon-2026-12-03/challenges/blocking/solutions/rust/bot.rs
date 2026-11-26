// Blocking — Rust port of cpp/bot.cpp (same score, same tie-break).
// Build: cargo build --release
use std::collections::HashMap;
use std::io::{self, BufRead, Write};

const SIZE_WEIGHT: i64 = 100;

fn main() {
    let stdin = io::stdin();
    let mut words = stdin.lock().lines().map_while(Result::ok).flat_map(|l| l.split_whitespace().map(str::to_string).collect::<Vec<_>>());
    let mut next = move || words.next();
    let int = |w: Option<String>| w.and_then(|t| t.parse::<i64>().ok());
    let Some(shape_count) = int(next()) else { return };
    let mut sizes: HashMap<char, i64> = HashMap::new();
    for _ in 0..shape_count {
        let (Some(letter), _, _, Some(definition)) = (next(), next(), next(), next()) else { return };
        sizes.insert(letter.chars().next().unwrap_or(' '), definition.matches('#').count() as i64);
    }
    for _ in 0..4 {
        if next().is_none() { return; } // players, our id, board size, letters
    }
    let stdout = io::stdout();
    let mut out = stdout.lock();
    let mut size = 0i64;
    while let Some(first_row) = next() {
        if size == 0 { size = first_row.len() as i64; }
        for _ in 0..size - 1 { next(); }
        let Some(played) = int(next()) else { return };
        for _ in 0..4 * played { next(); }
        let Some(count) = int(next()) else { return };
        let mut best: Option<(i64, String)> = None;
        for _ in 0..count {
            let (Some(col), Some(row), Some(shape)) = (int(next()), int(next()), next()) else { return };
            // doubled distance to the centre keeps everything integer
            let distance = (2 * col - (size - 1)).abs() + (2 * row - (size - 1)).abs();
            let squares = sizes.get(&shape.chars().next().unwrap_or(' ')).copied().unwrap_or(0);
            let score = 2 * SIZE_WEIGHT * squares - distance;
            if best.as_ref().map_or(true, |(s, _)| score > *s) { best = Some((score, format!("{} {} {}", col, row, shape))); }
        }
        writeln!(out, "{}", best.map_or("0 0 A001".to_string(), |(_, m)| m)).unwrap();
        out.flush().unwrap();
    }
}
