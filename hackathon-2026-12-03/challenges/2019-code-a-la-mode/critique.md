# Critique — Code a la Mode

## Avant implémentation

| Risque | Évaluation | Mitigation |
|---|---|---|
| Décalage d'une ligne à la lecture | Élevé | Lire explicitement `turnsRemaining`, nous, partenaire. |
| `USE` hors de portée | Moyen | Autorisé : le referee rapproche automatiquement. |
| Ordre des ingrédients dans `DISH-...` | Moyen | Comparaison par `strstr` / `contains`. |
| Vaisselle épuisée | Faible | `USE D` invalide = `WAIT`, on réessaie. |
| Divergence 4 langages | Moyen | Même ordre de priorité, fuzz. |

## Après implémentation

- `tests/run.sh` : 10 cas × 6 tours × 4 langages → OK.
- Fuzz différentiel : 200 parties aléatoires de 5 tours → **0 divergence**.
- Un bug de décalage de ligne a été détecté par les tests dans les trois
  ports compilés (ligne `turnsRemaining` non consommée) et corrigé.
- Validation officielle : **4 test games gagnés** sur 4 (agent 0) :
  - vs Bossyardee (boss) + FabDev06 (rang 1) : `15889 / 14587 / 13940` ;
  - vs hez99 + Elektordi : `13293 / 10678 / 8741` ;
  - vs janfyl + DoudouTouDou : `9372 / 5014 / 4358` ;
  - vs Uhut + am0ka : `10696 / 8089 / 6145`.
- Le port C++ a aussi gagné : `13275 / 12620 / 11973`.
- Conclusion : la stratégie « plat BLUEBERRIES + ICE_CREAM, livraison » suffit
  largement en Wood 4. Les nombreux `WAIT` correspondent à la vaisselle
  épuisée ; un dépôt d'ingrédients sur table quand il n'y a plus de plat
  pourrait encore améliorer le score.
