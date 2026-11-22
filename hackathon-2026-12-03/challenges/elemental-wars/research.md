# Elemental Wars — recherche

## Sources

- Énoncé officiel récupéré via `cg.py show elemental-wars` → `statement.md`.
- Opponents API : en Wood 3 `6910638` et `6697588` ; en Wood 2 `6697631`
  (`BOSS 2`) et `6699103` (`derjack`, rang 1).
- Aucun dépôt public de referee ou de bot trouvé via GitHub pour ce challenge.
  L'énoncé et les logs de test games restent les sources primitives.

## Faits mécaniques retenus

- Cycle : `W → F → P → W`. Pour notre tribu `T`, la proie est `PREY[T]` et le
  prédateur est `PRED[T]`.
- Capture simultanée après déplacements : partager la case d'une proie donne
  +1 point ; partager celle du prédateur nous fait capturer (-1 pour nous).
- `id MOVE x y` déplace d'une case vers la cible. Émettre directement la case
  voisine choisie évite toute ambiguïté sur le tie-breaking du referee.
- En Wood 3 : 3 élémentaux (1 par tribu), plateau 11×11. En Wood 2 :
  **6, 9 ou 12 élémentaux**, plateau 11×11 ou 13×13, 50 ms/coup.
- Un élémental prisonnier ne bouge plus et doit être ignoré comme cible.

## Hypothèses validées

- Wood 3 : fuite prioritaire du prédateur, `5/5` contre `6910638,6697588`.
- Wood 2 : la version Wood 3 (tableaux de taille 8, un seul élémental commandé)
  déborde et timeout dès que `elementalsCount > 8`. La correction v3 (tous les
  élémentaux, tableaux dynamiques, BFS par ennemi) donne **5/5** contre
  `6697631,6699103`.
