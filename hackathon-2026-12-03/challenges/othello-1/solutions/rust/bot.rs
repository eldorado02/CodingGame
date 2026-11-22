// Othello 8x8 - determinist bitboard engine, fixed-depth alpha-beta (depth 6).
// Parity port of solutions/cpp/bot.cpp: same moves, same tie-breaking, same protocol.
use std::io::{self, BufRead, Write};

type U64 = u64;

const FA: U64 = 0x0101010101010101;
const FH: U64 = 0x8080808080808080;

const POS: [i32; 64] = [
     120, -20,  20,   5,   5,  20, -20, 120,
     -20, -40,  -5,  -5,  -5,  -5, -40, -20,
      20,  -5,  15,   3,   3,  15,  -5,  20,
       5,  -5,   3,   3,   3,   3,  -5,   5,
       5,  -5,   3,   3,   3,   3,  -5,   5,
      20,  -5,  15,   3,   3,  15,  -5,  20,
     -20, -40,  -5,  -5,  -5,  -5, -40, -20,
     120, -20,  20,   5,   5,  20, -20, 120,
];

const WIN: i32 = 1_000_000_000;
const INF: i32 = 2_000_000_000;
const DEPTH: i32 = 6;

#[inline(always)]
fn sh_e(x: U64) -> U64 { x.wrapping_shl(1) & !FA }
#[inline(always)]
fn sh_o(x: U64) -> U64 { x.wrapping_shr(1) & !FH }
#[inline(always)]
fn sh_n(x: U64) -> U64 { x.wrapping_shl(8) }
#[inline(always)]
fn sh_s(x: U64) -> U64 { x.wrapping_shr(8) }
#[inline(always)]
fn sh_ne(x: U64) -> U64 { x.wrapping_shl(9) & !FA }
#[inline(always)]
fn sh_no(x: U64) -> U64 { x.wrapping_shl(7) & !FH }
#[inline(always)]
fn sh_se(x: U64) -> U64 { x.wrapping_shr(7) & !FA }
#[inline(always)]
fn sh_so(x: U64) -> U64 { x.wrapping_shr(9) & !FH }

// Apply one of the 8 shifts, then 6 dilation steps total (matches C++ DILATE).
#[inline(always)]
fn dilate(sh: fn(U64) -> U64, p: U64, o: U64, empty: U64, m: &mut U64) {
    let mut t = sh(p) & o;
    t |= sh(t) & o;
    t |= sh(t) & o;
    t |= sh(t) & o;
    t |= sh(t) & o;
    t |= sh(t) & o;
    *m |= sh(t) & empty;
}

#[inline]
fn moves(p: U64, o: U64) -> U64 {
    let empty = !(p | o);
    let mut m = 0;
    dilate(sh_e, p, o, empty, &mut m);
    dilate(sh_o, p, o, empty, &mut m);
    dilate(sh_n, p, o, empty, &mut m);
    dilate(sh_s, p, o, empty, &mut m);
    dilate(sh_ne, p, o, empty, &mut m);
    dilate(sh_no, p, o, empty, &mut m);
    dilate(sh_se, p, o, empty, &mut m);
    dilate(sh_so, p, o, empty, &mut m);
    m
}

// Walk from the placed stone over opponent discs; commit only if we hit our own disc.
#[inline(always)]
fn walk(sh: fn(U64) -> U64, mv: U64, p: U64, o: U64, f: &mut U64) {
    let mut s = sh(mv);
    let mut t = 0;
    while s & o != 0 {
        t |= s;
        s = sh(s);
    }
    if s & p != 0 {
        *f |= t;
    }
}

#[inline]
fn flips(p: U64, o: U64, mv: U64) -> U64 {
    let mut f = 0;
    walk(sh_e, mv, p, o, &mut f);
    walk(sh_o, mv, p, o, &mut f);
    walk(sh_n, mv, p, o, &mut f);
    walk(sh_s, mv, p, o, &mut f);
    walk(sh_ne, mv, p, o, &mut f);
    walk(sh_no, mv, p, o, &mut f);
    walk(sh_se, mv, p, o, &mut f);
    walk(sh_so, mv, p, o, &mut f);
    f
}

#[inline]
fn apply_move(p: U64, o: U64, mv: U64) -> (U64, U64) {
    let f = flips(p, o, mv);
    (p | mv | f, o & !f)
}

#[inline]
fn neighbors(x: U64) -> U64 {
    sh_e(x) | sh_o(x) | sh_n(x) | sh_s(x) | sh_ne(x) | sh_no(x) | sh_se(x) | sh_so(x)
}

fn evaluate(p: U64, o: U64) -> i32 {
    let mut pos = 0;
    let mut x = p;
    while x != 0 {
        let b = x.trailing_zeros() as usize;
        pos += POS[b];
        x &= x - 1;
    }
    x = o;
    while x != 0 {
        let b = x.trailing_zeros() as usize;
        pos -= POS[b];
        x &= x - 1;
    }
    let empty = !(p | o);
    let nb = neighbors(empty);
    let mut score = pos
        + 20 * (moves(p, o).count_ones() as i32 - moves(o, p).count_ones() as i32)
        + 8 * ((o & nb).count_ones() as i32 - (p & nb).count_ones() as i32);
    if empty.count_ones() <= 12 {
        score += 8 * (p.count_ones() as i32 - o.count_ones() as i32);
    }
    score
}

#[inline]
fn terminal(p: U64, o: U64) -> i32 {
    let d = p.count_ones() as i32 - o.count_ones() as i32;
    if d > 0 { WIN } else if d < 0 { -WIN } else { 0 }
}

// Extract bits low-to-high, then stable insertion sort by POS[bit] descending.
fn ordered_bits(mut m: U64, out: &mut [u32; 32]) -> usize {
    let mut n = 0;
    while m != 0 {
        let b = m.trailing_zeros();
        out[n] = b;
        n += 1;
        m &= m - 1;
    }
    for i in 1..n {
        let v = out[i];
        let mut j = i as i32 - 1;
        while j >= 0 && POS[out[j as usize] as usize] < POS[v as usize] {
            out[(j + 1) as usize] = out[j as usize];
            j -= 1;
        }
        out[(j + 1) as usize] = v;
    }
    n
}

fn search(p: U64, o: U64, depth: i32, mut alpha: i32, beta: i32, passed: bool) -> i32 {
    let m = moves(p, o);
    if m == 0 {
        if passed {
            return terminal(p, o);
        }
        return -search(o, p, depth, -beta, -alpha, true);
    }
    if depth <= 0 {
        return evaluate(p, o);
    }
    let mut ord = [0u32; 32];
    let n = ordered_bits(m, &mut ord);
    let mut best = -INF;
    for i in 0..n {
        let (np, no) = apply_move(p, o, 1u64 << ord[i]);
        let val = -search(no, np, depth - 1, -beta, -alpha, false);
        if val > best {
            best = val;
        }
        if best > alpha {
            alpha = best;
        }
        if alpha >= beta {
            break;
        }
    }
    best
}

fn choose(p: U64, o: U64) -> i32 {
    let m = moves(p, o);
    if m == 0 {
        return -1;
    }
    let mut ord = [0u32; 32];
    let n = ordered_bits(m, &mut ord);
    let mut best = ord[0] as i32;
    for depth in 1..=DEPTH {
        let mut local_best = -INF;
        let mut local_move = best;
        let mut alpha = -INF;
        for i in 0..n {
            let (np, no) = apply_move(p, o, 1u64 << ord[i]);
            let val = -search(no, np, depth - 1, -INF, -alpha, false);
            if val > local_best {
                local_best = val;
                local_move = ord[i] as i32;
            }
            if val > alpha {
                alpha = val;
            }
        }
        best = local_move;
        if local_best >= WIN / 2 {
            break;
        }
    }
    best
}

// True if s is a single non-negative integer, with optional surrounding blanks.
fn parse_int(s: &str) -> Option<i32> {
    let s = s.trim_matches(' ');
    if s.is_empty() || !s.bytes().all(|c| c.is_ascii_digit()) {
        return None;
    }
    let x: i64 = s.parse().ok()?;
    if x > 1_000_000 {
        return None;
    }
    Some(x as i32)
}

fn is_board_row(s: &str) -> bool {
    let b = s.as_bytes();
    b.len() >= 8 && b[..8].iter().all(|&c| c == b'.' || c == b'0' || c == b'1')
}

fn main() {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();

    // id / boardSize only on the first three turns; a non-integer line is ignored.
    let mut id = 0;
    let mut size = 8;
    if let Some(Ok(line)) = lines.next() {
        if let Some(v) = parse_int(&line) { id = v; }
    }
    if let Some(Ok(line)) = lines.next() {
        if let Some(v) = parse_int(&line) { size = v; }
    }
    let _ = size;

    let stdout = io::stdout();
    let mut out = io::BufWriter::new(stdout.lock());

    loop {
        let mut p: U64 = 0;
        let mut o: U64 = 0;
        let mut rows = 0;
        while rows < 8 {
            let line = match lines.next() {
                Some(Ok(line)) => line,
                _ => return,
            };
            if !is_board_row(&line) {
                continue;
            }
            let b = line.as_bytes();
            for c in 0..8 {
                let bit = 1u64 << (rows * 8 + c);
                if b[c] == b'0' { p |= bit; }
                else if b[c] == b'1' { o |= bit; }
            }
            rows += 1;
        }
        if id == 1 {
            std::mem::swap(&mut p, &mut o);
        }

        // Optional "EXPERT" line: skip anything until the action count.
        let n = loop {
            let line = match lines.next() {
                Some(Ok(line)) => line,
                _ => return,
            };
            if let Some(v) = parse_int(&line) {
                break v;
            }
        };
        if n < 0 {
            break;
        }

        let mut actions: Vec<i32> = Vec::with_capacity(n as usize);
        for _ in 0..n {
            let line = match lines.next() {
                Some(Ok(line)) => line,
                _ => return,
            };
            let b = line.as_bytes();
            let mut k = 0;
            while k < b.len() && b[k] == b' ' { k += 1; }
            if k + 1 >= b.len() {
                continue;
            }
            actions.push((b[k] - b'a') as i32 + (b[k + 1] - b'1') as i32 * 8);
        }

        let mut mv = choose(p, o);
        if !actions.contains(&mv) && !actions.is_empty() {
            mv = actions[0];
        }
        if mv < 0 {
            let _ = writeln!(out, "PASS");
        } else {
            let file = (b'a' + (mv % 8) as u8) as char;
            let rank = (b'1' + (mv / 8) as u8) as char;
            let _ = writeln!(out, "{}{}", file, rank);
        }
        let _ = out.flush();
    }
}
