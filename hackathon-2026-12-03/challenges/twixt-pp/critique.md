# twixt-pp — critique et journal des tentatives

Format : constat / hypothèse / sources / action / résultat / leçon.

### Checklist plan (PLAYBOOK §3)
- Ordre des champs (pegs puis liens, moi puis lui). VALIDÉ (stub).
- Orientation lettre/chiffre et bords interdits. VALIDÉ (bot TidyMaze + 11 test games sans coup invalide).
- Création des liens identique au referee (PP : nos liens se croisent). VALIDÉ AVEC RÉSERVES : referee non trouvé, règle de l'énoncé appliquée.
- Budget : 30 ms au pire sur CG pour 300 ms. VALIDÉ.

### v1 — distance de cavalier + 2 plis, soumission `41796478`
- Constat : jeu jamais soumis.
- Hypothèse : éval distance (BFS 0-1) + 2 plis bat le milieu de tableau.
- Sources : énoncé, `TidyMaze/Twixt-PP` (bot aléatoire en Rust, sert de référence de protocole).
- Action : 4 langages en code « propre » (constantes nommées), fuzz 200/200.
- Résultat : 6/6 contre les rangs 65 et 49 (coups invalides de leur part), 0/5 contre les rangs 25 et 9 (défaites en 7–8 coups).
- Leçon : dans les jeux de connexion, une éval « distance » plate produit des égalités massives ; le départage par index de case pousse vers le bord de départ. Il faut un départage central ou une éval plus fine (nombre de chemins, résistance électrique) avant toute autre amélioration.
