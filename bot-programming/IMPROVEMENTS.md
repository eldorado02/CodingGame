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

### 2026-11-21 — Wondev Woman — correction du protocole (bot planté)
- Constat : score −1,55, rang 193 en Wood 1 ; le bot plantait au tour 1 sur `int('4 2')` car l'énoncé archivé dit 1 unité alors que le stub en annonce 2, et il n'envoyait qu'une ligne pour l'unité 0.
- Hypothèse : le plant venait du parsing et du nombre de lignes de sortie, pas de la stratégie.
- Sources : `cg.py stub wondev-woman`, log de test game `.../wondev-woman/logs/test-20261121-171943.log`, referee officiel `CodinGame/WondevWoman/Referee.java` (`outputs[0]` seulement, `FINAL_HEIGHT - 1 = 3`, `ACCEPT-DEFEAT` valide).
- Action : parser 2 unités par camp + legalActions indexées ; une seule action globale en sortie ; grille de score v2 (victoire 1e6, déni 5e4, tremplin 3e3, adjacent-case3 1.5e3) ; port C/C++/Rust ; fuzz 210 cas.
- Résultat : fuzz 210/210 identique ; 4 victoires + 1 nulle sur 5 vs boss 1393115 ; soumission C++ 41778172.
- Leçon : pour un puzzle ancien, le stub généré et le referee priment toujours sur l'énoncé archivé. Vérifier le nombre de lignes de sortie attendues avant d'optimiser la stratégie.

### 2026-11-21 — CodeBusters Wood 2 — micro-optimisations rejetées
- Constat : bot rang 1 mais score 18,53 < boss 19,59 ; le boss gagne la plupart des duels.
- Hypothèses : recul optimal 1330, dépôt anticipé 1550, affectation globale, double BUST coopératif.
- Sources : énoncé Wood 2, stub officiel, referee `eulerscheZahl/RefereeCollection`, bots publics GitHub (version complète seulement).
- Action : 5 prototypes Python testés sur 4–5 parties chacun contre le boss 630129 ; aucun porté dans `solutions/`.
- Résultat : 0/4 à 1/5 victoires selon les variantes, pas mieux que le bot validé (1/4 sur la même fenêtre). Tous rejetés.
- Leçon : vérifier l'endurance des fantômes dans la ligue courante avant de transposer une stratégie de la version complète ; ici `state=0`, donc le levier est le débit de captures, pas le recul de 50 unités.

### 2026-11-21 — Troll Farm Bronze — revert du prototype multi-trolls
- Constat : soumission 41778069 (3 trolls + harvestPower) converge vers score ligue ~20,5 et rang ~100-170, contre 22,88 et rang 1 pour le bot initial.
- Hypothèse : le gain observé en test games contre le rank 2 ne se généralise pas aux 400 joueurs de la salle ; trop de ressources partent dans l'entraînement.
- Sources : `GET /puzzles/spring-challenge-2026-troll-farm/progress` (pct 15→24, score 18,3→20,9→20,53), logs de test games.
- Action : restauration des 4 langages depuis `54bffd1`, fuzz 210/210, soumission C++ 41778446.
- Résultat : en cours de classement.
- Leçon : le score de ligue (Elo sur toute la salle) est la seule métrique décisionnelle ; 3/5 victoires vs boss ne garantissent pas une promotion.

### 2026-11-21 — Spring Challenge 2022 — prédiction longue rejetée
- Constat : bot rang 1 mais score 19,42 < boss 20,8.
- Hypothèse : extrapoler 3 tours la trajectoire des monstres améliorerait les interceptions.
- Sources : stub officiel (`vx`,`vy`,`nearBase`,`threatFor`), test games vs boss 4258167.
- Action : prototype `sc22_v2` testé sur 5 parties ; harnais porté à 210 cas ; resoumission C++ du bot validé (41778517).
- Résultat : prototype 0/5 victoires contre 5/5 pour le bot validé → rejeté.
- Leçon : les monstres changent de cap près de la base ; une extrapolation longue est contre-productive, seule la prédiction à 1 tour est fiable.

### 2026-11-21 — Olymbits Silver — décision jointe 4 mini-jeux (soumission 41778730)
- Constat : après montée en Silver, le bot Bronze traite tous les registres comme des pistes de haies ; score ligue 5,8 (rang 1227) contre boss 18,93.
- Hypothèse : classifier les 4 machines et additionner des scores par action permet de jouer l'action commune optimale.
- Sources : énoncé officiel Silver (`GPU`, `reg0`..`reg6`), referee Java `CGjupoulton/SummerChallenge2024Olymbits`, bot `UAKobzar/Olymbits/Player.cs`.
- Action : prototype `oly_v2` (pondérations 3/1/2/2), portage 4 langages, générateur de fuzz étendu aux 4 types ; soumission C++ 41778730.
- Résultat : 3/5 victoires contre boss+rank1 (ancien bot 0/5), fuzz 210/210.
- Leçon : l'égalité bit-à-bit entre langages a révélé un 0 fantôme dans la liste des adversaires roller ; le fuzz doit couvrir chaque format de registres, pas seulement la piste de haies.

### 2026-11-21 — A Code of Ice and Fire Silver — prototype économie/tours rejeté
- Constat : bot Wood 3 score -1 contre le boss Silver 2425714 ; le rush L1 est sans effet face aux L3 et aux coupures de territoire.
- Hypothèse : mines précoces + tours de défense + unités L2/L3 permettraient de survivre.
- Sources : starter kit officiel `Azkellas/a-code-of-ice-and-fire`, referee `Referee.java` / `Cell.java`, notes `odoucet/cig-fire/strategies.md`, bot Gold `DollarAkshay/CodinGame`.
- Action : prototype `/tmp/opencode/ice_silver.py` (mines, tours, upkeep, BFS), 6 test games.
- Résultat : 6 défaites, QG détruit aux tours 13–16 ; prototype non retenu, bot initial conservé.
- Leçon : le boss Silver détruit l'économie par coupure de connectivité avant que la production de tours ne rentre en jeu ; il faut d'abord résoudre la défense de connectivité, pas seulement le nombre d'unités.

### 2026-11-21 — Ghost in the Cell Wood 4 — projection temporelle (soumission 41778890)
- Constat : bot initial et prototypes multi-ordres simples plafonnent à 1 victoire sur 7-9 contre le boss 1079501.
- Hypothèse : projeter l'état de chaque usine sur 6 tours (production, troupes, bombes) et garder une réserve de défense permet d'enchaîner des ordres rentables.
- Sources : post-mortem Agade, README Legend devYaoYH, bot Lif3line, énoncé/stub officiels.
- Action : prototype `gic_strong.py`, portage 4 langages, fuzz 206/206, soumission C++ 41778890.
- Résultat : 4/5 victoires en vérification indépendante (boss à 0), 5/5 lors de la recherche initiale.
- Leçon : une projection explicite de l'état futur et la mise à jour après chaque ordre comptent plus que le nombre d'attaques ; la variante « défense serrée » a régressé à 2/6.

### 2026-11-21 — A Code of Ice and Fire Silver — deuxième prototype rejeté
- Constat : le boss Silver détruit le QG aux tours 13-16 ; le rush L1 et l'économie+tours échouent tous deux.
- Hypothèse : forteresse de tours + coupe de connectivité adverse + L3 tardif pourraient tenir.
- Sources : referee/engine officiels `Referee.java`, `GameState.java`, `Cell.java`, `Pathfinding.java`, `Constants.java` ; `/tmp/opencode/ice_def_research.md`.
- Action : prototype `ice_def.py`, 12 parties contre le boss 2425714.
- Résultat : 0/12 victoires, score adverse 194–297, aucun crash de notre côté.
- Leçon : il faut un mur de tours/L3 opérationnel vers le tour 10, ce que l'économie actuelle ne permet pas de financer ; autre angle à trouver avant de réessayer.

### 2026-11-21 — Olymbits/Ghost — timeout C++ par sortie non flushée
- Constat : Olymbits 41778730 converge vers -8,32 (rang 1592/1598) alors que le même bot gagne 3/5 en test games ; Ghost 41778890 démarre à 0.
- Hypothèse : divergence de décision entre Python et C++ sur des entrées réelles.
- Sources : logs de test game (`summary: $0 has not provided 1 lines in time`, frame 3), stderr du binaire de debug montrant tous les DBG mais aucune stdout.
- Action : ajout de `<< std::flush` après chaque ligne produite dans les deux `solutions/cpp/bot.cpp` ; fuzz 210/210 et 206/206 rejoués ; resoumissions C++ 41778911 (Olymbits) et 41778912 (Ghost).
- Résultat : test game C++ Olymbits repassé de -1 à 7280 (victoire).
- Leçon : avec `sync_with_stdio(false)`, `'\n'` ne vide pas le tampon ; CodinGame tue le processus avant la fin du tour. Toujours flusher explicitement la sortie dans les bots C++.

### 2026-11-21 — Cultist Wars — timeout C++ identique corrigé
- Constat : soumission C++ précédente notée -1 en test game (score ligue 12,27 mais rang 49/266).
- Hypothèse : même absence de `flush` que Olymbits/Ghost.
- Sources : `solutions/cpp/bot.cpp`, test game `cg.py test cultist-wars`.
- Action : `<< flush` sur toutes les sorties, fuzz porté à 210/210, soumission C++ 41778956.
- Résultat : test game passé de -1 à 3–7 points (2 victoires / 5 contre le boss).
- Leçon : auditer systématiquement `sync_with_stdio(false)` sans `flush` dans toutes les soumissions C++.

### 2026-11-21 — Xmas Rush Bronze — repli MOVE (v6)
- Constat : `xmas_v5` joue souvent `PASS` sur les tours MOVE alors que des quêtes
  sont accessibles ; 2/5 victoires environ contre le boss 2209776, alors que
  le port C++ perdait encore plus souvent (0/10) malgré un fuzz 210/210 vert.
- Hypothèse : `multi_bfs` ne renvoie un chemin que si un état est strictement
  meilleur que l'état initial ; sans repli, le bot gaspille le tour. La
  divergence Python/C++ observée venait de deux erreurs de port : `bfs_dist`
  décalé de 1 et positions de quêtes dupliquées Indexées sur le dernier indice
  en Python seulement.
- Sources : logs de test games (`stdout: PASS` répétés), `Referee.java`,
  prototypes `/tmp/opencode/xmas_v5.py` et `xmas_v6.py`, `tests/fuzz/`.
- Action : ajout de `target_path` (BFS vers la quête la plus proche) en repli
  dans les 4 langages, correction des deux erreurs de port, fuzz 210/210.
- Résultat : `xmas_v6` Python 8 victoires / 10 test games contre le boss.
- Leçon : un BFS « strictement améliorant » doit toujours avoir un repli
  exploitant ; sans cela le bot perd des tours entiers. Vérifier l'égalité des
  ports sur des entrées réelles, pas seulement sur le fuzz généré.

### 2026-11-21 — Xmas Rush — divergence de parsing C++ (root cause)
- Constat : avec la même `game_options` (`seed=-7503483396589402000`), le bot
  Python gagne 12–9 alors que le bot C++ perd 4–12 ; premier PUSH différent
  dès la frame 1.
- Hypothèse : divergence de parsing des quêtes, pas de stratégie.
- Sources : API docs (`game_options` / `replay_game_id`), copies debug des bots
  écrivant l'état parsé et les scores sur `stderr`, logs de test games.
- Action : debug sur rejeu même seed → C++ `targets=[]` contre Python
  `targets=[(5,3),(2,4),(6,3)]`. Correction : ignorer les items dont
  `itemPlayerId != 0` au lieu de `break` sur le premier nom correspondant.
- Résultat : rejeu identique 12–9 dans les deux langages ; fuzz 210/210 ;
  C++ 3/5 victoires ; soumission 41779754.
- Leçon : le fuzz doit refléter la structure réelle des entrées (ici noms d'items
  identiques chez les deux joueurs). Pour comparer deux ports, utiliser un
  rejeu `game_options`/`replay_game_id` et des traces `stderr`, pas seulement
  des cas aléatoires.

### 2026-11-21 — Ghost in the Cell — promotion Bronze
- Constat : score Wood 1 supérieur au boss, promotion vers Bronze effective ;
  nouveau score 19,29 contre boss 26,12 (classement en cours).
- Hypothèse : la nouvelle règle Bronze `INC` (10 cyborgs pour +1 production,
  max 3) n'est pas exploitée par le bot Wood 1.
- Sources : énoncé relu via `cg.py show ghost-in-the-cell`, `statement.md`.
- Action : documenter la règle, préparer une décision INC conditionnelle.
- Résultat : en attente.
- Leçon : relire l'énoncé après chaque promotion ; la première nouveauté de
  ligue est souvent la levier le plus direct.

### 2026-11-21 — Ghost in the Cell Bronze — rule INC : research + 7 prototypes rejected
- Constat : promotion Bronze ; le bot Wood 1 n'émet jamais INC et score ~20 contre
  un boss à 26.06.
- Hypothèse : investir 10 unités pour +1 production (rentable en ~10 tours)
  devrait combler l'écart ; la littérature recommande un seul INC/tour sur une
  usine arrière sûre.
- Sources : referee officiel, postmortem Agade, bots Kodle/Tbigman/RoboStac/martin,
  fil de stratégie CodinGame (voir `research.md`).
- Action : 7 variantes Python testées en 5–17 test games contre `1094665` ;
  rejeu même seed avec traces `stderr` pour analyser les défaites.
- Résultat : meilleure variante v3 à 6/17, pas de gain significatif vs le bot
  Wood 1 (~37 %) ; v4a/v5/v6/v7 franchement moins bons. Aucune soumission.
- Leçon : l'INC n'est pas le facteur limitant ici ; le boss gagne par
  concentration de force et notre bot disperse ses MOVE. Tester des idées sur
  un échantillon ≥10 avant de porter en 4 langages ; les lots de 5 sont trop
  bruités. Ne pas confondre quota épuisé et erreur réseau dans les logs.

### 2026-11-22 — Ataxx — orientation du plateau, pas mapping joueur
- Constat : 0/5 contre le boss Wood 2 `6649970` malgré un fuzz 210/210 et un
  mapping `me = 2 si id == 0` déjà corrigé.
- Hypothèse : le mapping était la cause ; un rejeu avec traces `stderr` montre
  au contraire que le bot joue depuis des pièces qui ne sont pas les siennes
  dans sa propre représentation.
- Sources : log `bot-programming/games/ataxx/logs/test-20261122-125039.log` et
  rejeu seed `8726614551035298000` (`test-20261122-125743.log`).
- Action : restaurer `me = id + 1` et convertir les rangs avec `y = '7' - rang`
  (les 7 lignes sont reçues du haut vers le bas) dans Python/C/C++/Rust.
- Résultat : fuzz 210/210 dans les 4 langages ; toujours 0/5 contre `6649970`.
  Prototype 1,5 pli (beam 8, pire retour adverse) : 0/5 également. Aucune
  soumission.
- Leçon : vérifier l'orientation des lignes avec un rejeu tracé avant de
  modifier le mapping joueur ; un fuzz auto-cohérent ne détecte pas une
  convention de coordonnées erronée partagée par les 4 langages.

### 2026-11-22 — Xmas Rush Silver — énoncé 12 quêtes et simulation des poussées
- Constat : promotion Bronze → Silver effective (score 12,16, rang 570,
  `eligible_for_promotion=False`) ; nouveau boss `Santa Claus` `2215206`
  score 20,76 ; bot Bronze 1/5.
- Hypothèse : le bot Bronze gère déjà les 12 quêtes, mais sa simulation des
  poussées transpose `grid[y][x]` en `grid[x][y]` et évalue tous les objets,
  y compris les quêtes non révélées.
- Sources : énoncé Silver via `cg.py show xmas-rush` (remplace `statement.md`),
  dépôt public `elokaac/cg-xmas-rush` (`GreedyEvaluator`, `BruteForcePusher`).
- Action : prototype Python v7 corrigeant la transposition et lisant quêtes
  révélées des deux joueurs ; v8 n'évalue plus que les quêtes révélées ; v9
  remplace le tie-break Manhattan par une distance BFS exacte.
- Résultat : v7 1/5 (scores 7/11/2/1/9), v8 1/5 (10/12/7/11/3). v9 en attente
  du quota test games (30/900s atteint). Aucune soumission.
- Leçon : la transposition est invisible dans un fuzz qui ne compare que nos
  propres langages ; la comparer au rejeu referee. Les scores proches (11–12)
  indiquent un problème de cadence de collecte, pas de crash.

### 2026-11-22 — Xmas Rush Silver — distance Manhattan même hors connexité
- Constat : v7–v13 plafonnent à 1/5 malgré des scores proches (11–12). Le rejeu
  `test-20261122-140801.log` montre 131 `PASS` sur 150 tours MOVE : le bot
  reste déconnecté de ses quêtes révélées et n'avance plus.
- Hypothèse : pénaliser l'inatteignable par `50/(d+1)` est trop faible pour
  inciter une poussée à rapprocher l'objet ; il faut un gradient fort
  `500 - 30*d` même quand la cible est hors composante connexe.
- Sources : referee officiel `CodinGameCommunity/XmasRush` (`Referee.java`,
  `checkForFinishedItems`, `doPushAction`, `updatePlayerPos`), énoncé Silver.
- Action : prototype Python v17 = v13 + gradient Manhattan fort + correction
  du collect instantané (`instant` seulement si on atterrit sur la tuile
  insérée). Deux lots de 5 test games contre `2215206`.
- Résultat : **3/5 puis 3/5** en Python (scores 5–12, 9–8, 11–12, 7–4, 12–10
  puis 12–11, 12–6, 11–6, 8–12, 6–12). Port 4 langages terminé ; fuzz
  **210/210** identique. C++ validé à **1/5 + 1 nulle, puis 4/5, puis 4/5 + 1
  nulle** (9 victoires / 15). Soumission C++ `41785676`.
- Leçon : dans un labyrinthe recomposé à chaque poussée, un gradient continu
  vers la cible bat une heuristique binaire « atteignable / non ». Vérifier
  le nombre de `PASS` dans les logs : c'est le meilleur diagnostics de cadence.

### 2026-11-22 — Ataxx — moteur bitboard, murs et profondeur
- Constat : le rapport de recherche attribue le 0/5 à une évaluation inversée
  (centre au lieu des coins) et à un horizon d'un coup.
- Hypothèse : un negamax bitboard avec PST coins/arêtes et profondeur 4
  devrait battre le boss `6649970`.
- Sources : `jdermont/AtaxxCodingame` (referee), `kz04px/tiktaxx`,
  `kz04px/autaxx`, `pressibus.org/ataxx/gen/gbstrategies.html`.
- Action : prototypes C++ `ataxx_bb` (divers depths), simulateur local
  `referee_sim.py` + boss 2 plis, ajout des 9 murs réels.
- Résultat : 0/5 → 1/5 en depth 3 contre le boss réel ; depth 4/5/6 régressent
  dans le simulateur local. Aucune soumission. Le moteur du rapport de
  recherche génère des coups invalides dès qu'il y a des murs.
- Leçon : valider le moteur de recherche sur la vraie config (murs) avant de
  conclure ; un eval PST trop centré ou un `TURN` trop fort font régresser
  les profondeurs supérieures. Ne pas confondre un binaire compilé et son
  source lors des test games.

### 2026-11-22 — Ataxx — correction du flip racine, le simulateur local ment
- Constat : `ataxx_v2_d3.cpp` est 1/5 contre le boss réel `6649970` mais
  `draw 2 / loss 3 / win 3` dans le duel local avec murs.
- Hypothèse : la racine appliquait le déplacement sans convertir les voisins
  retournés (`nme` perdait les pièces au lieu de les gagner). Le corriger
  devrait améliorer nettement le jeu réel.
- Sources : referee `jdermont/AtaxxCodingame`, local `referee_sim.py` +
  `ataxx_local_duel.py`, PST `tiktaxx`.
- Action : `ataxx_v3_rootflip.cpp` = depth 3 + `nme |= ADJ[dst] & opp` à la
  racine. Duel local : **12 victoires / 12**. Test games réels : **0/5**.
- Résultat : rejeté, aucune soumission. Le boss local (heuristique 2 plis) est
  trop faible et ne prédit pas le comportement du boss CodinGame.
- Leçon : un simulateur local ne valide qu'une mécanique, pas une force de jeu.
  Ne jamais remplacer les test games réels par un duel local, même avec les
  murs et le même parser. Conserver le correctif de flip pour la suite, mais
  chercher ailleurs l'écart de force (évaluation, profondeur, gestion du tempo).

### 2026-11-22 — Shobu Wood 4 — swap systématique + bords de board
- Constat : v1 (1 ply + réponse adverse, sans positionnel) fait 1/5 contre le
  boss greedy `6906517` ; les défaites arrivent en 10–24 tours.
- Hypothèse : le boss ne swap jamais et ne regarde pas la réponse. Prendre le
  swap systématiquement annule son premier coup ; pénaliser nos pierres sur
  le bord d'un board 4×4 (et récompenser les siennes) cible la mécanique de
  sortie de plateau.
- Sources : énoncé officiel, referee `Waffle3z/CG-Shobu` (`Board.java`,
  `aggressiveOk`, `sameBoard`, `legalMoves`).
- Action : v5 = always SWAP + positionnel 12 (2 victoires, 3 nulles), puis
  v6 = positionnel **25** (4/5). Port 4 langages, fuzz 200/200.
- Résultat : validation C++ **2/5 puis 3/5** (5/10 cumulés). Soumission C++
  `41785855`.
- Leçon : l'ordre de génération du referee est sémantique. Les ports triaient
  les réponses adverses avant d'en garder 24, ce qui changeait le coup joué
  sur 5 cas de fuzz sur 24. Îlots de comportement -> toujours comparer les 4
  langages sur des positions réelles, pas seulement sur des cas trivials.

### 2026-11-22 — Xmas Rush Silver — v18/v19/v20 rejetés, v22 bloqué
- Constat : `v17` (soumis `41785676`) plafonne à `score=16.13`, `percentage=14`,
  boss `2215206` à `20.76`.
- Hypothèses testées : `v18` gradient BFS étendu, `v19` distance Manhattan
  accessible, `v20` pénalité -100 sur les cases inatteignables.
- Action : 5 test games par variante après fuzz 210/210.
- Résultat : **0/5 pour v18, v19 et v20**. `v22` n'a pas pu être testé :
  HTTP `429` sur le quota `30 test games / 900 s`.
- Leçon : les heuristiques de distance sur la grille de Xmas ne captent pas
  la contrainte réelle de déplacement (poussée + orientation du tile). Ne pas
  ajouter de terme de distance sans modèle de poussée validé par le referee.

### 2026-11-22 — Brandubh Wood — attaquants incapables d'arrêter le roi
- Constat : tous les prototypes (Python 1/2 plis, garde du anneau, C++ 4 plis)
  font 0/5 contre `jacek` (`6913632`). Le roi s'échappe en 10 à 30 tours.
- Hypothèses testées : distance BFS, mobilité du roi, poids matériel 300,
  bonus sur les 8 cases de garde initiales, fenêtre alpha-beta complète.
- Sources : énoncé officiel, referee `n3l-ctrl/brandubh-codingame`
  (`Referee.java` : `isHostileSquare`, `checkEncirclement`, `checkWinCondition`).
- Action : correction de trois bugs de modèle (boucle de lecture, orientation
  des lignes, test roi/défenseur absent du BFS d'encerclement), puis balayage
  d'évaluations et port C++ avec coupe à 42 ms.
- Résultat : échecs répétés ; aucune soumission. Deux timeouts Python corrigés
  en réduisant la profondeur / en restaurant l'élagage alpha-beta.
- Leçon : l'évaluation « distance du roi au coin » ne suffit pas : les attaquants
  se font capturer un par un avant de pouvoir former un mur. Il faut un modèle
  de contrôle des cases de fuite (couloirs vers les 4 coins) et non une simple
  distance. Ne pasступider les test games tant que ce modèle n'existe pas.

### 2026-11-22 — Elemental Wars Wood 3 — fuite prioritaire, 5/5
- Constat : le premier prototype lisait tout `stdin` d'un coup et timeout au
  frame 1 (0/5). Après correction de la lecture, 3/5 avec une fuite modérée.
- Hypothèse : en Wood 3 (3 élémentaux), la partie se joue sur la distance au
  prédateur ; privilégier la survie puis la poursuite de la proie.
- Sources : énoncé officiel, API `opponents` (`6910638`, `6697588`), logs de
  test games. Aucun dépôt public trouvé.
- Action : `score = -100*d(proie) + 20*min(d(prédateur),6)` + bonus/malus
  `+10000/-10000/-2000` ; ports C/C++/Rust ; fuzz 200/200.
- Résultat : Python **5/5**, C++ **5/5**, soumission C++ `41786190`.
- Leçon : deux bugs de port (`pred_of('P')`, prédateur prisonnier) n'étaient
  visibles que dans le fuzz : toujours exécuter le différentiel 4 langages avant
  de soumettre, même quand le prototype Python est 5/5.

### 2026-11-22 — Chain Reaction — moteur alpha-beta validé mais dominé
- Constat : le moteur C++ (alpha-beta, 6 plis, simulation conforme au referee
  Java public `darkhorse64/CG_ChainReaction`) gagne 5/5 contre le joueur de rang
  73 mais perd 0/5 contre `jacek` (rang 1) et les rangs 2 et 3.
- Hypothèses testées : poids matériel 100/300/1000, potentiel orbs/critical,
  heuristique Brilliant (orbs, voisins critiques, coins, orbs non-orphelins),
  pénalité défensive renforcée, tri des coups par évaluation immédiate.
- Sources : referee et `Board.java` publics (simulation validée 299/300 puis
  300/300 après correction du flag « deux joueurs ont joué »), dépôts
  `notshridhar/chain-reaction-ai` et `BALLISTICrobin/chain-reaction-ai-bot-`.
- Action : prototype C++ `/tmp/opencode/cr_v1..v6.cpp` ; aucune soumission.
- Résultat : 0/5 contre le repère `jacek` pour chaque variante.
- Leçon : la simulation est correcte mais l'évaluation ne suffit pas face à un
  bot de tête ; ne pas soumettre sans benchmark favorable. Passer à un jeu avec
  un boss officiel et une recherche plus décisive (Connect 4).

### 2026-11-22 — Connect 4 — alpha-beta profondeur 6 retenu
- Constat : le boss officiel `6797564` est batable ; un premier moteur
  itératif a timeout au tour 0 à cause d'une vérification de délai tous les
  1024 nœuds seulement.
- Hypothèses testées : profondeur fixe 6/7, bonus centre (colonne vs case du
  haut), ordre des coups centre → bords, table de transpositions.
- Sources : énoncé officiel `cg.py show connect-4`, dépôt public
  `AshKcg/cg-multi-connect4` (`Boss.java`, `Referee.java`, `Connect4Board.java`),
  Opponents API. arXiv et Semantic Scholar n'ont rien apporté pour le plateau
  7×9.
- Action : 4 ports (`solutions/{python,c,cpp,rust}`), fuzz différentiel
  **200/200**, test games C++ contre `6797564`.
- Résultat : `c4_fixed7` **0/5** ; `c4_v2` **4/5** ; `c4_center` **4/5** puis
  **5/5** ; la version exacte du dépôt **5/5**. Soumission C++ `41786612`.
- Leçon : la profondeur fixe 6 est plus fiable que la recherche itérative sous
  100 ms ; rejouer la version du dépôt avant soumission car les séries de 5
  parties sont bruitées.

### 2026-11-22 — Shobu Wood 3 → Wood 2 — budget d'évaluations
- Constat : la version Wood 4 (1 ply + 24 réponses) perd 0/5 contre le boss
  MCTS `6906535`. Le budget temps (82 ms) gagne 5/5 mais casse le fuzz
  identique ; le budget de nœuds (200) timeout à ~180 ms (50 000 évaluations).
- Hypothèses testées : budget d'évaluations 18 000 / 20 000 / 22 000 / 26 000,
  profondeur fixe 3/4, variantes d'évaluation (matériel renforcé, plateau faible
  renforcé, coups de push directs), swap ou non.
- Sources : énoncé relu après promotion, referee public `Waffle3z/CG-Shobu`
  (`Board.java`, `Referee.java`), logs de test games.
- Action : 4 ports maintenus, fuzz **200/200**, test games C++ contre `6906535`
  puis contre le boss Wood 2 `6906553`.
- Résultat : budget 20 000 → **4V+1N/5 puis 4V+1N/5** contre Wood 3, sans
  timeout. Soumission C++ `41786861`, promotion immédiate en Wood 2. Contre
  Wood 2 : 1V+1N+3D ; sans swap 0/5 ; variantes d'évaluation 0–1V/5.
- Leçon : le budget d'évaluations est la bonne unité déterministe. Le boss
  Wood 2 (minimax 3 plis) reste à battre ; privilégier une recherche plus
  profonde à budget constant plutôt que des poids d'évaluation ajustés à la main.

### 2026-11-22 — Othello Wood 2 — alpha-bêta avec élagage à la racine
- Constat : le boss `6797560` est un joueur aléatoire (seed 13, `MultiStruct/Othello`).
  Un moteur alpha-bêta simple (profondeur fixe 4/5/6 ou budget de nœuds
  1200–2500) timeout ou plafonne à 1V/5 : le coût de `evaluate` (mobilité +
  frontier) dépend trop de la position.
- Hypothèse : le budget temps (115 ms, vérifié toutes les 256 entrées) combiné
  à un **élagage alpha-bêta à la racine** permet d'atteindre 5–7 plis sans timeout.
- Sources : énoncé officiel, referee et `Boss.java` publics (`MultiStruct/Othello`),
  logs de test games, opponents API.
- Action : 4 langages (C/C++/Python/Rust), fuzz 200/200 avec un budget de nœuds
  déterministe pour valider la parité des ports ; test games C++ de la version
  temps réel contre `6797560`.
- Résultat : version temps réel **9V/1D/10** ; soumission C++ `41787330`,
  score 31.85 > boss 26.42 en cours de classement (pas encore promu).
- Leçon : l'élagage à la racine est indispensable (division du temps par ~3).
  Le budget temps casse l'identicité du fuzz entre langages ; un moteur à
  bitboards avec profondeur fixe est la piste pour réconcilier les deux.

### 2026-11-22 — Elemental Wars Wood 2 — correction du débordement et commande groupée
- Constat : la version Wood 3 soumise (`41786190`) timeout en Wood 2
  (`0=-1` sur 3 test games sur 5). Cause : tableaux de taille 8 alors que
  `elementalsCount` vaut 9 ou 12, et un seul élémental commandé.
- Hypothèse : en Wood 2 il faut commander **tous** les élémentaux alliés, avec
  une carte BFS par ennemi et un score mêlant poursuite et survie.
- Sources : énoncé récupéré via `cg.py show elemental-wars` (constraints
  `elementalsCount = 6, 9, 12`, `50 ms`/tour), opponents API
  (`6697631`, `6699103`), logs de test games.
- Action : v3 (BFS par ennemi, `-100*min(dp,20)+15*min(dq,8)`, bonus/malus
  `+10000/-12000/-2000`, départage x puis y), générateur de fuzz étendu à
  6/9/12 élémentaux et 11×11/13×13, 4 ports, fuzz 200/200.
- Résultat : C++ **5/5** contre `6697631,6699103` (`0=2..4 1=0 2=0`) ;
  soumission C++ `41787544`.
- Leçon : un fuzz trop étroit (3 élémentaux) laisse passer un débordement
  de tableau qui ne se manifeste qu'à la ligue supérieure. Toujours régénérer
  le fuzz avec les constraints de la nouvelle ligue après promotion.

### 2026-11-22 — Othello — moteur bitboard déterministe (suite)
- Constat : la version temps réel `41787330` gagne mais casse l'identicité du
  fuzz 4 langages (16 divergences) ; les budgets de nœuds du moteur tableau
  timeout car `evaluate` coûte ~6500 opérations.
- Hypothèse : représenter le plateau par deux `uint64` pour réduire le coût
  d'évaluation d'environ 20× et permettre une profondeur fixe.
- Sources : referee public `MultiStruct/Othello`, 10 000 positions aléatoires
  validées contre l'implémentation tableau, logs de test games.
- Action : moteur bitboard (dilatation 6 étages, `flips`, évaluation POS +
  mobilité + frontier, tri stable POS, alpha-bêta profondeur 6), 4 ports,
  fuzz 200/200, validation 10 000/10 000 `moves`/`flips`.
- Résultat : max 34,9 ms par coup sur le fuzz, **5/5** contre `6797560` ;
  soumission C++ `41787653`.
- Leçon : la dilatation doit partir des pions propres (`sh(P)&O`), pas
  `sh(O)&P`. Une évaluation rapide rend la profondeur fixe compétitive et
  réconcilie déterminisme et fuzz multi-langages.

### 2026-11-22 — Elemental Wars — Wood 1, prédiction et libération
- Constat : promotion en Wood 1. Nouvelle règle (libération des prisonniers
  via le camp du prédateur), cartes 11/13/15 jusqu'à 15 élémentaux, et fin
  immédiate si une tribu est entièrement prisonnière. La v3 empilait tous ses
  élémentaux sur une même case : un seul prédateur capturait la pile entière.
- Hypothèse : prédire le déplacement adverse (un pas sur un plus court chemin,
  priorité N,E,S,W) permet d'intercepter les proies et d'éviter le prédateur ;
  répartir les alliés sur des cases distinctes évite la défaite instantanée.
- Sources : énoncé officiel relu après promotion, logs de test games, recherche
  GitHub/web (aucun bot public exploitable trouvé).
- Action : 8 variantes C++ locales (`v4`…`v11`), 5 test games chacune contre
  `6710922,6788011` (deux meilleurs Wood 1). Rétention de `v6` : prédiction
  déterministe, interception (+12000), évitement (-18000), anti-regroupement
  (-5000), libérateur. Ports 4 langages et fuzz étendu à 15×15 / 15 élémentaux.
- Résultat : `v6` **2/5** victoires contre les deux meilleurs et meilleur rang
  moyen ; `v11` (probabilités + garde de camp) marque plus mais finit 0/5.
  Test décisif : **5/5** contre notre ancienne soumission `6918338` et le rang
  40 `6758748`. Fuzz 200/200 (11/13/15, 6/9/12/15). Soumission `41788154`.
- Leçon : dans un jeu à captures simultanées, empiler ses unités est une perte
  immédiate ; la prédiction du mouvement adverse est plus utile que le
  renforcement des pénalités de fuite. Le quota de 30 test games / 900 s
  limite fortement l'exploration : privilégier des variantes distinctes.

### 2026-11-22 — Othello — Wood 1, profondeur 7 rejetée
- Constat : profondeur 6 perd 0/5 contre le rank 1 Wood 1 `6886490` (petites
  défaites), et Wood 1 n'affiche pas de boss.
- Hypothèse : gagner un ply avec un moteur optimisé suffit à basculer les
  parties serrées.
- Sources : logs de test games, profil local sur 200 cas de fuzz.
- Action : table de sommes positionnelles sur 16 bits + réutilisation des coups
  dans `evaluate`, puis tests C++ contre `6886490`.
- Résultat : 108 ms → 85 ms local, mais 0/5 dont un timeout sur CG. Version
  profondeur 6 conservée (`41787653`).
- Leçon : garder ≥2× de marge sous la limite de temps officielle ; une
  optimisation de 20 % ne rend pas un budget serré sûr.

### 2026-11-23 — Infra (session Claude Code)
- Constat : la campagne reprend sous Claude Code ; 3/100 jeux sont dans leur ligue maximale ; aucun outil ne savait mesurer « ligue max existante ».
- Hypothèse : beaucoup de jeux communautaires ont une ligue unique `Wood` ou un plafond Wood 1 ; une soumission notée suffit à les « finir ».
- Sources : `GET /puzzles/<id>/leaderboard?limit=1` (ligue du rang 1), `/progress`.
- Action : `tools/status.py` (statut des 100 jeux → `status.json`), `tools/bench.py` (N test games → victoires), `tools/research.sh` (recherche via opencode), `ALGO.md`, template `board-2p` 4 langages, skills `.claude/skills/cg-*`.
- Résultat : 23 jeux à ligue unique sans soumission, ~30 jeux communautaires plafonnés à Wood 1, 31 concours à amener en Legend.
- Leçon : la « Légende » n'existe pas pour beaucoup de jeux communautaires : cible = ligue du rang 1.

### 2026-11-23 — CodinGame compile le C++ sans -O2
- Constat : Breakthrough v1 déclenche le garde-temps à chaque tour ; 600k nœuds = 494 ms sur CG contre 114 ms en local -O2.
- Hypothèse : pas d'optimisation côté serveur.
- Sources : logs `breakthrough/logs/diag-*.log` ; build local sans flag : 601 ms (identique au serveur).
- Action : `#pragma GCC optimize("O3,unroll-loops")` avant les includes (pas de `target` : casse g++ 13), tri par insertion maison.
- Résultat : 33/34 bots C++ du dépôt n'ont pas le pragma → tous tournent ~2–4× plus lentement que prévu.
- Leçon : ajouter le pragma à chaque bot et re-soumettre les jeux bloqués près du boss (CodeBusters, Troll Farm, Olymbits, Green Circle, Shobu).

### 2026-11-23 — Breakthrough — v3 soumise
- Constat : v1/v2 perdent contre le rang 97 sur des percées à 5 plis.
- Hypothèse : règle exacte des coureurs + bitboards.
- Sources : énoncé, logs de test games.
- Action : v3 4 langages, fuzz 200/200 (flux auto-générés), bench.
- Résultat : 4/4 contre le rang 97, 0/4 contre le rang 33. Soumission `41795905`.
- Leçon : voir `challenges/breakthrough/critique.md`.

### 2026-11-23 — Brandubh — v9 réécrite et soumise
- Constat : aucune soumission ; v8 perd 0/3 contre le rang 15/27.
- Hypothèse : modèle de règles exact (referee) + anti-répétition + template board-2p.
- Sources : `n3l-ctrl/brandubh-codingame` Referee.java ; logs `games/brandubh/logs/v9*`.
- Action : v9 4 langages, fuzz 200/200, 3 variantes d'éval des coins testées.
- Résultat : 0/3 contre le rang 15 (attaquant). Soumission `41795979` (nouvelle règle utilisateur : 1re soumission sans seuil 3/5).
- Leçon : garde des coins depuis une case adjacente = pièce capturable contre le coin (pattern tafl, ajouté à ALGO.md).

### 2026-11-23 — Checkers — v1 soumise
- Constat : jeu jamais joué (ligue unique).
- Hypothèse : template board-2p + générateur de rafles suffit pour le milieu de tableau.
- Sources : énoncé, test game sonde.
- Action : 4 langages, fuzz 200/200, bench.
- Résultat : 3/3 contre les rangs 177 et 89 ; soumission `41796035`.
- Leçon : racine = liste fournie par le referee, générateur maison seulement en profondeur.

### 2026-11-23 — Chain Reaction — v1 réécrite et soumise
- Constat : aucune soumission, code de la session OpenCode perdu.
- Hypothèse : αβ + heuristique classique (vulnérabilité, coins/bords, cases critiques).
- Sources : énoncé ; logs `games/chain-reaction-1/logs/v1-*`.
- Action : 4 langages, fuzz 200/200 ; budget abaissé 150k → 50k après TIMEGUARD sur CG.
- Résultat : 1/3 rang 37, 0/2 rang 19 ; soumission `41796092`.
- Leçon : toujours lire `d= n= ms=` dans les logs serveur avant de fixer le budget.

### 2026-11-23 — Mad Knights — v1 soumise
- Constat : jeu jamais joué (3 joueurs, ligue unique).
- Hypothèse : recherche paranoïde αβ + mobilité.
- Sources : énoncé, referee darkhorse64/CG_MadKnights (cité).
- Action : 4 langages, fuzz 200/200, bench.
- Résultat : 3/3 contre rangs 69 + 86 ; soumission `41796137`.
- Leçon : jeux à 3 joueurs → paranoïde ; vérifier `ms=` serveur : 60k nœuds = 8 ms ici.

### 2026-11-23 — Impasse — v1 gloutonne soumise
- Constat : jeu jamais joué, règles lourdes (transposition, bear-off, couronnement, impasse)
- Hypothèse : un glouton 1 coup sur la liste légale fournie (progression vers bear-off/couronnement) bat le milieu de tableau
- Sources : énoncé, test game sonde (orientation), referee acatai/CG_Impasse cité
- Action : 4 langages, fuzz 200/200 (plateaux/chaînes aléatoires)
- Résultat : 3/3 contre le rang 71 (12–4) ; soumission `41796185`
- Leçon : pour un jeu à règles lourdes avec liste légale fournie, un glouton de progression donne une 1re soumission en ~15 min

### 2026-11-23 — Minishogi — v1 gloutonne soumise
- Constat : jeu jamais joué ; règles shogi (parachutages, promotions)
- Hypothèse : glouton capture/promotion + carte d'attaque adverse bat le milieu de tableau
- Sources : énoncé, referee jdermont/MiniShogi cité, test game sonde (orientation des coordonnées)
- Action : 4 langages, fuzz 200/200 (plateaux/actions aléatoires)
- Résultat : 3/3 contre le rang 61 ; soumission `41796267`
- Leçon : toujours sonder l'orientation des coordonnées (ici colonne 1 = droite) avant d'écrire l'éval

### 2026-11-23 — Dice Shogi — v1 gloutonne soumise
- Constat : jeu jamais joué ; minishogi + dé imposant la colonne
- Hypothèse : le glouton de minishogi suffit pour une 1re soumission
- Sources : énoncé, referee jdermont/DiceShogi cité
- Action : copie de minishogi avec ligne de dé, fuzz 200/200
- Résultat : 1/3 contre le rang 40 ; soumission `41796284` (1re soumission autorisée sous 3/5)
- Leçon : le hasard du dé demande une anticipation des réponses adverses (expectimax), le glouton ne suffit pas

### 2026-11-23 — Isola — v1 soumise
- Constat : jeu jamais joué ; ligues Wood 4 → Wood 1
- Hypothèse : αβ sur (déplacement, retrait) avec retraits limités aux libertés adverses + éval Voronoi bat le boss Wood 4
- Sources : énoncé, source wala-fr/CodingameIsola citée, ALGO.md classe P
- Action : 4 langages, fuzz 200/200 auto-joué ; budget abaissé 30k → 10k nœuds après TIMEGUARD sur CG
- Résultat : 6/6 contre le boss Wood 4 `4534449` ; soumission `41796400`
- Leçon : une éval à BFS par feuille coûte ~6× une éval matérielle : recalibrer le budget sur les logs serveur

### 2026-11-23 — Ataxx — v2 αβ bitboards soumise
- Constat : v1 gloutonne 1 pli : 0/5 contre le boss Wood 2, jamais soumise
- Hypothèse : αβ + différence de pièces à prof. 4–6 bat le boss
- Sources : énoncé, logs v1 (orientation : 1re ligne = rangée 7), dépôts GitHub jdermont/AtaxxCodingame (referee) et bots αβ publics (Tomatofishh/ataxx, pascalc/Ataxx-Reloaded-Agent)
- Action : réécriture 4 langages bitboards, fuzz 200/200 ; budget 15k puis 120k nœuds (CG 26–38 ms)
- Résultat : 2/4 puis 3/4 contre le boss `6649970` ; soumission `41796475`
- Leçon : la profondeur compte plus que l'éval dans Ataxx : budget ×8 = +1 victoire / 4

### 2026-11-23 — Clobber — v1 αβ mobilité soumise
- Constat : jeu jamais joué ; pas de liste de coups fournie
- Hypothèse : αβ + éval de mobilité (classique Clobber) bat le boss Wood 2
- Sources : énoncé, referee darkhorse64/CG_Clobber, GitHub TidyMaze/clobber (MCTS + minimax mobilité), dépôts αβ académiques (gh_search.sh)
- Action : 4 langages ; comptage de mobilité par décalages/popcount (×2,6) ; budget 60k nœuds
- Résultat : 3/4 contre le boss `3835619` mais grâce à ses timeouts ; soumission `41796537`
- Leçon : un boss qui dépasse le temps gonfle les résultats : compter les parties complètes séparément

### 2026-11-23 — Oware Abapa — v1 αβ soumise
- Constat : jeu jamais joué ; ligues Wood 2 → Wood 1
- Hypothèse : αβ prof. ~10 sur règles exactes du referee bat le boss
- Sources : referee MaximeCheramy/cg-oware (lu), marchete/CGZero, joansalasoler/oware (Aalina) via gh_search.sh
- Action : 4 langages, fuzz 200/200 auto-joué, scores reconstitués depuis les graines disparues
- Résultat : 4/4 contre le boss `2469367` (25–7) ; soumission `41796595`
- Leçon : lire le referee donne des règles exactes (grand chelem, obligation de nourrir) qu'un énoncé tronqué omet

### 2026-11-23 — Counting TicTacToe — v1→v2 αβ sélectif soumise
- Constat : v1 (éval complète des 288 fenêtres, prof. 2) : 0/3 contre le boss Wood 3 (≈200–230)
- Hypothèse : éval incrémentale + tri par gain immédiat + faisceau de 10 coups aux nœuds internes donne prof. 4
- Sources : referee RezaSi/counting-tic-tac-toe (règle de comptage), aucun bot public trouvé (gh_search.sh)
- Action : éval incrémentale (×10), tri stable + faisceau 10, budget 60k (TIMEGUARD) puis 2500 nœuds ; 4 langages, fuzz 200/200
- Résultat : 2/3 puis 3/4 contre le boss `3856980` ; soumission `41796704`
- Leçon : un nœud qui trie ses fils coûte ~100 évaluations : budgéter en nœuds triés, pas en feuilles

### 2026-11-23 — Onitama — v1 αβ soumise
- Constat : jeu jamais joué ; cartes orientées selon le prochain utilisateur
- Hypothèse : αβ prof. 5–6 avec cartes normalisées dans un repère unique bat le boss
- Sources : énoncé, sonde (orientation), referee eulerscheZahl/onitama, chareca/onitama-ai (αβ public)
- Action : 4 langages, fuzz 200/200 (cartes aléatoires)
- Résultat : 3/3 contre le boss `6797555` ; soumission `41796749`
- Leçon : normaliser toute donnée orientée (cartes, directions) dans le repère du joueur 0 évite les erreurs de signe

### 2026-11-23 — Amazons — v1 territoire 2 étages soumise
- Constat : jeu jamais joué ; ~2000 actions par tour, liste non fournie
- Hypothèse : classer les déplacements puis tester les flèches des 10 meilleurs avec une éval territoire suffit contre le boss
- Sources : énoncé, referee MultiStruct/Amazons, ALGO.md (territoire = Voronoi en distance-reine)
- Action : 4 langages ; 1re version à budget d'actions (préfixe biaisé, 61 ms) remplacée par la recherche en 2 étages ; fuzz 200/200
- Résultat : 3/3 (2–0) contre le boss `6797567` ; soumission `41796775`
- Leçon : quand le branchement explose, un pré-classement grossier + faisceau bat un budget qui ne voit qu'un préfixe de la liste

### 2026-11-23 — Lines Of Action — v1 αβ soumise (nuls)
- Constat : jeu jamais joué ; boss Wood 2 de type αβ
- Hypothèse : concentration + nombre de groupes suffisent pour connecter avant le boss
- Sources : énoncé, referee darkhorse64/CG_LinesOfAction, littérature Winands (MIA, évaluateur LOA)
- Action : 4 langages, fuzz 200/200 ; tests anti-répétition et poids des groupes
- Résultat : v1 : 3 nuls (boucle) ; anti-répétition 0/3 ; poids 25 0/3 ; v1 soumise `41796817`
- Leçon : une éval faible + anti-répétition transforme les nuls en défaites : l'anti-répétition ne paie qu'avec une éval qui progresse

### 2026-11-23 — Oware Abapa — v1.1 correctif joueur 1
- Constat : après soumission v1 : score 2,63, rang 45/54 alors que 4/4 contre le boss en test
- Hypothèse : en test nous sommes toujours joueur 0 ; le chemin joueur 1 est faux
- Sources : referee MaximeCheramy/cg-oware Player.getAction : num += 6 pour le joueur 1
- Action : sortie = index relatif 0..5 dans les 4 langages ; fuzz 200/200
- Résultat : re-soumission `41796829` (comportement joueur 0 inchangé, 4/4 conservé)
- Leçon : les test games ne couvrent que le siège 0 : lire le parsing de sortie du referee pour l'autre siège, et surveiller le score juste après soumission

### 2026-11-23 — Dots and Boxes — v1 αβ soumise
- Constat : jeu jamais joué ; état complet fourni à chaque tour
- Hypothèse : αβ avec tour supplémentaire à la fermeture d'une boîte résout le 2×2 de Wood 2
- Sources : énoncé, loicpetit/codingame-dots-and-boxes (gh_search.sh)
- Action : 4 langages, fuzz 200/200 (tailles 2–5)
- Résultat : 4/4 (4–0) contre le boss `6797561` ; soumission `41796861`
- Leçon : un tour supplémentaire se modélise sans changer le signe dans le negamax

### 2026-11-23 — Penguins — v1→v2 gloutonnes, v2 soumise
- Constat : jeu jamais joué ; grille hexagonale décalée, 2–4 joueurs
- Hypothèse : un glouton poissons + mobilité suffit contre le boss Wood 3
- Sources : énoncé, 2 sondes (géométrie déduite des listes de coups légaux), redblobgames hexagons (cité par l'énoncé)
- Action : v1 destination, v1b + poissons mangés à la source, v2 + mobilité ; ids de pingouins par joueur corrigés ; fuzz 200/200
- Résultat : 0/3, 1/4, 0/4 contre le boss `6797562` ; v2 soumise `41796912` (1re soumission)
- Leçon : le boss Wood 3 cherche : un glouton perd de ~1 poisson ; il faut une recherche + éval de territoire


## Entrées du sous-agent (fusionnées le 2026-11-23)
# IMPROVEMENTS — journal de l'agent 2 (à fusionner dans IMPROVEMENTS.md)

### 2026-11-23 — Hex — v1 MCTS + RAVE soumise
- Constat : jeu jamais joué (ligue unique Wood, 125 joueurs) ; l'énoncé ne précise pas la diagonale voisine.
- Hypothèse : MCTS + RAVE à remplissage aléatoire (sans terme d'exploration) bat le milieu de tableau.
- Sources : énoncé, test games sondes (`probe3` : chemin gagnant adverse `(5,4)→(6,3)` → voisinage HexWiki), HexWiki.
- Action : 4 langages (tableaux statiques, RNG xorshift64 + multiplication-décalage), fuzz 200/200 ; budget 2600 itérations après mesure CG (3000 = 44 ms).
- Résultat : 3/3 contre le rang 61 ; soumission `41796234` (rang 57 provisoire à 14 %).
- Leçon : vérifier la géométrie par une partie réelle ; en MCTS, expansion après k visites, sinon la création de nœuds domine.

### 2026-11-23 — Yavalath — v1 αβ + coups forcés soumise
- Constat : jeu jamais joué (ligue unique Wood, 256 joueurs).
- Hypothèse : αβ + extension des blocages forcés + éval fenêtres de 4 suffit pour le milieu de tableau.
- Sources : referee MultiStruct/Yavalath (lu), sonde protocole (vol autorisé seulement au 1er tour du joueur 2).
- Action : 4 langages, fuzz 200/200, 14000 nœuds/tour.
- Résultat : 3/3 rang 133, 3/3 rang 61 ; soumission `41796345`.
- Leçon : lire le referee quand le dépôt est cité dans l'énoncé (règle « ligne la plus longue » ; vol à `turn == 1`).

### 2026-11-23 — TwixT-PP — v1 distance de cavalier + 2 plis soumise
- Constat : jeu jamais joué (ligue unique Wood, 128 joueurs), referee introuvable.
- Hypothèse : éval distance en pegs (BFS 0-1, segments coupés par les liens adverses) + 2 plis.
- Sources : énoncé, bot public TidyMaze/Twixt-PP (protocole), test games.
- Action : 4 langages, fuzz 200/200, 30 ms au pire sur CG.
- Résultat : 6/6 contre rangs 65/49 (bots stub invalides), 0/5 contre rangs 25/9 ; soumission `41796478`.
- Leçon : éval de connexion plate → égalités départagées par l'index → agglutinement au bord ; ajouter un départage central.

### 2026-11-23 — Paper Soccer — v1b αβ tours complets soumise
- Constat : jeu jamais joué (ligue unique Wood, 241 joueurs).
- Hypothèse : αβ sur tours complets (DFS des rebonds) + éval rangs/distance BFS (Lecorbio, rang 2) bat le milieu de tableau.
- Sources : referee jdermont/CodinGame-paper-soccer (lu), Lecorbio/paper-soccer-strategy-engine `docs/algorithms.md`, rapport OpenCode (37 URL).
- Action : 4 langages, fuzz 200/200 (flux multi-tours, réponses par la référence Python) ; garde-temps 150 → 80 ms vérifié à chaque nœud après 153 ms observés.
- Résultat : 9/9 contre rangs 97, 49, 33 ; soumission `41796586`.
- Leçon : coups composés = coût par nœud très variable → garde-temps à chaque nœud.

### 2026-11-23 — Domain Expansion — v1 αβ + Voronoi soumise
- Constat : jeu jamais joué (ligue unique Wood, 92 joueurs), aucun bot public.
- Hypothèse : αβ sur (déplacement, mur) + Voronoi + fin exacte bat le milieu de tableau.
- Sources : referee Mortis66666/CG-Domain-Expansion (Kotlin, lu ; chemins de 4 pas autorisés).
- Action : 4 langages, fuzz 200/200, 10000 nœuds (28 ms CG).
- Résultat : 5/6 (2/3 rang 43, 3/3 rang 25) ; soumission `41796695`.
- Leçon : lire le BFS du referee, l'énoncé simplifie la règle de déplacement.

### 2026-11-23 — Bandas — v1 αβ profondeur 7–8 soumise
- Constat : jeu jamais joué (ligue unique Wood, 649 joueurs).
- Hypothèse : branching 4 → negamax profond + éval matériel/distance de chute.
- Sources : referee Oli8/CG-bandas (port exact des poussées et du rognage).
- Action : 4 langages, fuzz 200/200, 10000 nœuds (29 ms CG).
- Résultat : 6/6 contre rangs 302 et 130, 0/2 contre rang 44 ; soumission `41796751`.
- Leçon : pour un petit branching, la vitesse du moteur (bitboards) décide du rang au-dessus du milieu de tableau.

### 2026-11-23 — Tower Dereference — v1c glouton couverture + colle soumise
- Constat : jeu jamais joué (ligue unique Wood, 425 joueurs) ; v1 canons seuls perd contre le rang 197.
- Hypothèse : la colle multiplie l'exposition ; les dégâts au-delà des PV sont perdus ; les conflits de construction se perdent sur le demi-terrain adverse.
- Sources : referee eulerscheZahl/TowerDefense (constantes, ordre du tour, règle de conflit), logs diagnostics argent/vies.
- Action : glouton gain/coût entier (canon, colle, améliorations), poids colle ×4, plafond 15 dégâts, décote 10 % côté adverse ; 4 langages, fuzz 200/200.
- Résultat : v1b 2/2 rang 197 ; v1c 2/3 rang 113 ; soumission `41796833`.
- Leçon : premier test en Python (langage CG Python3) = validation du protocole avant de porter dans 3 langages.

### 2026-11-23 — nine men's morris — v1 αβ soumise
- Constat : jeu jamais joué ; pierres en main absentes de l'entrée
- Hypothèse : αβ + matériel/moulins/mobilité bat le boss Wood 2
- Sources : énoncé, referee bindstone/nine-mens-morris (cité)
- Action : moulins déduits de l'adjacence, compteur de poses, 4 langages, fuzz 200/200
- Résultat : 3/3 (5–0) contre le boss `3863794` ; soumission `41796948`
- Leçon : déduire la géométrie (moulins) des données d'init évite de coder un plateau en dur

### 2026-11-26 — Code Keeper - The Hero — v1->v3 règles + BFS, portages C/C++/Rust, soumission
- Constat : C/C++/Rust encore le template TTT, gen_fuzz template ; v1 0/2 (attend 2 tours sur 3, pas de sortie) ; v2 0/3 (oscille entre 2 cases)
- Hypothèse : les monstres s'arrêtent en diagonale (pas d'attente utile) ; un monstre hors vue ne bouge pas donc on peut le mémoriser
- Sources : referee acatai/CodeKeeper-TheHero MazeManager.turnMonsters/monsterPathfinding, MazeGenerator
- Action : portages 4 langages, gen_fuzz instantanés, suppression de l'attente, mémoire des monstres
- Résultat : fuzz 200/200 ; v3 0/3 (1 sortie 10185, 2 morts en 9 tours encerclé) ; soumis (premier envoi)
- Leçon : lire le pathfinding des monstres avant d'écrire une règle d'attente ; prochain : fuir vers un couloir quand >=2 monstres attaquent

### 2026-11-26 — 6 nimmt! / 6 qui prend! / Take 5! — v1 glouton coût espéré binomial
- Constat : 1er bench : disqualifié tour 1 ('PICK 0' au lieu de PLAY)
- Hypothèse : ordre des lignes d'entrée différent de l'énoncé
- Sources : sonde stdin brut (select + RANDOM) ; repos hub2/nimmt6, TimothyJones/6nimmt-bot (gh_search)
- Action : phase lue en dernière ligne du tour (4 langages + gen_fuzz)
- Résultat : fuzz 200/200 ; 4/4 parties 2e place (bat 2 joueurs rang ~108, perd contre rang 2) ; soumis
- Leçon : sonder le stdin brut avant d'écrire le parsing ; piste v2 : Monte-Carlo sur les mains adverses

### 2026-11-26 — Clash of Bots — v1 règles locales par robot
- Constat : jeu non soumis ; vision 5x5 sans position absolue
- Hypothèse : les mouvements précèdent les attaques : l'embuscade sur la case d'approche punit l'adversaire
- Sources : referee stivens/Clash-of-Bots (Interpreter.kt, MoveGraph.kt)
- Action : règles : explosion >=2 tués, attaque adjacente, embuscade distance 2, approche, dérive
- Résultat : fuzz 200/200 ; 4/4 contre rang 276/550 ; soumis
- Leçon : lire MoveGraph avant toute règle de déplacement groupé

### 2026-11-26 — Start-up! — v1 réputation d'abord -> v2 marché rapide
- Constat : v1 0/3 : score 0, un rival atteint 80 % au tour 12 pendant notre phase tests
- Hypothèse : le marché libre va au premier vendeur ; copier le rythme du gagnant (vendeurs au tour 4)
- Sources : log de test : actions de Hevea 5147362
- Action : embauche max, +1 manager/tour, vendeurs dès 10 features, moitié des devs en maintenance
- Résultat : fuzz 200/200 ; v2 3/3 (80 % de part) contre 3 joueurs de milieu ; soumis
- Leçon : dans un jeu de course au marché, lire les actions du gagnant dans le log avant d'optimiser une formule

### 2026-11-26 — Back to the Code — v1 boucles rectangulaires
- Constat : jeu non soumis
- Hypothèse : rectangles sûrs au meilleur ratio gain/périmètre suffisent pour être classé
- Sources : énoncé (règles d'encerclement, déplacement case par case)
- Action : sommes préfixes 2D, rectangles <= 12x12 avec nous sur le bord, rejet si adversaire à < coût/2
- Résultat : fuzz 200/200 ; 1/4 contre rang 500/1000 (scores serrés 264-412) ; soumis (premier envoi)
- Leçon : piste v2 : BACK pour annuler une boucle coupée, rectangles appuyés sur nos cellules existantes

### 2026-11-26 — Platinum Rift - Episode 2 — v1 s'étaler + marcher sur le QG
- Constat : jeu non soumis ; bench affichait me=None
- Hypothèse : bench ne lisait que 12 lignes d'en-tête, les options de carte repoussent le bloc agents
- Sources : log de test (bloc options map=...)
- Action : bench.py lit le bloc agents: où qu'il soit ; bot règles BFS QG ennemi
- Résultat : fuzz 200/200 ; 5/7 contre rang 500/1000 ; soumis
- Leçon : outil : ne jamais supposer une position fixe dans l'en-tête du log

### 2026-11-26 — Legends of Code & Magic: Constructed — v1 deck valeur/mana + combat glouton
- Constat : jeu non soumis
- Hypothèse : un deck à bonne courbe + gardes puis tête bat le milieu
- Sources : referee1.5 ConstructPhase.java (CHOOSE = baseId)
- Action : deck 15x2, sac à dos des invocations, objets, attaques
- Résultat : fuzz 200/200 ; 4/4 contre rang 57/114 ; soumis
- Leçon : lire le parsing du referee pour l'identifiant attendu (baseId vs instanceId)

### 2026-11-26 — Yinsh — v1 glouton 1 pli sur les actions légales
- Constat : jeu non soumis ; format de grille non documenté
- Hypothèse : demander les coups légaux (yes) évite d'écrire le générateur ; simuler + évaluer suffit
- Sources : sonde stdin brut ; referee MultiStruct/Yinsh
- Action : parse colonnes a..k, simulation (déplacement, retournement, retrait), score fenêtres de 5
- Résultat : fuzz 200/200 ; 4/4 contre rang 71/142 ; soumis
- Leçon : quand le referee fournit les actions légales, un 1-pli simulé est la v1 la plus rapide

### 2026-11-26 — Dice Duel — v1 alpha-bêta matériel, génération DFS
- Constat : jeu non soumis ; budget 8000 nœuds = TIMEGUARD 38x et 45 ms
- Hypothèse : coût par nœud très variable (16 dés), garde horloge trop espacée
- Sources : referee eulerscheZahl/Dice-Duel ; logs d= n= ms=
- Action : 4000 nœuds, garde 30 ms à chaque nœud ; v1d cassé par un sed (commentaire inline) puis corrigé
- Résultat : fuzz 200/200 ; 3/4, 3/3, 3/3 contre boss Wood 2, max 31 ms ; soumis
- Leçon : horloge à chaque nœud si nœud lourd ; jamais de sed pour insérer un commentaire sur une ligne de code

### 2026-11-26 — Tulips and Daisies — v1->v3 glouton fenêtres + trous
- Constat : v1 timeout (60 ms, copies de grille) + or épuisé ; v2 rapide (2 ms) mais 17 vs 374
- Hypothèse : le boss comble des trous entre tronçons de 3 (33 or) ; nous récoltons des 4 (7 or)
- Sources : sonde stdin ; log des coups du boss ; referee WildSmilodon
- Action : v2 sans copie (case surchargée), réserve d'or ; v3 potentiel trou
- Résultat : fuzz 200/200 ; v3 1/3 (victoire = timeout du boss), 21-43 vs 281-374 ; soumis (premier envoi)
- Leçon : Fibonacci : la valeur est dans les grandes jonctions, pas les 4 ; prévoir une éval de plateau v4

### 2026-11-26 — Blocking — v1 plus grande pièce + centre
- Constat : jeu non soumis ; coups valides fournis par le referee
- Hypothèse : les grosses pièces d'abord suffisent contre le boss Wood 2
- Sources : énoncé
- Action : score = taille x 200 - distance doublée au centre
- Résultat : fuzz 200/200 ; 0/4 (1 nul) contre boss, écarts 0 à 5 points ; soumis (premier envoi)
- Leçon : v2 : géométrie des orientations pour compter les coins (heuristique Blokus classique)

### 2026-11-26 — Abalone — v1 glouton 1 pli sur coups légaux
- Constat : jeu non soumis ; repère hexagonal non documenté
- Hypothèse : directions déductibles des coups légaux de la sonde
- Sources : sonde stdin (coups légaux du 1er tour)
- Action : axial + directions E,NE,NW,W,SW,SE ; simulation Sumito ; centre + cohésion
- Résultat : fuzz 200/200 ; 4/4 contre boss Wood 2 (6-0) ; soumis
- Leçon : les coups légaux d'une sonde suffisent à identifier les vecteurs de direction

### 2026-11-26 — Fireworks — v1 Hanabi conventions sûres
- Constat : jeu non soumis ; format des lignes de connaissance non documenté
- Hypothèse : le boss joue une carte dont il connaît couleur et niveau ; ne jamais répéter un indice
- Sources : sondes stdin (emplacements remplacés sur place) ; résumés des logs
- Action : jouer sûr / indice sûr non répété / défausse ; bloc final = mains visibles
- Résultat : fuzz 200/200 ; py 2/3, C++ 2/3 (1er sur 4) ; soumis
- Leçon : v1 répétait le même indice de niveau (0/2) : suivre la connaissance des coéquipiers

### 2026-11-26 — Vindinium — v1 mines + bière -> v2 évitement et achèvement
- Constat : v1 1/3 : tué 4 fois par le même boss, mines perdues
- Hypothèse : éviter les ennemis plus sains et achever les faibles garde nos mines
- Sources : résumés des logs (kills, beers)
- Action : zone de danger bloquée dans le BFS, proie <= 20 PV à 2 pas, bière <= 50 PV
- Résultat : fuzz 200/200 ; v2 1/4 (461 en victoire, 4 à 91 sinon, 50 bières dans une partie) ; soumis (premier envoi)
- Leçon : compter morts / bières / mines dans les logs avant de régler ; v3 : limiter les bières

### 2026-11-26 — Dice Duel — v1.1 règle des 7 en Wood 1
- Constat : promu Wood 1 (8.74, rang 94) avec SUM_RULE=false : captures invalides = défaites
- Hypothèse : activer la règle du referee league level 2
- Sources : Board.java playMove (getLeagueLevel()==2)
- Action : SUM_RULE=true dans les 4 langages
- Résultat : fuzz 200/200 ; 5/5 contre le milieu de Wood 1 ; re-soumis
- Leçon : préparer les constantes de règle de la ligue suivante dès la v1, les basculer à la promotion

### 2026-11-26 — Chess — v1 1 pli sur coups légaux + pièces en prise
- Constat : jeu non soumis ; coups légaux fournis (input moves)
- Hypothèse : éviter les pièces en prise et prendre le matériel suffit contre le boss Wood 2
- Sources : énoncé (protocole fen/moves, roque Chess960)
- Action : simulation du coup sur la FEN, cartes d'attaque, perte maximale adverse
- Résultat : fuzz 200/200 ; 1 victoire 2 nuls contre boss ; soumis
- Leçon : les nuls viennent du manque de mat : v2 alpha-bêta Chess960

### 2026-11-26 — Poker — v1 paliers -> v2 équité Monte-Carlo
- Constat : v1 0/4 : 33 FOLD, 77 CHECK, éliminé par un boss qui suit et mise
- Hypothèse : décider à l'équité contre la cote du pot au lieu de paliers fixes
- Sources : sonde stdin (format des actions)
- Action : Monte-Carlo déterministe (xorshift32 partagé), évaluateur 7 cartes
- Résultat : fuzz 200/200 ; v2 2/4 contre boss ; soumis
- Leçon : aléatoire reproductible : même PRNG entier dans les 4 langages = fuzz différentiel possible
