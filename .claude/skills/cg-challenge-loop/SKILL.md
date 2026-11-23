---
name: cg-challenge-loop
description: Full improvement loop for one CodinGame bot-programming game of this repo's Legend campaign (status, README, statement, opencode research, plan, critique, 4-language code, differential fuzz, test games, C++ submit, logs, commit+push). Use when asked to work on, climb, improve or submit any game listed in bot-programming/games.json, or to "continue the campaign".
---

# CodinGame challenge loop

Repo root: `/home/arafaram/Projects/codingGame`. Never print or commit
`~/.config/codingame/token`. If the user says "arrête tout": stop at once.

## 0. Pick the game
1. `python3 bot-programming/tools/status.py --only <ids>` (or full scan,
   ~4 min) → `bot-programming/status.json`. A game is DONE when our league =
   league of leaderboard rank 1 (single-league games: DONE once scored).
2. Order: (a) single-league games without submission (fast DONE),
   (b) smallest `gap` to boss, (c) community games below their top league,
   (d) contests far from Legend. One game at a time.

## 1. Before any code
1. Read/write `hackathon-2026-12-03/challenges/<id>/README.md`. New game:
   `bash hackathon-2026-12-03/templates/new_challenge.sh <id> [board-2p]`.
2. Re-read the statement: `python3 bot-programming/tools/cg.py show <id>` and
   `stub <id>` (rules change per league).
3. Read `bot-programming/ALGO.md` → classify the game, pick the approach.
4. Read the challenge `critique.md` — do not redo a rejected experiment.

## 2. Research (opencode only, never Claude subagents)
`bash bot-programming/tools/research.sh <id> "<precise question>"` — runs the
free-model chain in `opencode`. Run it with `run_in_background` when long.
Verify every claim it makes against a primary source (referee, statement,
test-game log) before relying on it. Merge useful facts + all URLs into
`research.md`. For small lookups use WebSearch/WebFetch directly.

## 3. Plan + critique
Fill `plan.md` (protocol, state invariant, algorithm + budget, deterministic
tie-break, fuzz matrix, win hypothesis + refutation signal) and `critique.md`
(PLAYBOOK §3 checklist). Improve the plan when a loss reveals a gap.

## 4. Code (Claude writes it — no coding subagents)
Python reference first, then C, C++, Rust, same logic. Rules in ALGO.md §0.
Rust `target/` and binaries never committed.

## 5. Validate
1. `bash hackathon-2026-12-03/challenges/<id>/tests/run.sh` → 200/200.
2. Read `d['limits']` (`cg.py get <id> progress`) before tests.
3. Opponents: `cg.py opponents <id>` (boss = `league_boss` from progress).
   Exact opponent count per game (see ALGO.md §0.6).
4. `cg.py test <id> .../solutions/cpp/bot.cpp --language C++ --opponents <id>`
   5 games, fixed seeds when comparing variants
   (`--extra '{"game_options":"seed=111111111"}'`). Need ≥3/5 vs boss
   (single-league / no boss: ≥3/5 vs a mid-table player).
5. Analyse each loss in `bot-programming/games/<id>/logs/` (scores, summary,
   stderr TIMEGUARD) before changing anything. One change at a time.

## 6. Submit + log
1. `cg.py submit <id> .../solutions/cpp/bot.cpp --language C++`.
2. Log in `bot-programming/IMPROVEMENTS.md` + challenge `critique.md`:
   constat / hypothèse / sources / action / résultat / leçon (failures too).
3. Update `bot-programming/PROGRESS.md`, README, and ALGO.md §6 if a pattern
   repeats on ≥2 games.
4. Commit `<Jeu> : description (fuzz 200/200, x/5), submission <id>` and
   `git push origin main`. Then poll progress later (ranking takes minutes);
   after promotion restart at step 1 for the new league.
