<!-- opencode model: hy4-flash-free, 2026-11-23T19:19:27+03:00 -->

> build · hy4-flash-free
Je vais lancer une recherche structurée sur le jeu Breakthrough (CodinGame), en respectant vos contraintes : aucun fichier modifié, aucun build, aucune lecture de `~/.config/codingame`. Je commence par le referee officiel, puis les bots publics et les articles.
✗ Web Search via OpenCode ""Programming Breakthrough" Lorentz Horey Saffidine article breakthrough board game AI" failed
Error: Web search request failed (HTTP 402)
✗ Web Search via OpenCode "CodinGame Breakthrough bot programming referee rules github" failed
Error: Web search request failed (HTTP 402)
> build · hy4-flash-free
La recherche web est indisponible (HTTP 402). Je bascule sur l'API GitHub et les URLs connues directement.
✗ WebFetch https://api.github.com/repos/CodinGame/Breakthrough failed
Error: StatusCode: non 2xx status code (404 GET https://api.github.com/repos/CodinGame/Breakthrough)
% WebFetch https://api.github.com/repos/eulerscheZahl/RefereeCollection/contents 
% WebFetch https://api.github.com/search/repositories?q=breakthrough+codingame 
> build · hy4-flash-free
Le referee n'est pas dans RefereeCollection. Je explore les bots publics trouvés et cherche d'autres dépôts.
% WebFetch https://api.github.com/repos/muy31/Breakthrough/contents 
% WebFetch https://api.github.com/repos/Trollkirsch3/breakthroughBot/contents 
% WebFetch https://api.github.com/repos/chelsea0x3b/breakthrough-mcts/contents 
% WebFetch https://api.github.com/search/repositories?q=breakthrough+codingame+bot&sort=stars 
> build · hy4-flash-free
Je lis maintenant le code source des bots publics et cherche le referee/énoncé officiel.
✗ WebFetch https://api.github.com/search/code?q=breakthrough+referee+codingame failed
Error: StatusCode: non 2xx status code (401 GET https://api.github.com/search/code?q=breakthrough+referee+codingame)
% WebFetch https://raw.githubusercontent.com/muy31/Breakthrough/main/README.md 
% WebFetch https://raw.githubusercontent.com/muy31/Breakthrough/main/Player.java 
% WebFetch https://api.github.com/repos/chelsea0x3b/breakthrough-mcts/contents/src 
% WebFetch https://api.github.com/repos/Trollkirsch3/breakthroughBot/contents/src 
> build · hy4-flash-free
% WebFetch https://raw.githubusercontent.com/chelsea0x3b/breakthrough-mcts/master/src/env_bitboard.rs 
% WebFetch https://raw.githubusercontent.com/chelsea0x3b/breakthrough-mcts/master/src/main.rs 
% WebFetch https://www.codingame.com/multiplayer/bot-programming/breakthrough 
% WebFetch https://raw.githubusercontent.com/chelsea0x3b/breakthrough-mcts/master/src/mcts.rs 
> build · hy4-flash-free
% WebFetch https://api.github.com/search/repositories?q=breakthrough+referee+game 
% WebFetch https://api.github.com/search/repositories?q=user:eulerscheZahl+breakthrough 
% WebFetch https://api.github.com/search/repositories?q=org:CodinGame+breakthrough 
✗ WebFetch https://api.semanticscholar.org/graph/v1/paper/search?query=Programming%20Breakthrough&limit=20&fields=title,authors,year,url,abstract failed
Error: StatusCode: non 2xx status code (429 GET https://api.semanticscholar.org/graph/v1/paper/search?query=Programming%20Breakthrough&limit=20&fields=title,authors,year,url,abstract)
% WebFetch https://export.arxiv.org/api/query?search_query=all:breakthrough+AND+all:game&start=0&max_results=20 
> build · hy4-flash-free
% WebFetch https://export.arxiv.org/api/query?search_query=ti:breakthrough&start=0&max_results=50 
% WebFetch https://github.com/search?q=%22breakthrough%22+%22codingame%22&type=code 
% WebFetch https://api.github.com/search/repositories?q=breakthrough+in:name&sort=stars&order=desc&per_page=50 
> build · hy4-flash-free
% WebFetch https://api.github.com/repos/Trollkirsch3/breakthroughBot/contents/src/Main.java 
✗ WebFetch https://api.semanticscholar.org/graph/v1/paper/search?query=breakthrough%20board%20game%20alpha-beta&limit=10&fields=title,authors,year,url,abstract failed
Error: StatusCode: non 2xx status code (429 GET https://api.semanticscholar.org/graph/v1/paper/search?query=breakthrough%20board%20game%20alpha-beta&limit=10&fields=title,authors,year,url,abstract)
% WebFetch https://api.github.com/search/repositories?q=breakthrough+codingame+in:readme&per_page=100 
> build · hy4-flash-free
$ grep -o '"full_name":"[^"]*"' /home/arafaram/.local/share/opencode/tool-output/tool_20931b34b001vCoayocApVnVy3 | sort -u
"full_name":"2000zxy/rgzn"
"full_name":"ArgoFlash/summer2025-selfplay-codingames"
"full_name":"bogdanmosica/internet-tools-collection"
"full_name":"CodinGame/LegendsOfCodeAndMagic"
"full_name":"cuteboy0323/A-to-Z-Resources-for-Students"
"full_name":"EnigmaVSSUT/Learning-Materials"
"full_name":"Ernest-NamHee-FE/A-To-Z-Ressources"
"full_name":"ethicaldesigns/crispy-octo-disco"
"full_name":"kitsunecommune/LegendsOfCodingAndMagic"
"full_name":"koleharv/napkin-100k-tictac"
"full_name":"msshahari/A-to-Z-Resources-for-Students"
"full_name":"muy31/Breakthrough"
"full_name":"nikitinalexx/LegendsOfCodeAndMagic"
"full_name":"oskardudycz/ArchitectureWeekly"
"full_name":"pb4git/Nash-DQN-CSB-Article"
"full_name":"sandysanthosh/code-A-TO-Z-for-students"
"full_name":"Telokis/cg-selfarena"
"full_name":"YahyaOukharta/breakthrough"
"full_name":"yassine-nid/Summer-Challenge-2025-Post-Mortem"
"full_name":"ZenithGGSIPU/Awesome_Resources"
"full_name":"Zweer/codingame"
% WebFetch https://raw.githubusercontent.com/Trollkirsch3/breakthroughBot/master/src/Main.java 
% WebFetch https://api.github.com/repos/YahyaOukharta/breakthrough/contents 
% WebFetch https://api.github.com/orgs/CodinGame/repos?per_page=100 
> build · hy4-flash-free
$ grep -o '"full_name":"[^"]*"' /home/arafaram/.local/share/opencode/tool-output/tool_2093243060015x3QGhYddJyvJz | sort -u
"full_name":"CodinGame/a-code-of-ice-and-fire"
"full_name":"CodinGame/advanced-python-features-playground"
"full_name":"CodinGame/angularjs-assessment-runner"
"full_name":"CodinGame/artillery"
"full_name":"CodinGame/CG-Dispatch-Tests"
"full_name":"CodinGame/Code4Life"
"full_name":"CodinGame/coders-of-the-caribbean"
"full_name":"CodinGame/codingame-game-engine"
"full_name":"CodinGame/codingame-sdk-anim-module"
"full_name":"CodinGame/codingame-sdk-assets"
"full_name":"CodinGame/codingame-sdk-doc"
"full_name":"CodinGame/codingame-sdk-playground"
"full_name":"CodinGame/commitlint-config-codingame"
"full_name":"CodinGame/Contribute-Help"
"full_name":"CodinGame/cypress-real-events"
"full_name":"CodinGame/DefinitelyTyped"
"full_name":"CodinGame/docker-client"
"full_name":"CodinGame/dropbear"
"full_name":"CodinGame/esbuild-import-meta-url-plugin"
"full_name":"CodinGame/FallChallenge2020"
"full_name":"CodinGame/FallChallenge2022-KeepOffTheGrass"
"full_name":"CodinGame/FallChallenge2023-SeabedSecurity"
"full_name":"CodinGame/FallChallenge2024-SeleniaCity"
"full_name":"CodinGame/Fuse"
"full_name":"CodinGame/game-fishy-adventures"
"full_name":"CodinGame/gameofdrones"
"full_name":"CodinGame/game-pong"
"full_name":"CodinGame/game-skeleton"
"full_name":"CodinGame/game-tictactoe"
"full_name":"CodinGame/game-ultimate-tictactoe"
"full_name":"CodinGame/ghost-in-the-cell"
"full_name":"CodinGame/h2go"
"full_name":"CodinGame/hydra"
"full_name":"CodinGame/javascript"
"full_name":"CodinGame/java-server-sdk-redis"
"full_name":"CodinGame/json-rpc-2.0"
"full_name":"CodinGame/json-schema-l10n"
"full_name":"CodinGame/kotlin"
"full_name":"CodinGame/kubevirt-api"
"full_name":"CodinGame/languageserver-mutualized"
"full_name":"CodinGame/LegendsOfCodeAndMagic"
"full_name":"CodinGame/material-ui"
"full_name":"CodinGame/MeanMax"
"full_name":"CodinGame/monaco-collab-ext"
"full_name":"CodinGame/monaco-editor"
"full_name":"CodinGame/monaco-editor-react"
"full_name":"CodinGame/monaco-editor-wrapper"
"full_name":"CodinGame/monaco-emacs"
"full_name":"CodinGame/monaco-jsonrpc"
"full_name":"CodinGame/monaco-languageclient-react"
"full_name":"CodinGame/monaco-languageclient-wrapper"
"full_name":"CodinGame/monaco-vscode-api"
"full_name":"CodinGame/node-config"
"full_name":"CodinGame/nx"
"full_name":"CodinGame/ory-sdk"
"full_name":"CodinGame/redis-semaphore"
"full_name":"CodinGame/rollup-import-meta-assets-plugin"
"full_name":"CodinGame/semantic-release-config"
"full_name":"CodinGame/semantic-release-config-github"
"full_name":"CodinGame/semantic-release-rules"
"full_name":"CodinGame/SpringChallenge2020"
"full_name":"CodinGame/SpringChallenge2021"
"full_name":"CodinGame/SpringChallenge2022"
"full_name":"CodinGame/SpringChallenge2023"
"full_name":"CodinGame/sqls"
"full_name":"CodinGame/SummerChallenge2024Olymbits"
"full_name":"CodinGame/SummerChallenge2025-SoakOverflow"
"full_name":"CodinGame/svlangserver"
"full_name":"CodinGame/thenBy.js"
"full_name":"CodinGame/topics"
"full_name":"CodinGame/tsconfig"
"full_name":"CodinGame/typescript-language-server"
"full_name":"CodinGame/use-resize-observer"
"full_name":"CodinGame/vite"
"full_name":"CodinGame/vscode"
"full_name":"CodinGame/vscode-focus-conflict-demo"
"full_name":"CodinGame/vscode-languagedetection"
"full_name":"CodinGame/vscode-loc"
"full_name":"CodinGame/vscode-textmate"
"full_name":"CodinGame/web"
"full_name":"CodinGame/WinterChallenge2024-Cellularena"
"full_name":"CodinGame/WinterChallenge2026-Exotec"
"full_name":"CodinGame/WondevWoman"
"full_name":"CodinGame/xterm.js"
% WebFetch https://raw.githubusercontent.com/YahyaOukharta/breakthrough/main/README.md 
% WebFetch https://api.github.com/repos/YahyaOukharta/breakthrough/contents/docs 
> build · hy4-flash-free
J'ai trouvé le referee officiel (`darkhorse64/CG_Breakthrough`) et un bot classé 1er avec documentation détaillée. Je les lis en parallèle.
% WebFetch https://raw.githubusercontent.com/YahyaOukharta/breakthrough/main/docs/resources.md 
% WebFetch https://raw.githubusercontent.com/YahyaOukharta/breakthrough/main/docs/search.md 
% WebFetch https://raw.githubusercontent.com/YahyaOukharta/breakthrough/main/docs/engine.md 
% WebFetch https://api.github.com/repos/darkhorse64/CG_Breakthrough/contents 
> build · hy4-flash-free
% WebFetch https://raw.githubusercontent.com/YahyaOukharta/breakthrough/main/docs/nnue.md 
% WebFetch https://raw.githubusercontent.com/YahyaOukharta/breakthrough/main/docs/history.md 
% WebFetch https://api.github.com/repos/darkhorse64/CG_Breakthrough/git/trees/master?recursive=1 
% WebFetch https://forum.codingame.com/t/community-puzzle-breakthrough/137971 
> build · hy4-flash-free
% WebFetch https://raw.githubusercontent.com/darkhorse64/CG_Breakthrough/master/src/main/java/com/codingame/game/Referee.java 
% WebFetch https://raw.githubusercontent.com/darkhorse64/CG_Breakthrough/master/config/config.ini 
% WebFetch https://raw.githubusercontent.com/darkhorse64/CG_Breakthrough/master/config/statement_en.html 
% WebFetch https://raw.githubusercontent.com/darkhorse64/CG_Breakthrough/master/config/Boss.java 
> build · hy4-flash-free
% WebFetch https://arxiv.org/abs/1406.0486 
✗ WebFetch https://www.researchgate.net/publication/290565002_Programming_Breakthrough failed
Error: StatusCode: non 2xx status code (403 GET https://www.researchgate.net/publication/290565002_Programming_Breakthrough)
✗ WebFetch https://www.lamsade.dauphine.fr/~cazenave/papers/solving.pdf failed
Error: Unsupported fetched file content type: application/pdf
% WebFetch https://arxiv.org/abs/2012.10700 
% WebFetch https://link.springer.com/chapter/10.1007/978-3-319-71649-7_12 
> build · hy4-flash-free
% WebFetch https://api.github.com/repos/MartinMSPedersen/Breakthrough-Web/contents 
% WebFetch https://api.github.com/repos/Pehz63/Breakthrough/contents 
% WebFetch https://api.semanticscholar.org/graph/v1/paper/DOI:10.1007/978-3-319-09165-5_5?fields=title,abstract,authors,year,url,openAccessPdf 
% WebFetch https://link.springer.com/chapter/10.1007/978-3-319-09165-5_5 
> build · hy4-flash-free
% WebFetch https://raw.githubusercontent.com/MartinMSPedersen/Breakthrough-Web/main/ARCHITECTURE.md 
% WebFetch https://raw.githubusercontent.com/Pehz63/Breakthrough/main/minimax_params.txt 
% WebFetch https://export.arxiv.org/api/query?search_query=ti:%22Solving%20Breakthrough%22&start=0&max_results=10 
% WebFetch https://raw.githubusercontent.com/Pehz63/Breakthrough/main/README.md 
> build · hy4-flash-free
% WebFetch https://raw.githubusercontent.com/YahyaOukharta/breakthrough/main/docs/workflow.md 
% WebFetch https://raw.githubusercontent.com/YahyaOukharta/breakthrough/main/versions/v1.1.cpp 
% WebFetch https://raw.githubusercontent.com/MartinMSPedersen/Breakthrough-Web/main/src/engine/evaluator.ts 
% WebFetch https://r.jina.ai/https://www.lamsade.dauphine.fr/~cazenave/papers/solving.pdf 
> build · hy4-flash-free
# Recherche — Breakthrough 8×8 (CodinGame)

**Méthode.** `websearch` était indisponible (HTTP 402) ; toutes les sources ci‑dessous ont été lues via `webfetch` / API GitHub / `raw.githubusercontent.com` / arXiv / Springer / Semantic Scholar. Aucun fichier du dépôt n'a été modifié, aucun build lancé, `~/.config/codingame` non lu.

> ⚠️ Le referee **n'est ni** dans `github.com/CodinGame/Breakthrough` (404 vérifié) **ni** dans `eulerscheZahl/RefereeCollection` (listage vérifié, aucun dossier Breakthrough). Le referee public est **`darkhorse64/CG_Breakthrough`**, référencé comme tel par le bot classé 1er.

---

## 1) Faits du referee (sources officielles lues)

Source : `https://github.com/darkhorse64/CG_Breakthrough/blob/master/src/main/java/com/codingame/game/Referee.java` — énoncé : `.../blob/master/config/statement_en.html` — config : `.../blob/master/config/config.ini`.

| Fait | Valeur exacte | URL |
|---|---|---|
| Plateau / pièces | 8×8, 16 pièces par camp. Blanc (`w`) lignes 6‑7 (rangs 1‑2), noir (`b`) lignes 0‑1 (rangs 8‑7). Blanc commence (`gameTurn = 0` → `p1turn = true`). | Referee.java `init()` |
| Déplacement | Un pion avance **d'une case** vers la rangée adverse : tout droit si la case est `.` ; en diagonale si la cible n'est pas une pièce alliée (case vide **ou** pièce ennemie capturée). Capture non obligatoire, non chaînée. Le referee n'engendre que les coups « vers l'avant » pour chaque couleur. | Referee.java `getMoves()` ; statement_en.html |
| Codification | `a2a3` = `[colonne rangée][colonne rangée]` ; rangée affichée = `8 - row` interne. | `make_move()`, `getMoves()` |
| **Ordre d'énumération des coups légaux** | Boucle `row = 0..7`, `col = 0..7` ; pour chaque pion actif : **diagonale droite, tout droit, diagonale gauche** (version « up » pour le blanc, « down » pour le noir). Cet ordre est utilisé tel quel pour le `legal[0]` de secours. | `getMoves()` |
| **Ordre de résolution d'un tour** | 1) `sendPlayerInputs` (`lastMove`, `N`, puis `N` coups) → 2) `player.execute()` → 3) lecture du **premier** ligne de sortie, regex `^(?<move>\w+)(\s+(?<message>.+))?` → 4) test d'égalité stricte avec la liste `getMoves()` → 5) `make_move` + `lastMove = move` → 6) `gameTurn++` → 7) `hasWinner()` → 8) `endGame()` si gagnant, sinon tour suivant. Message optionnel tronqué à 30 caractères. | `gameTurn(int turn)` |
| Temps | `setTurnMaxTime(100)` ; `setFirstTurnMaxTime(1000)`. Dépassement → `deactivate("Time limit exceeded!")` → défaite. Coup illégal / sortie invalide → `deactivate` → défaite. | `init()`, `gameTurn()` |
| Limite | `setMaxTurns(209)` ; l'énoncé dit « jusqu'à 209 coups théoriquement, généralement < 100 en pratique ». | `init()`, statement_en.html |
| Victoire | Atteindre **la première** la rangée adverse (`board[0]` pour le blanc, `board[7]` pour le noir) **ou** capturer toutes les pièces adverses. `hasWinner()` teste d'abord la rangée, puis l'absence de pièces ennemies. | `hasWinner()`, statement_en.html |
| Score final | `onEnd()` : `score = 1` si joueur encore actif, `0` sinon. | `onEnd()` |

**Départages [NON VÉRIFIÉ]** : le referee ne contient aucun départage explicite. Si la limite de 209 tours est atteinte sans gagnant, les deux joueurs restent actifs donc `onEnd()` donne `1‑1` (inference directe du code, non exécutée). Le mode dont la ligue CodinGame classe une telle partie (égalité, score relatif, etc.) n'a **pas** été vérifié. Noter aussi que `hasWinner()` appelle `next_player.deactivate()` (le joueur suivant) avant `endGame()` ; c'est cohérent avec `onEnd()` mais inhabituel — non vérifié en exécution.

---

## 2) Stratégies des meilleurs joueurs / bots publics

### 2.1 Bot n°1 déclaré — `YahyaOukharta/breakthrough` (C++, 2026‑11)
URL : `https://github.com/YahyaOukharta/breakthrough` — README : `.../blob/main/README.md` — docs : `docs/search.md`, `docs/nnue.md`, `docs/engine.md`, `docs/history.md`, `docs/workflow.md`.

- **Résultat déclaré par l'auteur** : **rang 1/381** le 2026‑11‑13 (`yhyoxx`), score arène 44,25 (v3.0) puis 44,23 (v3.1). *[NON VÉRIFIÉ indépendamment : nous n'avons pas interrogé le classement live.]*
- **Recherche** : alpha‑beta **PVS** itératif, TT 2 M entrées, quiescence captures + 2 plies d'évasions, LMR, futility (`FUT=60`), reverse futility (`RFP=60`, profondeur ≤7), aspiration (`ASP=30`), **singular extensions** (`SE=2`, `SE_DEPTH=6`, **+95 Elo**), IIR/LMP/improving/history (**+107 Elo**), ordonnancement TT → captures (plus proches de la maison, non défendues d'abord) → killers → history. Profondeurs 11–16 en 95 ms, ~4,0 M nœuds/s.
- **Évaluation artisanale (v1.1, code lu)** : `P[0..6] = {100, 100, 118, 142, 180, 240, 340}` (valeur d'un pion selon le rang, du rang 1 au rang 7), `P[7] = 6` bonus par pion **protégé** (pion allié en diagonale arrière), `P[8] = 12` pénalité par pion **attaqué et non protégé** ; `evaluate = sideScore(own,opp) − sideScore(flip(opp),flip(own))`. `WIN = 30000 − ply`.
- **Règles tactiques exactes** : pion sur le rang 7 = gain ; pion ennemi sur le rang 2 qui ne peut être capturé (ou deux) = perte ; un seul → capture forcée ; `safe7` (montée sur rang 7 non capturable) = gain en 3 plies.
- **Évaluation finale (v3.x)** : NNUE 128 entrées (pion allié case + pion ennemi case), 384→**640** cachés *mirror‑tied*, SCReLU, 4 têtes de sortie par nombre de pions, int16 + AVX2, entraînement self‑play 166 M positions, cible `0,6·résultat + 0,4·sigmoid(score/200)`.
- **Pourquoi ça marche** : le jeu est **tactique et zugzwang‑lourd** → l'alpha‑beta avec règles exactes bat la recherche générique ; la nette supériorité du NNUE sur l'éval artisanale est documentée (94 % de victoires au premier net, +506/+274 Elo sur les premières générations). **Échecs mesurés** : null‑move **−35** (zugzwang), race rule **−9/−10**, best‑first minimax avec UCT **−9…−23**, double/singular négative **−56/−6**.

### 2.2 MCTS UCT avec évaluation riche — `Trollkirsch3/breakthroughBot` (Java)
URL : `https://github.com/Trollkirsch3/breakthroughBot/blob/master/src/Main.java` (lu en entier).

- MCTS + **UCT** `eval/visits + 0,5·√(2·ln(visits_parent)/visits) + 0,2·histoire/(visits+1)`, réutilisation du sous‑arbre par transposition, 900 ms au 1er coup puis 80 ms.
- **Évaluation (termes concrets, lus dans `Game.EVAL`)** : matériel `10·(pions_blancs − pions_noirs)` ; chaîne protégée `+1` ; pions ennemis **attaqués et non protégés** `+2` ; pions protégés `+1` ; centre **uniquement si protégé** `+0,5` ; avancement quadratique `+0,6·Σ max(0, rang−1)²`. Gain/perte `±1 000 000`. Les termes « home row / backrow » sont **présents mais commentés** (donc non utilisés).
- Rollout profondeur 25 avec détection de gain immédiat, sélection biaisée : gains immédiats > avancées sûres vers l'avant‑dernière rangée (80 %) > captures (80 %) > coups aléatoires ; retour `tanh(EVAL/50)`.
- Filtre de sécurité à la racine : élimine les coups laissant un gain immédiat à l'adversaire, sauf si aucun coup sûr n'existe.
- **Pourquoi ça marche** : UCT + heuristique de rollout + historique donnent une_policy très orientée « course » dans un jeu où les gains sont souvent des coups **calmes** vers la dernière rangée.

### 2.3 MCTS minimaliste — `chelsea0x3b/breakthrough-mcts` (Rust, 2020)
URLs : `src/main.rs`, `src/mcts.rs`, `src/env_bitboard.rs` (lus).

- UCT `reward/n + √(2·log₂(N_parent)/n)`, expansion de **tous** les enfants en une fois, rollout **uniformément aléatoire jusqu'à la fin**, récompense **±1 uniquement** (aucune évaluation positionnelle).
- Astuce notable dans `action_bitboards()` : les coups engendrés sont filtrés par priorité **gain immédiat > coup bloquant un gain adverse > tous les coups** — l'ordre affecte directement le choix UCT et les rollouts.
- ⚠️ `main()` appelle `local_main()` ; `codingame_main()` est **commenté** : le dépôt tel qu'il est n'est donc pas une soumission prête. *[recherche locale comparant MCTS vs FrozenMCTS, 300 parties.]*

### 2.4 Alpha‑beta adaptatif simple — `muy31/Breakthrough` (Java, 2019/2020)
URL : `https://github.com/muy31/Breakthrough/blob/main/Player.java` (lu).

- Profondeur adaptative : 4 si ≤5 coups légaux, 3 si ≤20, sinon 2 ; alpha‑beta avec `±100 − depth` sur gain/perte.
- Évaluation : `mscore − escore` (matériel 1/pion) **plus** bonus/malus `±50` pour un pion sur la ligne 7 (avant‑dernière) avec chemin dégagé vers la maison, `±25` sur la ligne 6. Égalité → choix aléatoire parmi les meilleurs coups.

### 2.5 Autres implémentations publiques (termes d'évaluation utiles, non soumissions CodinGame)
- `MartinMSPedersen/Breakthrough-Web` — `src/engine/evaluator.ts` : poids d'avancement **`[25,22,23,27,41,58,127,1000]`**, bonus défenseur = pion allié en diagonale arrière (`defenderScale`, 0 par défaut) ; `WIN_SCORE=100_000`. Recherche negamax/αβ + ID + TT + PVS + quiescence étendant aussi les « winning pushes » (`ARCHITECTURE.md` lu).
- `Pehz63/Breakthrough` — `minimax_params.txt` : `chip=1`, `wall=0`, `column=0`, `turn=0`, profondeur 9. `README.md` décrit les évaluateurs **Classic** (turn/chip/wall/column), **Experimental** (+ avancement), **Advanced** (+ support, center, mobility, hole, control, open, race, overext, noise, RaceWin), **LearnedValue**, et un explorateur **GumbelMCTS**. Hill‑climbing de 13 poids à Elo constant.

### 2.6 Articles / post‑mortems

| Source | Ce qui est vérifié | URL |
|---|---|---|
| **Lorentz & Horey, « Programming Breakthrough »**, CG 2013, LNCS 8427, pp. 49‑59 (2014) | Résumé Springer lu : programme **MCTS**, « ajustements appropriés », bat la plupart des humains ; stratégie subtile puis tactique profonde (séquences de 20+ coups). **Mots‑clés : MCTS, Breakthrough, Evaluation function.** Les **termes chiffrés de la fonction d'évaluation sont payants → [NON VÉRIFIÉ]**. | `https://link.springer.com/chapter/10.1007/978-3-319-09165-5_5` |
| **Lorentz & Zosa, « Machine Learning in the Game of Breakthrough »**, ACG 2017, LNCS 10664, pp. 140‑150 | Résumé lu : TD learning en MCTS ≈ *Wanderer* (éval très ajustée) ; CNN entraîné sur les coups experts de Wanderer > Wanderer. | `https://link.springer.com/chapter/10.1007/978-3-319-71649-7_12` |
| **Saffidine, Jouandeau & Cazenave, « Solving Breakthrough with Race Patterns and Job‑Level PNS »** (2012) | **PDF intégral lu** : *race patterns* = motifs statiques détectant un gain jusqu'à **7 coups** avant la fin (26 motifs écrits à la main, ordre ≤2, pru­nage ~4 coups avant la fin) ; **mobility heuristic** ; PPN² (JLPNS + PN² distribué) ; 6×5 résolu en 7 h 7 (second joueur gagne) ; **zugzwang = limite**. 8×8 **non résolu**. | `https://www.lamsade.dauphine.fr/~cazenave/papers/solving.pdf` (texte via `https://r.jina.ai/...`) |
| **Lanctot, Winands, Pepels, Sturtevant**, « MCTS with heuristic evaluations using implicit minimax backups », arXiv:1406.0486 | Résumé lu : sépare taux de victoire et éval heuristique, backup minimax implicite ; gains sur Kalah, **Breakthrough**, Lines of Action. | `https://arxiv.org/abs/1406.0486` |
| **Cohen‑Solal & Cazenave, « Minimax Strikes Back »**, arXiv:2012.10700 | Résumé lu : Athénan/Descent (minimax + réseau de valeurs, sans politique), bien plus efficient que Polygames. Implication pour Breakthrough **[NON VÉRIFIÉ]** (pas de déclaration explicite dans le résumé). | `https://arxiv.org/abs/2012.10700` |
| Baier & Winands, *MCTS‑minimax hybrids* ; JAIR « heuristic evaluations in MCTS » ; CodeProject « Simple AI for Breakthrough » ; fil de discussion CodinGame | **Uniquement référencés** dans `docs/resources.md` du bot n°1 ; contenu **[NON VÉRIFIÉ]** (nouveau texte du forum non récupéré : page JS, titre seul). | voir §4 |

---

## 3) Recommandation concrète et testable pour battre le boss

**Priorité : alpha‑beta rapide + règles tactiques exactes, PAS MCTS pure.** Le n°1 l'a mesuré : le best‑first UCT est neutre/négatif à réseau égal, et le MCTS sans évaluation (chelsea0x3b) dépend entièrement du hasard des rollouts.

**Recette minimale (testable en un cycle) :**

1. **Moteur** : 2 bitboards, camp courant toujours « vers le haut », flip par `bswap64` après chaque coup (16 octets/position) ; PVS itératif, TT ~2 M entrées, killers + history, quiescence captures **plus évasions** (les gains sont des coups calmes).
2. **Règles exactes avant recherche** (vérifiées referee + v1.1) : pion sur rang 7 → gain `WIN−ply` ; pion adverse seul sur rang 2 non capturable (ou ≥2) → perte ; s'il est seul et capturable → **forcer** les captures ; `safe7` → gain en 3 plies ; si aucun coup légal → perte.
3. **Fonction d'évaluation chiffrée, une seule échelle cohérente** (à partir de `v1.1.cpp`, code lu) :
   ```
   P[rang 1..7] = {100, 100, 118, 142, 180, 240, 340}
   eval = Σ_pions_alliés P[rang] − Σ_pions_ennemis P[rang adverse]
        + 6 * (# pions alliés avec pion allié en diagonale arrière)
        − 12 * (# pions alliés attaqués en diagonale par l'ennemi et non protégés)
        + 0,5 * (# pions alliés au centre et protégés)
        + 0,6 * (Σ max(0, rang−1)² alliés − Σ max(0, rang−1)² ennemis)   // option, à tester isolément
   ```
   Ne **pas** mettre de poids « home row » dans l'éval statique : la maison est déjà un gain terminal (le bot n°1 a commenté ses termes backrow ; `muy31` utilise un bonus de « chemin dégagé » ±50/±25 comme proxy, à tester).
4. **Améliorations à ajouter dans cet ordre** (Elo mesurés par le bot n°1) : singular extensions (+95), puis lot IIR + LMP + improving + history (+107). **Ne pas** tester null‑move (−35) ni best‑first UCT en remplacement.
5. **Temps/protocol** : démarrer le chrono **après la première ligne lue** ; deadline dure 95–96 ms (1000 ms au 1er coup) ; imprimer le coup **avant** tout diagnostic ; vérifier le coup contre la liste fournie et rejouer `legal[0]` sinon ; message optional ≤30 chars.
6. **Test reproductible** : referee local `https://github.com/darkhorse64/CG_Breakthrough` (ou `tools/referee.cpp` du bot n°1, « same move order and protocol »). 300–600 parties, deux couleurs, ouvertures aléatoires, **budget fixe de nœuds** d'abord, puis confirmation à 100 ms ; ne soumettre que si le gate bat la version précédente. Le bruit mesuré est ±35–40 Elo à 300–400 parties et ~±0,4 point d'arène pour 90 parties.
7. **Si vous avez du temps de calcul** : remplacer l'éval par un NNUE 128→384 cachés, 4 buckets, cible `0,6·résultat + 0,4·sigmoid(score/200)`, self‑play génération par génération. C'est le seul levier documenté qui a fait passer du rang 31 au rang 1 ; mais la largeur (640) n'aide qu'après ~100 M positions.

**Test décisif recommandé** : parties d'entraînement `A = v1.1 + singular extensions` contre `B = v1.1` à 100 000 nœuds fixes, 400 parties ; si gain > ~40 Elo, confirmer à 100 ms puis soumettre.

---

## 4) Liste de toutes les URL lues

**Referee / énoncé / configuration**
- https://api.github.com/repos/CodinGame/Breakthrough → **404**
- https://api.github.com/repos/eulerscheZahl/RefereeCollection/contents
- https://api.github.com/search/repositories?q=org:CodinGame+breakthrough → 0
- https://api.github.com/search/repositories?q=user:eulerscheZahl+breakthrough → 0
- https://api.github.com/orgs/CodinGame/repos?per_page=100
- https://api.github.com/repos/darkhorse64/CG_Breakthrough/contents
- https://api.github.com/repos/darkhorse64/CG_Breakthrough/git/trees/master?recursive=1
- https://raw.githubusercontent.com/darkhorse64/CG_Breakthrough/master/src/main/java/com/codingame/game/Referee.java
- https://raw.githubusercontent.com/darkhorse64/CG_Breakthrough/master/config/config.ini
- https://raw.githubusercontent.com/darkhorse64/CG_Breakthrough/master/config/statement_en.html
- https://raw.githubusercontent.com/darkhorse64/CG_Breakthrough/master/config/Boss.java
- https://www.codingame.com/multiplayer/bot-programming/breakthrough → page JS (« Loading... »)
- https://forum.codingame.com/t/community-puzzle-breakthrough/137971 → titre seul

**Bots publics**
- https://api.github.com/search/repositories?q=breakthrough+codingame ; https://api.github.com/search/repositories?q=breakthrough+codingame+bot&sort=stars ; https://api.github.com/search/repositories?q=breakthrough+in:name&sort=stars&order=desc&per_page=50 ; https://api.github.com/search/repositories?q=breakthrough+codingame+in:readme&per_page=100
- https://api.github.com/search/code?q=breakthrough+referee+codingame → **401** ; https://github.com/search?q=%22breakthrough%22+%22codingame%22&type=code → login requis
- https://api.github.com/repos/chelsea0x3b/breakthrough-mcts/contents ; .../contents/src
- https://raw.githubusercontent.com/chelsea0x3b/breakthrough-mcts/master/src/main.rs ; .../mcts.rs ; .../env_bitboard.rs
- https://api.github.com/repos/muy31/Breakthrough/contents
- https://raw.githubusercontent.com/muy31/Breakthrough/main/Player.java ; .../README.md
- https://api.github.com/repos/Trollkirsch3/breakthroughBot/contents ; .../contents/src
- https://api.github.com/repos/Trollkirsch3/breakthroughBot/contents/src/Main.java ; https://raw.githubusercontent.com/Trollkirsch3/breakthroughBot/master/src/Main.java
- https://api.github.com/repos/YahyaOukharta/breakthrough/contents ; .../contents/docs
- https://raw.githubusercontent.com/YahyaOukharta/breakthrough/main/README.md ; .../docs/engine.md ; .../docs/search.md ; .../docs/nnue.md ; .../docs/history.md ; .../docs/workflow.md ; .../docs/resources.md ; .../versions/v1.1.cpp
- https://api.github.com/repos/MartinMSPedersen/Breakthrough-Web/contents
- https://raw.githubusercontent.com/MartinMSPedersen/Breakthrough-Web/main/ARCHITECTURE.md ; .../src/engine/evaluator.ts
- https://api.github.com/repos/Pehz63/Breakthrough/contents
- https://raw.githubusercontent.com/Pehz63/Breakthrough/main/README.md ; .../minimax_params.txt

**Articles / académique**
- https://link.springer.com/chapter/10.1007/978-3-319-09165-5_5
- https://link.springer.com/chapter/10.1007/978-3-319-71649-7_12
- https://www.researchgate.net/publication/290565002_Programming_Breakthrough → **403**
- https://api.semanticscholar.org/graph/v1/paper/DOI:10.1007/978-3-319-09165-5_5?fields=title,abstract,authors,year,url,openAccessPdf
- https://api.semanticscholar.org/graph/v1/paper/search?query=Programming%20Breakthrough&limit=20&fields=title,authors,year,url,abstract → **429**
- https://api.semanticscholar.org/graph/v1/paper/search?query=breakthrough%20board%20game%20alpha-beta&limit=10&fields=title,authors,year,url,abstract → **429**
- https://www.lamsade.dauphine.fr/~cazenave/papers/solving.pdf → PDF non rendu par l'outil ; lu via https://r.jina.ai/https://www.lamsade.dauphine.fr/~cazenave/papers/solving.pdf
- https://arxiv.org/abs/1406.0486 ; https://arxiv.org/abs/2012.10700
- https://export.arxiv.org/api/query?search_query=all:breakthrough+AND+all:game&start=0&max_results=20
- https://export.arxiv.org/api/query?search_query=ti:breakthrough&start=0&max_results=50
- https://export.arxiv.org/api/query?search_query=ti:%22Solving%20Breakthrough%22&start=0&max_results=10 → 0 résultat
