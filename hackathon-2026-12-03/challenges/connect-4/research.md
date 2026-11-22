# Connect 4 — recherche

## Sources

- Énoncé officiel récupéré via `cg.py show connect-4` → `statement.md`.
- Opponents API : `BOSS 1` (`6797564`, Wood 2, `boss: true`), meilleur joueur
  `SafPlusPlus` (`6893191`, rang 1 Wood 2).
- Dépôt public du jeu : `https://github.com/AshKcg/cg-multi-connect4` ;
  `config/Boss.java`, `src/main/java/com/codingame/game/Referee.java` et
  `src/main/java/connectXgame/Connect4Board.java` ont été inspectés via
  l'API GitHub (le dépôt n'est pas cloné localement).
- Recherche GitHub API « connect 4 solver codingame » : aucun autre dépôt
  pertinent (0 résultat).
- arXiv API `all:"connect 4"` : 229 réponses, toutes hors sujet
  (géométrie/algèbre) ; aucune publication exploitable pour le plateau 7×9.
  Semantic Scholar API a répondu HTTP 429 (sans clé).
- Aucun post-mortem public utilisable trouvé : l'énoncé, le referee public et
  les logs de test games restent les sources primitives.

## Faits mécaniques retenus

- Plateau 7 lignes × 9 colonnes, victoire dès 4 pions alignés horizontalement,
  verticalement ou diagonalement.
- Le second joueur peut répondre `STEAL` à son premier tour : le pion unique de
  l'adversaire change de camp. C'est toujours beneficial en Wood 2 puisque
  l'adversaire n'a encore qu'un pion et que le trait passe au premier joueur.
- Chaque tour fournit `turnIndex`, 7 lignes de plateau, la liste des actions
  valides, puis l'action précédente de l'adversaire. La réponse doit être
  immédiate (`flush` obligatoire en C++ avec `sync_with_stdio(false)`).
- Limites : 1000 ms au premier tour, 100 ms ensuite ; 63 coups au maximum.

## Hypothèse validée

Une recherche alpha-beta à profondeur 6 avec évaluation fenêtre de 4 et tri
central des coups bat le boss officiel `6797564` sur 5 test games consécutifs.
La profondeur 7 a régressé à 0/5 dans les mêmes conditions (recherche
deterministe plus lente et horizon different), donc la version 6 est retenue.
