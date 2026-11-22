# Othello — guide pour reprendre le travail

Challenge CodinGame bot-programming (`othello-1`). Objectif : **Ligue Légende**.

## État courant

- Ligue : **Wood 1** (promu depuis Wood 2 avec `41787653`). Pas de boss
  affiché ; score de ligue ~20, rang ~116/287, classement terminé.
- Version retenue : moteur **bitboard déterministe**, profondeur fixe 6,
  évaluation positionnelle + mobilité + frontier. 5/5 contre l'ancien boss
  Wood 2 `6797560`, fuzz 200/200 en 4 langages.
- Profondeur 7 : ~85–108 ms local mais **timeout** sur les serveurs CG
  (1 défaite sur 5 par timeout) et toujours 0/5 contre le rank 1 `6886490`.
  Non retenue.
- Voir `critique.md` et `../../bot-programming/IMPROVEMENTS.md`.

## Ordre de lecture

1. `statement.md` — énoncé officiel.
2. `research.md` — sources et mécanique du referee.
3. `plan.md` — algorithme bitboard.
4. `critique.md` — bilan des tentatives.
5. `solutions/` — 4 implémentations.
6. `tests/` — fuzz différentiel (200 cas).

## Contrat d'entrée / sortie

Entrée : `id`, `boardSize` (3 premiers tours seulement), 8 lignes de 8
caractères (`.`/`0`/`1`), `actionCount`, puis les actions légales (`a1`..`h8`).
Sortie : une coordonnée (`e6`) suivie d'un retour à la ligne, avec flush.

## Pièges déjà rencontrés

- `Cell.toString()` = `(char)(97+x) + (y+1)` ; bit interne = `y*8+x`.
- Le referee n'envoie `id`/`boardSize` que les 3 premiers tours : le parseur
  doit détecter les lignes de plateau et ignorer les autres.
- Le boss active le mode EXPERT mais notre bot ne le fait pas : ne pas lire
  la ligne supplémentaire.
- Sans élagage à la racine, la recherche est trop lente (ancien moteur).
- Les budgets temps (115 ms) cassent l'identicité du fuzz entre langages :
  le moteur bitboard à profondeur fixe est déterministe et rapide (~35 ms max).

## Tests

`bash tests/run.sh` compare les 4 implémentations sur 200 cas de fuzz.
Critère : fuzz vert + ≥3/5 test games contre le boss.
