# counting-tictactoe — guide de reprise

- Énoncé : `statement.md` ; referee lu : github.com/RezaSi/counting-tic-tac-toe (`TicTacToeGrid.countScore` : toutes les fenêtres glissantes de 3, une ligne de 4 compte 2). 10×10, plateau rempli entièrement, 2 manches couleurs inversées. Ligues Wood 3 → Wood 1. Boss Wood 3 : `3856980`.
- Protocole : `oppRow oppCol` (`-1 -1`), N cases valides (= cases vides, ordre mélangé). Plateau reconstitué : occupé = absent de la liste ; nos cases = notre mémoire, remise à zéro si une de nos cases redevient vide (nouvelle manche ; `lastAction` n'est pas remis à zéro par le referee).
- Bot soumis : **v2** `41796704` — score de motifs incrémental (fenêtres : 1/10/100 si non contestées), αβ sélectif (nœuds internes = 10 meilleurs coups par gain immédiat, racine = tous), 2500 nœuds (CG 5–11 ms, prof. 4).
- Bench : v1 (éval complète, prof. 2) 0/3 ; v2 sans calibrage 2/3 avec TIMEGUARD ; **v2 calibrée 3/4** contre le boss Wood 3.
- Prochaine étape : budget ×3 (marge large), poids des motifs ajustés (parité de remplissage).
- Fuzz : `bash tests/run.sh` (flux auto-joués avec changement de manche, CG_NODES=300, ~2 min).
