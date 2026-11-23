# dice-shogi — critique et journal des tentatives

### 2026-11-23 — Dice Shogi — v1 gloutonne soumise
- Constat : jeu jamais joué ; minishogi + dé imposant la colonne
- Hypothèse : le glouton de minishogi suffit pour une 1re soumission
- Sources : énoncé, referee jdermont/DiceShogi cité
- Action : copie de minishogi avec ligne de dé, fuzz 200/200
- Résultat : 1/3 contre le rang 40 ; soumission `41796284` (1re soumission autorisée sous 3/5)
- Leçon : le hasard du dé demande une anticipation des réponses adverses (expectimax), le glouton ne suffit pas
