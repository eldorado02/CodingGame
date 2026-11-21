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
