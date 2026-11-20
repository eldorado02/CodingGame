# Critique — A*Craft

## Avant implémentation

| Risque | Évaluation | Mitigation |
|---|---|---|
| Sortie bloquée (stdin non fermé) | Élevé | Lire exactement 10 + 1 + `robotCount` lignes. |
| Flèche sur case pourvue / trou | Élevé | Jamais émise ; cases fixes jamais écrasées. |
| Direction initiale non changée | Moyen | Flèche sous robot si départ dans le vide. |
| État `(case, direction)` revisité | Élevé | `visits[190][4]`, arrêt immédiat. |
| Cycle infini | Moyen | Plafond 760 pas par robot. |
| Divergence 4 langages | Moyen | Entiers, départage `U,R,D,L`, fuzz. |

## Après implémentation

- `tests/run.sh` : 6 cas déterministes × 4 langages → OK.
- Fuzz différentiel : 200 cartes aléatoires (murs + flèches fixes +
  robots, `random.seed(53)`) → **0 divergence**.
- Deux bugs corrigés en chemin :
  - Python lisait tout stdin d'un coup → timeout sur CodinGame
    (stdin reste ouvert entre les tours) ;
  - indexage des robots en pas de 3 au lieu de 1 après passage aux lignes.
- Découverte importante : une flèche placée sous un robot au tour 0 change
  sa direction initiale. Le cas `Buggy Robot` passe de **1 à 22 points**.
- Validation officielle : **30/30 testcases joués sans erreur**, total
  **2487 points**. Exemples : Simple 23, Dual 46, Buggy 22, Roundabout 78,
  Portal 24, Multiple 3x3 62, One Long Road 104, Hard Choice 165,
  To the Right 231, StarCraft 144, Xel 178.
- Le port C++ a été rejoué sur le testcase 13 : **165 points**, identique
  au Python.
- Conclusion : la stratégie gloutonne « flèche seulement si le droit
  boucle » est robuste et très supérieure au score de base (19 pour un
  robot sur grille pleine sans flèche). Une recherche locale type
  recuit simulé pourrait encore améliorer les plateaux 3×3, mais le
  couple temps de calcul / complexité de portage est défavorable ici.
