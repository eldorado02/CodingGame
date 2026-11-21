// Troll Farm (Wood 1+) — entraîne un troll polyvalent, récolte, dépose.
use std::collections::VecDeque;
use std::io::{self, BufRead, Write};

const DX: [i32; 4] = [0, 1, 0, -1];
const DY: [i32; 4] = [-1, 0, 1, 0];

#[derive(Clone, Copy)]
struct Tree {
    x: usize,
    y: usize,
    fruits: i32,
}

#[derive(Clone, Copy)]
struct Troll {
    id: i32,
    x: usize,
    y: usize,
    speed: usize,
    carry: i32,
    load: i32,
}

fn main() {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();
    let first = match lines.next() {
        Some(Ok(s)) => s,
        _ => return,
    };
    let mut header = first.split_whitespace();
    let w: usize = header.next().unwrap().parse().unwrap();
    let h: usize = header.next().unwrap().parse().unwrap();
    let mut grid = Vec::with_capacity(h);
    for _ in 0..h {
        grid.push(lines.next().unwrap().unwrap().trim().to_string());
    }
    let walkable = |x: usize, y: usize| {
        x < w && y < h && grid[y].as_bytes().get(x) == Some(&b'.')
    };
    let mut shack = (0usize, 0usize);
    for y in 0..h {
        for x in 0..w {
            if grid[y].as_bytes()[x] == b'0' {
                shack = (x, y);
            }
        }
    }
    let idx = |x: usize, y: usize| y * w + x;
    let mut drop = Vec::new();
    for d in 0..4 {
        let nx = shack.0 as i32 + DX[d];
        let ny = shack.1 as i32 + DY[d];
        if nx >= 0 && ny >= 0 {
            let (x, y) = (nx as usize, ny as usize);
            if walkable(x, y) {
                drop.push(idx(x, y));
            }
        }
    }

    let mut trained = false;
    let stdout = io::stdout();
    let mut out = io::BufWriter::new(stdout.lock());
    loop {
        let line = match lines.next() {
            Some(Ok(s)) => s,
            _ => break,
        };
        if line.is_empty() {
            break;
        }
        let inv: Vec<i32> = line.split_whitespace().map(|t| t.parse().unwrap()).collect();
        if inv.len() < 6 {
            break;
        }
        let _foe: Vec<i32> = match lines.next() {
            Some(Ok(s)) => s.split_whitespace().map(|t| t.parse().unwrap()).collect(),
            _ => break,
        };
        let tree_n: usize = match lines.next() {
            Some(Ok(s)) => s.trim().parse().unwrap(),
            _ => break,
        };
        let mut trees = Vec::with_capacity(tree_n);
        for _ in 0..tree_n {
            let tree_line = lines.next().unwrap().unwrap();
            let f: Vec<&str> = tree_line.split_whitespace().collect();
            trees.push(Tree {
                x: f[1].parse().unwrap(),
                y: f[2].parse().unwrap(),
                fruits: f[5].parse().unwrap(),
            });
        }
        let troll_n: usize = match lines.next() {
            Some(Ok(s)) => s.trim().parse().unwrap(),
            _ => break,
        };
        let mut mine = Vec::new();
        let mut occupied = vec![false; w * h];
        for _ in 0..troll_n {
            let troll_line = lines.next().unwrap().unwrap();
            let f: Vec<&str> = troll_line.split_whitespace().collect();
            let player: i32 = f[1].parse().unwrap();
            let x: usize = f[2].parse().unwrap();
            let y: usize = f[3].parse().unwrap();
            occupied[idx(x, y)] = true;
            if player == 0 {
                let load: i32 = f[8..12].iter().map(|t| t.parse::<i32>().unwrap()).sum();
                mine.push(Troll {
                    id: f[0].parse().unwrap(),
                    x,
                    y,
                    speed: f[4].parse().unwrap(),
                    carry: f[5].parse().unwrap(),
                    load,
                });
            }
        }

        let mut actions: Vec<String> = Vec::new();
        if !trained && !mine.is_empty() {
            let n = mine.len() as i32;
            let mut best: Option<(i32, i32, i32, i32)> = None;
            for s in 1..=3 {
                for c in 1..=3 {
                    for hv in 1..=3 {
                        let (cp, cl, ca) = (n + s * s, n + c * c, n + hv * hv);
                        if cp > inv[0] || cl > inv[1] || ca > inv[2] {
                            continue;
                        }
                        let score = s + c + hv;
                        if best.map_or(true, |b| score > b.0) {
                            best = Some((score, s, c, hv));
                        }
                    }
                }
            }
            if let Some((_, s, c, hv)) = best {
                actions.push(format!("TRAIN {} {} {} 0", s, c, hv));
            }
            trained = true;
        }

        for t in &mine {
            let mut dist = vec![-1i32; w * h];
            let mut first = vec![0usize; w * h];
            let mut q = VecDeque::new();
            let start = idx(t.x, t.y);
            dist[start] = 0;
            first[start] = start;
            q.push_back(start);
            while let Some(cur) = q.pop_front() {
                let (x, y) = (cur % w, cur / w);
                for d in 0..4 {
                    let nx = x as i32 + DX[d];
                    let ny = y as i32 + DY[d];
                    if nx < 0 || ny < 0 {
                        continue;
                    }
                    let (nx, ny) = (nx as usize, ny as usize);
                    if !walkable(nx, ny) {
                        continue;
                    }
                    let ni = idx(nx, ny);
                    if occupied[ni] || dist[ni] >= 0 {
                        continue;
                    }
                    dist[ni] = dist[cur] + 1;
                    first[ni] = if cur == start { ni } else { first[cur] };
                    q.push_back(ni);
                }
            }
            let step = |goal: usize| {
                if dist[goal] <= t.speed as i32 {
                    return goal;
                }
                let mut cur = goal;
                while dist[cur] > t.speed as i32 {
                    cur = first[cur];
                }
                cur
            };
            let on_fruit = trees.iter().any(|tr| {
                tr.fruits > 0 && tr.x == t.x && tr.y == t.y
            });
            if on_fruit && t.load < t.carry {
                actions.push(format!("HARVEST {}", t.id));
                continue;
            }
            if t.load > 0 {
                if drop.contains(&start) {
                    actions.push(format!("DROP {}", t.id));
                    continue;
                }
                let mut best: Option<usize> = None;
                for &cell in &drop {
                    if dist[cell] < 0 {
                        continue;
                    }
                    let (x, y) = (cell % w, cell / w);
                    let bx = best.map_or(0, |b| b % w);
                    let by = best.map_or(0, |b| b / w);
                    if best.is_none()
                        || dist[cell] < dist[best.unwrap()]
                        || (dist[cell] == dist[best.unwrap()]
                            && (y < by || (y == by && x < bx)))
                    {
                        best = Some(cell);
                    }
                }
                match best {
                    None => actions.push("WAIT".to_string()),
                    Some(cell) => {
                        let st = step(cell);
                        actions.push(format!("MOVE {} {} {}", t.id, st % w, st / w));
                    }
                }
                continue;
            }
            let mut fruit_at = vec![0i32; w * h];
            for tr in &trees {
                fruit_at[idx(tr.x, tr.y)] = tr.fruits;
            }
            let mut best: Option<usize> = None;
            for tr in &trees {
                if tr.fruits <= 0 {
                    continue;
                }
                let cell = idx(tr.x, tr.y);
                if dist[cell] < 0 {
                    continue;
                }
                let bx = best.map_or(0, |b| b % w);
                let by = best.map_or(0, |b| b / w);
                let better = best.is_none()
                    || dist[cell] < dist[best.unwrap()]
                    || (dist[cell] == dist[best.unwrap()]
                        && (tr.fruits > fruit_at[best.unwrap()]
                            || (tr.fruits == fruit_at[best.unwrap()]
                                && (tr.y < by || (tr.y == by && tr.x < bx)))));
                if better {
                    best = Some(cell);
                }
            }
            match best {
                None => actions.push("WAIT".to_string()),
                Some(cell) => {
                    let st = step(cell);
                    actions.push(format!("MOVE {} {} {}", t.id, st % w, st / w));
                }
            }
        }
        if actions.is_empty() {
            actions.push("WAIT".to_string());
        }
        writeln!(out, "{}", actions.join(";")).unwrap();
        out.flush().unwrap();
    }
}
