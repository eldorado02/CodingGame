<!-- opencode model: hy4-flash-free, 2026-11-23T20:28:41+03:00 -->

> build · hy4-flash-free
Je vais effectuer une recherche approfondie sur 'ataxx' en respectant l'ordre des sources. Commençons par plusieurs recherches en parallèle.
✗ Web Search via OpenCode "CodinGame ataxx referee github rules constants" failed
Error: Web search request failed (HTTP 402)
✗ Web Search via OpenCode "github CodinGame ataxx bot alpha-beta solution" failed
Error: Web search request failed (HTTP 402)
✗ Web Search via OpenCode "eulerscheZahl RefereeCollection ataxx" failed
Error: Web search request failed (HTTP 402)
✗ Web Search via OpenCode "CodinGame ataxx post-mortem forum strategy" failed
Error: Web search request failed (HTTP 402)
> build · hy4-flash-free
La recherche web échoue (HTTP 402). Je vais utiliser directement l'API GitHub et les URLs connues via webfetch.
✗ WebFetch https://api.github.com/repos/CodinGame/Ataxx failed
Error: StatusCode: non 2xx status code (404 GET https://api.github.com/repos/CodinGame/Ataxx)
% WebFetch https://api.github.com/search/repositories?q=ataxx+codingame 
% WebFetch https://api.github.com/search/repositories?q=ataxx+in%3Aname 
> build · hy4-flash-free
Le jeu semble récent. Explorons le referee `jdermont/AtaxxCodingame` et la collection `eulerscheZahl/RefereeCollection`, ainsi que la page officielle CodinGame.
% WebFetch https://api.github.com/repos/eulerscheZahl/RefereeCollection/contents/ 
% WebFetch https://api.github.com/repos/jdermont/AtaxxCodingame/contents/ 
% WebFetch https://www.codingame.com/multiplayer/bot-programming/ataxx 
> build · hy4-flash-free
La collection `eulerscheZahl` n'a pas ataxx. Le referee principal est `jdermont/AtaxxCodingame`. Explorons son arborescence complète et les fichiers de configuration.
% WebFetch https://api.github.com/repos/jdermont/AtaxxCodingame/git/trees/main?recursive=1 
% WebFetch https://raw.githubusercontent.com/jdermont/AtaxxCodingame/main/README.md 
> build · hy4-flash-free
Excellent ! Ce dépôt contient le referee, la config, l'énoncé ET le code du Boss. Lisons les fichiers clés en parallèle.
% WebFetch https://raw.githubusercontent.com/jdermont/AtaxxCodingame/main/config/config.ini 
% WebFetch https://raw.githubusercontent.com/jdermont/AtaxxCodingame/main/config/stub.txt 
% WebFetch https://raw.githubusercontent.com/jdermont/AtaxxCodingame/main/config/level1/Boss.cpp 
% WebFetch https://raw.githubusercontent.com/jdermont/AtaxxCodingame/main/config/statement_en.html 
> build · hy4-flash-free
Le Boss fourni est un bot aléatoire. Lisons maintenant le cœur du referee : `Referee.java` et `Board.java`, plus les classes de données.
% WebFetch https://raw.githubusercontent.com/jdermont/AtaxxCodingame/main/src/main/java/com/codingame/game/Referee.java 
% WebFetch https://raw.githubusercontent.com/jdermont/AtaxxCodingame/main/src/main/java/com/codingame/game/ataxx/Board.java 
> build · hy4-flash-free
J'ai les faits clés du referee. Cherchons maintenant les bots publics, post-mortems et articles, en parallèle.
% WebFetch https://www.codingame.com/forum/search?q=ataxx 
% WebFetch https://raw.githubusercontent.com/jdermont/AtaxxCodingame/main/src/main/resources/view/config.js 
% WebFetch https://html.duckduckgo.com/html/?q=codingame+ataxx+post+mortem+bot 
✗ WebFetch https://api.github.com/search/code?q=ataxx+codingame failed
Error: StatusCode: non 2xx status code (401 GET https://api.github.com/search/code?q=ataxx+codingame)
> build · hy4-flash-free
Le moteur de recherche DuckDuckGo fonctionne. Creusons le forum CodinGame, les bots GitHub et les articles scientifiques.
% WebFetch https://www.codingame.com/multiplayer/bot-programming/ataxx/discuss 
% WebFetch https://html.duckduckgo.com/html/?q=%22ataxx%22+codingame+github+bot+alpha-beta 
% WebFetch https://html.duckduckgo.com/html/?q=site%3Acodingame.com%2Fforum+ataxx 
% WebFetch https://arxiv.org/search/?query=ataxx&searchtype=all 
✗ WebFetch https://api.semanticscholar.org/graph/v1/paper/search?query=Ataxx&limit=20&fields=title,year,abstract,url,externalIds failed
Error: StatusCode: non 2xx status code (429 GET https://api.semanticscholar.org/graph/v1/paper/search?query=Ataxx&limit=20&fields=title,year,abstract,url,externalIds)
> build · hy4-flash-free
✗ WebFetch https://api.semanticscholar.org/graph/v1/paper/search?query=Ataxx%20game%20artificial%20intelligence&limit=10 failed
Error: StatusCode: non 2xx status code (429 GET https://api.semanticscholar.org/graph/v1/paper/search?query=Ataxx%20game%20artificial%20intelligence&limit=10)
% WebFetch https://html.duckduckgo.com/html/?q=%22ataxx%22+%22codingame%22+site%3Agithub.com 
% WebFetch https://html.duckduckgo.com/html/?q=codingame+ataxx+discuss+algorithm+boss 
✗ WebFetch https://cs.curtisbright.com/reports/cs686project.pdf failed
Error: Unsupported fetched file content type: application/pdf
> build · hy4-flash-free
Trouvé un article MCTS sur Ataxx. Le forum CodinGame semble être Discourse — essayons son API JSON, et un proxy de lecture pour les PDF.
% WebFetch https://html.duckduckgo.com/html/?q=site%3Aforum.codingame.com+ataxx 
% WebFetch https://r.jina.ai/https://cs.curtisbright.com/reports/cs686project.pdf 
% WebFetch https://r.jina.ai/https://leoribeiro.github.io/papers/mcts-ataxx-eniac2018.pdf 
% WebFetch https://forum.codingame.com/search.json?q=ataxx 
> build · hy4-flash-free
Le forum n'a qu'un fil minimal. Essayons de rendre la page de discussion CodinGame via un proxy JS, et cherchons des bots avec les termes du protocole.
% WebFetch https://r.jina.ai/https://www.codingame.com/multiplayer/bot-programming/ataxx/discuss 
% WebFetch https://r.jina.ai/https://www.codingame.com/multiplayer/bot-programming/ataxx 
% WebFetch https://html.duckduckgo.com/html/?q=%22actionsCount%22+%22lastAction%22+ataxx+codingame 
% WebFetch https://html.duckduckgo.com/html/?q=%22ataxx%22+%22codingame%22+bot+solution+legend 
% WebFetch https://api.github.com/search/repositories?q=ataxx+bot&sort=stars&order=desc&per_page=30 
> build · hy4-flash-free
La page officielle confirme MCTS + Minimax comme concepts clés. Cherchons des bots publics avec des termes d'évaluation concrets.
% WebFetch https://api.github.com/repos/feifeiobama/AtaxxBot/contents/ 
% WebFetch https://api.github.com/repos/QtLab/Ataxx/contents/ 
% WebFetch https://api.github.com/repos/leoribeiro/mcts-ataxx/contents/ 
% WebFetch https://api.github.com/search/repositories?q=ataxx+codingame+in:readme&per_page=30 
> build · hy4-flash-free
Lisons le code de bots Ataxx publics pour extraire des termes d'évaluation concrets (en les distinguant bien des bots CodinGame).
% WebFetch https://raw.githubusercontent.com/feifeiobama/AtaxxBot/master/README.md 
% WebFetch https://raw.githubusercontent.com/feifeiobama/AtaxxBot/master/ataxx.cpp 
% WebFetch https://raw.githubusercontent.com/leoribeiro/mcts-ataxx/master/mcts.py 
% WebFetch https://raw.githubusercontent.com/feifeiobama/AtaxxBot/master/MCTS.cpp 
% WebFetch https://r.jina.ai/https://raw.githubusercontent.com/QtLab/Ataxx/master/AI%E7%AE%80%E8%A6%81%E8%AF%B4%E6%98%8E.pdf 
> build · hy4-flash-free
Vérifions le classement/la discussion live et recherchons d'éventuels bots récents liés à la sortie du jeu.
% WebFetch https://api.github.com/search/repositories?q=ataxx+created:%3E2026-08-01&sort=updated&order=desc&per_page=50 
% WebFetch https://r.jina.ai/https://www.codingame.com/multiplayer/bot-programming/ataxx/leaderboard 
% WebFetch https://html.duckduckgo.com/html/?q=%22ataxx%22+codingame+MCTS+%22100+ms%22+bot+strategy 
% WebFetch https://html.duckduckgo.com/html/?q=%22codiname%22+ataxx+bot+github+source+code 
> build · hy4-flash-free
% WebFetch https://api.github.com/repos/titaniummachine1/titanium-ataxx/contents/README.md 
% WebFetch https://html.duckduckgo.com/html/?q=%22titanium-ataxx%22+evaluation+search+engine 
% WebFetch https://forum.codingame.com/t/community-puzzle-ataxx-puzzle-discussion/208495.json 
✗ WebFetch https://api.semanticscholar.org/graph/v1/paper/search?query=Ataxx&limit=5&fields=title,year,url failed
Error: StatusCode: non 2xx status code (429 GET https://api.semanticscholar.org/graph/v1/paper/search?query=Ataxx&limit=5&fields=title,year,url)
> build · hy4-flash-free
% WebFetch https://raw.githubusercontent.com/titaniummachine1/titanium-ataxx/main/README.md 
> build · hy4-flash-free
Pour compléter, lisons le document de recherche communautaire référencé par le moteur Titanium (KataAtaxx/MCTS).
% WebFetch https://raw.githubusercontent.com/titaniummachine1/titanium-ataxx/main/docs/COMMUNITY_RESEARCH_2026.md 
> build · hy4-flash-free
% WebFetch https://raw.githubusercontent.com/jdermont/AtaxxCodingame/main/src/main/java/com/codingame/game/ataxx/Viewer.java 
> build · hy4-flash-free
# Deep search — CodinGame « ataxx »

> Périmètre respecté : aucune modification de fichier, aucun build, aucune lecture de `~/.config/codingame`. Sources lues via `webfetch`, API GitHub, `raw.githubusercontent.com`, forum Discourse CodinGame, arXiv et proxy de lecture PDF. La recherche web native étant en erreur 402, j'ai utilisé DuckDuckGo HTML comme moteur de découverte.

---

## 1) Faits du referee (constantes, ordre de résolution, départages)

**Source primaire** (referee de la contribution, publié par `jdermont`) : https://github.com/jdermont/AtaxxCodingame — README : https://raw.githubusercontent.com/jdermont/AtaxxCodingame/main/README.md
**Page officielle du jeu** : https://www.codingame.com/multiplayer/bot-programming/ataxx
**Énoncé embarqué** : https://raw.githubusercontent.com/jdermont/AtaxxCodingame/main/config/statement_en.html

> ⚠️ `https://api.github.com/repos/CodinGame/Ataxx` renvoie **404**, et `eulerscheZahl/RefereeCollection` ne contient **pas** ataxx (https://api.github.com/repos/eulerscheZahl/RefereeCollection/contents/). Le dépôt `jdermont/AtaxxCodingame` est donc la seule source referee disponible. [NON VÉRIFIÉ] qu'il soit byte-identique au referee déployé sur codingame.com.

### Constantes

| Élément | Valeur observée | Source exacte |
|---|---|---|
| Plateau | 7×7, 2 joueurs, joueur 0 commence | `Board.java`, `Referee.java` |
| Configurations de murs | **596** annoncées ; choix `parts.get((int)(Math.abs(seed) % parts.size()))`; murs par symétrie 4-fold `wallAndMirror(x,y)`, `x` = case centrale (3,3) | `statement_en.html`, `Board.java` |
| Placement initial | J0 : **a7** (`cells[6][0]`) et **g1** (`cells[0][6]`) ; J1 : **a1** et **g7** | `Board.java` constructeur |
| Limite de tours | `max_turns = 300`, `setMaxTurns(300)` | `Referee.java` |
| Temps | 1er tour **1000 ms**, chaque tour **100 ms** | `Referee.java`, `config.ini`/statement |
| Spawn | 8 directions (Chebyshev 1), **dédupliqué par case cible** (`HashSet spawns`) | `Board.getActions()` |
| Jump | 16 directions (Chebyshev 2), **non dédupliqué** ; cible vide et hors mur obligatoire | `Board.getActions()` |
| Conversion | Les 8 voisins de la case d'arrivée appartenant à l'adversaire changent de camp | `Board.applyAction()` |
| Fin de partie | `units.size() == 49 − wall_count` (plateau plein) **ou** un joueur à 0 pièce | `Board.isGameOver()` |
| Score final | Joueur ayant **le plus de pièces** → score 1, l'autre 0 ; sinon 0–0 | `Referee.gameTurn()` / `onEnd()` |
| Répétition | `hash(player)` = `bitboard_J0 ; bitboard_J1 ; index_joueur` ; **3e occurrence → nul** (`"Draw by repetition!"`) | `Referee.java`, `Board.hash()` |
| Limite atteinte | `turn == 300` → **nul** (`"Draw, turn limit reached!"`) | `Referee.java` |
| Faute | Action invalide ou timeout → `deactivate`, **score −1**, fin immédiate | `Referee.java` |

URLs exactes des fichiers lus :
- https://raw.githubusercontent.com/jdermont/AtaxxCodingame/main/src/main/java/com/codingame/game/Referee.java
- https://raw.githubusercontent.com/jdermont/AtaxxCodingame/main/src/main/java/com/codingame/game/ataxx/Board.java
- https://raw.githubusercontent.com/jdermont/AtaxxCodingame/main/config/config.ini
- https://raw.githubusercontent.com/jdermont/AtaxxCodingame/main/src/main/java/com/codingame/game/ataxx/Viewer.java

### Ordre de résolution (par tour, `Referee.gameTurn`)

1. `sendInputs(turn)` : envoie l'`id` (tours ≤ 2), **7 lignes**, `lastMoves` de l'adversaire, `actionsCount`, puis les actions légales.
2. **Détail critique de parsing** : le referee envoie `cells[7-y-1][x]` pour `y=0..6`, donc **la ligne 0 correspond à la case `a7` (row 7), la ligne 6 à `a1` (row 1)**. Formule : `row = 7 − ligneIndex`, `col = a + x`. L'énoncé dit « from bottom to top », ce qui est cohérent avec le rendu du Viewer (la ligne 7 est en bas de l'écran) mais **contredisant la convention usuelle a1 en bas à gauche**. À vérifier empiriquement dans l'IDE. [NON VÉRIFIÉ] pour une éventuelle correction côté plateforme.
3. `execute()` ; premier token mis en minuscules ; un message après l'action est autorisé (tronqué à 16 caractères).
4. Token `"random"` → choix uniforme dans `actions` via le RNG du joueur ; sinon **égalité exacte** de `action.toString()` avec la liste ; sinon `InvalidAction`.
5. `applyAction` (création/déplacement + conversions).
6. Recalcul des pièces, test `isGameOver()` → attribution du score, fin.
7. Sinon calcul des coups de l'adversaire → test de répétition → test des 300 tours → soit passage à l'adversaire, soit **si l'adversaire n'a aucun coup, le joueur courant rejoue** et `lastAction = "pass"` est envoyé à l'adversaire (`;` accumulateur).

**Bug favorable** : `actionsNew` est créé et mélangé (`Collections.shuffle(actionsNew, ...)`) mais la boucle itère sur `actions` non mélangé → **l'ordre des coups envoyés est déterministe**.

**Boss** : `config/level1/Boss.cpp` et `config/level2/Boss.cpp` ont le **même SHA** et se contentent de `cout << "random"`. Le boss de ce referee joue donc **uniformément au hasard** parmi les coups légaux. [NON VÉRIFIÉ] que le boss en ligne soit exactement ce fichier.
- https://raw.githubusercontent.com/jdermont/AtaxxCodingame/main/config/level1/Boss.cpp
- https://raw.githubusercontent.com/jdermont/AtaxxCodingame/main/config/level2/Boss.cpp

---

## 2) Stratégies des meilleurs joueurs — état réel des sources

### 2.1 Players CodinGame : aucune stratégie publique trouvée

- Classement lu : https://r.jina.ai/https://www.codingame.com/multiplayer/bot-programming/ataxx/leaderboard — **88 joueurs**, uniquement **Wood 1 (52)** et **Wood 2 (37)**. Top : `Pduhard-` (C++, 41.46), `Astrobytes` (41.01), `jacek` (40.66), `Crazy_Remi` (39.87), `tadc_fan` (38.71), `PetyFloyd` (Pascal, 38.42), `aCat`, `Zylo`, `Waffle3z`.
- Discussion du jeu : https://r.jina.ai/https://www.codingame.com/multiplayer/bot-programming/ataxx/discuss → **aucun commentaire chargeable**.
- Forum : https://forum.codingame.com/search.json?q=ataxx et https://forum.codingame.com/t/community-puzzle-ataxx-puzzle-discussion/208495.json → **1 seul post** (CommunityBot), **0 réponse**, aucun post-mortem.
- Recherche GitHub `ataxx codingame in:readme` → **1 seul dépôt** (le referee). Aucun gist public de joueur CG identifié.

**[NON VÉRIFIÉ] : l'algorithme, l'évaluation et les poids des joueurs du top CodinGame ataxx. Aucune source publique lue ne les documente.** Le jeu est très récent (création du referee le 2026-08-23, annonce forum le 2026-08-22).

### 2.2 Bots / moteurs Ataxx publics (hors CodinGame — à ne pas confondre avec le top CG)

| Source | Idée + constantes lues | Pourquoi ça marche |
|---|---|---|
| https://github.com/feifeiobama/AtaxxBot (`ataxx.cpp`, `MCTS.cpp`) | Alpha-beta à approfondissement itératif ; `val() = sum[moi] − sum[adv]` ; terminal `±50` ; MCTS : `mean − 2·√ln(N)/n`, `DEAD=50`, `MAX_VAL=1000`, budget `0.88` du temps (`limitT 0.9`, `initDepth 4`, `detectN 3`) | Le nombre de pièces est **monotone** (une capture rapporte jusqu'à 8 cases) : la différence de pièces est un signal stable et directement corrélé au résultat ; l'UCT pénalise les coups peu visités. |
| https://github.com/QtLab/Ataxx + https://r.jina.ai/https://raw.githubusercontent.com/QtLab/Ataxx/master/AI%E7%AE%80%E8%A6%81%E8%AF%B4%E6%98%8E.pdf | Minimax/α-β ; évaluation principale = différence de pièces ; **gros bonus si > 24 pièces** (majorité) ; recommandations : bitboards 64 bits, hash de transposition anti-cycle, ordre de coups « pièces proches du bord adverse en premier », randomisation, iterative deepening | Le seuil 24/49 est le basculement vers une position gagnante ; l'ordre de coups maximise les coupures α-β. |
| https://cs.curtisbright.com/reports/cs686project.pdf (CS 686, 2010) | Negamax + α-β, 2 entiers 64 bits, **X Score = nbX − nbO** ; profondeur 5 « instantanée », 6 ≈ 30 s au milieu de partie ; battu *The 7th Guest* 35–14 et *SlimeWar* 27–22 | Le X Score est l'évaluation naturelle de Ataxx ; l'α-β rend la profondeur 5–6 abordable. |
| https://leoribeiro.github.io/papers/mcts-ataxx-eniac2018.pdf + https://github.com/leoribeiro/mcts-ataxx | MCTS, **UCT avec Cp = 1** ; score **binaire** (victoires/visites) vs **fractionnel** (fraction de pièces) ; 100 itérations → ~80 % de victoires contre minimax2 ; MCTS+UCB+rollouts minimax2 = meilleur ; facteur de branchement ~61.6, longueur ~110.9 plis, complexité ~2.91×10¹⁹⁸ | Le score fractionnel est **plus informatif** que la victoire binaire ; plus d'itérations réduisent la variance (surtout au milieu de partie où le branchement culmine à ~90). |
| https://github.com/titaniummachine1/titanium-ataxx (`README.md`) | Moteur Rust : bitboard `u64` 49 bits, PVS/α-β itératif, TT Zobrist 24 octets, LMR/futility, **ordre de coups par nombre de pièces infectées**, éval **matériel + mobilité**, NNUE 37,7 M positions, head de politique **+71 Elo à 5k nœuds / +126 Elo à 100 ms**, petit filet −49 % d'appels NNUE, élagage des cycles de saut | Le nombre de captures est le meilleur critère d'ordre ; la mobilité capte les positions « étouffées » ; sous 100 ms, un NNUE lourd est inutilisable en direct mais les principes (ordre, TT, cycles) se transposent. |
| https://raw.githubusercontent.com/titaniummachine1/titanium-ataxx/main/docs/COMMUNITY_RESEARCH_2026.md | Doc communautaire citant KataAtaxx (MCTS), Monty, Mexx, Moonbird, ataxx-dot-cpp (HCE), datasets NNUE, etc. **[NON VÉRIFIÉ]** : je n'ai lu que la doc de synthèse, pas ces moteurs | — |

---

## 3) Recommandation concrète et testable pour battre le boss / monter au classement

### 3.1 Exploiter le referee

1. **Parser correctement** : `ligne i → row 7−i` (voir §1.2). C'est le bug/piège n°1.
2. **Boss aléatoire** : un greedy 1-ply qui choisit le coup maximisant `mes_pièces − ses_pièces` bat déjà un adversaire uniforme. Pour le classement, monter en **negamax α-β itératif 3–5 plis**.
3. **Anticiper les nuls** : le referee donne 0–0 à 300 tours et à la 3e répétition. Suivez exactement son hash `(bb0;bb1;joueur)`. **Si vous êtes derrière**, recherchez la répétition / le passage à 300 tours ; **si vous êtes devant**, évitez la répétition et cherchez à **remplir le plateau avant le tour 300** (seul chemin vers une victoire nette).
4. **Budget temps** : 85 ms de limite dure par tour (marge sous les 100 ms), 900 ms au premier tour ; approfondissement itératif avec TT pour toujours avoir un coup valide prêt.
5. **Ordre de coups** : captures d'abord (`nb voisins adverses convertis` décroissant), puis mobilité. C'est le gain α-β le mieux documenté (Titanium, QtLab).

### 3.2 Évaluation à tester (poids initiaux, tous [NON VÉRIFIÉ] pour ataxx/CG)

```
E = (mesPièces - sesPièces)
  + wM * (mesCoups - sesCoups)
  + B  * signe(majorité)      // +B si mesPièces >= 25, -B si sesPièces >= 25
  + terminal: +1000 / -1000 / 0 (nul)
Grille : wM ∈ {0, 0.05, 0.10, 0.20} ; B ∈ {0, 20, 50}
```

### 3.3 Protocole de test A/B (vérifiable en 1 session)

- Portez `getActions()`/`applyAction()`/`isGameOver()` du `Board.java` du referee (spawns dédupliqués, jumps non dédupliqués, 8 conversions) dans votre harness local.
- **200 parties par configuration**, graines fixées, couleurs alternées (seat-balanced), contre (a) le boss `random`, (b) un greedy 1-ply.
- Métriques : **taux de victoire** (cible ≥ 95 % vs boss), taux vs greedy, marge moyenne de pièces, profondeur médiane atteinte en 85 ms, nœuds/s, **0 timeout**, taux de nuls.
- Alternative anti-boss encore plus directe, inspirée de l'article ENIAC : **1 étape de Monte-Carlo** — pour chaque coup légal, échantillonnez 100 réponses aléatoires du boss et choisissez la plus grande marge espérée. L'article mesure > 90 % de victoires contre un joueur aléatoire **dès 1 itération** ; 100 échantillons devraient être quasi certains.
- N'acceptez un changement de `wM`/`B` que si le taux de victoire progresse d'au moins **5 points** avec intervalles de confiance 95 % disjoints (ou SPRT).

---

## 4) Liste de toutes les URL lues

**Referee / jeu officiel**
1. https://github.com/jdermont/AtaxxCodingame
2. https://raw.githubusercontent.com/jdermont/AtaxxCodingame/main/README.md
3. https://api.github.com/repos/jdermont/AtaxxCodingame/contents/
4. https://api.github.com/repos/jdermont/AtaxxCodingame/git/trees/main?recursive=1
5. https://raw.githubusercontent.com/jdermont/AtaxxCodingame/main/config/config.ini
6. https://raw.githubusercontent.com/jdermont/AtaxxCodingame/main/config/statement_en.html
7. https://raw.githubusercontent.com/jdermont/AtaxxCodingame/main/config/stub.txt
8. https://raw.githubusercontent.com/jdermont/AtaxxCodingame/main/config/level1/Boss.cpp
9. https://raw.githubusercontent.com/jdermont/AtaxxCodingame/main/config/level2/Boss.cpp
10. https://raw.githubusercontent.com/jdermont/AtaxxCodingame/main/src/main/java/com/codingame/game/Referee.java
11. https://raw.githubusercontent.com/jdermont/AtaxxCodingame/main/src/main/java/com/codingame/game/ataxx/Board.java
12. https://raw.githubusercontent.com/jdermont/AtaxxCodingame/main/src/main/java/com/codingame/game/ataxx/Viewer.java
13. https://raw.githubusercontent.com/jdermont/AtaxxCodingame/main/src/main/resources/view/config.js
14. https://www.codingame.com/multiplayer/bot-programming/ataxx (via r.jina.ai)
15. https://r.jina.ai/https://www.codingame.com/multiplayer/bot-programming/ataxx/discuss
16. https://r.jina.ai/https://www.codingame.com/multiplayer/bot-programming/ataxx/leaderboard
17. https://api.github.com/repos/CodinGame/Ataxx (**404**)
18. https://api.github.com/repos/eulerscheZahl/RefereeCollection/contents/

**Forum / discussion**
19. https://www.codingame.com/forum/search?q=ataxx
20. https://forum.codingame.com/search.json?q=ataxx
21. https://forum.codingame.com/t/community-puzzle-ataxx-puzzle-discussion/208495.json

**Bots / code public**
22. https://api.github.com/search/repositories?q=ataxx+codingame
23. https://api.github.com/search/repositories?q=ataxx+in%3Aname
24. https://api.github.com/search/repositories?q=ataxx+bot&sort=stars&order=desc&per_page=30
25. https://api.github.com/search/repositories?q=ataxx+codingame+in:readme&per_page=30
26. https://api.github.com/search/repositories?q=ataxx+created:%3E2026-08-01&sort=updated&order=desc&per_page=50
27. https://api.github.com/search/code?q=ataxx+codingame (**401**)
28. https://github.com/feifeiobama/AtaxxBot ; https://raw.githubusercontent.com/feifeiobama/AtaxxBot/master/README.md ; .../MCTS.cpp ; .../ataxx.cpp
29. https://github.com/QtLab/Ataxx ; https://api.github.com/repos/QtLab/Ataxx/contents/ ; https://r.jina.ai/https://raw.githubusercontent.com/QtLab/Ataxx/master/AI%E7%AE%80%E8%A6%81%E8%AF%B4%E6%98%8E.pdf
30. https://github.com/leoribeiro/mcts-ataxx ; https://api.github.com/repos/leoribeiro/mcts-ataxx/contents/ ; https://raw.githubusercontent.com/leoribeiro/mcts-ataxx/master/mcts.py
31. https://github.com/titaniummachine1/titanium-ataxx ; https://raw.githubusercontent.com/titaniummachine1/titanium-ataxx/main/README.md ; https://api.github.com/repos/titaniummachine1/titanium-ataxx/contents/README.md ; https://raw.githubusercontent.com/titaniummachine1/titanium-ataxx/main/docs/COMMUNITY_RESEARCH_2026.md

**Articles / académique**
32. https://r.jina.ai/https://leoribeiro.github.io/papers/mcts-ataxx-eniac2018.pdf
33. https://r.jina.ai/https://cs.curtisbright.com/reports/cs686project.pdf
34. https://arxiv.org/search/?query=ataxx&searchtype=all (**aucun résultat**)
35. https://api.semanticscholar.org/graph/v1/paper/search?query=Ataxx... (**429**, plusieurs tentatives)

**Découverte (DuckDuckGo HTML)**
36. https://html.duckduckgo.com/html/?q=codingame+ataxx+post+mortem+bot
37. https://html.duckduckgo.com/html/?q=%22ataxx%22+codingame+github+bot+alpha-beta
38. https://html.duckduckgo.com/html/?q=site%3Acodingame.com%2Fforum+ataxx
39. https://html.duckduckgo.com/html/?q=%22ataxx%22+%22codingame%22+site%3Agithub.com
40. https://html.duckduckgo.com/html/?q=codingame+ataxx+discuss+algorithm+boss
41. https://html.duckduckgo.com/html/?q=%22actionsCount%22+%22lastAction%22+ataxx+codingame
42. https://html.duckduckgo.com/html/?q=%22ataxx%22+%22codingame%22+bot+solution+legend
43. https://html.duckduckgo.com/html/?q=site%3Aforum.codingame.com+ataxx
44. https://html.duckduckgo.com/html/?q=%22ataxx%22+codingame+MCTS+%22100+ms%22+bot+strategy
45. https://html.duckduckgo.com/html/?q=%22codiname%22+ataxx+bot+github+source+code
46. https://html.duckduckgo.com/html/?q=%22titanium-ataxx%22+evaluation+search+engine
