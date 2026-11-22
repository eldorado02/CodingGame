# Elemental Wars — critique

## Avant implémentation

- Le challenge est à 3 joueurs : un seul appel `test` avec les deux adversaires
  (`--opponents 6910638,6697588`). Un appel avec un seul ID répond HTTP 400
  (`INVALID_PLAYER_COUNT`).
- Wood 3 ne compte qu'un élémental par tribu : une stratégie de poursuite/fuite
  suffit, sans gestion de groupe.
- Les captures étant simultanées, une case occupée par le prédateur est fatale
  même si elle contient aussi la proie.

## Après implémentation

### Tentatives

- `v1` (lecture globale `stdin.read()`) : timeout au frame 1, 0/5. Cause : le
  processus attend la fin du flux au lieu de répondre tour par tour.
- `v2` (lecture ligne à ligne, fuite modérée `-500`) : **3/5**.
- `v3` (fuite renforcée `-2000`) : **5/5** en Python puis **5/5** en C++.
- Ports C/C++/Rust : deux bugs de traduction détectés par le fuzz :
  `pred_of('P')` renvoyait `P` au lieu de `F`, et le prédateur prisonnier n'était
  pas ignoré. Après correction : fuzz **200/200** identique.

## Leçons

- Le fuzz différentiel est indispensable dès que la logique dépend d'une table
  de correspondance : une erreur de port passe inaperçue en test game si la
  tribu concernée n'apparaît pas dans les 5 parties.
- La lecture bloquante est le défaut récurrent de cette campagne (Brandubh,
  Elemental Wars) : toujours vérifier le premier log de test game avant de
  conclure sur la stratégie.
