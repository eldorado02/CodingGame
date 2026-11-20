Tu es le sous-agent de DEEP SEARCH pour le challenge CODINGAME {YEAR} — {CHALLENGE_NAME} (`{CHALLENGE_ID}`). Réponds et écris en FRANÇAIS.

À FAIRE :
1. Récupérer l'énoncé officiel du challenge via l'API publique :
   `GET https://www.codingame.com/api/v1/puzzles/{CHALLENGE_ID}` (champ `statement`).
   Sauvegarder le texte brut (markdown) dans le fichier statement.md ci-dessous.
   compléter avec webfetch (blog CodinGame, GitHub solutions) si le texte est incomplet.
2. Faire une recherche approfondie : stratégies gagnantes, contraintes de performance, pièges classiques, solutions open-source de référence, benchmarks de la communauté.
3.Identifier les algorithmes du socle DSA qui s'appliquent (voir /home/arafaram/Projects/codingGame/hackathon-2026-12-03/dsa/README.md).

FICHIERS À ÉCRIRE (chemins absolus) :
- /home/arafaram/Projects/codingGame/hackathon-2026-12-03/{CHALLENGE_DIR}/statement.md
- /home/arafaram/Projects/codingGame/hackathon-2026-12-03/{CHALLENGE_DIR}/research.md
  → avec : résumé du jeu, entrées/sorties exactes, contraintes, stratégies trouvées, sources URL, algorithmes du socle à réutiliser, recommandation d'approche.

Ne modifie aucun autre fichier. Ne lance pas d'autre sous-agent. Ne soumets rien à CodinGame. Ne lis jamais ~/.config/codingame/token.
