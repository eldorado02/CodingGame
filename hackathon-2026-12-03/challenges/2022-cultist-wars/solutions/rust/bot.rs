// Cultist Wars — conversions, neutralisation des menaces, tirs.
use std::io::{self, BufRead, Write};

#[derive(Clone, Copy)]
struct Unit {
    id: i32,
    kind: i32,
    hp: i32,
    x: i32,
    y: i32,
    owner: i32,
}

fn manhattan(a: &Unit, b: &Unit) -> i32 {
    (a.x - b.x).abs() + (a.y - b.y).abs()
}

// Bresenham du moteur : tracé depuis la coordonnée Y la plus basse.
// Renvoie l'index de l'unité touchée, ou None.
fn ray_victim(shooter: &Unit, target: &Unit, units: &[Unit], grid: &[String]) -> Option<usize> {
    let (x0, y0, x1, y1, backward) = if shooter.y < target.y {
        (shooter.x, shooter.y, target.x, target.y, false)
    } else {
        (target.x, target.y, shooter.x, shooter.y, true)
    };
    let dx = (x1 - x0).abs();
    let dy = (y1 - y0).abs();
    let sx = if x0 < x1 { 1 } else { -1 };
    let sy = if y0 < y1 { 1 } else { -1 };
    let mut err = dx - dy;
    let (mut cx, mut cy) = (x0, y0);
    let (mut bx, mut by) = (x1, y1);
    let w = grid[0].len() as i32;
    let h = grid.len() as i32;
    loop {
        let e2 = 2 * err;
        if e2 > -dy {
            err -= dy;
            cx += sx;
        }
        if e2 < dx {
            err += dx;
            cy += sy;
        }
        if cx == x1 && cy == y1 {
            break;
        }
        if cx < 0 || cx >= w || cy < 0 || cy >= h {
            return None;
        }
        let obstacle = grid[cy as usize].as_bytes()[cx as usize] == b'x';
        let blocker = units.iter().position(|u| {
            u.hp > 0 && u.x == cx && u.y == cy
        });
        if obstacle || blocker.is_some() {
            if backward {
                bx = cx;
                by = cy;
            } else if let Some(v) = blocker {
                return Some(v);
            } else {
                return None;
            }
        }
    }
    units.iter().position(|u| u.hp > 0 && u.x == bx && u.y == by)
}

fn bfs_distance(start: &Unit, goal: &Unit, grid: &[String]) -> i32 {
    if start.x == goal.x && start.y == goal.y {
        return 0;
    }
    let w = grid[0].len();
    let h = grid.len();
    let mut seen = vec![vec![false; w]; h];
    let mut queue = vec![(start.x, start.y, 0i32)];
    seen[start.y as usize][start.x as usize] = true;
    let dirs = [(0, -1), (1, 0), (0, 1), (-1, 0)];
    let mut head = 0usize;
    while head < queue.len() {
        let (x, y, d) = queue[head];
        head += 1;
        for (dx, dy) in dirs {
            let nx = x + dx;
            let ny = y + dy;
            if nx < 0 || nx >= w as i32 || ny < 0 || ny >= h as i32 {
                continue;
            }
            if seen[ny as usize][nx as usize] || grid[ny as usize].as_bytes()[nx as usize] == b'x' {
                continue;
            }
            if nx == goal.x && ny == goal.y {
                return d + 1;
            }
            seen[ny as usize][nx as usize] = true;
            queue.push((nx, ny, d + 1));
        }
    }
    1_000_000
}

type ShotKey = (i32, i32, i32, i32, i32);

fn find_shot(
    cultists: &[usize],
    enemies: &[usize],
    units: &[Unit],
    grid: &[String],
    my_id: i32,
    allowed: Option<&[usize]>,
) -> Option<(usize, usize)> {
    let mut best: ShotKey = (9, 0, 0, 0, 0);
    let mut result = None;
    for &c in cultists {
        for &e in enemies {
            if let Some(a) = allowed {
                if !a.contains(&e) {
                    continue;
                }
            }
            if manhattan(&units[c], &units[e]) > 6 {
                continue;
            }
            let v = match ray_victim(&units[c], &units[e], units, grid) {
                Some(v) => v,
                None => continue,
            };
            if units[v].owner == my_id || units[v].owner == 2 {
                continue;
            }
            let d = manhattan(&units[c], &units[v]);
            let dmg = 7 - d;
            let key = (
                if dmg >= units[v].hp { 0 } else { 1 },
                -dmg,
                d,
                units[v].id,
                units[c].id,
            );
            if key < best {
                best = key;
                result = Some((c, v));
            }
        }
    }
    result
}

fn main() {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();
    let mut next = || lines.next().and_then(|l| l.ok()).unwrap_or_default();

    let my_id: i32 = next().trim().parse().unwrap_or(0);
    let dims: Vec<i32> = next().split_whitespace().filter_map(|s| s.parse().ok()).collect();
    let w = dims[0] as usize;
    let h = dims[1] as usize;
    let grid: Vec<String> = (0..h).map(|_| next()).collect();

    let stdout = io::stdout();
    let mut out = io::BufWriter::new(stdout.lock());
    loop {
        let line = next();
        if line.is_empty() {
            break;
        }
        let n: usize = match line.trim().parse() {
            Ok(v) => v,
            Err(_) => break,
        };
        let mut units = Vec::with_capacity(n);
        for _ in 0..n {
            let f: Vec<i32> = next().split_whitespace().filter_map(|s| s.parse().ok()).collect();
            if f.len() < 6 {
                continue;
            }
            units.push(Unit {
                id: f[0], kind: f[1], hp: f[2], x: f[3], y: f[4], owner: f[5],
            });
        }

        let mut leader = None;
        let mut mine = Vec::new();
        let mut cultists = Vec::new();
        let mut enemies = Vec::new();
        let mut enemy_cultists = Vec::new();
        let mut neutrals = Vec::new();
        for (i, u) in units.iter().enumerate() {
            if u.hp <= 0 {
                continue;
            }
            if u.owner == my_id {
                mine.push(i);
                if u.kind == 1 {
                    leader = Some(i);
                } else {
                    cultists.push(i);
                }
            } else if u.owner == 2 {
                neutrals.push(i);
            } else {
                enemies.push(i);
                if u.kind == 0 {
                    enemy_cultists.push(i);
                }
            }
        }

        // 1. Neutraliser un cultiste ennemi qui menace nos unités.
        let mut threats = Vec::new();
        for &e in &enemy_cultists {
            let found = mine.iter().any(|&u| {
                manhattan(&units[e], &units[u]) <= 6
                    && ray_victim(&units[e], &units[u], &units, &grid) == Some(u)
            });
            if found {
                threats.push(e);
            }
        }
        if !threats.is_empty() {
            if let Some((c, v)) = find_shot(&cultists, &enemies, &units, &grid, my_id, Some(&threats)) {
                writeln!(out, "{} SHOOT {}", units[c].id, units[v].id).unwrap();
                out.flush().unwrap();
                continue;
            }
        }

        // 2. Le leader convertit les neutres de sa moitié.
        if let Some(l) = leader {
            let mid = (w / 2) as i32;
            let mut target = None;
            let mut best_d = 1_000_000;
            for &idx in &neutrals {
                if (my_id == 0 && units[idx].x > mid) || (my_id == 1 && units[idx].x < mid) {
                    continue;
                }
                let d = bfs_distance(&units[l], &units[idx], &grid);
                if d < best_d || (d == best_d && target.map_or(true, |t: usize| units[idx].id < units[t].id)) {
                    best_d = d;
                    target = Some(idx);
                }
            }
            if let Some(t) = target {
                writeln!(out, "{} CONVERT {}", units[l].id, units[t].id).unwrap();
                out.flush().unwrap();
                continue;
            }
        }

        // 3. Vol d'un cultiste ennemi voisin.
        if let Some(l) = leader {
            let mut target = None;
            let mut best_d = 1_000_000;
            for &e in &enemy_cultists {
                let d = bfs_distance(&units[l], &units[e], &grid);
                if d <= 2 && (d < best_d || (d == best_d && target.map_or(true, |t: usize| units[e].id < units[t].id))) {
                    best_d = d;
                    target = Some(e);
                }
            }
            if let Some(t) = target {
                writeln!(out, "{} CONVERT {}", units[l].id, units[t].id).unwrap();
                out.flush().unwrap();
                continue;
            }
        }

        // 4. Tir libre.
        if let Some((c, v)) = find_shot(&cultists, &enemies, &units, &grid, my_id, None) {
            writeln!(out, "{} SHOOT {}", units[c].id, units[v].id).unwrap();
            out.flush().unwrap();
            continue;
        }

        // 5. Un SHOOT hors portée déplace le cultiste vers la cible.
        if !cultists.is_empty() && !enemies.is_empty() {
            let mut best_c = cultists[0];
            let mut best_d = 1_000_000;
            for &c in &cultists {
                let md = enemies.iter().map(|&e| manhattan(&units[c], &units[e])).min().unwrap_or(1_000_000);
                if md < best_d || (md == best_d && units[c].id < units[best_c].id) {
                    best_d = md;
                    best_c = c;
                }
            }
            let mut best_e = enemies[0];
            best_d = 1_000_000;
            for &e in &enemies {
                let d = manhattan(&units[best_c], &units[e]);
                if d < best_d || (d == best_d && units[e].id < units[best_e].id) {
                    best_d = d;
                    best_e = e;
                }
            }
            writeln!(out, "{} SHOOT {}", units[best_c].id, units[best_e].id).unwrap();
            out.flush().unwrap();
            continue;
        }

        writeln!(out, "WAIT").unwrap();
        out.flush().unwrap();
    }
}
