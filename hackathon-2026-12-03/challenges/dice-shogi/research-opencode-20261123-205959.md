<!-- opencode model: hy4-flash-free, 2026-11-23T21:03:57+03:00 -->

> build · hy4-flash-free
Je vais lancer une recherche approfondie en parallèle sur plusieurs sources. Commençons par localiser le jeu, le referee officiel et les bots publics.
✗ Web Search via OpenCode "CodinGame dice-shogi bot programming game rules" failed
Error: Web search request failed (HTTP 402)
✗ Web Search via OpenCode "eulerscheZahl RefereeCollection dice-shogi" failed
Error: Web search request failed (HTTP 402)
✗ Web Search via OpenCode "github CodinGame dice-shogi referee" failed
Error: Web search request failed (HTTP 402)
✗ Web Search via OpenCode "dice shogi CodinGame post mortem strategy top player" failed
Error: Web search request failed (HTTP 402)
> build · hy4-flash-free
Websearch indisponible (HTTP 402). Je bascule sur l'API GitHub et le fetch direct.
% WebFetch https://www.codingame.com/multiplayer/bot-programming/dice-shogi 
% WebFetch https://api.github.com/repos/eulerscheZahl/RefereeCollection/contents/ 
% WebFetch https://api.github.com/search/repositories?q=codingame+dice+shogi 
% WebFetch https://api.github.com/search/repositories?q=dice-shogi 
> build · hy4-flash-free
Le referee n'est pas dans RefereeCollection. Je trouve un bot Java `jdermont/DiceShogi` (topics MCTS/minimax) et le jeu original `takahiro-itou/DiceShogi`. Approfondissons.
✗ WebFetch https://api.github.com/search/code?q=dice-shogi+referee+codingame failed
Error: StatusCode: non 2xx status code (401 GET https://api.github.com/search/code?q=dice-shogi+referee+codingame)
✗ WebFetch https://api.github.com/repos/takahiro-itou/DiceShogi/contents/ failed
Error: StatusCode: non 2xx status code (404 GET https://api.github.com/repos/takahiro-itou/DiceShogi/contents/)
% WebFetch https://api.github.com/repos/jdermont/DiceShogi/contents/ 
% WebFetch https://api.github.com/search/repositories?q=CodinGame+contest+referee 
> build · hy4-flash-free
% WebFetch https://api.github.com/repos/jdermont/DiceShogi/contents/src?ref=main 
% WebFetch https://api.github.com/repos/jdermont/DiceShogi/contents/config?ref=main 
% WebFetch https://raw.githubusercontent.com/jdermont/DiceShogi/main/README.md 
% WebFetch https://api.github.com/orgs/CodinGame/repos?per_page=100 
> build · hy4-flash-free
Le dépôt `jdermont` contient même l'énoncé officiel et le code du Boss. Je récupère ces pièces maîtresses.
$ grep -io '"full_name":"[^"]*\(dice\|shogi\)[^"]*"' /home/arafaram/.local/share/opencode/tool-output/tool_20990fc5d001M7uwxZyKzk43Il | head -50
(no output)
% WebFetch https://raw.githubusercontent.com/jdermont/DiceShogi/main/config/statement_en.html 
% WebFetch https://raw.githubusercontent.com/jdermont/DiceShogi/main/config/stub.txt 
% WebFetch https://raw.githubusercontent.com/jdermont/DiceShogi/main/config/Boss.java 
> build · hy4-flash-free
L'énoncé officiel est récupéré (et pointe vers ce dépôt comme "referee"). Je récupère l'arbre complet du code pour trouver le moteur et l'évaluation du bot.
% WebFetch https://api.github.com/repos/jdermont/DiceShogi/git/trees/main?recursive=1 
> build · hy4-flash-free
% WebFetch https://raw.githubusercontent.com/jdermont/DiceShogi/main/src/main/java/com/codingame/game/Referee.java 
% WebFetch https://raw.githubusercontent.com/jdermont/DiceShogi/main/config/config.ini 
% WebFetch https://raw.githubusercontent.com/jdermont/DiceShogi/main/src/main/java/com/codingame/game/Game.java 
% WebFetch https://raw.githubusercontent.com/jdermont/DiceShogi/main/src/main/java/com/codingame/game/Piece.java 
> build · hy4-flash-free
Le referee confirme déjà : 100 ms/tour, 300 tours max, dés tirés aléatoirement, coups légaux **melangés**. Je lis maintenant `GameState.java` (génération des coups, échec, promotion, départage).
% WebFetch https://raw.githubusercontent.com/jdermont/DiceShogi/main/src/main/java/com/codingame/game/GameState.java 
% WebFetch https://raw.githubusercontent.com/jdermont/DiceShogi/main/src/main/java/com/codingame/game/Move.java 
> build · hy4-flash-free
Referee entièrement lu. Je cherche maintenant bots/post-mortems/articles et le code du boss dans `demo.js`.
% WebFetch https://raw.githubusercontent.com/jdermont/DiceShogi/main/src/main/resources/view/demo.js 
% WebFetch https://export.arxiv.org/api/query?search_query=all:%22dice%20shogi%22&start=0&max_results=10 
✗ WebFetch https://api.semanticscholar.org/graph/v1/paper/search?query=dice%20shogi&fields=title,abstract,url,year,authors&limit=20 failed
Error: StatusCode: non 2xx status code (429 GET https://api.semanticscholar.org/graph/v1/paper/search?query=dice%20shogi&fields=title,abstract,url,year,authors&limit=20)
% WebFetch https://html.duckduckgo.com/html/?q=codingame+dice-shogi+post+mortem 
> build · hy4-flash-free
DuckDuckGo fonctionne. Je cherche bots publics, discussions et travaux académiques ciblés.
% WebFetch https://www.codingame.com/multiplayer/bot-programming/dice-shogi/discuss 
% WebFetch https://html.duckduckgo.com/html/?q=site%3Agithub.com+%22dice-shogi%22+codingame+bot+evaluation 
% WebFetch https://html.duckduckgo.com/html/?q=codingame+%22dice-shogi%22+algorithm+mcts+strategy+ranking 
% WebFetch https://html.duckduckgo.com/html/?q=%22Dice+Shogi%22+AI+paper+Monte+Carlo+minishogi 
> build · hy4-flash-free
Trouvé un article IEEE/Semantic Scholar sur Dice-Shogi (Star1/Star2/Star2.5). Je le lis en détail, ainsi qu'un AI公开 de variante.
% WebFetch https://www.semanticscholar.org/paper/cb023eeaacb4187a0e9e9fbd85fe16dba526926f 
% WebFetch https://ieeexplore.ieee.org/document/8356914 
✗ WebFetch https://raw.githubusercontent.com/sankanou/HyperDiceShogi/master/README.md failed
Error: Unsupported fetched file content type: application/octet-stream
✗ WebFetch https://api.semanticscholar.org/graph/v1/paper/cb023eeaacb4187a0e9e9fbd85fe16dba526926f?fields=title,abstract,year,authors,venue,externalIds,openAccessPdf,url,citationCount,references failed
Error: StatusCode: non 2xx status code (429 GET https://api.semanticscholar.org/graph/v1/paper/cb023eeaacb4187a0e9e9fbd85fe16dba526926f?fields=title,abstract,year,authors,venue,externalIds,openAccessPdf,url,citationCount,references)
> build · hy4-flash-free
% WebFetch https://html.duckduckgo.com/html/?q=%22Improvement+and+Evaluation+of+Search+Algorithm+in+Computer+Dice-Shogi%22+pdf 
% WebFetch https://html.duckduckgo.com/html/?q=github+%22dice+shogi%22+bot+alpha-beta+OR+mcts+OR+minimax+codingame 
% WebFetch https://html.duckduckgo.com/html/?q=%22dice-shogi%22+codingame+solution+github+bot 
% WebFetch https://api.github.com/repos/sankanou/HyperDiceShogi/contents 
> build · hy4-flash-free
% WebFetch https://raw.githubusercontent.com/jdermont/DiceShogi/main/src/test/java/Player2.java 
% WebFetch https://raw.githubusercontent.com/jdermont/DiceShogi/main/src/test/java/Player1.java 
% WebFetch https://html.duckduckgo.com/html/?q=Baba+Ito+%22Dice-Shogi%22+Star2+evaluation+function+paper 
% WebFetch https://html.duckduckgo.com/html/?q=site%3Aforum.codingame.com+dice+shogi+strategy 
✗ WebFetch https://github.com/sankanou/HyperDiceShogi/blob/master/README.md failed
Error: StatusCode: non 2xx status code (503 GET https://github.com/sankanou/HyperDiceShogi/blob/master/README.md)
> build · hy4-flash-free
% WebFetch https://forum.codingame.com/search.json?q=dice%20shogi 
% WebFetch https://www.bing.com/search?q=%22Computer+Dice-Shogi%22+Star2+evaluation+function+Baba+Ito+pdf 
% WebFetch https://export.arxiv.org/api/query?search_query=all:%22shogi%22+AND+all:%22dice%22&start=0&max_results=10 
% WebFetch https://www.mojeek.com/search?q=codingame+dice-shogi+bot+github 
> build · hy4-flash-free
% WebFetch https://forum.codingame.com/t/203767/posts.json 
✗ WebFetch https://api.semanticscholar.org/graph/v1/paper/search?query=Computer%20Dice-Shogi&limit=5&fields=title,abstract,year,authors,externalIds,openAccessPdf failed
Error: StatusCode: non 2xx status code (429 GET https://api.semanticscholar.org/graph/v1/paper/search?query=Computer%20Dice-Shogi&limit=5&fields=title,abstract,year,authors,externalIds,openAccessPdf)
% WebFetch https://forum.codingame.com/search.json?q=shogi 
% WebFetch https://api.openalex.org/works?search=Computer%20Dice-Shogi%20search%20algorithm&per-page=10 
> build · hy4-flash-free
% WebFetch https://arxiv.org/abs/2611.24486 
% WebFetch https://raw.githubusercontent.com/jdermont/DiceShogi/main/src/test/java/Perft.java 
% WebFetch https://arxiv.org/html/2611.24486 
% WebFetch https://api.openalex.org/works?filter=openalex_id:W1947291763%7CW1958962762%7CW1990043405%7CW2041810683%7CW2172618108%7CW2257979135%7CW2291772168%7CW2579730587%7CW2951565863&per-page=20&select=id,title,publication_year,doi 
> build · hy4-flash-free
% WebFetch https://www.codingame.com/ide/puzzle/dice-shogi 
% WebFetch https://cdn.jsdelivr.net/gh/sankanou/HyperDiceShogi@master/README.md 
% WebFetch https://api.github.com/search/repositories?q=shogi+codingame+bot&per_page=30 
% WebFetch https://gist.github.com/search?q=dice-shogi+codingame 
> build · hy4-flash-free
% WebFetch https://raw.githubusercontent.com/sankanou/HyperDiceShogi/master/src/search.c 
% WebFetch https://api.github.com/repos/sankanou/HyperDiceShogi/contents/src 
> build · hy4-flash-free
% WebFetch https://raw.githubusercontent.com/sankanou/HyperDiceShogi/master/src/header.h 
% WebFetch https://api.github.com/repos/sankanou/HyperDiceShogi/contents/log 
% WebFetch https://api.github.com/repos/nakatsuka-y/HyperDiceShogi/contents 
> build · hy4-flash-free
# Recherche approfondie — CodinGame « Dice Shogi »

**Méthode / limites.** `websearch` était indisponible (HTTP 402) ; j'ai utilisé l'API GitHub, `raw.githubusercontent.com`, les pages HTML de DuckDuckGo/Bing, l'API Discourse de CodinGame, OpenAlex, arXiv et Semantic Scholar. Aucun fichier de code n'a été modifié, aucun build lancé, `~/.config/codingame` non lu.

**Constat important :** je n'ai trouvé **aucun bot public avec fonction d'évaluation chiffrée pour ce jeu précis**, ni post-mortem de meilleur joueur. Le dépôt public lié par l'énoncé officiel est le **referee**, et ses seuls « bots » sont des joueurs aléatoires de test. Le topic forum officiel ne contient que 2 messages, aucun strategy. Je le signale explicitement plutôt que d'attribuer des stratégies à des joueurs sans source.

---

## 1. Faits du referee (URL exactes)

Source primaire : l'énoncé officiel lui-même renvoie vers `https://github.com/jdermont/DiceShogi/` avec le libellé « Click here for the referee ».

| Fait | Valeur vérifiée | URL exacte |
|---|---|---|
| Plateau | 5×5, 6 pièces/joueur ; setup interne : joueur 0 (bas) `K,G,S,B,R` en ligne 0 et `P` en (1,0) ; joueur 1 (haut) miroir en ligne 4 et `P` en (3,4) | https://raw.githubusercontent.com/jdermont/DiceShogi/main/src/main/java/com/codingame/game/GameState.java |
| Temps | 1er tour ≤ **1000 ms**, chaque tour ≤ **100 ms**, durée d'animation 1500 ms | https://raw.githubusercontent.com/jdermont/DiceShogi/main/src/main/java/com/codingame/game/Referee.java |
| Fin | **300 tours maximum** (`rounds >= 300`) → **nul, scores 0/0** | https://raw.githubusercontent.com/jdermont/DiceShogi/main/src/main/java/com/codingame/game/Game.java |
| Dé | `random.nextInt(6)+1`, uniforme 1–6 ; si `seed==0` alors toujours **6** (minishogi pur) | https://raw.githubusercontent.com/jdermont/DiceShogi/main/src/main/java/com/codingame/game/Game.java |
| Dé → colonne | `colTo == 4-(dé-1)` ; dé 5 → colonne index 0 (notée « 5 »), dé 1 → index 4 (notée « 1 »). **Dé 6 = toutes colonnes** ; si aucun coup légal dans la colonne → **tous les coups redeviennent légaux** | https://raw.githubusercontent.com/jdermont/DiceShogi/main/src/main/java/com/codingame/game/Game.java |
| Entrée par tour | 5 lignes plateau, main joueur 1, main joueur 2, dé, dernier coup adverse (`none` au départ), `actionCount`, puis `actionCount` coups | https://raw.githubusercontent.com/jdermont/DiceShogi/main/config/statement_en.html |
| Sortie | `colFrom rowFrom colTo rowTo` (`1425+` pour promouvoir) ou `Piece*colToRowTo` (`G*33`) ou `random` ; un message après espace est accepté et ignoré | https://raw.githubusercontent.com/jdermont/DiceShogi/main/config/stub.txt |
| Coups légaux fournis | Le referee **génère lui-même** les coups légaux filtrés par le dé et **les mélange aléatoirement** (`Collections.shuffle`) avant envoi : l'ordre de la liste n'est donc pas une information | https://raw.githubusercontent.com/jdermont/DiceShogi/main/src/main/java/com/codingame/game/Referee.java |
| Résolution d'un tour | 1) dé tiret ; 2) coups filtrés/mélangés envoyés ; 3) `player.execute()` ; 4) `parsePlayerOutput` (1re ligne seulement, comparaison insensible à la casse) ; 5) `game.makeMove` ; 6) test `isGameOver()` ; 7) timeout/coup invalide → défaite immédiate de ce joueur | https://raw.githubusercontent.com/jdermont/DiceShogi/main/src/main/java/com/codingame/game/Referee.java |
| Échec / mat / pat | `generateMoves()` vide → **le joueur courant perd** (qu'il soit mat ou pat) ; `getWinner()` renvoie `currentPlayer^1` | https://raw.githubusercontent.com/jdermont/DiceShogi/main/src/main/java/com/codingame/game/Game.java |
| Promotions | Rook/Bishop/Silver/Pawn : promotion possible si **arrivée OU départ** de la dernière rangée ; Pawn promu obligatoirement ; Roi et Or non promouvables ; capture → pièce **dé-promue** dans la main | https://raw.githubusercontent.com/jdermont/DiceShogi/main/src/main/java/com/codingame/game/GameState.java · https://raw.githubusercontent.com/jdermont/DiceShogi/main/src/main/java/com/codingame/game/Piece.java |
| Drops | Pion interdit sur dernière rangée et sur toute colonne contenant déjà un pion du joueur ; **drop pion mat interdit** (`ALLOW_PAWN_DROP_CHECKMATE=false`) ; un seul coup de drop généré par type de pion (déduplication) | https://raw.githubusercontent.com/jdermont/DiceShogi/main/src/main/java/com/codingame/game/GameState.java |
| Scores / départage | Victoire = `+1/-1`, nul = `0/0`. **Aucun autre départage n'existe dans le code du referee** ; le départage Elo/league de CodinGame n'est pas dans cette source → **[NON VÉRIFIÉ]** | https://raw.githubusercontent.com/jdermont/DiceShogi/main/src/main/java/com/codingame/game/Referee.java |
| Vecteurs de test perft | position initiale : 14, 181, 2512, 35401, 533203, 8276188 coups (profondeurs 1→6) avec `ALLOW_PAWN_DROP_CHECKMATE=true` | https://raw.githubusercontent.com/jdermont/DiceShogi/main/src/test/java/Perft.java |

À noter : **RefereeCollection d'eulerscheZahl ne contient pas dice-shogi** (vérifié : seulement BackToTheCode, CodeBusters, GameOfDrones, HyperSonic, PokerChipRace, SmashTheCode, TheGreatEscape, Tron) — https://api.github.com/repos/eulerscheZahl/RefereeCollection/contents/. Le créateur coté CodinGame est `@jacek`, validateurs `@field3`, `@MSmits`, `@Illedan — https://forum.codingame.com/t/203767/posts.json.

---

## 2. Stratégies des meilleurs joueurs — ce qui est réellement vérifiable

### 2.1 Bot public « officiel » : en réalité un referee + joueurs aléatoires
- URL : https://github.com/jdermont/DiceShogi (README : https://raw.githubusercontent.com/jdermont/DiceShogi/main/README.md)
- Le dépôt a les topics `mcts`, `minimax`, mais l'arbre complet (https://api.github.com/repos/jdermont/DiceShogi/git/trees/main?recursive=1) ne contient **aucun bot MCTS/alpha-beta** : seulement `com/codingame/game/*` (referee), `view/*`, et `src/test/java/Player1.java` / `Player2.java` qui jouent `"random"` ou un coup aléatoire.
- https://raw.githubusercontent.com/jdermont/DiceShogi/main/src/test/java/Player2.java
- **Conclusion : toute affirmation du type « les meilleurs joueurs font du MCTS avec tel poids » est [NON VÉRIFIÉ].** Recherches complémentaires infructueuses : gist search (`"dice-shogi codingame"` → 0 résultat), `api.github.com/search/repositories?q=shogi+codingame+bot` → 0, DuckDuckGo `site:github.com "dice-shogi" codingame bot evaluation` → aucun bot.

### 2.2 Code public d'IA pour une variante proche (poids concrets)
- URL : https://raw.githubusercontent.com/sankanou/HyperDiceShogi/master/src/search.c
- Projet : **Hyper Dice Shogi** (variante japonaise apparentée, **pas le jeu CodinGame**) — https://github.com/sankanou/HyperDiceShogi ; README via CDN : https://cdn.jsdelivr.net/gh/sankanou/HyperDiceShogi@master/README.md.
- Algorithme lu dans le code : négamax à **profondeur fixe 5** (`SEARCH_DEPTH`, https://raw.githubusercontent.com/sankanou/HyperDiceShogi/master/src/header.h), avec gestion dédiée des nœuds matés (`SCORE_MATED + ply`). Aux nœuds non matés, il **génère les coups pour chaque valeur de dé 1..5, prend le max par dé, puis moyenne sur 6** : `(dice_max[0]+…+dice_max[4]+max)/6`, le terme `max` approximant le cas « dé 6 = n'importe quelle colonne ». C'est un **expectiminimax approximé sur le dé**.
- Évaluation lue (coupures rouge/bleu, `popuCount` pour les pièces sur plateau, `W_HAND/B_HAND` pour la main) :

| Pièce | Sur plateau | En main |
|---|---|---|
| Pion | 100 | 100 |
| Argent | 300 | 280 |
| Or | 350 | 370 |
| Fou | 350 | 340 |
| Tour | 400 | 390 |
| +Pion | 200 | — |
| +Argent | 350 | — |
| +Fou (cheval) | 450 | — |
| +Tour (dragon) | 500 | — |

- **Pourquoi ça marche (interprétation) :** les pièces en main valent presque autant que sur plateau (et l'Or en main *plus* cher : 370 vs 350), ce qui reflète la mécanique de drop du shogi ; la moyenne sur les 6 dés évite de choisir un coup bon uniquement pour une valeur de dé.
- ⚠️ Le transférabilité de ces poids à **Dice Shogi (CodinGame)** est **[NON VÉRIFIÉ]** : jeu différent, et aucun test comparatif publié dans ce dépôt (`log/` ne contient qu'un `.gitkeep`).

### 2.3 Articles académiques
1. **Baba & Ito, « Improvement and Evaluation of Search Algorithm in Computer Dice-Shogi », TAAI 2017**, DOI `10.1109/TAAI.2017.49` — https://doi.org/10.1109/TAAI.2017.49 ; index OpenAlex : https://openalex.org/W2802885584 ; Semantic Scholar : https://www.semanticscholar.org/paper/cb023eeaacb4187a0e9e9fbd85fe16dba526926f.
   -(Abstract **lu** via OpenAlex/IEEE) : `*`-Minimax appliqué à Dice-Shogi ; implémentation de **Star1, Star2, Star2.5** ; **« Star2 can search the most efficiently »**.
   - Les détails (profondeurs, fonctions d'évaluation, tailles d'échantillons) sont **[NON VÉRIFIÉ]** : le texte intégral est payant/OpenAccess=false.
2. **Cohen-Solal & Cazenave, « Study and Improvement of Search Algorithms in Stochastic Perfect-Information Games », arXiv:2611.24486 (18 nov. 2026)** — https://arxiv.org/abs/2611.24486 ; texte intégral lu : https://arxiv.org/html/2611.24486.
   - **Unbounded Expectiminimax** (best-first, un seul successeur de hasard échantillonné par itération, backup expectiminimax exact) + **Completion** (ne jamais jouer une action résolue perdante ; jouer une action gagnante à 100 %).
   - Résultats lus (10 s/coup, score binaire moyen contre expectiminimax de référence) : Unbounded E + C **+6,77** ; Expectiminimax + C −0,99 ; MCTS_h −7,24 ; **MCTS pur −55,73**. À 1 s : Safe Unbounded E + C **+4,69**, MCTS pur **−70,22**. Completion ≈ **+8 %** ; MCTS sans évaluation apprise est le pire.
   - ⚠️ Les jeux testés sont Azul, Backgammon, Can't Stop, EinStein, Kingdomino, Martian Dice, Shogun — **pas Dice Shogi**. L'applicabilité directe est donc une hypothèse **[NON VÉRIFIÉ]**, mais le parallèle est fort (dé = nœud de hasard).

### 2.4 Forum / post-mortems
- Unique topic trouvé : https://forum.codingame.com/t/community-puzzle-dice-shogi-puzzle-discussion/203767 (2 messages, le second étant « Updated game. player texts under avatars now have colors ») — **aucun contenu stratégique**.
- Page de discussion du jeu : https://www.codingame.com/multiplayer/bot-programming/dice-shogi/discuss → renvoie « Loading... » côté fetch, contenu non lu → **[NON VÉRIFIÉ]**.
- Aucun post-mortem de(top player) sur dice-shogi trouvé via DDG/Bing/forum search → **[NON VÉRIFIÉ]**.

---

## 3. Recommandation concrète et testable pour battre le boss

**Hypothèse centrale (testable) :** remplacer un alpha-beta/MCTS naïf par un **expectiminimax à budget temps avec modèle de dé exact**, puis n'ajouter que des termes d'évaluation mesurables. Le jeu est particulièrement adapté : le hasard n'a que **6 issues uniformes**, et le referee fournit déjà la liste des coups légaux au root (économie de génération).

### Protocole en 3 étapes

1. **Modèle de chance exact (gratuit, vérifié referee).** Au lieu d'un coup « moyen », étendre 6 nœuds : pour `d=1..5`, coups se terminant en `colTo = 4-(d-1)` ; si la liste est vide → **tous les coups** ; pour `d=6` → **tous les coups**. Valoir `Σ V(d)/6`. C'est exactement le schéma de sankanou (mesure indirecte de sa pertinence) et la forme la plus simple d'expectiminimax.
2. **Évaluation de départ (chiffres lus, variante proche).** Material + main : P 100 / S 300 / G 350 / B 350 / R 400 / +P 200 / +S 350 / +B 450 / +R 500 ; main : P 100 / S 280 / G 370 / B 340 / R 390. Ajouter ensuite, **uniquement si le test le valide**, un terme de sécurité du roi et un terme « nombre de coups légaux après mon coup » (mobilité) — coefficients **[NON VÉRIFIÉ]**, à régler par balayage.
3. **Recherche.** Iterative deepening + transposition table, arrêt à ~85–90 ms par tour (marge pour le timeout de 100 ms), et **règle de Completion** : si un coup force `generateMoves()==∅` chez l'adversaire, le jouer immédiatement ; écarter tout coup permettant un mat adverse immédiat.

### Expérience A/B à lancer (vérifiable en local avec le referee public)

| Test | Protocole | Critère de réussite |
|---|---|---|
| T1 — contrôle sans dé | Jouer avec `seed=0` (tous dés = 6) : c'est du minishogi pur | Isoler la qualité de l'éval ; un gain ici vient de l'éval, pas du hasard |
| T2 — expectiminimax vs alpha-beta | 200 parties par couleur, mêmes graines, même budget 100 ms | ≥ +10 % de winrate contre le bot actuel |
| T3 — Completion seule | Activer/désactiver uniquement la règle « mat immédiat » | Diminution mesurable du nombre de parties perdues par mat évitable |
| T4 — poids | Balayage ±20 % sur G-main (370) et R-main (390) | Winrate significatif (intervalle de confiance non chevauchant) |
| T5 — robustesse protocole | Vérifier zéro timeout, zéro `InvalidMove`, sortie toujours une chaîne exactement présente dans la liste (liste mélangée par le referee) | 0 erreur sur 200 parties |

**Pièges concrets à exploiter / éviter (tous vérifiés dans le referee) :**
- Ne **jamais** supposer un ordre dans les `actionCount` coups reçus : ils sont **shufflés** par le referee.
- Le dé et la liste reçus sont déjà cohérents : ne pas refiltrer au root, économiser les 100 ms.
- Le pion-drop qui mate est illégal : ne pas le sélectionner (perte de temps et coup rejeté).
- Attention au passif : `ALLOW_PAWN_DROP_CHECKMATE=false` par défaut, alors que les perfts publics sont calculés avec `true` — ne pas comparer directement vos perfts à ceux de `Perft.java` sans aligner ce drapeau.
- Égalité à 300 tours = 0/0 : si vous êtes en avant matériellement, accélérer ; si vous êtes en retard, forcer les échanges/drops rather than laisser le compteur tourner.

---

## 4. Liste de toutes les URL lues

**Sources primaires (referee / énoncé / protocole)**
- https://www.codingame.com/multiplayer/bot-programming/dice-shogi (page JS, contenu non rendu au fetch)
- https://github.com/jdermont/DiceShogi
- https://raw.githubusercontent.com/jdermont/DiceShogi/main/README.md
- https://raw.githubusercontent.com/jdermont/DiceShogi/main/config/statement_en.html
- https://raw.githubusercontent.com/jdermont/DiceShogi/main/config/stub.txt
- https://raw.githubusercontent.com/jdermont/DiceShogi/main/config/Boss.java
- https://raw.githubusercontent.com/jdermont/DiceShogi/main/config/config.ini
- https://api.github.com/repos/jdermont/DiceShogi/contents/
- https://api.github.com/repos/jdermont/DiceShogi/contents/src
- https://api.github.com/repos/jdermont/DiceShogi/contents/config
- https://api.github.com/repos/jdermont/DiceShogi/git/trees/main?recursive=1
- https://raw.githubusercontent.com/jdermont/DiceShogi/main/src/main/java/com/codingame/game/Referee.java
- https://raw.githubusercontent.com/jdermont/DiceShogi/main/src/main/java/com/codingame/game/Game.java
- https://raw.githubusercontent.com/jdermont/DiceShogi/main/src/main/java/com/codingame/game/GameState.java
- https://raw.githubusercontent.com/jdermont/DiceShogi/main/src/main/java/com/codingame/game/Move.java
- https://raw.githubusercontent.com/jdermont/DiceShogi/main/src/main/java/com/codingame/game/Piece.java
- https://raw.githubusercontent.com/jdermont/DiceShogi/main/src/test/java/Player1.java
- https://raw.githubusercontent.com/jdermont/DiceShogi/main/src/test/java/Player2.java
- https://raw.githubusercontent.com/jdermont/DiceShogi/main/src/test/java/Perft.java
- https://raw.githubusercontent.com/jdermont/DiceShogi/main/src/main/resources/view/demo.js (très volumineux, lu partiellement)

**Bots / dépôts connexes**
- https://api.github.com/search/repositories?q=dice-shogi
- https://api.github.com/search/repositories?q=codingame+dice+shogi
- https://api.github.com/search/repositories?q=CodinGame+contest+referee
- https://api.github.com/search/repositories?q=shogi+codingame+bot
- https://api.github.com/repos/eulerscheZahl/RefereeCollection/contents/
- https://api.github.com/orgs/CodinGame/repos?per_page=100
- https://api.github.com/repos/takahiro-itou/DiceShogi/contents/ (404)
- https://gist.github.com/search?q=dice-shogi+codingame (0 résultat)
- https://github.com/sankanou/HyperDiceShogi
- https://api.github.com/repos/sankanou/HyperDiceShogi/contents
- https://api.github.com/repos/sankanou/HyperDiceShogi/contents/src
- https://api.github.com/repos/sankanou/HyperDiceShogi/contents/log
- https://cdn.jsdelivr.net/gh/sankanou/HyperDiceShogi@master/README.md (Shift-JIS, décodage partiel)
- https://raw.githubusercontent.com/sankanou/HyperDiceShogi/master/src/search.c
- https://raw.githubusercontent.com/sankanou/HyperDiceShogi/master/src/header.h
- https://api.github.com/repos/nakatsuka-y/HyperDiceShogi/contents

**Forum / discussions**
- https://forum.codingame.com/search.json?q=dice%20shogi
- https://forum.codingame.com/t/203767/posts.json
- https://forum.codingame.com/search.json?q=shogi
- https://www.codingame.com/multiplayer/bot-programming/dice-shogi/discuss (JS, non lu)
- https://www.codingame.com/ide/puzzle/dice-shogi (JS, non lu)

**Articles / moteurs de recherche**
- https://doi.org/10.1109/TAAI.2017.49 · https://openalex.org/W2802885584 · https://ieeexplore.ieee.org/document/8356914 · https://www.semanticscholar.org/paper/cb023eeaacb4187a0e9e9fbd85fe16dba526926f (API S2 en 429 ; résumé lu via OpenAlex/DDG)
- https://api.openalex.org/works?search=Computer%20Dice-Shogi%20search%20algorithm&per-page=10
- https://api.openalex.org/works?filter=openalex_id:W1947291763|...|W2951565863&per-page=20&select=id,title,publication_year,doi
- https://arxiv.org/abs/2611.24486 · https://arxiv.org/html/2611.24486
- https://export.arxiv.org/api/query?search_query=all:%22dice%20shogi%22&start=0&max_results=10 (0 résultat)
- https://export.arxiv.org/api/query?search_query=all:%22shogi%22+AND+all:%22dice%22&start=0&max_results=10 (0 résultat)
- https://html.duckduckgo.com/html/?q=codingame+dice-shogi+post+mortem
- https://html.duckduckgo.com/html/?q=site%3Agithub.com+%22dice-shogi%22+codingame+bot+evaluation
- https://html.duckduckgo.com/html/?q=%22Dice+Shogi%22+AI+paper+Monte+Carlo+minishogi
- https://html.duckduckgo.com/html/?q=codingame+%22dice-shogi%22+algorithm+mcts+strategy+ranking
- https://html.duckduckgo.com/html/?q=%22Improvement+and+Evaluation+of+Search+Algorithm+in+Computer+Dice-Shogi%22+pdf
- https://html.duckduckgo.com/html/?q=github+%22dice+shogi%22+bot+alpha-beta+OR+mcts+OR+minimax+codingame
- https://html.duckduckgo.com/html/?q=%22dice-shogi%22+codingame+solution+github+bot
- https://www.bing.com/search?q=%22Computer+Dice-Shogi%22+Star2+evaluation+function+Baba+Ito+pdf (0 résultat)
- https://www.mojeek.com/search?q=codingame+dice-shogi+bot+github (captcha)
- Tentatives en échec : `https://api.github.com/search/code?q=dice-shogi+referee+codingame` (401), `https://api.semanticscholar.org/graph/v1/paper/search?...` (429), `https://raw.githubusercontent.com/sankanou/HyperDiceShogi/master/README.md` (content-type octet-stream).
