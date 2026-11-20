// Green Circle (Wood 4) — releases utiles et bureaux utiles.
use std::io::{self, BufRead, Write};

type Hand = [i32; 10];
type App = [i32; 8];

fn good_score(hand: &Hand, app: &App) -> i32 {
    let mut covered = 0;
    let mut total = 0;
    for i in 0..8 {
        covered += app[i].min(2 * hand[i]);
        total += app[i];
    }
    covered + hand[8].min(total - covered)
}

fn best_app(hand: &Hand, ids: &[i32], apps: &[(i32, App)]) -> i32 {
    let mut best_id = -1;
    let mut best_score = -1;
    for aid in ids {
        let Some((_, app)) = apps.iter().find(|(id, _)| id == aid) else { continue };
        let score = good_score(hand, app);
        if score > best_score || (score == best_score && *aid < best_id) {
            best_score = score;
            best_id = *aid;
        }
    }
    best_id
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();
    let stdout = io::stdout();
    let mut out = stdout.lock();

    loop {
        let phase_line = match lines.next() {
            Some(l) => l?,
            None => break,
        };
        if phase_line.trim().is_empty() {
            break;
        }
        let phase = phase_line.split_whitespace().next().unwrap_or("").to_string();

        let app_count: usize = lines.next().unwrap()?.parse()?;
        let mut apps: Vec<(i32, App)> = Vec::new();
        for _ in 0..app_count {
            let l = lines.next().unwrap()?;
            let f: Vec<&str> = l.split_whitespace().collect();
            if f.len() < 10 {
                continue;
            }
            let mut app = [0i32; 8];
            for j in 0..8 {
                app[j] = f[2 + j].parse()?;
            }
            apps.push((f[1].parse()?, app));
        }

        lines.next(); // nous
        lines.next(); // adversaire

        let loc_count: usize = lines.next().unwrap()?.parse()?;
        let mut hand = [0i32; 10];
        for _ in 0..loc_count {
            let l = lines.next().unwrap()?;
            let f: Vec<&str> = l.split_whitespace().collect();
            if f.len() < 11 || f[0] != "HAND" {
                continue;
            }
            for j in 0..10 {
                hand[j] = f[1 + j].parse()?;
            }
        }

        let move_count: usize = lines.next().unwrap()?.parse()?;
        let mut moves = Vec::new();
        for _ in 0..move_count {
            moves.push(lines.next().unwrap()?);
        }

        if phase == "MOVE" {
            let all_ids: Vec<i32> = apps.iter().map(|(id, _)| *id).collect();
            let mut chosen = -1;
            let mut best_score = -1;
            for m in &moves {
                let f: Vec<&str> = m.split_whitespace().collect();
                if f.len() < 2 || f[0] != "MOVE" {
                    continue;
                }
                let dest: i32 = f[1].parse()?;
                let mut h2 = hand;
                h2[dest as usize] += 1;
                let aid = best_app(&h2, &all_ids, &apps);
                let score = if aid >= 0 {
                    good_score(&h2, &apps.iter().find(|(id, _)| *id == aid).unwrap().1)
                } else {
                    0
                };
                if score > best_score || (score == best_score && dest < chosen) {
                    best_score = score;
                    chosen = dest;
                }
            }
            if chosen >= 0 {
                writeln!(out, "MOVE {}", chosen)?;
            } else {
                writeln!(out, "{}", moves.first().map(String::as_str).unwrap_or("WAIT"))?;
            }
        } else if phase == "RELEASE" {
            let mut ids = Vec::new();
            for m in &moves {
                let f: Vec<&str> = m.split_whitespace().collect();
                if f.len() >= 2 && f[0] == "RELEASE" {
                    ids.push(f[1].parse()?);
                }
            }
            let aid = best_app(&hand, &ids, &apps);
            if aid >= 0 {
                writeln!(out, "RELEASE {}", aid)?;
            } else {
                writeln!(out, "WAIT")?;
            }
        } else {
            writeln!(out, "{}", moves.first().map(String::as_str).unwrap_or("WAIT"))?;
        }
        out.flush()?;
    }
    Ok(())
}
