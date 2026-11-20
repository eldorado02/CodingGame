# Recherche — Fall Challenge 2020 (2020-11-12)

## Sources
- Énoncé officiel : `statement.md`.
- Stub officiel : `cg.py stub fall-challenge-2020` (format des 11 champs par ordre).

## Mécanique (Wood 4 = première ligue)
- 2 joueurs, 4 niveaux d'ingrédients, inventaire initial `[10,10,10,10]`.
- Jusqu'à 5 client orders disponibles par tour, chacune avec 4 `delta` ≤ 0 et
  un `price` entre 6 et 23.
- Une action par tour : `BREW id` (consomme les ingrédients, gagne `price`) ou
  `WAIT`. Les deux joueurs peuvent brasser la même commande et gagner tous deux.
- Fin de partie dès qu'un joueur a brassé 2 potions (ou après 100 tours).

## Format d'entrée (piège)
- Ligne d'ordre : `actionId actionType delta0 delta1 delta2 delta3 price
  tomeIndex taxCount castable repeatable`.
- Le prix est le **7e** champ (index 6), pas le 6e ; une première version du bot
  lisait `delta3` comme prix et perdait contre le boss.

## Stratégie retenue
1. Chaque tour, filtrer les ordres `BREW` dont les besoins (`-delta`) tiennent
   dans notre inventaire.
2. Choisir le prix maximal ; départage `sum(besoins)` croissant puis `id` croissant.
3. Brasser immédiatement : la partie s'arrête dès l'adversaire à 2 potions,
   attendre est donc risqué.

## Risques
| Risque | Mitigation |
|---|---|
| Champ prix mal indexé | Documenté et vérifié avec le stub officiel. |
| Inventaire insuffisant pour le 2e brew | Départage par consommation minimale. |
| Divergence 4 langages | Même clé `(-price, somme, id)` + fuzz différentiel. |
