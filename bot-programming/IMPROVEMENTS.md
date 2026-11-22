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
