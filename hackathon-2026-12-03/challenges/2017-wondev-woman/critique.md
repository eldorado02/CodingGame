# Critique — Wondev Woman

## Avant implémentation

| Risque | Évaluation | Mitigation |
|---|---|---|
| Choisir une action illégale | Élevé | On ne parcourt que `legalActions`. |
| Ne pas voir la victoire imminente | Élevé | Priorité absolue à `h_dest == 3`. |
| L'adversaire gagne en premier | Moyen | Déni : élever sa case 3 à 4. |
| Hauteur mal lue (trou, hors grille) | Moyen | `-1` pour `.`/hors limites. |
| Timeout 50 ms | Faible | ≤ 64 actions. |
| Divergence entre langages | Moyen | Score entier identique, fuzz. |

## Après implémentation

- `tests/run.sh` : 6 cas déterministes × 4 langages → OK (victoire, montée,
  déni, construction, adversaire invisible, trous).
- Fuzz différentiel : 200 grilles/actions aléatoires (`random.seed(23)`) →
  **0 divergence** entre C, C++, Rust et Python.
- Test game réel : **victoire 1–0** ; niveau 3 atteint au tour 11 sans
  commande invalide ni timeout. La priorité « victoire immédiate » a joué :
  dès qu'une montée sur une case 3 est dans `legalActions`, elle est sortie.
- Conclusion : l'évaluation des actions fournies suffit en Wood 3. Les ligues
  supérieures (2 unités, PUSH, visibilité partielle) demanderont une vraie
  recherche de coups et la gestion de l'adversaire.

## Après correction du protocole (2026-11-21)

- **Constat** : scoreCodinGame −1,55, rang 193 en Wood 1, bot disqualifié au
  tour 1 (`int('4 2')`) ; l'énoncé archivé annonçait 1 unité, le stub en annonce 2.
- **Hypothèse** : le plant venait du parsing, pas de la stratégie. Le referee
  lit une seule ligne de sortie → il faut la meilleure action globale.
- **Sources** : `cg.py stub wondev-woman`, log de test game
  `bot-programming/games/wondev-woman/logs/test-20261121-171943.log`,
  `CodinGame/WondevWoman/Referee.java`.
- **Action** : parser réécrit dans les 4 langages, grille de score v2, sortie
  unique, `ACCEPT-DEFEAT` en repli, harnais 210 cas (10 déterministes + 200 fuzz).
- **Résultat** :
  - fuzz différentiel **210/210** identiques dans 4 langages ;
  - test games C++ vs boss `1393115` : **4 victoires + 1 nulle sur 5**
    (3–2, 8–7, 5–4, 16–16, 3–2), aucune erreur referee ;
  - soumission **C++** `41778172`.
- **Leçons** :
  - un énoncé archivé peut être faux : le **stub** et le **referee** font foi ;
  - `unitsPerPlayer > 1` ne signifie pas plusieurs lignes de sortie ;
  - `WAIT` est spécifique à certains jeux ; ici il faut `ACCEPT-DEFEAT`.
