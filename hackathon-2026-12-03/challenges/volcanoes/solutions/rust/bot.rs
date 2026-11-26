// Volcanoes — Rust port of cpp/bot.cpp (same simulation, same distance, same tie-break).
// Build: cargo build --release
use std::collections::{HashMap, VecDeque};
use std::io::{self, BufRead, Write};

const MAX_LEVEL: i32 = 4;
const DIST_WEIGHT: i32 = 100;
const BLOCKED: i32 = 1000; // no chain possible

struct Board { index: HashMap<String, usize>, neighbours: Vec<[usize; 3]>, pairs: Vec<(usize, usize)> }

impl Board {
    fn play(&self, levels: &[i32], tile: usize) -> Vec<i32> {
        let mut after = levels.to_vec();
        after[tile] = if after[tile] > 0 { after[tile] + 1 } else { 1 };
        let mut erupted = vec![false; after.len()];
        let mut queue: VecDeque<usize> = if after[tile] >= MAX_LEVEL { VecDeque::from([tile]) } else { VecDeque::new() };
        while let Some(t) = queue.pop_front() {
            if erupted[t] { continue; }
            erupted[t] = true;
            for &n in &self.neighbours[t] {
                if after[n] == 0 {
                    after[n] = 1;
                } else if after[n] > 0 && !erupted[n] {
                    after[n] = (after[n] + 1).min(MAX_LEVEL);
                    if after[n] >= MAX_LEVEL { queue.push_back(n); }
                } else if after[n] < 0 {
                    after[n] = 0;
                }
            }
        }
        after
    }

    fn cost(levels: &[i32], sign: i32, tile: usize) -> Option<i32> {
        let v = levels[tile] * sign;
        if v > 0 { Some(0) } else if v == 0 { Some(1) } else { None }
    }

    /// Fewest empty tiles to fill for a N<k>-S<k> chain of `sign` volcanoes.
    fn distance(&self, levels: &[i32], sign: i32) -> i32 {
        let mut best = BLOCKED;
        for &(north, south) in &self.pairs {
            let Some(start) = Self::cost(levels, sign, north) else { continue };
            let mut dist = vec![BLOCKED; levels.len()];
            dist[north] = start;
            let mut queue = VecDeque::from([north]);
            while let Some(t) = queue.pop_front() {
                for &n in &self.neighbours[t] {
                    let Some(step) = Self::cost(levels, sign, n) else { continue };
                    if dist[t] + step >= dist[n] { continue; }
                    dist[n] = dist[t] + step;
                    if step == 0 { queue.push_front(n); } else { queue.push_back(n); }
                }
            }
            best = best.min(dist[south]);
        }
        best
    }

    fn choose<'a>(&self, levels: &[i32], moves: &'a [String]) -> &'a str {
        let mut best: Option<(usize, i32)> = None;
        for (i, name) in moves.iter().enumerate() {
            let Some(&tile) = self.index.get(name) else { continue };
            let after = self.play(levels, tile);
            let total: i32 = after.iter().filter(|&&v| v > 0).sum();
            let score = DIST_WEIGHT * (self.distance(&after, -1) - self.distance(&after, 1)) + total;
            if best.map_or(true, |(_, s)| score > s) { best = Some((i, score)); }
        }
        match best {
            Some((i, _)) => &moves[i],
            None => moves.first().map_or("RANDOM", |m| m.as_str()),
        }
    }
}

fn main() {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines().map_while(Result::ok);
    let Some(first) = lines.next() else { return };
    let Ok(count) = first.trim().parse::<usize>() else { return };
    let (mut names, mut neighbours) = (Vec::new(), Vec::new());
    for _ in 0..count {
        let line = lines.next().unwrap_or_default();
        let p: Vec<&str> = line.split_whitespace().collect();
        if p.len() < 4 { return; }
        names.push(p[0].to_string());
        neighbours.push([p[1].parse().unwrap_or(0), p[2].parse().unwrap_or(0), p[3].parse().unwrap_or(0)]);
    }
    let index: HashMap<String, usize> = names.iter().enumerate().map(|(i, n)| (n.clone(), i)).collect();
    let pairs = names
        .iter()
        .enumerate()
        .filter(|(_, n)| n.starts_with('N'))
        .filter_map(|(i, n)| index.get(&format!("S{}", &n[1..])).map(|&s| (i, s)))
        .collect();
    let board = Board { index, neighbours, pairs };
    let stdout = io::stdout();
    let mut out = stdout.lock();
    while let Some(position) = lines.next() {
        if position.trim().is_empty() { return; }
        let mut levels: Vec<i32> = position.split_whitespace().filter_map(|t| t.parse().ok()).take(count).collect();
        levels.resize(count, 0);
        let moves: Vec<String> = lines.next().unwrap_or_default().split_whitespace().map(str::to_string).collect();
        writeln!(out, "{}", board.choose(&levels, &moves)).unwrap();
        out.flush().unwrap();
    }
}
