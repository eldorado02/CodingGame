<!-- opencode model: hy4-flash-free, 2026-11-23T20:42:06+03:00 -->

> build · hy4-flash-free
Je vais effectuer une recherche approfondie sur 'mad-knights'. Je commence par plusieurs recherches en parallèle pour identifier les sources.
✗ Web Search via OpenCode "CodinGame "Mad Knights" game rules referee" failed
Error: Web search request failed (HTTP 402)
✗ Web Search via OpenCode "eulerscheZahl RefereeCollection mad-knights" failed
Error: Web search request failed (HTTP 402)
✗ Web Search via OpenCode "CodinGame mad knights post-mortem strategy" failed
Error: Web search request failed (HTTP 402)
✗ Web Search via OpenCode "github CodinGame mad-knights referee" failed
Error: Web search request failed (HTTP 402)
> build · hy4-flash-free
La recherche web est indisponible (402). Je vais utiliser directement l'API GitHub et les URLs connues.
✗ WebFetch https://github.com/CodinGame/mad-knights failed
Error: StatusCode: non 2xx status code (404 GET https://github.com/CodinGame/mad-knights)
✗ WebFetch https://api.github.com/repos/CodinGame/mad-knights failed
Error: StatusCode: non 2xx status code (403 GET https://api.github.com/repos/CodinGame/mad-knights)
✗ WebFetch https://api.github.com/repos/eulerscheZahl/RefereeCollection/contents/ failed
Error: StatusCode: non 2xx status code (403 GET https://api.github.com/repos/eulerscheZahl/RefereeCollection/contents/)
% WebFetch https://api.github.com/search/repositories?q=mad-knights 
> build · hy4-flash-free
Le jeu semble récent (fin 2025). Je trouve un bot `darkhorse64/CG_MadKnights`. J'explore plusieurs sources en parallèle.
% WebFetch https://www.codingame.com/multiplayer/bot-programming/mad-knights 
% WebFetch https://raw.githubusercontent.com/eulerscheZahl/RefereeCollection/master/README.md 
% WebFetch https://github.com/darkhorse64/CG_MadKnights 
% WebFetch https://github.com/gabrielcamargols/Madly-Knights- 
> build · hy4-flash-free
Le dépôt `CG_MadKnights` contient le **code source du jeu** (dérivé de « Knight Isolation » d'Udacity). J'explore son arborescence et les référees officiels leakés.
% WebFetch https://raw.githubusercontent.com/darkhorse64/CG_MadKnights/main/README.md 
% WebFetch https://github.com/darkhorse64/CG_MadKnights/tree/main/src 
% WebFetch https://github.com/darkhorse64/CG_MadKnights/tree/main/config 
% WebFetch https://github.com/SpiritusSancti5/codinGame/tree/master/Referees 
> build · hy4-flash-free
Excellente trouvaille : le dépôt contient `config/Boss.java`, `config.ini` et les énoncés. Je les lis tous.
% WebFetch https://raw.githubusercontent.com/darkhorse64/CG_MadKnights/main/config/statement_en.html 
% WebFetch https://raw.githubusercontent.com/darkhorse64/CG_MadKnights/main/config/Boss.java 
% WebFetch https://raw.githubusercontent.com/darkhorse64/CG_MadKnights/main/config/stub.txt 
% WebFetch https://raw.githubusercontent.com/darkhorse64/CG_MadKnights/main/config/config.ini 
% WebFetch https://github.com/darkhorse64/CG_MadKnights/tree/main/src/main 
> build · hy4-flash-free
Règles officielles obtenues. Je lis maintenant le code du jeu (référee) pour les constantes et l'ordre de résolution.
% WebFetch https://www.codingame.com/forum/search?q=mad%20knights 
% WebFetch https://raw.githubusercontent.com/darkhorse64/CG_MadKnights/main/config/statement_fr.html 
% WebFetch https://github.com/darkhorse64/CG_MadKnights/tree/main/src/test 
% WebFetch https://github.com/darkhorse64/CG_MadKnights/tree/main/src/main/java/com/codingame/game 
> build · hy4-flash-free
% WebFetch https://raw.githubusercontent.com/darkhorse64/CG_MadKnights/main/src/main/java/com/codingame/game/Constants.java 
% WebFetch https://raw.githubusercontent.com/darkhorse64/CG_MadKnights/main/src/main/java/com/codingame/game/Referee.java 
% WebFetch https://raw.githubusercontent.com/darkhorse64/CG_MadKnights/main/src/main/java/com/codingame/game/Player.java 
% WebFetch https://github.com/darkhorse64/CG_MadKnights/tree/main/src/test/java 
% WebFetch https://github.com/darkhorse64/CG_MadKnights/tree/main/src/test/resources 
% WebFetch https://github.com/darkhorse64/CG_MadKnights/tree/main/src/main/java/com/codingame/game/engine 
> build · hy4-flash-free
Le code du referee est public. Je lis le moteur de jeu et les agents de test (candidats au boss).
% WebFetch https://raw.githubusercontent.com/darkhorse64/CG_MadKnights/main/src/main/java/com/codingame/game/engine/Action.java 
% WebFetch https://raw.githubusercontent.com/darkhorse64/CG_MadKnights/main/src/test/java/Agent2.java 
% WebFetch https://raw.githubusercontent.com/darkhorse64/CG_MadKnights/main/src/main/java/com/codingame/game/engine/Square.java 
% WebFetch https://raw.githubusercontent.com/darkhorse64/CG_MadKnights/main/src/main/java/com/codingame/game/engine/Board.java 
% WebFetch https://raw.githubusercontent.com/darkhorse64/CG_MadKnights/main/src/test/java/Main.java 
% WebFetch https://raw.githubusercontent.com/darkhorse64/CG_MadKnights/main/src/test/java/Agent1.java 
> build · hy4-flash-free
Le dépôt officiel ne contient que des agents aléatoires (pas le boss). Je cherche maintenant bots publics, post-mortems et littérature.
✗ WebFetch https://www.codingame.com/forum/search.json?q=mad%20knights failed
Error: StatusCode: non 2xx status code (404 GET https://www.codingame.com/forum/search.json?q=mad%20knights)
% WebFetch https://export.arxiv.org/api/query?search_query=all:%22isolation%20game%22%20AND%20all:%22alpha-beta%22&max_results=10 
✗ WebFetch https://api.semanticscholar.org/graph/v1/paper/search?query=adversarial%20search%20isolation%20game%20knight&limit=10&fields=title,url,abstract,year failed
Error: StatusCode: non 2xx status code (429 GET https://api.semanticscholar.org/graph/v1/paper/search?query=adversarial%20search%20isolation%20game%20knight&limit=10&fields=title,url,abstract,year)
✗ WebFetch https://api.github.com/search/gists?q=MadKnights failed
Error: StatusCode: non 2xx status code (403 GET https://api.github.com/search/gists?q=MadKnights)
% WebFetch https://api.github.com/search/repositories?q=MadKnights+codingame 
> build · hy4-flash-free
✗ WebFetch https://raw.githubusercontent.com/udacity/cd0349-adversarial-search/master/sample_players.py failed
Error: StatusCode: non 2xx status code (404 GET https://raw.githubusercontent.com/udacity/cd0349-adversarial-search/master/sample_players.py)
% WebFetch https://raw.githubusercontent.com/udacity/cd0349-adversarial-search/master/README.md 
% WebFetch https://html.duckduckgo.com/html/?q=site%3Acodingame.com%2Fforum+%22mad+knights%22 
% WebFetch https://html.duckduckgo.com/html/?q=%22MadKnights%22+codingame+bot+alpha-beta 
> build · hy4-flash-free
✗ WebFetch https://raw.githubusercontent.com/udacity/cd0349-adversarial-search/master/starter/game_agent.py failed
Error: StatusCode: non 2xx status code (404 GET https://raw.githubusercontent.com/udacity/cd0349-adversarial-search/master/starter/game_agent.py)
% WebFetch https://api.github.com/search/repositories?q=mad-knights+codingame 
% WebFetch https://raw.githubusercontent.com/udacity/cd0349-adversarial-search/master/starter/sample_players.py 
% WebFetch https://html.duckduckgo.com/html/?q=%22mad+knights%22+codingame+forum 
% WebFetch https://www.bing.com/search?q=%22Mad+Knights%22+codingame+bot 
> build · hy4-flash-free
% WebFetch https://www.codingame.com/multiplayer/bot-programming/mad-knights/discuss 
% WebFetch https://html.duckduckgo.com/html/?q=%22Mad+Knights%22+codingame+%22post+mortem%22 
% WebFetch https://html.duckduckgo.com/html/?q=%22Mad+Knights%22+alpha-beta+OR+MCTS+OR+minimax 
% WebFetch https://html.duckduckgo.com/html/?q=site%3Agithub.com+MadKnights+codingame 
% WebFetch https://github.com/search?q=MadKnights&type=repositories 
> build · hy4-flash-free
% WebFetch https://www.codingame.com/forum/c/bot-programming/l/1 
% WebFetch https://html.duckduckgo.com/html/?q=%22mad-knights%22+strategy+codingame 
% WebFetch https://html.duckduckgo.com/html/?q=site%3Acodingame.com%2Fforum+%22Mad+Knights%22 
% WebFetch https://export.arxiv.org/api/query?search_query=all:%22isolation%22+AND+all:%22adversarial+search%22&max_results=10 
✗ WebFetch https://grep.app/api/search?q=MadKnights failed
Error: StatusCode: non 2xx status code (429 GET https://grep.app/api/search?q=MadKnights)
> build · hy4-flash-free
✗ WebFetch https://api.semanticscholar.org/graph/v1/paper/search?query=knight%20isolation%20adversarial%20search%20agent&limit=10&fields=title,url,year,abstract failed
Error: StatusCode: non 2xx status code (429 GET https://api.semanticscholar.org/graph/v1/paper/search?query=knight%20isolation%20adversarial%20search%20agent&limit=10&fields=title,url,year,abstract)
% WebFetch https://export.arxiv.org/api/query?search_query=all:%22multiplayer%22+AND+all:%22game+tree+search%22&max_results=10 
% WebFetch https://www.google.com/search?q=%22mad+knights%22+codingame+strategy+bot 
% WebFetch https://www.google.com/search?q=site%3Acodingame.com%2Fforum+%22Mad+Knights%22 
> build · hy4-flash-free
% WebFetch https://www.bing.com/search?q=codingame+mad+knights+post+mortem 
✗ WebFetch https://r.jina.ai/https://www.google.com/search?q=site%3Acodingame.com%2Fforum+%22Mad+Knights%22 failed
Error: StatusCode: non 2xx status code (403 GET https://r.jina.ai/https://www.google.com/search?q=site%3Acodingame.com%2Fforum+%22Mad+Knights%22)
% WebFetch https://r.jina.ai/https://www.codingame.com/multiplayer/bot-programming/mad-knights 
% WebFetch https://r.jina.ai/https://www.codingame.com/multiplayer/bot-programming/mad-knights/discuss 
> build · hy4-flash-free
% WebFetch https://www.bing.com/search?q=%22Mad+Knights%22+codingame 
% WebFetch https://www.mojeek.com/search?q=%22Mad+Knights%22+codingame 
% WebFetch https://r.jina.ai/https://www.codingame.com/multiplayer/bot-programming/mad-knights/leaderboard 
% WebFetch https://r.jina.ai/https://www.codingame.com/forum/search?q=mad%20knights 
> build · hy4-flash-free
% WebFetch https://www.bing.com/search?q=%22Mad+Knights%22+post-mortem 
% WebFetch https://lite.duckduckgo.com/lite/?q=%22Mad+Knights%22+codingame 
✗ WebFetch https://web.archive.org/cdx/search/cdx?url=codingame.com/forum/*&output=json&filter=urlkey:.*mad.*&filter=statuscode:200&collapse=urlkey&limit=50 failed
Error: StatusCode: non 2xx status code (503 GET https://web.archive.org/cdx/search/cdx?url=codingame.com/forum/*&output=json&filter=urlkey:.*mad.*&filter=statuscode:200&collapse=urlkey&limit=50)
✗ WebFetch https://gist.github.com/search?q=mad-knights failed
Error: Transport error (GET https://gist.github.com/search?q=mad-knights)
> build · hy4-flash-free
Une piste majeure : `config/Boss.java` est le boss du jeu et il joue `random`. Je vérifie le rôle de ce fichier dans le template officiel CodinGame.
✗ WebFetch https://raw.githubusercontent.com/CodinGame/game-template/master/README.md failed
Error: StatusCode: non 2xx status code (404 GET https://raw.githubusercontent.com/CodinGame/game-template/master/README.md)
% WebFetch https://export.arxiv.org/api/query?search_query=all:%22multi-player%20games%22%20AND%20all:%22Monte%20Carlo%22&max_results=10 
✗ WebFetch https://github.com/CodinGame/game-template failed
Error: StatusCode: non 2xx status code (404 GET https://github.com/CodinGame/game-template)
✗ WebFetch https://api.github.com/repos/CodinGame/game-template/contents/config failed
Error: StatusCode: non 2xx status code (403 GET https://api.github.com/repos/CodinGame/game-template/contents/config)
> build · hy4-flash-free
% WebFetch https://www.bing.com/search?q=github+CodinGame+game+template+Boss.java+config 
✗ WebFetch https://r.jina.ai/https://github.com/search?q=org%3ACodinGame+game-template&type=repositories failed
Error: StatusCode: non 2xx status code (403 GET https://r.jina.ai/https://github.com/search?q=org%3ACodinGame+game-template&type=repositories)
✗ WebFetch https://r.jina.ai/https://github.com/CodinGame?tab=repositories&q=template failed
Error: StatusCode: non 2xx status code (403 GET https://r.jina.ai/https://github.com/CodinGame?tab=repositories&q=template)
> build · hy4-flash-free
✗ WebFetch https://api.github.com/orgs/CodinGame/repos?per_page=100 failed
Error: StatusCode: non 2xx status code (403 GET https://api.github.com/orgs/CodinGame/repos?per_page=100)
✗ WebFetch https://raw.githubusercontent.com/CodinGame/game-development-kit/master/README.md failed
Error: StatusCode: non 2xx status code (404 GET https://raw.githubusercontent.com/CodinGame/game-development-kit/master/README.md)
% WebFetch https://raw.githubusercontent.com/CodinGame/codingame-game-engine/master/README.md 
> build · hy4-flash-free
% WebFetch https://raw.githubusercontent.com/CodinGame/game-skeleton/master/README.md 
% WebFetch https://github.com/CodinGame/game-skeleton 
% WebFetch https://github.com/CodinGame/game-skeleton/tree/master/config 
> build · hy4-flash-free
% WebFetch https://raw.githubusercontent.com/CodinGame/game-skeleton/master/config/Boss.py3 
% WebFetch https://raw.githubusercontent.com/CodinGame/codingame-sdk-doc/master/README.md 
% WebFetch https://github.com/CodinGame/codingame-sdk-doc 
> build · hy4-flash-free
% WebFetch https://export.arxiv.org/api/query?search_query=all:%22isolation%20game%22&max_results=20 
% WebFetch https://r.jina.ai/https://www.codingame.com/playgrounds/25775/ 
% WebFetch https://www.semanticscholar.org/search?q=knight%20isolation%20game&sort=relevance 
> build · hy4-flash-free
% WebFetch https://sourcegraph.com/search?q=context:global+MadKnights&patternType=literal 
% WebFetch https://r.jina.ai/https://www.codingame.com/forum 
> build · hy4-flash-free
% WebFetch https://raw.githubusercontent.com/darkhorse64/CG_MadKnights/main/src/main/java/com/codingame/game/engine/Piece.java 
> build · hy4-flash-free
# Deep search — CodinGame multi « Mad Knights »

> Légende : **[VÉRIFIÉ]** = lu dans une source citée · **[INFÉRÉ]** = déduction à partir de sources lues · **[NON VÉRIFIÉ]** = non confirmé par une source lue.

---

## 1) Faits du referee (source officielle)

Le dépôt public est explicitement désigné comme « le referee » par l'énoncé officiel du jeu :
`https://www.codingame.com/multiplayer/bot-programming/mad-knights` → « Click here for the referee » → `https://github.com/darkhorse64/CG_MadKnights`. **[VÉRIFIÉ]**

### 1.1 Constantes (URL exacte : `https://raw.githubusercontent.com/darkhorse64/CG_MadKnights/main/src/main/java/com/codingame/game/Constants.java`) **[VÉRIFIÉ]**

| Constante | Valeur | Commentaire |
|---|---|---|
| `Columns`, `Rows` | 8, 8 | Échiquier 8×8 |
| `KnightMoves` | 8 | 8 sauts de cavalier |
| `KnightDeltaColumn` | `{-1, 1, -2, 2, -2, 2, -1, 1}` | décalages colonne |
| `KnightDeltaRow` | `{-2, -2, -1, -1, 1, 1, 2, 2}` | décalages ligne |
| `maxTurns` | **62** | limite de sécurité (64 cases − 3 pièces ≈ 61 coups) |
| `firstTurnMaxTime` | **1000 ms** | premier tour |
| `turnMaxTime` | **100 ms** | tours suivants |
| `maxCommentLength` | **16** | commentaire tronqué à 16 caractères |
| `frameDuration` / `moveTime` | 1800 / 700 | animation uniquement |
| `Colors = 2`, `hasPlayerWon(player ^ 1)` | restes du jeu 2 joueurs | **non utilisés par le referee** ; ne pas s'appuyer dessus |

`config.ini` (`https://raw.githubusercontent.com/darkhorse64/CG_MadKnights/main/config/config.ini`) : `title=Mad Knights`, `type=multi`, `min_players=3`, `max_players=3`. **[VÉRIFIÉ]**

### 1.2 Plateau, positions initiales, blocage

- 3 joueurs **r, g, b**, un seul cavalier chacun ; ordre fixe **Rouge → Vert → Bleu**. **[VÉRIFIÉ]** (`statement_en.html`)
- Positions initiales aléatoires mais **bords exclus** : le code tire `x, y ∈ [0;7]` avec rejet si `x<1 || x>6 || y<1 || y>6` ou case occupée → **sous-grille intérieure 6×6 = 36 cases**. **[VÉRIFIÉ]** (`engine/Board.java`)
- Une case occupée devient `free=false` **dès le départ** (`Square.setUnit`) ; à chaque coup, la case d'arrivée passe `free=false` (`Piece.moveTo`). La case quittée reste donc bloquée : le plateau ne fait que se remplir. **[VÉRIFIÉ]** (`engine/Square.java`, `engine/Piece.java`)
- Notations : colonne `a..h` = 0..7, ligne `1..8` = 0..7 ; l'affichage grid est **du haut vers le bas**, donc rank `8` puis `7` … `1`. **[VÉRIFIÉ]** (`Square.java` + `Referee.sendInputs` : `squares[rows-y-1][x]`)

### 1.3 Ordre de résolution exact (URL : `https://raw.githubusercontent.com/darkhorse64/CG_MadKnights/main/src/main/java/com/codingame/game/Referee.java`) **[VÉRIFIÉ]**

À chaque `gameTurn(turn)` :

1. **Victoire** : si `getActivePlayers().size() == 1`, le dernier actif gagne, reçoit `score++`, `endGame()`.
2. **Coups légaux** de `currentPlayer` = `KNIGHT_ATTACKS[pos] & ~occupied` (8 décalages, case d'arrivée `free`).
3. **Aucun coup** : le joueur reçoit `score++`, est `deactivate` (« has no more moves ») — *il n'est pas pénalisé*.
4. **Sinon** : envoi des inputs, `execute()`, lecture de la **première** ligne de sortie :
   - découpe du commentaire au **premier espace** (tronqué à 16) ;
   - sortie `random` → le **referee** tire uniformément parmi les coups légaux avec un `Random(seed)` propre au jeu ;
   - sinon : premier `action` dont `action.toString()` est **préfixe** de la sortie → appliqué (ex. `e3blabla` accepte `e3`) ;
   - introuvable → `InvalidActionException` → `deactivate` + **`score = -1`** ;
   - timeout → `deactivate` + **`score = -1`**.
5. Joueur suivant = premier actif dans l'ordre cyclique `r → g → b`.
6. **Fin** au bout de 62 tours si personne n'a gagné. **[VÉRIFIÉ]** (`Constants.maxTurns`, `Referee.init`)

### 1.4 Protocole et départages **[VÉRIFIÉ]**

- Input initial `color` envoyé seulement les 3 premiers tours (condition `turn < playerCount + 1`).
- Puis chaque tour : **3 lignes** `player status lastMove` (tous les joueurs, même éliminés ; `lastMove = "null"` au début), **8 lignes** de grille, `moveCount`, puis `moveCount` lignes de coups **triés alphabétiquement** (`Comparator.comparing(Action::toString)`).
- Sortie : une destination (`e3`), `random`, ou `e3 commentaire`.
- **Départage** : le seul mécanisme visible est le champ `score` — éliminé « proprement » → 0, 1, 2… ; gagnant → score suivant ; triche/timeout → **−1**. L'usage de ce score par le classement CodinGame est **[INFÉRÉ]** (non spécifié dans le code lu). Aucun départage explicite supplémentaire (ex. nombre de coups) n'apparaît dans le referee. **[NON VÉRIFIÉ]**

---

## 2) Stratégies des meilleurs joueurs

### 2.1 Ce qui est réellement vérifiable

- **Algorithmes officiellement suggérés** par la page du jeu : *Monte Carlo tree search, Minimax, Neural network, Bitboarding, Heuristic search*. **[VÉRIFIÉ]** (`https://www.codingame.com/multiplayer/bot-programming/mad-knights`)
- **Classement (23/11/2026)** : 137 CodinGamers, **tous en ligue Wood** ; top 9 : `nafis277` (C, 31.01), `N3l` (C++, 30.41), `tarriel` (Rust, 30.25), `Waffle3z` (C++, 30.05), `aCat` (C++, 29.30), `YALOKGAR` (C++, 28.90), `MSmits` (C++, 28.78), `ArturBieniek` (C++, 28.77), `Zylo` (C++, 28.74). **[VÉRIFIÉ]** (`.../mad-knights/leaderboard`) — le leaderboard ne publie **aucun** détail d'algorithme.
- **Recherche de bots publics** : la recherche GitHub ne renvoie que le dépôt du referee (`darkhorse64/CG_MadKnights`) ; aucun bot MadKnights public, aucun gist, aucun post-mortem n'a été trouvé via GitHub search, gists, forum CodinGame (SPA non indexable), DuckDuckGo/Bing/Mojeek. **[NON VÉRIFIÉ]** : les algorithmes, termes d'évaluation et poids exacts des joueurs ci-dessus.

### 2.2 Source canonique de la mécanique (Udacity « Knight Isolation », dont est dérivé Mad Knights) **[VÉRIFIÉ]**

- `https://github.com/udacity/cd0349-adversarial-search` (README) : « minimax search with alpha-beta pruning and iterative deepening », heuristique de référence **`#my_moves − #opponent_moves`**.
- `https://raw.githubusercontent.com/udacity/cd0349-adversarial-search/master/starter/sample_players.py` :
  - `GreedyPlayer.score = len(state.liberties(own_loc))` → profondeur 1 ;
  - `MinimaxPlayer.score = len(own_liberties) − len(opp_liberties)`, minimax profondeur 3, aléatoire pour les 2 premiers plis.
- **Limite** : ce travail est **2 joueurs** ; Mad Knights est **3 joueurs et « dernier survivant »**, donc l'heuristique doit être adaptée (cf. §3).

### 2.3 Littérature (générale, pas spécifique à Mad Knights)

- `https://arxiv.org/abs/2006.08659` — Goodman & Lucas, *Does it matter how well I know what you're thinking? Opponent Modelling in an RTS game* (2020) : en jeu multi-joueurs avec budget faible, **MCTS reste robuste même avec un modèle d'adversaire inexact**, contrairement à RHEA. **[VÉRIFIÉ]** (résumé lu). Idée transposable : ne pas sur-investir dans un modèle adverse précis à 100 ms/coup.
- `https://arxiv.org/abs/2409.14180` — *Isolation game on graphs* (2024) : jeu **mathématiquement différent** (domination/isolation sur graphes), pas directement applicable. **[VÉRIFIÉ]**
- Aucun article arXiv/Semantic Scholar spécifique à Mad Knights trouvé ; l'API Semantic Scholar a renvoyé 429. **[NON VÉRIFIÉ]**

---

## 3) Recommandation concrète et testable pour battre le boss

### 3.1 Le boss est probablement… aléatoire

`config/Boss.java` du dépôt officiel contient exactement le stub auto-généré qui输出 `System.out.println("random")`. **[VÉRIFIÉ]** (`https://raw.githubusercontent.com/darkhorse64/CG_MadKnights/main/config/Boss.java`)

Le template officiel `CodinGame/game-skeleton` place justement l'IA du boss dans `config/Boss.*` (`Boss.py3`). **[VÉRIFIÉ]** (`https://github.com/CodinGame/game-skeleton/tree/master/config`)

→ **Le boss joue un coup légal uniformément au hasard** (et ne peut donc ni sortir de plateau, ni timeout). **[INFÉRÉ]** (le fait que ce fichier soit le boss effectivement utilisé en ligue n'est pas confirmé par une source lue : **[NON VÉRIFIÉ]**).

### 3.2 Recommandation principale : « survivance par composante », pas « libertés immédiates »

Dans ce jeu, la vraie ressource n'est pas le nombre de libertés *maintenant*, mais le **nombre de cases libres atteignables dans la composante connexe du cavalier** (les cavaliers sautent par-dessus les cases bloquées, mais ne peuvent atterrir que sur une case libre). Un bot qui maximise seulement ses libertés immédiates se retrouve piégé dans une petite région.

**Implémentation minimale (testable en < 100 ms) :**

1. Bitboard 64 bits `occupied` + 3 positions ; voisinage cavalier précalculé `ATTACKS[64]` (bitboard). **[VÉRIFIÉ que c'est faisable : 64 cases, ≤8 coups/coup]**
2. **Ne jamais deviner la sortie** : choisir uniquement parmi la liste `move` fournie par le referee (elle est déjà filtrée et triée) → élimine tout risque de `score=-1`. **[VÉRIFIÉ]** (`Referee.sendInputs`)
3. Calculer pour chaque joueur依然 actif `comp[i]` = taille de la composante connexe (BFS sur `ATTACKS & ~occupied`) et `lib[i] = popcount(ATTACKS[pos] & ~occupied)`.
4. Évaluation (poids de départ, à régler par A/B) :
   ```
   E = 1.0*comp[me] - 0.5*(comp[opp1] + comp[opp2])
     + 3.0*lib[me] - 0.5*(lib[opp1] + lib[opp2])
     + 0.2*centerBonus[me]
   ```
   Terminal : plus de coups pour moi → −∞ ; il ne reste que moi → +∞. **[INFÉRÉ]** (heuristique proposée, non lue chez un top joueur).
5. Recherche : **alpha-bêta itératif (paranoïaque `max-min-min`, 3 joueurs)**, profondeur 4→6 plis avec contrôle temps toutes les 1024 feuilles, marge de sécurité 10 ms ; en Python, rester à 2–3 plis + MCTS léger ; C++/Rust pour 6–8 plis (le top du classement estmajoritairement C++/Rust). **[INFÉRÉ]**

### 3.3 Protocole d'expérimentation (pour valider, pas deviner)

- **A/B déterministe** : le referee utilise `Random(seed)` pour les positions initiales **et** pour le `random` du boss → mêmes graines = mêmes parties. Comparer sur **≥500 graines identiques**. **[VÉRIFIÉ]** (`Referee.init`, `Board` constructor)
- Paliers : (A) `max(lib)` → (B) `max(comp)` → (C) alpha-bêta + évaluation ci-dessus.
- Métriques : **taux de victoire vs boss**, **placement moyen** (score 2/1/0), **nombre moyen de coups survivus**, **taux d'invalidité (doit être 0)**.
- Critère d'acceptation : (C) bat (A) d'au moins **+5 points de pourcentage** avec IC 95 % sans chevauchement, et 0 coup invalide.
- Si le boss est vraiment aléatoire, (B) devrait déjà approcher 100 % de victoires ; si ce n'est pas le cas, c'est que le boss de ligue diffère de `Boss.java` → passer directement à (C) et à un modèle d'adversaire « minimise ma composante ». **[INFÉRÉ]**

### 3.4 Pièges concrets à éviter (tous vérifiés dans le code)

- Ne pas utiliser `Board.hasPlayerWon()` : il teste `player ^ 1` (reste 2 joueurs). **[VÉRIFIÉ]**
- Ne pas inverser l'axe des lignes : la ligne 0 affichée est le rank **8**. **[VÉRIFIÉ]**
- Ne pas dépasser 16 caractères de commentaire ni écrire plusieurs lignes (`getExpectedOutputLines() = 1`). **[VÉRIFIÉ]**
- Ne jamais sortir `random` soi-même : on perd le contrôle du choix. **[VÉRIFIÉ]**

---

## 4) Liste de toutes les URL lues

**Referee / jeu (sources primaires)**
1. https://www.codingame.com/multiplayer/bot-programming/mad-knights
2. https://www.codingame.com/multiplayer/bot-programming/mad-knights/discuss
3. https://www.codingame.com/multiplayer/bot-programming/mad-knights/leaderboard
4. https://github.com/darkhorse64/CG_MadKnights
5. https://raw.githubusercontent.com/darkhorse64/CG_MadKnights/main/README.md
6. https://raw.githubusercontent.com/darkhorse64/CG_MadKnights/main/config/statement_en.html
7. https://raw.githubusercontent.com/darkhorse64/CG_MadKnights/main/config/statement_fr.html
8. https://raw.githubusercontent.com/darkhorse64/CG_MadKnights/main/config/config.ini
9. https://raw.githubusercontent.com/darkhorse64/CG_MadKnights/main/config/Boss.java
10. https://raw.githubusercontent.com/darkhorse64/CG_MadKnights/main/config/stub.txt
11. https://raw.githubusercontent.com/darkhorse64/CG_MadKnights/main/src/main/java/com/codingame/game/Constants.java
12. https://raw.githubusercontent.com/darkhorse64/CG_MadKnights/main/src/main/java/com/codingame/game/Referee.java
13. https://raw.githubusercontent.com/darkhorse64/CG_MadKnights/main/src/main/java/com/codingame/game/Player.java
14. https://raw.githubusercontent.com/darkhorse64/CG_MadKnights/main/src/main/java/com/codingame/game/engine/Board.java
15. https://raw.githubusercontent.com/darkhorse64/CG_MadKnights/main/src/main/java/com/codingame/game/engine/Action.java
16. https://raw.githubusercontent.com/darkhorse64/CG_MadKnights/main/src/main/java/com/codingame/game/engine/Square.java
17. https://raw.githubusercontent.com/darkhorse64/CG_MadKnights/main/src/main/java/com/codingame/game/engine/Piece.java
18. https://github.com/darkhorse64/CG_MadKnights/tree/main/src/main/java/com/codingame/game
19. https://github.com/darkhorse64/CG_MadKnights/tree/main/src/main/java/com/codingame/game/engine
20. https://github.com/darkhorse64/CG_MadKnights/tree/main/src/test/java

**Template officiel / rôles des fichiers**
21. https://github.com/CodinGame/game-skeleton
22. https://raw.githubusercontent.com/CodinGame/game-skeleton/master/README.md
23. https://github.com/CodinGame/game-skeleton/tree/master/config
24. https://raw.githubusercontent.com/CodinGame/game-skeleton/master/config/Boss.py3
25. https://github.com/CodinGame/codingame-sdk-doc
26. https://raw.githubusercontent.com/CodinGame/codingame-sdk-doc/master/README.md
27. https://raw.githubusercontent.com/CodinGame/codingame-game-engine/master/README.md
28. https://raw.githubusercontent.com/eulerscheZahl/RefereeCollection/master/README.md
29. https://github.com/SpiritusSancti5/codinGame/tree/master/Referees

**Algorithme / littérature**
30. https://github.com/udacity/cd0349-adversarial-search
31. https://raw.githubusercontent.com/udacity/cd0349-adversarial-search/master/README.md
32. https://raw.githubusercontent.com/udacity/cd0349-adversarial-search/master/starter/sample_players.py
33. https://arxiv.org/abs/2006.08659
34. https://arxiv.org/abs/2409.14180
35. https://export.arxiv.org/api/query?search_query=all:%22isolation%20game%22&max_results=20
36. https://export.arxiv.org/api/query?search_query=all:%22multi-player%20games%22%20AND%20all:%22Monte%20Carlo%22&max_results=10
37. https://github.com/search?q=MadKnights&type=repositories

**Tentatives sans résultat exploitable** (pour information) : `api.github.com/search/repositories?q=mad-knights`, `.../search/gists`, `.../repos/CodinGame/mad-knights` (404), `www.codingame.com/forum/search?q=mad%20knights` (SPA vide), `api.semanticscholar.org/...` (429), DuckDuckGo/Bing/Mojeek/Google (captchas ou aucun résultat pertinent), `grep.app` (429), `web.archive.org/cdx` (503).
