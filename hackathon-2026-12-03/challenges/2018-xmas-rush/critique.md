# Critique — Xmas Rush

## Avant implémentation

| Risque | Évaluation | Mitigation |
|---|---|---|
| Processus tué après le premier tour | Élevé | Boucle `while` sur les tours ; stdin reste ouvert. |
| Lecture bloquante (`read()` global) | Élevé | `readline()` / `getline()` exacts par tour. |
| Item sur tuile joueur (`-1`) | Moyen | Poussée = insertion sur la case d'entrée. |
| Item éjecté (`pop`) | Moyen | Revient sur la tuile du joueur. |
| Déplacement sans connexion des deux tuiles | Élevé | BFS vérifie `current[d]` **et** `next[opp(d)]`. |
| Poussée simultanée même ligne | Faible | Non modélisée (adversaire inconnu). |
| Divergence 4 langages | Moyen | Départage `(distance, ligne, direction)` identique. |

## Après implémentation

- `tests/run.sh` : 10 cas × 4 tours × 4 langages → OK.
- Fuzz différentiel : 200 parties aléatoires de 4 tours → **0 divergence**.
- Deux bugs corrigés en chemin :
  - `sys.stdin.read()` bloquait sur stdin non fermé → passage aux lignes ;
  - le bot quittait après un tour → timeout dès le tour suivant ; boucle ajoutée.
- Validation officielle : **4 test games gagnés** contre St Nicolas (boss Wood 2,
  agent `2194292`), M Qasim Aslam (rang 1), aymané212 (rang 2) et vexilla72
  (rang 3) ; scores `0=1 1=0` à chaque fois.
- Le port C++ a également gagné contre le boss (`0=1 1=0`).
- Conclusion : un BFS court + une poussée greedy suffisent en Wood. Le vrai
  gain viendrait d'une évaluation à 2 coups (poussée adverse puis la nôtre),
  mais la complexité de portage est élevée pour un gain marginal à ce niveau.

## 2026-11-21 — Bronze : itérations

La version validée en Wood 2 (1 quête) passe en Bronze mais plafonne : elle
traite une quête sur notre tuile (`itemX=-1`) comme inaccessible et boucle
`PUSH`/`PASS` pendant une vingtaine de tours. Le log de test game confirme
la boucle (frames 45–88, `PUSH 5 LEFT/RIGHT` + `PASS`).

- `xmas_v3` corrige le rapprochement mais compte à tort une quête éjectée comme
  collectée : **0/5** victoires.
- `xmas_v4` modélise correctement le cycle tuile en main ↔ plateau et ajoute
  un BFS multi-quêtes sur 20 pas : **2/5** victoires (12–1, 12–3) et deux
  défaites 11–12.
- `xmas_v4b` renforce l'anti-éjection et le rapprochement : **1/5** victoire
  mais quatre défaites serrées 10–12.
- `xmas_v5` ajoute les poussées locales, l'évaluation par distance de Manhattan
  atteignable et l'anti-stale ; validation en cours.

## 2026-11-21 — Bronze : v6 (repli BFS) retenu

- `xmas_v5` plafonne à 2/5 victoires environ : dès que `multi_bfs` ne trouve pas
  d'état strictement meilleur, le bot joue `PASS` pendant tout le tour MOVE,
  même quand une quête est accessible. Les logs montrent des séries de `PASS`.
- `xmas_v6` ajoute un repli : si `multi_bfs` ne renvoie aucun mouvement et que
  des quêtes sont sur le plateau, un BFS simple vers la quête la plus proche
  fournit le chemin. Résultat : **8 victoires / 10 test games** contre le boss
  `2209776` (dont 12–5, 12–11, 12–8, 12–1, 12–10, 12–9).
- Port 4 langages terminé : fuzz **210/210** identique. Deux divergences
  découvertes pendant le port et corrigées dans C/C++/Rust :
  - `bfs_dist` renvoyait `distance + 1` par rapport à Python (off-by-one) ;
  - `index` de Python associe une position dupliquée au **dernier** indice,
    alors que les ports positionnaient tous les bits.

### Cause racine de la divergence C++ / Python (trouvée par replay même seed)
- Rejeu de `seed=-7503483396589402000` avec des copies debug des bots :
  Python parsait `targets=[(5,3),(2,4),(6,3)]`, C++ parsait `targets=[]`.
- Le C++ stockait **tous** les items (y compris `itemPlayerId=1`) puis, pour une
  quête, s'arrêtait au premier item de même nom. Les deux joueurs ayant des
  items aux mêmes noms, le C++ tombait systématiquement sur l'item adverse et
  abandonnait la quête. C et Rust filtraient déjà sur `pid == 0`.
- Correction : `if (pid != 0) continue;` dans la boucle de recherche d'item.
- Après correction, rejeu même seed : **12–9 dans les deux langages**, décisions
  identiques. Fuzz 210/210 toujours vert.
- Validation finale C++ : **3 victoires / 5** (12–11, 12–11, 12–5), 1 nulle,
  1 défaite ; soumission `41779754`.
