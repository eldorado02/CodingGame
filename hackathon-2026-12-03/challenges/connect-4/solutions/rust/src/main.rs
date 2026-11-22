// Connect 4 (7x9): alpha-beta depth 6, window evaluation, STEAL.
use std::io::{self, BufRead, Write};

const R: usize = 7;
const C: usize = 9;
const WIN: i32 = 1_000_000;
const DR: [i32; 4] = [0, 1, 1, 1];
const DC: [i32; 4] = [1, 0, 1, -1];
const CONST: u64 = 0x9E37_79B9_7F4A_7C15;
const TSIZE: usize = 1 << 19;

#[derive(Clone)]
struct Board {
    cell: [u8; R * C],
    h: [u8; C],
}

#[derive(Clone, Copy)]
struct Entry {
    key: u64,
    depth: i32,
    val: i32,
    flag: i32,
    move_: i32,
    used: bool,
}

fn wins_at(b: &Board, r: usize, c: usize, p: u8) -> bool {
    for d in 0..4 {
        let mut cnt = 1;
        for s in 1..4 {
            let nr = r as i32 + DR[d] * s;
            let nc = c as i32 + DC[d] * s;
            if nr < 0 || nr >= R as i32 || nc < 0 || nc >= C as i32
                || b.cell[nr as usize * C + nc as usize] != p { break; }
            cnt += 1;
        }
        for s in 1..4 {
            let nr = r as i32 - DR[d] * s;
            let nc = c as i32 - DC[d] * s;
            if nr < 0 || nr >= R as i32 || nc < 0 || nc >= C as i32
                || b.cell[nr as usize * C + nc as usize] != p { break; }
            cnt += 1;
        }
        if cnt >= 4 { return true; }
    }
    false
}

fn play(b: &mut Board, col: usize, p: u8) -> bool {
    let r = R - 1 - b.h[col] as usize;
    b.cell[r * C + col] = p;
    b.h[col] += 1;
    wins_at(b, r, col, p)
}

fn evaluate(b: &Board, p: u8) -> i32 {
    let opp = 3 - p;
    let mut score = 0;
    for r in 0..R {
        for c in 0..C {
            for d in 0..4 {
                let ar = r as i32 - DR[d] * 3;
                let ac = c as i32 - DC[d] * 3;
                if ar < 0 || ac < 0 || ac >= C as i32 { continue; }
                let (mut mine, mut theirs, mut ok) = (0, 0, true);
                for s in 0..4 {
                    let rr = ar + DR[d] * s;
                    let cc = ac + DC[d] * s;
                    if rr >= R as i32 || cc >= C as i32 { ok = false; break; }
                    let v = b.cell[rr as usize * C + cc as usize];
                    if v == p { mine += 1; }
                    else if v == opp { theirs += 1; }
                }
                if !ok { continue; }
                if mine > 0 && theirs == 0 { score += 1 << (mine - 1); }
                if theirs > 0 && mine == 0 { score -= 1 << (theirs - 1); }
            }
        }
    }
    score += 2 * (b.cell[4] == p) as i32 - 2 * (b.cell[4] == opp) as i32;
    score
}

fn key(b: &Board, p: u8) -> u64 {
    let mut h = 1_469_598_103_934_665_603u64;
    for &v in &b.cell {
        h ^= (v as u64).wrapping_add(CONST)
            .wrapping_add(h << 6).wrapping_add(h >> 2);
    }
    h ^ p as u64
}

struct Tt(Vec<Entry>);
impl Tt {
    fn new() -> Self { Tt(vec![Entry { key: 0, depth: 0, val: 0, flag: 0, move_: -1, used: false }; TSIZE]) }
    fn get_mut(&mut self, k: u64) -> &mut Entry {
        let mut i = (k as usize) & (TSIZE - 1);
        for _ in 0..8 {
            if !self.0[i].used || self.0[i].key == k { return &mut self.0[i]; }
            i = (i + 1) & (TSIZE - 1);
        }
        &mut self.0[i]
    }
}

fn search(tt: &mut Tt, b: &Board, p: u8, depth: i32, mut alpha: i32, beta: i32, ply: i32) -> i32 {
    if depth <= 0 { return evaluate(b, p); }
    let mut best = -2 * WIN;
    let mut best_move = -1;
    let k = key(b, p);
    let mut tt_move = -1;
    {
        let mut i = (k as usize) & (TSIZE - 1);
        let mut e = &tt.0[i];
        for _ in 0..8 {
            if !e.used || e.key == k { break; }
            i = (i + 1) & (TSIZE - 1);
            e = &tt.0[i];
        }
        if e.used && e.key == k && e.depth >= depth {
            if e.flag == 0 { return e.val; }
            if e.flag == 1 && e.val >= beta { return e.val; }
            if e.flag == 2 && e.val <= alpha { return e.val; }
            tt_move = e.move_;
        }
    }
    let mut moves = Vec::with_capacity(C);
    if tt_move >= 0 && (tt_move as usize) < C && b.h[tt_move as usize] < R as u8 {
        moves.push(tt_move as usize);
    }
    let mut c = 4usize;
    let mut k2 = 0i32;
    while k2 < C as i32 {
        if b.h[c] < R as u8 && !moves.contains(&c) { moves.push(c); }
        k2 += 1;
        c = ((c as i32 + if k2 % 2 != 0 { k2 } else { -k2 }) + C as i32) as usize % C;
    }
    let mut full = true;
    for mv in moves {
        let mut nb = b.clone();
        let win = play(&mut nb, mv, p);
        let val = if win { WIN - ply }
            else if nb.h.iter().all(|&x| x as usize >= R) { 0 }
            else { -search(tt, &nb, 3 - p, depth - 1, -beta, -alpha, ply + 1) };
        if val > best { best = val; best_move = mv as i32; }
        if best > alpha { alpha = best; }
        if alpha >= beta { full = false; break; }
    }
    let e = tt.get_mut(k);
    e.key = k; e.depth = depth; e.val = best; e.move_ = best_move;
    e.flag = if full { 0 } else if best >= beta { 1 } else { 2 };
    e.used = true;
    best
}

fn choose(tt: &mut Tt, b: &Board, valid: &[i32], p: u8) -> i32 {
    let mut best_col = if valid.is_empty() { 4 } else { valid[0] };
    let mut best_val = -2 * WIN;
    for depth in 1..=6 {
        let mut alpha = -2 * WIN;
        let (mut local_best, mut local_val) = (-1, -2 * WIN);
        for &c in valid {
            let mut nb = b.clone();
            let win = play(&mut nb, c as usize, p);
            let val = if win { WIN }
                else if depth == 1 { evaluate(&nb, p) }
                else { -search(tt, &nb, 3 - p, depth - 1, -2 * WIN, -alpha, 1) };
            if val > local_val { local_val = val; local_best = c; }
            if val > alpha { alpha = val; }
        }
        best_col = local_best; best_val = local_val;
        if best_val >= WIN / 2 { break; }
    }
    best_col
}

fn main() {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines().map(|l| l.unwrap());
    let init = lines.next().unwrap_or_default();
    let parts: Vec<&str> = init.split_whitespace().collect();
    if parts.len() < 2 { return; }
    let me: u8 = (parts[0].parse::<i32>().unwrap() + 1) as u8;
    let stdout = io::stdout();
    let mut out = stdout.lock();
    let mut tt = Tt::new();
    while let Some(line) = lines.next() {
        if line.trim().is_empty() { continue; }
        let mut b = Board { cell: [0; R * C], h: [0; C] };
        for r in 0..R {
            let row = lines.next().unwrap_or_default();
            for c in 0..C {
                let ch = row.as_bytes().get(c).copied().unwrap_or(b'.');
                b.cell[r * C + c] = if ch == b'.' { 0 } else { ch - b'0' + 1 };
            }
        }
        for c in 0..C {
            b.h[c] = (0..R).filter(|&r| b.cell[r * C + c] != 0).count() as u8;
        }
        let n: usize = lines.next().unwrap_or_default().trim().parse().unwrap_or(0);
        let mut valid = Vec::with_capacity(n);
        let mut steal = false;
        for _ in 0..n {
            let v: i32 = lines.next().unwrap_or_default().trim().parse().unwrap();
            if v == -2 { steal = true; }
            valid.push(v);
        }
        let _ = lines.next(); // opponent previous action
        if steal { writeln!(out, "STEAL").unwrap(); }
        else { writeln!(out, "{}", choose(&mut tt, &b, &valid, me)).unwrap(); }
        out.flush().unwrap();
    }
}
