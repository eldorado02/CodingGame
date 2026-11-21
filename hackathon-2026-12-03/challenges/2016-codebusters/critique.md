# Critique — CodeBusters

## Avant implémentation

| Risque | Évaluation | Mitigation |
|---|---|---|
| `MOVE` dépasse la cible | Élevé | On vise un point à exactement 800 ; si d < 900 on recule. |
| Fantôme en fuite | Moyen | Bande 900–1760 Large ; anticipation 400 si buster à ≤ 2200. |
| Deux busters sur le même fantôme | Moyen | Réservation globale par tour. |
| Contestations adverses | Faible en Wood | Pas de counter fiable sans historique ; on privilégie la proximité. |
| Timeout 100 ms | Faible | 5 × 28 comparaisons → trivial. |
| Divergence entre langages | Moyen | Même plan, cas déterministes + fuzz différentiel. |

## Après implémentation

- `tests/run.sh` : 8 cas déterministes × 4 langages → OK.
- Fuzz différentiel : 200 grilles aléatoires (`random.seed(7)`) → **0 divergence**.
- Test game réel : le bot tourne en boucle sans timeout (score 3–5 contre
  Bosster en Wood). Les 4 fantômes manquants sont dus à une stratégie encore
  simple (poursuite du plus proche), pas à un bug de protocole.
- Améliorations possibles (Wood+) : départage par distance à notre base,
  anticipation de fuite à 400, gestion des contestations 2v1.

## Itérations 2026-11-21 (Wood 2, toutes rejetées)

Bot validé : 1 fantôme par buster, `RELEASE` à ≤800 du coin, `BUST` à
900–1760, déplacement par pas de 800. Score ligue 18,53 < boss 19,59.

| Prototype | Modification | Résultat vs boss `630129` | Décision |
|---|---|---|---|
| `cb_bustrange` | recul jusqu'à 1330 au lieu de 800 quand le fantôme est trop proche | 1/4 (8–10, 8–9, 3–8, 13–7) | rejeté |
| `cb_v2` | affectation globale buster↔fantôme + dépôt à 1550 + recul 1330 | 1/4 (5–9, 5–9, 6–8, 4–10) | rejeté |
| `cb_rel` | dépôt à 1550 seulement | 0/4 (2–8, 6–9, 5–8, 6–11) | rejeté |
| `cb_pos` | recul 1330 seulement | 1/4 (2–5, 4–5, 4–8, 4–7) | rejeté |
| `cb_dual` | deux busters peuvent `BUST` le même fantôme pour gagner un duel | 1/5 (6–10, 4–10, 6–6, 6–11, 2–5) | rejeté |

Lecture : le boss gagne la plupart des duels (scores ~9 contre ~5). Les
micro-optimisations de déplacement ne compensent pas le déficit de débit ; il
faut une vraie amélioration de la stratégie (choix des cibles, défense des
fantômes contestés). Le bot validé est conservé.

### Sources
- Énoncé officiel Wood 2 (`statement.md`) et stub `cg.py stub codebusters`.
- Referee `eulerscheZahl/RefereeCollection/CodeBustersReferee.cs` : fantômes
  avec endurances 3/15/40 en version complète ; **dans la ligue Wood 2 les
  fantômes ont `state=0`** (vérifié dans les logs de test game), donc pas
  d'endurance à gérer.
- Bots publics étudiés : `Covfefe3/codebusters-codingame`, `GuyBol/codebusters`,
  `Sabotaz/codebusters`, `blueCicada/CodeBusters-AI` — tous écrits pour la
  version complète (STUN/RADAR/endurance), non transposables directement.
