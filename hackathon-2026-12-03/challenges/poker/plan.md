# poker — plan

1. Protocole (sonde 2026-11-26) : init 8 lignes (blinds, mains par niveau,
   multiplicateur, buy-in, 1re big blind, joueurs, id) ; par tour round,
   handNb, `stack chipInPot` par joueur, board `AD_QH_X_X_X`, nos cartes,
   actions passées, showdowns, actions possibles (CALL, CHECK, FOLD, ALL-IN,
   BET_x = relance minimale). Sortie `BET x`, `CALL`...
2. Invariant : aucun.
3. v2 : équité Monte-Carlo (400 tirages, xorshift32 graine = round x sel +
   main, identique dans les 4 langages), évaluateur 7 cartes ; tapis >= 80 %,
   relance min >= 62 %, check gratuit, suivre si équité > cote du pot, sinon
   coucher ; tapis à 50 % si tapis <= 10 grosses blinds.
4. Départage : déterministe (graine).
5. Fuzz : 200 cas, CG_NODES=25 tirages.
6. Résultat : v1 (paliers) 0/4 ; v2 2/4 contre boss Wood 2.
