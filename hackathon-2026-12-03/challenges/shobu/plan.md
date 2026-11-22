# Plan — Shobu

## Algorithme

1. Lire la position et les actions légales fournies par le referee.
2. Si `SWAP` est disponible, décider selon l'évaluation de la position après
   le premier coup noir : garder sa couleur ou échanger.
3. Générer tous les coups légaux (referee `Board.legalMoves`).
4. Pour chaque coup :
   - s'il vide un board adverse, le jouer immédiatement ;
   - sinon simuler, puis évaluer la meilleure réponse adverse (un ply) ;
   - score = progression matérielle + pression sur le board adverse le plus
     faible + sécurité (ne pas offrir de victoire immédiate).
5. Jouer le meilleur coup ; départage déterministe par la chaîne d'action.

## Évaluation

-victoire = +1e9, défaite = -1e9.
- Pour chaque board : écart de stones, avec un bonus fort quand un board
  adverse approche de zéro stone (`4 - count` au carré).
- Pénalité si notre board le plus faible approche de zéro.

## Complexité

Génération ~1 000 coups au pire ; avec réponse adverse, on ne considère que
les meilleurs coups selon un pré-score (beam de largeur 24) pour rester sous
100 ms en Python.
