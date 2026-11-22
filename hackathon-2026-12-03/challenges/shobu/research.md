# Recherche — Shobu

## Sources

- Énoncé officiel CodinGame (`cg.py show shobu`), ligues Wood 4 → Wood 1.
- Referee officiel : https://github.com/Waffle3z/CG-Shobu
  - `src/main/java/com/codingame/game/shobu/Board.java` : règles pures,
    génération des coups, simulation, victoire.
  - `Referee.java` : protocole, pie rule, limites de temps.

## Mécanique confirmée par le referee

- Plateau 8×8 = quatre boards 4×4 ; `board = 2*(y/4) + x/4`.
- Boards pairs = dark (x<4), impairs = light (x≥4).
- Noir (0) : home boards 2 et 3 ; blanc (1) : home boards 0 et 1.
- Un coup = déplacement passif sur un home board, puis déplacement agressif
  de même vecteur sur un board de l'autre couleur.
- Un coup passif doit rester sur le même board et ne rencontrer que des cases
  vides.
- Un coup agressif peut pousser exactement un stone adverse ; le stone poussé
  finit une case au-delà de l'arrivée. Hors du board 4×4 ⇒ stone retiré.
- Victoire de rond : un board adverse vidé, ou adversaire sans coup légal.
- `SWAP` uniquement au premier tour blanc de chaque rond.

## Boss Wood 4

Greedy : prend un coup gagnant immédiat, sinon le coup qui retire le plus de
matériel, sans considérer la réponse. Une évaluation avec réponse adverse doit
donc suffire à le battre.
