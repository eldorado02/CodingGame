# lines-of-action — guide de reprise

- Énoncé : `statement.md` ; referee github.com/darkhorse64/CG_LinesOfAction. 8×8, 12 pions chacun ; déplacement = nombre de pions sur la ligne ; saut des amis, pas des ennemis ; capture en arrivant sur un ennemi. Gagne en connectant tous ses pions (8-voisinage) ; si un coup connecte les deux camps, le joueur qui joue gagne. 150 coups = nul. Ligues Wood 2 → Wood 1. Boss Wood 2 : `3861076`.
- Protocole : couleur ; 8 lignes (rangée 8 en haut) ; dernier coup ; N coups légaux (`e2e3`), `pass` si aucun. 1000 / 150 ms.
- Bot soumis : **v1** `41796817` — negamax αβ ID ; éval par camp = −(Σ distances de Chebyshev au centre de masse) − 10 × (groupes − 1) ; 20k nœuds (CG ≈ 22 ms, prof. 4).
- Bench : v1 3 nuls (boucle de répétition des deux bots jusqu'à 150 coups) ; v1 + anti-répétition 0/3 ; + poids groupes 25 : 0/3 → retour à v1 pour la 1re soumission (nul > défaite).
- Prochaine étape : évaluateur Winands (MIA) — concentration normalisée, centralisation, quads/nombre d'Euler, mobilité — puis réactiver l'anti-répétition.
- Fuzz : `bash tests/run.sh` (CG_NODES=1500, 80 s).
