# Plan — A*Craft

## Algorithme commun

### Init

Lire 10 lignes de 19 caractères. `arrow[y][x] = 0..3` pour `URDL`
préexistantes, `-1` sinon (case libre) ; `void[y][x]` pour `#`.
Lire `robotCount`, puis les `(x, y, dir)` des robots.

### Construction (une passe, ordre `order`)

`visits[x][d]` = nombre de fois où l'état `(case, direction d)` a été
enregistré. Pour chaque robot de `order` :

1. `d = arrow[y][x]` si flèche préexistante, sinon direction fournie.
   Enregistrer l'état initial `(x, y, d)`.
2. Tant que le nombre de pas < 760 :
   - `nx = (x + dx[d]) mod 19`, `ny = (y + dy[d]) mod 10` ;
   - si `(nx, ny)` est `#` → arrêt ;
   - choisir `nd` parmi `0..3` tels que `visits[nx][nd] == 0` :
     - si `(nx, ny)` a une flèche fixe → `nd` imposé ;
     - sinon évaluer chaque `d2` : `look = sum(visits[(nx,ny)+d2])`,
       clé `(look, d2)` ; le minimum gagne ; si `d2 != d`, poser la flèche
       `(nx, ny, d2)` ;
   - si aucun `nd` valide → arrêt ;
   - enregistrer `(nx, ny, nd)`, `x,y,d = nx,ny,nd`.
3. Accumuler les pas dans le score de cette passe.

On exécute la construction avec `order = [0..n-1]` puis avec l'ordre
inversé ; on garde la passe au meilleur score (égalité → ordre original).

### Output

Une seule ligne : tous les triplets `x y D` des flèches posées, séparés
par des espaces. Aucun output les tours suivants (le referee n'en lit plus).

### Cas de test

- `01_row.in` : couloir de 5 cases, 1 robot → flèches aux extrémités.
- `02_full.in` : grille pleine 19×10, 1 robot → serpent, score ≫ 19.
- `03_fixed.in` : flèche préexistante, jamais écrasée.
- `04_void.in` : robot adjacent à un trou → direction détournée.
- `05_multi.in` : 3 robots sur une même plateforme.
- `06_wrap.in` : robot sur le bord, test du torus.

## Livrables

`solutions/{c,cpp,rust,python}`, `tests/gen_cases.py`, `tests/*.in`,
`tests/run.sh`, `critique.md`.
