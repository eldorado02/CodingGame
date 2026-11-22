// Shobu — minimax déterministe à budget d'évaluations.
// Traduction fidèle de /tmp/opencode/shobu_work22.cpp.
use std::io::{self, Read, Write};

const WIN: i32 = 1_000_000_000;
const ROOT_BEAM: usize = 24;
const NODE_BEAM: usize = 16;
const WORK_LIMIT: i32 = 20_000;
const DIRS: [(i32, i32); 8] = [
    (0, -1), (1, -1), (1, 0), (1, 1),
    (0, 1), (-1, 1), (-1, 0), (-1, -1),
];

#[derive(Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Debug)]
struct Move {
    px: i32,
    py: i32,
    ax: i32,
    ay: i32,
    dx: i32,
    dy: i32,
}

type Cells = [i8; 64];

#[inline]
fn idx(x: i32, y: i32) -> usize {
    (y * 8 + x) as usize
}

fn board_of(x: i32, y: i32) -> i32 {
    2 * (y / 4) + x / 4
}

fn is_dark(b: i32) -> bool {
    b % 2 == 0
}

fn is_home(b: i32, color: i32) -> bool {
    if color == 0 { b >= 2 } else { b < 2 }
}

fn same_board(x: i32, y: i32, ux: i32, uy: i32, k: i32) -> bool {
    let nx = x + ux * k;
    let ny = y + uy * k;
    nx >= 0 && nx < 8 && ny >= 0 && ny < 8 && nx / 4 == x / 4 && ny / 4 == y / 4
}

fn gen_moves(c: &Cells, me: i32) -> Vec<Move> {
    let mut out = Vec::with_capacity(256);
    let op = 1 - me;
    let mei = me as i8;
    let opi = op as i8;
    for py in 0..8 {
        for px in 0..8 {
            if c[idx(px, py)] != mei || !is_home(board_of(px, py), me) {
                continue;
            }
            let passive_dark = is_dark(board_of(px, py));
            for &(ux, uy) in &DIRS {
                for k in 1..=2 {
                    if !same_board(px, py, ux, uy, k)
                        || c[idx(px + ux * k, py + uy * k)] != -1
                    {
                        break;
                    }
                    for ay in 0..8 {
                        for ax in 0..8 {
                            if c[idx(ax, ay)] != mei
                                || is_dark(board_of(ax, ay)) == passive_dark
                            {
                                continue;
                            }
                            if !same_board(ax, ay, ux, uy, k) {
                                continue;
                            }
                            let mut n = 0;
                            let mut ok = true;
                            for j in 1..=k {
                                let v = c[idx(ax + ux * j, ay + uy * j)];
                                if v == mei {
                                    ok = false;
                                    break;
                                }
                                if v == opi {
                                    n += 1;
                                }
                            }
                            if !ok || n > 1 {
                                continue;
                            }
                            if n == 1 && same_board(ax, ay, ux, uy, k + 1) {
                                if c[idx(ax + ux * (k + 1), ay + uy * (k + 1))] != -1 {
                                    continue;
                                }
                            }
                            out.push(Move {
                                px,
                                py,
                                ax,
                                ay,
                                dx: ux * k,
                                dy: uy * k,
                            });
                        }
                    }
                }
            }
        }
    }
    out
}

struct Step {
    cells: Cells,
    win: bool,
}

fn apply_step(cells: &Cells, me: i32, m: Move) -> Step {
    let mut g = *cells;
    let op = 1 - me;
    let mei = me as i8;
    let opi = op as i8;
    let k = m.dx.abs().max(m.dy.abs());
    let ux = m.dx.signum();
    let uy = m.dy.signum();
    g[idx(m.px, m.py)] = -1;
    g[idx(m.px + m.dx, m.py + m.dy)] = mei;
    for j in 1..=k {
        let x = m.ax + ux * j;
        let y = m.ay + uy * j;
        if g[idx(x, y)] == opi {
            g[idx(x, y)] = -1;
            let tx = m.ax + ux * (k + 1);
            let ty = m.ay + uy * (k + 1);
            if same_board(m.ax, m.ay, ux, uy, k + 1) {
                g[idx(tx, ty)] = opi;
            }
            break;
        }
    }
    g[idx(m.ax, m.ay)] = -1;
    g[idx(m.ax + m.dx, m.ay + m.dy)] = mei;
    let mut win = false;
    for b in 0..4 {
        let bx = (b % 2) * 4;
        let by = (b / 2) * 4;
        let mut n = 0;
        for y in by..by + 4 {
            for x in bx..bx + 4 {
                if g[idx(x, y)] == opi {
                    n += 1;
                }
            }
        }
        if n == 0 {
            win = true;
            break;
        }
    }
    Step { cells: g, win }
}

fn stones(g: &Cells, b: i32, color: i32) -> i32 {
    let bx = (b % 2) * 4;
    let by = (b / 2) * 4;
    let color = color as i8;
    let mut n = 0;
    for y in by..by + 4 {
        for x in bx..bx + 4 {
            if g[idx(x, y)] == color {
                n += 1;
            }
        }
    }
    n
}

fn evaluate(g: &Cells, me: i32) -> i32 {
    let op = 1 - me;
    let mut om = [0i32; 4];
    let mut mm = [0i32; 4];
    let mut mn_o = 99;
    let mut mn_m = 99;
    for b in 0..4 {
        om[b] = stones(g, b as i32, op);
        mm[b] = stones(g, b as i32, me);
        mn_o = mn_o.min(om[b]);
        mn_m = mn_m.min(mm[b]);
    }
    if mn_o == 0 {
        return WIN;
    }
    if mn_m == 0 {
        return -WIN;
    }
    let sm: i32 = mm.iter().sum();
    let so: i32 = om.iter().sum();
    let mut score = 35 * (sm - so);
    score += 420 * (4 - mn_o) * (4 - mn_o);
    score -= 320 * (4 - mn_m) * (4 - mn_m);
    for b in 0..4 {
        score += 18 * (mm[b] - om[b]);
    }
    let mei = me as i8;
    let mut our_edge = 0;
    let mut op_edge = 0;
    let mut our_corner = 0;
    let mut op_corner = 0;
    for y in 0..8 {
        for x in 0..8 {
            let c = g[idx(x, y)];
            if c == -1 {
                continue;
            }
            let lx = x % 4;
            let ly = y % 4;
            let edge = lx == 0 || lx == 3 || ly == 0 || ly == 3;
            let corner = (lx == 0 || lx == 3) && (ly == 0 || ly == 3);
            if c == mei {
                our_edge += edge as i32;
                our_corner += corner as i32;
            } else {
                op_edge += edge as i32;
                op_corner += corner as i32;
            }
        }
    }
    score += 30 * (op_edge - our_edge) + 45 * (op_corner - our_corner);
    score
}

struct ScoredMove {
    score: i32,
    mv: Move,
    cells: Cells,
    win: bool,
}

fn ordered_moves(
    g: &Cells,
    me: i32,
    beam: usize,
    state: &mut SearchState,
) -> Vec<ScoredMove> {
    let moves = gen_moves(g, me);
    let mut out = Vec::with_capacity(moves.len());
    for m in moves {
        if state.work >= WORK_LIMIT {
            state.aborted = true;
            break;
        }
        state.work += 1;
        let st = apply_step(g, me, m);
        let s = if st.win { WIN } else { evaluate(&st.cells, me) };
        out.push(ScoredMove {
            score: s,
            mv: m,
            cells: st.cells,
            win: st.win,
        });
    }
    out.sort_by(|a, b| {
        if a.score != b.score {
            b.score.cmp(&a.score)
        } else {
            a.mv.cmp(&b.mv)
        }
    });
    out.truncate(beam);
    out
}

struct SearchState {
    work: i32,
    aborted: bool,
}

fn search(
    g: &Cells,
    me: i32,
    depth: i32,
    mut alpha: i32,
    beta: i32,
    state: &mut SearchState,
) -> i32 {
    if state.aborted {
        return evaluate(g, me);
    }
    if depth <= 0 {
        return evaluate(g, me);
    }
    let moves = ordered_moves(g, me, NODE_BEAM, state);
    if moves.is_empty() {
        return -WIN;
    }
    let mut best = -2 * WIN;
    for sm in &moves {
        let val = if sm.win {
            WIN - depth
        } else {
            -search(
                &sm.cells,
                1 - me,
                depth - 1,
                -beta,
                -alpha,
                state,
            )
        };
        if val > best {
            best = val;
        }
        if best > alpha {
            alpha = best;
        }
        if alpha >= beta || state.aborted {
            break;
        }
    }
    best
}

fn choose(g: &Cells, me: i32) -> Move {
    let all = gen_moves(g, me);
    if all.is_empty() {
        return Move {
            px: 0,
            py: 0,
            ax: 0,
            ay: 0,
            dx: 0,
            dy: 0,
        };
    }
    let mut best = all[0];
    let mut best_val = -2 * WIN;
    let mut state = SearchState {
        work: 0,
        aborted: false,
    };
    for depth in 1..=4 {
        let moves = ordered_moves(g, me, ROOT_BEAM, &mut state);
        let mut local_best = -2 * WIN;
        let mut local_move = best;
        let mut aborted = state.aborted;
        for sm in &moves {
            if state.aborted {
                aborted = true;
                break;
            }
            let beta = if local_best == -2 * WIN {
                2 * WIN
            } else {
                -local_best
            };
            let val = if sm.win {
                WIN - depth
            } else {
                -search(
                    &sm.cells,
                    1 - me,
                    depth - 1,
                    -2 * WIN,
                    beta,
                    &mut state,
                )
            };
            if val > local_best {
                local_best = val;
                local_move = sm.mv;
            }
        }
        if !aborted {
            best = local_move;
            best_val = local_best;
        }
        if best_val >= WIN / 2 || aborted {
            break;
        }
    }
    if all.contains(&best) {
        best
    } else {
        all[0]
    }
}

// Équivalent de std::stoi pour des jetons bien formés.
fn stoi(s: &str) -> i32 {
    let bytes = s.as_bytes();
    let mut i = 0;
    while i < bytes.len() && bytes[i].is_ascii_whitespace() {
        i += 1;
    }
    let neg = bytes.get(i) == Some(&b'-');
    if neg || bytes.get(i) == Some(&b'+') {
        i += 1;
    }
    let start = i;
    let mut value: i64 = 0;
    while i < bytes.len() && bytes[i].is_ascii_digit() {
        value = value * 10 + (bytes[i] - b'0') as i64;
        i += 1;
    }
    if i == start {
        return 0;
    }
    if neg {
        (-value) as i32
    } else {
        value as i32
    }
}

fn main() {
    let mut input = String::new();
    io::stdin().read_to_string(&mut input).unwrap();
    let mut tokens = input.split_whitespace();
    let stdout = io::stdout();
    let mut out = stdout.lock();

    while let Some(round_token) = tokens.next() {
        let _round_no = stoi(round_token);
        let my_color = match tokens.next() {
            Some(t) => stoi(t),
            None => break,
        };
        let mut cells: Cells = [-1; 64];
        for y in 0..8 {
            let row = match tokens.next() {
                Some(t) => t,
                None => break,
            };
            let bytes = row.as_bytes();
            for x in 0..8 {
                cells[idx(x, y)] = match bytes.get(x as usize) {
                    Some(b'b') => 0,
                    Some(b'w') => 1,
                    _ => -1,
                };
            }
        }
        let last = match tokens.next() {
            Some(t) => t,
            None => break,
        };
        if last != "-" && last != "SWAP" {
            for _ in 0..5 {
                tokens.next();
            }
        }
        let n = match tokens.next() {
            Some(t) => stoi(t),
            None => break,
        };
        let mut actions: Vec<Move> = Vec::new();
        let mut can_swap = false;
        for _ in 0..n {
            let a = match tokens.next() {
                Some(t) => t,
                None => break,
            };
            if a == "SWAP" {
                can_swap = true;
                continue;
            }
            let px = stoi(a);
            let py = stoi(tokens.next().unwrap_or(""));
            let ax = stoi(tokens.next().unwrap_or(""));
            let ay = stoi(tokens.next().unwrap_or(""));
            let dx = stoi(tokens.next().unwrap_or(""));
            let dy = stoi(tokens.next().unwrap_or(""));
            actions.push(Move {
                px,
                py,
                ax,
                ay,
                dx,
                dy,
            });
        }
        if can_swap {
            writeln!(out, "SWAP").ok();
            out.flush().ok();
            continue;
        }
        let mut m = choose(&cells, my_color);
        if !actions.contains(&m) {
            if let Some(&first) = actions.first() {
                m = first;
            }
        }
        writeln!(
            out,
            "{} {} {} {} {} {}",
            m.px, m.py, m.ax, m.ay, m.dx, m.dy
        )
        .ok();
        out.flush().ok();
    }
}
