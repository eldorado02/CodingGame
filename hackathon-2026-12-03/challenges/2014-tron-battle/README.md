# Tron Battle — guide pour reprendre le travail

Challenge CodinGame bot-programming, dossier `2014-tron-battle`. Objectif : **Gold minimum**, Légende en stretch.
Ce README est écrit pour qu'une autre IA puisse comprendre et améliorer le bot
sans deviner. Mettre à jour **avant** toute modification du code.

## État courant

- Soumissions : `41777481` (version buguée, remplacée), **`41777582` (version validée)**.
- Ligue/rang : Wood 2, **rang 1/1621**, score 21,14 > boss 18,91 (`630143`).
  Promotion pas encore appliquée (en attente du cycle `next_promotion_in_seconds`).
- Boss Wood 2 : `JarvBoss`, `agent_id 630143`. Les boss des ligues supérieures
  ne sont pas exposés tant que le compte n'y est pas — les récupérer via
  `GET /puzzles/tron-battle/opponents` après promotion.
- Journal global : [`../../bot-programming/IMPROVEMENTS.md`](../../bot-programming/IMPROVEMENTS.md)
- Règles du projet : [`../../bot-programming/PLAYBOOK.md`](../../bot-programming/PLAYBOOK.md)

## Ordre de lecture

1. `statement.md` — énoncé officiel (à **relire à chaque promotion de ligue**).
2. `research.md` — deep search : dépots GitHub, arXiv, mécanique exacte, pièges.
3. `plan.md` — algorithme, complexité, règle de départage.
4. `critique.md` — risques identifiés, puis bilan après validation.
5. `solutions/` — 4 implémentations (Python/C/C++/Rust).
6. `tests/` — 10 cas déterministes + fuzz différentiel 200 cas.

## Contrat d'entrée / sortie

```
Ligne 1 : N P                  (N joueurs, P = votre index)
Lignes 2..N+1 : X0 Y0 X1 Y1    (queue, tête) pour chaque joueur
Joueur mort : -1 -1 -1 -1 et SA BANDE DISPARAÎT du plateau
Sortie : une seule ligne UP | DOWN | LEFT | RIGHT, puis flush
```

- Grille **30 × 20**, **tours séquentiels** (P0 puis P1…). Celui qui bouge en
  second ne peut pas entrer dans la case prise en premier.
- 100 ms/tour (1ᵉʳ tour ≈ 1000 ms en pratique). Ne jamais appliquer sa propre
  sortie au tracker : le bloc suivant contient déjà la nouvelle tête.

## Architecture décisionnelle (extrait de `plan.md`)

1. **Tracker** : mémoriser toutes les têtes vues par joueur ; effacer la bande
   d'un joueur mort ; reconstruire `blocked` à chaque tour.
2. **Voronoi 1-ply** : pour chaque coup légal, BFS multi-source depuis la tête
   de chaque joueur **dans l'ordre des tours** ; case au plus proche, égalité
   contestée (0). Score `100000·voronoi + 100·reach + 10·mob + fill`.
3. **Greedy primé** : `fill` = remplissage glouton en choisissant le voisin qui
   retire le moins d'arêtes (wall-hugging).
4. **Départage** : `(score, -ny, -nx, direction)` — entiers uniquement, donc
   fuzz reproductible.

Fait décisionnel (sources dans `research.md`) : celui qui a le plus de
territoire au premier « cut » gagne ~96–99 % des duels.

## Validation

- Exemple officiel : 3 tours → 3 coups, sorties identiques dans 4 langages.
- **Fuzz 210/210** (10 cas déterministes + 200 fuzz) bit à bit identiques Python/C/C++/Rust
  (`bash tests/run.sh`).
- **5/5 victoires** contre le boss `630143` (scores 100–90).
- Bugs corrigés pendant le fuzz (à ne pas réintroduire) :
  - `NEI` construit avec des doublons → `mob` valait 6–12 au lieu de ≤ 4.
  - BFS sauté si la tête était marquée bloquée → Voronoi toujours 0.
  - `best_score` initialisé à `-1` en C alors que les scores sont négatifs.
  - joueur mort : oublier de lire la ligne suivante en Python → crash de parsing.

## Montée en ligue (à faire)

- Wood 1 → Bronze : le boss change ; relire l'énoncé (`cg.py show tron-battle`)
  et relancer deep search + plan + critique + fuzz + 3/5 test games.
- Silver ajoute un 3ᵉ joueur, Gold un 4ᵉ : le Voronoi 2-joueurs ne suffira pas.
  Passer au « paranoid mixte » (le rival le plus proche joue Min) ou max^n/PUCT.
  Sources : `nathanWolo/tron-codingame`, `xavierfeltin/tron_battle`,
  thèse de Nijssen (2013) — détails dans `research.md`.
- Tester les deux sièges : 70–83 % des mêmes ouvertures sont gagnées par le
  même siège quel que soit le moteur.
