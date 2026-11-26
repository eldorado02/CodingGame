# dice-duel — critique et journal des tentatives

Format : constat / hypothèse / sources / action / résultat / leçon.

### 2026-11-26 — Dice Duel — v1 alpha-bêta matériel, génération DFS
- Constat : jeu non soumis ; budget 8000 nœuds = TIMEGUARD 38x et 45 ms
- Hypothèse : coût par nœud très variable (16 dés), garde horloge trop espacée
- Sources : referee eulerscheZahl/Dice-Duel ; logs d= n= ms=
- Action : 4000 nœuds, garde 30 ms à chaque nœud ; v1d cassé par un sed (commentaire inline) puis corrigé
- Résultat : fuzz 200/200 ; 3/4, 3/3, 3/3 contre boss Wood 2, max 31 ms ; soumis
- Leçon : horloge à chaque nœud si nœud lourd ; jamais de sed pour insérer un commentaire sur une ligne de code

### 2026-11-26 — Dice Duel — v1.1 règle des 7 en Wood 1
- Constat : promu Wood 1 (8.74, rang 94) avec SUM_RULE=false : captures invalides = défaites
- Hypothèse : activer la règle du referee league level 2
- Sources : Board.java playMove (getLeagueLevel()==2)
- Action : SUM_RULE=true dans les 4 langages
- Résultat : fuzz 200/200 ; 5/5 contre le milieu de Wood 1 ; re-soumis
- Leçon : préparer les constantes de règle de la ligue suivante dès la v1, les basculer à la promotion
