# Shobu — plan

## Objectif

Atteindre la Ligue Légende. Étape courante : valider Wood 3 contre le boss MCTS
`6906535`, puis se préparer à Wood 2 (minimax adverse).

## Algorithme retenu

1. Lire l'état, générer les actions légales exactement comme
   `Board.legalMoves()` du referee (ordre de génération conservé).
2. Si `SWAP` est disponible, le jouer (le boss ne swap jamais).
3. Recherche alpha-beta itérative profondeurs 1→4 avec **budget global de
   20 000 évaluations** (déterministe, donc identique dans les 4 langages).
4. À chaque nœud, trier les coups par évaluation immédiate décroissante, puis
   par ordre lexicographique `px, py, ax, ay, dx, dy` ; garder 24 coups à la
   racine et 16 aux nœuds internes.
5. Évaluation :
   `35*(matériel) + 420*(4-min adverse)^2 - 320*(4-min nous)^2`
   `+ 18*Σ(bord par plateau) + 30*(bords adverses - nos bords)`
   `+ 45*(coins adverses - nos coins)` ; `±WIN` si un plateau est vidé.
6. Si le budget s'épuise pendant une profondeur, conserver le meilleur coup de
   la profondeur précédente.
7. Écrire le coup avec `flush`.

## Ports et tests

- 4 langages : Python3 (référence fuzz), C, C++, Rust.
- Fuzz différentiel : `bash tests/run.sh` → **200/200** identiques.
- Test games C++ Wood 3 : budget 18 000 → 3V+2N ; budget 20 000 → **4V+1N** ;
  budget 26 000 → timeout. Version retenue : 20 000.
- Soumission C++ : à venir.

## Pièges rencontrés

- Le budget temps (82 ms) battait le boss 5/5 mais rendait le fuzz
  dépendant de la vitesse de chaque langage.
- Le budget de nœuds (200) ne comptait pas le coût réel de génération/tri des
  coups : ~50 000 évaluations, ~180 ms, timeout en test game.
- Le budget d'évaluations (20 000) borne directement le travail coûteux et
  reste parfaitement déterministe.
