# Plan — Olymbits

1. Récupérer l'énoncé, le stub et la liste des adversaires via l'API.
2. Lire le moteur officiel (`HurdleRace.java`, `Game.java`) pour fixer la
   mécanique exacte : arrivée en 29, étourdissement de 2 tours, fin anticipée,
   classement par position.
3. Écrire le bot Python : BFS sans collision, relancé à chaque tour.
4. Porter en C, C++ et Rust en conservant l'ordre des actions du BFS
   (`LEFT`, `DOWN`, `RIGHT`, `UP`) pour garantir des sorties identiques.
5. Générer 10 scénarios déterministes (pistes officielles + resets) et 200 cas
   de fuzz, comparer les 4 implémentations.
6. Jouer des test games contre le boss Wood et les meilleurs adversaires dans
   les 4 langages.
