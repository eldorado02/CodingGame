// Mean Max Wood 3 — simulation 1 pas des actions, reste dans les épaves.
use std::io::{self, BufRead, Write};

struct Wreck {
    x: f64,
    y: f64,
    r: f64,
    water: f64,
}

struct Candidate {
    target: Option<(f64, f64)>,
    power: f64,
}

fn round_away(z: f64) -> i32 {
    if z >= 0.0 {
        (z + 0.5).floor() as i32
    } else {
        -((-z + 0.5).floor() as i32)
    }
}

fn simulate(x: f64, y: f64, vx: f64, vy: f64, c: &Candidate) -> (i32, i32, i32, i32) {
    let (mut ax, mut ay) = (0.0, 0.0);
    if let (Some((tx, ty)), true) = (c.target, c.power > 0.0) {
        let (dx, dy) = (tx - x, ty - y);
        let dist = (dx * dx + dy * dy).sqrt();
        if dist > 1e-9 {
            let coef = 2.0 * c.power / dist;
            ax = dx * coef;
            ay = dy * coef;
        }
    }
    let (sfx, sfy) = (vx + ax, vy + ay);
    (
        round_away(x + sfx),
        round_away(y + sfy),
        round_away(sfx * 0.8),
        round_away(sfy * 0.8),
    )
}

fn evaluate(nx: i32, ny: i32, nvx: i32, nvy: i32, wrecks: &[Wreck]) -> f64 {
    let (mut inside, mut water) = (0.0_f64, 0.0_f64);
    let mut closest = 1e18_f64;
    for w in wrecks {
        let dx = nx as f64 - w.x;
        let dy = ny as f64 - w.y;
        let d = (dx * dx + dy * dy).sqrt();
        if d <= w.r {
            inside += 1.0;
            water += w.water;
        }
        closest = closest.min(d);
    }
    let speed = ((nvx * nvx) as f64 + (nvy * nvy) as f64).sqrt();
    inside * 1_000_000.0 + water * 10.0 - closest - speed
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
    let stdout = io::stdout();
    let mut out = stdout.lock();

    loop {
        // 3 scores + 3 lignes de rages (placeholders).
        for _ in 0..6 {
            if next()?.is_none() {
                return Ok(());
            }
        }
        let n: usize = match next()? {
            Some(line) => line.trim().parse()?,
            None => return Ok(()),
        };
        let mut reaper: Option<(f64, f64, f64, f64)> = None;
        let mut wrecks: Vec<Wreck> = Vec::new();
        for _ in 0..n {
            let line = match next()? {
                Some(line) => line,
                None => return Ok(()),
            };
            let p: Vec<&str> = line.split_whitespace().collect();
            let kind: i32 = p[1].parse()?;
            let player: i32 = p[2].parse()?;
            let radius: f64 = p[4].parse()?;
            let x: f64 = p[5].parse()?;
            let y: f64 = p[6].parse()?;
            let vx: f64 = p[7].parse()?;
            let vy: f64 = p[8].parse()?;
            let extra: f64 = p[9].parse()?;
            if kind == 0 && player == 0 {
                reaper = Some((x, y, vx, vy));
            } else if kind == 4 {
                wrecks.push(Wreck { x, y, r: radius, water: extra });
            }
        }
        let (rx, ry, rvx, rvy) = match reaper {
            Some(v) => v,
            None => {
                writeln!(out, "WAIT\nWAIT\nWAIT")?;
                out.flush()?;
                continue;
            }
        };

        let mut cands = vec![Candidate { target: None, power: 0.0 }];
        for w in &wrecks {
            for p in [0.0, 100.0, 200.0, 300.0] {
                cands.push(Candidate { target: Some((w.x, w.y)), power: p });
            }
        }
        if rvx != 0.0 || rvy != 0.0 {
            for p in [150.0, 300.0] {
                cands.push(Candidate { target: Some((rx - rvx, ry - rvy)), power: p });
            }
        }

        let mut best = -1e18_f64;
        let mut best_target: Option<(f64, f64)> = None;
        let mut best_power = 0.0;
        for c in &cands {
            let (nx, ny, nvx, nvy) = simulate(rx, ry, rvx, rvy, c);
            let s = evaluate(nx, ny, nvx, nvy, &wrecks);
            if s > best {
                best = s;
                best_target = c.target;
                best_power = c.power;
            }
        }
        match (best_target, best_power) {
            (Some((tx, ty)), p) if p > 0.0 => {
                writeln!(out, "{} {} {}", tx as i32, ty as i32, p as i32)?;
            }
            _ => writeln!(out, "WAIT")?,
        }
        writeln!(out, "WAIT\nWAIT")?;
        out.flush()?;
    }
}
