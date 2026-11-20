// Fantastic Bits Wood 2 — attaquant + défenseur, THROW au but après grab.
use std::io::{self, BufRead, Write};

const W: i64 = 16_000;
const H: i64 = 7_500;
const GOAL_Y: i64 = 3_750;

#[derive(Clone, Copy)]
struct Entity {
    x: i64,
    y: i64,
    vx: i64,
    vy: i64,
    state: i32,
}

fn d2(ax: i64, ay: i64, bx: i64, by: i64) -> i64 {
    let dx = ax - bx;
    let dy = ay - by;
    dx * dx + dy * dy
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
    let team: i32 = match next()? {
        Some(line) => line.trim().parse()?,
        None => return Ok(()),
    };
    let goal_x = if team == 0 { W } else { 0 };
    let own_x = if team == 0 { 0 } else { W };

    let stdout = io::stdout();
    let mut out = stdout.lock();
    loop {
        // Scores sur deux lignes, puis le nombre d'entités sur une troisième.
        for _ in 0..2 {
            if next()?.is_none() {
                return Ok(());
            }
        }
        let n: usize = match next()? {
            Some(line) => line.trim().parse()?,
            None => return Ok(()),
        };

        let mut wizards = Vec::new();
        let mut snaffles: Vec<Entity> = Vec::new();
        for _ in 0..n {
            let line = match next()? {
                Some(line) => line,
                None => return Ok(()),
            };
            let p: Vec<&str> = line.split_whitespace().collect();
            let e = Entity {
                x: p[2].parse()?,
                y: p[3].parse()?,
                vx: p[4].parse()?,
                vy: p[5].parse()?,
                state: p[6].parse()?,
            };
            match p[1] {
                "WIZARD" => wizards.push(e),
                "SNAFFLE" => snaffles.push(e),
                _ => {}
            }
        }

        let mut chosen = vec![false; snaffles.len()];
        for (i, w) in wizards.iter().enumerate() {
            if w.state == 1 {
                writeln!(out, "THROW {goal_x} {GOAL_Y} 500")?;
                continue;
            }
            let mut best: Option<(usize, i64)> = None;
            if i == 0 {
                for (s, g) in snaffles.iter().enumerate() {
                    if chosen[s] {
                        continue;
                    }
                    let dd = d2(w.x, w.y, g.x, g.y);
                    if best.is_none_or(|(_, bd)| dd < bd) {
                        best = Some((s, dd));
                    }
                }
            } else {
                for (s, g) in snaffles.iter().enumerate() {
                    if chosen[s] {
                        continue;
                    }
                    let in_own = (g.x < W / 2) == (i64::from(team) == 0);
                    if !in_own {
                        continue;
                    }
                    let dd = d2(own_x, GOAL_Y, g.x, g.y);
                    if best.is_none_or(|(_, bd)| dd < bd) {
                        best = Some((s, dd));
                    }
                }
                if best.is_none() {
                    for (s, g) in snaffles.iter().enumerate() {
                        if chosen[s] {
                            continue;
                        }
                        let dd = d2(w.x, w.y, g.x, g.y);
                        if best.is_none_or(|(_, bd)| dd < bd) {
                            best = Some((s, dd));
                        }
                    }
                }
            }
            match best {
                None => writeln!(out, "MOVE {goal_x} {GOAL_Y} 150")?,
                Some((s, _)) => {
                    chosen[s] = true;
                    let g = snaffles[s];
                    let tx = (g.x + 3 * g.vx).clamp(0, W);
                    let ty = (g.y + 3 * g.vy).clamp(0, H);
                    let thrust = if d2(w.x, w.y, tx, ty) > 1200 * 1200 { 150 } else { 60 };
                    writeln!(out, "MOVE {tx} {ty} {thrust}")?;
                }
            }
        }
        out.flush()?;
    }
}
