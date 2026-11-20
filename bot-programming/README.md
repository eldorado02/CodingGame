# CodinGame — bot programming

Everything the agent plays lives here, one folder per game under `games/<pretty_id>/`
(bot, notes, saved test logs).

- `tools/cg.py` — API client (list, show rules, stub generator, test, submit, progress).
- `games.json` — raw index of the 100 bot programming games.
- [MODELS.md](MODELS.md) — which available model is strongest for research/code, with benchmark sources.
- Token: `~/.config/codingame/token` (never in this repo).

## Workflow per game

```sh
cd games/<pretty_id>
../../tools/cg.py show <pretty_id>        # rules
../../tools/cg.py stub <pretty_id>        # input/output format
../../tools/cg.py test <pretty_id> bot.py # one test game, log saved in logs/
../../tools/cg.py submit <pretty_id> bot.py
../../tools/cg.py get <pretty_id> progress
```

## Bot programming games (100)

| Game | id | rating | players | played |
|---|---|---|---|---|
| [Shobu](games/shobu/README.md) | `shobu` | - | ?v? |  |
| [Brandubh](games/brandubh/README.md) | `brandubh` | 5.0 | ?v? |  |
| [Back Track King — Summer Challenge 2026](games/summer-challenge-2026-back-track-king/README.md) | `summer-challenge-2026-back-track-king` | 4.38 | ?v? | yes |
| [Elemental Wars](games/elemental-wars/README.md) | `elemental-wars` | 5.0 | ?v? |  |
| [Ataxx](games/ataxx/README.md) | `ataxx` | 5.0 | ?v? |  |
| [Chain Reaction](games/chain-reaction-1/README.md) | `chain-reaction-1` | 5.0 | ?v? |  |
| [Troll Farm - Spring Challenge 2026](games/spring-challenge-2026-troll-farm/README.md) | `spring-challenge-2026-troll-farm` | 4.97 | ?v? | yes |
| [SnakeByte - Winter Challenge 2026](games/winter-challenge-2026-snakebyte/README.md) | `winter-challenge-2026-snakebyte` | 4.93 | ?v? | yes |
| [BeeMineGame](games/beeminegame/README.md) | `beeminegame` | 4.92 | ?v? |  |
| [Soak Overflow - Summer Challenge 2025](games/soak-overflow/README.md) | `soak-overflow` | 4.86 | ?v? |  |
| [Game of Life or Death](games/game-of-life-or-death/README.md) | `game-of-life-or-death` | 5.0 | ?v? |  |
| [Domain Expansion](games/domain-expansion/README.md) | `domain-expansion` | 4.86 | ?v? |  |
| [Cellularena - Winter Challenge 2024](games/winter-challenge-2024/README.md) | `winter-challenge-2024` | 4.83 | ?v? |  |
| [Gargoyles versus Santas](games/gargoyles-versus-santas/README.md) | `gargoyles-versus-santas` | 4.9 | ?v? |  |
| [Hex](games/hex/README.md) | `hex` | 4.89 | ?v? |  |
| [Olymbits - Summer Challenge 2024](games/summer-challenge-2024-olymbits/README.md) | `summer-challenge-2024-olymbits` | 4.57 | ?v? |  |
| [Dice Shogi](games/dice-shogi/README.md) | `dice-shogi` | 5.0 | ?v? |  |
| [Minishogi](games/minishogi/README.md) | `minishogi` | 5.0 | ?v? |  |
| [Seabed Security - Fall Challenge 2023](games/seabed-security/README.md) | `seabed-security` | 4.75 | ?v? |  |
| [Poker](games/poker/README.md) | `poker` | 4.73 | ?v? |  |
| [Mad Knights](games/mad-knights/README.md) | `mad-knights` | 4.57 | ?v? |  |
| [Spring Challenge 2023 - Fourmis](games/spring-challenge-2023-ants/README.md) | `spring-challenge-2023-ants` | 4.3 | ?v? |  |
| [Keep Off The Grass](games/keep-off-the-grass-fall-challenge-2022/README.md) | `keep-off-the-grass-fall-challenge-2022` | 4.81 | ?v? |  |
| [Impasse](games/impasse/README.md) | `impasse` | 5.0 | ?v? |  |
| [Git Patchwork](games/git-patchwork/README.md) | `git-patchwork` | 5.0 | ?v? |  |
| [6 nimmt! / 6 qui prend! / Take 5!](games/6-nimmt-6-qui-prend-take-5/README.md) | `6-nimmt-6-qui-prend-take-5` | 4.9 | ?v? |  |
| [Isola](games/isola/README.md) | `isola` | 4.72 | ?v? |  |
| [Green Circle](games/green-circle/README.md) | `green-circle` | 4.02 | ?v? |  |
| [Legends of Code & Magic: Constructed](games/legends-of-code-magic-constructed/README.md) | `legends-of-code-magic-constructed` | 5.0 | ?v? |  |
| [Spider Attack](games/spring-challenge-2022/README.md) | `spring-challenge-2022` | 4.89 | ?v? |  |
| [Start-up!](games/start-up/README.md) | `start-up` | 3.6 | ?v? |  |
| [Code Keeper - The Hero](games/code-keeper---the-hero/README.md) | `code-keeper---the-hero` | 4.94 | ?v? |  |
| [nine men's morris](games/nine-mens-morris/README.md) | `nine-mens-morris` | 4.73 | ?v? |  |
| [Lines Of Action](games/lines-of-action/README.md) | `lines-of-action` | 4.33 | ?v? |  |
| [Counting TicTacToe](games/counting-tictactoe/README.md) | `counting-tictactoe` | 4.19 | ?v? |  |
| [Clash of Bots](games/clash-of-bots/README.md) | `clash-of-bots` | 4.69 | ?v? |  |
| [Space Shooter](games/space-shooter/README.md) | `space-shooter` | 4.73 | ?v? |  |
| [Abalone](games/abalone/README.md) | `abalone` | 5.0 | ?v? |  |
| [Volcanoes](games/volcanoes/README.md) | `volcanoes` | 4.47 | ?v? |  |
| [Dice Duel](games/dice-duel/README.md) | `dice-duel` | 5.0 | ?v? |  |
| [Amazons](games/amazons/README.md) | `amazons` | 4.92 | ?v? |  |
| [Spring Challenge 2021](games/spring-challenge-2021/README.md) | `spring-challenge-2021` | 4.89 | ?v? |  |
| [Night Of War](games/night-of-war/README.md) | `night-of-war` | 4.78 | ?v? |  |
| [Clobber](games/clobber/README.md) | `clobber` | 4.71 | ?v? |  |
| [Connect 4](games/connect-4/README.md) | `connect-4` | 4.9 | ?v? |  |
| [Fireworks](games/fireworks/README.md) | `fireworks` | 4.38 | ?v? |  |
| [TryAngle Catch](games/tryangle-catch/README.md) | `tryangle-catch` | 4.94 | ?v? |  |
| [Chess](games/chess/README.md) | `chess` | 4.88 | ?v? |  |
| [Fall Challenge 2020](games/fall-challenge-2020/README.md) | `fall-challenge-2020` | 4.78 | ?v? |  |
| [Blocking](games/blocking/README.md) | `blocking` | 4.1 | ?v? |  |
| [Penguins](games/penguins/README.md) | `penguins` | 4.6 | ?v? |  |
| [Atari Go 9x9](games/atari-go-9x9/README.md) | `atari-go-9x9` | 5.0 | ?v? |  |
| [Dots and Boxes](games/dots-and-boxes/README.md) | `dots-and-boxes` | 4.94 | ?v? |  |
| [Atari Go](games/atari-go/README.md) | `atari-go` | 4.93 | ?v? |  |
| [Othello](games/othello-1/README.md) | `othello-1` | 4.92 | ?v? |  |
| [Yinsh](games/yinsh/README.md) | `yinsh` | 4.56 | ?v? |  |
| [Spring Challenge 2020](games/spring-challenge-2020/README.md) | `spring-challenge-2020` | 4.77 | ?v? |  |
| [Ocean of Code](games/ocean-of-code/README.md) | `ocean-of-code` | 4.81 | ?v? |  |
| [Tulips and Daisies](games/tulips-and-daisies/README.md) | `tulips-and-daisies` | 4.94 | ?v? |  |
| [Crystal Rush](games/crystal-rush/README.md) | `crystal-rush` | 4.6 | ?v? |  |
| [Coders of the Realm](games/coders-of-the-realm/README.md) | `coders-of-the-realm` | 4.18 | ?v? |  |
| [Coders of the Realm - 1v1](games/coders-of-the-realm---1v1/README.md) | `coders-of-the-realm---1v1` | 4.5 | ?v? |  |
| [Twixt-PP](games/twixt-pp/README.md) | `twixt-pp` | 4.63 | ?v? |  |
| [Tower Dereference](games/tower-dereference/README.md) | `tower-dereference` | 4.18 | ?v? |  |
| [Onitama](games/onitama/README.md) | `onitama` | 4.66 | ?v? |  |
| [Paper soccer](games/paper-soccer/README.md) | `paper-soccer` | 4.68 | ?v? |  |
| [Breakthrough](games/breakthrough/README.md) | `breakthrough` | 4.82 | ?v? |  |
| [Oware Abapa](games/oware-abapa/README.md) | `oware-abapa` | 4.58 | ?v? |  |
| [A Code of Ice And Fire](games/a-code-of-ice-and-fire/README.md) | `a-code-of-ice-and-fire` | 4.89 | ?v? |  |
| [Bandas](games/bandas/README.md) | `bandas` | 4.74 | ?v? |  |
| [Bit Runner 2048](games/bit-runner-2048/README.md) | `bit-runner-2048` | 4.81 | ?v? |  |
| [Cultist Wars](games/cultist-wars/README.md) | `cultist-wars` | 4.76 | ?v? |  |
| [Code a la Mode](games/code-a-la-mode/README.md) | `code-a-la-mode` | 4.71 | ?v? |  |
| [Yavalath](games/yavalath/README.md) | `yavalath` | 4.79 | ?v? |  |
| [Checkers](games/checkers/README.md) | `checkers` | 4.64 | ?v? |  |
| [Langton s Ant](games/langton-s-ant/README.md) | `langton-s-ant` | 4.68 | ?v? |  |
| [Vindinium](games/vindinium/README.md) | `vindinium` | 4.81 | ?v? |  |
| [Xmas Rush](games/xmas-rush/README.md) | `xmas-rush` | 4.85 | ?v? |  |
| [Legends of Code & Magic](games/legends-of-code-magic/README.md) | `legends-of-code-magic` | 4.76 | ?v? |  |
| [Code of Kutulu](games/code-of-kutulu/README.md) | `code-of-kutulu` | 4.56 | ?v? |  |
| [Code Royale](games/code-royale/README.md) | `code-royale` | 4.72 | ?v? |  |
| [Botters of the Galaxy](games/botters-of-the-galaxy/README.md) | `botters-of-the-galaxy` | 4.26 | ?v? |  |
| [Ultimate Tic-Tac-Toe](games/tic-tac-toe/README.md) | `tic-tac-toe` | 4.83 | ?v? |  |
| [Code4Life](games/code4life/README.md) | `code4life` | 4.71 | ?v? |  |
| [Mean Max](games/mean-max/README.md) | `mean-max` | 4.42 | ?v? |  |
| [Wondev Woman](games/wondev-woman/README.md) | `wondev-woman` | 4.53 | ?v? |  |
| [Coders of the Caribbean](games/coders-of-the-caribbean/README.md) | `coders-of-the-caribbean` | 4.61 | ?v? |  |
| [Ghost in the Cell](games/ghost-in-the-cell/README.md) | `ghost-in-the-cell` | 4.83 | ?v? |  |
| [Fantastic Bits](games/fantastic-bits/README.md) | `fantastic-bits` | 4.86 | ?v? |  |
| [Hypersonic](games/hypersonic/README.md) | `hypersonic` | 4.84 | ?v? |  |
| [CodeBusters](games/codebusters/README.md) | `codebusters` | 4.79 | ?v? |  |
| [Smash The Code](games/smash-the-code/README.md) | `smash-the-code` | 4.34 | ?v? |  |
| [Mad Pod Racing](games/mad-pod-racing/README.md) | `mad-pod-racing` | 4.7 | ?v? | yes |
| [Back to the Code](games/back-to-the-code/README.md) | `back-to-the-code` | 4.42 | ?v? |  |
| [Great escape](games/great-escape/README.md) | `great-escape` | 4.59 | ?v? |  |
| [Platinum Rift - Episode 2](games/platinum-rift-episode-2/README.md) | `platinum-rift-episode-2` | 4.62 | ?v? |  |
| [Platinum Rift - Episode 1](games/platinum-rift-episode-1/README.md) | `platinum-rift-episode-1` | 4.4 | ?v? |  |
| [Poker Chip Race](games/poker-chip-race/README.md) | `poker-chip-race` | 4.29 | ?v? |  |
| [Game of Drones](games/game-of-drones/README.md) | `game-of-drones` | 4.67 | ?v? |  |
| [Tron Battle](games/tron-battle/README.md) | `tron-battle` | 4.83 | ?v? |  |

## Optimization (20)

| Game | id | rating |
|---|---|---|
| One Away | `one-away` | 5.0 |
| Flames Extinguisher | `flames-extinguisher` | 4.82 |
| Fall Challenge 2024 - Selenia City | `selenia-city` | 4.71 |
| Vehicle Routing Problem | `vehicle-routing-problem` | 4.7 |
| Snake | `snake` | 4.55 |
| Wordle | `wordle` | 4.76 |
| Block the spreading fire ! | `block-the-spreading-fire` | 4.5 |
| Travelling Salesman | `travelling-salesman` | 4.41 |
| 2048 | `2048` | 4.59 |
| SameGame | `samegame` | 4.7 |
| Search Race | `search-race` | 4.63 |
| Bulls and Cows 2 | `bulls-and-cows-2` | 4.33 |
| Number Shifting | `number-shifting` | 4.69 |
| CGFunge Prime | `cgfunge-prime` | 4.63 |
| Bender - Episode 4 | `bender---episode-4` | 4.67 |
| A*Craft | `a-star-craft` | 4.56 |
| CodinGame Sponsored Challenge | `codingame-sponsored-contest` | 4.03 |
| Code vs Zombies | `code-vs-zombies` | 4.66 |
| Code of the Rings | `code-of-the-rings` | 4.64 |
| Mars Lander | `mars-lander` | 4.62 |

## Code golf (23)

| Game | id | rating |
|---|---|---|
| Calcul Maya - Code Golf | `mayan-calculation-codesize` | 4.65 |
| The Fall - Episode 2 - Code Golf | `the-fall-episode-2-codesize` | 4.95 |
| Numéros de téléphone - Code Golf | `telephone-numbers-codesize` | 4.59 |
| Super calculateur - Code Golf | `super-computer-codesize` | 4.63 |
| Câblage réseau - Code Golf | `network-cabling-codesize` | 4.45 |
| Des nains sur des épaules de géants - Code Golf | `dwarfs-standing-on-the-shoulders-of-giants-codesize` | 4.59 |
| Blunder - Episode 1 - Code Golf | `blunder-episode-1-codesize` | 4.54 |
| The Bridge - Code Golf | `the-bridge-episode-2-codesize` | 4.47 |
| Shadows of the Knight - Episode 1 - Code Golf | `shadows-of-the-knight-episode-1-codesize` | 4.59 |
| Partitions de musique - Code Golf | `music-scores-codesize` | 4.76 |
| Surface - Code Golf | `surface-codesize` | 4.6 |
| Montagnes russes - Code Golf | `roller-coaster-codesize` | 4.76 |
| Le labyrinthe - Code Golf | `the-labyrinth-codesize` | 4.5 |
| Séquençage du génôme - Code Golf | `genome-sequencing-codesize` | 4.6 |
| La résistance - Code Golf | `the-resistance-codesize` | 4.57 |
| La descente - Code Golf | `the-descent-codesize` | 4.52 |
| Mars Lander - Episode 3 - Code Golf | `mars-lander-episode-3-codesize` | 4.6 |
| Blunder - Episode 3 - Code Golf | `blunder-episode-3-codesize` | 4.62 |
| Vox Codei - Episode 1 - Code Golf | `vox-codei-episode-1-codesize` | 4.32 |
| Vox Codei - Episode 2 - Code Golf | `vox-codei-episode-2-codesize` | 4.7 |
| The Fall - Episode 3 - Code Golf | `the-fall-episode-3-codesize` | 4.54 |
| ASCII Art - Code Golf | `ascii-art-codesize` | 4.48 |
| Shadows of the Knight - Episode 2 - Code Golf | `shadows-of-the-knight-episode-2-codesize` | 4.63 |
