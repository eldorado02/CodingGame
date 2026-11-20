// Spring Challenge 2021 (Wood 4) — compléter les meilleurs arbres au plus vite.
use std::io::{self, BufRead, Write};

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();
    let stdout = io::stdout();
    let mut out = stdout.lock();

    let n: usize = lines.next().unwrap()?.parse()?;
    let mut richness = vec![0i32; 64];
    for _ in 0..n {
        let l = lines.next().unwrap()?;
        let f: Vec<&str> = l.split_whitespace().collect();
        if f.len() >= 2 {
            let idx: usize = f[0].parse()?;
            if idx < richness.len() { richness[idx] = f[1].parse()?; }
        }
    }

    loop {
        let line = match lines.next() { Some(l) => l?, None => break };
        if line.trim().is_empty() { break; }
        lines.next(); // nutrients
        let sunline = lines.next().unwrap()?;
        let f: Vec<&str> = sunline.split_whitespace().collect();
        if f.len() < 2 { break; }
        let my_sun: i32 = f[0].parse()?;
        lines.next(); // adversaire
        let nt: usize = lines.next().unwrap()?.parse()?;
        let mut best: Option<(i32, i32)> = None; // (-richness, idx)
        for _ in 0..nt {
            let l = lines.next().unwrap()?;
            let t: Vec<&str> = l.split_whitespace().collect();
            if t.len() < 4 { continue; }
            let idx: i32 = t[0].parse()?;
            let size: i32 = t[1].parse()?;
            let mine: i32 = t[2].parse()?;
            if mine == 1 && size == 3 {
                let rch = richness.get(idx as usize).copied().unwrap_or(0);
                let key = (-rch, idx);
                if best.map_or(true, |b| key < b) { best = Some(key); }
            }
        }
        let na: usize = lines.next().unwrap()?.parse()?;
        for _ in 0..na { lines.next(); }
        if my_sun >= 4 {
            if let Some((_, idx)) = best {
                writeln!(out, "COMPLETE {}", idx)?;
            } else {
                writeln!(out, "WAIT")?;
            }
        } else {
            writeln!(out, "WAIT")?;
        }
        out.flush()?;
    }
    Ok(())
}
