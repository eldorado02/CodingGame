# Brandubh — critique

## Avant implémentation

- Le premier prototype ne lisait qu'un tour puis quittait : timeout au frame 3.
- L'évaluation initiale utilisait une distance Manhattan au coin, ce qui
  sous-estime la capacité du roi à fuir en ligne droite sur une longueur libre.
- Le BFS d'encerclement appelé à chaque feuille coûtait ~46 ms ; il a été retiré
  de l'évaluation pour respecter la limite de 50 ms, puis réintroduit avec une
  garde terminale peu coûteuse.

## Après implémentation

### Tentatives rejetées

- `v1` (alpha-beta 2 plis, BFS d'encerclement à chaque feuille) : timeout.
- `v2` (sans BFS d'encerclement, distance Manhattan) : 0/5, le roi s'échappe.
- `v3` (poids distance 300, contrôle 100) : 0/5, le roi s'échappe en ~30 tours.
- `v4` (BFS d'évasion) : le BFS laissait traverser les défenseurs ; 0/5.
- `v5` (proximité des attaquants avec mauvais signe) : 0/5, repousse les attaquants.
- `v6` (BFS avec murs + proximité corrigée) : en cours de validation.

## Leçons

- Corriger d'abord la boucle de lecture avant d'attribuer les défaites à l'évaluation.
- Une évaluation doit être testée sur une position où le roi a une sortie évidente,
  pas seulement sur la position initiale.
- Le quota `30 test games / 900 s` est vite épuisé quand on balaie des variantes ;
  regrouper les modifications indépendantes.
