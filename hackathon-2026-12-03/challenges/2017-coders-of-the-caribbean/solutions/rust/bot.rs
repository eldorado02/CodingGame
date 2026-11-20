// Coders of the Caribbean Wood 3 — viser le baril le plus proche.
use std::io::{self, BufRead, Write};

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
        let _ships = match next()? {
            Some(line) => line,
            None => return Ok(()),
        };
        let n: usize = match next()? {
            Some(line) => line.trim().parse()?,
            None => return Ok(()),
        };
        let mut me: Option<(i64, i64)> = None;
        let mut barrels: Vec<(i64, i64)> = Vec::new();
        for _ in 0..n {
            let line = match next()? {
                Some(line) => line,
                None => return Ok(()),
            };
            let p: Vec<&str> = line.split_whitespace().collect();
            let (x, y): (i64, i64) = (p[2].parse()?, p[3].parse()?);
            if p[1] == "SHIP" && p[7] == "1" {
                me = Some((x, y));
            } else if p[1] == "BARREL" {
                barrels.push((x, y));
            }
        }
        match (me, barrels.iter().min_by_key(|b| {
            let (sx, sy) = me.unwrap();
            (b.0 - sx).pow(2) + (b.1 - sy).pow(2)
        })) {
            (Some(_), Some(&(bx, by))) => writeln!(out, "MOVE {bx} {by}")?,
            _ => writeln!(out, "WAIT")?,
        }
        out.flush()?;
    }
}
