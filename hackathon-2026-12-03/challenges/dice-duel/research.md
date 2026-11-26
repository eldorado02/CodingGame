# dice-duel — recherche

## Sources
- Referee : https://github.com/eulerscheZahl/Dice-Duel (engine/Die.java, Board.java,
  Move.java, Referee.java), lu le 2026-11-26.

## Faits du referee
- rotateUp = (1,2,3,0,4,5), rotateRight = (4,1,5,3,2,0) ; D et L inverses.
- `move()` : U = y+1 (listMoves étiquette ses chemins à l'envers, mais seul
  `move()` compte pour la validité).
- League level 2 (Wood 1) : capture invalide si top roulé + top capturé != 7 ;
  pas de coup = défaite ; score = dés restants.
