// Back Track King (Bronze+) — connexion désirée la moins chère et DISRUPT
// ciblé sur les régions où l'adversaire possède le plus de rails.
use std::cmp::Reverse;
use std::collections::{BinaryHeap, HashSet, VecDeque};
use std::io::{self, BufRead, Write};

fn is_inked(s: &str) -> bool {
    let c = s.as_bytes().first().copied().unwrap_or(b'0');
    c != b'0' && c != b'f' && c != b'F'
}

fn parse_pairs(s: &str) -> HashSet<(usize, usize)> {
    let mut out = HashSet::new();
    for item in s.split(',') {
        if let Some((a, b)) = item.split_once('-') {
            if !a.is_empty() && !b.is_empty() && a.bytes().all(|c| c.is_ascii_digit())
                && b.bytes().all(|c| c.is_ascii_digit())
            {
                let x: usize = a.parse().unwrap_or(0);
                let y: usize = b.parse().unwrap_or(0);
                out.insert((x.min(y), x.max(y)));
            }
        }
    }
    out
}

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
    let mut terrain = Vec::with_capacity(cells);
    for _ in 0..cells {
        region_at.push(token().unwrap().parse::<i32>().unwrap_or(0));
        terrain.push(token().unwrap().parse::<i32>().unwrap_or(0));
    }

    let mut towns: Vec<(i32, i32)> = vec![(-1, -1); 64];
    let mut desired: HashSet<(usize, usize)> = HashSet::new();
    let mut town_regions: HashSet<i32> = HashSet::new();
    let town_count: usize = token().unwrap().parse().unwrap_or(0);
    for _ in 0..town_count {
        let id: usize = token().unwrap().parse().unwrap_or(0);
        let x: i32 = token().unwrap().parse().unwrap_or(0);
        let y: i32 = token().unwrap().parse().unwrap_or(0);
        let wanted = token().unwrap();
        if id >= towns.len() {
            continue;
        }
        towns[id] = (x, y);
        let idx = (y as usize) * width + (x as usize);
        if let Some(&rid) = region_at.get(idx) {
            town_regions.insert(rid);
        }
        if wanted != "x" {
            for w in wanted.split(',') {
                if !w.is_empty() && w.bytes().all(|c| c.is_ascii_digit()) {
                    let v: usize = w.parse().unwrap_or(0);
                    desired.insert((id.min(v), id.max(v)));
                }
            }
        }
    }

    let stdout = io::stdout();
    let mut out = io::BufWriter::new(stdout.lock());
    const DIRS: [(i32, i32); 4] = [(0, -1), (1, 0), (0, 1), (-1, 0)];

    while token().is_some() {
        token(); // foeScore
        let mut owner = Vec::with_capacity(cells);
        let mut instability = Vec::with_capacity(cells);
        let mut inked = Vec::with_capacity(cells);
        let mut cell_active = Vec::with_capacity(cells);
        let mut active: HashSet<(usize, usize)> = HashSet::new();
        for _ in 0..cells {
            owner.push(token().unwrap().parse::<i32>().unwrap_or(0));
            instability.push(token().unwrap().parse::<i32>().unwrap_or(0));
            inked.push(is_inked(&token().unwrap()));
            let pairs = parse_pairs(&token().unwrap());
            active.extend(pairs.iter().copied());
            cell_active.push(pairs);
        }

        let path_cost = |a: usize, b: usize| -> Option<i64> {
            if towns[a].0 < 0 || towns[b].0 < 0 {
                return None;
            }
            let si = towns[a].1 as usize * width + towns[a].0 as usize;
            let ti = towns[b].1 as usize * width + towns[b].0 as usize;
            let mut dist = vec![i64::MAX; cells];
            let mut heap: BinaryHeap<Reverse<(i64, usize)>> = BinaryHeap::new();
            dist[si] = 0;
            heap.push(Reverse((0, si)));
            while let Some(Reverse((d, i))) = heap.pop() {
                if d != dist[i] {
                    continue;
                }
                if i == ti {
                    return Some(d);
                }
                let x = (i % width) as i32;
                let y = (i / width) as i32;
                for (dx, dy) in DIRS {
                    let nx = x + dx;
                    let ny = y + dy;
                    if nx < 0 || nx >= width as i32 || ny < 0 || ny >= height as i32 {
                        continue;
                    }
                    let ni = ny as usize * width + nx as usize;
                    if inked[ni] {
                        continue;
                    }
                    let nd = d + if owner[ni] >= 0 { 0 } else { terrain[ni] as i64 + 1 };
                    if nd < dist[ni] {
                        dist[ni] = nd;
                        heap.push(Reverse((nd, ni)));
                    }
                }
            }
            None
        };

        let mut targets: Vec<(i64, usize, usize, i32, i32, i32, i32)> = Vec::new();
        let mut pairs: Vec<(usize, usize)> = desired.iter().copied().collect();
        pairs.sort_unstable();
        for (a, b) in pairs {
            if active.contains(&(a, b)) {
                continue;
            }
            if let Some(d) = path_cost(a, b) {
                if d == 0 {
                    continue;
                }
                targets.push((d, a, b, towns[a].0, towns[a].1, towns[b].0, towns[b].1));
            }
        }
        targets.sort();

        let max_reg = 4096usize;
        let mut mine = vec![0i32; max_reg];
        let mut foe = vec![0i32; max_reg];
        let mut inst = vec![0i32; max_reg];
        let mut reg_active = vec![0i32; max_reg];
        for i in 0..cells {
            let rid = region_at[i];
            if rid < 0 || rid as usize >= max_reg || town_regions.contains(&rid) || inked[i] {
                continue;
            }
            let r = rid as usize;
            if owner[i] == my_id {
                mine[r] += 1;
            } else if owner[i] >= 0 && owner[i] != 2 {
                foe[r] += 1;
            }
            inst[r] = inst[r].max(instability[i]);
            if !cell_active[i].is_empty() {
                reg_active[r] = 1;
            }
        }
        let mut candidates: Vec<(i32, i32, i32, usize)> = Vec::new();
        for rid in 0..max_reg {
            if foe[rid] > mine[rid] {
                candidates.push((reg_active[rid], foe[rid], inst[rid], rid));
            }
        }
        candidates.sort_by(|a, b| b.cmp(a));

        let mut answer = String::new();
        if let Some(t) = targets.first() {
            answer = format!("AUTOPLACE {} {} {} {}", t.3, t.4, t.5, t.6);
        }
        let disrupted = candidates.first().map(|c| c.3);
        if let Some(rid) = disrupted {
            if !answer.is_empty() {
                answer.push(';');
            }
            answer.push_str(&format!("DISRUPT {}", rid));
        }

        // Fin de partie : vol des chemins actifs par le tie-break N/E/S/O.
        if targets.is_empty() {
            let mut town_cell = vec![false; cells];
            for &(x, y) in towns.iter().filter(|t| t.0 >= 0) {
                let idx = y as usize * width + x as usize;
                if idx < cells {
                    town_cell[idx] = true;
                }
            }
            let mut steal: Vec<(i32, i32, i32, i32, i32, usize)> = Vec::new();
            for i in 0..cells {
                if owner[i] != -1 || inked[i] || town_cell[i] {
                    continue;
                }
                if let Some(rid) = disrupted {
                    if region_at[i] == rid as i32 {
                        continue;
                    }
                }
                let x = (i % width) as i32;
                let y = (i / width) as i32;
                let mut n = 0;
                let mut f = 0;
                for (dx, dy) in DIRS {
                    let nx = x + dx;
                    let ny = y + dy;
                    if nx < 0 || nx >= width as i32 || ny < 0 || ny >= height as i32 {
                        continue;
                    }
                    let j = ny as usize * width + nx as usize;
                    if cell_active[j].is_empty() {
                        continue;
                    }
                    n += 1;
                    if owner[j] >= 0 && owner[j] != 2 && owner[j] != my_id {
                        f += 1;
                    }
                }
                if n > 0 {
                    steal.push((n, f, -terrain[i], y, x, i));
                }
            }
            steal.sort_by(|a, b| b.cmp(a));
            for s in steal.iter().take(3) {
                if !answer.is_empty() {
                    answer.push(';');
                }
                answer.push_str(&format!("PLACE_TRACKS {} {}", s.4, s.3));
            }
        }
        if answer.is_empty() {
            answer.push_str("WAIT");
        }
        writeln!(out, "{}", answer).unwrap();
        out.flush().unwrap();
    }
}
