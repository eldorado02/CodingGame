// Shobu — évaluation 1 ply + meilleure réponse adverse + position bord.
use std::cmp::Ordering;
use std::io::{self, BufRead, Write};

const WIN: i32 = 1_000_000_000;
const BEAM: usize = 24;
const DIRS: [(i32, i32); 8] = [
    (0, -1), (1, -1), (1, 0), (1, 1),
    (0, 1), (-1, 1), (-1, 0), (-1, -1),
];

#[derive(Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Debug)]
struct Move { px: i32, py: i32, ax: i32, ay: i32, dx: i32, dy: i32 }

type Cells = [i32; 64];

fn board_of(x: i32, y: i32) -> i32 { 2 * (y / 4) + x / 4 }
fn is_dark(b: i32) -> bool { b % 2 == 0 }
fn is_home(b: i32, color: i32) -> bool { if color == 0 { b >= 2 } else { b < 2 } }

fn same_board(x: i32, y: i32, ux: i32, uy: i32, k: i32) -> bool {
    let nx = x + ux * k;
    let ny = y + uy * k;
    nx >= 0 && nx < 8 && ny >= 0 && ny < 8 && nx / 4 == x / 4 && ny / 4 == y / 4
}

fn gen_moves(c: &Cells, me: i32) -> Vec<Move> {
    let mut out = Vec::new();
    let op = 1 - me;
    for py in 0..8 {
        for px in 0..8 {
            if c[(py * 8 + px) as usize] != me || !is_home(board_of(px, py), me) { continue; }
            let passive_dark = is_dark(board_of(px, py));
            for &(ux, uy) in &DIRS {
                for k in 1..=2 {
                    let idx = ((py + uy * k) * 8 + px + ux * k) as usize;
                    if !same_board(px, py, ux, uy, k) || c[idx] != -1 { break; }
                    for ay in 0..8 {
                        for ax in 0..8 {
                            if c[(ay * 8 + ax) as usize] != me
                                || is_dark(board_of(ax, ay)) == passive_dark { continue; }
                            if !same_board(ax, ay, ux, uy, k) { continue; }
                            let mut n = 0;
                            let mut ok = true;
                            for j in 1..=k {
                                let v = c[((ay + uy * j) * 8 + ax + ux * j) as usize];
                                if v == me { ok = false; break; }
                                if v == op { n += 1; }
                            }
                            if !ok || n > 1 { continue; }
                            if n == 1 && same_board(ax, ay, ux, uy, k + 1) {
                                let v = c[((ay + uy * (k + 1)) * 8 + ax + ux * (k + 1)) as usize];
                                if v != -1 { continue; }
                            }
                            out.push(Move { px, py, ax, ay, dx: ux * k, dy: uy * k });
                        }
                    }
                }
            }
        }
    }
    out
}

fn apply_move(cells: &Cells, me: i32, m: Move) -> Cells {
    let mut g = *cells;
    let op = 1 - me;
    let k = m.dx.abs().max(m.dy.abs());
    let ux = m.dx.signum();
    let uy = m.dy.signum();
    g[(m.py * 8 + m.px) as usize] = -1;
    g[((m.py + m.dy) * 8 + m.px + m.dx) as usize] = me;
    for j in 1..=k {
        let x = m.ax + ux * j;
        let y = m.ay + uy * j;
        if g[(y * 8 + x) as usize] == op {
            g[(y * 8 + x) as usize] = -1;
            let tx = m.ax + ux * (k + 1);
            let ty = m.ay + uy * (k + 1);
            if same_board(m.ax, m.ay, ux, uy, k + 1) {
                g[(ty * 8 + tx) as usize] = op;
            }
            break;
        }
    }
    g[(m.ay * 8 + m.ax) as usize] = -1;
    g[((m.ay + m.dy) * 8 + m.ax + m.dx) as usize] = me;
    g
}

fn stones(g: &Cells, b: i32, color: i32) -> i32 {
    let bx = (b % 2) * 4;
    let by = (b / 2) * 4;
    let mut n = 0;
    for y in by..by + 4 {
        for x in bx..bx + 4 {
            if g[(y * 8 + x) as usize] == color { n += 1; }
        }
    }
    n
}

fn evaluate(g: &Cells, me: i32) -> i32 {
    let op = 1 - me;
    let mut om = [0; 4];
    let mut mm = [0; 4];
    let mut mno = 99;
    let mut mnm = 99;
    for b in 0..4 {
        om[b] = stones(g, b as i32, op);
        mm[b] = stones(g, b as i32, me);
        mno = mno.min(om[b]);
        mnm = mnm.min(mm[b]);
    }
    if mno == 0 { return WIN; }
    if mnm == 0 { return -WIN; }
    let sm: i32 = mm.iter().sum();
    let so: i32 = om.iter().sum();
    let mut score = 40 * (sm - so);
    score += 300 * (4 - mno) * (4 - mno);
    score -= 250 * (4 - mnm) * (4 - mnm);
    for b in 0..4 { score += 15 * (mm[b] - om[b]); }
    let mut our_edge = 0;
    let mut op_edge = 0;
    for y in 0..8 {
        for x in 0..8 {
            let c = g[(y * 8 + x) as usize];
            if c == -1 { continue; }
            let lx = x % 4;
            let ly = y % 4;
            if lx == 0 || lx == 3 || ly == 0 || ly == 3 {
                if c == me { our_edge += 1; } else { op_edge += 1; }
            }
        }
    }
    score += 25 * (op_edge - our_edge);
    score
}

fn choose(g: &Cells, me: i32) -> Move {
    let moves = gen_moves(g, me);
    if moves.is_empty() { return Move { px: 0, py: 0, ax: 1, ay: 1, dx: 0, dy: 1 }; }
    let mut scored: Vec<(i32, Move, Cells)> = moves.iter().map(|&m| {
        let ng = apply_move(g, me, m);
        let s = evaluate(&ng, me);
        (s, m, ng)
    }).collect();
    scored.sort_by(|a, b| b.0.cmp(&a.0).then(a.1.cmp(&b.1)));
    let mut best = moves[0];
    let mut best_score = -2 * WIN;
    let mut have = false;
    for &(s, m, ref ng) in scored.iter().take(BEAM) {
        if s >= WIN { return m; }
        let replies = gen_moves(ng, 1 - me);
        if replies.is_empty() { return m; }
        let mut worst = WIN;
        for &r in replies.iter().take(BEAM) {
            let rg = apply_move(ng, 1 - me, r);
            worst = worst.min(evaluate(&rg, me));
        }
        match worst.cmp(&best_score) {
            Ordering::Greater => { best_score = worst; best = m; have = true; }
            Ordering::Equal if !have || m < best => { best = m; have = true; }
            _ => {}
        }
    }
    best
}

fn main() {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();
    let stdout = io::stdout();
    let mut out = stdout.lock();
    while let Some(header) = lines.next() {
        let header = match header { Ok(h) => h, Err(_) => break };
        let h: Vec<&str> = header.split_whitespace().collect();
        if h.len() < 2 { continue; }
        let my_color: i32 = h[1].parse().unwrap_or(0);
        let mut cells: Cells = [-1; 64];
        for y in 0..8 {
            let row = match lines.next() { Some(Ok(r)) => r, _ => break };
            for (x, ch) in row.trim().chars().take(8).enumerate() {
                cells[y * 8 + x] = if ch == 'b' { 0 } else if ch == 'w' { 1 } else { -1 };
            }
        }
        let _last = lines.next();
        let count = match lines.next() { Some(Ok(l)) => l.trim().parse::<usize>().unwrap_or(0), _ => 0 };
        let mut actions: Vec<Move> = Vec::new();
        let mut can_swap = false;
        for _ in 0..count {
            let line = match lines.next() { Some(Ok(l)) => l, _ => break };
            let a: Vec<&str> = line.split_whitespace().collect();
            if a.first() == Some(&"SWAP") {
                can_swap = true;
            } else if a.len() >= 6 {
                let nums: Vec<i32> = a.iter().take(6).filter_map(|s| s.parse().ok()).collect();
                if nums.len() == 6 {
                    actions.push(Move {
                        px: nums[0], py: nums[1], ax: nums[2], ay: nums[3],
                        dx: nums[4], dy: nums[5],
                    });
                }
            }
        }
        if can_swap {
            writeln!(out, "SWAP").ok();
            out.flush().ok();
            continue;
        }
        let mut m = choose(&cells, my_color);
        if !actions.contains(&m) {
            if let Some(&first) = actions.first() { m = first; }
        }
        writeln!(out, "{} {} {} {} {} {}", m.px, m.py, m.ax, m.ay, m.dx, m.dy).ok();
        out.flush().ok();
    }
}
