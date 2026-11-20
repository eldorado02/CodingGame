// Code Royale Wood 3 — barracks KNIGHT, TRAIN en boucle.
use std::io::{self, BufRead, Write};

#[derive(Clone)]
struct Site {
    id: i32,
    x: i32,
    y: i32,
    stype: i32,
    owner: i32,
    p1: i32,
    p2: i32,
}

fn dist(ax: i32, ay: i32, bx: i32, by: i32) -> f64 {
    let dx = f64::from(ax - bx);
    let dy = f64::from(ay - by);
    (dx * dx + dy * dy).sqrt()
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
    let nums = |line: &str| -> Result<Vec<i32>, Box<dyn std::error::Error>> {
        Ok(line
            .split_whitespace()
            .map(|v| v.parse::<i32>())
            .collect::<Result<_, _>>()?)
    };

    let n: usize = match next()? {
        Some(line) => line.trim().parse()?,
        None => return Ok(()),
    };
    let mut sites: Vec<Site> = Vec::with_capacity(n);
    for _ in 0..n {
        let p = nums(&match next()? {
            Some(line) => line,
            None => return Ok(()),
        })?;
        sites.push(Site {
            id: p[0], x: p[1], y: p[2],
            stype: -1, owner: -1, p1: -1, p2: -1,
        });
    }

    let stdout = io::stdout();
    let mut out = stdout.lock();
    loop {
        let header = nums(&match next()? {
            Some(line) => line,
            None => return Ok(()),
        })?;
        let (gold, _touched) = (header[0], header[1]);
        for _ in 0..n {
            let p = nums(&match next()? {
                Some(line) => line,
                None => return Ok(()),
            })?;
            if let Some(s) = sites.iter_mut().find(|s| s.id == p[0]) {
                s.stype = p[3];
                s.owner = p[4];
                s.p1 = p[5];
                s.p2 = p[6];
            }
        }

        let nu: usize = match next()? {
            Some(line) => line.trim().parse()?,
            None => return Ok(()),
        };
        let (mut mx, mut my) = (200, 200);
        let (mut ex, mut ey) = (-1, -1);
        for _ in 0..nu {
            let p = nums(&match next()? {
                Some(line) => line,
                None => return Ok(()),
            })?;
            if p[3] == -1 {
                if p[2] == 0 { mx = p[0]; my = p[1]; }
                else { ex = p[0]; ey = p[1]; }
            }
        }
        if ex < 0 { ex = 1920 - mx; ey = 1000 - my; }

        // 1. Détruire un barracks ennemi proche par contact.
        let destroy = sites
            .iter()
            .filter(|s| s.stype == 2 && s.owner == 1 && dist(mx, my, s.x, s.y) < 250.0)
            .min_by(|a, b| {
                dist(mx, my, a.x, a.y)
                    .partial_cmp(&dist(mx, my, b.x, b.y))
                    .unwrap()
            });
        if let Some(s) = destroy {
            writeln!(out, "MOVE {} {}", s.x, s.y)?;
        } else {
            // 2. Meilleur site libre pour un barracks KNIGHT.
            let mut best: Option<(f64, i32)> = None;
            for s in &sites {
                if s.owner == 1 { continue; }
                if s.stype == 2 && s.p1 > 0 { continue; }
                if s.stype == 2 && s.p2 == 0 { continue; }
                let de = dist(s.x, s.y, ex, ey);
                let dm = dist(s.x, s.y, mx, my);
                let mut score = de + 0.6 * dm;
                if de < 140.0 { score += 10000.0; }
                if best.map_or(true, |(b, _)| score < b) { best = Some((score, s.id)); }
            }
            match best {
                Some((_, sid)) => writeln!(out, "BUILD {} BARRACKS-KNIGHT", sid)?,
                None => writeln!(out, "WAIT")?,
            }
        }

        // 3. Entraîner les barracks KNIGHT prêts et abordables.
        let mut train: Vec<i32> = Vec::new();
        let mut left = gold;
        for s in &sites {
            if s.stype == 2 && s.owner == 0 && s.p1 == 0 && s.p2 == 0 && left >= 80 {
                train.push(s.id);
                left -= 80;
            }
        }
        if train.is_empty() {
            writeln!(out, "TRAIN")?;
        } else {
            let ids: Vec<String> = train.iter().map(|id| id.to_string()).collect();
            writeln!(out, "TRAIN {}", ids.join(" "))?;
        }
        out.flush()?;
    }
}
