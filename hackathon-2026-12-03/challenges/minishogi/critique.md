# minishogi — critique et journal des tentatives

Format : constat / hypothèse / sources / action / résultat / leçon.

### 2026-11-23 — Minishogi — v1 gloutonne soumise
- Constat : jeu jamais joué ; règles shogi (parachutages, promotions)
- Hypothèse : glouton capture/promotion + carte d'attaque adverse bat le milieu de tableau
- Sources : énoncé, referee jdermont/MiniShogi cité, test game sonde (orientation des coordonnées)
- Action : 4 langages, fuzz 200/200 (plateaux/actions aléatoires)
- Résultat : 3/3 contre le rang 61 ; soumission `41796267`
- Leçon : toujours sonder l'orientation des coordonnées (ici colonne 1 = droite) avant d'écrire l'éval
