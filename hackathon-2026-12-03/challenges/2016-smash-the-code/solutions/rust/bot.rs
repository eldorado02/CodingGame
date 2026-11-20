// Smash The Code — solution Rust (Wood league).
// Sans dépendance : std uniquement. Aucun unwrap dans la logique de jeu.
use std::collections::VecDeque;
use std::io::{self, BufRead, Write};

const W: usize = 6;
const H: usize = 12;
const DR: [isize; 4] = [1, -1, 0, 0];
const DC: [isize; 4] = [0, 0, 1, -1];

type Grid = Vec<Vec<u8>>;

#[derive(Clone, Copy, Default)]
struct Outcome {
    cleared: usize,
    chains: usize,
}

/// Dépose une paire verticale ; `Err` si la colonne déborde.
fn drop_pair(grid: &mut Grid, col: usize, top: u8, bottom: u8) -> Result<(), &'static str> {
    let row = (0..H).find(|&r| grid[r][col] != b'.').unwrap_or(H);
    if row < 2 {
        return Err("top out");
    }
    grid[row - 2][col] = top;
    grid[row - 1][col] = bottom;
    Ok(())
}

/// Compacte chaque colonne vers le bas, en conservant l'ordre des blocs.
fn gravity(grid: &mut Grid) {
    for col in 0..W {
        // Ordre haut -> bas : la réécriture ci-dessous se fait aussi haut -> bas,
        // sinon l'ordre des blocs serait inversé (chaînes erronées).
        let cells: Vec<u8> = (0..H)
            .filter_map(|r| {
                let v = grid[r][col];
                (v != b'.').then_some(v)
            })
            .collect();
        let first = H - cells.len();
        for (row, cell) in grid.iter_mut().enumerate() {
            cell[col] = if row >= first {
                cells[row - first]
            } else {
                b'.'
            };
        }
    }
}

/// Efface les groupes de 4+, applique la gravité, recommence jusqu'à stabilisation.
fn resolve(grid: &mut Grid) -> Outcome {
    let mut out = Outcome::default();
    loop {
        let mut visited = vec![vec![false; W]; H];
        let mut to_clear = vec![vec![false; W]; H];
        let mut found = false;

        for row in 0..H {
            for col in 0..W {
                let colour = grid[row][col];
                if colour == b'.' || colour == b'0' || visited[row][col] {
                    continue;
                }
                let mut queue = VecDeque::new();
                let mut group = Vec::new();
                queue.push_back((row, col));
                visited[row][col] = true;
                while let Some((r, c)) = queue.pop_front() {
                    group.push((r, c));
                    for d in 0..4 {
                        let nr = r as isize + DR[d];
                        let nc = c as isize + DC[d];
                        if nr < 0 || nr >= H as isize || nc < 0 || nc >= W as isize {
                            continue;
                        }
                        let (nr, nc) = (nr as usize, nc as usize);
                        if !visited[nr][nc] && grid[nr][nc] == colour {
                            visited[nr][nc] = true;
                            queue.push_back((nr, nc));
                        }
                    }
                }
                if group.len() >= 4 {
                    found = true;
                    for (r, c) in group {
                        to_clear[r][c] = true;
                    }
                }
            }
        }
        if !found {
            break;
        }

        for row in 0..H {
            for col in 0..W {
                if grid[row][col] != b'0' {
                    continue;
                }
                for d in 0..4 {
                    let nr = row as isize + DR[d];
                    let nc = col as isize + DC[d];
                    if nr < 0 || nr >= H as isize || nc < 0 || nc >= W as isize {
                        continue;
                    }
                    if to_clear[nr as usize][nc as usize] {
                        to_clear[row][col] = true;
                        break;
                    }
                }
            }
        }
        for row in 0..H {
            for col in 0..W {
                if to_clear[row][col] {
                    grid[row][col] = b'.';
                    out.cleared += 1;
                }
            }
        }
        out.chains += 1;
        gravity(grid);
    }
    out
}

fn height(grid: &Grid) -> usize {
    (0..W)
        .map(|col| (0..H).filter(|&r| grid[r][col] != b'.').count())
        .max()
        .unwrap_or(0)
}

fn evaluate(grid: &Grid, col: usize, top: u8, bottom: u8) -> Option<i32> {
    let mut trial = grid.clone();
    drop_pair(&mut trial, col, top, bottom).ok()?;
    let out = resolve(&mut trial);
    Some(
        (out.cleared as i32) * 100 + (out.chains as i32) * 50 - (height(&trial) as i32) * 2,
    )
}

fn choose_column(grid: &Grid, top: u8, bottom: u8) -> usize {
    // `>` (et non max_by_key) conserve le premier score maximal : départage
    // déterministe vers la colonne la plus à gauche, comme dans les 3 autres langues.
    let mut best_col = 0;
    let mut best_score: Option<i32> = None;
    for col in 0..W {
        if let Some(score) = evaluate(grid, col, top, bottom) {
            if best_score.is_none_or(|best| score > best) {
                best_col = col;
                best_score = Some(score);
            }
        }
    }
    best_col
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    // Lecture ligne par ligne : CodinGame stream les tours, read_to_string()
    // bloquerait en attendant la fin du flux.
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

    // Une entree complete = un tour : on joue puis on recommence.
    loop {
        // Nombre de paires variable : on lit jusqu'a la ligne de score (1 seul entier).
        let mut first: Option<(u8, u8)> = None;
        loop {
            let pair_line = match next()? {
                Some(line) => line,
                None => return Ok(()), // fin du flux
            };
            let parts: Vec<&str> = pair_line.split_whitespace().collect();
            if parts.len() >= 2 {
                if first.is_none() {
                    first = Some((parts[0].as_bytes()[0], parts[1].as_bytes()[0]));
                }
                continue;
            }
            break; // ligne de score consommee
        }
        let (top, bottom) = match first {
            Some(pair) => pair,
            None => return Ok(()), // fin du flux
        };

        let mut grid = vec![vec![b'.'; W]; H];
        for row in 0..H {
            let line = match next()? {
                Some(line) => line,
                None => return Ok(()),
            };
            for (col, ch) in line.as_bytes().iter().take(W).enumerate() {
                grid[row][col] = *ch;
            }
        }
        if next()?.is_none() {
            return Ok(()); // score adverse
        }
        for _ in 0..H {
            if next()?.is_none() {
                return Ok(()); // grille adverse
            }
        }

        writeln!(out, "{}", choose_column(&grid, top, bottom))?;
        out.flush()?;
    }
}
