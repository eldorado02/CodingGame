<!-- opencode model: hy4-flash-free, 2026-11-23T20:46:37+03:00 -->

> build · hy4-flash-free
Je vais effectuer une recherche structurée, en commençant par le referee officiel, puis les bots publics, les post-mortems, et enfin les articles académiques. Je vais lire chaque source et marquer explicitement ce que je n'ai pas pu vérifier.
✗ Web Search via OpenCode "CodinGame checkers multi game referee GitHub rules" failed
Error: Web search request failed (HTTP 402)
✗ Web Search via OpenCode "CodinGame "Checkers" contest post-mortem bot alpha-beta" failed
Error: Web search request failed (HTTP 402)
% WebFetch https://api.github.com/search/repositories?q=referee+checkers+in:name 
% WebFetch https://api.github.com/search/repositories?q=checkers+codingame+referee 
> build · hy4-flash-free
Websearch est indisponible (HTTP 402). Je vais utiliser l'API GitHub et webfetch directement. Explorons les dépôts trouvés et les sources officielles.
✗ WebFetch https://api.github.com/repos/SteamDwarf/CheckersReferee/contents failed
Error: StatusCode: non 2xx status code (403 GET https://api.github.com/repos/SteamDwarf/CheckersReferee/contents)
✗ WebFetch https://api.github.com/repos/linjoehan/Codingame_Checkers/contents failed
Error: StatusCode: non 2xx status code (403 GET https://api.github.com/repos/linjoehan/Codingame_Checkers/contents)
✗ WebFetch https://api.github.com/repos/eulerscheZahl/RefereeCollection/contents failed
Error: StatusCode: non 2xx status code (403 GET https://api.github.com/repos/eulerscheZahl/RefereeCollection/contents)
% WebFetch https://api.github.com/search/repositories?q=org:CodinGame+checkers 
> build · hy4-flash-free
L'API contents est bloquée (403), mais la recherche fonctionne. Essayons les pages GitHub HTML et raw.githubusercontent.com.
% WebFetch https://github.com/eulerscheZahl/RefereeCollection 
% WebFetch https://github.com/linjoehan/Codingame_Checkers 
% WebFetch https://github.com/SteamDwarf/CheckersReferee 
> build · hy4-flash-free
Le README d'eulerscheZahl pointe vers les vrais referee "leakés", mais sa collection ne contient pas Checkers. Le dépôt `linjoehan/Codingame_Checkers` contient un dossier `game-checkers-master` prometteur. Explorons-le, ainsi que le code du referee SteamDwarf.
% WebFetch https://api.github.com/search/repositories?q=codingame+checkers+bot+in:name,description 
% WebFetch https://github.com/linjoehan/Codingame_Checkers/tree/master/src 
% WebFetch https://github.com/SteamDwarf/CheckersReferee/tree/main/server 
% WebFetch https://github.com/linjoehan/Codingame_Checkers/tree/master/game-checkers-master 
> build · hy4-flash-free
! retrying (attempt 2): Can't reach opencode.ai. Check the network connection. (getaddrinfo EAI_AGAIN opencode.ai)
Allons plus loin dans le code du jeu `game-checkers-master` et dans les sources du referee SteamDwarf, et élargissons la recherche de bots.
% WebFetch https://api.github.com/search/repositories?q=checkers+codingame 
% WebFetch https://github.com/linjoehan/Codingame_Checkers/tree/master/game-checkers-master/src 
% WebFetch https://github.com/linjoehan/Codingame_Checkers/tree/master/src/main 
% WebFetch https://github.com/SteamDwarf/CheckersReferee/tree/main/server/src 
> build · hy4-flash-free
J'ai trouvé des bots publics. Allons lire la logique de jeu Java et les bots C++/Python.
% WebFetch https://github.com/linjoehan/Codingame_Checkers/tree/master/src/main/java/com/codingame/game 
% WebFetch https://github.com/linjoehan/Codingame_Checkers/tree/master/game-checkers-master/src/main 
% WebFetch https://github.com/Watashiku/checkers 
% WebFetch https://github.com/romulopro/Checkers_from_Codingame 
> build · hy4-flash-free
Je vais maintenant lire le contenu brut des fichiers clés : le referee Java, les bots C++ et Python, et la config.
% WebFetch https://raw.githubusercontent.com/linjoehan/Codingame_Checkers/master/src/main/java/com/codingame/game/Referee.java 
% WebFetch https://raw.githubusercontent.com/romulopro/Checkers_from_Codingame/master/checkers.py 
% WebFetch https://raw.githubusercontent.com/Watashiku/checkers/master/main.cpp 
% WebFetch https://github.com/linjoehan/Codingame_Checkers/tree/master/config 
> build · hy4-flash-free
Le referee Java et deux bots sont lus. Je vais maintenant lire les fichiers de configuration — surtout `Boss.java` (le boss de la ligue) et `config.ini` (constantes) — ainsi que l'énoncé officiel.
% WebFetch https://raw.githubusercontent.com/linjoehan/Codingame_Checkers/master/config/config.ini 
% WebFetch https://raw.githubusercontent.com/linjoehan/Codingame_Checkers/master/config/statement_en.html 
% WebFetch https://raw.githubusercontent.com/linjoehan/Codingame_Checkers/master/config/Boss.java 
% WebFetch https://raw.githubusercontent.com/linjoehan/Codingame_Checkers/master/config/stub.txt 
> build · hy4-flash-free
Découverte majeure : `Boss.java` joue simplement le **premier coup légal** de la liste. Je vais vérifier l'existence d'un referee officiel, chercher d'autres bots/post-mortems, et interroger Semantic Scholar / arXiv.
% WebFetch https://www.codingame.com/multiplayer/bot-programming/checkers 
% WebFetch https://export.arxiv.org/api/query?search_query=all:checkers+AND+all:artificial+intelligence&start=0&max_results=10 
✗ WebFetch https://api.semanticscholar.org/graph/v1/paper/search?query=checkers%20draughts%20artificial%20intelligence%20alpha-beta&fields=title,authors,year,abstract,url&limit=10 failed
Error: StatusCode: non 2xx status code (429 GET https://api.semanticscholar.org/graph/v1/paper/search?query=checkers%20draughts%20artificial%20intelligence%20alpha-beta&fields=title,authors,year,abstract,url&limit=10)
% WebFetch https://api.github.com/search/repositories?q=draughts+codingame+OR+checkers+bot+in:description 
> build · hy4-flash-free
Trouvé un bot de très haut niveau : `VikSolve/English-Draughts-3rd-Worldwide` (« 3rd Global and 2nd Poland on codingame »). C'est la source clé des meilleurs joueurs. Je vais la lire en profondeur.
✗ WebFetch https://api.github.com/repos/VikSolve/English-Draughts-3rd-Worldwide/contents failed
Error: StatusCode: non 2xx status code (403 GET https://api.github.com/repos/VikSolve/English-Draughts-3rd-Worldwide/contents)
% WebFetch https://export.arxiv.org/api/query?search_query=ti:%22English%20Draughts%22+OR+ti:%22checkers%22+AND+cat:cs.AI&start=0&max_results=20 
% WebFetch https://github.com/VikSolve/English-Draughts-3rd-Worldwide 
> build · hy4-flash-free
Le bot n°3 mondial est un moteur bitboard C++ sans réseau de neurones. Lisons son code source pour extraire l'évaluation et les heuristiques exactes.
% WebFetch https://raw.githubusercontent.com/VikSolve/English-Draughts-3rd-Worldwide/main/src.cpp 
> build · hy4-flash-free
Le moteur de VikSolve est très instructif. Je vais compléter avec les issues du dépôt referee, d'autres bots, et retenter Semantic Scholar pour les articles académiques.
% WebFetch https://raw.githubusercontent.com/VikSolve/English-Draughts-3rd-Worldwide/main/README.md 
✗ WebFetch https://api.semanticscholar.org/graph/v1/paper/search?query=draughts%20checkers%20engine&fields=title,year,url&limit=5 failed
Error: StatusCode: non 2xx status code (429 GET https://api.semanticscholar.org/graph/v1/paper/search?query=draughts%20checkers%20engine&fields=title,year,url&limit=5)
% WebFetch https://api.github.com/search/repositories?q=%22English+Draughts%22+codingame+bot 
% WebFetch https://api.github.com/repos/linjoehan/Codingame_Checkers/issues 
> build · hy4-flash-free
Les issues confirment que ce dépôt est bien lié au développement du jeu (eulerscheZahl y a contribué). Vérifions la collection de referee « leakés » et cherchons les post-mortems du forum.
% WebFetch https://www.codingame.com/forum/search?q=checkers 
% WebFetch https://api.github.com/search/repositories?q=user:eulerscheZahl+checkers 
% WebFetch https://export.arxiv.org/api/query?search_query=ti:draughts+OR+ti:checkers&start=0&max_results=20 
% WebFetch https://github.com/SpiritusSancti5/codinGame/tree/master/Referees 
> build · hy4-flash-free
La collection de referees officiels leakés ne contient pas Checkers. Je vais tenter les endpoints JSON du forum CodinGame et compléter la recherche académique.
✗ WebFetch https://www.codingame.com/forum/l/latest.json failed
Error: StatusCode: non 2xx status code (404 GET https://www.codingame.com/forum/l/latest.json)
✗ WebFetch https://www.codingame.com/forum/search.json?q=checkers failed
Error: StatusCode: non 2xx status code (404 GET https://www.codingame.com/forum/search.json?q=checkers)
✗ WebFetch https://api.semanticscholar.org/graph/v1/paper/search/bulk?query=English%20draughts%20checkers%20search%20algorithm&fields=title,year,url failed
Error: StatusCode: non 2xx status code (429 GET https://api.semanticscholar.org/graph/v1/paper/search/bulk?query=English%20draughts%20checkers%20search%20algorithm&fields=title,year,url)
% WebFetch https://export.arxiv.org/api/query?search_query=all:%22checkers%22+AND+all:%22Chinook%22&start=0&max_results=10 
> build · hy4-flash-free
Dernières vérifications : articles d'IA sur les dames et bots additionnels, puis je synthétise.
% WebFetch https://export.arxiv.org/api/query?search_query=all:draughts+AND+all:%22alpha-beta%22&start=0&max_results=10 
% WebFetch https://api.github.com/search/repositories?q=codingame+checkers+alpha-beta+OR+negamax+OR+bitboard 
% WebFetch https://export.arxiv.org/api/query?search_query=all:%22English%20draughts%22+AND+all:%22artificial%20intelligence%22&start=0&max_results=10 
> build · hy4-flash-free
# Recherche deep — CodinGame multi « Checkers » (English Draughts)

> ⚠️ **Avertissement sourcing** : je n'ai **pas trouvé de referee officiel CodinGame** pour ce jeu. La collection des vrais referees leakés (`SpiritusSancti5/codinGame`) ne contient **pas** Checkers. La source la plus fiable disponible est le dépôt de développement du jeu `linjoehan/Codingame_Checkers` (SDK CodinGame, avec `config/`, `statement_en.html`, `Boss.java`, et des issues ouvertes par eulerscheZahl). Je le qualifie donc de **refereur de référence communautaire**, pas de referee serveur certifié. Tout ce qui n'a pas été lu dans une source est marqué **[NON VÉRIFIÉ]**.

---

## 1. Faits du referee (avec URL exactes)

Source : `https://raw.githubusercontent.com/linjoehan/Codingame_Checkers/master/src/main/java/com/codingame/game/Referee.java` (lu intégralement).

### Constantes et plateau

| Fait | Valeur lue | URL |
|---|---|---|
| Plateau | 8×8, cases `.` `r` `R` `b` `B` | Referee.java + `config/statement_en.html` |
| Pièces initiales | **`b` aux lignes 5-7, `r` aux lignes 0-2, `(ligne+colonne)%2==1` → 12 pièces par camp** | Referee.java, `init()` |
| Écart avec l'énoncé | L'énoncé dit « each player has 16 pieces » → **incohérence** ; le code initialise 12 par camp (dames anglaises standard) | `config/statement_en.html` |
| `GAMETURN_LIMIT` | **300** coups ; `setMaxTurns(300+100)=400` frames | Referee.java |
| Type de jeu | `type=multi`, `min_players=2`, `max_players=2` | `https://raw.githubusercontent.com/linjoehan/Codingame_Checkers/master/config/config.ini` |

### Ordre de résolution (très important)

1. **`getMoves()` construit d'abord TOUS les sauts** (multi-sauts inclus) via un automate `j_move_status` (0 = à étendre, 1 = terminal légal, 2 = invalide). `add_jump()` interdit de re-capturer une case déjà capturée (`cap[][]`) mais **autorise le retour sur la case de départ**.
2. **Si au moins un saut existe, les coups simples ne sont pas générés du tout** → capture obligatoire.
3. Sinon, coups simples générés dans l'ordre : ligne `0→7`, colonne `0→7`, directions **haut-droite, haut-gauche, bas-droite, bas-gauche** (avec restriction de sens pour les pions).
4. **Ceci définit l'index 0 de la liste des coups légaux** — exploitables contre le boss (voir §3).
5. **Multi-saut** : `gameTurn()` insère des frames supplémentaires, n'affiche pas le joueur adverse (`setExpectedOutputLines(0)`) ; la sortie est une chaîne de coordonnées (`a3c5a7`).
6. **Couronnement pendant le saut** : dans `make_move()`, le pion devient dame dès qu'il atteint la dernière rangée, mais `add_jump()` avait figé les directions selon la pièce de départ → les directions arrière ne sont pas acquises pour le *reste du même multi-saut*. (Déduit de la lecture croisée `add_jump()`/`make_move()`.)

### Conditions de fin et départages

- **Adversaire sans coup** → `deactivate("No moves left for next player")`, fin de partie. C'est la vraie condition de victoire (pas « plus de pièces »).
- **`gameturn == 300`** → `endGame()` (limite de coups).
- **Coup illégal** → `deactivate("Move:… is not a legal move")`, fin.
- **Timeout** → `deactivate("Time limit exceeded!")`, fin.
- **Score final** : `onEnd()` → `setScore(actif ? 1 : 0)`. Un jeu arrêté à 300 coups avec deux joueurs actifs donne donc **1-1** (pas de départage fin). **[Déduit du code lu]**
- **Protocole** : 8 lignes plateau, 1 ligne couleur, 1 entier `N`, `N` coups ; sortie 1 ligne. **[attention]** Le referee envoie **`b` ou `w`** (`sendPlayerInputs`), alors que l'énoncé annonce `r`/`b`. Les deux bots publics gèrent `w` comme « rouge » — quirk confirmé par recoupement.
- **Temps** : l'énoncé dit « ≤ 50 seconds » (texte boilerplate), mais les bots top niveau utilisent **97–99 ms par coup** et **970 ms au premier coup** → le budget réel est ~100 ms. **[Conflit de sources ; le budget réel serveur n'est pas vérifié]**

---

## 2. Stratégies des meilleurs joueurs

### 2.1 VikSolve — 3ᵉ mondial, 2ᵉ Pologne (source la plus forte)
URL : `https://github.com/VikSolve/English-Draughts-3rd-Worldwide` + `https://raw.githubusercontent.com/VikSolve/English-Draughts-3rd-Worldwide/main/src.cpp` + README (tous lus).

- **Algorithme** : minimax négamax / alpha-beta, **iterative deepening** de profondeur 2→60, **table de transposition** de `1<<22` entrées (hash Zobrist-like multiplicatif), **killer moves** (2 par ply), **hash-move ordering**, génération de coups **bitboard** (4 × `uint32_t` : pions/dames rouges et noirs), recherche des multi-sauts récursive (`dfs_pom`).
- **Poids d'évaluation exacts (lu dans `dfs`, feuille `depth == MAX_DEPTH`)** :
  - Matériel : `pion = 1`, `dame = 2`, puis `<< 5` → **pion = 32, dame = 64** (dame = 2 pions).
  - +1 par pièce dans un masque de cases défini (`270336`), +2 par pion en zone de promotion (`2130706686`), +1 par pièce dans la bande centrale (`1044480`), +4 si les deux cases d coin sont occupées (`0xA` noir / `1342177280` rouge).
  - Terme « **triangles** » (structures défensives, masques `0x00077777`, `0x000EEEEE`, etc.) et « **wyrównanie** » (équilibrage, masques `0x3333377F`, `0x77333333`, `0xFEECCCCC`, `0xCCCCCCEE`), **multipliés par 3**.
  - Score final : `res = black - red + triangles*3`, puis amortissement « agresja » : `res - 8*round(res/64)`.
  - Scores de mat : `±10000 - depth`.
- **Temps** : `timeLimit = firstMove ? 970000 : 97000` µs ; check toutes les 128 visites.
- **Pourquoi ça marche** : le README précise que CodinGame compile en **-O0** → il compense avec `#pragma GCC optimize "Ofast,unroll-loops,omit-frame-pointer,inline"` et `target("popcnt")`. Aucun réseau de neurones ; objectif déclaré : battre les bots NN. TODO : **book d'ouvertures**.

### 2.2 Watashiku — bot C++ public (2 étoiles)
URL : `https://raw.githubusercontent.com/Watashiku/checkers/master/main.cpp` (lu).

- Alpha-beta + **progressive deepening**, budget **99 ms**, `INFINITY=100000`, **`MAN_VALUE=100`, `KING_VALUE=150`**.
- Évaluation : matériel, **normalisé par le matériel restant**, plus un terme de **centralisation des dames en fin de partie** (`materialLeft < 15` : `-max(|2r-7|,|2c-7|)`).
- Égalité de score → **mélange aléatoire** des meilleurs coups (anti-déterminisme).
- Gère le quirk `w → rouge`.
- **Pourquoi ça marche** : robuste et simple ; le terme de centralisation des dames est un vrai gain en finale de dames anglaises.

### 2.3 Le boss de la ligue — faiblesse critique
URL : `https://raw.githubusercontent.com/linjoehan/Codingame_Checkers/master/config/Boss.java` (lu).

```java
System.out.println(Moves.get(0));   // joue TOUJOURS le 1er coup légal
```

C'est un bot **déterministe et naïf**. Couplé à l'ordre de `getMoves()` du referee, son premier coup depuis la position initiale est prévisible (déduit du code, **non observé en match**) : noir → `a3b4`, rouge → `b8c7`.

### 2.4 Autres bots publics (bas niveau,-only baseline)
- `https://raw.githubusercontent.com/romulopro/Checkers_from_Codingame/master/checkers.py` (lu) : choisit le **plus long saut** sinon un coup aléatoire, aucune recherche. Sert de référence « plancher ».
- `https://github.com/Watashiku/checkers` (lu).

### 2.5 Post-mortems du forum CodinGame
**[NON VÉRIFIÉ]** — Je n'ai **aucun post-mortem du forum CodinGame lu**. Le site est rendu en JavaScript (`webfetch` → « Loading... »), les endpoints `.json` du forum renvoient 404, et `websearch` a échoué (HTTP 402). Aucune affirmation sur les post-mortems des meilleurs joueurs ne peut être avancée.

### 2.6 Articles académiques
- arXiv (résumé lu) : **« Losing at Checkers is Hard »**, Bosboom, Congero, Demaine, Demaine, Lynch — `https://arxiv.org/abs/1806.05657` — complexité : décider un coup forçant la défaite en 1 est NP-complet ; les variantes à saut obligatoire sont PSPACE-complet. Intérêt : confirme que le **saut obligatoire** rend la recherche de coups piégeuse, donc qu'une évaluation statique seule est insuffisante.
- arXiv (résumé lu) : **« Transfinite game values in infinite draughts »**, Hamkins & Leonessi — `https://arxiv.org/abs/2111.02053` (distance au jeu du CodinGame, référence théorique seulement).
- **[NON VÉRIFIÉ]** Semantic Scholar a renvoyé **429** sur toutes les requêtes ; aucun article S2 lu. Les recherches arXiv ciblées (`draughts + alpha-beta`, `English draughts + artificial intelligence`) ont retourné **0 résultat** pertinent.

---

## 3. Recommandation concrète et testable pour battre le boss / monter au classement

### A. Battre le boss du ligue — test immédiat (haut rendement, faible effort)

1. **Reproduire localement** l'ordre de `getMoves()` (sauts d'abord, puis coups simples ligne/colonne croissants, directions HD/HG/BD/BG) et prend `list[0]`.
2. Vérifier sur 100 parties locales que votre bot bat ce « Boss simulé ». Un alpha-beta même à **profondeur 2** avec l'évaluation 100/150 doit gagner ~100 % : le boss ne défend pas ses pièces.
3. **Pièges à tester explicitement** avant soumission : (a) couleur `w` = rouge ; (b) sortie multi-saut complète `a3c5a7` ; (c) coup illégal = défaite immédiate.

### B. Grimper au classement — feuille de route A/B mesurable

| Étape | Action | Gain attendu | Test |
|---|---|---|---|
| 1 | Parser le plateau + **utiliser la liste de coups légaux fournie** à la racine (évite les bugs de génération) | Fiabilité | 0 coup illégal sur 100 matchs |
| 2 | Alpha-beta + iterative deepening, évaluation `pion=100, dame=150`, normalisée, **+ centralisation des dames en finale** (`matériel<15`) | Socle | Win-rate vs bot aléatoire > 95 % |
| 3 | Budget **97 ms** (970 ms au 1ᵉʳ coup),Arrêt propre de l'itération | Profondeur exploitable | Profondeur atteinte ≥ 6 en milieu de jeu |
| 4 | Ajouter **table de transposition + killer moves + hash-move ordering** | +2 à +4 plis | A/B : même position, comparer profondeur atteinte et win-rate |
| 5 | Passer en **bitboard 4×uint32_t** (si C++) avec `#pragma Ofast/ popcnt` pour compenser le -O0 | Vitesse | Mesurer coups/s |
| 6 | **Book d'ouvertures** — c'est le TODO explicite du bot n°3 mondial | Évite les pièges d'ouverture | 20 premières parties du classement |

### C. Différenciateur testable contre les bots NN
Le bot n°3 mondial prouve qu'un moteur **sans réseau de neurones** peut tenir le top 3. Le levier le plus rentable et le **moins exploré** est donc le **book d'ouvertures** (TODO de VikSolve) combiné au temps élargi du premier coup (970 ms). Test : mesurer le win-rate avec/sans book sur 200 parties en debut position, en gardant tout le reste identique.

---

## 4. Liste de toutes les URL lues

1. https://api.github.com/search/repositories?q=checkers+codingame+referee
2. https://api.github.com/search/repositories?q=referee+checkers+in:name
3. https://api.github.com/search/repositories?q=org:CodinGame+checkers
4. https://api.github.com/search/repositories?q=codingame+checkers+bot+in:name,description
5. https://api.github.com/search/repositories?q=checkers+codingame
6. https://api.github.com/search/repositories?q=draughts+codingame+OR+checkers+bot+in:description
7. https://api.github.com/search/repositories?q=%22English+Draughts%22+codingame+bot
8. https://api.github.com/search/repositories?q=user:eulerscheZahl+checkers
9. https://api.github.com/search/repositories?q=codingame+checkers+alpha-beta+OR+negamax+OR+bitboard
10. https://github.com/linjoehan/Codingame_Checkers
11. https://github.com/linjoehan/Codingame_Checkers/tree/master/game-checkers-master
12. https://github.com/linjoehan/Codingame_Checkers/tree/master/src
13. https://github.com/linjoehan/Codingame_Checkers/tree/master/src/main
14. https://github.com/linjoehan/Codingame_Checkers/tree/master/src/main/java/com/codingame/game
15. https://github.com/linjoehan/Codingame_Checkers/tree/master/config
16. https://github.com/linjoehan/Codingame_Checkers/tree/master/game-checkers-master/src/main
17. https://github.com/linjoehan/Codingame_Checkers/tree/master/game-checkers-master/src
18. https://raw.githubusercontent.com/linjoehan/Codingame_Checkers/master/src/main/java/com/codingame/game/Referee.java
19. https://raw.githubusercontent.com/linjoehan/Codingame_Checkers/master/config/Boss.java
20. https://raw.githubusercontent.com/linjoehan/Codingame_Checkers/master/config/config.ini
21. https://raw.githubusercontent.com/linjoehan/Codingame_Checkers/master/config/statement_en.html
22. https://raw.githubusercontent.com/linjoehan/Codingame_Checkers/master/config/stub.txt
23. https://api.github.com/repos/linjoehan/Codingame_Checkers/issues
24. https://github.com/SteamDwarf/CheckersReferee
25. https://github.com/SteamDwarf/CheckersReferee/tree/main/server
26. https://github.com/SteamDwarf/CheckersReferee/tree/main/server/src
27. https://github.com/eulerscheZahl/RefereeCollection
28. https://github.com/SpiritusSancti5/codinGame/tree/master/Referees
29. https://github.com/Watashiku/checkers
30. https://raw.githubusercontent.com/Watashiku/checkers/master/main.cpp
31. https://github.com/romulopro/Checkers_from_Codingame
32. https://raw.githubusercontent.com/romulopro/Checkers_from_Codingame/master/checkers.py
33. https://github.com/VikSolve/English-Draughts-3rd-Worldwide
34. https://raw.githubusercontent.com/VikSolve/English-Draughts-3rd-Worldwide/main/README.md
35. https://raw.githubusercontent.com/VikSolve/English-Draughts-3rd-Worldwide/main/src.cpp
36. https://www.codingame.com/multiplayer/bot-programming/checkers (→ « Loading... », non lisible)
37. https://www.codingame.com/forum/search?q=checkers (→ « Loading... », non lisible)
38. https://www.codingame.com/forum/l/latest.json (404)
39. https://www.codingame.com/forum/search.json?q=checkers (404)
40. https://export.arxiv.org/api/query?... (plusieurs requêtes ; pertinentes : `abs/1806.05657`, `abs/2111.02053`)
41. https://api.semanticscholar.org/graph/v1/paper/search... (429 — **non lu**)

**Non vérifié / non trouvé** : referee officiel serveur, post-mortems du forum CodinGame, articles Semantic Scholar, budget temps réel réel (50 s annoncé vs ~100 ms déduit des bots).
