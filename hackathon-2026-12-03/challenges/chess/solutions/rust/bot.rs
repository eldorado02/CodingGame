// Chess (Chess960) — Rust port of cpp/bot.cpp (same play, same score, same tie-break).
// Build: cargo build --release
use std::io::{self, BufRead, Write};

const CHECK_BONUS: i32 = 30;
const KING_ATTACKER: i32 = 10000; // a king only takes an undefended piece
const CENTER_WEIGHT: i32 = 4; // per step closer to the centre for knights and bishops
const PAWN_ADVANCE: i32 = 3; // per rank advanced
const INPUTS: &str = "fen moves";
const KNIGHT: [(i32, i32); 8] = [(1, 2), (2, 1), (2, -1), (1, -2), (-1, -2), (-2, -1), (-2, 1), (-1, 2)];
const KING: [(i32, i32); 8] = [(1, 0), (1, 1), (0, 1), (-1, 1), (-1, 0), (-1, -1), (0, -1), (1, -1)];
const DIAGONALS: [(i32, i32); 4] = [(1, 1), (1, -1), (-1, 1), (-1, -1)];
const LINES: [(i32, i32); 4] = [(1, 0), (-1, 0), (0, 1), (0, -1)];
const EMPTY: u8 = b'.';

type Board = [[u8; 8]; 8]; // [file][rank]

fn value(piece: u8) -> i32 {
    match piece.to_ascii_lowercase() {
        b'p' => 100,
        b'n' => 320,
        b'b' => 330,
        b'r' => 500,
        b'q' => 900,
        _ => 0,
    }
}

fn is_white(piece: u8) -> bool { piece.is_ascii_uppercase() }
fn inside(f: i32, r: i32) -> bool { (0..8).contains(&f) && (0..8).contains(&r) }

fn parse_board(field: &str) -> Board {
    let mut b = [[EMPTY; 8]; 8];
    let (mut rank, mut file) = (7i32, 0i32);
    for ch in field.bytes() {
        if ch == b'/' {
            rank -= 1;
            file = 0;
        } else if ch.is_ascii_digit() {
            file += (ch - b'0') as i32;
        } else {
            if inside(file, rank) { b[file as usize][rank as usize] = ch; }
            file += 1;
        }
    }
    b
}

fn play(mut b: Board, mv: &[u8], white: bool) -> Board {
    let (sf, sr, df, dr) = (mv[0] as i32 - 'a' as i32, mv[1] as i32 - '1' as i32, mv[2] as i32 - 'a' as i32, mv[3] as i32 - '1' as i32);
    if !inside(sf, sr) || !inside(df, dr) || b[sf as usize][sr as usize] == EMPTY { return b; }
    let (sf, sr, df, dr) = (sf as usize, sr as usize, df as usize, dr as usize);
    let (mut piece, target) = (b[sf][sr], b[df][dr]);
    b[sf][sr] = EMPTY;
    if piece.to_ascii_lowercase() == b'k' && target != EMPTY && is_white(target) == white && target.to_ascii_lowercase() == b'r' {
        b[df][dr] = EMPTY; // castling: king onto its own rook
        let king_side = df > sf;
        b[if king_side { 6 } else { 2 }][dr] = piece;
        b[if king_side { 5 } else { 3 }][dr] = target;
        return b;
    }
    if piece.to_ascii_lowercase() == b'p' && sf != df && target == EMPTY { b[df][sr] = EMPTY; } // en passant
    if mv.len() > 4 { piece = if white { mv[4].to_ascii_uppercase() } else { mv[4].to_ascii_lowercase() }; }
    b[df][dr] = piece;
    b
}

/// Cheapest attacker of `white` on (f, r), None when none.
fn cheapest_attacker(b: &Board, f: i32, r: i32, white: bool) -> Option<i32> {
    let mut best: Option<i32> = None;
    let mut keep = |v: i32| best = Some(best.map_or(v, |b: i32| b.min(v)));
    let pawn_rank = if white { r - 1 } else { r + 1 };
    for df in [-1, 1] {
        let pf = f + df;
        if inside(pf, pawn_rank) {
            let p = b[pf as usize][pawn_rank as usize];
            if p != EMPTY && is_white(p) == white && p.to_ascii_lowercase() == b'p' { keep(value(b'p')); }
        }
    }
    let groups: [(&[(i32, i32)], &[u8], bool); 4] =
        [(&KNIGHT, b"n", false), (&KING, b"k", false), (&DIAGONALS, b"bq", true), (&LINES, b"rq", true)];
    for (steps, kinds, slide) in groups {
        for &(sf, sr) in steps {
            let (mut nf, mut nr) = (f + sf, r + sr);
            while inside(nf, nr) {
                let p = b[nf as usize][nr as usize];
                if p != EMPTY {
                    let k = p.to_ascii_lowercase();
                    if is_white(p) == white && kinds.contains(&k) { keep(if k == b'k' { KING_ATTACKER } else { value(p) }); }
                    break;
                }
                if !slide { break; }
                nf += sf;
                nr += sr;
            }
        }
    }
    best
}

fn evaluate(b: &Board, white: bool) -> i32 {
    let (mut score, mut worst_loss) = (0, 0);
    for f in 0..8i32 {
        for r in 0..8i32 {
            let p = b[f as usize][r as usize];
            if p == EMPTY { continue; }
            let mine = is_white(p) == white;
            let k = p.to_ascii_lowercase();
            let mut v = value(p);
            if k == b'n' || k == b'b' {
                v += CENTER_WEIGHT * (6 - ((2 * f - 7).abs() + (2 * r - 7).abs()) / 2);
            } else if k == b'p' {
                v += PAWN_ADVANCE * if is_white(p) { r } else { 7 - r };
            }
            score += if mine { v } else { -v };
            if mine && k != b'k' {
                if let Some(hit) = cheapest_attacker(b, f, r, !white) {
                    let defended = cheapest_attacker(b, f, r, white).is_some();
                    worst_loss = worst_loss.max(if defended { value(p) - hit } else { value(p) });
                }
            }
            if !mine && k == b'k' && cheapest_attacker(b, f, r, white).is_some() { score += CHECK_BONUS; }
        }
    }
    score - worst_loss
}

fn main() {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines().map_while(Result::ok);
    let stdout = io::stdout();
    let mut out = stdout.lock();
    let Some(first) = lines.next() else { return };
    let constants: usize = first.trim().parse().unwrap_or(0);
    for _ in 0..constants { lines.next(); }
    writeln!(out, "{}", INPUTS).unwrap();
    out.flush().unwrap();
    while let Some(fen) = lines.next() {
        let fen = fen.trim().to_string();
        if fen.is_empty() { return; }
        let Some(count_line) = lines.next() else { return };
        let count: usize = count_line.trim().parse().unwrap_or(0);
        let moves: Vec<String> = (0..count).map(|_| lines.next().unwrap_or_default().trim().to_string()).collect();
        let mut fields = fen.split_whitespace();
        let board = parse_board(fields.next().unwrap_or(""));
        let white = fields.next().map_or(true, |c| c == "w");
        let mut best: Option<(usize, i32)> = None;
        for (i, m) in moves.iter().enumerate() {
            if m.len() < 4 { continue; }
            let score = evaluate(&play(board, m.as_bytes(), white), white);
            if best.map_or(true, |(_, s)| score > s) { best = Some((i, score)); }
        }
        let answer = match best {
            Some((i, _)) => moves[i].as_str(),
            None => moves.first().map_or("random", |m| m.as_str()),
        };
        writeln!(out, "{}", answer).unwrap();
        out.flush().unwrap();
    }
}
