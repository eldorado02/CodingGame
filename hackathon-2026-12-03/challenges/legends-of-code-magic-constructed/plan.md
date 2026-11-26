# legends-of-code-magic-constructed — plan

1. Protocole : 2 lignes joueur (hp, mana, deck, draw), `opponentHand
   opponentActions` + lignes d'actions, `cardCount` + lignes carte (13 champs).
   Tour de construction = mana 0 : `CHOOSE baseId` x30 (baseId = cardNumber,
   referee1.5 ConstructPhase.java). Combat : `SUMMON/ATTACK/USE/PASS` séparés par `;`.
2. Invariant : aucun (pas de mémoire).
3. Algorithme : valeur = corps + capacités + effets (x2 si Area) ; deck = 15
   cartes x2 au meilleur ratio valeur/(coût+1), <= 3 objets, <= 2 cartes >= 7.
   Combat : sous-ensemble de créatures de valeur max (2^8), objets, gardes puis tête.
4. Départage : ratio puis numéro ; id d'instance partout ailleurs.
5. Fuzz : construction (120 cartes) + 0 à 15 instantanés de combat.
6. Hypothèse : glouton suffit contre le milieu. Résultat : 4/4 contre rang 57/114.
