// Ghost in the Cell (Wood 4) — heuristics time-aware + plusieurs ordres/tour.
use std::io::{self, BufRead, Write};

const INF: i32 = 1_000_000;
const HORIZON: usize = 6;

#[derive(Clone, Copy)]
struct Arrival { owner: i32, src: i32, dst: i32, cnt: i32, eta: i32 }
#[derive(Clone, Copy)]
struct BombEvent { dst: i32, eta: i32 }

struct State {
    owner: Vec<i32>, units: Vec<i32>, prod: Vec<i32>, block: Vec<i32>,
    bombs: Vec<BombEvent>,
}

impl State {
    fn project(&self, i: usize, t: i32, arrivals: &[Arrival]) -> (i32, i32, i32) {
        let mut own = if self.owner[i] == 1 { self.units[i] } else { 0 };
        let mut hostile = if self.owner[i] != 1 { self.units[i] } else { 0 };
        let mut ownr = self.owner[i];
        let mut blk = self.block[i];
        for turn in 1..=t {
            for a in arrivals {
                if a.dst as usize == i && a.eta == turn {
                    if a.owner == 1 { own += a.cnt; } else { hostile += a.cnt; }
                }
            }
            let mut suppressed = blk > 0;
            for b in &self.bombs {
                if b.dst as usize == i && b.eta <= turn && turn <= b.eta + 4 {
                    suppressed = true;
                }
            }
            if !suppressed && ownr != 0 {
                if ownr == 1 { own += self.prod[i]; } else { hostile += self.prod[i]; }
            }
            for b in &self.bombs {
                if b.dst as usize == i && b.eta == turn {
                    let victim = if ownr == 1 { own } else { hostile };
                    let loss = (victim / 2).max(10);
                    if ownr == 1 { own = (own - loss).max(0); }
                    else { hostile = (hostile - loss).max(0); }
                }
            }
            if blk > 0 { blk -= 1; }
            if own > hostile { ownr = 1; own -= hostile; hostile = 0; }
            else if hostile > own { ownr = -1; hostile -= own; own = 0; }
            else { own = 0; hostile = 0; }
        }
        (ownr, own, hostile)
    }

    fn defense_need(&self, i: usize, arrivals: &[Arrival]) -> i32 {
        if self.owner[i] != 1 { return 0; }
        let mut need = 0;
        for t in 1..=HORIZON as i32 {
            let (_o, mine, hostile) = self.project(i, t, arrivals);
            if hostile > mine { need = need.max(hostile + 1 - mine); }
        }
        need
    }

    fn capture_need(&self, i: usize, eta: i32, arrivals: &[Arrival]) -> i32 {
        let (_o, mine, hostile) = self.project(i, eta, arrivals);
        if mine > hostile { 0 } else { hostile + 1 }
    }
}

fn used_from(arrivals: &[Arrival], src: i32) -> i32 {
    arrivals.iter().filter(|a| a.src == src).map(|a| a.cnt).sum()
}

fn decide(state: &mut State, dist: &[Vec<i32>], value: &[f64], turn: i32,
          mut bombs_left: i32, orders: &mut Vec<String>, arrivals: &mut Vec<Arrival>) -> i32 {
    let n = state.owner.len();
    let mut threatened: Vec<usize> = (0..n).filter(|&i| state.owner[i] == 1).collect();
    threatened.sort_by(|&a, &b| value[b].partial_cmp(&value[a]).unwrap());
    for dst in threatened {
        for _ in 0..2 {
            let mut need = state.defense_need(dst, arrivals);
            if need <= 0 { break; }
            let mut helpers: Vec<usize> = (0..n)
                .filter(|&s| s != dst && state.owner[s] == 1 && dist[s][dst] < INF
                        && dist[s][dst] <= need + 2).collect();
            helpers.sort_by(|&a, &b| {
                dist[a][dst].cmp(&dist[b][dst])
                    .then_with(|| state.units[b].cmp(&state.units[a]))
            });
            let mut sent = false;
            for src in helpers {
                let spare = state.units[src] - used_from(arrivals, src as i32)
                            - state.defense_need(src, arrivals);
                if spare <= 0 { continue; }
                let cnt = spare.min(need);
                if cnt <= 0 { continue; }
                arrivals.push(Arrival { owner: 1, src: src as i32, dst: dst as i32,
                                        cnt, eta: dist[src][dst] });
                orders.push(format!("MOVE {} {} {}", src, dst, cnt));
                need -= cnt;
                sent = true;
                if need <= 0 { break; }
            }
            if !sent { break; }
        }
    }

    for _ in 0..40 {
        let mut best = 0.0f64;
        let mut chosen: Option<(usize, usize, i32)> = None;
        for src in 0..n {
            if state.owner[src] != 1 { continue; }
            let spare = state.units[src] - used_from(arrivals, src as i32)
                        - state.defense_need(src, arrivals);
            if spare <= 0 { continue; }
            for dst in 0..n {
                if dst == src || state.owner[dst] == 1 || dist[src][dst] >= INF { continue; }
                let eta = dist[src][dst];
                let need = state.capture_need(dst, eta, arrivals);
                let (mut score, cnt) = if state.owner[dst] == 0 {
                    let cnt = spare.min(need);
                    if cnt < 1 { continue; }
                    let denom = (eta as f64 + 0.35).powi(2)
                                * (need as f64).powf(0.7).max(2.0);
                    (value[dst] / denom, cnt)
                } else {
                    if (spare as f64) < 0.7 * (need as f64) { continue; }
                    (value[dst] / ((eta as f64 + 0.35).powi(2) * 8.0), spare)
                };
                score += 0.0001 * cnt as f64;
                if chosen.is_none() || score > best {
                    best = score; chosen = Some((src, dst, cnt));
                }
            }
        }
        match chosen {
            Some((src, dst, cnt)) => {
                arrivals.push(Arrival { owner: 1, src: src as i32, dst: dst as i32,
                                        cnt, eta: dist[src][dst] });
                orders.push(format!("MOVE {} {} {}", src, dst, cnt));
            }
            None => break,
        }
    }

    if bombs_left > 0 {
        let bombed: Vec<i32> = state.bombs.iter().map(|b| b.dst).collect();
        let mut bomb_dst = -1i32;
        let mut bomb_score = 0.0f64;
        if turn <= 2 {
            let mut sources: Vec<usize> = (0..n).filter(|&i| state.owner[i] == -1).collect();
            if sources.is_empty() {
                sources = (0..n).filter(|&i| state.owner[i] == 0).collect();
            }
            for dst in 0..n {
                if state.owner[dst] == 1 || bombed.contains(&(dst as i32)) { continue; }
                let et = sources.iter().map(|&e| dist[e][dst]).min().unwrap_or(INF);
                if et >= INF || et > 10 || state.prod[dst] <= 0 { continue; }
                let score = (state.prod[dst] as f64).powi(3) / (et.max(1) as f64);
                if bomb_dst < 0 || score > bomb_score { bomb_score = score; bomb_dst = dst as i32; }
            }
        } else {
            for dst in 0..n {
                if state.owner[dst] != -1 || state.units[dst] < 18 { continue; }
                if bombed.contains(&(dst as i32)) { continue; }
                let score = state.units[dst] as f64 + 10.0 * state.prod[dst] as f64;
                if bomb_dst < 0 || score > bomb_score { bomb_score = score; bomb_dst = dst as i32; }
            }
        }
        if bomb_dst >= 0 {
            let d = bomb_dst as usize;
            let prefix = |s: usize| format!("MOVE {} {} ", s, d);
            let mut src: Option<usize> = None;
            for s in 0..n {
                if state.owner[s] != 1 || dist[s][d] >= INF { continue; }
                if orders.iter().any(|o| o.starts_with(&prefix(s))) { continue; }
                let take = match src { None => true, Some(p) => dist[s][d] < dist[p][d] };
                if take { src = Some(s); }
            }
            if let Some(s) = src {
                orders.push(format!("BOMB {} {}", s, d));
                bombs_left -= 1;
            }
        }
    }
    bombs_left
}

fn main() {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines().map(|l| l.unwrap_or_default());
    let n: usize = lines.next().unwrap_or_default().trim().parse().unwrap_or(0);
    let m: usize = lines.next().unwrap_or_default().trim().parse().unwrap_or(0);
    let mut dist = vec![vec![INF; n]; n];
    for i in 0..n { dist[i][i] = 0; }
    for _ in 0..m {
        let v: Vec<i32> = lines.next().unwrap_or_default().split_whitespace()
            .filter_map(|x| x.parse().ok()).collect();
        if v.len() < 3 { return; }
        if v[2] < dist[v[0] as usize][v[1] as usize] {
            dist[v[0] as usize][v[1] as usize] = v[2];
            dist[v[1] as usize][v[0] as usize] = v[2];
        }
    }
    for k in 0..n {
        for i in 0..n {
            for j in 0..n {
                let nd = dist[i][k] + dist[k][j];
                if nd < dist[i][j] { dist[i][j] = nd; }
            }
        }
    }
    let mut through = vec![0i32; n];
    for i in 0..n {
        for k in 0..n {
            if i == k { continue; }
            for j in 0..n {
                if j == i || j == k { continue; }
                if dist[i][j] == dist[i][k] + dist[k][j] { through[k] += 1; }
            }
        }
    }

    let stdout = io::stdout();
    let mut out = stdout.lock();
    let mut turn = 0i32;
    let mut bombs_left = 2i32;
    loop {
        let line = match lines.next() { Some(l) => l, None => break };
        if line.trim().is_empty() { continue; }
        let cnt: usize = line.trim().parse().unwrap_or(0);
        let mut state = State {
            owner: vec![0; n], units: vec![0; n], prod: vec![0; n],
            block: vec![0; n], bombs: Vec::new(),
        };
        let mut arrivals = Vec::new();
        let mut own_bombs = 0i32;
        for _ in 0..cnt {
            let l = match lines.next() { Some(l) => l, None => return };
            let p: Vec<String> = l.split_whitespace().map(|s| s.to_string()).collect();
            if p.len() < 7 { return; }
            let eid: usize = p[0].parse().unwrap_or(0);
            let v: Vec<i32> = p[2..7].iter().filter_map(|s| s.parse().ok()).collect();
            if v.len() < 5 { return; }
            match p[1].as_str() {
                "FACTORY" => {
                    state.owner[eid] = v[0]; state.units[eid] = v[1];
                    state.prod[eid] = v[2]; state.block[eid] = v[3];
                }
                "TROOP" => arrivals.push(Arrival {
                    owner: v[0], src: -1, dst: v[2], cnt: v[3], eta: v[4],
                }),
                "BOMB" => {
                    if v[0] == 1 {
                        own_bombs += 1;
                        if v[2] >= 0 && v[3] > 0 {
                            state.bombs.push(BombEvent { dst: v[2], eta: v[3] });
                        }
                    }
                }
                _ => {}
            }
        }
        bombs_left = bombs_left.min(2 - own_bombs);
        let value: Vec<f64> = (0..n).map(|i| {
            state.prod[i] as f64 + 0.1 + 0.01 * (through[i] as f64).sqrt()
        }).collect();
        let mut orders = Vec::new();
        bombs_left = decide(&mut state, &dist, &value, turn, bombs_left, &mut orders, &mut arrivals);
        writeln!(out, "{}", if orders.is_empty() { "WAIT".to_string() } else { orders.join(";") }).ok();
        out.flush().ok();
        turn += 1;
    }
}
