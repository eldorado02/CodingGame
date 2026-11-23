# PASSATION — Campagne CodinGame « tous les bots en Ligue Légende »

Note de reprise pour une nouvelle session Claude. Rédigée le 23/11/2026 après arrêt de la session précédente.

## 1. Mission et règles non négociables

Objectif : amener **les 100 jeux** `bot-programming` en **Ligue Légende**. Boucle rotative : dès qu'un jeu monte de ligue, on repart de zéro sur ce jeu (énoncé → recherche → plan → critique → 4 langages → fuzz → test games → soumission C++), puis on passe à un autre jeu.

À chaque tentative, respecter strictement :

1. **Lire/écrire le `README.md` du challenge avant toute modification de code.**
2. Relire l'énoncé officiel via `python3 bot-programming/tools/cg.py show <pretty_id>` (surtout après promotion : les règles changent, ex. Elemental Wars Wood 1).
3. Deep search : GitHub (API + raw), referee, arXiv / Semantic Scholar / Papers with Code / post-mortems. Noter **toutes** les sources dans `research.md`. `websearch` renvoie HTTP 402 → utiliser l'API GitHub et `webfetch`.
4. Chaînes de modèles demandées par l'utilisateur :
   - recherche : `deepseek-v4.1-flash-free` → `longcat-2.5-free` → `hy4-flash-free` → `nemotron-3.5-super-vl-free`
   - implémentation : `deepseek-v4.1-flash-free` → `flint-40b-free` → `longcat-2.5-free` → `mimo-v2.6-flash-free`
5. **4 langages** systématiques : Python (référence fuzz), C, C++, Rust.
6. Fuzz différentiel **≥200 cas identiques** (`bash tests/run.sh`).
7. **≥3/5 test games gagnés** contre le boss / le meilleur joueur avant soumission.
8. Soumission **toujours en C++** avec `--language C++`. Chaque ligne de sortie doit être flushée (`std::flush` / `flush`, avec `sync_with_stdio(false)`).
9. Journaliser **chaque** tentative dans `bot-programming/IMPROVEMENTS.md` et dans le `critique.md` du challenge, au format : **constat / hypothèse / sources / action / résultat / leçon** — y compris les échecs.
10. Ne **jamais** afficher ni committer `~/.config/codingame/token`. Exclure du git : `__pycache__`, `*.pyc`, `target/`, exécutables de build.
11. Promotion « classique » : `percentage=100` **et** `score >= boss.score`. Tant que ce n'est pas atteint, `eligible_for_promotion=False`. `boss_battles` ne compte que les victoires contre le boss.
12. Si l'utilisateur dit « arrête tout » : arrêt immédiat, plus aucune commande, test ou commit.

## 1bis. Contexte du projet (structure et historique)

Complément indispensable à la section 1 : **où on est, pourquoi, et comment le dépôt est organisé**.

### Arborescence et rôles

- `bot-programming/games.json` — **liste autoritaire des 100 jeux** de la campagne (format JSON : liste de 100 entrées). C'est elle qui fixe le périmètre, pas le contenu du dossier `challenges/`.
- `bot-programming/PROGRESS.md` — table de suivi des 100 jeux. Règle de priorité : jeux déjà soumis classés par ligue **décroissante** (Silver avant Bronze, etc.), puis par rang de ligue **croissant** ; les jeux sans soumission viennent ensuite, du plus ancien au plus récent. Un seul challenge actif à la fois ; reprendre au premier `À FAIRE` quand le quota sous-agent revient.
- `bot-programming/PLAYBOOK.md` — **consigne de travail** lue avant chaque deep search / plan / critique / implémentation. Définit l'ordre de priorité des sources : énoncé officiel → métadonnées des test games réels → source du referee → dépôts GitHub de solutions → post-mortems/blogs → ArXiv / Semantic Scholar → agrégateurs (Papers with Code, Gists, Codeforces…). Règle de méfiance : une stratégie venue d'un dépôt public est une **hypothèse**, validée seulement par un test game contre le boss ; si une source contredit l'énoncé, croire les **métadonnées du test game** et documenter la contradiction.
- `bot-programming/IMPROVEMENTS.md` — journal global de toutes les tentatives, au format constat / hypothèse / sources / action / résultat / leçon.
- `bot-programming/tools/cg.py` — client API (lit le token dans `~/.config/codingame/token` ; ne jamais l'afficher). Sous-commandes : `list`, `show`, `stub`, `opponents`, `get <game> [progress|battles|leaderboard|submissions|stats|opponents]`, `test`, `submit`. Option `--extra` pour injecter des champs JSON dans la requête.
- `bot-programming/games/<game>/logs/test-*.log` — logs des test games : `options: seed=...`, `stdout:` des deux agents, `summary:`, `scores:`, `ranking`, tooltips. C'est la vérité terrain pour comprendre une défaite.
- `dsa/` — référence d'algorithmes (graphes, BFS, géométrie entière, etc.) utilisée pendant la recherche.
- `hackathon-2026-12-03/challenges/<id>/` — **code et docs par jeu**. Contrat de documentation : `README.md` (guide de reprise, à écrire **avant** toute modification), `statement.md` (énoncé officiel archivé), `research.md` (sources), `plan.md` (algorithme), `critique.md` (risques + bilan des tentatives), `solutions/{python,c,cpp,rust}` (4 implémentations byte-identiques), `tests/` (cas déterministes et/ou fuzz).
- `DFS_1/`, `Takuzu_mode_easy/`, `Tic_Tac_Toe_Engine/`, `reconstruct_tree/`, `out.txt` — éléments **étrangers à la campagne** ; ne pas toucher.

### Harnais de validation

`tests/gen_fuzz.py` génère 200 fichiers `tests/fuzz/case_*.in` reproductibles ; `tests/run.sh` compile les 4 langages, exécute chaque cas sur les 4 implémentations et compare les sorties **octet par octet**. Python sert de référence. Critère : `200/200 cas identiques dans 4 langages` + ≥3/5 test games contre le boss avant soumission C++.

### Historique récent (commits)

`187d3cb` docs Othello/EW → `787d8a4` Elemental Wars v6 `41788154` → `151eb60` Othello bitboard `41787653` → `171afc1` Elemental Wars v3 `41787544` → `dd8ef70`/`e198aa6` Shobu `41786861` (promu Wood 2) → `ff0fd27` Connect 4 `41786612` → `33fa2bd` Elemental Wars Wood 3 `41786190` → `cb0a05a` Brandubh → plus tôt Ataxx, Chain Reaction, Xmas Rush. Convention de message : `<Jeu> : description (fuzz 200/200, x/5), submission <id>`.

### Connaissances transversales acquises

- Les règles changent **à chaque ligue** : toujours relire l'énoncé et chercher un nouveau boss après promotion. Exemples déjà rencontrés : Winter 2024 Wood 3 active `HARVESTER` malgré l'énoncé générique ; Olymbits passe d'1 à 4 mini-jeux en Silver ; Elemental Wars Wood 1 ajoute la libération des prisonniers et passe à 15×15 / 15 élémentaux.
- Le referee prime sur l'énoncé quand ils divergent (ex. CodeBusters `STUN_DURATION = 2` alors que le texte dit 3 ; fuite des fantômes basée sur `PreviousViewers`).
- Protocole : nombre d'adversaires variable par jeu, lignes d'init parfois limitées aux premiers tours, identifiants parfois uniques **par tribu** seulement, budget temps serveur ~1,5–2× plus lent que le local.
- Toutes les sources consultées et les décisions prises sont déjà tracées dans les `research.md` / `critique.md` de chaque challenge : **les relire avant de refaire une expérience**.

## 2. État du dépôt

- Working directory : `/home/arafaram/Projects/codingGame`
- Dernier commit au moment de la rédaction : `187d3cb` — *Docs: Othello Wood 1 (depth 7 rejected) and EW v6 follow-up*
- `git status` : propre au moment de la passation (ce fichier est le seul ajout depuis).
- Aucun push confirmé ; ne pas supposer le dépôt distant à jour.
- Fichiers centraux :
  - `bot-programming/PROGRESS.md` — table des 100 jeux, ligue, rang, état (à tenir à jour).
  - `bot-programming/IMPROVEMENTS.md` — journal global.
  - `bot-programming/PLAYBOOK.md` — règles opérationnelles.
  - `bot-programming/tools/cg.py` — client API.
  - `hackathon-2026-12-03/challenges/<id>/` — un dossier par jeu (ex. `elemental-wars`, `othello-1`, `2016-codebusters`) avec `README.md`, `statement.md`, `research.md`, `plan.md`, `critique.md`, `solutions/{c,cpp,python,rust}`, `tests/`.
- ~29 lignes de `PROGRESS.md` ont un score renvoyé par l'API ; les autres restent à traiter.

## 3. État des ligues au 23/11/2026

Jeux **en cours de classement** (`in_progress=True`) :

- **Elemental Wars** — Wood 1, soumission `41788154`, score ~24,5, rang **11/76**, `percentage≈45`. Pas de boss. À surveiller : une promotion peut survenir à la prochaine évaluation (`next_promotion_in_seconds`).

Jeux au **classement terminé** (à surveiller / améliorer si bloqués) :

- **Othello** — Wood 1, `41787653`, score 20.03, rang 116/684, **pas de boss**, `percentage=100`, non promu.
- **Connect 4** — Wood 1, `41786612`, score 14.58, rang 521/1002, pas de boss.
- **CodeBusters** — Wood 2, score 18.53, boss `630129` à 19.59 (gap **1,06**). Le plus proche d'une promotion avec infrastructure complète.
- **Shobu** — Wood 2, score 12.94, boss `6906553` à 14.81 (gap 1,87), rang 12/20.
- Autres gaps faibles : Olymbits (Silver, 1,60), Troll Farm (Bronze, 1,65), Green Circle (Wood 1, 1,80), Xmas Rush (2,30), Code Royale (2,69).

Plateaux déjà documentés, **ne pas y revenir sans nouvelle idée** : Chain Reaction (`jacek` `6785873`, toutes versions 0/5), Ataxx (`6649970`), Brandubh, Shobu Wood 2.

## 4. Derniers travaux réalisés

### Elemental Wars — Wood 1 (terminé, en attente de classement)

- Promotion Wood 1 avec v3 `41787544`. Nouvelle règle : un allié libre sur le camp du prédateur **libère nos prisonniers** ; fin immédiate si une tribu est entièrement prisonnière ; cartes 11/13/15, 6/9/12/15 élémentaux, `NMAX=16`.
- 8 prototypes locaux `v4`…`v11` testés contre `6710922,6788011` (les deux meilleurs) : `v6` est le meilleur (**2/5**, meilleur rang moyen) ; `v11` (probabilités + garde de camp) marque plus mais **0/5**.
- **v6 retenu** : prédiction déterministe du coup adverse (un pas vers la proie libre la plus proche, BFS depuis la cible, priorité N,E,S,W), interception (+12000), évitement (-18000), anti-regroupement (-5000, évite la défaite instantanée), libérateur.
- v6 validé : fuzz **200/200** en 4 langages (générateur étendu à 15×15 / 15 élémentaux), **5/5** contre l'ancien bot `6918338` et le rang 40 `6758748`. Soumission C++ `41788154`.
- Attention : il faut **exactement 2 adversaires** pour les test games (1 seul → HTTP 400 `INVALID_PLAYER_COUNT`).

### Othello — Wood 1 (terminé)

- Moteur bitboard déterministe profondeur 6, 4 ports, fuzz 200/200, 5/5 contre l'ancien boss Wood 2 `6797560` (joueur aléatoire `new Random(13)` ; notre bot n'active pas EXPERT donc pas de ligne d'entrée supplémentaire). Soumission `41787653`, promotion Wood 1.
- Protocole : `id`/`boardSize` seulement aux tours ≤2 ; détecter les 8 lignes de plateau et ignorer les autres lignes ; bit = `y*8+x` ; sortir une coordonnée avec flush.
- **Profondeur 7 rejetée** : ~85–108 ms en local mais **timeout sur CG** (1 défaite/5 par timeout) et 0/5 contre le rank 1 `6886490`. Garder ≥2× de marge sous 150 ms.
- Attention : il faut **exactement 1 adversaire** pour les test games (2 → HTTP 400 `INVALID_PLAYER_COUNT`).

### CodeBusters — travail **interrompu** par l'utilisateur

- Aucune modification du dépôt, aucune soumission. Prototypes uniquement dans `/tmp/opencode/` (éphémères).
- Bot soumis inchangé : un fantôme par buster, réservation **uniquement au moment du BUST**, `RELEASE` à ≤800 de la base, `BUST` à 900–1760, pas de recul spécial, MOVE par pas de 800.
- Énoncé officiel relu (identique au `statement.md` local).
- Deep search fait : referee `eulerscheZahl/RefereeCollection/CodeBusters/CodeBustersReferee.cs` (raw GitHub). Faits importants :
  - les fantômes fuient en se basant sur les **viewers du tour précédent** (`PreviousViewers`) et **uniquement s'ils n'ont pas été attaqués ce tour** ; vitesse 400, direction opposée au viewer le plus proche (ou moyenne en cas d'égalité) ;
  - un `BUST` réussi empêche le fantôme de bouger ce tour ;
  - le score final = fantômes relâchés + fantômes encore transportés ; la partie s'arrête dès qu'une équipe a > la moitié des fantômes ;
  - `RELEASE` est valable à ≤1600 de la base (le bot utilise ≤800, plus sûr mais plus lent) ;
  - bots publics étudiés (`Covfefe3`, `GuyBol`, `Sabotaz`, `blueCicada`) : tous pour la version complète STUN/RADAR/endurance, non transposables.
- Variantes déjà rejetées (avant cette session, logs dans `critique.md`) : `cb_bustrange`, `cb_v2` (affectation globale + dépôt 1550 + recul 1330), `cb_rel` (dépôt 1550 seul), `cb_pos` (recul 1330 seul), `cb_dual` (double BUST) — toutes ≤1/4.
- Nouveau test de cette session : `cb_v7.cpp` = réservation dès la **sélection** de la cible (au lieu de seulement au BUST) → **0/5** (scores 2–6, 4–7, 7–11, 5–11, 3–9). Rejeté.
- **A/B à seeds fixes** commencé : baseline (bot soumis), `--extra '{"game_options":"seed=..."}'`, contre `630129` :
  - seed `111111111` → 3–8
  - seed `222222222` → 5–11
  - seed `333333333` → 6–11
  - seed `444444444` → 4–9
  - seed `555555555` → 8–12
  Ces 5 seeds sont la référence pour comparer toute nouvelle variante. Le boss gagne largement ; il faut une vraie amélioration stratégique (gestion des contestations, ciblage de fantômes non disputés, exploitation du `PreviousViewers` pour prédire la fuite), pas une micro-optimisation de déplacement.

### Connect 4

- Alpha-bêta profondeur 6, 4 fenêtres, centre ±2, toujours `STEAL`. Soumission `41786612`, 5/5 contre le boss `6797564`, promu Wood 1, pas de boss. API/progress réconciliés.

## 5. Outils et commandes

```bash
cd /home/arafaram/Projects/codingGame
python3 bot-programming/tools/cg.py list
python3 bot-programming/tools/cg.py show <game>            # énoncé Markdown
python3 bot-programming/tools/cg.py stub <game>            # format E/S
python3 bot-programming/tools/cg.py get <game> progress    # JSON brut
python3 bot-programming/tools/cg.py opponents <game>
python3 bot-programming/tools/cg.py test <game> <src> --language C++ --opponents <id>[,<id2>] \
    --extra '{"game_options":"seed=123456789"}'
python3 bot-programming/tools/cg.py submit <game> <src> --language C++
bash hackathon-2026-12-03/challenges/<game>/tests/run.sh   # fuzz 4 langages
```

Python utile (inline) pour lire l'état :

```python
import sys; sys.path.insert(0,'bot-programming/tools'); import cg
d = cg.request('GET','/puzzles/<game>/progress')
print(d['league'], d['score'], d['percentage'], d['rank'], d.get('league_boss'), d['in_progress'])
print(d['limits']['test_games'])   # quota du jeu
```

## 6. Quotas, pièges, style

- Limites **par jeu**, pas globales. Observé : Elemental Wars `test_games_15m` = 30/900 s ; CodeBusters `test_games_1m` = 10/60 s. **Toujours lire `d['limits']` avant une session de tests.**
- Sur HTTP 429 : respecter `retry_after_seconds` / `Retry-After`, ne pas réessayer avant.
- Requêtes API : 120/60 s.
- Nombre d'adversaires : **variable selon le jeu** — Elemental Wars = 2, Othello = 1, CodeBusters = 1 (boss `630129`).
- Ne jamais lire de fichier binaire comme source Python : `cg.py test` fait `open(src).read()` en UTF-8 → un exécutable compilé donné en source lève `UnicodeDecodeError`. Toujours passer le `.cpp`.
- `cmp` des sorties 4 langages : la référence est Python ; C/C++/Rust doivent être **byte-identiques**.
- Après une soumission, interroger `progress` et mettre à jour `PROGRESS.md` + `IMPROVEMENTS.md` + `critique.md`, puis committer.

## 7. Prochaines actions recommandées (dans cet ordre)

1. Vérifier `progress` d'**Elemental Wars** et **Othello** au réveil : si une promotion a eu lieu, relancer la boucle complète sur ce jeu (énoncé, recherche, plan, critique, 4 langages, fuzz, tests, soumission).
2. Reprendre **CodeBusters** avec les 5 seeds fixes ci-dessus comme référence. Pistes non encore testées :
   - prédire la fuite avec `PreviousViewers` (positions des busters au tour précédent) et viser la position future ;
   - éviter les fantômes déjà visibles par un buster adverse proche, ou envoyer 2 busters pour garantir la contestation ;
   - dispatcher les busters sur des fantômes distincts **sans** le bug du `v7` (qui a échoué) ;
   - ne réserver qu'au BUST mais départager par « distance à notre base / distance à l'adversaire ».
   Une seule modification à la fois, 5 games à seeds fixes, puis fuzz 200 et ports 4 langages si ≥3/5.
3. Ensuite, traiter les petits gaps : Olymbits, Troll Farm, Green Circle, Shobu.
4. Ne pas oublier de committer chaque étape et de tenir `PROGRESS.md` à jour.

## 8. À ne pas refaire

- Elemental Wars `v4`…`v11` (déjà comparés ; `v6` est le meilleur).
- Othello profondeur 7 (timeout CG).
- CodeBusters `cb_bustrange`, `cb_v2`, `cb_rel`, `cb_pos`, `cb_dual`, `cb_v7`.
- Chain Reaction / Ataxx / Brandubh / Shobu Wood 2 sans nouvelle hypothèse issue d'une recherche.
- Divulguer le token ; committer des artifacts de build.
