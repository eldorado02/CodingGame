# Plan — Xmas Rush (2018-12-07)

## Algorithme

1. **Lecture** : `turnType`, 7×7 masques `URDL`, 2 joueurs, items, quêtes.
2. **Cible** : item dont `itemPlayerId == 0` et dont le nom est la quête révélée.
3. **Tour MOVE** : BFS sur les 49 tuiles (deux masques doivent se connecter),
   sortir jusqu'à 20 directions, ou `PASS`.
4. **Tour PUSH** : simuler 14 poussées (7 lignes × 2 sens, 7 colonnes × 2 sens),
   décaler joueur et item, recalculer la distance BFS ; choisir le minimum.
5. **Livrables** : `statement.md`, `research.md`, `plan.md`, `critique.md`,
   `solutions/{c,cpp,rust,python}`, `tests/{gen_cases.py,run.sh}`.
6. **Validation** : cas déterministes + fuzz différentiel 200 cas + test games
   CodinGame (Wood 3).

## Mise à jour Bronze (v6)

- MOVE : `multi_bfs` (≤20 pas, quêtes collectées, Manhattan) → repli BFS vers
  la quête accessible la plus proche → repli BFS vers une tuile de bord si une
  quête est en main → sinon `PASS`.
- PUSH : 12 poussées locales autour du joueur, score
  `(instant, -éjections, -Manhattan atteignable, collectables, connectivité,
  -main, -ligne, -direction)`, anti-stale après 3 répétitions.
- Validation : fuzz différentiel 210 cas, puis ≥3/5 test games C++ contre le
  boss `2209776` avant soumission.
