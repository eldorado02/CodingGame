# Playbook d'amélioration continue — recherche, plan, critique, code 4 langages

Ce document est la **consigne de travail** appliquée à chaque challenge de
`PROGRESS.md`. Il est lu avant chaque deep search, chaque plan, chaque critique
et chaque implémentation. Objectif : **gagner contre le boss de ligue puis
monter jusqu'en Légende**, pas seulement « faire tourner » un bot.

---

## 1. Deep search — chercher mieux, vérifier, puis décider

### 1.1 Ordre de priorité des sources
1. **Énoncé officiel** (`GET /puzzles/<pretty_id>`) — toujours le lire *à chaque
   promotion de ligue* : les règles changent (ex. Winter 2024 Wood 3 active
   `HARVESTER` alors que l'énoncé générique dit « BASIC only » ; Olymbits Bronze
   passe d'1 à 4 mini-games).
2. **Métadonnées des test games réels** (`cg.py test` + log) — la vérité terrain.
   Chercher `growthTypes_*`, `harvestedProteins_*`, `scores:`, `summary:`, les
   `MESSAGE` du boss et les `stdout:` des deux agents.
3. **Source du referee** (lien `github.com/CGjupoulton/...`, `Waffle3z/...`, etc.) :
   elle donne les règles exactes (ex. `STUN_DURATION = 2` alors que l'énoncé dit
   3 ; `risk >= 5` déclenche le stun ; les registres signés de l'archerie).
4. **Dépôts GitHub de solutions** — requêtes API :
   `https://api.github.com/search/repositories?q=<game>+codingame`,
   `.../search/code?q=<game>+language:python`, puis `git/trees/<branch>?recursive=1`
   et `raw.githubusercontent.com`. Toujours lire le code, pas seulement le README.
5. **Post-mortems/blogs** (CodingGame, Medium, DEV, Reddit) pour les astuces de
   départage et les pièges de parsing.

### 1.2 Règle de méfiance
- Une stratégie venant d'un dépôt public est une **hypothèse**, pas une preuve :
  elle doit être validée par un test game contre le boss de la ligue courante.
- Noter dans `research.md` : URL, langage, idée retenue, idée rejetée et pourquoi.
- Si une source contredit l'énoncé, **croire les métadonnées du test game** et
  documenter la contradiction.

### 1.3 Chaîne de repli des modèles (jamais de payant)
| Étape | 1er choix | Repli 1 | Repli 2 | Repli 3 |
|---|---|---|---|---|
| Deep search | `deepseek-v4.1-flash-free` | `longcat-2.5-free` | `hy4-flash-free` | `nemotron-3.5-super-vl-free` |
| Implémentation | `deepseek-v4.1-flash-free` | `flint-40b-free` | `longcat-2.5-free` | `mimo-v2.6-flash-free` |

Quand un sous-agent renvoie `Free usage limit reached` : descendre d'un rang ;
si tous les rangs échouent, continuer **manuellement** dans la session
principale, écrire les mêmes fichiers, et retenter un sous-agent à l'étape
suivante. Ne jamais basculer sur un modèle payant.

---

## 2. Plan — rendre le problème testable avant de coder

Un plan valide doit contenir, explicitement :
1. **Protocole exact** : nombre de lignes d'init, ordre des champs par ligne,
   champs entrelacés vs groupés, types (signé, non signé, chaîne), nombre de
   lignes de sortie par tour.
2. **Invariant d'état** : ce qui est recomputé chaque tour, ce qui est mémorisé
   (ex. sources déjà harvestées, score courant, historique de reset).
3. **Algorithme** avec complexité et **budget temps** : mesurer le pire cas en
   nombre d'itérations, pas seulement en O(). Diviser par 3 pour la marge Python.
4. **Règle de départage déterministe** (ex. `(distance, y, x, rang_direction)`),
   identique dans les 4 langages — sinon le fuzz différentiel échoue.
5. **Matrice de tests** : cas nominal, map dégénérée, entrée vide, reset, égalité,
   valeur négative, horizon atteint, adversaire plus rapide.
6. **Hypothèse de victoire** : « je gagne si … » et le signal qui prouvera que
   l'hypothèse est fausse (ex. `gained 3A from absorption`).

---

## 3. Critique — attaquer le plan avant de l'implémenter

Checklist obligatoire (note `VALIDÉ` / `VALIDÉ AVEC RÉSERVES` / `À REVOIR`) :
- Le plan lit-il **exactement** les champs dans le bon ordre ? (BTK : lire
  `owner instability inked active` groupés au lieu d'entrelacés donnait
  `ValueError: invalid literal for int() with base 10: 'x'`.)
- Les registres sont-ils tous des positions ? (Olymbits Bronze : l'archerie stocke
  `x`/`-y` **signés** ; indexer une liste avec un négatif fait planter Python et
  lit hors bornes en C/C++.)
- Une action peut-elle **détruire notre propre avantage** ? (Winter 2024 :
  `GROW BASIC` traversait la source harvestée et l'absorbait — il fallait
  **bloquer la case dans le BFS**, pas seulement l'exclure des objectifs.)
- L'horizon de recherche est-il dans le budget ? (Olymbits : horizon 4 = 1/5,
  horizon 5 = 10/10, horizon 6 = timeout `scores: 0=-1`.)
- Y a-t-il une règle de ligue supérieure non couverte ? Relire l'énoncé après
  chaque promotion.
- Les 4 langages produiront-ils exactement la même sortie sur les cas limites ?

Après chaque défaite en test game : écrire une **hypothèse**, un test qui la
réfute ou la confirme, puis mettre à jour `research.md` + `critique.md`. Ne pas
empiler les changements : un seul changement à la fois, re-testé.

---

## 4. Implémentation 4 langages — idiomatique, pas traduit

### Règles communes
- Copier la **logique**, pas la syntaxe. Mêmes règles de départage, mêmes
  constantes, mêmes invariants.
- Interdire les effets non déterministes (`random`, hash iteration order,
  float pour des comparaisons d'égalité) dans la décision.
- Toute valeur lue depuis l'entrée est **validée** (bornes, signe, existence).
- Un seul point de sortie : `print(...)`/`puts`/`cout`/`writeln!` — et le nombre
  exact de lignes par tour (Olymbits/Winter： `required - 1` lignes `WAIT`).
- Commiter uniquement après fuzz vert **et** test game ≥3/5.

### C (`gcc -O2 -std=c11 -Wall -Wextra`)
- `snake_case`, structs + fonctions, tableaux statiques dimensionnés aux
  constantes du jeu, `memset` par tour.
- Vérifier chaque `scanf` retour ; indexer uniquement après test de bornes ;
  **aucune lecture négative** (ramener à 0 ou ignorer).
- `fflush(stdout)` chaque tour.

### C++ (`g++ -O2 -std=c++20 -Wall -Wextra`)
- STL (`vector`, `queue`, `array`, `tuple`), RAII, `const`, types forts.
- `std::tuple` pour les clés de départage ; éviter les `std::map` itérées pour
  la décision si l'ordre importe.
- `std::ios::sync_with_stdio(false)` + `cin.tie(nullptr)`.

### Rust (`cargo build --release`)
- Ownership/borrowing, `Option`/`Result`, erreurs typées, **pas d'`unwrap` dans
  la logique métier** ; `unwrap_or` acceptable uniquement pour un registre
  invalide (comportement de repli explicite).
- `usize` pour les index après validation ; convertir depuis `i32` avec
  `try_into()`/`max(0)` ; `VecDeque` pour BFS, `BinaryHeap` + `Reverse` pour
  min-heap, `HashSet`/`HashMap` avec ordre de départage explicite.
- `BufWriter` + `flush()` chaque tour.

### Python 3.11+
- PEP 8, `typing` (`dict[tuple[int,int], ...]`), `dataclass`/`__slots__`,
  `main()` + `if __name__ == "__main__"`.
- `sys.stdin.readline()` + `strip()` ; **toujours borne inférieure** sur un
  index (`if pos < 0: pos = 0`) ; `deque` pour BFS, `heapq` pour files de priorité.
- Mesurer le pire cas : si > ~30 ms, réduire l'horizon ou mémoïser.

### Socle DSA
Réutiliser `hackathon-2026-12-03/dsa/` ; copier localement le code nécessaire
(CodinGame ne permet pas d'import multi-fichers) en gardant les mêmes noms et
la même qualité.

---

## 5. Validation — le seul critère est la victoire réelle

1. **Fuzz différentiel** : générateur déterministe, ≥200 cas, 10 cas figés
   commités, 3 langages comparés à la référence Python. Le générateur doit
   couvrir **les bugs déjà rencontrés** (positions négatives, champs entrelacés,
   resets, GAME_OVER, harvesters, stocks C/D non nuls).
2. **Test games réels** : contre le boss de la ligue courante, ≥3/5 victoires.
   Espacer les appels de 6 s ; respecter `retry_after_seconds` sur HTTP 429
   (5 soumissions/300 s, 30 test games/900 s, 120 requêtes/60 s).
3. **Analyse des pertes** : lire `scores:`, `metadata:` et `summary:`. Une
   défaite n'est pas « de la variance » tant qu'une cause mécanique n'a pas
   été cherchée dans le log.
4. **Soumission** puis `GET /puzzles/<id>/progress`. À chaque promotion :
   relire l'énoncé, chercher le nouveau boss, reprendre la boucle depuis l'étape 1.
5. **Écriture** : `statement.md`, `research.md`, `plan.md`, `critique.md`,
   `solutions/{c,cpp,rust,python}`, `tests/` ; puis commit + push.

---

## 6. Erreurs déjà payées — ne pas les re-payer

| Challenge | Erreur | Leçon |
|---|---|---|
| Back Track King | Champs du tour lus groupés au lieu d'entrelacés | Lire `owner instability inked active` case par case |
| Olymbits | Horizon 6 en Python | Timeout `scores: 0=-1` ; horizon 5 validé 10/10 |
| Olymbits Bronze | Registre d'archerie signé | Borner par défaut à 0 avant d'indexer, dans les 4 langages |
| Winter 2024 | Source harvestée traversée par le BFS | Bloquer la case, pas seulement la cible |
| Winter 2024 | Énoncé « BASIC only » en Wood 04 | Croire la metadata réelle, pas l'énoncé générique |
| Général | Sous-agents à court de quota | Repli dans le ranking, puis session principale, reprise auto |
