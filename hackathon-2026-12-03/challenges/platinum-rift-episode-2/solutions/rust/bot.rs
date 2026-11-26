// Platinum Rift 2 — Rust port of cpp/bot.cpp (same orders, same tie-breaks).
// Build: cargo build --release
use std::collections::VecDeque;
use std::io::{self, Write};

const GUARD_MARGIN: i32 = 1; // spare defenders when enemies stand next to our HQ
const UNREACHABLE: i32 = 1 << 20;

#[derive(Clone, Copy, Default)]
struct Zone { id: usize, owner: i32, pods: [i32; 2], visible: i32, platinum: i32 }

fn bfs(adj: &[Vec<usize>], start: Option<usize>) -> Vec<i32> {
    let mut dist = vec![UNREACHABLE; adj.len()];
    let Some(s) = start else { return dist };
    dist[s] = 0;
    let mut queue = VecDeque::from([s]);
    while let Some(z) = queue.pop_front() {
        for &n in &adj[z] {
            if dist[n] == UNREACHABLE {
                dist[n] = dist[z] + 1;
                queue.push_back(n);
            }
        }
    }
    dist
}

struct Game {
    me: usize,
    adj: Vec<Vec<usize>>,
    platinum: Vec<i32>, // last seen platinum per zone
    my_hq: Option<usize>,
    enemy_hq: Option<usize>,
    dist: Vec<i32>,
}

impl Game {
    fn observe(&mut self, zones: &[Zone]) {
        for z in zones.iter().filter(|z| z.visible != 0) { self.platinum[z.id] = z.platinum; }
        if self.my_hq.is_some() { return; }
        for z in zones {
            if z.owner == self.me as i32 && self.my_hq.is_none() {
                self.my_hq = Some(z.id);
            } else if z.owner == 1 - self.me as i32 && self.enemy_hq.is_none() {
                self.enemy_hq = Some(z.id);
            }
        }
        self.dist = bfs(&self.adj, self.enemy_hq);
    }

    /// Pods kept home: the enemies next to our HQ plus a margin.
    fn guards(&self, z: usize, theirs: &[i32]) -> i32 {
        if self.my_hq != Some(z) { return 0; }
        let threat: i32 = self.adj[z].iter().map(|&n| theirs[n]).sum();
        if threat > 0 { threat + GUARD_MARGIN } else { 0 }
    }

    fn spread(&self, z: usize, mut available: i32, owner: &[i32], theirs: &[i32], moves: &mut Vec<(i32, usize, usize)>) -> i32 {
        let mut targets: Vec<usize> = self.adj[z].iter().copied().filter(|&n| owner[n] != self.me as i32).collect();
        targets.sort_by_key(|&n| (-self.platinum[n], self.dist[n], n));
        for n in targets {
            if available <= 0 { break; }
            let send = available.min(theirs[n] + 1);
            moves.push((send, z, n));
            available -= send;
        }
        available
    }

    fn march(&self, z: usize, available: i32, moves: &mut Vec<(i32, usize, usize)>) {
        if available <= 0 { return; }
        let mut best: Option<usize> = None;
        for &n in &self.adj[z] {
            if self.dist[n] < self.dist[z] && best.map_or(true, |b| self.dist[n] < self.dist[b]) { best = Some(n); }
        }
        if let Some(b) = best { moves.push((available, z, b)); }
    }

    fn orders(&self, zones: &[Zone]) -> Vec<(i32, usize, usize)> {
        let owner: Vec<i32> = zones.iter().map(|z| z.owner).collect();
        let mine: Vec<i32> = zones.iter().map(|z| z.pods[self.me]).collect();
        let theirs: Vec<i32> = zones.iter().map(|z| z.pods[1 - self.me]).collect();
        let mut moves = Vec::new();
        for z in 0..zones.len() {
            if mine[z] == 0 || theirs[z] > 0 { continue; }
            let available = mine[z] - self.guards(z, &theirs);
            let available = self.spread(z, available, &owner, &theirs, &mut moves);
            self.march(z, available, &mut moves);
        }
        moves
    }
}

/// Integers as they arrive, line by line (reading all stdin at once would block the game).
fn tokens() -> impl Iterator<Item = i32> {
    io::stdin()
        .lines()
        .map_while(Result::ok)
        .flat_map(|line| line.split_ascii_whitespace().map(|t| t.parse::<i32>().unwrap_or(0)).collect::<Vec<_>>())
}

fn main() {
    let mut tokens = tokens();
    let head: Vec<i32> = tokens.by_ref().take(4).collect();
    if head.len() < 4 { return; }
    let (me, zone_count, link_count) = (head[1] as usize, head[2] as usize, head[3] as usize);
    if tokens.by_ref().take(2 * zone_count).count() < 2 * zone_count { return; }
    let mut adj = vec![Vec::new(); zone_count];
    for _ in 0..link_count {
        let link: Vec<i32> = tokens.by_ref().take(2).collect();
        if link.len() < 2 { return; }
        let (a, b) = (link[0] as usize, link[1] as usize);
        adj[a].push(b);
        adj[b].push(a);
    }
    for n in adj.iter_mut() {
        n.sort();
        n.dedup();
    }
    let mut game = Game { me, platinum: vec![0; zone_count], my_hq: None, enemy_hq: None, dist: vec![UNREACHABLE; zone_count], adj };
    let stdout = io::stdout();
    let mut out = stdout.lock();
    // pods are bought automatically: the platinum stock is unused
    while tokens.next().is_some() {
        let mut zones = vec![Zone::default(); zone_count];
        for _ in 0..zone_count {
            let v: Vec<i32> = tokens.by_ref().take(6).collect();
            if v.len() < 6 { return; }
            let id = v[0] as usize;
            zones[id] = Zone { id, owner: v[1], pods: [v[2], v[3]], visible: v[4], platinum: v[5] };
        }
        game.observe(&zones);
        let moves = game.orders(&zones);
        let line = if moves.is_empty() {
            "WAIT".to_string()
        } else {
            moves.iter().map(|(c, f, t)| format!("{} {} {}", c, f, t)).collect::<Vec<_>>().join(" ")
        };
        writeln!(out, "{}\nWAIT", line).unwrap();
        out.flush().unwrap();
    }
}
