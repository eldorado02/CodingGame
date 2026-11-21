# Journal des améliorations — append-only

Chaque amélioration est écrite ici le jour où elle est appliquée. Voir
[`PLAYBOOK.md`](PLAYBOOK.md) §7 pour le format. Ce fichier est la mémoire
entre sessions et le point de reprise quand les sous-agents sont à court de
quota gratuit.

### 2026-11-21 — Back Track King — parsing entrelacé des champs du tour
- Constat / défaite : `ValueError: invalid literal for int() with base 10: 'x'`.
- Hypothèse : les champs `owner instability inked active` sont lus groupés alors qu'ils sont entrelacés case par case.
- Sources : log de test game réel, énoncé `summer-challenge-2026-back-track-king`.
- Action : lecture `regionId type` à l'init, puis `owner instability inked active` par case.
- Résultat : 210/210 fuzz 4 langages, 8/10 vs boss Bronze `6767605`, soumission `41773849`.
- Leçon réutilisable : toujours lire les registres dans l'ordre exact du protocole, case par case.

### 2026-11-21 — Olymbits — horizon de recherche 5
- Constat / défaite : horizon 4 → 1/5 ; horizon 6 → timeout `scores: 0=-1`.
- Hypothèse : l'action étant partagée entre les 4 machines, énumérer les séquences communes sur un horizon moyen.
- Sources : test games `5507122,5507574`, dépôts `UAKobzar/Olymbits`, `JakubDotPy/codingame_olymbits`.
- Action : énumération `4^5`, simulation des courses, premier pas de la meilleure séquence.
- Résultat : 210/210 fuzz, 10/10 vs boss Wood 1, soumission `41773894`.
- Leçon réutilisable : mesurer le pire cas en itérations ; diviser par 3 pour la marge Python.

### 2026-11-21 — Winter 2024 — blocage structural des sources harvestées
- Constat / défaite : 16–17, `gained 3A from absorption` au lieu du `gained 1A from harvest` répété du boss.
- Hypothèse : le `GROW BASIC` vers une source lointaine pose le premier organe SUR la source déjà harvesée.
- Sources : metadata `growthTypes_harvester_0=1`, `harvestedProteins_A_1=2`, log de test game `5736695`.
- Action : exclure les sources harvestées **du BFS lui-même**, pas seulement de la liste des objectifs.
- Résultat : 210/210 fuzz, 9/9 vs boss Wood 3, soumission `41776467` ; ligue Wood 2 atteinte.
- Leçon réutilisable : une ressource exploitée doit être retirée du graphe, pas seulement déclarée non-cible.

### 2026-11-21 — Olymbits Bronze — registres d'archerie signés
- Constat / défaite : `IndexError: string index out of range` sur ~30 % des parties, score `-1`.
- Hypothèse : `reg[player]` de l'archerie est une coordonnée `x`/`-y` signée, pas une position de piste.
- Sources : `CGjupoulton/SummerChallenge2024Olymbits/src/main/java/.../Archery.java`, log de test game `5507123`.
- Action : ramener toute position négative à 0 avant la simulation, dans les 4 langages ; générateur de fuzz étendu aux valeurs négatives.
- Résultat : 210/210 fuzz, 13/13 vs boss Bronze `5507123`, soumission `41776913` ; rank 104/577 en Bronze.
- Leçon réutilisable : valider le signe et les bornes de chaque registre avant de l'utiliser comme index.

### 2026-11-21 — Procédure — replay global et reprise automatique
- Constat / défaite : les sous-agents gratuits atteignent `Free usage limit reached` et le travail s'arrête.
- Hypothèse : un état persistant permet de reprendre exactement là où le quota s'est épuisé.
- Sources : `bot-programming/MODELS-RANKING.md`, `MODELS.md`, `games.json` (100 jeux).
- Action : création de `PROGRESS.md` (file chronologique des 100 jeux), `PLAYBOOK.md` (consignes), `IMPROVEMENTS.md` (ce journal).
- Résultat : commits `c669b43`, `27cc8e3` poussés ; boucle relancée sur `tron-battle`.
- Leçon réutilisable : écrire l'état avant de continuer, jamais après.

### 2026-11-21 — Recherche — ajout d'ArXiv et des agrégateurs académiques
- Constat / défaite : la recherche s'arrêtait à GitHub et aux blogs ; aucune source théorique pour les stratégies de jeu.
- Hypothèse : les articles sur MCTS/minimax/partage de plateau et allocation de temps peuvent fournir des heuristiques transférables.
- Sources : arXiv API, Semantic Scholar, Papers with Code (ajoutés au playbook).
- Action : étendre la hiérarchie des sources du playbook (§1.1) avec arXiv, Semantic Scholar, Papers with Code, Gists, Codeforces, Kaggle, Rosetta Code.
- Résultat : playbook mis à jour ; à appliquer au deep search de `tron-battle`.
- Leçon réutilisable : toute amélioration du processus doit être journalisée ici avant d'être considérée comme faite.

### 2026-11-21 — Documentation — README IA par challenge
- Constat / défaite : 26 dossiers de challenge sans mode d'emploi ; une autre IA devait deviner l'architecture et les pièges.
- Hypothèse : un README normé par challenge permet une reprise sans perte d'information.
- Sources : `hackathon-2026-12-03/challenges/*/{statement,plan,critique}.md`, `bot-programming/PLAYBOOK.md` §8.
- Action : génération d'un `README.md` dans les 26 dossiers (contrat E/S, architecture, bilan, cartographie 4 langages, tests, pièges, pistes d'amélioration) + ajout de l'obligation « documenter avant de coder ».
- Résultat : commit `66df848` poussé, 27 fichiers, 3283 lignes.
- Leçon réutilisable : une amélioration non documentée dans le README du challenge est incomplète.

### 2026-11-21 — Objectif — Gold minimum pour tous les jeux
- Constat / défaite : sur 25 jeux déjà soumis, aucun n'atteint Gold (2 Silver, 7 Bronze, 16 Wood).
- Hypothèse : traiter d'abord les jeux les plus proches de Gold maximise le nombre de jeux au seuil.
- Sources : `GET /puzzles/<id>/progress` pour les 25 jeux soumis, snapshot `bot-programming/league_state.json`.
- Action : objectif réécrit dans `PROGRESS.md` (Gold minimum, Légende stretch), file réordonnée par ligue courante décroissante puis rang de ligue croissant ; les 75 jeux sans soumission suivent.
- Résultat : 100 jeux suivis, priorité #1 `a-code-of-ice-and-fire` (Silver 184/323), #2 Back Track King (Silver 484/720).
- Leçon réutilisable : mesurer l'état réel de toutes les ligues avant de fixer l'ordre de travail.

### 2026-11-21 — Troll Farm Bronze — prototype PLANT (rejeté)
- Constat / défaite : note 22,88 sous le boss 22,96 malgré le rang 1/401 ; le bot perd parfois contre le rank 2 (`6910022`).
- Hypothèse : le rank 2 plante 17 arbres ; un troll jardinier près de la cabane créerait un verger et augmenterait la note.
- Sources : logs de test games (`/tmp/opencode/troll_vs_rank2.txt`), referee `eulerscheZahl/Troll-Farm`.
- Action : prototype `/tmp/opencode/troll_plant.py` (troll le plus lent = PICK + PLANT sur cases voisines de la cabane).
- Résultat : 2/6 victoires contre le prototype (identique au bot de base, 2/6), scores très bruités → **rejeté**, bot validé conservé.
- Leçon réutilisable : ne pas remplacer un bot validé par un prototype sur un résultat bruité ; exiger ≥3/5 reproductibles.

### 2026-11-21 — Tron Battle — bot Voronoi validé Wood 2
- Constat : aucun bot Tron ; deep search terminé (sous-agent hy4-flash-free après quota épuisé sur les 2 premiers modèles).
- Hypothèse : Voronoi 1-ply + greedy primé suffit en 2 joueurs (le territoire au 1er cut gagne 96–99 % des duels).
- Sources : vks.ai (78 lignes), a1k0n post-mortem, nathanWolo/tron-codingame, arXiv:1110.3211, thread officiel #49.
- Action : bot Python + ports C/C++/Rust, harnais fuzz 210 cas, soumission `41777582`.
- Résultat : 210/210 fuzz identiques 4 langages, 5/5 victoires vs boss 630143 (100–90), rang 1/1621 score 21,14 > boss 18,91. Promotion en attente du cycle.
- Leçon : le fuzz différentiel a trouvé 4 bugs réels (NEI dupliqué, BFS sauté sur tête bloquée, best_score=-1, joueur mort non consommé) invisibles sur l'exemple officiel. Toujours générer des parties LÉGALES pour le fuzz.

### 2026-11-21 — Directive campagne (utilisateur)
- Constat : Gold comme objectif minimal laissait 100 jeux à traiter sans fin claire.
- Hypothèse : boucle tournante + objectif Légende maximise les promotions (on ne s'épuise pas sur un jeu bloqué).
- Sources : directive utilisateur directe.
- Action : PROGRESS.md et PLAYBOOK.md réécrits (Légende pour tous, rotation après promotion, 4 langages codés mais **soumission C++**, documentation renforcée).
- Résultat : en cours.
- Leçon : ne pas confondre « bot validé en Python » et « bot soumis » — la référence de soumission est désormais C++.

### 2026-11-21 — Troll Farm Bronze — entraînement multiple + priorité harvestPower
- Constat : bot rang 1 mais score 22,88 < boss 22,96 ; une seule formation au tour 1 plafonne la récolte.
- Hypothèse : former jusqu'à 3 trolls et privilégier harvestPower/carryCapacity augmente le débit de fruits.
- Sources : logs de test games vs boss 6479370, rank2 6910022, rank3 6480078 ; énoncé Bronze (coûts de TRAIN).
- Action : score de formation `3*hv + 2*c + s`, plafond 3 trolls, port C/C++/Rust, fuzz 210/210.
- Résultat : 4/4 victoires vs rank 2, ~4/9 vs boss. Soumission C++ en attente du quota test games.
- Leçon : tester une variable à la fois puis combiner ; le prototype PLANT et le CHOP ont été rejetés malgré leur intuition initiale.
