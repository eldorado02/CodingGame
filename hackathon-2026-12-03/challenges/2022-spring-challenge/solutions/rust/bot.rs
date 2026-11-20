// Spring Challenge 2022 (Wood 4) — défense de base par 3 héros.
use std::io::{self, BufRead, Write};

const MAP_W: f64 = 17630.0;
const MAP_H: f64 = 9000.0;

#[derive(Clone, Copy)]
struct Monster {
    x: i32,
    y: i32,
    nx: i32,
    ny: i32,
    hp: i32,
}

#[derive(Clone, Copy)]
struct Hero {
    x: i32,
    y: i32,
}

fn dist(ax: i32, ay: i32, bx: i32, by: i32) -> f64 {
    let dx = (ax - bx) as f64;
    let dy = (ay - by) as f64;
    (dx * dx + dy * dy).sqrt()
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();
    let stdout = io::stdout();
    let mut out = stdout.lock();

    let head = lines.next().unwrap()?;
    let mut h = head.split_whitespace();
    let bx: i32 = h.next().unwrap().parse()?;
    let by: i32 = h.next().unwrap().parse()?;
    let nh: usize = lines.next().unwrap()?.parse()?;

    loop {
        let line = match lines.next() {
            Some(l) => l?,
            None => break,
        };
        if line.trim().is_empty() {
            break;
        }
        let _mine = line;
        lines.next(); // adversaire
        let count: usize = lines.next().unwrap()?.parse()?;
        let mut monsters = Vec::new();
        let mut heroes = Vec::new();

        for _ in 0..count {
            let l = lines.next().unwrap()?;
            let f: Vec<&str> = l.split_whitespace().collect();
            if f.len() < 11 {
                continue;
            }
            let etype: i32 = f[1].parse()?;
            let x: i32 = f[2].parse()?;
            let y: i32 = f[3].parse()?;
            let hp: i32 = f[6].parse()?;
            let vx: i32 = f[7].parse()?;
            let vy: i32 = f[8].parse()?;
            let near: i32 = f[9].parse()?;
            let threat: i32 = f[10].parse()?;

            if etype == 0 {
                let (nx, ny) = if near == 1 {
                    let (px, py) = if threat == 1 {
                        (bx as f64, by as f64)
                    } else {
                        (MAP_W - bx as f64, MAP_H - by as f64)
                    };
                    let dx = px - x as f64;
                    let dy = py - y as f64;
                    let d = dist(x, y, px as i32, py as i32).max(1.0);
                    (
                        x + (400.0 * dx / d) as i32,
                        y + (400.0 * dy / d) as i32,
                    )
                } else {
                    (x + vx, y + vy)
                };
                if threat == 1 {
                    monsters.push(Monster { x, y, nx, ny, hp });
                }
            } else if etype == 1 {
                heroes.push(Hero { x, y });
            }
        }

        monsters.sort_by(|a, b| {
            dist(a.nx, a.ny, bx, by)
                .partial_cmp(&dist(b.nx, b.ny, bx, by))
                .unwrap()
                .then(a.hp.cmp(&b.hp))
                .then(a.x.cmp(&b.x))
                .then(a.y.cmp(&b.y))
        });

        let mut used = vec![false; monsters.len()];
        for hero in &heroes {
            let mut best: Option<(usize, f64)> = None;
            for (i, m) in monsters.iter().enumerate() {
                if used[i] {
                    continue;
                }
                let d = dist(hero.x, hero.y, m.nx, m.ny);
                if best.map_or(true, |(_, bd)| d < bd) {
                    best = Some((i, d));
                }
            }
            match best {
                None => writeln!(out, "WAIT")?,
                Some((i, d)) => {
                    used[i] = true;
                    let (mut tx, mut ty) = (monsters[i].nx, monsters[i].ny);
                    if d <= 800.0 {
                        tx = hero.x;
                        ty = hero.y;
                    }
                    writeln!(out, "MOVE {} {}", tx, ty)?;
                }
            }
        }
        for _ in heroes.len()..nh {
            writeln!(out, "WAIT")?;
        }
        out.flush()?;
    }
    Ok(())
}
