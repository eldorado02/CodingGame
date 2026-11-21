# Suivi global — challenges `bot-programming` (2016 → 2026)

Objectif : **tous les jeux** de `bot-programming/games.json` (100), traités
séquentiellement, du plus ancien au plus récent, montés jusqu'en **Ligue Légende**.

## Procédure appliquée à chaque jeu

1. **Deep search** sous-agent — modèle `opencode/deepseek-v4.1-flash-free`
   (repli `longcat-2.5-free` → `hy4-flash-free` → `nemotron-3.5-super-vl-free`).
   Chercher : énoncé, métadonnées de ligue, dépôts GitHub de solutions,
   post-mortems, stratégies connues.
2. `statement.md`, `research.md`, `plan.md`, `critique.md` dans
   `hackathon-2026-12-03/challenges/<année>-<id>/`.
3. **Critique du plan** avant tout code.
4. **Implémentation 4 langages** sous-agent — même 1er modèle
   (repli `flint-40b-free` → `longcat-2.5-free` → `mimo-v2.6-flash-free`),
   socle DSA phase 1, code idiomatique par langue.
5. **Validation** : fuzz ≥200 cas différentiels 4 langages + test games réels
   (≥3/5 victoires contre le boss de ligue) avant soumission.
6. **Soumission** + surveillance du rang jusqu’à Légende ; à chaque promotion,
   relire l'énoncé de la nouvelle ligue et reprendre la boucle.

Quand un sous-agent atteint sa limite gratuite : descendre d'un rang dans la
chaîne, puis continuer manuellement dans la session principale ; dès que le
quota revient, relancer un sous-agent pour l'étape suivante.

## File d’attente chronologique

Ordre = inverse de `games.json` (la liste API est triée du plus récent au plus
ancien). `PHASE2` = bot documenté/testé/soumis en phase 2 (ligues à confirmer
et à monter ensuite jusqu’à Légende). `ACTIF` = travail en cours.

| # | pretty_id | Titre | État | Ligue cible |
|---|---|---|---|---|
| 1 | `tron-battle` | Tron Battle | À FAIRE | Légende |
| 2 | `game-of-drones` | Game of Drones | À FAIRE | Légende |
| 3 | `poker-chip-race` | Poker Chip Race | À FAIRE | Légende |
| 4 | `platinum-rift-episode-1` | Platinum Rift - Episode 1 | À FAIRE | Légende |
| 5 | `platinum-rift-episode-2` | Platinum Rift - Episode 2 | À FAIRE | Légende |
| 6 | `great-escape` | Great escape | À FAIRE | Légende |
| 7 | `back-to-the-code` | Back to the Code | À FAIRE | Légende |
| 8 | `mad-pod-racing` | Mad Pod Racing | À FAIRE | Légende |
| 9 | `smash-the-code` | Smash The Code | PHASE2 | Légende |
| 10 | `codebusters` | CodeBusters | PHASE2 | Légende |
| 11 | `hypersonic` | Hypersonic | PHASE2 | Légende |
| 12 | `fantastic-bits` | Fantastic Bits | PHASE2 | Légende |
| 13 | `ghost-in-the-cell` | Ghost in the Cell | PHASE2 | Légende |
| 14 | `coders-of-the-caribbean` | Coders of the Caribbean | PHASE2 | Légende |
| 15 | `wondev-woman` | Wondev Woman | PHASE2 | Légende |
| 16 | `mean-max` | Mean Max | PHASE2 | Légende |
| 17 | `code4life` | Code4Life | À FAIRE | Légende |
| 18 | `tic-tac-toe` | Ultimate Tic-Tac-Toe | À FAIRE | Légende |
| 19 | `botters-of-the-galaxy` | Botters of the Galaxy | PHASE2 | Légende |
| 20 | `code-royale` | Code Royale | PHASE2 | Légende |
| 21 | `code-of-kutulu` | Code of Kutulu | PHASE2 | Légende |
| 22 | `legends-of-code-magic` | Legends of Code & Magic | À FAIRE | Légende |
| 23 | `xmas-rush` | Xmas Rush | PHASE2 | Légende |
| 24 | `vindinium` | Vindinium | À FAIRE | Légende |
| 25 | `langton-s-ant` | Langton s Ant | À FAIRE | Légende |
| 26 | `checkers` | Checkers | À FAIRE | Légende |
| 27 | `yavalath` | Yavalath | À FAIRE | Légende |
| 28 | `code-a-la-mode` | Code a la Mode | PHASE2 | Légende |
| 29 | `cultist-wars` | Cultist Wars | PHASE2 | Légende |
| 30 | `bit-runner-2048` | Bit Runner 2048 | À FAIRE | Légende |
| 31 | `bandas` | Bandas | À FAIRE | Légende |
| 32 | `a-code-of-ice-and-fire` | A Code of Ice And Fire | PHASE2 | Légende |
| 33 | `oware-abapa` | Oware Abapa | À FAIRE | Légende |
| 34 | `breakthrough` | Breakthrough | À FAIRE | Légende |
| 35 | `paper-soccer` | Paper soccer | À FAIRE | Légende |
| 36 | `onitama` | Onitama | À FAIRE | Légende |
| 37 | `tower-dereference` | Tower Dereference | À FAIRE | Légende |
| 38 | `twixt-pp` | Twixt-PP | À FAIRE | Légende |
| 39 | `coders-of-the-realm---1v1` | Coders of the Realm - 1v1 | À FAIRE | Légende |
| 40 | `coders-of-the-realm` | Coders of the Realm | À FAIRE | Légende |
| 41 | `crystal-rush` | Crystal Rush | À FAIRE | Légende |
| 42 | `tulips-and-daisies` | Tulips and Daisies | À FAIRE | Légende |
| 43 | `ocean-of-code` | Ocean of Code | PHASE2 | Légende |
| 44 | `spring-challenge-2020` | Spring Challenge 2020 | PHASE2 | Légende |
| 45 | `yinsh` | Yinsh | À FAIRE | Légende |
| 46 | `othello-1` | Othello | À FAIRE | Légende |
| 47 | `atari-go` | Atari Go | À FAIRE | Légende |
| 48 | `dots-and-boxes` | Dots and Boxes | À FAIRE | Légende |
| 49 | `atari-go-9x9` | Atari Go 9x9 | À FAIRE | Légende |
| 50 | `penguins` | Penguins | À FAIRE | Légende |
| 51 | `blocking` | Blocking | À FAIRE | Légende |
| 52 | `fall-challenge-2020` | Fall Challenge 2020 | PHASE2 | Légende |
| 53 | `chess` | Chess | À FAIRE | Légende |
| 54 | `tryangle-catch` | TryAngle Catch | À FAIRE | Légende |
| 55 | `fireworks` | Fireworks | À FAIRE | Légende |
| 56 | `connect-4` | Connect 4 | À FAIRE | Légende |
| 57 | `clobber` | Clobber | À FAIRE | Légende |
| 58 | `night-of-war` | Night Of War | À FAIRE | Légende |
| 59 | `spring-challenge-2021` | Spring Challenge 2021 | PHASE2 | Légende |
| 60 | `amazons` | Amazons | À FAIRE | Légende |
| 61 | `dice-duel` | Dice Duel | À FAIRE | Légende |
| 62 | `volcanoes` | Volcanoes | À FAIRE | Légende |
| 63 | `abalone` | Abalone | À FAIRE | Légende |
| 64 | `space-shooter` | Space Shooter | À FAIRE | Légende |
| 65 | `clash-of-bots` | Clash of Bots | À FAIRE | Légende |
| 66 | `counting-tictactoe` | Counting TicTacToe | À FAIRE | Légende |
| 67 | `lines-of-action` | Lines Of Action | À FAIRE | Légende |
| 68 | `nine-mens-morris` | nine men's morris | À FAIRE | Légende |
| 69 | `code-keeper---the-hero` | Code Keeper - The Hero | À FAIRE | Légende |
| 70 | `start-up` | Start-up! | À FAIRE | Légende |
| 71 | `spring-challenge-2022` | Spider Attack | PHASE2 | Légende |
| 72 | `legends-of-code-magic-constructed` | Legends of Code & Magic: Constructed | À FAIRE | Légende |
| 73 | `green-circle` | Green Circle | PHASE2 | Légende |
| 74 | `isola` | Isola | À FAIRE | Légende |
| 75 | `6-nimmt-6-qui-prend-take-5` | 6 nimmt! / 6 qui prend! / Take 5! | À FAIRE | Légende |
| 76 | `git-patchwork` | Git Patchwork | À FAIRE | Légende |
| 77 | `impasse` | Impasse | À FAIRE | Légende |
| 78 | `keep-off-the-grass-fall-challenge-2022` | Keep Off The Grass | À FAIRE | Légende |
| 79 | `spring-challenge-2023-ants` | Spring Challenge 2023 - Fourmis | À FAIRE | Légende |
| 80 | `mad-knights` | Mad Knights | À FAIRE | Légende |
| 81 | `poker` | Poker | À FAIRE | Légende |
| 82 | `seabed-security` | Seabed Security - Fall Challenge 2023 | À FAIRE | Légende |
| 83 | `minishogi` | Minishogi | À FAIRE | Légende |
| 84 | `dice-shogi` | Dice Shogi | À FAIRE | Légende |
| 85 | `summer-challenge-2024-olymbits` | Olymbits - Summer Challenge 2024 | ACTIF | Légende |
| 86 | `hex` | Hex | À FAIRE | Légende |
| 87 | `gargoyles-versus-santas` | Gargoyles versus Santas | À FAIRE | Légende |
| 88 | `winter-challenge-2024` | Cellularena - Winter Challenge 2024 | PHASE2 | Légende |
| 89 | `domain-expansion` | Domain Expansion | À FAIRE | Légende |
| 90 | `game-of-life-or-death` | Game of Life or Death | À FAIRE | Légende |
| 91 | `soak-overflow` | Soak Overflow - Summer Challenge 2025 | À FAIRE | Légende |
| 92 | `beeminegame` | BeeMineGame | À FAIRE | Légende |
| 93 | `winter-challenge-2026-snakebyte` | SnakeByte - Winter Challenge 2026 | À FAIRE | Légende |
| 94 | `spring-challenge-2026-troll-farm` | Troll Farm - Spring Challenge 2026 | PHASE2 | Légende |
| 95 | `chain-reaction-1` | Chain Reaction | À FAIRE | Légende |
| 96 | `ataxx` | Ataxx | À FAIRE | Légende |
| 97 | `elemental-wars` | Elemental Wars | À FAIRE | Légende |
| 98 | `summer-challenge-2026-back-track-king` | Back Track King — Summer Challenge 2026 | PHASE2 | Légende |
| 99 | `brandubh` | Brandubh | À FAIRE | Légende |
| 100 | `shobu` | Shobu | À FAIRE | Légende |

## Reprise automatique

- Toujours reprendre au **premier** `À FAIRE` (ou `ACTIF`) de ce tableau.
- Cocher `FAIT — Légende` uniquement après vérification `GET /puzzles/<id>/progress`.
- Commiter/pousser après chaque challenge validé et soumis.
- Ne jamais committer de token, ni `bot_course.py` / `autoplay_state.json`.
