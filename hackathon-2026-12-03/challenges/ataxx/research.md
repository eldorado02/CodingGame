# Research — Ataxx (CodinGame, port of arcade Ataxx)

Deep search performed 2026-11-21. Sources are public/free. Hypothesis status is
explicit: a public strategy is a *hypothesis*, not a proof, until a test game
against the boss confirms it. (Test games were NOT run in this session —
see `critique.md` risks.)

## 1. Official sources

- **Official statement** — `python3 bot-programming/tools/cg.py show ataxx`
  → `https://www.codingame.com/api/v1/puzzles/ataxx`. Rules copied verbatim to
  `statement.md`. Key mechanical fact: **the referee supplies the list of legal
  actions** (`actionsCount` + one action per line). The bot does *not* need to
  generate moves; it only has to *choose* one. This removes the single largest
  source of portability bugs (move generation).
- **Opponents / boss** — `python3 bot-programming/tools/cg.py opponents ataxx`
  → `https://www.codingame.com/api/v1/puzzles/ataxx/opponents`.
  Current league **Wood 2**, boss = agent_id **6649970** ("BOSS 1").
  (Seeded opponents with ids 6649970, 6649992 and others appear in the same
  league; boss flag is on 6649970.)

## 2. Game mechanics (free sources, URLs)

| URL | What it confirmed / idea kept |
|---|---|
| https://en.wikipedia.org/wiki/Ataxx | 7x7 board, clone (distance 1, source kept) vs jump (distance 2, source vacated), Chebyshev distance, conversion of all adjacent enemy pieces, pass when no move, threefold repetition optional. Matches the statement. |
| https://www.pressibus.org/ataxx/gen/gbblocs.html | The 596 starting board configurations referenced by the statement (walls). Confirms walls are part of the initial position. |
| http://www.pressibus.org/ataxx/gen/gborigines.html | Origins of Infection/Ataxx (cited by Wikipedia). Historical, no algorithm. |
| https://www.ataxx.org/ | Ataxx AI community (link from Wikipedia). Attempted, transport error; not relied on. |

## 3. Public solution repositories (read the code, not just the README)

Queried `https://api.github.com/search/repositories?q=ataxx+in:name&sort=stars`.

| URL / repo | Language | Idea kept | Idea rejected / why |
|---|---|---|---|
| https://github.com/GeckoX94/Ataxx (`algorithme/heuristics.py`, read in full) | Python | Heuristic ladder **v1** piece difference `5*(A-B)`; **v2** adds center-control weights `WEIGHTS[idx]`; **v3** adds *local pressure* (+1 per enemy adjacent to my piece, symmetric); **v4** adds clustering + game-phase weighting. Our evaluation is an integer-only v2+v3 variant (see `plan.md`). | Alpha-Beta depth 4 / Zobrist transposition tables / iterative deepening / killer moves: too expensive for the 100 ms/turn budget and hard to port identically to 4 languages for the *initial* bot. Deferred to `plan.md` improvements. |
| https://github.com/curtisbright/ataxx-ai | C++ | Search-based Ataxx AI exists and is the most-starred result — confirms minimax is the standard approach. | Full engine not copied: we consume the referee's legal-action list instead. |
| https://github.com/QtLab/Ataxx | C++ | Minimax AI with high rank on botzone — supports piece-count + positional evaluation. | Qt UI / botzone protocol irrelevant. |
| https://github.com/petersn/AtaxxZero | Python | AlphaZero-style approach noted for future work. | Needs training, impossible within campaign budget. |
| https://github.com/mingi-sid/ataxx-ai | Jupyter | Modified AlphaZero (KAIST-POSTECH) — confirms MCTS/NN is the high-end ceiling. | Not reproducible here. |
| https://github.com/kz04px/python-ataxx | Python | Clean rules engine reference for move application / conversion. | Library API only, no bot. |
| https://github.com/feifeiobama/AtaxxBot | C++ | Topics `alpha-beta-pruning`, `mcts` — search baseline. | Not read in full; hypothesis only. |

## 4. Adapted idea for THIS challenge (hypothesis → implementation)

Because the referee hands us the exact legal actions, a deterministic **1-ply
greedy** over those actions is sufficient for a correct, reproducible initial
bot. For each offered action we simulate it and score the resulting board with
an integer evaluation inspired by GeckoX94 v2+v3:

- terminal: opponent wiped out → +1000000, self wiped out → -1000000;
- `+20 * (myPieces - oppPieces)` (piece difference dominates);
- center-control weight matrix (same numbers as GeckoX94 `WEIGHTS`);
- local pressure: +1 per enemy adjacent to each of my pieces, symmetric.

Deterministic tie-break: highest evaluation wins; on equality the
**lexicographically smallest ASCII action string** wins. This is byte-equal in
Python (`str <`), C (`strcmp`), C++ (`std::string <`) and Rust (`str Ord`) for
the ASCII action alphabet (`a`–`g`, `1`–`7`).

## 5. Contradictions / trust rules

- No source contradicts the statement. Where public Ataxx pages describe the
  "four corners" start, the CodinGame statement says a board is chosen among
  596 configurations (with walls). **We trust the statement/referee**: the
  input board is whatever the referee sends; the bot makes no start-position
  assumption.
- The 300-turn draw and threefold repetition are referee-side rules; the bot
  does not model them in v1 (see `critique.md`).
