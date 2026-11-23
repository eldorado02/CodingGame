# breakthrough — recherche

## Sources
- Énoncé officiel (`cg.py show breakthrough`) : 8×8, 16 pions chacun, avance droit (case vide) ou en diagonale (vide ou capture), pas de prise obligatoire, victoire en atteignant la ligne adverse ou en capturant tout. 1000 ms / 100 ms.
- Wikipedia Breakthrough (lien de l'énoncé) : jeu sans nul ; les programmes forts utilisent αβ/MCTS avec éval sur pièces, avancement, protection.
- Recherche opencode (deepseek-v4.1-flash-free) lancée le 2026-11-23 : voir `research-opencode-*.md` quand disponible.

## Faits vérifiés par test game
- Notre agent est toujours l'agent 0 et joue Blanc dans les test games observés (coup `None` au 1er tour).
- Vitesse serveur CG : ~1,2 M nœuds/s sur v1 (tableau), 60k nœuds en 10–36 ms sur v3.

## Classe (ALGO.md) : B — plateau abstrait, αβ + éval matériel/avancement/protection + règle des coureurs.


## Deep search opencode (hy4-flash-free) — 2026-11-23
Rapport complet : `research-opencode-20261123-190002.md` (non vérifié ligne à ligne ; à valider par test game).

Recommandation extraite :

> (pas de section recommandation détectée)
