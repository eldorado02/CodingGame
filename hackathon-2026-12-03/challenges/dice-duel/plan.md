# dice-duel — plan

1. Protocole : par tour `diceCount` puis `owner cell top front bottom back left
   right` ; sortie `B5 URRUL` (U = y+1, D = y-1, R = x+1, L = x-1).
2. Invariant : aucun (instantané complet).
3. Algorithme : chemins auto-évitants de longueur `top` (DFS U, D, R, L), sans
   traverser de dé, capture au dernier pas ; dédoublonnage (dé, case, faces).
   Negamax alpha-bêta, approfondissement itératif, 4000 nœuds, garde 30 ms
   testée à chaque nœud (un nœud = une génération coûteuse).
4. Départage : captures d'abord, ordre de génération, `>` strict.
5. Fuzz : 200 cas, 2 à 16 dés, orientations atteignables ; CG_NODES=300.
6. Wood 1 : capture seulement si les faces du dessus font 7 (SUM_RULE=true à
   la promotion, relire l'énoncé).
