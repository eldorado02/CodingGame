# Elemental Wars — recherche

## Sources

- Énoncé officiel récupéré via `cg.py show elemental-wars` → `statement.md`.
- Opponents API : `6910638` (`A0-0`, rang 1 Wood 3) et `6697588` (`BOSS 1`, Wood 3).
- Aucun dépôt public de referee ou de bot n'a été trouvé via l'API GitHub de
  recherche pour « elemental wars codingame » ; l'énoncé et les logs de test
  games sont donc les sources primitives.

## Faits mécaniques retenus

- Cycle : `W → F → P → W`. Pour notre tribu `T`, la proie est `PREY[T]` et le
  prédateur est `PRED[T]`.
- Une capture est résolue **simultanément** après les déplacements : si nous
  partageons une case avec la proie, nous marquons un point ; si nous partageons
  celle du prédateur, nous sommes capturés.
- Le déplacement est d'une case vers une cible, chemin le plus court, priorité
  nord → est → sud → ouest. Il suffit donc d'émettre `id MOVE nx ny` avec
  `(nx, ny)` voisin choisi par notre propre BFS.
- En Wood 3, `elementalsCount = 3` : un élémental par tribu, plateau 11×11 avec
  15 % de murs. La partie s'arrête dès qu'un élémental est prisonnier.
- Un élémental prisonnier ne bouge plus et ne participe pas aux captures : il
  doit être ignoré comme prédateur/proie.

## Hypothèse validée

Fuir le prédateur (distance BFS ≥ 2) avant de poursuivre la proie. La pondération
`score = -100 * dist(proie) + 20 * min(dist(prédateur), 6)` avec une pénalité
de `-2000` si le prédateur est à distance ≤ 1 et la proie à distance > 1 a donné
**5/5** en test games contre `6910638` et `6697588`.
