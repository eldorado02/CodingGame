# Critique — Shobu

## Avant implémentation

- Le nombre d'actions légales peut dépasser 1 000 : une recherche exhaustive
  à deux plis est trop chère en Python.
- Le pie rule change la couleur ; le bot doit continuer à jouer les stones
  `b` après un swap, pas inverser le plateau.
- Un stone poussé hors du board 4×4 disparaît ; un stone poussé dans une case
  encore sur le board y reste. La simulation doit utiliser `sameBoard`.

## Après implémentation

_À remplir après les premiers test games._

## 2026-11-22 — Wood 4 : v6 retenu

- v1 (1 ply + réponse adverse, sans positionnel, swap conditionnel) : **1/5**.
- v2 (always swap) : 0 victoire, 3 nulles, 2 défaites.
- v4 (positionnel bords, poids 12, swap conditionnel) : **2/5**.
- v5 (positionnel 12 + always swap) : 2 victoires, 3 nulles, 0 défaite.
- v6 (positionnel **25** + always swap) : **4/5**.
- Ports C/C++/Rust : un bug de port majeur corrigé — les réponses adverses
  doivent être prises dans l'ordre de génération du referee (`replies[:24]`),
  pas triées par évaluation. Après correction : fuzz **200/200** identique.
- Leçon : contre un boss greedy qui ne regarde pas la réponse, le swap
  systématique élimine les défaites ; le poids positionnel sur les bords
  transforme les nulles en victoires. L'ordre de génération du referee fait
  partie du comportement de l'algorithme : tout tri change le résultat.
