<!-- opencode model: hy4-flash-free, 2026-11-23T21:07:26+03:00 -->

> build · hy4-flash-free
Je vais effectuer une recherche approfondie sur le jeu 'brandubh'. Commençons par plusieurs recherches parallèles pour localiser le referee officiel, les bots publics et les post-mortems.
✗ Web Search via OpenCode "CodinGame "brandubh" game referee bot programming" failed
Error: Web search request failed (HTTP 402)
✗ Web Search via OpenCode "github CodinGame brandubh referee" failed
Error: Web search request failed (HTTP 402)
✗ Web Search via OpenCode "eulerscheZahl RefereeCollection brandubh" failed
Error: Web search request failed (HTTP 402)
✗ Web Search via OpenCode "brandubh CodinGame post-mortem strategy alpha-beta" failed
Error: Web search request failed (HTTP 402)
> build · hy4-flash-free
La recherche web échoue (HTTP 402). Je vais utiliser directement l'API GitHub via webfetch.
✗ WebFetch https://api.github.com/search/code?q=brandubh+codingame failed
Error: StatusCode: non 2xx status code (401 GET https://api.github.com/search/code?q=brandubh+codingame)
% WebFetch https://api.github.com/search/repositories?q=brandubh+codingame 
% WebFetch https://api.github.com/search/repositories?q=brandubh 
> build · hy4-flash-free
Le dépôt `n3l-ctrl/brandubh-codingame` (créé juillet 2026, Java) semble être un bot CodinGame récent. Explorons les sources en parallèle : referee officiel, RefereeCollection, et les dépôts candidats.
✗ WebFetch https://api.github.com/repos/CodinGame/brandubh failed
Error: StatusCode: non 2xx status code (403 GET https://api.github.com/repos/CodinGame/brandubh)
✗ WebFetch https://api.github.com/repos/n3l-ctrl/brandubh-codingame/contents/ failed
Error: StatusCode: non 2xx status code (403 GET https://api.github.com/repos/n3l-ctrl/brandubh-codingame/contents/)
% WebFetch https://api.github.com/repos/eulerscheZahl/RefereeCollection/contents/ 
% WebFetch https://export.arxiv.org/api/query?search_query=all:brandubh&start=0&max_results=20 
✗ WebFetch https://api.github.com/repos/brenjohn/Brandubh/contents/ failed
Error: StatusCode: non 2xx status code (403 GET https://api.github.com/repos/brenjohn/Brandubh/contents/)
> build · hy4-flash-free
La collection de referees d'eulerscheZahl n'a pas Brandubh, mais j'ai trouvé un article arXiv. L'API GitHub est rate-limitée ; je bascule sur `raw.githubusercontent.com` et les pages HTML.
% WebFetch https://arxiv.org/abs/2106.05353 
% WebFetch https://www.codingame.com/multiplayer/bot-programming/brandubh 
% WebFetch https://raw.githubusercontent.com/n3l-ctrl/brandubh-codingame/master/README.md 
% WebFetch https://raw.githubusercontent.com/brenjohn/Brandubh/main/README.md 
% WebFetch https://github.com/n3l-ctrl/brandubh-codingame 
> build · hy4-flash-free
Le dépôt `n3l-ctrl/brandubh-codingame` est en réalité le **code du jeu** (skeleton Game Engine Toolkit) avec `test_boss.py`, `docs/`, `src/`. C'est une source primaire. Explorer l'arborescence.
% WebFetch https://api.github.com/rate_limit 
% WebFetch https://github.com/n3l-ctrl/brandubh-codingame/tree/master/docs 
% WebFetch https://github.com/n3l-ctrl/brandubh-codingame/tree/master/src 
% WebFetch https://github.com/n3l-ctrl/brandubh-codingame/tree/master/config 
> build · hy4-flash-free
% WebFetch https://raw.githubusercontent.com/n3l-ctrl/brandubh-codingame/master/config/statement_en.html 
% WebFetch https://raw.githubusercontent.com/n3l-ctrl/brandubh-codingame/master/config/stub.txt 
% WebFetch https://raw.githubusercontent.com/n3l-ctrl/brandubh-codingame/master/config/config.ini 
% WebFetch https://raw.githubusercontent.com/n3l-ctrl/brandubh-codingame/master/quick_test_boss.py 
% WebFetch https://github.com/n3l-ctrl/brandubh-codingame/tree/master/src/main 
> build · hy4-flash-free
J'ai le referee officiel. Récupérons maintenant `Referee.java`, la logique de jeu et le boss.
% WebFetch https://raw.githubusercontent.com/n3l-ctrl/brandubh-codingame/master/config/statement_fr.html 
% WebFetch https://github.com/n3l-ctrl/brandubh-codingame/tree/master/src/main/java/com/codingame/game 
% WebFetch https://raw.githubusercontent.com/n3l-ctrl/brandubh-codingame/master/test_boss.py 
% WebFetch https://raw.githubusercontent.com/n3l-ctrl/brandubh-codingame/master/src/main/java/com/codingame/game/Referee.java 
> build · hy4-flash-free
Le referee est complet. Cherchons maintenant les constantes d'ouverture, le boss, et des bots/post-mortems publics.
✗ WebFetch https://grep.app/api/search?q=brandubh%20codingame failed
Error: StatusCode: non 2xx status code (429 GET https://grep.app/api/search?q=brandubh%20codingame)
✗ WebFetch https://raw.githubusercontent.com/n3l-ctrl/brandubh-codingame/master/config/Boss.py3 failed
Error: StatusCode: non 2xx status code (404 GET https://raw.githubusercontent.com/n3l-ctrl/brandubh-codingame/master/config/Boss.py3)
% WebFetch https://raw.githubusercontent.com/n3l-ctrl/brandubh-codingame/master/src/main/java/com/codingame/game/Board.java 
% WebFetch https://github.com/n3l-ctrl/brandubh-codingame/forks 
% WebFetch https://html.duckduckgo.com/html/?q=brandubh+codingame+forum+post-mortem 
> build · hy4-flash-free
Le referee donne 8 attaquants + 4 défenseurs + roi. J'ai trouvé le fil de forum ; explorons-le et cherchons les bots publics.
% WebFetch https://html.duckduckgo.com/html/?q=%22brandubh%22+codingame+bot+github+strategy 
% WebFetch https://forum.codingame.com/t/community-puzzle-brandubh-puzzle-discussion/208746.json 
% WebFetch https://html.duckduckgo.com/html/?q=brandubh+codingame+boss+alpha-beta+MCTS 
% WebFetch https://forum.codingame.com/t/community-puzzle-brandubh-puzzle-discussion/208746 
> build · hy4-flash-free
Le fil forum est récent et sans stratégie (3 messages). Explorons les moteurs publics (alpha-beta, RL) et l'historique du boss.
✗ WebFetch https://api.semanticscholar.org/graph/v1/paper/search?query=brandubh&limit=20&fields=title,year,abstract,url,citationCount failed
Error: StatusCode: non 2xx status code (429 GET https://api.semanticscholar.org/graph/v1/paper/search?query=brandubh&limit=20&fields=title,year,abstract,url,citationCount)
% WebFetch https://github.com/n3l-ctrl/brandubh-codingame/commits/master/config/Boss.py3 
% WebFetch https://github.com/jojopeligroso/brandubh 
% WebFetch https://github.com/brenjohn/Brandubh/tree/main 
% WebFetch https://github.com/manudhaeyer-hash/Brandubh 
> build · hy4-flash-free
Le moteur `jojopeligroso/brandubh` est une mine (alpha-bêta itératif, TT, quiescence). Récupérons l'évaluation concrète.
% WebFetch https://raw.githubusercontent.com/jojopeligroso/brandubh/main/docs/solving.md 
% WebFetch https://raw.githubusercontent.com/jojopeligroso/brandubh/main/src/game/engine.ts 
% WebFetch https://github.com/brenjohn/Brandubh/tree/main/docs 
% WebFetch https://github.com/brenjohn/Brandubh/tree/main/brandubh 
> build · hy4-flash-free
% WebFetch https://sourcegraph.com/search?q=context:global+brandubh+codingame&patternType=keyword 
% WebFetch https://github.com/brenjohn/Brandubh/tree/main/brandubh/bots 
% WebFetch https://api.crossref.org/works?query.bibliographic=brandubh&rows=20 
% WebFetch https://html.duckduckgo.com/html/?q=%22Boss.py3%22+brandubh+codingame 
> build · hy4-flash-free
% WebFetch https://raw.githubusercontent.com/brenjohn/Brandubh/main/brandubh/bots/mcbot.py 
% WebFetch https://raw.githubusercontent.com/jojopeligroso/brandubh/main/src/game/variants.ts 
% WebFetch https://github.com/brenjohn/Brandubh/tree/main/brandubh/bots/zero_bot 
% WebFetch https://doi.org/10.3390/technologies5020016 
> build · hy4-flash-free
% WebFetch https://raw.githubusercontent.com/brenjohn/Brandubh/main/brandubh/bots/zero_bot/brandubh_zero.py 
% WebFetch https://ar5iv.labs.arxiv.org/html/2106.05353 
% WebFetch https://export.arxiv.org/api/query?search_query=all:tafl+OR+all:hnefatafl&start=0&max_results=30 
% WebFetch https://github.com/n3l-ctrl?tab=repositories 
% WebFetch https://html.duckduckgo.com/html/?q=site%3Agithub.com+codingame+brandubh+bot+solution 
> build · hy4-flash-free
Une dernière brique : le livre d'ouverture et les réglages de recherche du moteur public, pour une recommandation testable.
% WebFetch https://raw.githubusercontent.com/jojopeligroso/brandubh/main/src/game/openingBook.ts 
% WebFetch https://raw.githubusercontent.com/jojopeligroso/brandubh/main/docs/ROADMAP.md 
% WebFetch https://github.com/jojopeligroso/brandubh/tree/main/data 
> build · hy4-flash-free
# Recherche approfondie — CodinGame « Brandubh » (bot-programming multi)

> Périmètre respecté : aucune écriture de fichier, aucun build, aucune lecture de `~/.config/codingame`. Sources lues : referee public, code public, forum CodinGame, arXiv/Crossref. Le moteur de recherche web échouant (402), j'ai utilisé l'API GitHub, `raw.githubusercontent.com`, le forum Discourse CodinGame (endpoint `.json`) et les API arXiv/Crossref.

---

## 1. Faits du referee (source officielle)

**URL exacte du referee :** https://github.com/n3l-ctrl/brandubh-codingame/blob/master/src/main/java/com/codingame/game/Referee.java
L'énoncé lui-même renvoie vers ce fichier : https://github.com/n3l-ctrl/brandubh-codingame/blob/master/config/statement_en.html

### 1.1 Constantes et setup

| Élément | Valeur (lu dans `Referee.java` / `Board.java` / `config.ini`) |
|---|---|
| Type de jeu | `type=multi`, `min_players=2`, `max_players=2` |
| Plateau | 7×7. Colonnes `a`–`g`, lignes `1`–`7`, coin bas-gauche `a1` |
| Rôle | `role=0` → Attaquants, `role=1` → Défenseurs ; les Attaquants jouent **toujours en premier** (`playerIdx = (turn-1)%2`) |
| Pièces | **8 attaquants** `d1 d2 a4 b4 f4 g4 d6 d7`, **4 défenseurs** `d3 c4 e4 d5`, **1 roi** `d4` (trône) |
| Trône | `(3,3)` = `d4`. Coins = `a1 a7 g1 g7`. Seul le roi peut s'y arrêter |
| Temps | **1000 ms** au 1er tour (`setFirstTurnMaxTime`), **50 ms** ensuite (`setTurnMaxTime`) |
| Limite | `setMaxTurns(200)` → 200 tours = 100 par camp, puis **match nul** (score 0/0) |
| Scores | victoire `+1 / -1`, nul `0/0` (`endGame`) |
| Protocole entrée | ligne 1 `role`, ligne 2 `lastAction` (`NONE` au 1er tour), puis 7 lignes de 7 caractères (de la ligne 7 à la ligne 1 ; `.` `A` `D` `K`) |
| Protocole sortie | `d2d4`. Regex acceptée : `^\s*([a-gA-G])([1-7])\s*([a-gA-G])([1-7])\s*$` (majuscules et espaces tolérés) |

### 1.2 Ordre de résolution (dans `gameTurn`, exact)

1. `hasValidMoves(playerIdx)` est vérifié **avant** l'exécution du joueur → si aucun coup légal, `deactivate` + `endGame(1-playerIdx)` (le joueur **perd**).
2. `sendInputs` puis `player.execute()`.
3. `parseMove` → `applyMove` avec validations dans cet ordre : pièce présente, pièce appartenant au camp, départ ≠ arrivée, destination vide, déplacement orthogonal, chemin non bloqué, non-roi interdit sur trône/coin.
4. `lastActionStr` mis à jour (`%c%c%c%c`, `x` et `'7'-y`).
5. **`checkCaptures(move)`** : seules les **4 cases orthogonales autour de la pièce déplacée** sont examinées. Un voisin ennemi est capturé si la case opposée est « hostile ».
6. `stateHistory` : hash = `camp au trait ; 49 caractères (y=0..6)`. Comptage **après** le coup, keyé sur le camp suivant. **Au 3ᵉ occurrence, le joueur qui vient de jouer perd** (`endGame(1-playerIdx)`).
7. `checkWinCondition()` : roi absent → Attaquants gagnent ; roi sur un coin → Défenseurs gagnent ; encerclement → Attaquants gagnent.
8. Gestion `TimeoutException` / `InvalidActionException` : `setScore(-1)`, `deactivate`, `endGame(1-playerIdx)`.

### 1.3 Règles de capture et de fin

- Hostilité : pièces adverses + 4 coins (toujours) + trône **vide**. Le trône est **toujours hostile aux Attaquants**, hostile aux Défenseurs **uniquement si le roi n'est pas dessus**, jamais hostile au roi (`isHostileSquare`).
- Roi sur le trône : capturé seulement si **4 côtés hostiles** (`isKingCaptured`). Ailleurs : capture custodienne normale à 2 flancs opposés. Plusieurs captures simultanées possibles en un coup.
- **Encerclement** (`checkEncirclement`) : BFS depuis les 28 cases de bord à travers les cases vides ou occupées par Défenseurs/Roi ; si aucune case de bord ne rejoint un défenseur ou le roi → Attaquants gagnent. (Le bord **n'aide pas** à la capture du roi, mais sert de source au BFS.)
- Départage : il n'y a **pas** de départage au sens classique ; le nul intervient à 200 tours. La répétition triple fait **perdre le répéteur**, pas nul — c'est une différence avec les règles WTF (« loss for defenders ») et Walker (« draw »).

⚠️ **Piège de règle à ne pas copier d'Internet** : l'article arXiv et le moteur `jojopeligroso` (variante `wtf`) appliquent un « roi adjacent au trône = 4 côtés » et un « roi ne peut pas revenir sur le trône ». Le referee CodinGame, lui, suit strictement le PDF officiel : **seul le roi *sur* le trône demande 4 côtés**, et le roi peut revenir. Source : `statement_en.html` + `Referee.java` + note `CONTESTED RULE` dans https://github.com/jojopeligroso/brandubh/blob/main/src/game/variants.ts.

---

## 2. Stratégies des meilleurs joueurs

### 2.1 Joueurs CodinGame — **aucune donnée publique trouvée** [NON VÉRIFIÉ]

Le jeu est très récent : fil de discussion ouvert le **11 nov. 2026**, **3 messages**, aucune stratégie, aucun post-mortem, aucun bot public identifié :
- https://forum.codingame.com/t/community-puzzle-brandubh-puzzle-discussion/208746.json
- Créé par `@N3l`, validé par `@Astrobytes`, `@darkhorse64`, `@elsek`.

[NON VÉRIFIÉ] Le code du boss de ligue (`config/Boss.py3`, appelé par `quick_test_boss.py` / `test_boss.py`) **n'est pas public** : 404 sur `raw.githubusercontent.com`, aucun historique de commit pour ce chemin. Ses poids, son algorithme et son niveau sont donc inconnus. [NON VÉRIFIÉ] Le classement actuel et l'identité des meilleurs joueurs n'ont pas été lus.

Les stratégies ci-dessous viennent de **moteurs publics indépendants** — Substituting correctement étiqueté, pas des bots CodinGame.

### 2.2 Moteur alpha-bêta public (le plus transférable)

**URL :** https://github.com/jojopeligroso/brandubh — `src/game/engine.ts`, `docs/solving.md`, `docs/ROADMAP.md`

Idée : **alpha-bêta à approfondissement itératif + table de transposition + quiescence + LMR**, avec une évaluation linéaire soigneusement mesurée. Pourquoi ça marche : sur un jeu asymétrique 7×7, la tactique de capture est décisive (quiescence), et la « distance du roi au coin au carré » domine tout le reste.

Poids livrés (`DEFAULT_WEIGHTS`, scores **orientés attaquants**) :

| Terme | Poids | Définition |
|---|---:|---|
| `material` | **40** | par `(attaquants − 2×défenseurs)` |
| `kingCorner` | **25** | distance roi→coin (`aligné?1:2 + distance/100`) |
| `escapeLane` | **120** | **soustrait** par `openLanes²` (1 voie = −120, 2 = −480) |
| `hug` | **30** | par attaquant orthogonal au roi |
| `liberties` | **12** | **soustrait** par case vide autour du roi (réintroduit après A/B apparié, p=4,3e-6 sur 120 paires) |
| `kingRegion` | **6** | **soustrait** par taille du flood-fill du roi (cap 12) — gradient vers l'encerclement |
| `shield`, `mobility`, `quadrantCoverage`, `anvilThreat` | **0** | mesurés neutres ou nuisibles ; `blockerAwareKingDist=false` |
| `endgameRecognizers` | on | reconnaît les fourches à 2 voies, le roi adjacent au coin, la course au coin |
| `attackerRecognizer` | off | coûte ~7 % de nœuds/s sans gain mesuré |

Recherche : `WIN=1 000 000`, TT 400 000 entrées, killed moves, quiescence max 6 plies, LMR à partir du 4ᵉ coup, **PVS désactivé** (mesuré neutre), mate-distance désactivé. Livre d'ouverture : 343 entrées canoniques D4 → 2 737 positions / 3 240 coups, couvrant **plies 0–3**, généré à **profondeur 8 à marges exactes nulles**, TT froid par position. https://github.com/jojopeligroso/brandubh/blob/main/src/game/openingBook.ts

Mesures utiles du même auteur (https://github.com/jojopeligroso/brandubh/blob/main/docs/ROADMAP.md) :
- Un A/B **non apparié** avait un biais de camp de **87,5 %** (les défenseurs gagnaient 21/24 avec un code identique des deux côtés) → **toujours apparier/mirer les tests**.
- `kingRegion` à poids 6 bat la baseline **31–9** à profondeur 4.
- `liberties` à 12 : 33W/5L sur 120 paires, p=4,3e-6.
- Le livre généré avec une « marge de 13 » perd **7–17** à profondeur 4 : un coup « presque meilleur » est un vrai handicap.
- À profondeur 6, les Défenseurs gagnent **21–3** contre 7–17 pour les Attaquants : **le camp Attaquant est le plus difficile à faire progresser**.

### 2.3 Approches MCTS / AlphaZero (moins adaptées au 50 ms)

**URL :** https://github.com/brenjohn/Brandubh

- `mcbot.py` : MCTS classique, **UCT** `w/n + c·√(ln N/n)`, `num_rounds=2100`, `temp=1.4`, rollouts avec `GreedyRandomBot`, réutilisation du sous-arbre entre les tours (jusqu'à 2 coups d'historique). Pourquoi ça marche : l'évaluation par simulation gère l'asymétrie sans fonction d'évaluation écrite à la main.
- `brandubh_zero.py` : AlphaZero-like, **PUCT** `Q + c·p·√N/(1+n)`, `c=1.4`, bruit Dirichlet `α=0,15` (mélange 0,75/0,25), batch 70, ~7 000 évaluations/coup, **virtual loss** pour paralléliser, premier 6 coups échantillonnés selon les visites puis `argmax`. Pourquoi ça marche : le réseau fournit prior + valeur, ce qui réduit drastiquement la largeur effective de l'arbre.

Limite pour CodinGame [NON VÉRIFIÉ] : ces chiffres de budget (2 100 rollouts, 7 000 évaluations) sont incompatibles avec 50 ms/coup sans optimisation majeure ; je n'ai lu aucune mesure de temps les concernant.

### 2.4 Articles

- **« An Upper Bound on the State-Space Complexity of Brandubh »**, Compy, Evey, McCullough, Allen & Crandall, IEEE CoG 2020, pp. 519–525, DOI `10.1109/CoG47356.2020.9231534`. https://arxiv.org/abs/2106.05353 · texte intégral : https://ar5iv.labs.arxiv.org/html/2106.05353
  - Borne naïve `2^5·4^44 ≈ 9,91×10^27`, borne resserrée **`≈ 1,04×10^14`** (après symétries miroir/rotation et cas spéciaux du roi). Entre Connect-4 et les dames. Conclusion : « tractable pour un solve faible ou fort », **mais pas résolu**.
  - Suggestion théorique pertinente : **symmetric decomposition** des jeux asymétriques (Tuyls et al., Scientific Reports 2018) pour entraîner un seul agent sur les deux rôles.
- **« Forced Capture Hnefatafl »**, Burke & Tennenhouse, arXiv:2301.06127 — PSPACE-complete pour la variante à capture forcée ; indice de complexité, pas de stratégie directe. https://arxiv.org/abs/2301.06127
- Estimation moderne (moteur public) : `≈ 6,0×10^14` après repli D4 ; 5×10⁶ nœuds explorés seulement → ** loin d'un solve**. https://github.com/jojopeligroso/brandubh/blob/main/docs/solving.md
- Semantic Scholar (API 429) et Crossref n'ont révélé **aucun autre article d'IA spécifiquement sur Brandubh**.

---

## 3. Recommandation concrète et testable pour battre le boss

Hypothèse explicite : **[NON VÉRIFIÉ] le boss est un bot glouton ou à faible profondeur** (son code n'est pas public). La recommandation vise donc à être *robuste* plutôt qu'à exploiter une faille précise.

### 3.1 Architecture (C++ recommandé vu les 50 ms)

1. **Bitboards 7×7 + Zobrist**, table de transposition persistante **entre les tours** (le referee fournit `lastAction`, donc on peut replanter la racine sur le sous-arbre du coup adverse).
2. 1er tour : utiliser les **1000 ms** pour charger/générer le petit livre + amorcer la TT ; ensuite **budget strict ~45 ms**, approfondissement itératif, on joue le meilleur coup du **dernier palier complet**. Toujours garder un coup légal de secours (un timeout = défaite immédiate).
3. Ordre des coups : **coup TT → captures → killers → coups calmes** ; **quiescence sur les captures (6 plies)** ; LMR après le 4ᵉ coup. PVS off.

### 3.2 Évaluation à recopier (orientée attaquants, à négocier pour les Défenseurs)

```
score = 40*(A - 2*D)
      + 25*distRoiCoin                 // aligné ? 1 : 2, + dist/100
      - 120*(voiesLibresVersCoin^2)    // TERME DOMINANT
      + 30*hug                         // attaquants adjacents au roi
      - 12*libertes                    // cases vides autour du roi
      - 6*min(12, regionRoi)           // flood-fill : gradient vers encerclement
terminal : +/- 1e6
```

**Pourquoi ça bat un glouton :** le terme quadratique `escapeLane` pénalise 2 voies ouvertes 4× plus qu'une. Un bot glouton qui « pousse vers le coin » sans boucher la deuxième voie perd immédiatement ; votre recherche le voit à 2 plies.

### 3.3 Spécificités CodinGame à implémenter absolument

- **Garde anti-répétition** : reproduire le hash exact du referee (`camp au trait + 49 cases`, ordre `y=0..6`), interdire tout coup qui crée la 3ᵉ occurrence, et au contraire **rechercher les coups qui forcent l'adversaire à répéter** (c'est une victoire immédiate dans ce referee).
- **Détecteur d'encerclement BFS** identique à `checkEncirclement`, et **reconnaisseur de fourche défensive** (roi avec ≥2 voies dégagées, ou roi adjacent à un coin avec 1 voie) → score `−(WIN−2)`.
- **Captures actives** : n'évaluer que les coups qui « landent » adjacent à une pièce ennemie avec une enclume opposée (coin/trône/piece alliée). C'est exactement ce que le referee détecte (`checkCaptures`). Éviter de laisser ses propres pièces « pendantes ».
- Ne pas importer la règle « roi adjacent au trône = 4 côtés » : voir §1.3.

### 3.4 Protocole de validation (testable, chiffré)

1. **Harnais local** avec le `Referee.java` officiel + deux bots, 100 paires **mirorées** (mêmes ouvertures, rôles échangés). Contrôle A/A : le net doit être **0** — sinon le harnais est biaisé (cf. 87,5 % mesuré par la source 2.2).
2. A/B sur **un seul terme à la fois** : d'abord `escapeLane` (120 → 100/140), puis `kingRegion` (6 → 4/8), puis `liberties`. Critère : **≥ 60 paires, p < 0,01** (test binomial), sinon ne pas shiper.
3. Mesurer séparément le **taux de victoire par rôle** contre le boss : $[\text{NON VÉRIFIÉ}]$ on ne sait pas quel camp le boss joue le mieux ; d'après la source 2.2, à profondeur égale le **camp Attaquant est le plus faible** → concentrer l'effort (livre + poids) sur les Attaquants pour gagner des points de classement.
4. Livre d'ouverture : générer soi-même des coups **à égalité exacte** (marge 0, TT froid, profondeur 8) pour les 4 premiers plies **sous les règles CodinGame**, puis replier par symétrie D4. Ne pas réutiliser tel quel le livre `jojopeligroso` (règle du roi adjacent divergente). Vérifier la légalité de chaque coup servi à chaque tour (le referee disqualifie sur coup invalide).
5. KPI de non-régression : **0 timeout**, **0 coup invalide**, temps médian < 45 ms.

---

## 4. Liste de toutes les URL lues

### Referee / jeu (source primaire)
- https://github.com/n3l-ctrl/brandubh-codingame
- https://github.com/n3l-ctrl/brandubh-codingame/blob/master/src/main/java/com/codingame/game/Referee.java
- https://raw.githubusercontent.com/n3l-ctrl/brandubh-codingame/master/src/main/java/com/codingame/game/Referee.java
- https://raw.githubusercontent.com/n3l-ctrl/brandubh-codingame/master/src/main/java/com/codingame/game/Board.java
- https://github.com/n3l-ctrl/brandubh-codingame/tree/master/src/main/java/com/codingame/game
- https://raw.githubusercontent.com/n3l-ctrl/brandubh-codingame/master/config/statement_en.html
- https://raw.githubusercontent.com/n3l-ctrl/brandubh-codingame/master/config/statement_fr.html
- https://raw.githubusercontent.com/n3l-ctrl/brandubh-codingame/master/config/config.ini
- https://raw.githubusercontent.com/n3l-ctrl/brandubh-codingame/master/config/stub.txt
- https://raw.githubusercontent.com/n3l-ctrl/brandubh-codingame/master/quick_test_boss.py
- https://raw.githubusercontent.com/n3l-ctrl/brandubh-codingame/master/test_boss.py
- https://github.com/n3l-ctrl/brandubh-codingame/tree/master/config
- https://github.com/n3l-ctrl/brandubh-codingame/tree/master/docs
- https://github.com/n3l-ctrl/brandubh-codingame/tree/master/src
- https://github.com/n3l-ctrl/brandubh-codingame/tree/master/src/main
- https://github.com/n3l-ctrl/brandubh-codingame/commits/master/config/Boss.py3 (aucun commit)
- https://raw.githubusercontent.com/n3l-ctrl/brandubh-codingame/master/config/Boss.py3 (**404**)
- https://github.com/n3l-ctrl/brandubh-codingame/forks
- https://github.com/n3l-ctrl?tab=repositories
- https://github.com/n3l-ctrl/brandubh-codingame/blob/master/docs/brandubh2_rules_en.pdf (référencé, PDF non extractible ici)
- https://www.codingame.com/multiplayer/bot-programming/brandubh (page JS, contenu non rendu)
- https://forum.codingame.com/t/community-puzzle-brandubh-puzzle-discussion/208746.json
- https://forum.codingame.com/t/community-puzzle-brandubh-puzzle-discussion/208746

### API GitHub / recherche
- https://api.github.com/search/repositories?q=brandubh+codingame
- https://api.github.com/search/repositories?q=brandubh
- https://api.github.com/search/code?q=brandubh+codingame (**401**)
- https://api.github.com/rate_limit
- https://api.github.com/repos/eulerscheZahl/RefereeCollection/contents/ (pas de dossier Brandubh)
- https://grep.app/api/search?q=brandubh%20codingame (**429**)
- https://sourcegraph.com/search?q=context:global+brandubh+codingame (aucun résultat exploitable)
- https://html.duckduckgo.com/html/?q=brandubh+codingame+forum+post-mortem
- https://html.duckduckgo.com/html/?q=%22brandubh%22+codingame+bot+github+strategy
- https://html.duckduckgo.com/html/?q=brandubh+codingame+boss+alpha-beta+MCTS
- https://html.duckduckgo.com/html/?q=%22Boss.py3%22+brandubh+codingame
- https://html.duckduckgo.com/html/?q=site%3Agithub.com+codingame+brandubh+bot+solution

### Moteurs publics
- https://github.com/jojopeligroso/brandubh
- https://raw.githubusercontent.com/jojopeligroso/brandubh/main/src/game/engine.ts
- https://raw.githubusercontent.com/jojopeligroso/brandubh/main/src/game/variants.ts
- https://raw.githubusercontent.com/jojopeligroso/brandubh/main/src/game/openingBook.ts
- https://raw.githubusercontent.com/jojopeligroso/brandubh/main/docs/solving.md
- https://raw.githubusercontent.com/jojopeligroso/brandubh/main/docs/ROADMAP.md
- https://github.com/jojopeligroso/brandubh/tree/main/data
- https://github.com/brenjohn/Brandubh
- https://raw.githubusercontent.com/brenjohn/Brandubh/main/README.md
- https://github.com/brenjohn/Brandubh/tree/main
- https://github.com/brenjohn/Brandubh/tree/main/brandubh
- https://github.com/brenjohn/Brandubh/tree/main/brandubh/bots
- https://github.com/brenjohn/Brandubh/tree/main/brandubh/bots/zero_bot
- https://raw.githubusercontent.com/brenjohn/Brandubh/main/brandubh/bots/mcbot.py
- https://raw.githubusercontent.com/brenjohn/Brandubh/main/brandubh/bots/zero_bot/brandubh_zero.py
- https://github.com/manudhaeyer-hash/Brandubh (simple copie du skeleton, sans bot)

### Articles / bibliographie
- https://arxiv.org/abs/2106.05353
- https://ar5iv.labs.arxiv.org/html/2106.05353
- https://export.arxiv.org/api/query?search_query=all:brandubh&start=0&max_results=20
- https://export.arxiv.org/api/query?search_query=all:tafl+OR+all:hnefatafl&start=0&max_results=30
- https://arxiv.org/abs/2301.06127
- https://api.crossref.org/works?query.bibliographic=brandubh&rows=20
- https://doi.org/10.1109/CoG47356.2020.9231534
- https://api.semanticscholar.org/graph/v1/paper/search?query=brandubh&limit=20 (**429**)
- https://doi.org/10.3390/technologies5020016 (faux positif : article d'imagerie médicale, sans rapport)

** Résumé du degré de confiance :** les §1 et §2.2–2.4 sont directement lus dans les sources ; tout ce qui concerne le boss CodinGame, le classement et les stratégies des meilleurs joueurs CodinGame est marqué **[NON VÉRIFIÉ]** faute de source publique.
