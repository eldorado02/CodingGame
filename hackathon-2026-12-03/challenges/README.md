# Index des challenges — Phase 2

`challenges.json` contient **45 concours CodinGame bot-programming de 2016 à
2026**, triés par date, avec `year`, `date`, `id`, `name`, `playable` et
`challenge_public_id`.

## Méthode

- Source : endpoint historique `POST https://www.codingame.com/services/Challenge/findPastChallenges`
  avec `[null]`. L’API v1 `/challenges` ne renvoie que les 20 plus récents et
  ignore `limit`/`offset` : elle ne suffit pas pour 2016–2018.
- `playable` : vrai si le `pretty_id` apparaît dans `GET /puzzles?arcade=false`
  (pagination 100). 28 challenges sur 45 sont rejouables via l’API ; les
  autres n’ont plus de statement accessible et sont traités en dernière
  priorité.
- Répartition : 2016:6, 2017:5, 2018:7, 2019:4, 2020:3, 2021:2, 2022:4,
  2023:4, 2024:4, 2025:2, 2026:4.

## Traitement séquentiel

Un challenge à la fois, dans l’ordre chronologique, parmi les `playable` :

```
challenges/<année>-<id>/
├── statement.md      énoncé officiel
├── research.md       sources + mécanique + décisions
├── plan.md           algorithme et détails
├── critique.md       risques avant / bilan après
├── solutions/        c/ cpp/ rust/ python/
└── tests/            cas déterministes + run.sh
```

## État

| # | Date | Challenge | État |
|---|---|---|---|
| 1 | 2016-04-30 | `smash-the-code` | ✅ research/plan/critique + 4 langages + 206 cas verts |
| — | 2016-02-27 | `coders-strike-back` | ⏭ non rejouable via API (statement indisponible) |
| 2 | 2016-06-25 | `codebusters` | ✅ research/plan/critique + 4 langages + 208 cas verts |
| 3 | 2016-09-24 | `hypersonic` | ✅ research/plan/critique + 4 langages + 208 cas verts + win réel |
| — | 2016-10-01 | `the-accountant` | ⏭ non rejouable via API |
| 4 | 2016-11-26 | `fantastic-bits` | ✅ 4 langages + 207 cas verts + win 4–0 |
| 5 | 2017-02-25 | `ghost-in-the-cell` | ✅ 4 langages + 206 cas verts + win 346–5 |
| 6 | 2017-04-14 | `coders-of-the-caribbean` | ✅ 4 langages + 207 cas verts + rang 1/2 |
| 7 | 2017-06-23 | `wondev-woman` | ✅ 4 langages + 206 cas verts + win 1–0 |
| 8 | 2017-11-17 | `mean-max` | ✅ 4 langages + 206 cas verts + win 50–20–21 |
| 9 | 2018-03-02 | `botters-of-the-galaxy` | ✅ 4 langages + 208 cas verts + win vs DoubleHulk |
| 10 | 2018-04-20 | `code-royale` | ✅ 4 langages + 207 cas verts + 2 wins (dont Duchess Boss) |
| 11 | 2018-06-15 | `code-of-kutulu` | ✅ 4 langages + 206 cas verts + win vs boss Davy Jones & rang 1/2 |
| — | 2018-07-25 | `legends-of-code-and-magic` | ⏭ non rejouable via API |
| — | 2018-07-27 | `legends-of-code-and-magic-marathon` | ⏭ non rejouable via API |
| 12 | 2018-10-19 | `a-star-craft` | ✅ 4 langages + 206 cas verts + 30/30 testcases, 2487 points |
| 13 | 2018-12-07 | `xmas-rush` | ✅ 4 langages + 206 cas verts + 4/4 test games gagnés |
| 14 | 2019-03-08 | `code-a-la-mode` | ✅ 4 langages + 206 cas verts + 4/4 test games gagnés |
| 15 | 2019-05-17 | `a-code-of-ice-and-fire` | ✅ 4 langages + 210 cas verts + 10/10 test games gagnés |
| 16 | 2020-03-20 | `ocean-of-code` | ✅ 4 langages + 210 cas verts + 8/8 test games gagnés |
| 17 | 2020-05-07 | `spring-challenge-2020` | ✅ 4 langages + 210 cas verts + 9/9 test games gagnés |
| 18 | 2020-11-12 | `fall-challenge-2020` | ✅ 4 langages + 210 cas verts + 8/8 test games gagnés |
| 19 | 2021-05-06 | `spring-challenge-2021` | ✅ 4 langages + 210 cas verts + 8/8 test games gagnés |
| 20 | 2022-04-21 | `spring-challenge-2022` | ✅ 4 langages + 210 cas verts + 8/8 test games gagnés |

Les challenges 2019 non rejouables via l’API (`detective-pikaptcha`,
`unleash-the-geek-amadeus`) sont ignorés de la séquence.
