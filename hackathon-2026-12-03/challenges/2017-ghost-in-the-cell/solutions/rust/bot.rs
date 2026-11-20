// Ghost in the Cell Wood 3 — forces effectives + meilleure action globale.
use std::io::{self, BufRead, Write};

const INF: i32 = 1_000_000;
const RESERVE: i32 = 5;

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();
    let mut next = || -> Result<Option<String>, Box<dyn std::error::Error>> {
        match lines.next() {
            Some(line) => Ok(Some(line?)),
            None => Ok(None),
        }
    };
    let n: usize = match next()? {
        Some(line) => line.trim().parse()?,
        None => return Ok(()),
    };
    let m: usize = match next()? {
        Some(line) => line.trim().parse()?,
        None => return Ok(()),
    };
    let mut dist = vec![vec![INF; n]; n];
    for i in 0..n {
        dist[i][i] = 0;
    }
    for _ in 0..m {
        let line = match next()? {
            Some(line) => line,
            None => return Ok(()),
        };
        let v: Vec<i32> = line.split_whitespace().map(|x| x.parse()).collect::<Result<_, _>>()?;
        let (a, b, d) = (v[0] as usize, v[1] as usize, v[2]);
        dist[a][b] = d;
        dist[b][a] = d;
    }
    for k in 0..n {
        for i in 0..n {
            for j in 0..n {
                dist[i][j] = dist[i][j].min(dist[i][k] + dist[k][j]);
            }
        }
    }

    let stdout = io::stdout();
    let mut out = stdout.lock();
    loop {
        let count: usize = match next()? {
            Some(line) => line.trim().parse()?,
            None => return Ok(()),
        };
        let mut owner = vec![0i32; n];
        let mut cyborgs = vec![0i32; n];
        let mut prod = vec![0i32; n];
        let mut troops: Vec<(i32, usize, i32)> = Vec::new(); // owner, dst, count
        for _ in 0..count {
            let line = match next()? {
                Some(line) => line,
                None => return Ok(()),
            };
            let p: Vec<&str> = line.split_whitespace().collect();
            let id: usize = p[0].parse()?;
            let a: i32 = p[2].parse()?;
            let b: i32 = p[3].parse()?;
            let c: i32 = p[4].parse()?;
            if p[1] == "FACTORY" {
                owner[id] = a;
                cyborgs[id] = b;
                prod[id] = c;
            } else if p[1] == "TROOP" {
                // p = [id, type, owner, src, dst, count, eta]
                let dst: usize = p[4].parse()?;
                let cnt: i32 = p[5].parse()?;
                troops.push((a, dst, cnt));
            }
        }
        // Appliquer les troupes après toutes les usines (ordre arbitraire).
        for (own, dst, cnt) in troops {
            cyborgs[dst] += if own == 1 { cnt } else { -cnt };
        }
        for i in 0..n {
            if owner[i] == 0 {
                cyborgs[i] = 0;
            }
        }

        let mut best: Option<(i32, usize, usize, i32)> = None; // score, src, dst, count
        for i in 0..n {
            if owner[i] != 1 || cyborgs[i] >= 0 {
                continue;
            }
            for j in 0..n {
                if owner[j] != 1 || cyborgs[j] <= -cyborgs[i] + RESERVE {
                    continue;
                }
                let cnt = (cyborgs[j] - RESERVE).min(-cyborgs[i] + 2);
                let score = 1000 - dist[j][i];
                if best.is_none_or(|(bs, _, _, _)| score > bs) {
                    best = Some((score, j, i, cnt));
                }
            }
        }
        if best.is_none() {
            for src in 0..n {
                if owner[src] != 1 || cyborgs[src] <= RESERVE {
                    continue;
                }
                for dst in 0..n {
                    if owner[dst] == 1 || dist[src][dst] >= INF {
                        continue;
                    }
                    let need = cyborgs[dst] + 1;
                    if need <= 0 || cyborgs[src] - RESERVE < need {
                        continue;
                    }
                    let score = if owner[dst] == 0 { 100 } else { 0 }
                        + prod[dst] * 20 - dist[src][dst] * 2 - need;
                    if best.is_none_or(|(bs, _, _, _)| score > bs) {
                        best = Some((score, src, dst, need));
                    }
                }
            }
        }
        match best {
            None => writeln!(out, "WAIT")?,
            Some((_, src, dst, cnt)) => writeln!(out, "MOVE {src} {dst} {cnt}")?,
        }
        out.flush()?;
    }
}
