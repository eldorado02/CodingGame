# Connect 4 — plan

## Objectif

Atteindre la Ligue Légende. Étape courante : valider Wood 2 avec la soumission
C++ `41786612`.

## Algorithme retenu

1. Lire `myId`, `oppId` ; notre pion vaut `myId + 1`.
2. Chaque tour : lire `turnIndex`, les 7 lignes, la liste des actions valides et
   l'action adverse précédente.
3. Si `-2` figure dans les actions valides, répondre `STEAL`.
4. Sinon, construire la table de transpositions et lancer une recherche
   alpha-beta itérative de profondeur 1 à 6 (arrêt anticipé sur victoire
   forcée). Le tri des coups est centre → bords.
5. Évaluation : somme des fenêtres de 4 entièrement occupées par un seul camp,
   poids `1,2,4,8` selon le nombre de pions ; bonus/malus `±2` pour la case
   centrale du haut ; `±(WIN - ply)` pour une victoire immédiate, `0` pour un
   plateau plein.
6. Écrire la colonne choisie avec un `flush` immédiat.

## Ports et tests

- 4 langages : Python3 (référence fuzz), C, C++, Rust.
- Fuzz différentiel : `bash tests/run.sh` → **200/200** identiques.
- Test games C++ : **5/5** contre `6797564` (deux séries : 4/5 puis 5/5 après
  correction de l'ordre des coups ; la version du dépôt a été reverifiée 5/5).
- Soumission C++ : `41786612`.

## Pièges rencontrés

- Le premier prototype testait le délai seulement tous les 1024 nœuds au
  niveau racine : à faible profondeur il ne s'arrêtait jamais et a timeout dès
  le tour 0. La version retenue n'a pas besoin de minuterie (profondeur fixe).
- Le bonus centre initial `+3 * (4 - height[4])` récompensait une colonne vide
  ; il a été remplacé par un test sur la case du haut.
- L'ordre des coups `4,3,5,...` omettait certaines colonnes dans un prototype
  intermédiaire à cause d'une formule de pas erronée ; la version du dépôt
  utilise l'ordre cyclique complet validé par le fuzz.
