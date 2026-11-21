// Wondev Woman — arbitre version 2 (2 unités, PUSH&BUILD possible).
//
// Protocole vérifié sur le stub officiel et des test games réels :
//   init  : size, unitsPerPlayer (= 2 ici, malgré l'énoncé archivé qui dit 1)
//   tour  : size lignes de grille ('.' = trou, '0'..'3' = hauteur),
//           unitsPerPlayer lignes "x y" pour nous, autant pour l'adversaire,
//           puis legalActions, puis legalActions lignes
//           "atype index dir1 dir2".
//   sortie : UNE seule ligne. Le referee ne lit que outputs[0] : on joue la
//            meilleure action parmi TOUTES les unités, une seule par tour.
//
// Décision : marcher sur une case de hauteur 3 marque 1 point (referee :
// `if (targetHeight == FINAL_HEIGHT - 1) result.scorePoint = true`). On
// construit donc des tremplins 2 -> 3 à côté de notre unité, on reste adjacent
// aux cases de hauteur 3, et on détruit (3 -> 4) celles dont l'adversaire
// pourrait profiter.

use std::io::{self, BufRead, Write};

// Ordre identique au Python : N, NE, E, SE, S, SW, W, NW.
const DIR_NAMES: [&str; 8] = ["N", "NE", "E", "SE", "S", "SW", "W", "NW"];
const DX: [i32; 8] = [0, 1, 1, 1, 0, -1, -1, -1];
const DY: [i32; 8] = [-1, -1, 0, 1, 1, 1, 0, -1];

fn dir_index(name: &str) -> usize {
    DIR_NAMES.iter().position(|&n| n == name).unwrap_or(0)
}

/// Hauteur d'une case : -1 si trou / hors plateau.
fn height(grid: &[Vec<i32>], x: i32, y: i32, size: usize) -> i32 {
    if x < 0 || y < 0 || x >= size as i32 || y >= size as i32 {
        -1
    } else {
        grid[y as usize][x as usize]
    }
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();
    let mut next = || -> Result<Option<String>, Box<dyn std::error::Error>> {
        match lines.next() {
            Some(line) => Ok(Some(line?)),
            None => Ok(None),
        }
    };

    let size: usize = match next()? {
        Some(line) => line.trim().parse()?,
        None => return Ok(()),
    };
    let units_per_player: usize = match next()? {
        Some(line) => line.trim().parse()?,
        None => return Ok(()),
    };

    let stdout = io::stdout();
    let mut out = stdout.lock();
    loop {
        let mut grid = Vec::with_capacity(size);
        for _ in 0..size {
            let line = match next()? {
                Some(line) => line,
                None => return Ok(()),
            };
            grid.push(
                line.trim()
                    .chars()
                    .map(|c| if c == '.' { -1 } else { c as i32 - '0' as i32 })
                    .collect::<Vec<_>>(),
            );
        }

        let mut ours: Vec<(i32, i32)> = Vec::with_capacity(units_per_player);
        for _ in 0..units_per_player {
            let line = match next()? {
                Some(line) => line,
                None => return Ok(()),
            };
            let p: Vec<i32> = line
                .split_whitespace()
                .map(|v| v.parse())
                .collect::<Result<_, _>>()?;
            if p.len() < 2 {
                return Ok(());
            }
            ours.push((p[0], p[1]));
        }
        let mut foes: Vec<(i32, i32)> = Vec::with_capacity(units_per_player);
        for _ in 0..units_per_player {
            let line = match next()? {
                Some(line) => line,
                None => return Ok(()),
            };
            let p: Vec<i32> = line
                .split_whitespace()
                .map(|v| v.parse())
                .collect::<Result<_, _>>()?;
            if p.len() < 2 {
                return Ok(());
            }
            foes.push((p[0], p[1]));
        }

        let legal_count: usize = match next()? {
            Some(line) => line.trim().parse()?,
            None => return Ok(()),
        };

        // Même départage que Python/C/C++ : la première action avec un score
        // strictement supérieur gagne (les égalités gardent la première).
        let mut best: Option<(i64, String)> = None;
        for _ in 0..legal_count {
            let line = match next()? {
                Some(line) => line,
                None => return Ok(()),
            };
            let p: Vec<&str> = line.split_whitespace().collect();
            if p.len() < 4 {
                return Ok(());
            }
            let atype = p[0];
            let index: usize = match p[1].parse() {
                Ok(v) => v,
                Err(_) => continue,
            };
            if index >= ours.len() {
                continue;
            }
            let a = dir_index(p[2]);
            let b = dir_index(p[3]);
            let (ux, uy) = ours[index];
            let (mx, my) = (ux + DX[a], uy + DY[a]);
            let h_dest = height(&grid, mx, my, size);
            let (cx, cy) = (mx + DX[b], my + DY[b]);
            let h_build = height(&grid, cx, cy, size);

            let mut score: i64 = 0;
            if h_dest == 3 {
                score += 1_000_000; // marche sur une case niveau 3 = 1 point
            } else if h_dest >= 0 {
                score += 400 * i64::from(h_dest);
            }
            if h_dest >= 0 {
                for k in 0..8 {
                    if height(&grid, mx + DX[k], my + DY[k], size) == 3 {
                        score += 1500; // adjacent à une case de score
                        break;
                    }
                }
            }
            if h_build == 3 {
                let near_foe = foes
                    .iter()
                    .any(|&(fx, fy)| (cx - fx).abs() <= 1 && (cy - fy).abs() <= 1);
                score += if near_foe { 50_000 } else { -40 };
            } else if h_build == 2 {
                score += if (cx - mx).abs() + (cy - my).abs() <= 1 {
                    3000
                } else {
                    500
                };
            } else if h_build == 1 {
                score += 100;
            } else if h_build == 0 {
                score += 10;
            }

            let token = format!("{} {} {} {}", atype, index, DIR_NAMES[a], DIR_NAMES[b]);
            if best.as_ref().map_or(true, |cur| score > cur.0) {
                best = Some((score, token));
            }
        }

        // `WAIT` est invalide dans cette version : concéder proprement.
        let answer = best.map_or_else(|| "ACCEPT-DEFEAT".to_string(), |(_, token)| token);
        writeln!(out, "{}", answer)?;
        out.flush()?;
    }
}
