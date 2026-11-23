# ataxx — guide de reprise

Challenge CodinGame bot-programming, dossier `hackathon-2026-12-03/challenges/ataxx`
(port CodinGame du jeu d'arcade Ataxx). Objectif final : **Ligue Légende**.

## État courant

- **Ligue actuelle : Wood 2.** Boss de ligue : **agent_id `6649970`** (« BOSS 1 »,
  flag `boss: true` dans `cg.py opponents ataxx`).
- Stratégie implémentée : **v1 — greedy déterministe 1-ply** sur la liste
  d'actions légales fournie par le referee (matériel + contrôle central +
  pression locale).
- **Aucun test game ni soumission n'a été lancé dans cette session** (consigne
  explicite du sous-agent : ne pas lancer `cg.py test` / `submit`). La stratégie
  reste donc une hypothèse non encore validée contre le boss.
- Dernier `submission_id` : aucun.
- Journal global : [`../../bot-programming/IMPROVEMENTS.md`](../../bot-programming/IMPROVEMENTS.md)
  (fichier partagé, non modifié par ce sous-agent — la session parente doit
  ajouter l'entrée). Règles du projet : [`../../bot-programming/PLAYBOOK.md`](../../bot-programming/PLAYBOOK.md).

## Ordre de lecture

1. `statement.md` — énoncé officiel (relu à chaque promotion de ligue via
   `python3 ../../bot-programming/tools/cg.py show ataxx`).
2. `research.md` — sources libres (URL), mécanique exacte, idées retenues/rejetées.
3. `plan.md` — contrat E/S, invariant, algorithme, complexité, départage, matrice de tests.
4. `critique.md` — checklist avant implémentation + risques ouverts.
5. `solutions/` — 4 implémentations.
6. `tests/` — 10 cas figés + fuzz différentiel 200 cas.

## Contrat d'entrée / sortie exact

```
Initialisation (1 ligne) :  id (0 ou 1)

Par tour :
  7 lignes   : grille 7x7, de BAS en HAUT (ligne 0 = rangée '1' = y=0)
               '.' vide | '0' pièce joueur 0 | '1' pièce joueur 1 | '#' mur
  1 ligne    : lastAction  (null | pass | actions séparées par ';')
  1 ligne    : actionsCount (entier >= 0)
  N lignes   : action légale, 2 chars (spawn "b7") ou 4 chars (jump "a7c7")
    -> coordonnées : colonne 'a'..'g' (x=0..6), rangée '1'..'7' (y=0..6)

Sortie (1 ligne par tour) : l'action choisie, ou "random" si aucune action.
Contraintes de temps : 1000 ms au 1er tour, 100 ms par tour suivant.
```

Point clé : **le referee fournit la liste exacte des actions légales**. Le bot
n'a donc pas à générer les coups, seulement à en choisir un. Aucun message
supplémentaire n'est émis (sortie byte-exacte pour le fuzz).

## Architecture décisionnelle (extrait de `plan.md`)

- **État conservé** : aucun. Tout est recalculé chaque tour (plateau 49 cases,
  liste d'actions, choix). Pas de RNG, pas de table de transposition, pas
  d'historique → chaque tour est indépendant et reproductible.
- **Invariant de coordonnées** : index = `y*7 + x`, y depuis le bas. La matrice
  de poids centraux est symétrique (invariante par retournement), donc cette
  convention ne change pas l'évaluation.
- **Décision** : pour chaque action proposée, simuler sur une copie (Jump vide
  la source ; conversion des pièces adverses voisines de la destination), puis
  évaluer en entier :
  - terminal : adversaire à 0 pièce → `+1 000 000`, soi à 0 → `-1 000 000` ;
  - `+20 * (mesPièces - piècesAdverses)` (le matériel domine) ;
  - poids de contrôle central `W[i]` (1 au bord, 6 au centre) ;
  - pression locale : `+1` par ennemi adjacent à une de mes pièces (symétrique).
- **Départage déterministe** : score maximal ; à égalité, **chaîne d'action
  ASCII la plus petite** (`strcmp` order). Identique dans les 4 langages
  (alphabet `a`-`g`, `1`-`7`), aucun flottant dans le chemin de décision.
- **Complexité réelle** : `O(N * 49 * 8)` ≈ `N*392` ops entières ; pire cas
  réaliste `N≈200` → ~78k ops, très sous les 100 ms (marge Python /3 ~26k).

## Cartographie des 4 langages

| Fichier | Rôle | Compilation | Conventions / pièges |
|---|---|---|---|
| `solutions/python/bot.py` (169 l.) | référence de fuzz | `python3 solutions/python/bot.py` | `typing`, `main()`, `readline()`, `print(..., flush=True)`. |
| `solutions/c/bot.c` (177 l.) | portabilité C | `gcc -O2 -std=c11 -Wall -Wextra ... -o bot_c` | tableaux statiques `[49]`, `memset`/copie par tour, `scanf` vérifiés, `fflush(stdout)`. |
| `solutions/cpp/bot.cpp` (164 l.) | **version soumise** | `g++ -O2 -std=c++20 -Wall -Wextra ... -o bot_cpp` | STL, `std::clamp`, `sync_with_stdio(false)`, sortie **flushée** via `std::endl`. |
| `solutions/rust/bot.rs` (216 l.) | portabilité Rust | `cargo build --release --manifest-path solutions/rust/Cargo.toml` | `BufReader::read_line`, `unwrap_or` uniquement sur registre invalide, `BufWriter` + `flush()`. |
| `solutions/rust/Cargo.toml`, `Cargo.lock` | manifeste Rust | — | binaire `ataxx_bot`. |

**Soumettre le C++** : `python3 ../../bot-programming/tools/cg.py submit ataxx solutions/cpp/bot.cpp --language C++`
(pas fait dans cette session).

## Tests

- Harnais : `bash tests/run.sh` (génère les 10 cas figés, compile les 3 langages
  natifs, compare leur sortie à la référence Python sur 10 + 200 cas).
- **Résultat : 210/210 cas identiques dans les 4 langages** (10/10 figés +
  200/200 fuzz).
- Générateur : `tests/gen_fuzz.py`, seeds **200..399** (200 cas reproductibles).
  Chaque action générée est **légale par construction** (dérivée du plateau).
  Couverture mesurée : **5713 actions** (2371 Spawn, 3342 Jump), **9 cas sans
  aucune action légale** (plateau plein → réponse `random`), **25 cas** avec
  `lastAction` multi-actions (`;`), joueur id 0 et 1, densités 0.15→1.0, murs.
- Ce que le fuzz **ne couvre pas** : draw 300 coups, triple répétition, parties
  multi-tours complètes, vraies positions de départ (596 configs), boss réel.

## Pièges déjà rencontrés

- Aucun au niveau du challenge (première implémentation).
- Pièges généraux du playbook évités ici : champs entrelacés (BTK), registres
  signés (Olymbits Bronze), horizon trop long (Olymbits). Voir
  [`../../bot-programming/PLAYBOOK.md`](../../bot-programming/PLAYBOOK.md) §6.
- Détail technique : la première version du builder de voisins en Rust indexait
  la case cible au lieu de la source ; corrigé avant validation, le fuzz vert
  confirme l'identité des 4 langages.

## Points d'amélioration à tester (un seul changement à la fois)

1. **Minimax 2-ply** sur les actions fournies, même évaluation, avec modèle
   d'adversaire « il maximise son propre eval ». Signal de gain : victoires
   contre le boss `6649970` alors que le greedy 1-ply perd, sans dépasser 100 ms.
2. **Clustering / phase de jeu** (heuristique v4 de GeckoX94) : bonus pour les
   pièces alliées groupées, poids du matériel croissant en fin de partie.
   Signal : moins de pièces isolées capturées en log de test game.
3. **Normalisation de la casse** des coordonnées à la lecture. Signal :
   `scores: 0=-1` / « unrecognized command » dans un log.
4. **Détection de répétition** (historique de hash de position) pour éviter les
   nulles par triple répétition. Signal : parties nulles alors qu'on est en
   avance sur le matériel.

Pour chaque tentative : 1 seule modification, fuzz 210/210, puis test games
contre le boss, puis écriture du résultat dans `critique.md` et du journal
global par la session parente.


## Mise à jour 2026-11-23 (session Claude Code) — v2 soumise
- **v2** `41796475` : réécriture bitboards (49 bits + murs), negamax αβ ID, éval = différence de pièces, tri (captures ×2 + bonus spawn), passe gérée, racine = liste légale du referee. Budget 120k nœuds/tour (CG ≈ 30 ms, prof. 6), 600k au tour 1.
- Bench : 2/4 à 15k nœuds, **3/4 à 120k nœuds** contre le boss Wood 2 `6649970` (v1 gloutonne : 0/5).
- Fuzz : `bash tests/run.sh` (positions de parties aléatoires avec murs symétriques, CG_NODES=2000, 20 s). Les anciens cas figés 01–10 ont été retirés (format du nouveau harnais).
- Referee : github.com/jdermont/AtaxxCodingame (à lire pour la fin de partie exacte).
- Prochaine étape : éval de stabilité (pièces non retournables), TT, suivi de la promotion vers Wood 1.
