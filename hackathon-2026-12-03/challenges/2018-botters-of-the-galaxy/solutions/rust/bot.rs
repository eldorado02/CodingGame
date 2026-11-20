// Botters of the Galaxy Wood 6 — HULK, ATTACK héros puis tour.
use std::io::{self, BufRead, Write};

#[derive(Clone)]
struct Entity {
    id: i32,
    x: i32,
    y: i32,
    kind: String,
}

fn d2(ax: i32, ay: i32, bx: i32, by: i32) -> i64 {
    let dx = i64::from(ax - bx);
    let dy = i64::from(ay - by);
    dx * dx + dy * dy
}

fn best_of<'a>(foes: &'a [Entity], kind: &str, mx: i32, my: i32) -> Option<&'a Entity> {
    foes.iter()
        .filter(|e| e.kind == kind)
        .min_by_key(|e| (d2(mx, my, e.x, e.y), e.id))
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
    let bushes: usize = match next()? {
        Some(line) => line.trim().parse()?,
        None => return Ok(()),
    };
    for _ in 0..bushes {
        next()?;
    }
    let items: usize = match next()? {
        Some(line) => line.trim().parse()?,
        None => return Ok(()),
    };
    for _ in 0..items {
        next()?;
    }

    let stdout = io::stdout();
    let mut out = stdout.lock();
    let (mut mx, mut my) = (0, 0);
    loop {
        // Une ligne par champ : gold, gold adverse, roundType, entityCount.
        let mut header = Vec::with_capacity(4);
        for _ in 0..4 {
            let line = match next()? {
                Some(line) => line,
                None => return Ok(()),
            };
            header.push(line.trim().parse::<i32>()?);
        }
        let (round_type, count) = (header[2], header[3] as usize);
        let mut foes: Vec<Entity> = Vec::new();
        for _ in 0..count {
            let line = match next()? {
                Some(line) => line,
                None => return Ok(()),
            };
            let p: Vec<&str> = line.split_whitespace().collect();
            let id: i32 = p[0].parse()?;
            let eteam: i32 = p[1].parse()?;
            let kind = p[2].to_string();
            let x: i32 = p[3].parse()?;
            let y: i32 = p[4].parse()?;
            let health: i32 = p[6].parse()?;
            if kind == "HERO" && eteam == team {
                mx = x;
                my = y;
            }
            if eteam == team || health <= 0 {
                continue;
            }
            foes.push(Entity { id, x, y, kind });
        }
        // La phase de pick contient aussi des entités : lues ci-dessus.
        if round_type < 0 {
            writeln!(out, "HULK")?;
            out.flush()?;
            continue;
        }
        let target = best_of(&foes, "HERO", mx, my)
            .or_else(|| best_of(&foes, "TOWER", mx, my))
            .or_else(|| {
                foes.iter()
                    .filter(|e| e.kind != "HERO" && e.kind != "TOWER")
                    .min_by_key(|e| (d2(mx, my, e.x, e.y), e.id))
            });
        for _ in 0..round_type {
            match target {
                Some(e) => writeln!(out, "ATTACK {}", e.id)?,
                None => writeln!(out, "WAIT")?,
            }
        }
        out.flush()?;
    }
}
