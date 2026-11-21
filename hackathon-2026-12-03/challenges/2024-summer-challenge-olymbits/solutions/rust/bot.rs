// Olymbits (Silver, 4 mini-jeux) — décision jointe, arithmétique entière.
use std::io::{self, BufRead, Write};

const NAMES: [&str; 4] = ["LEFT", "DOWN", "RIGHT", "UP"];
const DX: [i32; 4] = [-1, 0, 1, 0];
const DY: [i32; 4] = [0, 1, 0, -1];
const LETTER: [char; 4] = ['L', 'D', 'R', 'U'];
const STEPS: [i32; 4] = [1, 2, 3, 2];
const JUMP: [bool; 4] = [false, false, false, true];

fn clamp20(v: i32) -> i32 { v.max(-20).min(20) }
fn all_of(s: &str, set: &str) -> bool {
    !s.is_empty() && s.chars().all(|c| set.contains(c))
}

fn hurdle_step(track: &[u8], pos: i32, stun: i32, steps: i32, jump: bool,
               out_pos: &mut i32, out_stun: &mut i32) {
    if stun > 0 {
        *out_pos = pos;
        *out_stun = stun - 1;
        return;
    }
    let pos = if pos < 0 { 0 } else { pos };
    let last = track.len() as i32 - 1;
    if jump {
        let np = (pos + 2).min(last);
        *out_stun = if np < last && track[np as usize] == b'#' { 3 } else { 0 };
        *out_pos = np;
        return;
    }
    let mut np = pos;
    for s in 1..=steps {
        if pos + s >= last {
            *out_pos = last;
            *out_stun = 0;
            return;
        }
        if track[(pos + s) as usize] == b'#' {
            *out_pos = pos + s;
            *out_stun = 3;
            return;
        }
        np = pos + s;
    }
    *out_pos = np;
    *out_stun = 0;
}

fn hurdle_scores(track: &[u8], pos: i32, stun: i32) -> [i64; 4] {
    let last = track.len() as i32 - 1;
    let options = [2usize, 1, 3, 0];
    let mut out = [0i64; 4];
    for a in 0..4 {
        let mut p = pos;
        let mut s = stun;
        hurdle_step(track, p, s, STEPS[a], JUMP[a], &mut p, &mut s);
        let mut done = -1;
        if p >= last {
            done = 1;
        } else {
            for t in 0..3 {
                let mut moved = false;
                for &m in &options {
                    let mut q = p;
                    let mut ss = s;
                    hurdle_step(track, q, ss, STEPS[m], JUMP[m], &mut q, &mut ss);
                    if q > p {
                        p = q;
                        s = ss;
                        moved = true;
                        break;
                    }
                }
                if !moved { hurdle_step(track, p, s, 2, true, &mut p, &mut s); }
                if p >= last { done = t + 2; break; }
            }
        }
        if done < 0 { done = 5 + (last - p) / 2; }
        out[a] = -100 * done as i64;
    }
    out
}

fn archery_scores(gpu: &str, x: i32, y: i32) -> [i64; 4] {
    let winds: Vec<i32> = gpu.chars().filter_map(|c| c.to_digit(10).map(|d| d as i32)).collect();
    let w = winds.first().copied().unwrap_or(1);
    let mut out = [0i64; 4];
    for a in 0..4 {
        let nx = clamp20(x + DX[a] * w);
        let ny = clamp20(y + DY[a] * w);
        let d2 = nx * nx + ny * ny;
        let (mut px, mut py) = (nx, ny);
        for &fw in winds.iter().skip(1).take(5) {
            if px > 0 { px = (-20).max(px - fw); }
            else if px < 0 { px = 20.min(px + fw); }
            if py > 0 { py = 0.max(py - fw); }
            else if py < 0 { py = 0.min(py + fw); }
        }
        out[a] = (-2 * d2 - (px * px + py * py)) as i64;
    }
    out
}

fn roller_scores(gpu: &str, pos: i32, risk: i32, turns: i32,
                 others: &[i32]) -> [i64; 4] {
    let order = &gpu[..4.min(gpu.len())];
    let steps = [0i32, 1, 2, 2, 3];
    let mut out = [0i64; 4];
    for a in 0..4 {
        if let Some(idx) = order.find(LETTER[a]) {
            if risk < 0 {
                out[a] = -100;
            } else {
                let mut nr = risk + match idx { 0 => -1, 1 => 0, 2 => 1, _ => 2 };
                let collide = others.iter().any(|&p| (p - pos).rem_euclid(10) == 0) as i32;
                if collide == 1 { nr += 2; }
                if nr >= 5 {
                    out[a] = (-100 - turns) as i64;
                } else {
                    out[a] = (10 * steps[idx] - 7 * nr - 40 * collide) as i64;
                }
            }
        } else {
            out[a] = -10000;
        }
    }
    out
}

fn diving_scores(gpu: &str, combo: i32) -> [i64; 4] {
    let goal = gpu.chars().next().unwrap_or('U');
    let mut out = [0i64; 4];
    for a in 0..4 {
        if LETTER[a] == goal {
            out[a] = (10 * (8 + (combo + 1).min(10))) as i64;
        } else {
            out[a] = (10 * (-combo.min(10) - 1)) as i64;
        }
    }
    out
}

fn main() {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines().map(|l| l.unwrap_or_default());
    let player: usize = lines.next().unwrap_or_default().trim().parse().unwrap_or(0);
    let games: usize = lines.next().unwrap_or_default().trim().parse().unwrap_or(0);
    let stdout = io::stdout();
    let mut out = stdout.lock();

    loop {
        let mut alive = true;
        for _ in 0..3 {
            if lines.next().is_none() { alive = false; break; }
        }
        if !alive { break; }
        let mut total = [0i64; 4];
        for _ in 0..games {
            let line = match lines.next() { Some(l) => l, None => break };
            let mut it = line.split_whitespace();
            let gpu = match it.next() { Some(v) => v, None => continue };
            let reg: Vec<i32> = it.filter_map(|v| v.parse().ok()).collect();
            if reg.len() < 7 || gpu == "GAME_OVER" { continue; }
            let mut vals = [0i64; 4];
            let mut weight: i64 = 0;
            if gpu.len() >= 10 && all_of(gpu, ".#") {
                vals = hurdle_scores(gpu.as_bytes(), reg[player], reg[3 + player]);
                weight = 3;
            } else if all_of(gpu, "0123456789") {
                vals = archery_scores(gpu, reg[2 * player], reg[2 * player + 1]);
                weight = 1;
            } else if gpu.len() == 4 && all_of(gpu, "ULDR") && reg[6] >= 0 {
                let mut others = Vec::new();
                for i in 0..3 {
                    if i != player { others.push(reg[i]); }
                }
                vals = roller_scores(gpu, reg[player], reg[3 + player], reg[6], &others);
                weight = 2;
            } else if all_of(gpu, "ULDR") {
                vals = diving_scores(gpu, reg[3 + player]);
                weight = 2;
            }
            for a in 0..4 { total[a] += weight * vals[a]; }
        }
        let mut best = 0;
        for a in 1..4 { if total[a] > total[best] { best = a; } }
        writeln!(out, "{}", NAMES[best]).ok();
        out.flush().ok();
    }
}
