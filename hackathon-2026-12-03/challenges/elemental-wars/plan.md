# Elemental Wars — plan

## Objectif

Atteindre la Ligue Légende. Étape courante : consolider Wood 1 (64 joueurs,
pas de boss) puis progresser vers Bronze.

## Algorithme retenu (v6)

1. Lire l'init (tribu, taille 11/13/15, grille), puis chaque tour les scores et
   les 6/9/12/15 élémentaux (id, tribu, x, y, prisonnier).
2. Calculer une carte BFS de distances depuis chaque ennemi libre ; les séparer
   en proies / prédateurs.
3. **Prédiction** : pour chaque ennemi libre, trouver sa proie libre la plus
   proche (distance de Manhattan), calculer un BFS depuis cette cible, puis
   simuler un pas selon la règle officielle (plus court chemin, priorité
   N,E,S,W). Stocker les cases prévues des proies (`preyNext`) et des
   prédateurs (`predNext`).
4. Pour chaque allié libre (traité dans l'ordre), évaluer 5 candidats (rester +
   4 voisins) :
   ```
   score = -100*min(dp,20) + 20*min(dq,8)
           +12000 si la case est dans preyNext
           -18000 si la case est dans predNext
           -20000 si dq==0, -4000 si dq==1
           -5000 par allié déjà assigné à la même case
   ```
   `dp`/`dq` = distances minimales aux proies / prédateurs courants.
5. Libération : si nous avons des prisonniers et plus d'un allié libre,
   l'allié le plus proche du camp du prédateur vise ce camp (`≤14` pas) avec
   `+3000*prisonniers` sur la case du camp.
6. Départage déterministe : score décroissant, puis `x` croissant, puis `y`
   croissant. Sortie `id MOVE x y` / `id WAIT`, `;` séparateurs, flush.

## Justification des choix

- La prédiction exploite une mécanique **déterministe** du referee : le
  déplacement suit toujours un plus court chemin avec la priorité N,E,S,W.
- L'anti-regroupement évite la défaite instantanée (un prédateur capture toute
  une pile) sans interdire les captures à deux alliés.
- La libération restaure les unités perdues ; elle est désactivée s'il ne
  reste qu'un seul allié libre, pour ne pas risquer la fin de partie.

## Ports et tests

- 4 langages : Python3 (référence fuzz), C, C++, Rust.
- Fuzz différentiel : `bash tests/run.sh` → 200 cas avec 11/13/15 et
  6/9/12/15 élémentaux.
- Test games C++ v6 : 5/5 contre `6918338,6758748` (notre ancienne version et
  le rang 40) ; 2/5 contre `6710922,6788011` (les deux meilleurs).
- Soumission : `41788154`.

## Pièges rencontrés

- La version Wood 3 timeout en Wood 2 à cause de tableaux trop petits.
- `id` est unique par tribu, pas globalement.
- La lecture globale bloque au premier tour.
- Le fuzz calibré sur la ligue inférieure ne couvre pas `mapSize=15`.
