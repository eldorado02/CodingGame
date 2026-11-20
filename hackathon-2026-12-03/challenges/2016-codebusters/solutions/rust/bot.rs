// CodeBusters — stratégie Wood : un fantôme par buster, bande de capture.
use std::io::{self, BufRead, Write};

const W: i64 = 16_000;
const H: i64 = 9_000;
const STEP: i64 = 800;
const BUST_MIN2: i64 = 900 * 900;
const BUST_MAX2: i64 = 1760 * 1760;

#[derive(Clone, Copy)]
struct Entity {
    id: i32,
    x: i64,
    y: i64,
    state: i32,
}

fn dist2(ax: i64, ay: i64, bx: i64, by: i64) -> i64 {
    let dx = ax - bx;
    let dy = ay - by;
    dx * dx + dy * dy
}

fn step_toward(x: i64, y: i64, tx: i64, ty: i64) -> (i64, i64) {
    let dx = tx - x;
    let dy = ty - y;
    let d2 = dx * dx + dy * dy;
    if d2 == 0 {
        return (x, y);
    }
    let d = (d2 as f64).sqrt();
    let nx = (x as f64 + STEP as f64 * dx as f64 / d + 0.5).floor() as i64;
    let ny = (y as f64 + STEP as f64 * dy as f64 / d + 0.5).floor() as i64;
    (nx.clamp(0, W), ny.clamp(0, H))
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

    let read_int = |line: Option<String>| -> Result<Option<i32>, Box<dyn std::error::Error>> {
        match line {
            Some(line) => Ok(Some(line.trim().parse()?)),
            None => Ok(None),
        }
    };
    if read_int(next()?)?.is_none() { return Ok(()); } // bustersPerPlayer
    if read_int(next()?)?.is_none() { return Ok(()); } // ghostCount
    let team = match read_int(next()?)? {
        Some(team) => team,
        None => return Ok(()),
    };
    let (home_x, home_y) = if team == 0 { (0, 0) } else { (W, H) };

    let stdout = io::stdout();
    let mut out = stdout.lock();

    loop {
        let n: usize = match next()? {
            Some(line) => line.trim().parse()?,
            None => return Ok(()),
        };
        let mut mine = Vec::new();
        let mut ghosts: Vec<Entity> = Vec::new();
        for _ in 0..n {
            let line = match next()? {
                Some(line) => line,
                None => return Ok(()),
            };
            let v: Vec<i32> = line
                .split_whitespace()
                .map(|x| x.parse::<i32>())
                .collect::<Result<_, _>>()?;
            let e = Entity {
                id: v[0],
                x: v[1] as i64,
                y: v[2] as i64,
                state: v[4],
            };
            if v[3] == team {
                mine.push(e);
            } else if v[3] == -1 {
                ghosts.push(e);
            }
        }

        let mut reserved = vec![false; ghosts.len()];
        for b in &mine {
            let order = if b.state == 1 {
                if dist2(b.x, b.y, home_x, home_y) <= STEP * STEP {
                    "RELEASE".to_string()
                } else {
                    let (nx, ny) = step_toward(b.x, b.y, home_x, home_y);
                    format!("MOVE {nx} {ny}")
                }
            } else {
                let mut best: Option<(usize, i64)> = None;
                for (g, ghost) in ghosts.iter().enumerate() {
                    if reserved[g] {
                        continue;
                    }
                    let d2 = dist2(b.x, b.y, ghost.x, ghost.y);
                    if best.is_none_or(|(_, bd)| d2 < bd) {
                        best = Some((g, d2));
                    }
                }
                match best {
                    None => {
                        let (nx, ny) = step_toward(b.x, b.y, W / 2, H / 2);
                        format!("MOVE {nx} {ny}")
                    }
                    Some((g, d2)) => {
                        if (BUST_MIN2..=BUST_MAX2).contains(&d2) {
                            reserved[g] = true;
                            format!("BUST {}", ghosts[g].id)
                        } else {
                            let (tx, ty) = if d2 < BUST_MIN2 {
                                (2 * b.x - ghosts[g].x, 2 * b.y - ghosts[g].y)
                            } else {
                                (ghosts[g].x, ghosts[g].y)
                            };
                            let (nx, ny) = step_toward(b.x, b.y, tx, ty);
                            format!("MOVE {nx} {ny}")
                        }
                    }
                }
            };
            writeln!(out, "{order}")?;
        }
        out.flush()?;
    }
}
