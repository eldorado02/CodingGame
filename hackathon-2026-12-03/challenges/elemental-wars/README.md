# Elemental Wars — guide pour reprendre le travail

Challenge CodinGame bot-programming (`elemental-wars`). Objectif : **Ligue Légende**.

## État courant

- Ligue : **Wood 1** (promu depuis Wood 2 via `41787544`). Plus de boss : 64
  joueurs, nous sommes autour du rang 38–40.
- Nouvelle règle Wood 1 : un élémental libre envoyé sur le camp du **prédateur**
  libère tous nos prisonniers (résolu **après** les captures).
- Version en cours : **v6** — prédiction déterministe du prochain coup adverse
  (un pas vers la proie la plus proche, BFS depuis la cible, priorité N,E,S,W),
  interception des proies, évitement du prédateur, anti-regroupement,
  libération des prisonniers.
- Soumission C++ v6 : `41788154`. Test games : **5/5** contre notre ancienne
  version `6918338` et le joueur de rang 40 `6758748` ; **2/5** contre les deux
  meilleurs `6710922`, `6788011` (meilleur rang moyen des variantes essayées).
  Fuzz 200/200 sur 11/13/15 et 6/9/12/15 élémentaux.
- Voir `critique.md` et `../../bot-programming/IMPROVEMENTS.md`.

## Ordre de lecture

1. `statement.md` — énoncé officiel Wood 1 (relu après promotion).
2. `research.md` — sources, mécanique du referee, pistes explorées.
3. `plan.md` — algorithme v6.
4. `critique.md` — bilan des tentatives v3→v11.
5. `solutions/` — 4 implémentations.
6. `tests/` — fuzz différentiel 200 cas (11/13/15, 6/9/12/15 élémentaux).

## Contrat d'entrée / sortie

Init : `myTribe mapSize`, puis `mapSize` lignes de carte (`.`, `#`, `W`, `F`, `P`).
Par tour : `scoreW scoreF scoreP`, `elementalsCount`, puis
`elementalId tribe x y prisoner`. Les identifiants sont uniques **par tribu**.
Sortie : commandes `id MOVE x y` ou `id WAIT` séparées par `;`, `MSG` en repli,
terminées par un retour à la ligne avec flush.

## Pièges rencontrés

- Il faut exactement **2 adversaires** : un seul donnera HTTP 400.
- La lecture bloquante (`stdin.read()`) timeout au premier tour.
- `id` est unique par tribu, pas globalement.
- Rassembler tous ses élémentaux sur une case est fatal : un seul prédateur
  capture toute la pile, et si tous nos élémentaux sont prisonniers la partie
  s'arrête immédiatement.
- `mapSize=15` et `elementalsCount=15` apparaissent en Wood 1 : `NMAX=16`.
- Les budgets de test games sont stricts (30 / 900 s) : respecter `Retry-After`.

## Tests

`bash tests/run.sh` compare les 4 implémentations sur 200 cas de fuzz.
Critère : fuzz vert + au moins 3/5 test games contre le meilleur joueur.
