# Critique — Ocean of Code

## Avant implémentation

| Risque | Évaluation | Mitigation |
|---|---|---|
| Croyance adverse trop large | Élevé | Filtrage par ordres adverses + retour de dégâts de nos torpedoes. |
| Se enfermer dans un chemin sans case libre | Élevé | Warnsdorff (max de voisins libres) + `SURFACE | MOVE` en secours. |
| Tir invalide | Moyen | Portée BFS calculée depuis la position d'entrée, comme le referee. |
| Divergence 4 langages | Élevé | Mêmes structures, mêmes départages, fuzz différentiel. |

## Après implémentation

- `tests/run.sh` : 10 cas × 10 tours × 4 langages → OK.
- Fuzz différentiel : 200 parties aléatoires de 6 tours → **0 divergence**.
- Un bug de parsing C (imbrication de `strtok` pour les ordres séparés par `|`)
  a été détecté par les tests puis corrigé.
- Validation officielle (agent 0) :
  - Python : **5 victoires / 5** (boss Cpt. Levine, XSWL, Tyir, kvvck,
    Zefir13000), scores `6/6/6/6/5` contre `0/-1/0/-1/-1`.
  - Ports C (`4–0`), C++ (`4–0`) et Rust (`5–0`) : **3 victoires / 3**.
- Le bot gagne en restant à 4–6 PV et en coulant l'adversaire avant la fin ;
  le `SURFACE` de secours n'a jamais été nécessaire lors des test games.
- Amélioration possible : conserver un historique de chemin adverse par
  position pour exploiter l'interdiction de revisite, et utiliser le sonar en
  ligue supérieure.
- Conclusion : la combinaison « croyance simple + torpedo maximal + marche
  Warnsdorff » suffit largement ici.
