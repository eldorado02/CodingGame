# Critique — Ataxx

## Avant implémentation (checklist obligatoire)

| Point | Verdict | Détail |
|---|---|---|
| Lecture exacte des champs dans l'ordre | **VALIDÉ** | init = 1 entier (`id`) ; par tour = 7 lignes grille (**haut→bas**), 1 ligne `lastAction`, 1 entier `actionsCount`, puis `actionsCount` lignes d'action. Aucun champ entrelacé ici (contrairement à Back Track King). |
| Registres / indices signés | **VALIDÉ (corrigé)** | coords `a-g`/`1-7` converties en `x=c-'a'`, **`y='7'-r'`** (les lignes sont reçues du haut vers le bas) ; bornage défensif à `[0,6]` dans les 4 langages. L'ancien `y=r-'1'` inversait le plateau et provoquait des coups adverses au matériel. |
| Une action peut-elle détruire notre avantage ? | **VALIDÉ AVEC RÉSERVES** | un Jump vide sa case source : la simulation le modélise explicitement (`cells[src]=0`). La conversion n'écrase que des pièces *adverses*. Risque résiduel : un Jump peut isoler une pièce — l'évaluation de clustering n'est pas incluse en v1. |
| Horizon dans le budget | **VALIDÉ** | 1-ply seulement : ~78k ops au pire, ≪ 100 ms (voir `plan.md`). Pas de recherche. |
| Règle de ligue supérieure non couverte | **VALIDÉ AVEC RÉSERVES** | draw à 300 coups et triple répétition sont côté referee ; le bot ne les modélise pas (risque documenté ci-dessous). |
| Les 4 langages produisent-ils la même sortie ? | **VALIDÉ** | uniquement des entiers + comparaison de chaînes ASCII ; aucun float, aucun ordre de hachage, aucune source d'aléa. |

## Après implémentation (à remplir après fuzz)

- Fuzz différentiel cible : **210/210** (10 cas figés + 200 cas de fuzz) dans
  Python / C / C++ / Rust. Résultat réel : voir le rapport final et la sortie
  de `bash tests/run.sh`.
- Compilation : `gcc -O2 -std=c11 -Wall -Wextra` (C), `g++ -O2 -std=c++20
  -Wall -Wextra` (C++), `cargo build --release` (Rust), Python 3.11+.
- Sortie C++ **flushée** chaque tour (`std::endl`) ; C `fflush(stdout)` ;
  Rust `BufWriter::flush()` ; Python `print(..., flush=True)`.

## Risques identifiés (non résolus en v1)

1. **Horizon 1-ply (greedy).** Maximiser le matériel immédiat peut sacrifier
   une meilleure position à 2 coups. Signal : le boss gagne alors que notre
   évaluation par coup est positive. Correctif : minimax 2-ply sur les actions
   fournies (même éval), en mesurant le pire cas avant d'augmenter.
2. **Pas de modèle de triple répétition / draw 300 coups.** Le referee gère la
   règle ; le bot peut jouer un coup qui répète une position. Signal : parties
   nulles alors qu'on est en avance sur le matériel. Correctif : mémoriser un
   hash de position par tour (état supplémentaire à porter dans les 4 langages).
3. **Parsing supposé ASCII minuscule.** Si le referee envoie des majuscules ou
   des espaces dans l'action, le parsing échouerait. Le bornage défensif évite
   le crash mais pas un mauvais choix. Signal : `scores: 0=-1` (commandes non
   reconnues). Correctif : normaliser la casse à la lecture.
4. **Boss non testé cette session.** Conformément à la consigne du sous-agent,
   `cg.py test` / `submit` n'ont **pas** été lancés. La stratégie reste une
   hypothèse jusqu'à un test game contre le boss **6649970** (Wood 2).
5. **Tie-break ASCII dépend de l'ordre des actions fournies ?** Non : on trie
   implicitement par `(eval, chaîne)`, donc l'ordre de la liste du referee
   n'influe pas sur le résultat. Vérifié par le fuzz (listes mélangées).
6. **Boss Wood 2 `6649970` : 0/5 après correction des conventions.** Rejeu seed
   `8726614551035298000` (log `test-20261122-125743.log`) : le bot joue bien
   depuis ses propres pièces, mais le boss gagne au frame 74. Le plateau envoyé
   au bot est translaté : la première ligne reçue est `y=0` (haut), donc
   `y = '7' - rang`. L'hypothèse précédente (`me = id + 1` erroné) était fausse :
   c'est l'orientation des lignes qui était inversée. La recherche 1,5 pli
   (`/tmp/opencode/ataxx_v2.py`, beam=8, pire retour adverse) perd aussi 0/5.
   Correctif appliqué dans les 4 langages : fuzz 210/210, mais 0/5 persistants
   contre le boss. Piste ouverte : évaluation de connectivité / contrôle de
   centres, ou recherche minimax plus profonde. Aucune soumission.

## Journal global

L'entrée `bot-programming/IMPROVEMENTS.md` est un fichier **partagé** : ce
sous-agent ne l'a pas modifié (consigne « do NOT edit shared files »). La
session parente doit ajouter l'entrée de journal correspondante.

### 2026-11-23 — Ataxx — v2 αβ bitboards soumise
- Constat : v1 gloutonne 1 pli : 0/5 contre le boss Wood 2, jamais soumise
- Hypothèse : αβ + différence de pièces à prof. 4–6 bat le boss
- Sources : énoncé, logs v1 (orientation : 1re ligne = rangée 7), dépôts GitHub jdermont/AtaxxCodingame (referee) et bots αβ publics (Tomatofishh/ataxx, pascalc/Ataxx-Reloaded-Agent)
- Action : réécriture 4 langages bitboards, fuzz 200/200 ; budget 15k puis 120k nœuds (CG 26–38 ms)
- Résultat : 2/4 puis 3/4 contre le boss `6649970` ; soumission `41796475`
- Leçon : la profondeur compte plus que l'éval dans Ataxx : budget ×8 = +1 victoire / 4
