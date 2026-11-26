# tulips-and-daisies — plan

1. Protocole : init `w h`, `costSoil costGrass costRocks costFlower`, `tulips
   daisies` (minuscules ; 1re lettre = code grille T/D) ; par tour `turnsLeft`,
   `gold opponentGold`, h lignes `SGRTD`. Sortie `row col`.
2. Invariant : aucun.
3. Algorithme v3 : glouton 1 coup ; récolte N = 1 + somme(run - 1), gain =
   somme des N premiers Fibonacci ; menace adverse (cases où il récolte avant
   notre coup) ; fenêtres de 4 ; potentiel trou (3 + trou + 3 = 33).
   Réserve d'or : 10 plantations sur herbe (or épuisé = défaite).
4. Départage : ordre ligne par ligne, `>` strict.
5. Fuzz : 200 cas, tailles 8..16, coûts du referee ou aléatoires.
6. Constat : boss Wood 2 = 374 points (tronçons 3 + trou + 3), nous 20 à 120.
   Piste v4 : évaluation de plateau entière (toutes nos récoltes possibles −
   meilleure récolte adverse) recalculée sur les lignes du coup ; casser leurs
   tronçons de 3 tôt (planter sur leur fleur, 10 or).
