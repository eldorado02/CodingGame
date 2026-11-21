# Critique — Cultist Wars

## Avant implémentation
| Risque | Mitigation |
|---|---|
| Tir bloqué par une unité intermédiaire | Reproduire le Bresenham du moteur. |
| Leader trop lent à convertir | Cible la plus proche dans notre moitié, recalculée chaque tour. |
| Passage en infériorité numérique | Neutraliser les cultistes ennemis qui menacent nos unités. |

## Après implémentation
- `tests/run.sh` : 10 scénarios × 4 langages → sorties identiques.
- Fuzz différentiel : 200 scénarios aléatoires → **0 divergence**.
- Une divergence initiale venait du départage des tirs : Python utilisait la
  distance à la cible visée, les ports la distance à l'unité réellement touchée.
  Python a été aligné sur les ports.
- Test games CodinGame (adversaires Bronze `2411205`, `4779806`, `4524271`,
  `4459794`) : la configuration finale a gagné 2 parties sur 4 dans le dernier
  lot ; les itérations précédentes ont produit 23 victoires sur 80 parties.
- Limite de l'approche : la stratégie est réactive et ne coordonne pas les
  cultistes pour encercler le leader adverse. Amélioration possible : réserver
  deux culistes à la chasse du leader ennemi pendant que le leader convertit.

## 2026-11-21 — correctif sortie C++ (soumission 41778956)
Le bot C++ ne vidait pas `cout` (`sync_with_stdio(false)` + `'\n'`) et était
tué avant son premier tour (`scores: 0=-1`). Ajout de `<< flush` sur toutes
les sorties, fuzz 210/210 identique dans 4 langages, resoumission C++
`41778956`. Test games : 2 victoires / 5 contre le boss (3–7 pts).
