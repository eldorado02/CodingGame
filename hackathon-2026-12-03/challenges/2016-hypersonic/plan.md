# Plan — Hypersonic (Wood 3)

## Algorithme commun

Init : `width height myId`. Puis chaque tour : 11 lignes de grille (`.` ou
`0`), puis `entities`, puis `entities` lignes `type owner x y param1 param2`.

### Par tour

1. Lire grille et entités. Repérer ma position (`type==0 && owner==myId`) et
   mon nombre de bombes disponibles (`param1`).
2. Marquer les cases occupées par une bombe (toutes, alliées et ennemies) dans
   `blocked`.
3. **Si je peux poser une bombe** (`bombs > 0`) et qu’une boîte est dans ma
   croix de portée 3 (cases non bloquées avant la boîte) :
   - choisir la direction de fuite la plus éloignée des bombes (BFS vers une
     case à distance ≥ 4 de la bombe) ;
   - output `BOMB fuite_x fuite_y`.
4. **Sinon** : BFS depuis ma position (cases traversables = `.` sans bombe)
   vers la boîte accessible la plus proche. Output `MOVE casesuivante_x
   casesuivante_y`. Si aucune boîte accessible, `MOVE` vers le centre.
5. `flush`.

### Portée de bombe

Depuis (x,y), pour chaque direction, avancer case par case jusqu’à 3 :
- si case = bombe → inclure la case puis s’arrêter ;
- si case = boîte → inclure la case puis s’arrêter (et c’est un score) ;
- sinon continuer.

### BFS

File sur 13×11, `dist` initialisé à -1. Voisins 4-directions, bornes,
`grid=='.'`, `!blocked`. Pour trouver la boîte cible, on s’arrêt dès qu’un
voisin est une boîte (on ne peut pas entrer dessus) : la cible est la case
voisine de la boîte. On reconstruit le premier pas via un tableau `parent`.

### Cas de test

- `01_empty.in` : grille vide, aucune bombe → MOVE centre.
- `02_box_adjacent.in` : boîte juste à droite → BOMB (fuite à gauche).
- `03_box_wall.in` : boîte derrière une autre → BOMB (portée inclut la 1re).
- `04_bomb_block.in` : bombe sur la case voisine → MOVE contourne.
- `05_no_bomb_left.in` : `param1=0` → MOVE vers boîte.
- `06_chain.in` : deux bombes alignées → BOMB si boîte à portée.

## Livrables

- `solutions/c/bot.c`, `solutions/cpp/bot.cpp`, `solutions/rust/bot.rs`,
  `solutions/python/bot.py`.
- `tests/gen_cases.py`, `tests/*.in`, `tests/run.sh`.
- `critique.md` complété après validation + test game réel.
