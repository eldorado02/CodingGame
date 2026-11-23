---
name: cg-board-game
description: Build a CodinGame bot for a two-player perfect-information board game (chess-like, Othello, Breakthrough, Hex, Amazons, Oware, Checkers...) from the repo template hackathon-2026-12-03/templates/board-2p in Python, C, C++ and Rust with byte-identical output. Use when the game is turn-based, 2 players, full information.
---

# Board game bot from `templates/board-2p`

## Steps
1. Scaffold: `bash hackathon-2026-12-03/templates/new_challenge.sh <id> board-2p`.
2. From `statement.md` write in `plan.md`: board encoding (index = `y*W+x`),
   move encoding as printed, whether legal moves are given in the input,
   turn-1 extra lines, time limits (first turn / others).
3. Replace only the `GAME:` blocks in the 4 files, same order everywhere:
   `State`, `winner/terminal`, `genMoves` (fixed order), `applyMove`
   (copy-make), `evaluate` (side-to-move POV), `orderKey`, I/O loop.
   Keep the ENGINE block untouched unless the upgrade is applied to all 4.
4. If the input gives the legal moves: parse them, and **only play a move
   that is in that list** (fallback = first listed move). Cross-check your
   generator against the list in the fuzz (mismatch = rule bug).
5. Fuzz generator: random legal playouts from the start position, cut at a
   random ply, emit the input stream of that turn (snapshot cases), plus
   edge cases (first move, one legal move, immediate win, immediate loss).
6. Calibrate `NODE_LIMIT` in C++ so the worst case stays ≤40 % of the CG
   limit locally (`/usr/bin/time`, stderr TIMEGUARD must never print).
7. Boss usually random/greedy in Wood: depth 4–6 αβ wins. To climb: add TT
   Zobrist, then better eval, then MCTS if branching > 100 (ALGO.md §2).

## Eval starters
- Material + mobility (`#my moves - #opp moves`) + positional table.
- Race games (Breakthrough): most advanced safe piece, defended pieces.
- Territory (Amazons, Isola, Blocking): Voronoi BFS difference.
- Connection (Hex, Twixt): shortest-path / two-distance difference.

## Pitfalls
- Python reference is slow: fuzz runs with `CG_NODES=20000` (env).
- Tie-breaks: strict `>`; stable sort; never iterate hash maps.
- `endl`/flush after every output line.
