// Back Track King (Wood 1+) — ink la région ennemie la plus instable.
use std::collections::VecDeque;
use std::io::{self, BufRead, Write};

fn main() {
    let stdin = io::stdin();
    let mut input = stdin.lock();
    let mut buffer: VecDeque<String> = VecDeque::new();
    let mut tokens = std::iter::from_fn(|| {
        while buffer.is_empty() {
            let mut line = String::new();
            match input.read_line(&mut line) {
                Ok(0) | Err(_) => return None,
                Ok(_) => buffer.extend(line.split_whitespace().map(str::to_string)),
            }
        }
        buffer.pop_front()
    });
    let mut token = || tokens.next();

    let my_id: i32 = token().unwrap().parse().unwrap_or(0);
    let width: usize = token().unwrap().parse().unwrap_or(0);
    let height: usize = token().unwrap().parse().unwrap_or(0);
    let cells = width * height;
    let mut region_at = Vec::with_capacity(cells);
    for _ in 0..cells {
        region_at.push(token().unwrap().parse::<i32>().unwrap_or(0));
        token(); // type
    }

    let town_count: usize = token().unwrap().parse().unwrap_or(0);
    let mut town_region = vec![false; 4096];
    for _ in 0..town_count {
        token(); // townId
        let x: usize = token().unwrap().parse().unwrap_or(0);
        let y: usize = token().unwrap().parse().unwrap_or(0);
        token(); // desiredConnections
        if let Some(&rid) = region_at.get(y * width + x) {
            if rid >= 0 && (rid as usize) < town_region.len() {
                town_region[rid as usize] = true;
            }
        }
    }

    let opponent = 1 - my_id;
    let stdout = io::stdout();
    let mut out = io::BufWriter::new(stdout.lock());
    while token().is_some() {
        token(); // foeScore
        let mut best: Option<(i32, i32)> = None; // (instability, -regionId)
        for &rid in &region_at {
            let owner: i32 = token().unwrap().parse().unwrap_or(0);
            let instability: i32 = token().unwrap().parse().unwrap_or(0);
            let inked = token().unwrap();
            token(); // partOfActiveConnections
            if owner != opponent || rid < 0 || town_region[rid as usize] {
                continue;
            }
            let first = inked.as_bytes().first().copied().unwrap_or(b'0');
            if first != b'0' && first != b'f' && first != b'F' {
                continue;
            }
            let key = (instability, -rid);
            if best.is_none() || key > best.unwrap() {
                best = Some(key);
            }
        }
        match best {
            Some((_, rid)) => writeln!(out, "DISRUPT {}", -rid).unwrap(),
            None => writeln!(out, "WAIT").unwrap(),
        }
        out.flush().unwrap();
    }
}
