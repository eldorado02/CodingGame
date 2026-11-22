# Suivi global — challenges `bot-programming` (2016 → 2026)

Objectif : **tous les 100 jeux** de `bot-programming/games.json` atteignent
**la Ligue Légende** (Gold n’est qu’une étape). Boucle tournante : dès qu’un
challenge monte de ligue, on passe aux autres puis on revient. Un seul challenge
actif à la fois, dans l’ordre de priorité ci-dessous.

**Campagne en cours : Légende pour tous. 4 langages codés, version C++ soumise.**

## Règles du jeu

- Deep search (`deepseek-v4.1-flash-free` → `longcat-2.5-free` → `hy4-flash-free` → `nemotron-3.5-super-vl-free`), GitHub + arXiv + referee, puis `plan.md` et `critique.md`.
- Implémentation 4 langages (`deepseek-v4.1-flash-free` → `flint-40b-free` → `longcat-2.5-free` → `mimo-v2.6-flash-free`).
- Fuzz ≥200 cas + ≥3/5 test games contre le boss avant soumission.
- **Lire/écrire le README du challenge avant toute modification** (`PLAYBOOK.md` §8).
- Journaliser chaque amélioration dans `IMPROVEMENTS.md`.
- À chaque promotion : relire l’énoncé de la nouvelle ligue et relancer la boucle.
- Reprise automatique au premier `À FAIRE` quand le quota sous-agent revient.

## Priorité de traitement

Les jeux déjà soumis sont classés par ligue courante décroissante (Silver
avant Bronze, etc.), puis par rang de ligue croissant. Les jeux sans
soumission suivent, du plus ancien au plus récent.

| # | pretty_id | Titre | Ligue actuelle | Rang ligue | État | Cible |
|---|---|---|---|---|---|---|
| 1 | `a-code-of-ice-and-fire` | A Code of Ice And Fire | Silver | 170/323 | À AMÉLIORER | Legend |
| 2 | `summer-challenge-2026-back-track-king` | Back Track King — Summer Challenge 2026 | Silver | 483/720 | À AMÉLIORER | Legend |
| 3 | `spring-challenge-2026-troll-farm` | Troll Farm - Spring Challenge 2026 | Bronze | 3/402 | À AMÉLIORER | Legend |
| 4 | `summer-challenge-2024-olymbits` | Olymbits - Summer Challenge 2024 | Silver | 2/1598 | À AMÉLIORER | Legend |
| 5 | `xmas-rush` | Xmas Rush | Silver | 184/254 | À AMÉLIORER | Legend |
| 6 | `cultist-wars` | Cultist Wars | Bronze | 76/266 | À AMÉLIORER | Legend |
| 7 | `ocean-of-code` | Ocean of Code | Bronze | 169/443 | À AMÉLIORER | Legend |
| 8 | `code-of-kutulu` | Code of Kutulu | Bronze | 294/678 | À AMÉLIORER | Legend |
| 9 | `mean-max` | Mean Max | Bronze | 460/1290 | À AMÉLIORER | Legend |
| 10 | `code-royale` | Code Royale | Wood 3 | 3/766 | À AMÉLIORER | Legend |
| 11 | `botters-of-the-galaxy` | Botters of the Galaxy | Wood 3 | 153/321 | À AMÉLIORER | Legend |
| 12 | `codebusters` | CodeBusters | Wood 2 | 1/310 | À AMÉLIORER | Legend |
| 13 | `winter-challenge-2024` | Cellularena - Winter Challenge 2024 | Wood 2 | 12/158 | À AMÉLIORER | Legend |
| 14 | `code-a-la-mode` | Code a la Mode | Wood 2 | 191/360 | À AMÉLIORER | Legend |
| 15 | `coders-of-the-caribbean` | Coders of the Caribbean | Wood 2 | 253/1858 | À AMÉLIORER | Legend |
| 16 | `hypersonic` | Hypersonic | Wood 2 | 356/442 | À AMÉLIORER | Legend |
| 17 | `spring-challenge-2022` | Spider Attack | Wood 1 | 74/993 | À AMÉLIORER | Legend |
| 18 | `fantastic-bits` | Fantastic Bits | Wood 1 | 152/816 | À AMÉLIORER | Legend |
| 19 | `green-circle` | Green Circle | Wood 1 | 188/303 | À AMÉLIORER | Legend |
| 20 | `wondev-woman` | Wondev Woman | Bronze | 82/188 | À AMÉLIORER | Legend |
| 21 | `spring-challenge-2021` | Spring Challenge 2021 | Wood 1 | 314/580 | À AMÉLIORER | Legend |
| 22 | `smash-the-code` | Smash The Code | Wood 1 | 653/1161 | À AMÉLIORER | Legend |
| 23 | `ghost-in-the-cell` | Ghost in the Cell | Wood 1 | 1/1129 | À AMÉLIORER | Legend |
| 24 | `fall-challenge-2020` | Fall Challenge 2020 | Wood 1 | 2541/2875 | À AMÉLIORER | Legend |
| 25 | `tron-battle` | Tron Battle | Wood 1 | 1/563 | ACTIF | Legend |
| 26 | `shobu` | Shobu | Wood 4 | —/— | À FAIRE | Legend |
| 27 | `brandubh` | Brandubh | — | —/— | À FAIRE | Legend |
| 28 | `elemental-wars` | Elemental Wars | — | —/— | À FAIRE | Legend |
| 29 | `ataxx` | Ataxx | — | —/— | À FAIRE | Legend |
| 30 | `chain-reaction-1` | Chain Reaction | — | —/— | À FAIRE | Legend |
| 31 | `winter-challenge-2026-snakebyte` | SnakeByte - Winter Challenge 2026 | — | —/— | À FAIRE | Legend |
| 32 | `beeminegame` | BeeMineGame | — | —/— | À FAIRE | Legend |
| 33 | `soak-overflow` | Soak Overflow - Summer Challenge 2025 | — | —/— | À FAIRE | Legend |
| 34 | `game-of-life-or-death` | Game of Life or Death | — | —/— | À FAIRE | Legend |
| 35 | `domain-expansion` | Domain Expansion | — | —/— | À FAIRE | Legend |
| 36 | `gargoyles-versus-santas` | Gargoyles versus Santas | — | —/— | À FAIRE | Legend |
| 37 | `hex` | Hex | — | —/— | À FAIRE | Legend |
| 38 | `dice-shogi` | Dice Shogi | — | —/— | À FAIRE | Legend |
| 39 | `minishogi` | Minishogi | — | —/— | À FAIRE | Legend |
| 40 | `seabed-security` | Seabed Security - Fall Challenge 2023 | — | — | À FAIRE | Legend |
| 41 | `poker` | Poker | — | —/— | À FAIRE | Legend |
| 42 | `mad-knights` | Mad Knights | — | —/— | À FAIRE | Legend |
| 43 | `spring-challenge-2023-ants` | Spring Challenge 2023 - Fourmis | — | —/— | À FAIRE | Legend |
| 44 | `keep-off-the-grass-fall-challenge-2022` | Keep Off The Grass | — | —/— | À FAIRE | Legend |
| 45 | `impasse` | Impasse | — | —/— | À FAIRE | Legend |
| 46 | `git-patchwork` | Git Patchwork | — | —/— | À FAIRE | Legend |
| 47 | `6-nimmt-6-qui-prend-take-5` | 6 nimmt! / 6 qui prend! / Take 5! | — | —/— | À FAIRE | Legend |
| 48 | `isola` | Isola | — | —/— | À FAIRE | Legend |
| 49 | `legends-of-code-magic-constructed` | Legends of Code & Magic: Constructed | — | —/— | À FAIRE | Legend |
| 50 | `start-up` | Start-up! | — | —/— | À FAIRE | Legend |
| 51 | `code-keeper---the-hero` | Code Keeper - The Hero | — | —/— | À FAIRE | Legend |
| 52 | `nine-mens-morris` | nine men's morris | — | —/— | À FAIRE | Legend |
| 53 | `lines-of-action` | Lines Of Action | — | —/— | À FAIRE | Legend |
| 54 | `counting-tictactoe` | Counting TicTacToe | — | —/— | À FAIRE | Legend |
| 55 | `clash-of-bots` | Clash of Bots | — | —/— | À FAIRE | Legend |
| 56 | `space-shooter` | Space Shooter | — | —/— | À FAIRE | Legend |
| 57 | `abalone` | Abalone | — | —/— | À FAIRE | Legend |
| 58 | `volcanoes` | Volcanoes | — | —/— | À FAIRE | Legend |
| 59 | `dice-duel` | Dice Duel | — | —/— | À FAIRE | Legend |
| 60 | `amazons` | Amazons | — | —/— | À FAIRE | Legend |
| 61 | `night-of-war` | Night Of War | — | —/— | À FAIRE | Legend |
| 62 | `clobber` | Clobber | — | —/— | À FAIRE | Legend |
| 63 | `connect-4` | Connect 4 | — | —/— | À FAIRE | Legend |
| 64 | `fireworks` | Fireworks | — | —/— | À FAIRE | Legend |
| 65 | `tryangle-catch` | TryAngle Catch | — | —/— | À FAIRE | Legend |
| 66 | `chess` | Chess | — | —/— | À FAIRE | Legend |
| 67 | `blocking` | Blocking | — | —/— | À FAIRE | Legend |
| 68 | `penguins` | Penguins | — | —/— | À FAIRE | Legend |
| 69 | `atari-go-9x9` | Atari Go 9x9 | — | —/— | À FAIRE | Legend |
| 70 | `dots-and-boxes` | Dots and Boxes | — | —/— | À FAIRE | Legend |
| 71 | `atari-go` | Atari Go | — | —/— | À FAIRE | Legend |
| 72 | `othello-1` | Othello | — | —/— | À FAIRE | Legend |
| 73 | `yinsh` | Yinsh | — | —/— | À FAIRE | Legend |
| 74 | `spring-challenge-2020` | Spring Challenge 2020 | Wood 1 | 1220/1290 | À FAIRE | Legend |
| 75 | `tulips-and-daisies` | Tulips and Daisies | — | —/— | À FAIRE | Legend |
| 76 | `crystal-rush` | Crystal Rush | — | —/— | À FAIRE | Legend |
| 77 | `coders-of-the-realm` | Coders of the Realm | — | —/— | À FAIRE | Legend |
| 78 | `coders-of-the-realm---1v1` | Coders of the Realm - 1v1 | — | —/— | À FAIRE | Legend |
| 79 | `twixt-pp` | Twixt-PP | — | —/— | À FAIRE | Legend |
| 80 | `tower-dereference` | Tower Dereference | — | —/— | À FAIRE | Legend |
| 81 | `onitama` | Onitama | — | —/— | À FAIRE | Legend |
| 82 | `paper-soccer` | Paper soccer | — | —/— | À FAIRE | Legend |
| 83 | `breakthrough` | Breakthrough | — | —/— | À FAIRE | Legend |
| 84 | `oware-abapa` | Oware Abapa | — | —/— | À FAIRE | Legend |
| 85 | `bandas` | Bandas | — | —/— | À FAIRE | Legend |
| 86 | `bit-runner-2048` | Bit Runner 2048 | — | —/— | À FAIRE | Legend |
| 87 | `yavalath` | Yavalath | — | —/— | À FAIRE | Legend |
| 88 | `checkers` | Checkers | — | —/— | À FAIRE | Legend |
| 89 | `langton-s-ant` | Langton s Ant | — | —/— | À FAIRE | Legend |
| 90 | `vindinium` | Vindinium | — | —/— | À FAIRE | Legend |
| 91 | `legends-of-code-magic` | Legends of Code & Magic | — | —/— | À FAIRE | Legend |
| 92 | `tic-tac-toe` | Ultimate Tic-Tac-Toe | — | —/— | À FAIRE | Legend |
| 93 | `code4life` | Code4Life | — | —/— | À FAIRE | Legend |
| 94 | `mad-pod-racing` | Mad Pod Racing | Gold | 15591/22154 | À FAIRE | Legend |
| 95 | `back-to-the-code` | Back to the Code | — | —/— | À FAIRE | Legend |
| 96 | `great-escape` | Great escape | — | —/— | À FAIRE | Legend |
| 97 | `platinum-rift-episode-2` | Platinum Rift - Episode 2 | — | —/— | À FAIRE | Legend |
| 98 | `platinum-rift-episode-1` | Platinum Rift - Episode 1 | — | —/— | À FAIRE | Legend |
| 99 | `poker-chip-race` | Poker Chip Race | — | —/— | À FAIRE | Legend |
| 100 | `game-of-drones` | Game of Drones | — | —/— | À FAIRE | Legend |

Snapshot des ligues : `bot-programming/league_state.json` (régénéré avec
`GET /puzzles/<id>/progress`). Ne jamais y stocker de token.

