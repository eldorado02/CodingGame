// Fall Challenge 2020 (Wood 4) — brasser la recette la plus rentable.
use std::io::{self, BufRead, Write};

struct Order {
    id: i32,
    need: [i32; 4],
    price: i32,
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();
    let stdout = io::stdout();
    let mut out = stdout.lock();

    loop {
        let line = match lines.next() { Some(l) => l?, None => break };
        let line = line.trim();
        if line.is_empty() { break; }
        let n: usize = line.parse()?;
        let mut orders = Vec::new();
        for _ in 0..n {
            let l = lines.next().unwrap()?;
            let f: Vec<&str> = l.split_whitespace().collect();
            if f.len() < 7 || f[1] != "BREW" { continue; }
            let mut need = [0i32; 4];
            for j in 0..4 { need[j] = -f[2 + j].parse::<i32>()?; }
            orders.push(Order {
                id: f[0].parse()?,
                need,
                price: f[6].parse()?,
            });
        }
        let mine = lines.next().unwrap()?;
        let f: Vec<&str> = mine.split_whitespace().collect();
        if f.len() < 5 { break; }
        let mut inv = [0i32; 4];
        for j in 0..4 { inv[j] = f[j].parse()?; }
        lines.next(); // adversaire

        let mut best: Option<(i32, i32, i32)> = None; // (-price, sum, id)
        for o in &orders {
            let mut sum = 0;
            let mut ok = true;
            for j in 0..4 {
                if o.need[j] > inv[j] { ok = false; break; }
                sum += o.need[j];
            }
            if !ok { continue; }
            let key = (-o.price, sum, o.id);
            if best.map_or(true, |b| key < b) { best = Some(key); }
        }
        match best {
            Some((_, _, id)) => writeln!(out, "BREW {}", id)?,
            None => writeln!(out, "WAIT")?,
        }
        out.flush()?;
    }
    Ok(())
}
