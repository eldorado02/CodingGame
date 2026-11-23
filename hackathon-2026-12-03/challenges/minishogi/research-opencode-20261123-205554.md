<!-- opencode model: hy4-flash-free, 2026-11-23T20:59:59+03:00 -->

> build · hy4-flash-free
Je vais mener une recherche approfondie sur le jeu CodinGame « minishogi ». Je commence par paralléliser plusieurs searches et la lecture du contexte local.
✱ Glob "**/games.json" 1 match
✱ Glob "**/*minishogi*" 0 matches
✗ Web Search via OpenCode "CodinGame minishogi referee github rules" failed
Error: Web search request failed (HTTP 402)
✗ Web Search via OpenCode "CodinGame minishogi bot programming game" failed
Error: Web search request failed (HTTP 402)
> build · hy4-flash-free
Websearch indisponible (HTTP 402). Je bascule sur webfetch + API GitHub. Je lis d'abord le contexte local pour identifier le jeu.
→ Read bot-programming/games.json 
> build · hy4-flash-free
Le jeu est bien `minishogi` (jeu communautaire, URL `/multiplayer/bot-programming/minishogi`). Je lance maintenant les recherches web en parallèle : referee officiel, dépôts GitHub, articles.
% WebFetch https://www.codingame.com/multiplayer/bot-programming/minishogi 
✗ WebFetch https://api.github.com/repos/CodinGame/minishogi failed
Error: StatusCode: non 2xx status code (404 GET https://api.github.com/repos/CodinGame/minishogi)
% WebFetch https://api.github.com/search/repositories?q=minishogi+codingame&per_page=30 
% WebFetch https://api.github.com/search/repositories?q=minishogi&per_page=30 
> build · hy4-flash-free
Le referee officiel n'est pas `CodinGame/minishogi` (404). Je trouve un bot CodinGame prometteur (`jdermont/MiniShogi`) et la collection `eulerscheZahl/RefereeCollection`. Je creuse en parallèle.
✱ Grep "minishogi" in . · 27 matches
✗ WebFetch https://www.codingame.com/services/Challenge/getChallengeStatement failed
Error: StatusCode: non 2xx status code (405 GET https://www.codingame.com/services/Challenge/getChallengeStatement)
% WebFetch https://api.github.com/repos/eulerscheZahl/RefereeCollection/contents 
% WebFetch https://api.github.com/repos/jdermont/MiniShogi/contents 
> build · hy4-flash-free
Le referee n'est pas dans `RefereeCollection` (pas de dossier minishogi). Je lis la recherche locale existante (source vérifiable) et le dépôt `jdermont/MiniShogi`.
→ Read hackathon-2026-12-03/challenges/minishogi/research.md 
→ Read bot-programming/games/minishogi/README.md 
% WebFetch https://raw.githubusercontent.com/jdermont/MiniShogi/main/README.md 
% WebFetch https://api.github.com/repos/jdermont/MiniShogi/git/trees/3a7b68e5de17567df361363fad8389eb8fb4bca2?recursive=1 
> build · hy4-flash-free
Le dépôt `jdermont/MiniShogi` contient un `Referee.java` complet — c'est la source de référence la plus proche du referee officiel. Je récupère les fichiers clés.
% WebFetch https://raw.githubusercontent.com/jdermont/MiniShogi/main/src/main/java/com/codingame/game/Referee.java 
% WebFetch https://raw.githubusercontent.com/jdermont/MiniShogi/main/src/main/java/com/codingame/game/Piece.java 
% WebFetch https://raw.githubusercontent.com/jdermont/MiniShogi/main/src/main/java/com/codingame/game/Game.java 
% WebFetch https://api.github.com/repos/jdermont/MiniShogi/contents/config 
> build · hy4-flash-free
Très beau butin : `config/Boss.java` (le boss de la ligue !), l'énoncé officiel et le moteur de jeu. Je récupère ces fichiers.
% WebFetch https://raw.githubusercontent.com/jdermont/MiniShogi/main/config/config.ini 
% WebFetch https://raw.githubusercontent.com/jdermont/MiniShogi/main/src/test/java/Perft.java 
% WebFetch https://raw.githubusercontent.com/jdermont/MiniShogi/main/config/Boss.java 
% WebFetch https://raw.githubusercontent.com/jdermont/MiniShogi/main/config/stub.txt 
> build · hy4-flash-free
`Boss.java` ne contient qu'une sortie `random` — information majeure à confirmer. Je lis l'énoncé officiel, le moteur de jeu et les bots d'exemple, puis je cherche les post-mortems du forum.
✗ WebFetch https://raw.githubusercontent.com/jdermont/MiniShogi/main/test/java/Player1.java failed
Error: StatusCode: non 2xx status code (404 GET https://raw.githubusercontent.com/jdermont/MiniShogi/main/test/java/Player1.java)
% WebFetch https://raw.githubusercontent.com/jdermont/MiniShogi/main/config/statement_en.html 
✗ WebFetch https://raw.githubusercontent.com/jdermont/MiniShogi/main/test/java/Player2.java failed
Error: StatusCode: non 2xx status code (404 GET https://raw.githubusercontent.com/jdermont/MiniShogi/main/test/java/Player2.java)
% WebFetch https://raw.githubusercontent.com/jdermont/MiniShogi/main/src/main/java/com/codingame/game/Move.java 
% WebFetch https://html.duckduckgo.com/html/?q=codingame+minishogi+post+mortem+forum 
> build · hy4-flash-free
L'énoncé officiel confirme que `jdermont/MiniShogi` est le referee de référence. Je cherche maintenant les bots publics, le forum et les articles, et je lis le moteur de jeu.
% WebFetch https://raw.githubusercontent.com/jdermont/MiniShogi/main/src/test/java/Player1.java 
% WebFetch https://html.duckduckgo.com/html/?q=site%3Agithub.com+minishogi+codingame+bot+alpha-beta 
% WebFetch https://html.duckduckgo.com/html/?q=%22minishogi%22+codingame+forum+strategy+bot 
% WebFetch https://html.duckduckgo.com/html/?q=arxiv+minishogi+AlphaZero+shogi+AI 
> build · hy4-flash-free
Je trouve plusieurs moteurs minishogi publics (MCTS, Alpha-Beta, AlphaZero). Je vérifie le forum/discussion CodinGame et ces dépôts pour des termes d'évaluation concrets.
% WebFetch https://html.duckduckgo.com/html/?q=site%3Acodingame.com%2Fforum+minishogi 
% WebFetch https://www.codingame.com/multiplayer/bot-programming/minishogi/discuss 
✗ WebFetch https://api.github.com/search/code?q=minishogi+codingame failed
Error: StatusCode: non 2xx status code (401 GET https://api.github.com/search/code?q=minishogi+codingame)
% WebFetch https://api.github.com/repos/koduma/CodinGame/contents 
> build · hy4-flash-free
Le code search GitHub exige un token (401) et DuckDuckGo commence à captcha. J'explore directement les dépôts publics et le moteur de jeu.
✗ WebFetch https://raw.githubusercontent.com/yuchen0515/minishogi/main/README.md failed
Error: StatusCode: non 2xx status code (404 GET https://raw.githubusercontent.com/yuchen0515/minishogi/main/README.md)
% WebFetch https://raw.githubusercontent.com/jdermont/MiniShogi/main/src/main/java/com/codingame/game/GameState.java 
% WebFetch https://raw.githubusercontent.com/genga6/minishogi-rs/master/README.md 
% WebFetch https://raw.githubusercontent.com/jdermont/MiniShogi/main/src/test/java/Player2.java 
% WebFetch https://api.github.com/repos/jdermont/MiniShogi/commits?path=config/Boss.java&per_page=20 
> build · hy4-flash-free
J'ai les règles complètes et un premier jeu de poids d'évaluation. Je cherche maintenant les autres moteurs/publications et le forum CodinGame via Bing.
✗ WebFetch https://www.codingame.com/forum/search.json?q=minishogi failed
Error: StatusCode: non 2xx status code (404 GET https://www.codingame.com/forum/search.json?q=minishogi)
% WebFetch https://www.bing.com/search?q=%22minishogi%22+codingame+forum+bot+strategy 
% WebFetch https://raw.githubusercontent.com/nyashiki/erweitern_55/master/README.md 
% WebFetch https://api.github.com/repos/yuchen0515/minishogi 
% WebFetch https://raw.githubusercontent.com/Cheng0303/minizero-minishogi/main/README.md 
> build · hy4-flash-free
✗ WebFetch https://grep.app/api/search?q=minishogi failed
Error: StatusCode: non 2xx status code (429 GET https://grep.app/api/search?q=minishogi)
% WebFetch https://api.github.com/repos/yuchen0515/minishogi/git/trees/master?recursive=1 
% WebFetch https://github.com/search?q=minishogi+codingame&type=code 
% WebFetch https://www.google.com/search?q=%22minishogi%22+site%3Acodingame.com%2Fforum 
> build · hy4-flash-free
% WebFetch https://raw.githubusercontent.com/yuchen0515/minishogi/master/docs/agents.md 
% WebFetch https://raw.githubusercontent.com/yuchen0515/minishogi/master/README.md 
% WebFetch https://raw.githubusercontent.com/yuchen0515/minishogi/master/engine/agents/eval.cpp 
% WebFetch https://raw.githubusercontent.com/yuchen0515/minishogi/master/docs/verification.md 
% WebFetch https://forum.codingame.com/search?q=minishogi 
> build · hy4-flash-free
Le dépôt Pioneer donne des mesures Elo très utiles. Je récupère les poids exacts et tente d'accéder au forum CodinGame par son API.
✗ WebFetch https://www.codingame.com/forum/search.json?q=minishogi failed
Error: StatusCode: non 2xx status code (404 GET https://www.codingame.com/forum/search.json?q=minishogi)
% WebFetch https://forum.codingame.com/search.json?q=minishogi 
% WebFetch https://raw.githubusercontent.com/yuchen0515/minishogi/master/engine/agents/eval.h 
% WebFetch https://raw.githubusercontent.com/yuchen0515/minishogi/master/engine/core/types.h 
% WebFetch https://raw.githubusercontent.com/yuchen0515/minishogi/master/engine/agents/alphabeta_agent.h 
> build · hy4-flash-free
Le forum n'a qu'un seul fil « Minishogi ». Je le lis et vérifie s'il existe d'autres discussions utiles.
% WebFetch https://forum.codingame.com/t/203765.json 
% WebFetch https://forum.codingame.com/search.json?q=shogi 
% WebFetch https://forum.codingame.com/search.json?q=jdermont 
> build · hy4-flash-free
Le forum contient une correction importante (bug `toKey()` sur les répétitions). Je vérifie l'historique du referee et interroge Semantic Scholar / arXiv.
% WebFetch https://raw.githubusercontent.com/jdermont/MiniShogi/main/src/main/java/com/codingame/game/Player.java 
% WebFetch https://api.github.com/repos/jdermont/MiniShogi/commits?per_page=30 
% WebFetch http://export.arxiv.org/api/query?search_query=all:minishogi&start=0&max_results=10 
✗ WebFetch https://api.semanticscholar.org/graph/v1/paper/search?query=minishogi&limit=10&fields=title,year,authors,url,abstract failed
Error: StatusCode: non 2xx status code (429 GET https://api.semanticscholar.org/graph/v1/paper/search?query=minishogi&limit=10&fields=title,year,authors,url,abstract)
> build · hy4-flash-free
J'ai les 3 articles arXiv pertinents. Je récupère leurs résumés détaillés, puis je finalise.
% WebFetch https://arxiv.org/abs/2303.09449 
% WebFetch https://arxiv.org/abs/2206.03965 
% WebFetch https://arxiv.org/abs/2409.00129 
% WebFetch https://home.hccnet.nl/h.g.muller/shokidoki.html 
> build · hy4-flash-free
# Deep search — CodinGame multi « Minishogi »

## 0. Périmètre et limites de la recherche

- `websearch` a échoué (HTTP 402) ; j'ai donc utilisé `webfetch` + API GitHub + API de recherche du forum CodinGame + API arXiv.
- Le « referee officiel » n'est **pas** dans `CodinGame/minishogi` (404) ni dans `eulerscheZahl/RefereeCollection` (aucun dossier `minishogi`). C'est le dépôt du créateur du jeu, **explicitement lié comme « referee » depuis l'énoncé officiel** : `github.com/jdermont/MiniShogi` (créateur = `jdermont` sur GitHub = `jacek` sur le forum CodinGame).
- Aucun post-mortem de joueur de haut rang n'est public : la recherche du forum ne renvoie qu'**un seul fil**, à 2 messages (annonce + correction du auteur). Les stratégies des meilleurs joueurs CodinGame sont donc **[NON VÉRIFIÉ]**. Je remplace cette partie par les moteurs minishogi publics les plus documentés, en signalant clairement qu'ils ne sont pas des bots CodinGame.

---

## 1. Faits du referee (vérifiés dans le code)

### 1.1 Identification et constantes

| Fait | Valeur | URL exacte |
|---|---|---|
| Énoncé officiel (lien « Click here for the referee ») | plateau 5×5, 6 pièces/joueur, 300 rounds max, 1000 ms au 1er tour, 100 ms par tour | `https://raw.githubusercontent.com/jdermont/MiniShogi/main/config/statement_en.html` |
| Referee | `gameManager.setFrameDuration(1500)`, `setFirstTurnMaxTime(1000)`, `setTurnMaxTime(100)`, `setMaxTurns(300)` | `https://raw.githubusercontent.com/jdermont/MiniShogi/main/src/main/java/com/codingame/game/Referee.java` |
| Config challenge | `title=Minishogi`, `min_players=2`, `max_players=2`, `type=multi` | `https://raw.githubusercontent.com/jdermont/MiniShogi/main/config/config.ini` |
| Taille/protocole | `boardSize` toujours 5 ; 1 entrée de coup par tour ; sortie `random` acceptée ; message libre après un espace | `https://raw.githubusercontent.com/jdermont/MiniShogi/main/config/stub.txt` |
| Flag critique | `ALLOW_PAWN_DROP_CHECKMATE = false` (interdit l'uchifuzume, coup de pion droppé qui mate) | `https://raw.githubusercontent.com/jdermont/MiniShogi/main/src/main/java/com/codingame/game/GameState.java` |

**Pièces (code)** : `KING, ROOK, PROMOTED_ROOK, BISHOP, PROMOTED_BISHOP, GOLD, SILVER, PROMOTED_SILVER, PAWN, PROMOTED_PAWN` ; +R/+B ont les mouvements Roi+Toi/Cheval, +S/+P bougent comme Or. Source : `https://raw.githubusercontent.com/jdermont/MiniShogi/main/src/main/java/com/codingame/game/Piece.java`.

### 1.2 Ordre de résolution (vérifié)

Dans `Referee.gameTurn` (`.../Referee.java`) :

1. `t = turn-1` ; joueur = `t % 2`. **Le joueur 0 (Noir/« red ») commence toujours.**
2. `availableMoves = game.getAvailableMoves()`.
3. **`Collections.shuffle(availableMoves, gameManager.getRandom())`** → l'ordre des coups envoyés est **aléatoire et seedé**. Ne jamais dépendre de l'ordre d'entrée.
4. Envoi : plateau (5 lignes), main J1, main J2, dernier coup adverse (`none` au 1er tour), `actionCount`, puis les coups.
5. Parse de la sortie : comparaison **insensible à la casse** à la liste légale ; `random` = reshuffle puis `availableMoves.get(0)` ; texte après espace = message ; sinon `InvalidMove` → défaite.
6. `game.makeMove(...)`, mise à jour graphique, puis test de fin.

Dans `Game.makeMove` (`.../Game.java`) :
- **Déplacement** : la pièce capturée est **dé-promue** (`demote()`) et ajoutée à la main du capturant.
- **Drop** : la pièce est posée **non promue** (`board = move.piece`), même sur la dernière rangée.
- `currentPlayer ^= 1`, `rounds++` → `rounds` compte les **demi-coups** (300 = 150 tours par joueur), malgré le mot « rounds » de l'énoncé.

Génération des coups (`.../GameState.java`) :
- Promotion possible si **origine ou destination** est sur la dernière rangée (`r == 4 || row == 4` pour Noir en index interne, `r == 0 || row == 0` pour Blanc) ; **Pion forcé à la promotion** en dernière rangée.
- Drops : interdits sur dernière rangée pour le Pion ; **nifu** interdit (un seul pion non promu par colonne) ; **uchifuzume** interdit (le code teste spécifiquement un drop de pion juste devant le roi adverse suivi de `!temp.isOver()`).
- Filtre final : tous les coups laissant son propre roi en échec sont retirés ; **les drops en cas d'échec sont autorisés** (`if (move.colFrom == -1 && !wasInCheck) continue;`).

Notation (`.../Move.java`) : `colFrom rowFrom colTo rowTo` avec `+`, ou `Piece*colTo rowTo`, chiffres 1..5 -> `5 - indexInterne`. Ex. `4534`, `1425+`, `G*33`, `p*24`.

### 1.3 Conditions de fin et départages (vérifiés)

`Game.isGameOver()` (`.../Game.java`) teste **dans cet ordre** :
1. `rounds >= 300` → fin ( score 0/0, **nulle** car `getWinner()` renvoie `NONE`) ;
2. `repeatedFor3thTime()` (clé = **plateau + camp au trait + mains**, sans compteur de coups — `GameState.toKey()`) ;
3. `generateMoves().isEmpty()`.

`getWinner()` :
- plus aucun coup → **le camp au trait perd** (échec **et** pat = défaite, contrairement aux échecs) ;
- triple répétition : **si le camp au trait est en échec, c'est lui qui gagne** (donc le camp qui donne échec perd) ; **sinon le joueur 0 (Noir) perd** — c'est le départage asymétrique du jeu ;
- sinon `NONE` → nulle, scores `0/0`.

`Referee.setScores` : `+1 / -1` ou `0 / 0`. Source : `https://raw.githubusercontent.com/jdermont/MiniShogi/main/src/main/java/com/codingame/game/Referee.java`.

Correctif historique vérifié : le 2024‑05‑05 l'auteur a corrigé `toKey()` (« fixed toKey() typo regarding repetition states ») et le 2024‑05‑06 « fix random moves ». Sources : `https://api.github.com/repos/jdermont/MiniShogi/commits?per_page=30` et `https://forum.codingame.com/t/203765.json`. Le dépôtGitHub est donc à jour par rapport aux correctifs annoncés.

### 1.4 Perft de référence (vérifié)

`https://raw.githubusercontent.com/jdermont/MiniShogi/main/src/test/java/Perft.java` : startpos = **14, 181, 2 512, 35 401, 533 203, 8 276 188** coups aux profondeurs 1..6. Au niveau 7 : `132 680 617` (referee, uchifuzume interdit) vs `132 680 698` (Fairy‑Stockfish, qui l'autorise) — écart de exactement 81 drops de pion matants.

### 1.5 Le boss de la ligue — point important

`https://raw.githubusercontent.com/jdermont/MiniShogi/main/config/Boss.java` : le fichier se contente de lire l'entrée puis `System.out.println("random");`. Il est présent depuis le **commit initial** et n'a jamais été modifié (`https://api.github.com/repos/jdermont/MiniShogi/commits?path=config/Boss.java&per_page=20`). Le boss jouerait donc un **coup légal uniformément aléatoire (seedé par `gameManager`)**. **[NON VÉRIFIÉ]** : que ce fichier soit exactement l'artefact déployé sur les serveurs CodinGame (je n'ai pas pu lire la config du challenge, les endpoints `Challenge/...` exigent POST/auth).

---

## 2. Stratégies publiques (URL + idée + pourquoi ça marche)

> **Aucun post-mortem de meilleur joueur CodinGame n'a été trouvé.** sources : `https://forum.codingame.com/search.json?q=minishogi` (1 seul topic), `https://forum.codingame.com/t/203765.json` (2 messages, aucun contenu stratégique), `https://forum.codingame.com/search.json?q=shogi`, `https://forum.codingame.com/search.json?q=jdermont`. Les classements/stratégies des top joueurs sont **[NON VÉRIFIÉ]**.

### 2.1 Pioneer — moteur minishogi vérifié, le meilleur substitut documenté

URL : `https://raw.githubusercontent.com/yuchen0515/minishogi/master/README.md` et `https://raw.githubusercontent.com/yuchen0515/minishogi/master/docs/agents.md`.

- **Idée** : négamax alpha‑beta itératif, profondeur 4 par défaut, **MVV‑LVA**, **quiescence**, table de transposition, killers/history, null‑move + LMR ; MCTS UCT aussi implémenté ; solveur de mat **df‑pn**.
- **Pourquoi ça marche / mesures** :
  - ~**370 Elo par ply** (`d3` bat `d2` 89–11 ; `d4` bat `d3` 90–10) ;
  - **quiescence ≈ +263 à +363 Elo** à profondeur égale, pour 1/5 du coût d'un ply supplémentaire ;
  - **alpha‑beta écrase MCTS à playouts aléatoires** : 99–100 vs `mcts-s400`, 60–0 même avec 4× plus de playouts. Explication donnée : avec les drops, une pièce capturée réapparaît n'importe où → les playouts aléatoires sont du bruit ;
  - **piège majeur** : PST + king‑safety maison = **+228 Elo en auto‑jeu mais −230 Elo contre Fairy‑Stockfish** (`alphabeta-d4+pst+ks` 14–136 vs `fsf-d6`, contre 42–108 sans eux). Les termes sont désactivés par défaut ;
  - **profondeurs impaires trompeuses** : `d3` ne bat pas `d2`, `d4` bat `d2` 93,8 % → privilégier des profondeurs **paires** ;
  - **avantage du premier joueur** : Noir gagne ~58 % des parties décisives même entre bots aléatoires, malgré la règle de répétition qui le pénalise.

### 2.2 Pioneer — termes et poids d'évaluation concrets

URLs : `https://raw.githubusercontent.com/yuchen0515/minishogi/master/engine/agents/eval.h`, `https://raw.githubusercontent.com/yuchen0515/minishogi/master/engine/agents/eval.cpp`.

- Valeurs (centipions) : **Pion 100, Argent 340, Fou 500, Tour 570, Or 380, Roi 0, +P 390, +S 390, Cheval(+B) 640, Dragon(+R) 720**.
- **Pièce en main = valeur × 110 %** (`HAND_BONUS_PERCENT = 10`) : « une pièce en main peut être droppée n'importe où ».
- Matériel pur `weights.material = 100`, `placement = 0`, `king_safety = 0` (mesuré, cf. 2.1).
- Tables PST (si activées) : pion `_34,38,40,38,34_` sur la rangée de promotion ; générale `_10,16,18,..._` ; slideur centre 20 ; roi `_−30 … +16_` vers le bord. King safety : `−18 × cases_attaquées + 6 × cases_défendues − 10 × pièces_en_main_adverse`.
- `MATE_SCORE = 30000`, `MAX_EVAL = 10000` (`eval.h`).
- **Ordre des valeurs corrigé** : l'ancienne version avait Tour 54 < Or 74, « suffisant pour faire mal jouer un moteur sain ». Source : commentaire dans `eval.h`.
- Vérification croisée : `https://raw.githubusercontent.com/yuchen0515/minishogi/master/docs/verification.md` (perft identique à Fairy‑Stockfish sauf uchifuzume).

### 2.3 minishogi-rs — deuxième jeu de poids + MCTS

URL : `https://raw.githubusercontent.com/genga6/minishogi-rs/master/README.md`.

- Alpha‑beta profondeur 4 + MCTS UCB1 (20 000 itérations, playouts de 10 coups, sigmoid, xorshift64, génération allégée sans uchifuzume dans les playouts).
- Évaluation : **Pion 100, Argent 400, Or 500, Fou 600, Tour 700**, différentiel plateau + main.
- **Pourquoi ça marche** : convention cohérente « minishogi = pièces mineures相对 plus précieuses, la tour ne domine pas autant que sur 9×9 » (cf. aussi discussion `https://www.reddit.com/r/shogi/comments/iak1bc/mini_shogi_strategies/` vue uniquement comme extrait de recherche **[NON VÉRIFIÉ]**). Les deux sources divergent sur l'ordre Or/Argent/Fou/Tour → à régler par A/B, pas par intuition.

### 2.4 Shokidoki — moteur de référence (médaille d'or mini‑shogi 2013)

URL : `https://home.hccnet.nl/h.g.muller/shokidoki.html`.

- **Idée** : alpha‑beta avec **hash table, null‑move pruning, late‑move reductions, extension des échecs**, **réduction agressive des drops** (un drop n'est recherché que s'il fait échouer une réponse null‑move), et **détection statique des mats en 1 par drop dans la quiescence**, évaluation de sécurité du roi renforcée en 2015.
- **Pourquoi ça marche** : citation directe — « the search algorithm I used was much more suitable for mini‑Shogi than for regular Shogi » ; 2ᵉ place en 2010, **1ʳᵉ place en 2013**. Pioneer cite explicitement Shokidoki comme « strongest known minishogi program » et motive son choix de l'alpha‑beta plutôt que MCTS (`docs/agents.md`).

### 2.5 AlphaZero / MCTS académique

- `https://raw.githubusercontent.com/nyashiki/erweitern_55/master/README.md` — implémentation **AlphaZero sur Minishogi** (Python + `minishogilib`, moteur USI).
- `https://raw.githubusercontent.com/nyashiki/minishogilib/master/README.md` **[NON VÉRIFIÉ — non lu]** : bibliothèque Rust de règles minishogi citée par le dépôt ci‑dessus.
- `https://raw.githubusercontent.com/Cheng0303/minizero-minishogi/main/README.md` — fork de MiniZero (AlphaZero, MuZero, Gumbel) ; **le README ne documente pas réellement minishogi**, seulement Go/Othello/Atari → intérêt limité.
- Articles arXiv (résumés lus) :
  - `https://arxiv.org/abs/2409.00129` — nombre de positions atteignables ≈ **2,38 × 10¹⁸** → résolution forte hors de portée, l'heuristique reste obligatoire.
  - `https://arxiv.org/abs/2206.03965` — **PN‑MCTS** (proof/disproof numbers intégrés à UCT) bat le MCTS de base sur MiniShogi (entre autres), taux de victoire jusqu'à 94,0 %.
  - `https://arxiv.org/abs/2303.09449` — version étendue IEEE ToG 2024 : PN‑MCTS bat UCT dans **tous** les domaines testés dont MiniShogi.
  - `https://arxiv.org/abs/1712.01815` (AlphaZero) — **cité via extrait de recherche uniquement, [NON VÉRIFIÉ]** (je ne l'ai pas ouvert).

---

## 3. Recommandation concrète et testable pour monter au classement

### 3.1 D'abord battre le boss (court terme)

1. Vérifier en match le comportement du boss : s'il sort bien `random`, un bot **glouton matériel + mat‑en‑1 par drop** doit gagner ~100 % des parties. Référence mesurée : `greedy` bat `random` **196–4 / 200** (Elo +676). Source : `docs/agents.md`.
2. Implémenter le **mat‑en‑1 par drop** : pour chaque drop légal, tester si l'adversaire n'a plus aucun coup (`isOver()`) ; c'est exactement le test que le referee utilise pour l'uchifuzume, et c'est la seule « tactique » que Shokidoki traite en quiescence.
3. Ne jamais utilisez l'ordre des `actionCount` coups (shuffle seedé) ; sortir la chaîne exacte reçue (insensible à la casse).

### 3.2 Ensuite monter au classement (objectif principal)

**Recette** : alpha‑beta négamax **profondeur paire 4** (5–6 si le budget le permet), iterative deepening avec deadline ~85–90 ms (100 ms durs, 1 000 ms au 1er tour), **quiescence** (captures + checks de drop), **MVV‑LVA + killers/history**, **TT Zobrist**, null‑move + LMR.

**Évaluation initiale (à partir de sources lues)** :
```
P=100, S=340, G=380, B=500, R=570, +P/+S=390, +B=640, +R=720, K=0
pièce en main = 1,10 × valeur
MATE = 30000
PST et king-safety = 0 (DÉSACTIVÉS au départ)
```
Ne pas ajouter de PST/king‑safety « à la main » avant d'avoir un adversaire externe de référence : mesure vérifiée **+228 Elo en self‑play mais −230 Elo contre Fairy‑Stockfish**.

**Détails referee à coder impérativement** (sinon pertes incompréhensibles) : dé-promotion à la capture, nifu, uchifuzume, drop non promu même en zone de promotion, pat = défaite, triple répétition = défaite de Noir sauf si le camp au trait est en échec (alors c'est lui qui gagne), nulle à 300 demi‑coups.

### 3.3 Protocole de test A/B (testable, reproductible)

1. Matchs **alternés Noir/Blanc**, **100 parties minimum**, seed fixe, même pool d'adversaires (v1 gloutonne, random, bots locaux).
2. Métrique primaire : **% de victoires avec intervalle de Wilson** ; secondaires : nombre de mats manqués en 1, nombre de nulles à 300 coups, temps max par coup.
3. Paliers à valider dans l'ordre : (a) ajouter quiescence → viser **+250 Elo** ; (b) passer d2 → d4 → viser **+370 Elo/ply** ; (c) ajouter MVV‑LVA/TT → vérifier réduction des nœuds ≥ 2× ; (d) seulement ensuite tester PST **uniquement contre un adversaire externe**, pas en self‑play.
4. Règle de décision : ne garder un changement que si l'intervalle de confiance ne chevauche pas la baseline **et** si le temps par coup reste < 100 ms (pire cas < 95 ms).
5. Test de non‑régression sur les perft : `14 / 181 / 2512 / 35401 / 533203 / 8276188` (uchifuzume interdit) — tout écart = bug de génération.

**Pourquoi ça doit marcher** : les trois leviers les plus rentables mesurés sont la profondeur (~370 Elo/ply), la quiescence (~+300 Elo pour 1/5 du coût), et le rejet des évaluations positionnelles non validées contre une référence externe. Le MCTS à playouts purs est à éviter (99–1 contre alpha‑beta shallow) ; si vous tenez à MCTS, la littérature arXiv montre que **PN‑MCTS** est le variant qui bat UCT sur MiniShogi.

---

## 4. Liste de toutes les URL lues

**Referee / énoncé (officiel)**
1. `https://www.codingame.com/multiplayer/bot-programming/minishogi` (JS, contenu non rendu)
2. `https://raw.githubusercontent.com/jdermont/MiniShogi/main/config/statement_en.html`
3. `https://raw.githubusercontent.com/jdermont/MiniShogi/main/README.md`
4. `https://raw.githubusercontent.com/jdermont/MiniShogi/main/config/config.ini`
5. `https://raw.githubusercontent.com/jdermont/MiniShogi/main/config/stub.txt`
6. `https://raw.githubusercontent.com/jdermont/MiniShogi/main/config/Boss.java`
7. `https://raw.githubusercontent.com/jdermont/MiniShogi/main/src/main/java/com/codingame/game/Referee.java`
8. `https://raw.githubusercontent.com/jdermont/MiniShogi/main/src/main/java/com/codingame/game/Game.java`
9. `https://raw.githubusercontent.com/jdermont/MiniShogi/main/src/main/java/com/codingame/game/GameState.java`
10. `https://raw.githubusercontent.com/jdermont/MiniShogi/main/src/main/java/com/codingame/game/Piece.java`
11. `https://raw.githubusercontent.com/jdermont/MiniShogi/main/src/main/java/com/codingame/game/Move.java`
12. `https://raw.githubusercontent.com/jdermont/MiniShogi/main/src/main/java/com/codingame/game/Player.java`
13. `https://raw.githubusercontent.com/jdermont/MiniShogi/main/src/test/java/Perft.java`
14. `https://raw.githubusercontent.com/jdermont/MiniShogi/main/src/test/java/Player1.java`
15. `https://raw.githubusercontent.com/jdermont/MiniShogi/main/src/test/java/Player2.java`
16. `https://api.github.com/repos/jdermont/MiniShogi`
17. `https://api.github.com/repos/jdermont/MiniShogi/contents`
18. `https://api.github.com/repos/jdermont/MiniShogi/contents/config`
19. `https://api.github.com/repos/jdermont/MiniShogi/git/trees/3a7b68e5de17567df361363fad8389eb8fb4bca2?recursive=1`
20. `https://api.github.com/repos/jdermont/MiniShogi/commits?per_page=30`
21. `https://api.github.com/repos/jdermont/MiniShogi/commits?path=config/Boss.java&per_page=20`
22. `https://api.github.com/repos/CodinGame/minishogi` (404)
23. `https://api.github.com/repos/eulerscheZahl/RefereeCollection/contents` (pas de dossier minishogi)
24. `https://api.github.com/search/repositories?q=minishogi+codingame&per_page=30`
25. `https://api.github.com/search/repositories?q=minishogi&per_page=30`

**Forum / discussion**
26. `https://forum.codingame.com/search.json?q=minishogi`
27. `https://forum.codingame.com/t/203765.json`
28. `https://forum.codingame.com/search.json?q=shogi`
29. `https://forum.codingame.com/search.json?q=jdermont`
30. `https://www.codingame.com/multiplayer/bot-programming/minishogi/discuss` (JS, non rendu)
31. `https://www.codingame.com/services/Challenge/getChallengeStatement` (405)

**Bots / moteurs publics**
32. `https://raw.githubusercontent.com/yuchen0515/minishogi/master/README.md`
33. `https://raw.githubusercontent.com/yuchen0515/minishogi/master/docs/agents.md`
34. `https://raw.githubusercontent.com/yuchen0515/minishogi/master/docs/verification.md`
35. `https://raw.githubusercontent.com/yuchen0515/minishogi/master/engine/agents/eval.cpp`
36. `https://raw.githubusercontent.com/yuchen0515/minishogi/master/engine/agents/eval.h`
37. `https://raw.githubusercontent.com/yuchen0515/minishogi/master/engine/agents/alphabeta_agent.h`
38. `https://raw.githubusercontent.com/yuchen0515/minishogi/master/engine/core/types.h`
39. `https://api.github.com/repos/yuchen0515/minishogi`
40. `https://api.github.com/repos/yuchen0515/minishogi/git/trees/master?recursive=1`
41. `https://raw.githubusercontent.com/genga6/minishogi-rs/master/README.md`
42. `https://raw.githubusercontent.com/nyashiki/erweitern_55/master/README.md`
43. `https://raw.githubusercontent.com/Cheng0303/minizero-minishogi/main/README.md`
44. `https://home.hccnet.nl/h.g.muller/shokidoki.html`
45. `https://api.github.com/repos/koduma/CodinGame/contents` (pas de bot minishogi)

**Articles**
46. `http://export.arxiv.org/api/query?search_query=all:minishogi&start=0&max_results=10`
47. `https://arxiv.org/abs/2409.00129`
48. `https://arxiv.org/abs/2206.03965`
49. `https://arxiv.org/abs/2303.09449`
50. `https://api.semanticscholar.org/graph/v1/paper/search?query=minishogi&limit=10&fields=title,year,authors,url,abstract` (429)

**Moteurs de recherche (tentatives)**
51. `https://html.duckduckgo.com/html/?q=codingame+minishogi+post+mortem+forum`
52. `https://html.duckduckgo.com/html/?q=site%3Agithub.com+minishogi+codingame+bot+alpha-beta`
53. `https://html.duckduckgo.com/html/?q=%22minishogi%22+codingame+forum+strategy+bot`
54. `https://html.duckduckgo.com/html/?q=arxiv+minishogi+AlphaZero+shogi+AI`
55. `https://html.duckduckgo.com/html/?q=site%3Acodingame.com%2Fforum+minishogi` (captcha)
56. `https://www.bing.com/search?q=%22minishogi%22+codingame+forum+bot+strategy` (0 résultat)
57. `https://www.google.com/search?q=%22minishogi%22+site%3Acodingame.com%2Fforum` (pas de contenu)
58. `https://github.com/search?q=minishogi+codingame&type=code` (login requis)
59. `https://grep.app/api/search?q=minishogi` (429)
60. `https://api.github.com/search/code?q=minishogi+codingame` (401)

Fichiers locaux lus pour contexte uniquement (pas des sources web) : `bot-programming/games.json`, `bot-programming/games/minishogi/README.md`, `hackathon-2026-12-03/challenges/minishogi/research.md`.

---

## 5. Récapitulatif [NON VÉRIFIÉ]

- Stratégies, classement et identité des meilleurs joueurs CodinGame sur minishogi — **aucune source publique trouvée**.
- Le boss réellement déployé sur les serveurs CodinGame (le dépôt dit `random`, mais l'artefact de production n'a pas pu être lu) — **à confirmer par un match contrôlé**.
- Éventuelles différences entre le referee GitHub (dernier commit 2024‑08‑13, README) et la version de production après cette date.
- `minishogilib` (`nyashiki`), AlphaZero arXiv 1712.01815, Reddit r/shogi et lishogi.org : vus uniquement comme extraits/titres de recherche, **non ouverts**.
- Toute recommandation de poids autre que ceux cités avec URL ci‑dessus.
