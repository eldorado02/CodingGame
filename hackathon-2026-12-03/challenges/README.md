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

Le challenge `2019-code-a-la-mode` amorcé avant l’index a été conservé dans
`challenges/2019-code-a-la-mode/` (statement seul), il sera traité quand son
tour chronologique arrivera.
