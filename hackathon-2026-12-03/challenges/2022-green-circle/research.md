# Recherche — Green Circle (2022-06-16)

## Sources
- Énoncé officiel : `statement.md`.
- Stub officiel : `cg.py stub green-circle`.
- Diagnostic via une test game temporaire : les `cardLocations` ne contiennent
  que `HAND`, `DRAW`, `DISCARD` et `OPPONENT_CARDS` ; les cartes présentes sur
  les 8 bureaux ne sont **pas** exposées.

## Mécanique (Wood 4 = première ligue)
- Deckbuilding à 2 joueurs, 8 bureaux circulaires, déplacement obligatoire.
- La main contient 4 cartes tirées au début du tour ; le déplacement ajoute une
  carte de compétence du bureau choisi (ou un BONUS si le bureau est vide).
- Une carte de compétence donne 2 points de compétence du type correspondant et
  2 points « bâclés » ; une carte BONUS donne 1 point flexible et 1 point bâclé.
- Chaque release consomme la main entière ; les points bâclés utilisés ajoutent
  des cartes Technical Debt au deck.
- La 5e application ne peut pas utiliser de points bâclés.
- Fin dès qu'un joueur sort sa 5e application ; départage au nombre de cartes
  Technical Debt.

## Stratégie retenue
1. En phase `MOVE`, évaluer chaque destination possible en ajoutant virtuellement
   la carte du bureau à la main.
2. Score d'une application = somme de `min(besoin, 2 * cartes du type)` plus les
   BONUS flexibles restants (plafonnés au besoin total).
3. Choisir le bureau qui donne le meilleur score d'application, départage par
   identifiant de bureau croissant.
4. En phase `RELEASE`, le moteur fournit la liste exacte des applications
   réalisables ; choisir celle avec le meilleur score, départage par identifiant
   croissant. Le moteur garantit déjà la règle de la 5e application.

## Risques
| Risque | Mitigation |
|---|---|
| Compter à tort les points bâclés | On ne fait que classer des releases déjà légales. |
| Bureau vide (carte BONUS à la place) | Non observable ; le score BONUS est inclus dans l'évaluation. |
| Divergence 4 langages | Même score entier et mêmes départages + fuzz différentiel. |
| Timeout 50 ms | 12 applications et 8 destinations au plus, sans recherche. |
