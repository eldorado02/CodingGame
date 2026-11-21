// Back Track King (Wood) — une connexion active suffit : AUTOPLACE construit
// progressivement le chemin entre le couple de villes le plus proche.
use std::collections::{HashMap, VecDeque};
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

    token();  // myId
    let width: usize = token().unwrap().parse().unwrap_or(0);
    let height: usize = token().unwrap().parse().unwrap_or(0);
    for _ in 0..width * height {
        token();  // regionId
        token();  // type
    }

    let town_count: usize = token().unwrap().parse().unwrap_or(0);
    let mut towns: HashMap<i32, (i32, i32)> = HashMap::new();
    let mut pairs: Vec<(i32, i32)> = Vec::new();
    for _ in 0..town_count {
        let id: i32 = token().unwrap().parse().unwrap_or(0);
        let x: i32 = token().unwrap().parse().unwrap_or(0);
        let y: i32 = token().unwrap().parse().unwrap_or(0);
        let desired = token().unwrap();
        towns.insert(id, (x, y));
        if desired == "x" {
            continue;
        }
        for wanted in desired.split(',') {
            pairs.push((id, wanted.parse().unwrap_or(0)));
        }
    }

    let key = |&(u, v): &(i32, i32)| -> (i32, i32, i32) {
        let (ax, ay) = towns[&u];
        let (bx, by) = towns[&v];
        ((ax - bx).abs() + (ay - by).abs(), u, v)
    };
    let best = pairs.iter().min_by_key(|&&p| key(&p)).copied().unwrap_or((0, 0));
    let (x1, y1) = towns[&best.0];
    let (x2, y2) = towns[&best.1];

    let stdout = io::stdout();
    let mut out = io::BufWriter::new(stdout.lock());
    while token().is_some() {
        token();  // foeScore
        for _ in 0..width * height * 4 {
            token();
        }
        writeln!(out, "AUTOPLACE {} {} {} {}", x1, y1, x2, y2).unwrap();
        out.flush().unwrap();
    }
}
