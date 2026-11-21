# Plan — Back Track King (Wood)

1. Récupérer énoncé, stub, adversaires et AI starter officiel ; identifier le
   boss Wood `6767604` et l’objectif de la ligue 1.
2. Sonde sur une vraie partie : journaliser les dimensions, le terrain, les
   villes et les états de cases, puis vérifier le comportement de `AUTOPLACE`
   avec des coordonnées fixes.
3. Écrire le bot Python :
   - lire les entrées comme un flux de **jetons** (tampon de lignes), et non
     comme un nombre fixe de lignes ;
   - mémorier `townId -> (x, y)` et toutes les paires demandées ;
   - choisir la paire minimisant `(Manhattan, id demandeur, id cible)` ;
   - émettre chaque tour la même commande
     `AUTOPLACE x1 y1 x2 y2`.
4. Porter en C, C++, Rust avec le même départage. C/C++ lisent déjà des jetons
   via `scanf` / `>>` ; Rust reçoit un lecteur de jetons avec `VecDeque`.
5. Générer 10 scénarios déterministes + 200 cas de fuzz (dimensions, terrain,
   villes et `desiredConnections` aléatoires, deux tours par scénario) et
   comparer les 4 implémentations via `tests/run.sh`.
6. Jouer 5 test games officiels par langage contre le boss Wood `6767604` ;
   absorber les 429 (20 parties / 300 s) en attendant le délai indiqué.
7. Rédiger `research.md` / `critique.md`, mettre à jour l’index des challenges
   et `steps.json`, puis committer et pousser.

Complexité : `O(townCount²)` pour le choix de la paire (au plus 12 villes),
puis `O(width × height)` jetons lus par tour. Aucun calcul de chemin : le moteur
calcule le chemin le moins cher via `AUTOPLACE`.
