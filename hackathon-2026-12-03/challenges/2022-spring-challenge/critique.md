# Critique — Spring Challenge 2022

## Avant implémentation

| Risque | Évaluation | Mitigation |
|---|---|---|
| Dégâts infligés par surprise | Élevé | Prédiction de la position suivante et priorité aux menaces les plus proches de la base. |
| Duplication des cibles | Moyen | Une seule cible par héros via `used`. |
| Infractions de timeout | Faible | Algorithme linéaire/logarithmique, aucune simulation. |
| Divergence 4 langages | Moyen | Même prédiction flottante et mêmes clés de tri + fuzz. |

## Après implémentation

- `tests/run.sh` : 10 cas × 8 tours × 4 langages → OK.
- Fuzz différentiel : 200 parties aléatoires de 6 tours → **0 divergence**.
- Deux erreurs corrigées pendant la mise au point :
  1. `px, py = bx, by if threat == 1 else ...` en Python affectait `py` à un
     tuple au lieu de la paire de coordonnées ;
  2. `std::tie` en C++ ne peut pas lier des temporaires, remplacé par
     `std::make_tuple`.
- Validation officielle (agent 0) : **8 victoires / 8**, toujours `0=1 1=0`
  (notre base garde 1 PV, celle de l'adversaire est détruite) :
  - Python, C, C++, Rust : 2 victoires chacun contre BOSS 1 Wood 2 (4258166)
    et absoluce (4260028).
- La défense est volontairement pure : les sorts et la brume ne font pas partie
  de la première ligue et ne sont pas implémentés.
- Amélioration possible : regrouper les héros pour infliger 4 ou 6 dégâts à un
  monstre résistant, et intercepter les monstres non-ciblants avant qu'ils
  n'atteignent le rayon de 5000.

## 2026-11-21 — test de prédiction vélocité (rejeté)

- Prototype `sc22_v2` : extrapolation 3 tours des monstres via `vx,vy`, tri par
  ETA vers la base, affectation des héros au point d'interception.
- Résultat : **0/5 victoires** contre le boss `4258167`, contre **5/5** pour le
  bot validé sur la même fenêtre. Cause probable : les monstres changent de cap
  quand ils approchent de la base (`nearBase=1`), donc l'extrapolation les fait
  viser trop loin et désorganise la défense.
- Décision : conserver le bot validé (prédiction `vx,vy` 1 tour quand
  `nearBase=0`, cap vers la base sinon).
- Harnais : ajout de 200 cas de fuzz (210/210 identiques dans 4 langages).
- Nouvelle soumission C++ `41778517` du bot validé pour rafraîchir le score de
  ligue (19,42 < boss 20,8 malgré le rang 1).
