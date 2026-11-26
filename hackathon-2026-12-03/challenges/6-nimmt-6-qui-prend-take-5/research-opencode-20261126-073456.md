<!-- opencode model: hy4-flash-free, 2026-11-26T07:49:13+03:00 -->

> build · hy4-flash-free
Je vais lancer la recherche en parallèle sur plusieurs sources.
✗ Web Search via OpenCode "CodinGame "6 nimmt" take 5 game statement rules" failed
Error: Web search request failed (HTTP 402)
✗ Web Search via OpenCode "github hub2/nimmt6 TimothyJones/6nimmt-bot denisakkavim/sixnimmt" failed
Error: Web search request failed (HTTP 402)
✗ Web Search via OpenCode "github CodinGame 6 nimmt referee bot programming game" failed
Error: Web search request failed (HTTP 402)
✗ Web Search via OpenCode "eulerscheZahl RefereeCollection 6 nimmt take 5" failed
Error: Web search request failed (HTTP 402)
> build · hy4-flash-free
La recherche web est indisponible (HTTP 402). Je bascule sur l'API GitHub et les URLs directes.
% WebFetch https://api.github.com/repos/TimothyJones/6nimmt-bot 
% WebFetch https://api.github.com/repos/hub2/nimmt6 
% WebFetch https://api.github.com/repos/denisakkavim/sixnimmt 
% WebFetch https://api.github.com/search/repositories?q=6nimmt+in:name 
> build · hy4-flash-free
Les trois dépôts demandés existent. Je cherche maintenant le referee officiel et le contenu des dépôts.
% WebFetch https://api.github.com/search/repositories?q=org%3ACodinGame+nimmt 
% WebFetch https://raw.githubusercontent.com/hub2/nimmt6/master/README.md 
% WebFetch https://raw.githubusercontent.com/denisakkavim/sixnimmt/main/README.md 
% WebFetch https://api.github.com/search/repositories?q=org%3AeulerscheZahl+RefereeCollection 
✗ WebFetch https://api.github.com/repos/TimothyJones/6nimmt-bot/contents failed
Error: StatusCode: non 2xx status code (404 GET https://api.github.com/repos/TimothyJones/6nimmt-bot/contents)
> build · hy4-flash-free
✱ Grep "nimmt|take.?5|Take5|take5" in . · 36 matches
% WebFetch https://raw.githubusercontent.com/denisakkavim/sixnimmt/main/docs/bots.md 
% WebFetch https://api.github.com/repos/hub2/nimmt6/contents 
% WebFetch https://api.github.com/search/repositories?q=codingame+%22take+5%22+referee 
> build · hy4-flash-free
→ Read hackathon-2026-12-03/challenges/6-nimmt-6-qui-prend-take-5/statement.md 
→ Read hackathon-2026-12-03/challenges/6-nimmt-6-qui-prend-take-5/research.md 
% WebFetch https://raw.githubusercontent.com/hub2/nimmt6/master/nimmt.py 
% WebFetch https://raw.githubusercontent.com/denisakkavim/sixnimmt/main/docs/uncertainty-bots.md 
% WebFetch https://raw.githubusercontent.com/denisakkavim/sixnimmt/main/docs/strategy-families.md 
> build · hy4-flash-free
% WebFetch https://www.codingame.com/multiplayer/bot-programming/6-nimmt-6-qui-prend-take-5 
% WebFetch https://raw.githubusercontent.com/hub2/nimmt6/master/nimmt_gym.py 
% WebFetch https://api.github.com/repos/denisakkavim/sixnimmt/contents/experiments 
% WebFetch https://api.github.com/repos/eulerscheZahl/RefereeCollection/contents 
% WebFetch http://export.arxiv.org/api/query?search_query=all:%226%20nimmt%22&start=0&max_results=10 
✗ WebFetch https://api.semanticscholar.org/graph/v1/paper/search?query=6%20nimmt%20strategy%20artificial%20intelligence&limit=10&fields=title,url,abstract,year failed
Error: StatusCode: non 2xx status code (429 GET https://api.semanticscholar.org/graph/v1/paper/search?query=6%20nimmt%20strategy%20artificial%20intelligence&limit=10&fields=title,url,abstract,year)
> build · hy4-flash-free
✗ WebFetch https://grep.app/api/search?q=CHOOSE_LINE_TO_PICK failed
Error: StatusCode: non 2xx status code (429 GET https://grep.app/api/search?q=CHOOSE_LINE_TO_PICK)
✗ WebFetch https://api.github.com/search/code?q=CHOOSE_LINE_TO_PICK failed
Error: StatusCode: non 2xx status code (401 GET https://api.github.com/search/code?q=CHOOSE_LINE_TO_PICK)
% WebFetch https://raw.githubusercontent.com/denisakkavim/sixnimmt/main/experiments/mcmc/REPORT.md 
% WebFetch https://raw.githubusercontent.com/denisakkavim/sixnimmt/main/docs/comparisons.md 
% WebFetch http://export.arxiv.org/api/query?search_query=all:%22Take%205%22%20AND%20all:%22card%20game%22&start=0&max_results=10 
✗ WebFetch https://api.semanticscholar.org/graph/v1/paper/search?query=six%20nimmt%20board%20game&limit=5&fields=title,url,year failed
Error: StatusCode: non 2xx status code (429 GET https://api.semanticscholar.org/graph/v1/paper/search?query=six%20nimmt%20board%20game&limit=5&fields=title,url,year)
> build · hy4-flash-free
⚙ execute {"code":"const urls = [\n  [\"https://www.codingame.com/services/Challenge/getChallengeStatement\", [\"6-nimmt-6-qui-prend-take-5\"]],\n  [\"https://www.codingame.com/services/Challenge/findById\", [\"6-nimmt-6-qui-prend-take-5\"]],\n];\nfor (const [url, body] of urls) {\n  try {\n    const r = await fetch(url, {method:\"POST\", headers:{\"Content-Type\":\"application/json\"}, body: JSON.stringify(body)});\n    const t = await r.text();\n    console.log(url, r.status, t.slice(0,1500));\n  } catch(e) { console.log(url, \"ERR\", String(e)); }\n}"} 
> build · hy4-flash-free
✱ Grep "services|statement|show" in bot-programming/tools/cg.py · 4 matches
> build · hy4-flash-free
→ Read bot-programming/tools/cg.py [offset=100, limit=30]
> build · hy4-flash-free
→ Read bot-programming/tools/cg.py [offset=1, limit=45]
> build · hy4-flash-free
⚙ execute {"code":"for (const u of [\n \"https://www.codingame.com/api/v1/puzzles/6-nimmt-6-qui-prend-take-5\",\n \"https://www.codingame.com/api/v1/docs/api\"\n]) {\n  try { const r = await fetch(u); const t = await r.text(); console.log(u, r.status, t.slice(0,1200)); }\n  catch(e) { console.log(u, \"ERR\", String(e)); }\n}"} 
> build · hy4-flash-free
→ Read bot-programming/IMPROVEMENTS.md [offset=780, limit=25]
→ Read hackathon-2026-12-03/challenges/6-nimmt-6-qui-prend-take-5/critique.md 
% WebFetch https://raw.githubusercontent.com/denisakkavim/sixnimmt/main/experiments/performance/REPORT.md 
% WebFetch https://raw.githubusercontent.com/denisakkavim/sixnimmt/main/docs/game-rules.md 
% WebFetch https://raw.githubusercontent.com/denisakkavim/sixnimmt/main/docs/uncertainty-model.md 
> build · hy4-flash-free
⚙ execute {"code":"for (const u of [\n \"http://export.arxiv.org/api/query?search_query=all:%22six%20nimmt%22&start=0&max_results=5\",\n \"http://export.arxiv.org/api/query?search_query=all:%226%20qui%20prend%22&start=0&max_results=5\",\n \"http://export.arxiv.org/api/query?search_query=all:%22simultaneous%20move%22%20AND%20all:%22Monte%20Carlo%22%20AND%20all:%22card%20game%22&start=0&max_results=5\",\n \"https://api.semanticscholar.org/graph/v1/paper/search?query=6nimmt&limit=5\"\n]) {\n  try { const r = await fetch(u); const t = await r.text(); console.log(\"URL\",u,\"STATUS\",r.status,\"LEN\",t.length, t.slice(0,900).replace(/\\s+/g,\" \")); }\n  catch(e) { console.log(u,\"ERR\",String(e)); }\n}"} 
> build · hy4-flash-free
⚙ execute {"code":"for (const u of [\n \"http://export.arxiv.org/api/query?id_list=1502.05477\",\n \"http://export.arxiv.org/api/query?search_query=all:%22Information%20Set%20Monte%20Carlo%20Tree%20Search%22&start=0&max_results=5\",\n \"https://api.github.com/search/repositories?q=codingame+nimmt+in%3Aname%2Cdescription%2Creadme\",\n \"https://api.github.com/search/repositories?q=take5+bot+in%3Aname%2Cdescription\"\n]) {\n  try { const r = await fetch(u); const t = await r.text(); console.log(\"URL\",u,\"STATUS\",r.status); console.log(t.slice(0,1300).replace(/\\s+/g,\" \")); }\n  catch(e) { console.log(u,\"ERR\",String(e)); }\n}"} 
> build · hy4-flash-free
% WebFetch https://raw.githubusercontent.com/denisakkavim/sixnimmt/main/examples/arena-uncertainty.json 
% WebFetch https://raw.githubusercontent.com/hub2/nimmt6/master/sb3_train.py 
✗ WebFetch https://api.github.com/repos/TimothyJones/6nimmt-bot/git/trees/master?recursive=1 failed
Error: StatusCode: non 2xx status code (409 GET https://api.github.com/repos/TimothyJones/6nimmt-bot/git/trees/master?recursive=1)
% WebFetch https://api.github.com/repos/denisakkavim/sixnimmt/contents 
> build · hy4-flash-free
⚙ execute {"code":"for (const u of [\n \"https://www.codingame.com/forum/search.json?q=6%20nimmt\",\n \"https://www.codingame.com/forum/search?q=6%20nimmt\",\n \"https://api.github.com/repos/Zweer/codingame/git/trees/master?recursive=1\"\n]) {\n  try { const r = await fetch(u); const t = await r.text(); console.log(\"URL\",u,\"STATUS\",r.status,\"LEN\",t.length); console.log(t.slice(0,700).replace(/\\s+/g,\" \")); if(u.includes(\"Zweer\")){ const m=[...t.matchAll(/\"path\":\"([^\"]*nimmt[^\"]*)\"/gi)].map(x=>x[1]); console.log(\"NIMMT PATHS\", m.slice(0,20)); } }\n  catch(e) { console.log(u,\"ERR\",String(e)); }\n}"} 
> build · hy4-flash-free
→ Read hackathon-2026-12-03/challenges/6-nimmt-6-qui-prend-take-5/plan.md 
→ Read hackathon-2026-12-03/challenges/6-nimmt-6-qui-prend-take-5/solutions/python/bot.py 
> build · hy4-flash-free
# 6 nimmt! / Take 5 — recherche bot (4 joueurs, jeu simultané)

## 1) Faits du referee (constantes, ordre de résolution, départages) — avec URLs

- **Aucun referee officiel public trouvé** : `org:CodinGame nimmt` → 0 dépôt (https://api.github.com/search/repositories?q=org%3ACodinGame+nimmt) et `eulerscheZahl/RefereeCollection` ne contient **aucun** dossier 6-nimmt/Take5 (https://api.github.com/repos/eulerscheZahl/RefereeCollection/contents). Sources officielles accessibles : page du jeu https://www.codingame.com/multiplayer/bot-programming/6-nimmt-6-qui-prend-take-5 (coquille JS) et métadonnées publiques https://www.codingame.com/api/v1/puzzles/6-nimmt-6-qui-prend-take-5 (`bot-programming`, rating 4.9). Le *statement* est derrière token — **non lu en ligne**.
- **Constantes** [NON VÉRIFIÉ en ligne — copie locale du statement `statement.md`] : 104 cartes, **4 joueurs**, **10 cartes/joueur**, 4 lignes de départ, **5 cartes/ligne**, **5 manches** ; temps ≤ 100 ms/tour, 1000 ms au 1er tour. Corroboré partiellement par https://raw.githubusercontent.com/denisakkavim/sixnimmt/main/docs/game-rules.md (104 cartes, 4 lignes, 5 cartes, 10 cartes/donne).
- **Barème des vaches** (vérifié hors referee) : `55 → 7`, autre multiple de 11 → 5, multiple de 10 → 3, multiple de 5 → 2, sinon 1. URLs : https://raw.githubusercontent.com/hub2/nimmt6/master/nimmt.py et https://raw.githubusercontent.com/denisakkavim/sixnimmt/main/docs/game-rules.md (total 171 vaches dans le jeu complet).
- **Ordre de résolution** (vérifié, concordant avec le statement local) : cartes révélées simultanément puis **résolues par valeur croissante** ; chaque carte va sur la ligne dont la dernière carte est la **plus grande strictement inférieure** ; si cette ligne a déjà 5 cartes → le joueur **capture les 5** et sa carte démarre une ligne neuve ; si aucune ligne ne qualifie → phase `CHOOSE_LINE_TO_PICK`, le joueur capture la ligne choisie et sa carte la remplace. URLs : https://raw.githubusercontent.com/denisakkavim/sixnimmt/main/docs/game-rules.md, https://raw.githubusercontent.com/hub2/nimmt6/master/nimmt.py (`Board.update` trie bien par `x[1]` = valeur de carte).
- **Départages** : le referee n’en impose pas sur le choix de ligne (c’est au bot de choisir). Règle de politique dominante dans les implémentations publiques : **moins de vaches, puis index de ligne croissant** — https://raw.githubusercontent.com/denisakkavim/sixnimmt/main/docs/bots.md et https://raw.githubusercontent.com/denisakkavim/sixnimmt/main/docs/strategy-families.md. Égalité de coût entre cartes → carte la plus basse (https://raw.githubusercontent.com/denisakkavim/sixnimmt/main/docs/uncertainty-model.md, §9).
- **Piège de protocole local** [NON VÉRIFIÉ en ligne — sonde stdin du 2026-11-26] : la phase est envoyée en **dernière** ligne du tour, pas en première comme dans l’énoncé ; `-999` = disqualifié. Voir section 4 (fichiers locaux). C’est déjà corrigé dans la v1.
- **Différence de règles à ne pas importer** : le moteur `sixnimmt` s’arrête à **66 points** (https://raw.githubusercontent.com/denisakkavim/sixnimmt/main/docs/game-rules.md), alors que CodinGame joue **5 manches** [NON VÉRIFIÉ en ligne]. Ne pas copier la condition d’arrêt.

## 2) Stratégies des meilleurs joueurs (URL + idée + pourquoi)

- **hub2/nimmt6** — https://github.com/hub2/nimmt6 (description API : « *The best 6 nimmt bot in the world* »), https://raw.githubusercontent.com/hub2/nimmt6/master/README.md (« 105kk steps TRPO ») : apprentissage par renforcement, obs = plateau + cartes passées + taille de main, env gym 16 parallèles (https://raw.githubusercontent.com/hub2/nimmt6/master/nimmt_gym.py). **Pourquoi** : pas de heuristics codées à la main, politique apprise contre adversaires aléatoires. **Mais** [NON VÉRIFIÉ] : aucun benchmark publié, aucun poids entraîné dans le repo, et `sb3_train.py` (https://raw.githubusercontent.com/hub2/nimmt6/master/sb3_train.py) utilise **PPO 5 M steps**, pas TRPO 105 M — contradiction avec le README.
- ⚠️ **Ne pas copier tel quel** (vérifié dans le code lu) : dans `nimmt.py`, `AI.pick_lane` fait `sorted([(i, score), ...])[0][0]` → trie sur l’**index avant le score**, donc choisit toujours la ligne 0 malgré le commentaire ; dans `nimmt_gym.py`, `_smallest_row()` ne met jamais à jour l’index retourné et `_play()` trie les actions par **index joueur** (`key=lambda x: x[0]`) au lieu de la valeur de carte. Source : https://raw.githubusercontent.com/hub2/nimmt6/master/nimmt.py, https://raw.githubusercontent.com/hub2/nimmt6/master/nimmt_gym.py.
- **TimothyJones/6nimmt-bot** — https://api.github.com/repos/TimothyJones/6nimmt-bot : repo **vide** (`size: 0`, `/contents` → 404, arbre git → 409). **[NON VÉRIFIÉ]** toute stratégie qui lui serait attribuée.
- **denisakkavim/sixnimmt** — https://github.com/denisakkavim/sixnimmt, https://raw.githubusercontent.com/denisakkavim/sixnimmt/main/docs/bots.md, https://raw.githubusercontent.com/denisakkavim/sixnimmt/main/docs/strategy-families.md : 8 baselines déterministes (`random`, `lowest_card`, `highest_card`, `lowest_fitting_card`, `highest_fitting_card`, `closest_gap`, `coldest_row`, `hand_flexibility`) + lignes « cheapest row, tie by index ». *Pourquoi* : `closest_gap` saisit l’opportunité immédiate ; `hand_flexibility` garde une couverture `D(H) = moyenne_{x=1..104} min_{h∈H}|x−h|` pour survivre aux plateaux futurs.
- **Row-pick avancé** — https://raw.githubusercontent.com/denisakkavim/sixnimmt/main/docs/bots.md (`hand_aware_row_choice`, `max_extra_penalty`) : on n’accepte une ligne plus chère que si, **après remplacement par la carte jouée**, le nombre de cartes restantes qui « fittent » augmente. *Pourquoi* : la ligne choisie façonne les 9–10 tours suivants ; le coût immédiat seul est myope.
- **Burn/bait contrôlés** — même URL + https://raw.githubusercontent.com/denisakkavim/sixnimmt/main/docs/strategy-families.md (`controlled_burn` avec `K ∈ {3,5,8}`, `count_threshold_bait`) : provoquer volontairement une capture peu coûteuse, ou viser une ligne pleine quand beaucoup de cartes intermédiaires peuvent tomber. *Pourquoi* : un petit sacrifice maintenant évite une grosse capture plus tard et peut piéger un adversaire trop confiant.
- **Monte-Carlo sur mains adverses** — https://raw.githubusercontent.com/denisakkavim/sixnimmt/main/docs/uncertainty-model.md et https://raw.githubusercontent.com/denisakkavim/sixnimmt/main/docs/uncertainty-bots.md : inférence **jointe** (mains cachées cohérentes + étiquette de politique + ε ~ Beta(1,1)) par MCMC `emcee`, 3 politiques (`lowest_card`, `highest_card`, `closest_gap`), `learned_mixture`, **64 mondes**, **5000 burn-in**, échelle logit **2.5**, **512 rollouts/candidate**, horizon 1 ou 2, objectifs `mean / pickup_probability / threshold_exceedance / upper_tail`, **mêmes mondes et mêmes tirages pour toutes les cartes** (common random numbers). *Pourquoi* : réduit la variance comparative ; apprend du comportement observable sans voir les mains.
- **Calibration chiffrée** — https://raw.githubusercontent.com/denisakkavim/sixnimmt/main/experiments/mcmc/REPORT.md : 200 steps de burn-in → ≈ **0,99 vache** de regret en moyenne (cas 5 joueurs/3 tours) ; 5000 steps → erreur ε ≈ 0,001–0,038 ; 64 mondes/512 rollouts → **99,4 %** de sélection de la meilleure carte de référence. https://raw.githubusercontent.com/denisakkavim/sixnimmt/main/experiments/performance/REPORT.md : 8,6–33,4× plus rapide mais encore **0,6–2,0 s/décision** → **incompatible avec le budget 100 ms de CodinGame**.
- **Post-mortems CodinGame/forum** : recherche forum `https://www.codingame.com/forum/search?q=6%20nimmt` (page JS, contenu non lu) → **[NON VÉRIFIÉ]**, aucun post-mortem exploité.
- **Papers** : arXiv `all:"6 nimmt"`, `all:"six nimmt"`, `all:"6 qui prend"` → **0 résultat** (URLs en §4) ; Semantic Scholar → **429** (non lu). **[NON VÉRIFIÉ] : aucun papier spécifique à 6 nimmt trouvé.** Méthodes générales vérifiées seulement : TRPO https://arxiv.org/abs/1502.05477 (lien avec la revendication hub2) et ISMCTS https://arxiv.org/abs/2005.07156 (jeu à information imparfaite) — **transpositions, pas des références 6-nimmt**.

## 3) Recommandation concrète et testable pour battre le boss

- **Garder la base v1** (déjà soumise) : coût binomial `P(Binôme(adversaires, trou/inconnues) ≥ places libres) × vaches de la ligne`, départage `coût, trou, carte`, ligne `vaches, index` ; parsing avec phase en dernière ligne. Local : `solutions/python/bot.py`, `plan.md`, `critique.md`. Résultat observé : 4/4 secondes places, bat le rang ~108, perd contre le rang ~2.
- **Passer au MC « joint mais léger »** : au lieu du binomial indépendant, tirer **32–64 mondes cohérents** (les cartes inconnues de la manche réparties en 3×10 + reste, conditionnées sur les cartes vues), avec 3 politiques adverses pondérées apprises (`lowest_card`, `highest_card`, `closest_gap`) et un ε approximé. Pour **chaque carte**, résoudre le tour complet avec la règle officielle (tri par valeur croissante, ligne « greatest lower end », capture 6ᵉ, PICK le cas échéant), **mêmes mondes/tirages pour toutes les cartes**, choisir `argmin E[vaches]`, départage carte basse. C’est la version embarquable du modèle de https://raw.githubusercontent.com/denisakkavim/sixnimmt/main/docs/uncertainty-model.md ; garder le MCMC lourd **hors ligne** pour calibrer les poids de politiques.
- **Ajouter un objectif de risque** si le boss punit les captures : en plus de la moyenne, suivre `P(capture > 0)` et la moyenne du pire 20 % (https://raw.githubusercontent.com/denisakkavim/sixnimmt/main/docs/uncertainty-bots.md). Ne pas surenchérir en rollouts : le goulot est l’inférence, pas le nombre de tirages (https://raw.githubusercontent.com/denisakkavim/sixnimmt/main/experiments/mcmc/REPORT.md).
- **Row-pick : A/B tester `max_extra_penalty ∈ {0, 2, 4}`** avec le compteur de cartes « fittees » restantes (hand-aware) ; **`controlled_burn` K ∈ {3,5}`** uniquement si le MC montre un coût espéré strictement plus bas que la carte de fallback. Ne jamais implémenter le pick de `hub2` (bug d’index constaté).
- **Protocole de test** : ≥ 200 matchs appariés par graine contre le même boss, châssis par châssis `(v1) → (+ hand-aware) → (+ MC 1 tour) → (+ MC 2 tours) → (+ bait)`, métriques = vaches moyennes/manche, taux de capture, % de victoires, IC bootstrap, seuil pratique 2 points (méthode : https://raw.githubusercontent.com/denisakkavim/sixnimmt/main/docs/comparisons.md). Réinitialiser l’ensemble des cartes vues à chaque main de 10 cartes ; ne pas transposer la fin à 66 points.
- **Timing** : 100 ms/tour impose de précalculer les tables de coût et de borner le MC (ex. 48 mondes × 32 résolutions, ~1 500 évaluations) ; mesurer le pire cas sur la sonde stdin avant tout ajustement.

## 4) Liste de toutes les URL lues

**Officiel / CodinGame**
- https://www.codingame.com/multiplayer/bot-programming/6-nimmt-6-qui-prend-take-5 (JS uniquement)
- https://www.codingame.com/api/v1/puzzles/6-nimmt-6-qui-prend-take-5
- https://www.codingame.com/api/v1/docs/api
- https://www.codingame.com/services/Challenge/getChallengeStatement (422) ; https://www.codingame.com/services/Challenge/findById (422)
- https://www.codingame.com/forum/search?q=6%20nimmt (JS, non lu) ; https://www.codingame.com/forum/search.json?q=6%20nimmt (404)

**GitHub API / dépôts**
- https://api.github.com/search/repositories?q=6nimmt+in:name
- https://api.github.com/search/repositories?q=org%3ACodinGame+nimmt (0)
- https://api.github.com/search/repositories?q=org%3AeulerscheZahl+RefereeCollection
- https://api.github.com/search/repositories?q=codingame+%22take+5%22+referee (0)
- https://api.github.com/search/repositories?q=codingame+nimmt+in%3Aname%2Cdescription%2Creadme
- https://api.github.com/search/repositories?q=take5+bot+in%3Aname%2Cdescription
- https://api.github.com/search/code?q=CHOOSE_LINE_TO_PICK (401, non authentifié)
- https://api.github.com/repos/eulerscheZahl/RefereeCollection/contents
- https://api.github.com/repos/hub2/nimmt6 ; /contents
- https://raw.githubusercontent.com/hub2/nimmt6/master/README.md ; /nimmt.py ; /nimmt_gym.py ; /sb3_train.py
- https://api.github.com/repos/TimothyJones/6nimmt-bot ; /contents (404) ; /git/trees/master?recursive=1 (409)
- https://api.github.com/repos/denisakkavim/sixnimmt ; /contents ; /contents/experiments
- https://api.github.com/repos/Zweer/codingame/git/trees/master?recursive=1 (404)
- https://raw.githubusercontent.com/denisakkavim/sixnimmt/main/README.md ; /docs/bots.md ; /docs/game-rules.md ; /docs/strategy-families.md ; /docs/uncertainty-bots.md ; /docs/uncertainty-model.md ; /docs/comparisons.md ; /examples/arena-uncertainty.json ; /experiments/mcmc/REPORT.md ; /experiments/performance/REPORT.md
- https://grep.app/api/search?q=CHOOSE_LINE_TO_PICK (429)

**Papers / recherche**
- https://api.semanticscholar.org/graph/v1/paper/search?query=6%20nimmt%20strategy%20artificial%20intelligence&limit=10&fields=title,url,abstract,year (429)
- https://api.semanticscholar.org/graph/v1/paper/search?query=six%20nimmt%20board%20game&limit=5&fields=title,url,year (429)
- https://api.semanticscholar.org/graph/v1/paper/search?query=6nimmt&limit=5 (429)
- http://export.arxiv.org/api/query?search_query=all:%226%20nimmt%22 (0) ; `all:%22Take%205%22%20AND%20all:%22card%20game%22` (0) ; `all:%22six%20nimmt%22` (0) ; `all:%226%20qui%20prend%22` (0) ; `all:%22Information%20Set%20Monte%20Carlo%20Tree%20Search%22` (8) ; `id_list=1502.05477`
- https://arxiv.org/abs/1502.05477 ; https://arxiv.org/abs/2005.07156

**Sources locales sans URL publique** (consultées, non modifiées) : `hackathon-2026-12-03/challenges/6-nimmt-6-qui-prend-take-5/{statement.md, research.md, plan.md, critique.md, solutions/python/bot.py}` ; `bot-programming/{IMPROVEMENTS.md, ALGO.md, games.json}` ; `bot-programming/tools/cg.py`.
