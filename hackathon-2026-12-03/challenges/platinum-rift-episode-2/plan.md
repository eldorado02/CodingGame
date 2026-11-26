# platinum-rift-episode-2 — plan

1. Protocole : init `playerCount myId zoneCount linkCount`, zoneCount lignes
   `id 0` (brouillard), linkCount liens. Par tour : stock, puis zoneCount lignes
   `id owner p0 p1 visible platinum`. Sortie : `n from to ...` ou `WAIT`, puis
   une 2e ligne `WAIT` obligatoire.
2. Invariant : QG (zones possédées au 1er tour), distances BFS au QG ennemi,
   platine vu par zone.
3. Algorithme : par zone (id croissant) : rester si combat ; garde du QG =
   ennemis adjacents + 1 ; un pod (ou ennemis + 1) vers chaque voisin non à
   nous (platine, distance, id) ; reste d'un pas vers le QG ennemi.
4. Départage : tri stable (platine desc, distance asc, id asc).
5. Fuzz : 200 cartes hexagonales (liens dupliqués), 1 à 20 tours aléatoires.
6. Hypothèse : s'étaler + marcher sur le QG bat le milieu. Résultat : 5/7.
