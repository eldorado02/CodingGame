# Recherche — Spring Challenge 2021 (2021-05-06)

## Sources
- Énoncé officiel : `statement.md`.
- Stub officiel : `cg.py stub spring-challenge-2021`.
- Source du jeu : https://github.com/CodinGame/SpringChallenge2021 (lecture seule).

## Mécanique (Wood 4 = première ligue)
- Une seule journée (`day = 0`), 37 cases hexadécimales.
- Chaque joueur démarre avec quelques arbres de taille 3 ; chaque arbre donne
  `3` sun points au début de la journée.
- `COMPLETE index` coûte `4` sun, retire l'arbre et donne
  `nutrients + bonus(richness)` points (`+0/+2/+4`). `nutrients` baisse de 1
  par complétion (2 si les deux joueurs complètent le même tour).
- À la fin : `+1` point par tranche de 3 sun restants ; départage au nombre
  d'arbres restants.

## Stratégie retenue
1. Compléter tant que `mySun >= 4` et qu'il reste un arbre de taille 3.
2. Choisir l'arbre de plus grande `richness`, départage par index croissant :
   le bonus est fixe, mais complôt tôt maximise le `nutrients` courant.
3. Sinon `WAIT`.

## Risques
| Risque | Mitigation |
|---|---|
| Compléter trop tard (nutrient épuisé par l'adversaire) | Compléter dès le premier tour. |
| Gaspiller le soleil | Arrêt dès que `sun < 4`. |
| Divergence 4 langages | Même clé `(-richness, index)` + fuzz différentiel. |
