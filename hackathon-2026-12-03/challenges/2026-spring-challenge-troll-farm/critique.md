# Critique — Troll Farm (Wood)

## Avant implémentation
| Risque | Mitigation |
|---|---|
| Protocole mal cerné (draw du moteur, inventaires, arbres) | Lecture du moteur Java officiel puis sonde sur une vraie partie Wood. |
| Cabane traitée comme case praticable | `Cell.isWalkable()` lu dans le source : seule `GRASS` est valide, la cabane est exclue du BFS. |
| Cible non atteignable | Cibles = arbres ou voisins herbeux de la cabane, vérifiés dans `dist`. |
| Dépôt refusé | `DROP` émis dès que la distance de Manhattan à la cabane vaut 1, conformément à `DropTask.isNearShack()`. |
| Récolte multiple impossible en Wood | `harvestPower = carryCapacity = 1` : une seule unité par `HARVEST`. |
| Processus tué entre les tours | Lecture en flux par `readline()` / `getline()` / `lines()` dans une boucle infinie, jamais de `read()` global. |
| Dépassement de 50 ms | BFS `O(w × h)` recalculé chaque tour sur 128 cases. |

## Après implémentation
- `tests/run.sh` : **10/10 cas déterministes + 200/200 cas de fuzz identiques**
  entre Python, C, C++ et Rust, sans avertissement de compilation.
- Deux bugs réels trouvés puis corrigés pendant la validation :
  1. **Python** : les voisins de la cabane étaient calculés comme `(dx, dy)`
     au lieu de `(shack_x + dx, shack_y + dy)`, ce qui produisait
     `KeyError: (0, -1)` et un timeout. Correction dans les 4 langages.
  2. **BFS sur la cabane** : la première version autorisait le passage par la
     case `0`. Le moteur refuse alors le `MOVE` (« troll stayed at (3, 1) »)
     et le bot perdait du temps. Correction : `blocked = tout sauf '.'`
     (Python), `grid[y][x] == '.'` (C/C++) et `b'.'` (Rust). C’est ce correctif
     qui a fait passer les parties perdues en victoires.
- Un troisième point propre au C (copie de la ligne avec `strncpy` / `snprintf`)
  a été simplifié en `scanf("%31s", grid[y])` pour supprimer tout avertissement
  `-Wstringop-truncation`.
- Test games officiels :
  - **13/13 victoires** contre le boss Wood `6479367` après le correctif du
    BFS, dans les 4 langages (scores observés de `15–10` à `32–24`).
  - **4/4 victoires** contre `6480135 Tingus`, `6479677 maxoufilou`,
    `6481138 cybertrojan` et `6479712 Valisoa01` lors de la validation
    initiale.
  - Les réponses HTTP 429 rencontrées sont des limites de l’API
    (30 parties / 900 s), pas des défaites ; les tests ont été relancés après
    le délai indiqué.
- Limites connues : le bot ignore l’adversaire, la croissance des arbres et
  l’expansion future (entraînement de trolls / plantations). C’est suffisant
  en Wood, où un seul troll par camp récolte et dépose un fruit à la fois ; en
  Bronze il faudra gérer rivière, rochers, fer, vitesses/supports variables,
  plusieurs trolls, conflits de récolte et la montée en taille des arbres.

## Montée en ligue Wood 1 (soumission officielle)
- Après promotion automatique, le boss Wood 1 (`6479368`) dispose de 3 trolls
  et plante des arbres : le bot Wood 0 perdait 0/3 (scores 77–110, 87–116,
  85–128).
- Nouvelle stratégie : `TRAIN` d’un troll polyvalent au premier tour (choix du
  triplet `movementSpeed / carryCapacity / harvestPower` maximum abordable),
  contrôle de **tous** les trolls, récolte prioritaire, retour vers une case
  adjacente à la cabane, déplacement BFS multi-source avec évitement des cases
  occupées.
- Validation : **5/5 victoires** contre `6479368` (155–128, 136–133, 137–123,
  123–108, 149–92). 210/210 cas différentiels identiques dans les 4 langages.
- Nouvelle soumission officielle : `41773233`.

## Bronze — itération HV+multi (2026-11-21)

- Constat : bot Bronze initial (1 seul entraînement au tour 1) bloqué à
  score 22,88 < boss 22,96 malgré le rang 1/401.
- Hypothèses testées (une seule à la fois, puis combinée) :
  - `PLANT` jardinier près de la cabane : **rejeté** (2/6, bruité).
  - `CHOP` sur arbre épuisé : **rejeté** (1/6).
  - Entraînement multiple (5 trolls) : 3/6, bat le rank 2 (2/2) mais perd vs boss.
  - Priorité `harvestPower` (3×) + `carryCapacity` (2×) sur un seul troll : 8/18.
  - **Retenu** : combinaison entraînement jusqu'à 3 trolls + priorité HV →
    4/4 vs rank 2 `6910022`, ~4/9 vs boss `6479370`.
- Fuzz : 210/210 cas identiques dans 4 langages après port.
- Soumission : C++ (nouvelle règle de campagne), en attente du quota de test games.

### Leçons
- Un seul entraînement au tour 1 est un plafond dur : si l'inventaire initial
  est insuffisant, `trained=True` empêche tout entraînement ultérieur.
- `harvestPower` est le attribut le plus rentable (1 HARVEST = plusieurs fruits).
- Le fuzz ne détecte pas les erreurs de stratégie : seuls les test games comptent.

## Revert Bronze (2026-11-21)

- Le prototype « 3 trolls + priorité harvestPower » (`41778069`) a convergé
  vers un score de ligue ~20,5–20,9 (rang ~100–170) alors que le bot initial
  (1 troll polyvalent entraîné au tour 1, `41776913`) avait convergé vers
  **22,88 au rang 1**. Les test games à court terme (4/4 vs rank 2) étaient
  trompeurs : le classement ligue est joué contre toute la salle.
- Action : restauration des 4 langages depuis le commit `54bffd1`, fuzz
  **210/210** vert, nouvelle soumission **C++** `41778446`.
- Leçon : ne pas juger une stratégie uniquement sur le boss ; attendre la
  convergence du score de ligue (percentage = 100) avant de conclure.


## Bronze — 3 trolls entraînés séquentiellement (2026-11-21, rejeté)

- Nouveau test : conserver l'attribution équilibrée `movementSpeed/carryCapacity/harvestPower`
  et autoriser jusqu'à **3 entraînements** au fil des tours (au lieu d'un seul).
- Python : **10 victoires / 14** test games contre le boss `6479370` (vs 2/9 pour
  le bot à 1 troll). 4 trolls : 1/6 → rejeté.
- Port C/C++/Rust effectué, fuzz **210/210** identique.
- Mais C++ : 2/5, 1/5, 4/9 puis 0/5, soit **7/24** — aucun lot à ≥3/5.
  Conformément au critère de soumission, la variante est rejetée.
- Action : retour au bot validé `41778446` dans les 4 langages ; le commit de
  cette itération doit être annulé pour les fichiers de solution.
