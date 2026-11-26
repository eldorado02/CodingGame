# 6-nimmt-6-qui-prend-take-5 — critique et journal des tentatives

Format : constat / hypothèse / sources / action / résultat / leçon.

### 2026-11-26 — 6 nimmt! / 6 qui prend! / Take 5! — v1 glouton coût espéré binomial
- Constat : 1er bench : disqualifié tour 1 ('PICK 0' au lieu de PLAY)
- Hypothèse : ordre des lignes d'entrée différent de l'énoncé
- Sources : sonde stdin brut (select + RANDOM) ; repos hub2/nimmt6, TimothyJones/6nimmt-bot (gh_search)
- Action : phase lue en dernière ligne du tour (4 langages + gen_fuzz)
- Résultat : fuzz 200/200 ; 4/4 parties 2e place (bat 2 joueurs rang ~108, perd contre rang 2) ; soumis
- Leçon : sonder le stdin brut avant d'écrire le parsing ; piste v2 : Monte-Carlo sur les mains adverses
