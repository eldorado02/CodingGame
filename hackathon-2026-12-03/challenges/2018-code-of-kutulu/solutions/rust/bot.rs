// Code of Kutulu Wood 3 — réassurance prioritaire, fuite des wanderers.
use std::io::{self, BufRead, Write};

const DX: [i32; 5] = [0, 0, 0, -1, 1];
const DY: [i32; 5] = [0, -1, 1, 0, 0];

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();
    let mut next = || -> Result<Option<String>, Box<dyn std::error::Error>> {
        match lines.next() {
            Some(line) => Ok(Some(line?)),
            None => Ok(None),
        }
    };
    let w: usize = match next()? {
        Some(line) => line.trim().parse()?,
        None => return Ok(()),
    };
    let h: usize = match next()? {
        Some(line) => line.trim().parse()?,
        None => return Ok(()),
    };
    let mut grid: Vec<String> = Vec::with_capacity(h);
    for _ in 0..h {
        grid.push(match next()? {
            Some(line) => line,
            None => return Ok(()),
        });
    }
    next()?;  // 4 entiers sans signification

    let stdout = io::stdout();
    let mut out = stdout.lock();
    loop {
        let ec: usize = match next()? {
            Some(line) => line.trim().parse()?,
            None => return Ok(()),
        };
        let (mut mx, mut my) = (0, 0);
        let mut others: Vec<(i32, i32)> = Vec::new();
        let mut wanderers: Vec<(i32, i32)> = Vec::new();
        for i in 0..ec {
            let p = match next()? {
                Some(line) => line,
                None => return Ok(()),
            };
            let f: Vec<&str> = p.split_whitespace().collect();
            if f.len() < 7 { return Ok(()); }
            let x: i32 = f[2].parse()?;
            let y: i32 = f[3].parse()?;
            let p0: i32 = f[4].parse()?;
            let p1: i32 = f[5].parse()?;
            if i == 0 {
                mx = x; my = y;
            } else if f[0] == "EXPLORER" {
                if p0 > 0 { others.push((x, y)); }
            } else if f[0] == "WANDERER" && p1 == 1 {
                wanderers.push((x, y));
            }
        }

        let mut best: i64 = i64::MIN;
        let (mut bx, mut by) = (mx, my);
        for k in 0..5 {
            let px = mx + DX[k];
            let py = my + DY[k];
            if px < 0 || px >= w as i32 || py < 0 || py >= h as i32 { continue; }
            if grid[py as usize].as_bytes().get(px as usize) == Some(&b'#') { continue; }
            let mut reassured = false;
            let mut de = 100;
            for (ox, oy) in &others {
                let dd = (px - ox).abs() + (py - oy).abs();
                if dd <= 2 { reassured = true; }
                if dd < de { de = dd; }
            }
            let mut dw = 100;
            for (wx, wy) in &wanderers {
                let dd = (px - wx).abs() + (py - wy).abs();
                if dd < dw { dw = dd; }
            }
            let score = (if reassured { 100_000i64 } else { 0 }) + 100 * i64::from(dw) - i64::from(de);
            if score > best { best = score; bx = px; by = py; }
        }
        if bx == mx && by == my {
            writeln!(out, "WAIT")?;
        } else {
            writeln!(out, "MOVE {} {}", bx, by)?;
        }
        out.flush()?;
    }
}
