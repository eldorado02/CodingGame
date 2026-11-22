# Elemental Wars — plan

## Objectif

Atteindre la Ligue Légende. Étape courante : battre le boss Wood 2 `6697631`
puis gravir les ligues.

## Algorithme retenu (v3)

1. Lire l'init (tribu, taille, grille), puis chaque tour les scores et les 6/9/12
   élémentaux (id, tribu, x, y, prisonnier).
2. Calculer une carte BFS de distances depuis **chaque** ennemi non-prisonnier ;
   les stocker séparément selon qu'il est proie ou prédateur.
3. Pour chaque élémental allié non-prisonnier, évaluer 5 candidats (rester +
   4 voisins dans l'ordre N,E,S,O) :
   `-100*min(dp,20) + 15*min(dq,8)`, `+10000` si `dp==0`, `-12000` si `dq==0`,
   `-2000` si `dq<=1 && dp>1`.
4. Départage déterministe : score décroissant, puis `x` croissant, puis `y`
   croissant.
5. Émettre une commande par élémental, séparées par `;`, avec flush.

## Ports et tests

- 4 langages : Python3 (référence fuzz), C, C++, Rust.
- Fuzz différentiel : `bash tests/run.sh` → 200 cas avec 6/9/12 élémentaux.
- Test games C++ v3 : **5/5** contre `6697631,6699103`.

## Pièges rencontrés

- Le fuzz Wood 3 ne générait que 3 élémentaux : il n'a pas détecté le débordement
  des tableaux de taille 8 quand Wood 2 passe à 9/12 élémentaux. Le générateur
  couvre maintenant 6/9/12 élémentaux et les deux tailles de plateau.
- Un seul élémental commandé laissait les autres immobiles : il faut émettre une
  commande pour chaque élémental allié non-prisonnier.
- La lecture bloquante (`stdin.read()`) timeout au frame 1 : lire ligne à ligne.
