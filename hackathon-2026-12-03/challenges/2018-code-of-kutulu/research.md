# Recherche — Code of Kutulu (2018-06-15)

## Sources

- Statement officiel : `statement.md` (API `/api/v1/puzzles/code-of-kutulu`).
- Postmortem 2ᵉ place (Agade) :
  https://github.com/Agade09/Agade-Code-of-Kutulu-Postmortem/blob/master/Agade_CoK_Postmortem.md
  — **lecture uniquement, aucun code copié**. Le bilan confirme que l'éval
  dominante est : rester à distance ≤ 2 d'un explorateur, fuir les minions.
- Socle DSA : `../../dsa/README.md` (BFS).
- Aucun code tiers copié.

## Mécanique (Wood 3)

Carte 10–24 × 10–20, 4 explorateurs, 250 sanity chacun.
Toutes les distances sont **Manhattan** sauf le déplacement (plus court
chemin via BFS dans le labyrinthe).

- Perte passive : 3 sanity/tour, **1 seulement** si un autre explorateur est à
  distance Manhattan ≤ 2 (murs ignorés) — réassurance mutuelle.
- Wanderers : apparaissent sur les spawns `w`, ciblent un explorateur
  (`param2 = id`), se déplacent vers lui ; au contact → **−20 sanity** et
  le wanderer disparaît.
- Actions : `WAIT` ou `MOVE x y` (un pas vers la cible via BFS).
- Victoire : **dernier** explorateur à atteindre 0 sanity.

## Décisions de conception

1. La réassurance économise 2 sanity/tour : c'est la priorité absolue.
   Rester à distance Manhattan ≤ 2 d'au moins un autre explorateur vivant.
2. Les wanderers coûtent 20 sanity d'un coup : maximiser la distance au
   wanderer le plus proche **sans rompre la réassurance**.
3. Explorer les 5 coups possibles (rester + 4 voisins non-murs) et noter :
   `100000 si réassuré + 100 * distance_wanderer - distance_explorer`.
4. Le premier entity du tour est toujours nous-même (`id` variable).

## Risques

| Risque | Mitigation |
|---|---|
| Perte de réassurance en fuyant | Poids dominant (100000) pour la réassurance. |
| Wanderer sur notre case | Distance 0 → score minimal, fuite prioritaire. |
| Spawn en cours (`param1=0`) | Ignoré : seulement les `WANDERER` actifs comptent. |
| Mur sur la case cible | Voixins filtrés par `grid != '#'`. |
| Timeout 50 ms | ≤ 480 cases, 5 coups, distances Manhattan → O(1). |
| Divergence entre langages | Score entier, fuzz. |
