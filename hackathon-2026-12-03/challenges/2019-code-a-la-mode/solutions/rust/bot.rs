// Code a la Mode — plat BLUEBERRIES + ICE_CREAM puis livraison.
use std::io::{self, BufRead, Write};

#[derive(Clone, Copy)]
struct Pt { x: usize, y: usize }

struct Kitchen {
    dish: Option<Pt>,
    window: Option<Pt>,
    berries: Option<Pt>,
    icecream: Option<Pt>,
}

fn contents(s: &str) -> (bool, bool) {
    if !s.starts_with("DISH") { return (false, false); }
    (s.contains("BLUEBERRIES"), s.contains("ICE_CREAM"))
}

fn emit(p: Option<Pt>, out: &mut String) {
    match p {
        Some(q) => out.push_str(&format!("USE {} {}\n", q.x, q.y)),
        None => out.push_str("WAIT\n"),
    }
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines().map(|l| l.unwrap());
    let nall: usize = lines.next().unwrap().trim().parse()?;
    for _ in 0..nall { lines.next(); }
    let mut k = Kitchen { dish: None, window: None, berries: None, icecream: None };
    for y in 0..7usize {
        let row = lines.next().unwrap();
        for (x, c) in row.bytes().enumerate() {
            let p = Pt { x, y };
            match c {
                b'D' => if k.dish.is_none() { k.dish = Some(p); },
                b'W' => if k.window.is_none() { k.window = Some(p); },
                b'B' => if k.berries.is_none() { k.berries = Some(p); },
                b'I' => if k.icecream.is_none() { k.icecream = Some(p); },
                _ => {}
            }
        }
    }

    let stdout = io::stdout();
    let mut handle = stdout.lock();
    let mut out = String::new();
    while let Some(_turns) = lines.next() {
        if _turns.is_empty() { continue; }
        let line = match lines.next() { Some(s) => s, None => break };
        let f: Vec<&str> = line.split_whitespace().collect();
        let held = f[2];
        lines.next();  // partenaire
        let nt: usize = lines.next().unwrap().trim().parse()?;
        let mut pos = Vec::with_capacity(nt);
        let mut names = Vec::with_capacity(nt);
        for _ in 0..nt {
            let itemline = lines.next().unwrap();
            let t: Vec<&str> = itemline.split_whitespace().collect();
            pos.push(Pt { x: t[0].parse()?, y: t[1].parse()? });
            names.push(t[2].to_string());
        }
        lines.next();  // four
        let nc: usize = lines.next().unwrap().trim().parse()?;
        for _ in 0..nc { lines.next(); }

        let (bb, ic) = contents(held);
        let is_dish = held.starts_with("DISH");
        let target: Option<Pt> = if is_dish && bb && ic {
            k.window
        } else if is_dish {
            let (mbb, mic) = (!bb, !ic);
            let mut t = None;
            if mbb {
                for i in 0..nt { if names[i] == "BLUEBERRIES" { t = Some(pos[i]); break; } }
            }
            if t.is_none() && mic {
                for i in 0..nt { if names[i] == "ICE_CREAM" { t = Some(pos[i]); break; } }
            }
            t.or(if mbb { k.berries } else { k.icecream })
        } else if held == "BLUEBERRIES" || held == "ICE_CREAM" {
            k.dish
        } else {
            let mut t = None;
            for i in 0..nt {
                let (a, b) = contents(&names[i]);
                if names[i].starts_with("DISH") && a && b { t = Some(pos[i]); break; }
            }
            if t.is_none() {
                for i in 0..nt { if names[i].starts_with("DISH") { t = Some(pos[i]); break; } }
            }
            if t.is_none() {
                for i in 0..nt {
                    if names[i] == "BLUEBERRIES" || names[i] == "ICE_CREAM" {
                        t = Some(pos[i]); break;
                    }
                }
            }
            t.or(k.dish)
        };
        emit(target, &mut out);
        handle.write_all(out.as_bytes())?;
        handle.flush()?;
        out.clear();
    }
    Ok(())
}
