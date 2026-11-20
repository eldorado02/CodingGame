// A*Craft — flèches parcimonieuses, un seul tour de sortie.
use std::io::{self, BufRead, Write};

const W: usize = 19;
const H: usize = 10;
const LIMIT: usize = 4 * W * H;
const DX: [i32; 4] = [0, 1, 0, -1];
const DY: [i32; 4] = [-1, 0, 1, 0];
const CH: [u8; 4] = [b'U', b'R', b'D', b'L'];

fn wrap(v: i32, m: i32) -> usize {
    let mut v = v;
    if v < 0 { v += m; }
    if v >= m { v -= m; }
    v as usize
}

#[derive(Clone, Copy)]
struct Robot { x: usize, y: usize, d: usize }
type Placement = (usize, usize, usize);

fn build(
    board: &[[i32; W]; H],
    isvoid: &[[bool; W]; H],
    isstart: &[[bool; W]; H],
    robots: &[Robot],
    order: &[usize],
) -> (i32, Vec<Placement>) {
    let mut b = *board;
    let mut vis = [[0u8; 4]; W * H];
    let mut out = Vec::new();
    let mut total = 0;
    for &ri in order {
        let r = robots[ri];
        let (mut x, mut y, mut d) = (r.x, r.y, r.d);
        vis[y * W + x][d] = 1;
        for _ in 0..LIMIT {
            let nx = wrap(x as i32 + DX[d], W as i32);
            let ny = wrap(y as i32 + DY[d], H as i32);
            if isvoid[ny][nx] { break; }
            let nd;
            if b[ny][nx] != -1 {
                nd = b[ny][nx] as usize;
            } else if isstart[ny][nx] {
                nd = d;
            } else {
                let mut best_look = i32::MAX;
                let mut best_d = d;
                let mut found = false;
                for d2 in 0..4 {
                    if vis[ny * W + nx][d2] != 0 { continue; }
                    let tx = wrap(nx as i32 + DX[d2], W as i32);
                    let ty = wrap(ny as i32 + DY[d2], H as i32);
                    let look = if isvoid[ty][tx] {
                        99
                    } else {
                        vis[ty * W + tx].iter().map(|&v| i32::from(v)).sum()
                    };
                    if !found || look < best_look {
                        best_look = look;
                        best_d = d2;
                        found = true;
                    }
                }
                if !found { break; }
                nd = best_d;
                if nd != d {
                    b[ny][nx] = nd as i32;
                    out.push((nx, ny, nd));
                }
            }
            if vis[ny * W + nx][nd] != 0 { break; }
            vis[ny * W + nx][nd] = 1;
            total += 1;
            x = nx; y = ny; d = nd;
        }
    }
    (total, out)
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();
    let mut next = || -> Result<Option<String>, Box<dyn std::error::Error>> {
        match lines.next() {
            Some(line) => Ok(Some(line?)),
            None => Ok(None),
        }
    };

    let mut board = [[-1i32; W]; H];
    let mut isvoid = [[false; W]; H];
    let mut isstart = [[false; W]; H];
    for y in 0..H {
        let line = match next()? {
            Some(line) => line,
            None => return Ok(()),
        };
        let bytes = line.as_bytes();
        for x in 0..W {
            let c = bytes[x].to_ascii_uppercase();
            if c == b'#' { isvoid[y][x] = true; }
            for (d, ch) in CH.iter().enumerate() {
                if c == *ch { board[y][x] = d as i32; }
            }
        }
    }
    let rc: usize = match next()? {
        Some(line) => line.trim().parse()?,
        None => return Ok(()),
    };
    let mut robots: Vec<Robot> = Vec::with_capacity(rc);
    for _ in 0..rc {
        let line = match next()? {
            Some(line) => line,
            None => return Ok(()),
        };
        let f: Vec<&str> = line.split_whitespace().collect();
        let x: usize = f[0].parse()?;
        let y: usize = f[1].parse()?;
        let c = f[2].as_bytes()[0].to_ascii_uppercase();
        let d = CH.iter().position(|&ch| ch == c).unwrap_or(0);
        robots.push(Robot { x, y, d });
        isstart[y][x] = true;
    }

    let mut initial: Vec<Placement> = Vec::new();
    for r in &mut robots {
        let nx = wrap(r.x as i32 + DX[r.d], W as i32);
        let ny = wrap(r.y as i32 + DY[r.d], H as i32);
        if !isvoid[ny][nx] { continue; }
        for d2 in 0..4 {
            let tx = wrap(r.x as i32 + DX[d2], W as i32);
            let ty = wrap(r.y as i32 + DY[d2], H as i32);
            if !isvoid[ty][tx] {
                board[r.y][r.x] = d2 as i32;
                r.d = d2;
                initial.push((r.x, r.y, d2));
                break;
            }
        }
    }

    let order: Vec<usize> = (0..robots.len()).collect();
    let (s1, p1) = build(&board, &isvoid, &isstart, &robots, &order);
    let reversed: Vec<usize> = order.iter().rev().copied().collect();
    let (s2, p2) = build(&board, &isvoid, &isstart, &robots, &reversed);

    let mut out = String::new();
    let mut first = true;
    for (x, y, d) in initial.iter().chain(if s2 > s1 { p2.iter() } else { p1.iter() }) {
        if !first { out.push(' '); }
        out.push_str(&format!("{} {} {}", x, y, CH[*d] as char));
        first = false;
    }
    out.push('\n');
    let stdout = io::stdout();
    let mut handle = stdout.lock();
    handle.write_all(out.as_bytes())?;
    handle.flush()?;
    Ok(())
}
