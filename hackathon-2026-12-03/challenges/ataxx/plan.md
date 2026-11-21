# Plan — Ataxx (Wood 2, boss 6649970)

Status: **implemented (v1 greedy)**. This document was written before the code
per the playbook; it is the contract the four implementations must satisfy.

## 1. Exact I/O protocol

**Initialization (once)** — one line:
- `id` : integer, `0` or `1`. My internal piece symbol is `id + 1`
  (`0→1`, `1→2`); opponent symbol is `3 - mySymbol`.

**Every turn** — read in this exact order:
1. seven lines of 7 characters, **bottom to top** (line 0 = row `1`, y=0;
   line 6 = row `7`, y=6), character `j` = column `j` (`a`..`g`, x=0..6).
   Chars: `.`→0 (empty), `0`→1 (player 0 piece), `1`→2 (player 1 piece),
   `#`→3 (wall). Unknown chars → 0 (defensive default).
2. one line `lastAction` : a token (`null`, `pass`, or actions joined by `;`).
   Consumed but not used in the decision.
3. one line `actionsCount` : integer ≥ 0.
4. `actionsCount` lines, each a legal-action string:
   - spawn: 2 chars, `[a-g][1-7]`, e.g. `b7` → x=`b`-`a`, y=`7`-`1`.
   - jump: 4 chars, `[a-g][1-7][a-g][1-7]`, e.g. `a7c7` → src(a7), dst(c7).

**Output** — exactly **one line per turn**:
- the chosen action string verbatim, or
- `random` if `actionsCount == 0` / no parseable action.

An optional trailing message is legal in the game but we emit none (keeps the
differential byte-exact). The program loops: after one turn it tries to read
the next initialization? **No** — initialization is read once; the loop re-reads
only the per-turn block. With a single fuzz case, the second iteration hits EOF
and exits cleanly.

## 2. State invariant

- Recomputed **every turn**, nothing carried across turns: the 49-cell board,
  the action list, and the chosen action. No history, no transposition table,
  no RNG. This makes every turn independent and trivially reproducible.
- Coordinate mapping is invariant: index = `y*7 + x`, y from bottom, x from
  left. The center-weight matrix is symmetric under both axis flips, so this
  convention cannot change the evaluation.
- Simulation operates on a **copy** of the board; the original is never
  mutated between candidate evaluations.

## 3. Algorithm, complexity and time budget

For each of the `N = actionsCount` offered actions:
1. copy 49 ints;
2. if jump (4 chars) clear source; set destination to my symbol;
3. convert every `opp` neighbor (8-neighborhood) of the destination to mine;
4. evaluate the resulting board.

Evaluation (integer only, from my perspective):
```
if myCount == 0: return -1000000
if oppCount == 0: return +1000000
score = 20*(myCount - oppCount)
for i in 0..48:
    if cell == me:  score += W[i] + #{neighbors == opp}
    if cell == opp: score -= W[i] + #{neighbors == me}
return score
```
`W` = GeckoX94 center-control weights (1..6, center 6).

Complexity per turn: `O(N * 49 * 8)` ≈ `N * 392` integer ops. Worst realistic
`N` ≈ 200 (near-empty late board) → ~78k ops. Divide by 3 for the Python
safety margin → ~26k "Python ops" equivalent, far under the 100 ms budget
(first turn 1000 ms). No search, no allocation growth beyond the action list.

## 4. Deterministic tie-break

Choose candidate with maximal `eval`. If two candidates have equal `eval`,
choose the one whose action string is **smallest in ASCII byte order**
(`strcmp` order). Action strings contain only `a`–`g` and `1`–`7`, so Python,
C, C++, Rust all agree byte-for-byte. No floating point anywhere in the
decision path.

## 5. Test matrix (covered by `tests/gen_fuzz.py` + fixed cases)

| Case | Covered by generator |
|---|---|
| nominal board with walls + both colors | yes |
| only spawns / only jumps / mixed | yes |
| max-density board (few empties) | yes (parameterized fill) |
| no legal actions → `random` | yes (forced no-action cases) |
| jump that captures 0 / 1 / several neighbors | yes (random boards) |
| equal-evaluation actions → ASCII tie-break | yes (fuzz + fixed cases) |
| `lastAction` = `null` / `pass` / `x;y` | yes |
| player id 0 and 1 | yes |
| malformed/short action token | defensive: clamp/skip, covered by fixed cases |
| 300-turn draw / threefold repetition | **not** modeled (referee-side) |

## 6. Victory hypothesis

> "Choosing the action that maximizes material + center control + local
> pressure will out Score the Wood-2 boss on piece count at game end."

Signal that falsifies it: test games against boss **6649970** ending with a
negative final piece differential, or repeated losses where the log shows the
boss growing faster despite our per-move eval being higher. Next lever if
falsified: add a 2-ply minimax on the referee-provided actions (opponent model)
with the same evaluation, still inside 100 ms.

## Implementation order
1. Python reference (`solutions/python/bot.py`).
2. Port logic to C, C++, Rust with **identical** W, neighbor offsets, eval,
   tie-break.
3. `tests/gen_fuzz.py` (200 reproducible cases) + `tests/run.sh` (4-way
   differential). Iterate until 210/210 (10 fixed + 200 fuzz) match.
4. Only then consider test games / submissions — **not in this session**.
