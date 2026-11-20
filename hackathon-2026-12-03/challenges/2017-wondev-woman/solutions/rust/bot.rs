// Wondev Woman Wood 3 — meilleure action légale : victoire, déni, montée.
use std::io::{self, BufRead, Write};

fn delta(d: &str) -> (i32, i32) {
    match d {
        "N" => (0, -1),
        "NE" => (1, -1),
        "E" => (1, 0),
        "SE" => (1, 1),
        "S" => (0, 1),
        "SW" => (-1, 1),
        "W" => (-1, 0),
        _ => (-1, -1), // NW
    }
}

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
    let _up: usize = match next()? {
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
        let pos: Vec<i32> = match next()? {
            Some(line) => line.split_whitespace().map(|v| v.parse()).collect::<Result<_, _>>()?,
            None => return Ok(()),
        };
        let (ux, uy) = (pos[0], pos[1]);
        let opp: Vec<i32> = match next()? {
            Some(line) => line.split_whitespace().map(|v| v.parse()).collect::<Result<_, _>>()?,
            None => return Ok(()),
        };
        let (ox, oy) = (opp[0], opp[1]);
        let n: usize = match next()? {
            Some(line) => line.trim().parse()?,
            None => return Ok(()),
        };

        let mut best_score: i64 = -1_000_000_000;
        let mut best: Option<(String, String)> = None;
        for _ in 0..n {
            let line = match next()? {
                Some(line) => line,
                None => return Ok(()),
            };
            let p: Vec<&str> = line.split_whitespace().collect();
            let (d1s, d2s) = (p[2], p[3]);
            let (dx, dy) = delta(d1s);
            let (bx, by) = delta(d2s);
            let (mx, my) = (ux + dx, uy + dy);
            let h_dest = height(&grid, mx, my, size);
            let (cx, cy) = (mx + bx, my + by);
            let h_build = height(&grid, cx, cy, size);

            let mut score: i64 = 0;
            if h_dest == 3 {
                score += 1_000_000;
            }
            if h_dest < 0 {
                score -= 100_000;
            } else {
                score += 1000 * i64::from(h_dest);
            }
            if h_build == 3 {
                if ox >= 0 && (cx - ox).abs() <= 1 && (cy - oy).abs() <= 1 {
                    score += 50_000;
                } else {
                    score -= 50;
                }
            } else if h_build == 2 {
                score += 300;
            } else if h_build == 1 {
                score += 100;
            }

            if score > best_score {
                best_score = score;
                best = Some((d1s.to_string(), d2s.to_string()));
            }
        }
        match best {
            None => writeln!(out, "WAIT")?,
            Some((d1, d2)) => writeln!(out, "MOVE&BUILD 0 {d1} {d2}")?,
        }
        out.flush()?;
    }
}
