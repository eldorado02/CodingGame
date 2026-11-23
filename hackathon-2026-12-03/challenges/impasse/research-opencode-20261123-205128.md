<!-- opencode model: hy4-flash-free, 2026-11-23T20:55:54+03:00 -->

> build · hy4-flash-free
Je vais effectuer une recherche approfondie sur le jeu CodinGame « impasse ». Je commence par plusieurs recherches en parallèle pour identifier le referee officiel, les bots publics et les post-mortems.
✗ Web Search via OpenCode "CodinGame Impasse post-mortem strategy alpha-beta bot" failed
Error: Web search request failed (HTTP 402)
✗ Web Search via OpenCode "CodinGame impasse multiplayer game rules bot programming" failed
Error: Web search request failed (HTTP 402)
✗ Web Search via OpenCode "CodinGame "Impasse" game referee github eulerscheZahl RefereeCollection" failed
Error: Web search request failed (HTTP 402)
% WebFetch https://api.github.com/search/repositories?q=impasse+codingame 
> build · hy4-flash-free
La recherche web classique est indisponible (HTTP 402), je vais donc m'appuyer sur l'API GitHub, raw.githubusercontent et les URLs connues. Deux dépôt GitHub sont déjà identifiés. Explorons-les ainsi que le dépôt de referees d'eulerscheZahl.
% WebFetch https://api.github.com/repos/acatai/CG_Impasse/git/trees/main?recursive=1 
✗ WebFetch https://api.github.com/search/code?q=impasse+repo:eulerscheZahl/RefereeCollection failed
Error: StatusCode: non 2xx status code (401 GET https://api.github.com/search/code?q=impasse+repo:eulerscheZahl/RefereeCollection)
% WebFetch https://api.github.com/repos/eulerscheZahl/RefereeCollection/contents/ 
% WebFetch https://api.github.com/repos/penhauer/Impasse-for-CodinGame/git/trees/main?recursive=1 
> build · hy4-flash-free
Le dépôt `acatai/CG_Impasse` semble être l'implémentation officielle (auteur du jeu). Je vais lire ses fichiers clés (referee, constantes, état de jeu, boss) ainsi que le bot de penhauer (minimax + MCTS).
% WebFetch https://raw.githubusercontent.com/acatai/CG_Impasse/main/src/main/java/com/codingame/game/Referee.java 
% WebFetch https://raw.githubusercontent.com/acatai/CG_Impasse/main/README.md 
% WebFetch https://raw.githubusercontent.com/acatai/CG_Impasse/main/src/main/java/com/codingame/game/engine/Constants.java 
% WebFetch https://raw.githubusercontent.com/acatai/CG_Impasse/main/src/main/java/com/codingame/game/engine/GameState.java 
% WebFetch https://raw.githubusercontent.com/acatai/CG_Impasse/main/config/statement_en.html 
% WebFetch https://raw.githubusercontent.com/acatai/CG_Impasse/main/config/config.ini 
> build · hy4-flash-free
Le referee officiel est confirmé (`acatai/CG_Impasse`, auteur @aCat). Je vais maintenant lire le boss, le format d'action, puis le bot de penhauer.
% WebFetch https://raw.githubusercontent.com/acatai/CG_Impasse/main/src/main/java/com/codingame/game/engine/Action.java 
% WebFetch https://raw.githubusercontent.com/acatai/CG_Impasse/main/config/stub.txt 
% WebFetch https://raw.githubusercontent.com/acatai/CG_Impasse/main/config/Boss.java 
% WebFetch https://raw.githubusercontent.com/penhauer/Impasse-for-CodinGame/main/state.cpp 
% WebFetch https://raw.githubusercontent.com/penhauer/Impasse-for-CodinGame/main/README.md 
% WebFetch https://raw.githubusercontent.com/penhauer/Impasse-for-CodinGame/main/minimax_player.cpp 
> build · hy4-flash-free
Le boss est très informatif (recherche à 4 coups, `GAMMA=0.82`, évaluation pondérée). Je cherche maintenant d'autres bots, post-mortems et articles académiques.
% WebFetch https://www.codingame.com/contribute/view/343356f1616c1e7c6a6619ae239b06f2a7427 
✗ WebFetch https://api.semanticscholar.org/graph/v1/paper/search?query=Impasse%20board%20game%20artificial%20intelligence&fields=title,abstract,url,year,authors,externalIds&limit=20 failed
Error: StatusCode: non 2xx status code (429 GET https://api.semanticscholar.org/graph/v1/paper/search?query=Impasse%20board%20game%20artificial%20intelligence&fields=title,abstract,url,year,authors,externalIds&limit=20)
% WebFetch https://www.codingame.com/forum/search?query=impasse 
✗ WebFetch https://grep.app/api/search?q=impasse%20codingame failed
Error: StatusCode: non 2xx status code (429 GET https://grep.app/api/search?q=impasse%20codingame)
% WebFetch https://api.github.com/search/repositories?q=impasse+game+ai+in:name,description,readme&per_page=30 
✗ WebFetch http://export.arxiv.org/api/query?search_query=all:impasse+AND+all:game&max_results=20 failed
Error: Transport error (GET http://export.arxiv.org/api/query?search_query=all:impasse+AND+all:game&max_results=20)
> build · hy4-flash-free
Le repo `Puma1208/Impasse` (alpha-beta) est un autre candidat. Je l'explore, et je contourne les limitations des API (arXiv en HTTPS, recherche DDG via webfetch, API CodinGame).
✗ WebFetch https://www.codingame.com/services/Contribution/getContributionData/343356f1616c1e7c6a6619ae239b06f2a7427 failed
Error: StatusCode: non 2xx status code (405 GET https://www.codingame.com/services/Contribution/getContributionData/343356f1616c1e7c6a6619ae239b06f2a7427)
% WebFetch https://raw.githubusercontent.com/Puma1208/Impasse/main/README.md 
✗ WebFetch https://raw.githubusercontent.com/acatai/CG_Impasse/main/GAME_RULES.pdf failed
Error: Unsupported fetched file content type: application/octet-stream
% WebFetch https://export.arxiv.org/api/query?search_query=all:%22Impasse%22+AND+all:%22game%22&max_results=20 
% WebFetch https://api.github.com/repos/Puma1208/Impasse/git/trees/main?recursive=1 
% WebFetch https://html.duckduckgo.com/html/?q=CodinGame+Impasse+post+mortem+bot 
> build · hy4-flash-free
DuckDuckGo fonctionne via webfetch. Je vais chercher les discussions/post-mortems CodinGame et lire les évaluations des bots publics.
% WebFetch https://raw.githubusercontent.com/Puma1208/Impasse/main/src/GameElements/EvaluationFunction.java 
% WebFetch https://raw.githubusercontent.com/Puma1208/Impasse/main/src/AlphaBeta/AlphaBeta.java 
% WebFetch https://raw.githubusercontent.com/penhauer/Impasse-for-CodinGame/main/mcts_player.h 
% WebFetch https://raw.githubusercontent.com/penhauer/Impasse-for-CodinGame/main/common.h 
% WebFetch https://html.duckduckgo.com/html/?q=site%3Acodingame.com%2Fforum+impasse+bot+algorithm 
% WebFetch https://html.duckduckgo.com/html/?q=%22Impasse%22+codingame+%22post+mortem%22+OR+%22write-up%22+OR+%22alpha-beta%22 
> build · hy4-flash-free
% WebFetch https://www.codingame.com/multiplayer/bot-programming/impasse/discuss 
% WebFetch https://html.duckduckgo.com/html/?q=site%3Acodingame.com%2Fforum%2Ft%2F+%22Impasse%22 
% WebFetch https://html.duckduckgo.com/html/?q=codingame+impasse+legend+boss+strategy+bot 
% WebFetch https://html.duckduckgo.com/html/?q=github+%22impasse%22+codingame+bot+source+code 
% WebFetch https://html.duckduckgo.com/html/?q=%22impasse%22+%22Mark+Winands%22+game+AI 
✗ WebFetch https://marksteeregames.com/Impasse_rules.pdf failed
Error: Unsupported fetched file content type: application/pdf
> build · hy4-flash-free
% WebFetch https://html.duckduckgo.com/html/?q=site%3Aforum.codingame.com+impasse 
% WebFetch https://forum.codingame.com/search.json?q=impasse 
% WebFetch https://forum.codingame.com/t/community-puzzle-impasse/198658 
% WebFetch https://api.semanticscholar.org/graph/v1/paper/search?query=impasse+board+game+search&fields=title,year,url&limit=10 
> build · hy4-flash-free
% WebFetch https://forum.codingame.com/search.json?q=%22Impasse%22%20bot 
% WebFetch https://forum.codingame.com/t/community-puzzle-impasse/198658.json 
✗ WebFetch https://api.semanticscholar.org/graph/v1/paper/search?query=%22Impasse%22+Mark+Steere&fields=title,abstract,year,url,authors&limit=20 failed
Error: StatusCode: non 2xx status code (429 GET https://api.semanticscholar.org/graph/v1/paper/search?query=%22Impasse%22+Mark+Steere&fields=title,abstract,year,url,authors&limit=20)
% WebFetch https://export.arxiv.org/api/query?search_query=au:%22Kowalski%22+AND+all:%22game%22&max_results=20 
✗ WebFetch https://api.semanticscholar.org/graph/v1/paper/search?query=%22Impasse%22+game+search+Jakub+Kowalski&fields=title,abstract,year,url&limit=20 failed
Error: StatusCode: non 2xx status code (429 GET https://api.semanticscholar.org/graph/v1/paper/search?query=%22Impasse%22+game+search+Jakub+Kowalski&fields=title,abstract,year,url&limit=20)
> build · hy4-flash-free
% WebFetch https://raw.githubusercontent.com/penhauer/Impasse-for-CodinGame/main/main.cpp 
% WebFetch https://raw.githubusercontent.com/penhauer/Impasse-for-CodinGame/main/board.cpp 
% WebFetch https://export.arxiv.org/api/query?search_query=ti:%22Impasse%22&max_results=20 
% WebFetch https://api.github.com/search/repositories?q=impasse+codingame+in:name,description,readme&per_page=100 
% WebFetch https://html.duckduckgo.com/html/?q=site%3Agist.github.com+impasse+codingame 
> build · hy4-flash-free
% WebFetch https://raw.githubusercontent.com/penhauer/Impasse-for-CodinGame/main/game.cpp 
% WebFetch https://raw.githubusercontent.com/penhauer/Impasse-for-CodinGame/main/minimax_player.h 
% WebFetch https://raw.githubusercontent.com/penhauer/Impasse-for-CodinGame/main/common.cpp 
> build · hy4-flash-free
Je tente une dernière vérification via l'API publique CodinGame (commentaires de la contribution) pour confirmer le boss et chercher des post-mortems éventuels.
⚙ execute {"code":"const eps = [\n  [\"Comment/getComments\", [343356, 0, 10]],\n  [\"Contribution/getContributionComments\", [\"343356f1616c1e7c6a6619ae239b06f2a7427\", 0, 10]],\n  [\"Contribution/findById\", [\"343356f1616c1e7c6a6619ae239b06f2a7427\"]],\n  [\"Challenge/findAll\", []]\n];\nconst out = [];\nfor (const [ep, body] of eps) {\n  try {\n    const r = await fetch(\"https://www.codingame.com/services/\" + ep, {\n      method: \"POST\",\n      headers: {\"Content-Type\": \"application/json\", \"Accept\": \"application/json\"},\n      body: JSON.stringify(body)\n    });\n    const t = await r.text();\n    out.push({ep, status: r.status, sample: t.slice(0, 500)});\n  } catch (e) { out.push({ep, error: String(e)}); }\n}\nreturn out;"} 
> build · hy4-flash-free
# Deep search — jeu CodinGame « Impasse »

> Périmètre respecté : aucune modification/création de fichier de code, aucun build, aucune lecture de `~/.config/codingame`. La recherche web classique (`websearch`) a échoué (HTTP 402) ; les résultats ci-dessous proviennent de `webfetch` (DuckDuckGo HTML, API GitHub, raw.githubusercontent, forum Discourse, arXiv, Semantic Scholar). Tout ce qui n'a pas été lu directement est marqué **[NON VÉRIFIÉ]**.

---

## 1. Faits du referee (source officielle)

Source primaire : `https://github.com/acatai/CG_Impasse` — le README indique explicitement : « Repository contains the source code for the board game *Impasse* available as a multiplayer game on CodinGame », jeu créé par Mark Steere, implémenté par Jakub Kowalski (@aCat) :
`https://raw.githubusercontent.com/acatai/CG_Impasse/main/README.md`

**Le dépôt `eulerscheZahl/RefereeCollection` ne contient PAS Impasse** (listing vérifié : `https://api.github.com/repos/eulerscheZahl/RefereeCollection/contents/`). La source officielle est donc bien `acatai/CG_Impasse`.

### Constantes (`https://raw.githubusercontent.com/acatai/CG_Impasse/main/src/main/java/com/codingame/game/engine/Constants.java`)

| Constante | Valeur |
|---|---|
| `WIDTH`, `HEIGHT` | 8 × 8 |
| `TURNLIMIT` | 250 tours (coups) |
| `TIMELIMIT_INIT` | 1000 ms |
| `TIMELIMIT_TURN` | 100 ms |
| `MAX_MESSAGE_LENGTH` | 15 caractères (commentaire) |
| `UNIT_CHAR` | `.` vide, `w`/`b` singles, `W`/`B` doubles |
| `PLAYER_NAMES` | index 0 = White, index 1 = Black |

### Position initiale (`https://raw.githubusercontent.com/acatai/CG_Impasse/main/src/main/java/com/codingame/game/engine/GameState.java`, méthode `initial()`)

- White singles : `a1, d2, e1, h2` ; White doubles : `b8, c7, f8, g7`
- Black singles : `a7, d8, e7, h8` ; Black doubles : `b2, c1, f2, g1`
- 12 pions par joueur, White commence.

### Ordre de résolution (`GameState.java` + `Action.java`)

1. `generateLegalActions(player)` appelle d'abord `generateLegalBasicMoves()`. **Si et seulement si** la liste est vide, il appelle `generateLegalImpasses()`. Donc : coup de base prioritaire, impasse uniquement en secours.
2. Coups de base :
   - **single slide** : en diagonale vers l'avant (loin du propriétaire), sur un nombre quelconque de cases vides consécutives ;
   - **double slide** : en diagonale vers l'arrière ;
   - **transpose** : un single adjacent à un double, sur une rangée plus proche, reçoit la couronne du double.
3. **Bear-off automatique** : un double atteignant la rangée la plus proche perd immédiatement sa couronne (score +1), même au milieu du tour.
4. **Crown obligatoire** : un single atteignant la rangée la plus éloignée doit être couronné avec un autre single ; le joueur choisit la source **même s'il n'y a qu'un choix**.
5. Dans `applyAction()`, l'ordre d'application des effets est : coup principal → bear-off → crown. L'ordre des lignes source est `BASIC_SINGLE`, `BASIC_DOUBLE`, `BASIC_TRANSPOSE`, `IMPASSE_SINGLE`, `IMPASSE_DOUBLE`, puis `bearoff`, puis `crownfromxy`.
6. Impasse : retirer exactement un pion (single ou couronne d'un double) ; score +1 ; un crown peut suivre si un single est en rangée extrême.

Format de sortie (`https://raw.githubusercontent.com/acatai/CG_Impasse/main/src/main/java/com/codingame/game/engine/Action.java`) :
`fromxy` + `toxy` + (`crownfromxy` si présent) ; impasse = une seule coordonnée. Ex. `f8g7c5`, `b6`. `random` est accepté par le referee.

### Protocole (`Referee.java`, `statement_en.html`, `stub.txt`)

- `sendInputs()` envoie la couleur **tant que `turn < 3`**, puis 8 lignes de plateau **de y=7 vers y=0** (haut vers bas), puis `lastAction`, puis `moveCount`, puis les coups **triés lexicographiquement** (`actions.sort(Comparator.comparing(Action::toString))`).
  - L'énoncé décrit une seule ligne initiale ; le décalage éventuel avec `turn < 3` est à valider dans votre stub **[NON VÉRIFIÉ quant au comportement de la plateforme]**. Le boss et le stub ne lisent la couleur qu'une fois.
- Le commentaire après un espace est tronqué à 15 caractères.
- Erreur de parsing, action invalide ou timeout → `player.deactivate`, `setScore(-1)`, fin immédiate.

### Conditions de fin et départages (`Referee.java`)

- Victoire immédiate dès que `mainState.score[p] == 12` (테스트 après chaque coup).
- Sinon, le match s'arrête à 250 coups.
- `onEnd()` : compare les scores ; **score le plus élevé gagne, égalité = « Game is drawn »**. Le départage est donc purement le nombre de pions retirés, pas la position.

URL exactes : `https://raw.githubusercontent.com/acatai/CG_Impasse/main/src/main/java/com/codingame/game/Referee.java`, `https://raw.githubusercontent.com/acatai/CG_Impasse/main/config/statement_en.html`, `https://raw.githubusercontent.com/acatai/CG_Impasse/main/config/stub.txt`, `https://raw.githubusercontent.com/acatai/CG_Impasse/main/config/config.ini`.

*Anecdote vérifiée sur le forum : l'exemple de l'énoncé « d2f4 (move from d2 to f3) » contient une coquille, corrigée publiquement par @darkhorse64 en `f4` : `https://forum.codingame.com/t/community-puzzle-impasse/198658.json`.*

---

## 2. Stratégies des meilleurs joueurs — état réel des sources

**Aucun post-mortem de joueur Legend sur Impasse n'a été trouvé.** La recherche Discourse `https://forum.codingame.com/search.json?q=impasse` ne renvoie qu'un seul topic pertinent (198658, 3 messages : annonce, « Wrong game? ;o », coquille de l'énoncé). La requête `https://forum.codingame.com/search.json?q=%22Impasse%22%20bot` renvoie 0 résultat. Toute affirmation sur « la stratégie des meilleurs joueurs humains » est donc **[NON VÉRIFIÉ]**. Les meilleures stratégies *publiquement lisibles* sont celles du boss officiel et de deux dépôts étudiants.

### 2a. Le boss officiel — le favori measurable (`config/Boss.java`)

URL : `https://raw.githubusercontent.com/acatai/CG_Impasse/main/config/Boss.java`

Algorithme exact :

```
DEPTH = 4 ;  GAMMA = 0.82
eval(p) = 100 * (12 - singles[p] - 2*doubles[p])
          - Σ_singles distToCrown(p, xy)
          - Σ_doubles distToBearOff(p, xy)
terminal(p) → 100000
score = eval(S1) + γ·eval(S2) + γ²·eval(S3) + γ³·eval(S3')
```

- `distToCrown(White, xy) = |7 - y|`, `distToBearOff(White, xy) = |y - 0|` (inversé pour Black).
- La recherche `search(s, p, depth)` **ne alterne jamais de joueur** : elle simule 4 coups de suite du même joueur, avec actualisation γ=0.82.
- Égalité : `if (score > bestscore)` strict → le **premier maximum dans l'ordre de génération** est choisi. `Random RNG = new Random(36)` est instancié mais jamais utilisé : **le boss est déterministe**.
- Pourquoi ça marche : évaluation très alignée avec la règle (1 pion retiré = 100, 1 case de distance = 1), recherche courte mais suffisante pour toujours prendre un bear-off/impasse disponible.
- Faiblesse structurelle : **il ne modélise jamais l'adversaire** ; il ne voit donc pas qu'un slide prévu sera bloqué par vous. C'est le levier principal.
- Que ce fichier soit *exactement* le boss de la ligue Legend déployée : **[NON VÉRIFIÉ]** (c'est le boss fourni dans le dépôt officiel).

### 2b. `penhauer/Impasse-for-CodinGame` — alpha-beta + MCTS expérimental

URLs : `https://raw.githubusercontent.com/penhauer/Impasse-for-CodinGame/main/minimax_player.cpp`, `/state.cpp`, `/common.h`, `/mcts_player.h`, `/main.cpp`, `/board.cpp`, `/common.cpp`, `/game.cpp`.

- Minimax alpha-beta, `maxDepth = 3` dans `playInCodinGame()` (6 en test local), évaluation :
  `100 * out + 20 * ready + dis`, où `out` compte les pions sortis (pondéré ±1), `ready` les doubles, `dis = -distance_to_goal_row` accumulée par pion (coefficient 2 pour un double). Terminal = ±10 000 000.
- MCTS : UCB1 avec `C = 1`, playouts aléatoires, `wins/visits` inversé côté adverse ; present dans le code mais **commenté** dans `main()` → non utilisé en compétition.
- Détails notables : le bot reconstruit son état interne via `lastMove` et ne lit le plateau que pour des `assert` ; conventions de lignes inversées par rapport au referee (`getCrowningRow(WHITE)=0` contre `y=7` côté referee, mais `parseMove()` applique `ROWS-1-(chiffre-1)`). Deux疑似 bugs dans l'évaluation (`12 - 2*doubles + singles` au lieu de `- singles`, et `ready = whiteOut + blackReady` au lieu de `whiteReady`). **Aucun classement CodinGame vérifiable : [NON VÉRIFIÉ].**

### 2c. `Puma1208/Impasse` — alpha-beta académique

URLs : `https://raw.githubusercontent.com/Puma1208/Impasse/main/src/GameElements/EvaluationFunction.java`, `/src/AlphaBeta/AlphaBeta.java`, `https://raw.githubusercontent.com/Puma1208/Impasse/main/README.md`.

- Évaluation : `advantageMaterial - opponentAdvantage + ownAdvantage`, où `advantageMaterial = pions_adv - pions_moi` (double compté 2) et les deux termes d'avantage sont des sommes de `getCellsFromFurthestRow()` des singles.
- Alpha-beta fail-hard avec élagage ; projet Java avec GUI, pas de dépôt de bot CodinGame final. **Rang Legend : [NON VÉRIFIÉ].**

### 2d. Articles arXiv / Semantic Scholar

Aucun article traitant **spécifiquement** du jeu Impasse n'a été trouvé (recherches `all:"Impasse" AND all:"game"`, `ti:"Impasse"` : 5 et 8 résultats, tous sur d'autres sujets — « evolution impasse », « impasse surface » électrique, etc.).

Les travaux *du même auteur* (Jakub Kowalski, qui a créé le jeu) qui constituent la littérature transférable, lus via le flux arXiv :
- `https://arxiv.org/abs/2203.09608` — *Developing a Successful Bomberman Agent* : MCTS vs Rolling Horizon Evolution vs Beam Search, bitboards, évaluation simulation-based, **#1 sur 2 300 agents CodinGame**.
- `https://arxiv.org/abs/2105.01115` — *Evolving Evaluation Functions for Collectible Card Game AI* (évolution de poids d'évaluation).
- `https://arxiv.org/abs/2303.09449` — *Proof Number Based Monte-Carlo Tree Search*.
- `https://arxiv.org/abs/2112.07761` — *Split Moves for Monte-Carlo Tree Search*.

Leur applicabilité directe à Impasse est une inférence : **[NON VÉRIFIÉ]**.

---

## 3. Recommandation concrète et testable pour battre le boss

**Idée : « clone du boss + adversaire exact » au lieu d'un alpha-beta générique.**

Le boss est déterministe et myope. Le batteur optimal n'est pas un minimax contre *tous* les coups adverses, mais une recherche contre **le seul coup que le boss va réellement jouer**.

### Protocole en 3 étapes

1. **Cloner `Boss.java`** (génération de coups, ordre d'insertion des `singles`/`doubles`, `eval`, `GAMMA=0.82`, `DEPTH=4`, `>` strict). Le clone doit sortir exactement la même chaîne que le boss.
2. **Valider le prédicteur** : sur 20 parties contre le boss, à chaque tour, prédire le coup adverse depuis le plateau courant et comparer avec le `lastMove` reçu au tour suivant. Objectif : **100 % de correspondance**. Si ça échoue, le boss de la ligue n'est pas `config/Boss.java` → **[NON VÉRIFIÉ]** confirmé, basculez sur un alpha-beta classique.
3. **Remplacer le nœud adversaire par le clone** : pour chacun de vos coups légaux `a` :
   - appliquer `a` ;
   - demander au clone le coup boss `b`, l'appliquer ;
   - évaluer la position avec une évaluation *adversariale* :
     `E = 100*(retirés_moi − retirés_boss) − Σdist_crown_moi − Σdist_bearoff_moi + Σdist_crown_boss + Σdist_bearoff_boss + w·(mobilité_moi − mobilité_boss)`
     avec `mobilité = nombre de coups de base légaux`, `w ≈ 20` à tuner, et terminaux ±100 000.
   - choisir `argmax E`. Approfondir à 2–3 paires de coups si le budget de 100 ms le permet.

### Pourquoi ça doit marcher

- Le boss optimise `eval_mien` sur 4 de ses propres coups ; il est **incapable de voir qu'un pion qu'il avance sera bloqué** : votre terme de mobilité et votre prédiction exacte exploitent directement cet angle mort.
- Son actualisation 0.82 le fait privilégier le tempo immédiat ; l'évaluation différentielle `retirés_moi − retirés_boss` transforme la course en score direct, aligné avec le départage du referee.
- L'égalité se règle par `>` strict sur l'ordre de génération : en arrangeant deux coups boss de valeur égale, vous pouvez le **diriger** vers la branche qui vous arrange (la génération parcourt les singles dans l'ordre d'insertion, dir 0 avant dir 1, cases les plus proches d'abord).

### Expérience A/B mesurable

Avec le referee officiel en local (`GameState.java` + `Boss.java`) :
- 100 parties, votre bot en White puis en Black ;
- métriques : taux de victoire, écart moyen de score à 250 coups, nombre de coups où le boss est forcé à l'impasse ;
- comparer (A) bot sans modèle adverse, (B) bot avec clone + terme mobilité `w=20`, (C) `w ∈ {0, 10, 20, 40}`.
- Critère de réussite : **> 50 % de victoires en (B) et net progrès sur (A)** ; prédicteur à 100 % de justesse.

Budget : tout tient dans 100 ms/turn si le clone est implémenté avec des tableaux de 64 cases et sans allocation ; mesurer le nombre de nœuds/s dès la première version.

---

## 4. Liste de toutes les URL lues

**Referee / jeu officiel**
- `https://api.github.com/search/repositories?q=impasse+codingame`
- `https://api.github.com/repos/acatai/CG_Impasse/git/trees/main?recursive=1`
- `https://raw.githubusercontent.com/acatai/CG_Impasse/main/README.md`
- `https://raw.githubusercontent.com/acatai/CG_Impasse/main/config/config.ini`
- `https://raw.githubusercontent.com/acatai/CG_Impasse/main/config/stub.txt`
- `https://raw.githubusercontent.com/acatai/CG_Impasse/main/config/statement_en.html`
- `https://raw.githubusercontent.com/acatai/CG_Impasse/main/config/Boss.java`
- `https://raw.githubusercontent.com/acatai/CG_Impasse/main/src/main/java/com/codingame/game/Referee.java`
- `https://raw.githubusercontent.com/acatai/CG_Impasse/main/src/main/java/com/codingame/game/engine/Constants.java`
- `https://raw.githubusercontent.com/acatai/CG_Impasse/main/src/main/java/com/codingame/game/engine/GameState.java`
- `https://raw.githubusercontent.com/acatai/CG_Impasse/main/src/main/java/com/codingame/game/engine/Action.java`
- `https://api.github.com/repos/eulerscheZahl/RefereeCollection/contents/`
- `https://api.github.com/search/code?q=impasse+repo:eulerscheZahl/RefereeCollection` (401, non lu)
- `https://www.codingame.com/contribute/view/343356f1616c1e7c6a6619ae239b06f2a7427` (JS only, contenu non lu)
- `https://www.codingame.com/multiplayer/bot-programming/impasse/discuss` (JS only, contenu non lu)
- Tentatives PDF non lues (type `application/octet-stream` / `application/pdf`) : `https://raw.githubusercontent.com/acatai/CG_Impasse/main/GAME_RULES.pdf`, `https://marksteeregames.com/Impasse_rules.pdf`

**Forum / post-mortems**
- `https://forum.codingame.com/t/community-puzzle-impasse/198658` (titre seul)
- `https://forum.codingame.com/t/community-puzzle-impasse/198658.json` (3 messages lus)
- `https://forum.codingame.com/search.json?q=impasse`
- `https://forum.codingame.com/search.json?q=%22Impasse%22%20bot`

**Bots publics**
- `https://api.github.com/repos/penhauer/Impasse-for-CodinGame/git/trees/main?recursive=1`
- `https://raw.githubusercontent.com/penhauer/Impasse-for-CodinGame/main/{README.md,minimax_player.cpp,minimax_player.h,state.cpp,state.h,common.h,common.cpp,mcts_player.h,main.cpp,board.cpp,game.cpp}`
- `https://api.github.com/repos/Puma1208/Impasse/git/trees/main?recursive=1`
- `https://raw.githubusercontent.com/Puma1208/Impasse/main/README.md`
- `https://raw.githubusercontent.com/Puma1208/Impasse/main/src/GameElements/EvaluationFunction.java`
- `https://raw.githubusercontent.com/Puma1208/Impasse/main/src/AlphaBeta/AlphaBeta.java`
- `https://api.github.com/search/repositories?q=impasse+game+ai+in:name,description,readme&per_page=30`
- `https://api.github.com/search/repositories?q=impasse+codingame+in:name,description,readme&per_page=100`

**Recherche web (via webfetch)**
- `https://html.duckduckgo.com/html/?q=CodinGame+Impasse+post+mortem+bot`
- `https://html.duckduckgo.com/html/?q=site%3Acodingame.com%2Fforum+impasse+bot+algorithm`
- `https://html.duckduckgo.com/html/?q=%22Impasse%22+codingame+%22post+mortem%22+OR+%22write-up%22+OR+%22alpha-beta%22`
- `https://html.duckduckgo.com/html/?q=site%3Acodingame.com%2Fforum%2Ft%2F+%22Impasse%22`
- `https://html.duckduckgo.com/html/?q=codingame+impasse+legend+boss+strategy+bot`
- `https://html.duckduckgo.com/html/?q=github+%22impasse%22+codingame+bot+source+code`
- `https://html.duckduckgo.com/html/?q=%22impasse%22+%22Mark+Winands%22+game+AI`
- `https://html.duckduckgo.com/html/?q=site%3Aforum.codingame.com+impasse`
- `https://html.duckduckgo.com/html/?q=site%3Agist.github.com+impasse+codingame`
- `websearch` (4 requêtes) : échec HTTP 402, aucune donnée lue.

**Académique**
- `https://export.arxiv.org/api/query?search_query=all:%22Impasse%22+AND+all:%22game%22&max_results=20`
- `https://export.arxiv.org/api/query?search_query=ti:%22Impasse%22&max_results=20`
- `https://export.arxiv.org/api/query?search_query=au:%22Kowalski%22+AND+all:%22game%22&max_results=20`
- `https://api.semanticscholar.org/graph/v1/paper/search?query=impasse+board+game+search&fields=title,year,url&limit=10`
- Requêtes Semantic Scholar suivantes : 429, non lues (`"...Impasse Mark Steere..."`, `"...Impasse game search Jakub Kowalski..."`).

**API CodinGame (tentatives infructueuses, service introuvable / 422)** : `https://www.codingame.com/services/Comment/getComments`, `/Contribution/getContributionComments`, `/Contribution/findById`, `/Challenge/findAll` (POST).
