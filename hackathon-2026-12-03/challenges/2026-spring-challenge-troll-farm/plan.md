# Plan — Troll Farm (Wood)

1. Récupérer l’énoncé, le stub, la liste des adversaires et le moteur officiel
   (`cg.py stub` / `cg.py opponents` / dépôt `eulerscheZahl/Troll-Farm`) pour
   fixer le protocole exact, la durée (100 tours), les attributs Wood et le
   calcul du score.
2. Observer une vraie partie contre le boss Wood `6479367` avec un bot sonde
   qui journalise les entrées sur `stderr`, afin de confirmer le format des
   arbres, des trolls et les commandes jouées par le boss.
3. Écrire le bot Python :
   - lire la grille une seule fois, mémoriser la cabane `0` et les cases
     `GRASS` (`.`) comme seules cases praticables ;
   - lire exactement les lignes de chaque tour dans une boucle `while True` ;
   - BFS depuis la position réelle du troll vers toutes les cases herbeuses,
     en conservant la distance et le premier pas ;
   - si le troll porte un fruit : `DROP` dès que la distance de Manhattan à
     la cabane vaut 1, sinon `MOVE` d’un pas vers la case herbeuse adjacente
     à la cabane la plus proche (départage `distance, y, x`) ;
   - sinon : `HARVEST` si un arbre fruitier est sur la case courante, sinon
     `MOVE` d’un pas vers l’arbre fruitier atteignable le plus proche
     (départage `distance, -fruits, y, x`) ;
   - `WAIT` si aucun fruit n’est disponible.
4. Porter en C, C++, Rust avec le même ordre de voisinage
   `(N, E, S, O)` et le même départage, pour garantir des sorties bit à bit
   identiques. Chaque langage garde ses idiomes : tableaux statiques et BFS à
   file explicite en C, `std::tuple` / `std::queue` en C++, `VecDeque` et
   tuples en Rust.
5. Générer 10 scénarios déterministes + 200 cas de fuzz (grille 16×8, deux
   cabanes symétriques, arbres et deux trolls par camp) ; comparer les 4
   implémentations via `tests/run.sh`.
6. Jouer des test games officiels contre le boss Wood `6479367` dans les 4
   langages, puis quelques parties contre d’autres bots Wood ; absorber les
   erreurs 429 par le délai indiqué par l’API.
7. Mettre à jour `research.md`, `critique.md`, l’index des challenges et
   `steps.json`, puis committer et pousser.

Complexité : `O(width × height)` en temps et en mémoire par tour, soit 128
cases en ligue Wood, très en deçà des 50 ms autorisées.
