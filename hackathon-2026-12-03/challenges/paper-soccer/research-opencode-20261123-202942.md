<!-- opencode model: hy4-flash-free, 2026-11-23T20:35:47+03:00 -->

> build · hy4-flash-free
Je vais démarrer la recherche. Je charge d'abord la compétence de recherche du dépôt et je lance en parallèle les premières requêtes sur le referee officiel.
→ Skill "cg-research-opencode" 
> build · hy4-flash-free
La compétence confirme le mode manuel (WebFetch / GitHub API) comme solution de repli autorisé. Je lance les premières requêtes en parallèle.
% WebFetch https://api.github.com/repos/jdermont/CodinGame-paper-soccer/contents 
% WebFetch https://api.github.com/repos/jdermont/CodinGame-paper-soccer 
% WebFetch https://api.github.com/search/repositories?q=paper-soccer+codingame 
✗ WebFetch https://api.github.com/search/code?q=paper+soccer+codingame+language%3APython failed
Error: StatusCode: non 2xx status code (401 GET https://api.github.com/search/code?q=paper+soccer+codingame+language%3APython)
> build · hy4-flash-free
Le referee est un repo Java (jdermont). Un second repo très récent (`Lecorbio/paper-soccer-strategy-engine`) semble contenir exactement les techniques cherchées. Je explore les deux en parallèle.
% WebFetch https://api.github.com/repos/jdermont/CodinGame-paper-soccer/git/trees/f1c5fce995aca6610d380c3c7376a4ab38cb6e98?recursive=1 
% WebFetch https://raw.githubusercontent.com/jdermont/CodinGame-paper-soccer/master/README.md 
% WebFetch https://raw.githubusercontent.com/Lecorbio/paper-soccer-strategy-engine/main/README.md 
% WebFetch https://api.github.com/search/repositories?q=paper+soccer+jacek+OR+Deltaspace 
> build · hy4-flash-free
Je lis maintenant le code source du referee (brut) et les docs d'algorithmes du bot public.
% WebFetch https://raw.githubusercontent.com/jdermont/CodinGame-paper-soccer/master/src/main/java/com/codingame/game/Referee.java 
% WebFetch https://raw.githubusercontent.com/jdermont/CodinGame-paper-soccer/master/src/main/java/com/codingame/paper_soccer/Pitch.java 
% WebFetch https://raw.githubusercontent.com/jdermont/CodinGame-paper-soccer/master/src/main/java/com/codingame/paper_soccer/MoveCalculator.java 
% WebFetch https://raw.githubusercontent.com/jdermont/CodinGame-paper-soccer/master/src/main/java/com/codingame/paper_soccer/MoveVerificator.java 
> build · hy4-flash-free
Le referee est lu. Je cherche maintenant les post-mortems de jacek / Deltaspace et je complète les constantes (config + vue).
✗ Web Search via OpenCode "CodinGame "Paper Soccer" "jacek" bot algorithm" failed
Error: Web search request failed (HTTP 402)
✗ Web Search via OpenCode "site:codingame.com/forum paper-soccer post-mortem Deltaspace" failed
Error: Web search request failed (HTTP 402)
% WebFetch https://api.github.com/repos/jdermont/CodinGame-paper-soccer/contents/config?ref=master 
% WebFetch https://raw.githubusercontent.com/jdermont/CodinGame-paper-soccer/master/src/main/java/com/codingame/view/PitchView.java 
> build · hy4-flash-free
Le repo contient `config/Boss.java` — c'est le boss de la ligue. Je le lis avec l'énoncé officiel, et je tente un moteur de recherche alternatif (websearch est indisponible).
% WebFetch https://raw.githubusercontent.com/jdermont/CodinGame-paper-soccer/master/config/statement_en.html 
% WebFetch https://raw.githubusercontent.com/jdermont/CodinGame-paper-soccer/master/config/config.ini 
% WebFetch https://raw.githubusercontent.com/jdermont/CodinGame-paper-soccer/master/config/Boss.java 
% WebFetch https://html.duckduckgo.com/html/?q=%22Paper+Soccer%22+codingame+post-mortem+Deltaspace 
> build · hy4-flash-free
Le boss est identifié : `config/Boss.java` (recherche BFS à 1 tour, score 0 partout sauf blocage = −∞, choix aléatoire). Je cherche maintenant les sources jacek/Deltaspace et les détails d'algorithmes du bot classé 2e.
% WebFetch https://raw.githubusercontent.com/Lecorbio/paper-soccer-strategy-engine/main/submissions/codingame/bots/README.md 
% WebFetch https://raw.githubusercontent.com/Lecorbio/paper-soccer-strategy-engine/main/docs/algorithms.md 
% WebFetch https://html.duckduckgo.com/html/?q=site%3Acodingame.com%2Fforum+%22Paper+Soccer%22 
% WebFetch https://html.duckduckgo.com/html/?q=codingame+%22Paper+Soccer%22+Deltaspace 
> build · hy4-flash-free
J'ai trouvé un article CodinGame de **Jacek Dermont** sur les réseaux de neurones appliqués à Paper Soccer. Je le lis, ainsi que le bot classé 2e et le fil de discussion officiel.
% WebFetch https://www.codingame.com/playgrounds/157341/inputs-for-neural-networks-for-the-board-games/paper-soccer 
% WebFetch https://raw.githubusercontent.com/Lecorbio/paper-soccer-strategy-engine/main/submissions/codingame/bots/quantized_alpha_beta/README.md 
% WebFetch https://html.duckduckgo.com/html/?q=%22paper+soccer%22+%22post+mortem%22+codingame 
% WebFetch https://www.codingame.com/multiplayer/bot-programming/paper-soccer/discuss 
> build · hy4-flash-free
Les pages CodinGame sont rendues en JavaScript. Je tente un rendu texte (jina.ai) et des recherches ciblées sur l'article et Deltaspace.
% WebFetch https://html.duckduckgo.com/html/?q=%22Inputs+for+neural+networks+for+the+board+games%22+paper+soccer 
✗ WebFetch https://r.jina.ai/https://www.codingame.com/multiplayer/bot-programming/paper-soccer/discuss failed
Error: Transport error (GET https://r.jina.ai/https://www.codingame.com/multiplayer/bot-programming/paper-soccer/discuss)
✗ WebFetch https://r.jina.ai/https://www.codingame.com/playgrounds/157341/inputs-for-neural-networks-for-the-board-games/paper-soccer failed
Error: Transport error (GET https://r.jina.ai/https://www.codingame.com/playgrounds/157341/inputs-for-neural-networks-for-the-board-games/paper-soccer)
✗ WebFetch https://html.duckduckgo.com/html/?q=Deltaspace+%22paper+soccer%22+github+bot failed
Error: Transport error (GET https://html.duckduckgo.com/html/?q=Deltaspace+%22paper+soccer%22+github+bot)
> build · hy4-flash-free
! retrying (attempt 2): Can't reach opencode.ai. Check the network connection. (getaddrinfo EAI_AGAIN opencode.ai)
Je tente l'API publique CodinGame (POST) pour récupérer le contenu du playground, et je cherche un éventuel compte GitHub `Deltaspace`.
⚙ execute {"code":"const urls = [\n  [\"https://www.codingame.com/services/PlaygroundService/findPlaygroundById\", {method:\"POST\", headers:{\"Content-Type\":\"application/json\"}, body:JSON.stringify([157341])}],\n  [\"https://api.github.com/search/users?q=Deltaspace\", {headers:{\"User-Agent\":\"research\"}}],\n  [\"https://api.github.com/users/Deltaspace/repos?per_page=100\", {headers:{\"User-Agent\":\"research\"}}],\n  [\"https://html.duckduckgo.com/html/?q=Deltaspace%20%22paper%20soccer%22%20bot\", {}],\n];\nconst out = await Promise.allSettled(urls.map(async ([u,o]) => {\n  const r = await fetch(u,o); const t = await r.text(); return {u, status:r.status, text:t.slice(0,6000)};\n}));\nreturn out.map((r,i)=> r.status===\"fulfilled\" ? {i,...r.value} : {i,error:String(r.reason)});"} 
> build · hy4-flash-free
Je vais extraire proprement les résultats DuckDuckGo et tester les endpoints de l'API CodinGame.
⚙ execute {"code":"const queries = [\n 'Deltaspace \"paper soccer\" bot',\n '\"paper soccer\" \"post-mortem\" OR \"post mortem\"',\n '\"paper soccer\" jacek Deltaspace',\n 'site:codingame.com/playgrounds/157341 paper soccer',\n '\"paper soccer\" alpha-beta codingame github',\n];\nconst parse = (html) => {\n  const links=[...html.matchAll(/class=\"result__a\"[^>]*href=\"([^\"]+)\"[^>]*>([\\s\\S]*?)<\\/a>/g)].map(m=>({url:m[1].replace(/&amp;/g,'&'),title:m[2].replace(/<[^>]+>/g,'').trim()}));\n  const snips=[...html.matchAll(/class=\"result__snippet\"[^>]*>([\\s\\S]*?)<\\/a>/g)].map(m=>m[1].replace(/<[^>]+>/g,'').replace(/&amp;/g,'&').trim());\n  return links.map((l,i)=>({...l,snippet:snips[i]||''}));\n};\nconst out = await Promise.allSettled(queries.map(async q=>{\n  const r=await fetch('https://html.duckduckgo.com/html/?q='+encodeURIComponent(q)); const t=await r.text(); return {q,results:parse(t).slice(0,8)};\n}));\nreturn out.map(r=>r.status==='fulfilled'?r.value:{error:String(r.reason)});"} 
> build · hy4-flash-free
Trouvée une source primaire de Jacek Dermont : `YaPaperSoccer/ai_readme/paper_soccer.md`. Je la lis ainsi que les docs associées.
% WebFetch https://raw.githubusercontent.com/jdermont/YaPaperSoccer/master/ai_readme/paper_soccer.md 
% WebFetch https://api.github.com/repos/jdermont/YaPaperSoccer/contents/ai_readme?ref=master 
% WebFetch https://raw.githubusercontent.com/jdermont/YaPaperSoccer/master/README.md 
% WebFetch https://raw.githubusercontent.com/bytesroboticmx/soccerBot/master/README.md 
> build · hy4-flash-free
Je vérifie la collection de referees eulerscheZahl et je poursuis la traque de Deltaspace / du forum.
⚙ execute {"code":"const queries=[\n '\"Deltaspace\" \"paper soccer\" strategy',\n '\"paper soccer\" \"Deltaspace\" codingame forum',\n 'site:codingame.com/playgrounds Deltaspace paper soccer',\n 'papersoccer Deltaspace alpha beta',\n '\"paper soccer\" minimax codingame discuss',\n];\nconst parse=h=>[...h.matchAll(/class=\"result__a\"[^>]*href=\"([^\"]+)\"[^>]*>([\\s\\S]*?)<\\/a>/g)].map((m,i)=>{const s=[...h.matchAll(/class=\"result__snippet\"[^>]*>([\\s\\S]*?)<\\/a>/g)][i];return {url:decodeURIComponent((m[1].match(/uddg=([^&]+)/)||[])[1]||m[1].replace(/^\\/\\/duckduckgo.com\\/l\\/\\?uddg=/,'')),title:m[2].replace(/<[^>]+>/g,'').trim(),snippet:s?s[1].replace(/<[^>]+>/g,'').trim():''};});\nreturn await Promise.allSettled(queries.map(async q=>{const r=await fetch('https://html.duckduckgo.com/html/?q='+encodeURIComponent(q));return {q,results:parse(await r.text()).slice(0,8)};})).then(a=>a.map(r=>r.status==='fulfilled'?r.value:{error:String(r.reason)}));"} 
> build · hy4-flash-free
⚙ execute {"code":"const jobs=[\n [\"https://api.github.com/repos/eulerscheZahl/RefereeCollection/contents\",null],\n [\"https://api.github.com/repos/Lecorbio/paper-soccer-strategy-engine/git/trees/main?recursive=1\",null],\n [\"https://html.duckduckgo.com/html/?q=\"+encodeURIComponent('codingame forum \"paper soccer\" algorithm'),null],\n [\"https://www.codingame.com/services/\", null]\n];\nconst parse=h=>[...h.matchAll(/class=\"result__a\"[^>]*href=\"([^\"]+)\"[^>]*>([\\s\\S]*?)<\\/a>/g)].map((m,i)=>({url:decodeURIComponent((m[1].match(/uddg=([^&]+)/)||[])[1]||''),title:m[2].replace(/<[^>]+>/g,'').trim()}));\nreturn await Promise.allSettled(jobs.map(async ([u])=>{const r=await fetch(u,{headers:{\"User-Agent\":\"research\"}});const t=await r.text();let d;try{d=JSON.parse(t)}catch{}if(d&&d.tree){d={total:d.tree.length,interesting:d.tree.filter(x=>/research|forum|reference|statement|paper-soccer\\.md/i.test(x.path)).map(x=>x.path).slice(0,80)}}else if(u.includes('duckduckgo')){d=parse(t).slice(0,10)}else if(Array.isArray(d)){d=d.map(x=>x.name).slice(0,80)}else{d=t.slice(0,500)}return {u,status:r.status,d};})).then(a=>a.map(r=>r.status==='fulfilled'?r.value:{error:String(r.reason)}));"} 
> build · hy4-flash-free
Je lis les notes de recherche du repo classé 2e et le dossier du bot historique rank 5 (tour complet).
% WebFetch https://raw.githubusercontent.com/Lecorbio/paper-soccer-strategy-engine/main/submissions/codingame/bots/rank_5/README.md 
% WebFetch https://raw.githubusercontent.com/Lecorbio/paper-soccer-strategy-engine/main/docs/rank-two-live-research.md 
% WebFetch https://raw.githubusercontent.com/Lecorbio/paper-soccer-strategy-engine/main/docs/bots/rank-4.md 
% WebFetch https://raw.githubusercontent.com/jdermont/QtPaperSoccer/master/README.md 
> build · hy4-flash-free
J'ai trouvé le miroir GitHub de l'article playground de jacek. Je le lis avec l'article arXiv cité pour la recherche best-first.
% WebFetch https://arxiv.org/abs/2012.10700 
% WebFetch https://raw.githubusercontent.com/jdermont/playground-kvhfh5iv/master/papersoccer.md 
% WebFetch https://api.github.com/repos/jdermont/playground-kvhfh5iv/contents?ref=master 
> build · hy4-flash-free
Dernières vérifications : la référence arXiv et l'absence de post-mortem Deltaspace.
⚙ execute {"code":"const jobs=[\n ['https://api.semanticscholar.org/graph/v1/paper/search?query=Unbounded%20best-first%20minimax&limit=5&fields=title,authors,year,url,abstract',{}],\n ['https://html.duckduckgo.com/html/?q='+encodeURIComponent('Deltaspace codingame post mortem paper soccer playground'),{}],\n ['https://www.codingame.com/multiplayer/bot-programming/paper-soccer',{}],\n];\nreturn await Promise.allSettled(jobs.map(async ([u,o])=>{const r=await fetch(u,o);const t=await r.text();return {u,status:r.status,len:t.length,head:t.slice(0,1200)};})).then(a=>a.map(r=>r.status==='fulfilled'?r.value:{error:String(r.reason)}));"} 
> build · hy4-flash-free
⚙ execute {"code":"const r=await fetch('https://html.duckduckgo.com/html/?q='+encodeURIComponent('Deltaspace codingame post mortem paper soccer playground'));const h=await r.text();\nconst links=[...h.matchAll(/class=\"result__a\"[^>]*href=\"([^\"]+)\"[^>]*>([\\s\\S]*?)<\\/a>/g)];\nconst snips=[...h.matchAll(/class=\"result__snippet\"[^>]*>([\\s\\S]*?)<\\/a>/g)];\nreturn {count:links.length, results:links.map((m,i)=>({url:decodeURIComponent((m[1].match(/uddg=([^&]+)/)||[])[1]||''),title:m[2].replace(/<[^>]+>/g,''),snippet:snips[i]?snips[i][1].replace(/<[^>]+>/g,''):''})).slice(0,10)};"} 
> build · hy4-flash-free
# Recherche — Paper Soccer (CodinGame, referee `jdermont/CodinGame-paper-soccer`)

**Périmètre respecté** : aucune modification/création de fichier, aucun build, aucune lecture de `~/.config/codingame`. Sources lues via WebFetch, API GitHub, `raw.githubusercontent.com` et moteur de recherche HTML.

**Important sur les noms** : la page officielle crédite « Paper soccer **By jacek** » et « Approved by trictrac, BenjaminUrquhart, **Deltaspace** » (extrait lisible via résultats de recherche ; la page CodinGame est en JavaScript et n'a pas pu être lue intégralement). « jacek » = **Jacek Dermont**, auteur du referee et de l'article playground ; **je n'ai trouvé aucune stratégie publique attribuable à Deltaspace pour ce jeu** → toute affirmation sur son algorithme est `[NON VÉRIFIÉ]`.

---

## 1) Faits du referee (sources primaires lues)

| Fait | Valeur exacte | URL lue |
|---|---|---|
| Repo referee | `jdermont/CodinGame-paper-soccer`, Java, créé 2020-05-24, topic `minimax` | https://api.github.com/repos/jdermont/CodinGame-paper-soccer |
| Config jeu | `min_players=2`, `max_players=2`, `type=multi` | https://raw.githubusercontent.com/jdermont/CodinGame-paper-soccer/master/config/config.ini |
| Énoncé officiel embarqué | plateau 8×10 buts, 0=N,1=NE,…,7=NW, 1er joueur doit atteindre le but du haut | https://raw.githubusercontent.com/jdermont/CodinGame-paper-soccer/master/config/statement_en.html |
| Temps / tours | `setFrameDuration(600)`, `setMaxTurns(66)`, `setTurnMaxTime(200)` ; énoncé : **1000 ms au 1er appel, 200 ms ensuite** | https://raw.githubusercontent.com/jdermont/CodinGame-paper-soccer/master/src/main/java/com/codingame/game/Referee.java |
| Protocole | 1re ligne = `myId` ; chaque tour : `opponentMoveLength` puis `opponentMove` ; 1er tour `1` / `"-"` | idem + `statement_en.html` |
| Graphe | `(width+1)*(height+1)+6 = 9*11+6 = 105` sommets ; ballon `index 49` = `(x=4,y=5)` ; buts haut `y=-1`, bas `y=11`, ouverture `x∈{3,4,5}` | https://raw.githubusercontent.com/jdermont/CodinGame-paper-soccer/master/src/main/java/com/codingame/paper_soccer/Pitch.java |
| Lignes pré-dessinées | bords `x=0`, `x=8`, `y=0`, `y=10` (sauf bouche de but) + poteaux → rebonds sur les bords | idem (`Pitch()` constructeur) |
| Règle de rebond | `passNext(i)` : `libres < voisins_initiaux` ; `passNextDone()` : `libres < voisins_initiaux - 1` → le joueur **doit** continuer tant qu'il atterrit sur un sommet déjà touché | idem |
| Fin de tour / partie | `isGameOver()` = but **ou** `isBlocked(ball)` (`libres==0`) | idem |
| Départage / vainqueur | But → vainqueur = **adversaire du propriétaire du but** (but propre = défaite). Blocage → vainqueur = adversaire du joueur qui vient de jouer (`getWinner`). Aucun départage par score écrit dans le referee ;_timeout_/move invalide → `+1/-1` | `Pitch.getWinner()` + `Referee.setWinner()/setWinnerFromInvalid()` |
| Ordre de résolution d'un tour | `getPlayer(turn % playerCount)` → envoi du coup adverse → `player.execute()` → `verifyMove()` → `addEdge()` + déplacement → `isGameOver()` → `setWinner()` → `endGame()` | `Referee.gameTurn()` |
| Validation | caractères `0..8` acceptés syntaxiquement (`8` échoue ensuite comme « hors plateau »), interdit arête existante, coup trop court ou trop long | https://raw.githubusercontent.com/jdermont/CodinGame-paper-soccer/master/src/main/java/com/codingame/paper_soccer/MoveVerificator.java |
| Limite d'attente | 66 tours max dans le referee, mais **aucun vainqueur n'est explicitement fixé à cette limite dans le code lu** → comportement exact `[NON VÉRIFIÉ]` | `Referee.init()` |

**Le boss** : `config/Boss.java` est fourni dans le repo referee. Algorithme lu :
- `MoveCalculator` local, `SIZE_LIMIT = 10`, `INFINITY = 1000`, parcours de type pile (`ArrayDeque.push`/`pollFirst`) avec détection de cycles par hash d'arêtes ;
- score = **0 pour tout coup non-bloquant, y compris un but** (branche vide), **−1000 si le ballon est bloqué** ;
- `maxScore` = 0 ⇒ **choix aléatoire uniforme** parmi les coups complets non-bloquants trouvés ; aucune préférence pour marquer, aucune défense, aucune profondeur adverse.

URL : https://raw.githubusercontent.com/jdermont/CodinGame-paper-soccer/master/config/Boss.java

⚠️ L'identité entre ce `Boss.java` et le « boss de la ligue » que tu affrontes est **inférée** (c'est le boss livré avec le jeu) : `[NON VÉRIFIÉ]` sans log de match.

À noter : le `MoveCalculator.java` du referee (différent de celui du boss) utilise `+1000` but adverse, `−995` but propre, `−1000` blocage, sinon `±y`, et renvoie **tous les coups ex aequo** du meilleur score — c'est un utilitaire de démo, pas le bot de classement.

La collection `eulerscheZahl/RefereeCollection` ne contient **pas** paper-soccer (vérifié : https://api.github.com/repos/eulerscheZahl/RefereeCollection/contents).

---

## 2) Stratégies des meilleurs joueurs

### Jacek Dermont (« jacek ») — source primaire directe

| URL | Idée lue | Pourquoi ça marche |
|---|---|---|
| https://raw.githubusercontent.com/jdermont/YaPaperSoccer/master/ai_readme/paper_soccer.md | **Minimax + alpha-beta + un peu d'iterative deepening** ; un « coup » = **le tour complet avec rebonds** (1 ply) ; 105 sommets, matrice + liste d'adjacence O(1) ; génération DFS + BFS avec mémoire des cycles ; limites 20/30/100/200 coups par ply ; profondeurs 1/2/3+ ; **jouer « en arrière » gagne 2 à 5 fois plus souvent** qu'avancer bêtement | Le tour-complet comme ply respecte la possession ; dédupliquer `ABCDA2` et `ADCBA2` (57 → 52 coups dans son exemple) évite d'exploser l'arbre ; la stratégie défensive crée des pièges tout en laissant à l'adversaire le soin de se bloquer. Il mesure ~62 coups moyens, jusqu'à >400 000 coups possibles en position extrême |
| https://raw.githubusercontent.com/jdermont/playground-kvhfh5iv/master/papersoccer.md (miroir lisible de l'article CodinGame `playgrounds/157341`) | Évaluation par **réseau de neurones** : 316 arêtes + `105 × 8` one-hot de « vraie distance » (min. tours pour atteindre un nœud) = **1156 entrées** ; plateau pivoté de 180° pour le joueur 2 ; version CodinGame = **2 couches cachées 32 et 32, 200 ms, 1 thread**, « mène largement le leaderboard » ; >95 % de victoires vs son MCTS ; version PC (~7k entrées : `8*distance + degrés−1`) encore +65 % | Une règle d'évaluation simple (`aller au but`) est mauvaise ici ; la « vraie distance » en graphe et la rotation joueur-relative donnent au réseau une représentation invariante et riche ; le NN a battu MCTS à budget égal |
| https://raw.githubusercontent.com/jdermont/QtPaperSoccer/master/README.md | Recherche **« Unbounded best-first minimax with UCT component »**, evaluation NN ; cite https://arxiv.org/abs/2012.10700 (titre lu : *Minimax Strikes Back*, Cohen-Solal & Cazenave) | Remplace les playouts aléatoires par une évaluation apprise ; UCT gère l'exploration dans un arbre très branchu |

### Autres bots publics classés (hypothèses à valider, pas des sources « jacek/Deltaspace »)

| URL | Résultat annoncé | Algorithme lu |
|---|---|---|
| https://raw.githubusercontent.com/Lecorbio/paper-soccer-strategy-engine/main/submissions/codingame/bots/quantized_alpha_beta/README.md | **rank 2/212, 9 oct. 2026** (auto-reporté avec capture/manifest) | Alpha-beta sur **tours complets** + réseau de valeur `6301→12→8→1`, poids 4 bits, Huffman ; preuves terminales séparées ; budgets **550 ms / 140 ms**, 99 032 caractères |
| https://raw.githubusercontent.com/Lecorbio/paper-soccer-strategy-engine/main/submissions/codingame/bots/rank_5/README.md | rank 5/206, score 42.4277 | Alpha-beta tour complet + modèle de valeur entraîné sur 88 replays publics + **livre d'historique exact** (coups rejoués seulement si historique complet vérifié) ; 650/130 ms |
| https://raw.githubusercontent.com/Lecorbio/paper-soccer-strategy-engine/main/docs/bots/rank-4.md | rank 4/208, 66–24 | Même noyau + **résidu linéaire 24 features** appris de recherches professeur plus profondes, plafonné, désactivé en ouverture |
| https://raw.githubusercontent.com/Lecorbio/paper-soccer-strategy-engine/main/docs/algorithms.md | documentation d'engine | La profondeur de possession ne se consomme **qu'au changement de main** ; évaluation maison : but dispo 50 000, différence de distance BFS 120, progression verticale 80, possession 35, mobilité 20, tempo 15 ; terminaux ±1 000 000 ; profils 6 handoffs / 100k nœuds / 65 536 TT ; MCTS UCT + rollouts « Tactical », solver ; oracle de fin de partie ≤18 arêtes atteignables |
| https://raw.githubusercontent.com/bytesroboticmx/soccerBot/master/README.md | Bot gagnant du concours MIM UW 2014 | Alpha-beta **et** MCTS avec **RAVE**, stratégies composables (MCTS utilise une autre stratégie pour les playouts) |

**Deltaspace** : aucune source de stratégie paper-soccer trouvée malgré 6 requêtes ciblées ; son rôle vérifiable est « approver » de challenge. Ses algorithmes éventuels : `[NON VÉRIFIÉ]`.

**Synthèse** : oui, alpha-beta **sur tours complets** est le courant dominant ; l'évaluation est soit un réseau de valeur (jacek, rank 2), soit des features de graphe + historique (rank 5/rank 4) ; le MCTS est historiquement bon mais dépassé par l'évaluation apprise chez jacek ; les rebonds sont gérés en faisant du tour complet l'unité de profondeur, avec déduplication des ordres de segments.

---

## 3) Recommandation concrète et testable pour battre le boss

**Hypothèse testable** : si le boss est bien `config/Boss.java`, il ne fait **aucune recherche adverse** et tire au hasard parmi ≤10 tours complets non-bloquants, indifférent à un but propre. Un alpha-beta de profondeur 2–3 tours doit le battre très largement.

Plan minimal, mesurable :

1. **Représentation** : 105 sommets + arêtes utilisées en bitset ; reconstruire l'état depuis l'historique des coups adverses (le referee n'envoie que le dernier coup). Jour 1 = `myId`, 1/`-`.
2. **Générateur de coups** : DFS des rebonds avec `passNext`/`passNextDone`, arrêt sur but/blocage, **déduplication par ensemble d'arêtes triées** (pas par chaîne de chiffres), plafond ~100–200 coups par nœud, ordre : but adverse → but propre exclu → continuation vers but → coups « arrière/piège ».
3. **Recherche** : alpha-beta itératif sur **tours complets** (la possession ne change qu'en fin de tour), profondeur 1→3, TT compact, budget 550 ms / 140 ms (marge sous 1000/200), garde-mechanisme déterministe à nœuds épuisés.
4. **Évaluation** : terminal `+1e6 / −1e6 / blocked=−1e6+...` ; sinon distance BFS sur arêtes libres vers le but adverse − vers son propre but, mobilité du joueur au trait, pénalité si peu d'arêtes libres autour de la balle, petit bonus de possession/tempo. **Pas** de simple `±y` seul (jacek : « mindlessly going to opponent goal isn't helpful »).
5. **Test reproductible** : harness local avec `Boss.java` comme adverse, **100–200 parties couleurs inversées**, même seed ; mesures : taux de victoire cible **>90 %**, nombre de `INVALID MOVE`, temps max par coup, taux de buts propres (doit ≈0 pour toi, le boss en fera). Ablations à mesurer séparément : (a) terminaux seuls, (b) + BFS distance, (c) + featured « arrière/piège » ; ne garder que ce qui gagne >3 pts avec intervalle de confiance.
6. **Garde anti-règles** : valider chaque coup avec la logique de `MoveVerificator` avant sortie ; ne jamais émettre de chaîne se terminant sur un sommet où `passNextDone()` est vrai.

Si le boss réel est un bot fort (rank 1–5) et non `Boss.java`, la même recette reste valable mais il faut ajouter valeur apprise/replay-book : `[NON VÉRIFIÉ]` sans log de match.

---

## 4) Liste de toutes les URL lues

**Referee / règles**
1. https://api.github.com/repos/jdermont/CodinGame-paper-soccer
2. https://api.github.com/repos/jdermont/CodinGame-paper-soccer/contents
3. https://api.github.com/repos/jdermont/CodinGame-paper-soccer/git/trees/f1c5fce995aca6610d380c3c7376a4ab38cb6e98?recursive=1
4. https://raw.githubusercontent.com/jdermont/CodinGame-paper-soccer/master/README.md
5. https://raw.githubusercontent.com/jdermont/CodinGame-paper-soccer/master/config/config.ini
6. https://raw.githubusercontent.com/jdermont/CodinGame-paper-soccer/master/config/statement_en.html
7. https://raw.githubusercontent.com/jdermont/CodinGame-paper-soccer/master/config/Boss.java
8. https://api.github.com/repos/jdermont/CodinGame-paper-soccer/contents/config?ref=master
9. https://raw.githubusercontent.com/jdermont/CodinGame-paper-soccer/master/src/main/java/com/codingame/game/Referee.java
10. https://raw.githubusercontent.com/jdermont/CodinGame-paper-soccer/master/src/main/java/com/codingame/paper_soccer/Pitch.java
11. https://raw.githubusercontent.com/jdermont/CodinGame-paper-soccer/master/src/main/java/com/codingame/paper_soccer/MoveCalculator.java
12. https://raw.githubusercontent.com/jdermont/CodinGame-paper-soccer/master/src/main/java/com/codingame/paper_soccer/MoveVerificator.java
13. https://raw.githubusercontent.com/jdermont/CodinGame-paper-soccer/master/src/main/java/com/codingame/view/PitchView.java
14. https://api.github.com/repos/eulerscheZahl/RefereeCollection/contents
15. https://www.codingame.com/multiplayer/bot-programming/paper-soccer (tentative : JS, non lisible)
16. https://www.codingame.com/multiplayer/bot-programming/paper-soccer/discuss (tentative : JS, non lisible)
17. https://www.codingame.com/services/PlaygroundService/findPlaygroundById (POST, 422 — service indisponible)

**Jacek / sources primaires stratégie**
18. https://raw.githubusercontent.com/jdermont/YaPaperSoccer/master/ai_readme/paper_soccer.md
19. https://api.github.com/repos/jdermont/YaPaperSoccer/contents/ai_readme?ref=master
20. https://raw.githubusercontent.com/jdermont/YaPaperSoccer/master/README.md
21. https://raw.githubusercontent.com/jdermont/playground-kvhfh5iv/master/papersoccer.md
22. https://api.github.com/repos/jdermont/playground-kvhfh5iv/contents?ref=master
23. https://raw.githubusercontent.com/jdermont/QtPaperSoccer/master/README.md
24. https://www.codingame.com/playgrounds/157341/inputs-for-neural-networks-for-the-board-games/paper-soccer (tentative : JS ; contenu lu via #21)
25. https://arxiv.org/abs/2012.10700
26. https://api.semanticscholar.org/graph/v1/paper/search?query=Unbounded%20best-first%20minimax... (429, non exploité)

**Bots publics classés / algorithmes**
27. https://api.github.com/search/repositories?q=paper-soccer+codingame
28. https://api.github.com/search/repositories?q=paper+soccer+jacek+OR+Deltaspace
29. https://raw.githubusercontent.com/Lecorbio/paper-soccer-strategy-engine/main/README.md
30. https://raw.githubusercontent.com/Lecorbio/paper-soccer-strategy-engine/main/docs/algorithms.md
31. https://raw.githubusercontent.com/Lecorbio/paper-soccer-strategy-engine/main/submissions/codingame/bots/README.md
32. https://raw.githubusercontent.com/Lecorbio/paper-soccer-strategy-engine/main/submissions/codingame/bots/quantized_alpha_beta/README.md
33. https://raw.githubusercontent.com/Lecorbio/paper-soccer-strategy-engine/main/submissions/codingame/bots/rank_5/README.md
34. https://raw.githubusercontent.com/Lecorbio/paper-soccer-strategy-engine/main/docs/bots/rank-4.md
35. https://raw.githubusercontent.com/Lecorbio/paper-soccer-strategy-engine/main/docs/rank-two-live-research.md
36. https://api.github.com/repos/Lecorbio/paper-soccer-strategy-engine/git/trees/main?recursive=1
37. https://raw.githubusercontent.com/bytesroboticmx/soccerBot/master/README.md

**Recherches web (DuckDuckGo HTML, requêtes lues)**
38. `https://html.duckduckgo.com/html/?q=` + : `"Paper Soccer" codingame post-mortem Deltaspace` · `codingame "Paper Soccer" Deltaspace` · `site:codingame.com/forum "Paper Soccer"` · `"paper soccer" "post mortem" codingame` · `Deltaspace "paper soccer" bot` · `codingame forum "paper soccer" algorithm` · `Deltaspace codingame post mortem paper soccer playground` · `"paper soccer" alpha-beta codingame github` · `site:codingame.com/playgrounds/157341 paper soccer`.

Aucun post-mortem CodinGame/forum consultable n'a été trouvé pour paper-soccer ; les fils `discuss`/`forum` sont en JavaScript et leurs contenus restent `[NON VÉRIFIÉ]`.
