# ALGO.md — reconnaître le type de problème, choisir l'approche

Guide de décision pour les jeux `bot-programming` (et pour le hackathon du
03/12/2026). Lire **avant** le plan de chaque challenge. Chaque nouvelle
leçon récurrente (vue sur ≥2 jeux) est ajoutée ici avec le jeu qui l'a prouvée.

Templates : `hackathon-2026-12-03/templates/` (scaffold :
`bash hackathon-2026-12-03/templates/new_challenge.sh <pretty_id> [template]`).

---

## 0. Règles universelles (tous les jeux)

1. **Lire le protocole exactement** : lignes d'init (parfois seulement aux
   tours ≤2 — Othello), champs entrelacés vs groupés (Back Track King), nombre
   de lignes de sortie par tour (Olymbits/Winter 2024 : `required` lignes).
2. **Flush chaque ligne** (`endl` / `fflush` / `flush=True` / `out.flush()`).
3. **Déterminisme** pour le fuzz 4 langages : budget en **nœuds**
   (`CG_NODES`), jamais en temps seul ; garde-temps d'urgence qui écrit
   `TIMEGUARD` sur stderr ; tri **stable** sur clé entière ; départage
   « premier trouvé » (`>` strict). Pas de `HashMap` itérée, pas de float
   comparé à égalité, pas de `random` sans seed fixe.
4. **Budget temps** : le serveur CG est ~1,5–2× plus lent que le local. Viser
   ≤50 % de la limite (Othello prof. 7 : 85–108 ms local → timeout sur CG).
   Premier tour souvent 1000 ms : y faire les précalculs.
   **CodinGame compile le C++ sans `-O2`** : toujours mettre
   `#pragma GCC optimize("O3,unroll-loops")` avant les `#include` (pas de
   `#pragma GCC target(...)` : casse g++ 13 en local). Même avec le pragma,
   la STL reste lente (stable_sort + lambda) : tri par insertion maison dans
   le chemin chaud. Mesure Breakthrough : local -O2 4,6 M nœuds/s, local
   pragma 2,9 M/s, **serveur CG ≈ 1,2 M/s** → calibrer `NODE_LIMIT` sur
   ~1 M nœuds/s × (40 % de la limite). Afficher `d= n= ms=` sur stderr.
5. **Le referee prime sur l'énoncé** (CodeBusters `STUN_DURATION=2`, fuite des
   fantômes via `PreviousViewers`). Les métadonnées des test games priment sur tout.
6. **Nombre d'adversaires fixe par jeu** (EW = 2, Othello/CodeBusters = 1) :
   sinon HTTP 400 `INVALID_PLAYER_COUNT`.
7. **Valider chaque valeur lue** (index négatifs : archerie Olymbits).
8. **Une seule modification à la fois**, testée sur seeds fixes
   (`--extra '{"game_options":"seed=N"}'`) contre le même adversaire.
9. **Ligue cible** = la plus haute ligue **existante** (rang 1 du leaderboard,
   cf. `bot-programming/tools/status.py`). Jeux communautaires : souvent Wood 1
   ou une ligue unique `Wood` ; concours officiels : Legend.

---

## 1. Table de décision

| Signal dans l'énoncé | Classe | Approche de départ | Upgrade quand ça plafonne | Template | Vu sur |
|---|---|---|---|---|---|
| 2 joueurs, tour par tour, info parfaite, branching ≤ ~40 | Plateau abstrait | Negamax αβ + ID, éval matériel + mobilité + positionnel | TT Zobrist, killer/history, PVS, quiescence (captures), livre d'ouverture | `board-2p` | Connect 4, Othello, Shobu, Brandubh |
| Idem, branching ≥ 100 (Amazons, Hex, Go 9×9, Twixt) | Plateau large | MCTS-UCT (C≈1,0–1,4) + playouts légers, coups gagnants immédiats forcés | RAVE/AMAF, playouts biaisés, éval Voronoi/2-distance en fin | `board-2p` (moteur à remplacer) | Ataxx |
| Connexion (Hex, Twixt) | Connexion | MCTS + détection de pont/virtual connection | Éval « two-distance » / résistance électrique en αβ | `board-2p` | — |
| Territoire / blocage (Tron, Isola, Blocking, Amazons fin) | Partition | Voronoi BFS (cases atteintes avant l'adversaire) en éval, αβ 2–6 plis | Séparation détectée → remplissage max (flood fill / DFS chemin long) | `board-2p` + BFS | Tron |
| Semis/comptage (Oware, Mancala, Dots&Boxes) | Petit branching, longues lignes | αβ profond (6 coups) + TT | Fin de partie exacte, règles de chaîne (D&B : contrôle de parité) | `board-2p` | — |
| Hasard ou info cachée (Poker, Dice Duel, 6 nimmt, LoCM, Dice Shogi) | Stochastique | Expectimax 1–2 plis ou Monte-Carlo par échantillonnage de l'inconnu | ISMCTS, modèle adverse fréquentiste | `board-2p` modifié | — |
| Unités multiples, coups simultanés, carte continue (CodeBusters, Mean Max, CotC, Fantastic Bits, Mad Pod, Poker Chip Race) | Arène simultanée | Affectation gloutonne cible↔unité (coût = distance/temps), simulation 1 tour, règles prioritaires | Moteur exact + GA/RHEA (rolling horizon), Smitsimax | `agent-sim` (à créer) | CodeBusters, Mean Max |
| Grille, unités multiples, mouvements discrets (Pacman 2020, Code a la Mode, Kutulu, Hypersonic, Spider Attack) | Grille multi-agents | BFS multi-sources (cartes de distance) + scoring de cibles + réservation | Beam search sur séquences, simulation des bombes/collisions | `agent-sim` | Hypersonic, Spring 2020, EW |
| Croissance / expansion sur grille (Cellularena, Troll Farm, Keep off the Grass, Domain Expansion, Ice & Fire) | Expansion | BFS multi-sources depuis notre front, cibles prioritaires, blocage des cases sensibles | Simulation 1-2 tours de l'adversaire, coupe d'articulations | `agent-sim` | Winter 2024, Troll Farm |
| Économie / graphes de production (Ghost in the Cell, Platinum Rift, Code Royale, Code4Life, Fall 2020, Spring 2021) | Gestion | Règles prioritaires + graphe de distances, bilan production/coût | Recherche sur séquences d'actions (beam / BFS d'inventaire, Fall 2020), simulation du score | `agent-sim` | GitC, Code Royale |
| Puzzle personnel + interaction faible (Smash the Code, A-Star-Craft, Bit Runner) | Optimisation | Beam search / recherche exhaustive peu profonde sur nos coups | Monte-Carlo / GA, éval combos, anticipation adverse 1 tour | — | Smash the Code |
| Info cachée à reconstruire (Ocean of Code, Seabed Security) | Pistage | Ensemble de candidats filtré à chaque observation | Pondération bayésienne, actions d'info (SONAR/radar) | `agent-sim` | Ocean of Code |
| Jeu à prédiction de règle adverse déterministe (EW Wood 1 : l'ennemi va vers la proie la plus proche) | Exploitation | Simuler la règle adverse exacte (BFS + priorités) et intercepter | — | — | Elemental Wars |
| Mouvements légaux fournis en entrée | Raccourci | Toujours se rabattre sur la liste fournie si le moteur local diverge | Vérifier `move ∈ legal` avant d'écrire | `board-2p` | Breakthrough, Tic-tac-toe |

---

## 2. Techniques — recettes prêtes

### 2.1 Alpha-bêta (template `board-2p`)
- Iterative deepening, garder le résultat de la **dernière itération complète**.
- Ordre des coups : PV d'abord (rotation à la racine), puis captures/coups
  gagnants, puis clé heuristique ; tri stable.
- Score de mat : `WIN - ply` (préférer les gains rapides, les pertes lentes).
- Améliorations par ordre de rendement : TT Zobrist (hash 64 bits, entrée
  `{key, depth, flag, score, move}`) → killer + history → PVS/aspiration →
  LMR → quiescence (jeux à captures).
- Éval : matériel, mobilité (nb coups légaux), tables positionnelles,
  stabilité/coin (Othello), avance/protection (Breakthrough), parité.

### 2.2 MCTS
- UCT : `w/n + C*sqrt(ln N / n)`, C≈1,4 (jeux 0/1), ≈0,7 si playouts biaisés.
- Playout : coup gagnant immédiat si existe, sinon blocage d'un gain adverse,
  sinon aléatoire (seedé, xorshift identique dans les 4 langages).
- Déterminisme : RNG xorshift64 maison, même seed ; budget en itérations.
- Réutiliser l'arbre entre tours (descendre dans le fils joué).

### 2.3 Voronoi / partition
- BFS simultané depuis les deux têtes ; case à égalité = neutre.
- Éval = `mes_cases - ses_cases` (+ bonus articulation). Séparés → maximiser
  un chemin long (DFS borné, heuristique « wall hugging »).

### 2.4 Arènes simultanées (agent-sim)
- Étape 1 : reproduire le moteur du referee (positions, vitesses, arrondis
  **exacts** : `round`/`trunc` comme le Java/C# du referee).
- Étape 2 : affectation unité↔cible par glouton trié `(coût, id_unité, id_cible)`
  ou Hongrois si ≤ 10×10.
- Étape 3 : prédire l'adversaire par sa règle la plus probable (même glouton).
- Étape 4 (plafond) : RHEA/GA sur séquences de N tours, mutation sur 1 gène,
  éval = simulation complète, budget en évaluations.

### 2.5 Beam search (actions séquentielles)
- État compact + hash pour dédoublonner ; largeur W, profondeur D ; garder le
  premier coup de la meilleure feuille.

---

## 3. Budgets de référence

| Limite CG | Viser en local | Python ref (fuzz) |
|---|---|---|
| 50 ms | ≤ 20 ms | budget réduit via `CG_NODES` |
| 100 ms | ≤ 40 ms | idem |
| 150 ms | ≤ 60 ms (Othello : prof. 6 OK, 7 non) | idem |
| 1000 ms (1er tour) | ≤ 500 ms | précalculs |

`run.sh` exporte `CG_NODES=20000` par défaut pour que Python reste rapide ;
le bot soumis utilise sa constante `NODE_LIMIT` (calibrée avec le garde-temps).

---

## 4. Pièges de protocole déjà payés

| Jeu | Piège | Parade |
|---|---|---|
| Back Track King | champs entrelacés lus groupés | lire case par case |
| Olymbits | registre d'archerie signé | borner à 0 avant d'indexer |
| Winter 2024 | BFS traversant la source récoltée | bloquer la case dans le BFS |
| Winter 2024 Wood | énoncé « BASIC only » faux | croire les métadonnées |
| Othello | `id`/`boardSize` seulement aux 2 premiers tours | détecter les 8 lignes plateau |
| Othello | prof. 7 timeout sur CG | marge ×2 |
| Elemental Wars | 2 adversaires obligatoires en test | `--opponents a,b` |
| cg.py | binaire passé comme source → `UnicodeDecodeError` | toujours le `.cpp` |
| 6 nimmt | énoncé : phase en 1re ligne du tour ; referee : en dernière | sonde `select` + `RANDOM` qui recopie stdin brut sur stderr avant tout code |
| cg.py | corps vide en 200 sous charge → `JSONDecodeError` | `request()` relance comme une coupure réseau |

---

## 5. Classement des 100 jeux

Classe → template de départ. `B` = plateau abstrait αβ, `BL` = plateau large
MCTS, `C` = connexion, `P` = partition, `S` = stochastique/caché,
`A` = arène simultanée, `G` = grille multi-agents, `E` = expansion,
`M` = économie, `O` = optimisation, `T` = pistage.

- **B** : shobu, brandubh, ataxx, chain-reaction-1, connect-4, othello-1,
  breakthrough, clobber, abalone, onitama, lines-of-action, checkers, chess,
  minishogi, nine-mens-morris, counting-tictactoe, tic-tac-toe (Ultimate),
  yavalath, yinsh, oware-abapa, dots-and-boxes, isola (P), penguins (P),
  tulips-and-daisies, atari-go, atari-go-9x9, impasse, mad-knights, bandas,
  volcanoes, paper-soccer, tryangle-catch, night-of-war, clash-of-bots.
- **BL / C** : amazons (P), hex, twixt-pp, game-of-life-or-death.
- **P** : tron-battle, blocking, isola, amazons, back-to-the-code.
- **S** : poker, dice-duel, dice-shogi, 6-nimmt-6-qui-prend-take-5,
  legends-of-code-magic, legends-of-code-magic-constructed, fireworks,
  start-up, git-patchwork.
- **A** : codebusters, mean-max, coders-of-the-caribbean, fantastic-bits,
  mad-pod-racing, poker-chip-race, game-of-drones, space-shooter, beeminegame.
- **G** : hypersonic, spring-challenge-2020, code-a-la-mode, code-of-kutulu,
  spring-challenge-2022, xmas-rush, wondev-woman, botters-of-the-galaxy,
  vindinium, code-keeper---the-hero, elemental-wars, langton-s-ant,
  winter-challenge-2026-snakebyte, soak-overflow, great-escape (P),
  gargoyles-versus-santas, tower-dereference.
- **E** : winter-challenge-2024, spring-challenge-2026-troll-farm,
  keep-off-the-grass-fall-challenge-2022, domain-expansion,
  a-code-of-ice-and-fire, cultist-wars, spring-challenge-2023-ants,
  coders-of-the-realm, coders-of-the-realm---1v1, green-circle.
- **M** : ghost-in-the-cell, platinum-rift-episode-1, platinum-rift-episode-2,
  code-royale, code4life, fall-challenge-2020, spring-challenge-2021,
  summer-challenge-2026-back-track-king, summer-challenge-2024-olymbits.
- **O** : smash-the-code, bit-runner-2048.
- **T** : ocean-of-code, seabed-security, crystal-rush.

(Classement initial a priori ; corriger après lecture de l'énoncé.)

---

## 6. Journal des patterns récurrents (à compléter)

| Pattern | Preuve | Règle |
|---|---|---|
| Boss Wood = joueur aléatoire / glouton 1 pli | Othello Wood 2 (`new Random(13)`), Connect 4 | un αβ prof. 4–6 suffit pour sortir de Wood |
| Plateau atteint = changement de stratégie, pas micro-réglage | CodeBusters 6 variantes ≤1/4, Chain Reaction 0/5 | relire le referee, chercher le levier mécanique |
| C++ sans pragma O3 = bot 4× plus lent sur CG | Breakthrough TIMEGUARD chaque tour ; 33/34 bots du dépôt sans pragma | ajouter le pragma partout, re-soumettre les bots existants |
| Tafl : garder un coin depuis une case adjacente | Brandubh v9 (b7/f7 capturés par le roi contre le coin) | garder depuis la diagonale (b2/b6/f2/f6) |
| Jeu à 3 joueurs | Mad Knights v1 (3/3) | recherche paranoïde : nous max, tous les autres min, même moteur αβ |
| Placement à fort branchement (≥100 cases vides) | Counting TicTacToe (prof. 2 → 4, 0/3 → 3/4) | éval incrémentale (seules les fenêtres de la case jouée) + tri par gain immédiat + faisceau K=10 aux nœuds internes, racine complète |
| Budget mal calibré = TIMEGUARD = coups au hasard | Isola, Chain Reaction, Counting TTT | lire `d= n= ms=` des logs serveur après le 1er bench et fixer `TURN_NODES` à ~40 % de la limite |
| Boss qui dépasse le temps | Clobber Wood 2 | compter à part les victoires « timeout adverse » ; elles ne prouvent pas la force du bot |
| Test games = toujours siège 0 | Oware v1 (coups illégaux en joueur 1, score 2,63) | lire le parsing de sortie du referee pour chaque siège ; vérifier le score dans l'heure qui suit la soumission |
| Prédire la règle adverse déterministe | Elemental Wars v6 (+2 rangs) | simuler le bot boss si son code/règle est connu |
| Ordre d'entrée de l'énoncé faux | Othello (id 2 tours), 6 nimmt (phase en dernier) | 1er test = sonde stdin brut ; ne coder le parsing qu'après |
| Brouillard + monstres qui ne bougent qu'en nous voyant | Code Keeper v2 (oscillation entre 2 cases) | mémoriser les entités hors vue : leur dernière position reste vraie |


## Patterns du sous-agent (fusionnés le 2026-11-23)
# ALGO — patterns relevés par l'agent 2 (à fusionner dans ALGO.md §6)

| Pattern | Preuve | Règle |
|---|---|---|
| Géométrie non précisée par l'énoncé (diagonale hex, orientation lettre/chiffre) | Hex (sonde anti-diagonale), TwixT-PP (bot public) | vérifier sur une vraie partie (chemin gagnant final) avant d'écrire le moteur |
| Éval « distance de connexion » plate | TwixT-PP v1 : égalités → coups au bord de départ, 0/5 vs top 25 | toujours un départage secondaire (centralité, nombre de chemins courts) |
| Milieu de tableau des jeux communautaires = bots stub invalides | TwixT-PP rangs 49 et 65 | bencher aussi contre le top 25 pour mesurer la force réelle |
| MCTS sur CG : expansion à chaque visite = création de nœuds dominante | Hex v1 (14 µs → 6 µs/itération avec seuil 3 + tableaux statiques) | seuil d'expansion ≥ 3, pas de `std::vector` dans l'arbre |
| Coups composés (rebonds, rafles) : coût d'un nœud très variable | Paper Soccer v1 : 3000 nœuds = 4 ms au début, > 150 ms en fin de partie | garde-temps vérifié à chaque nœud, pas tous les 64 |
