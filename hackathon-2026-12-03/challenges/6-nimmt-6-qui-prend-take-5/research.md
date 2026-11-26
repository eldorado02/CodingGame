# 6-nimmt-6-qui-prend-take-5 — recherche

## Sources
- Opencode (2026-11-26) : `research-opencode-20261126-073456.md` (toutes les URL).
- https://github.com/denisakkavim/sixnimmt (docs/strategy-families.md,
  uncertainty-bots.md, experiments/mcmc/REPORT.md) : bots à mondes échantillonnés.
- https://github.com/hub2/nimmt6 (gym + RL), https://github.com/TimothyJones/6nimmt-bot.
- Aucun papier dédié (arXiv 0 résultat) ; aucun post-mortem CodinGame lu.

## Faits du referee (sonde stdin brut)
- La phase est la DERNIÈRE ligne du tour (l'énoncé la met en premier).
- Scores = vaches en négatif ; -999 = disqualifié. `RANDOM` est accepté.

## Classe de problème et approche
Jeu simultané à information cachée, 4 joueurs. v1 : coût espéré binomial.
Piste v2 (non testée) : 32 à 64 mondes cohérents (mains adverses tirées parmi
les cartes inconnues), résolution exacte du tour, coût moyen + risque.
