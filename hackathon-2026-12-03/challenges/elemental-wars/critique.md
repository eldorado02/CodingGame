# Elemental Wars — critique

## Avant implémentation

- Le challenge est à 3 joueurs : un seul appel `test` avec les deux adversaires
  (`--opponents 6910638,6697588`). Un appel avec un seul ID répond HTTP 400
  (`INVALID_PLAYER_COUNT`).
- Wood 3 ne compte qu'un élémental par tribu : une stratégie de poursuite/fuite
  suffit, sans gestion de groupe.
- Les captures étant simultanées, une case occupée par le prédateur est fatale
  même si elle contient aussi la proie.

## Après implémentation

### Tentatives

- `v1` (lecture globale `stdin.read()`) : timeout au frame 1, 0/5. Cause : le
  processus attend la fin du flux au lieu de répondre tour par tour.
- `v2` (lecture ligne à ligne, fuite modérée `-500`) : **3/5**.
- `v3` (fuite renforcée `-2000`) : **5/5** en Python puis **5/5** en C++.
- Ports C/C++/Rust : deux bugs de traduction détectés par le fuzz :
  `pred_of('P')` renvoyait `P` au lieu de `F`, et le prédateur prisonnier n'était
  pas ignoré. Après correction : fuzz **200/200** identique.

### Wood 2 — v3 multi-élémentaux

- Constat : la version Wood 3 timeout en Wood 2 (3 défaites sur 5 test games).
  Les tableaux de taille 8 débordent quand `elementalsCount` vaut 9 ou 12 ;
  un seul élémental était commandé.
- Hypothèse : commander tous les élémentaux et calculer une BFS par ennemi.
- Action : v3, générateur de fuzz étendu (6/9/12 élémentaux, 11×11 ou 13×13),
  4 ports, fuzz 200/200, test games contre `6697631,6699103`.
- Résultat : **5/5**, soumission `41787544`.

### Wood 1 — promotion et nouvelles règles

- Constat : après promotion en Wood 1, la carte passe à 11/13/15 et le nombre
  d'élémentaux à 6/9/12/15. Une **nouvelle règle** apparaît : un allié libre
  envoyé sur le camp du prédateur libère tous nos prisonniers. Le jeu s'arrête
  immédiatement si une tribu est entièrement prisonnière.
- Hypothèses testées (toutes en C++, 5 test games contre `6710922,6788011`,
  les deux meilleurs joueurs Wood 1) :
  - `v4` libérateur simple : **0/5** (défaite rapide).
  - `v5` anti-regroupement + fuite forte : **0/5** (score 0, trop passif).
  - `v6` prédiction du coup adverse + interception + anti-regroupement :
    **2/5** (meilleur rang moyen ; scores 0–2).
  - `v7` double modèle poursuite/fuite + distances prédictives : **0/5**.
  - `v8` fuite renforcée : **0/5**.
  - `v9` poursuite des cases prévues : **1/3** valide.
  - `v11` probabilités + garde de notre camp : **0/5** (scores plus élevés mais
    adversaires encore plus rapides).
- Action : retenir `v6`, porter en 4 langages, étendre le fuzz à `mapSize=15`
  et 15 élémentaux, soumettre le C++.
- Résultat : `v6` **2/5** contre les deux meilleurs (meilleur rang moyen des
  variantes) ; `v11` marque plus mais finit 0/5. Test décisif : **5/5** contre
  notre ancienne soumission `6918338` et le rang 40 `6758748` (scores 2–4
  contre 0–1). Fuzz 200/200. Soumission C++ `41788154`.
- Sources : énoncé officiel relu après promotion, logs de test games,
  recherche GitHub/web (aucun bot public exploitable trouvé).

## Leçons

- Le fuzz différentiel est indispensable dès que la logique dépend d'une table
  de correspondance : une erreur de port passe inaperçue en test game si la
  tribu concernée n'apparaît pas dans les 5 parties.
- Un fuzz calibré sur la ligue inférieure ne détecte pas les débordements
  apparaissant à la ligue supérieure : régénérer les cas après promotion.
- La lecture bloquante est le défaut récurrent de cette campagne (Brandubh,
  Elemental Wars) : toujours vérifier le premier log de test game avant de
  conclure sur la stratégie.
