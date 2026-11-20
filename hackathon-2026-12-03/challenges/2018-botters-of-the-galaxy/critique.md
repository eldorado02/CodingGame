# Critique — Botters of the Galaxy

## Avant implémentation

| Risque | Évaluation | Mitigation |
|---|---|---|
| Mauvais choix de héros | Moyen | HULK, le plus résistant. |
| Attaque hors portée | Faible | Le moteur rapproche automatiquement (`ATTACK`). |
| Héros adverse invisible | Moyen | On attaque tout héros adverse connu. |
| Cible morte | Faible | On ne parcourt que les entités du tour. |
| Timeout 50 ms | Faible | Parcours linéaire. |
| Divergence entre langages | Moyen | Priorité + distance + id, fuzz. |

## Après implémentation

- `tests/run.sh` : 8 cas déterministes × 4 langages → OK, dont :
  - `01_pick` : phase de pick **avec entités** ;
  - `07_multiturn` : pick puis partie (détecte toute ligne non consommée) ;
  - `08_realformat` : 22 champs sur `TOWER` (format réel du referee).
- Fuzz différentiel : 200 configurations aléatoires (`random.seed(31)`), dont
  30 % de phases de pick avec entités et des lignes à 13 ou 22 champs →
  **0 divergence** entre C, C++, Rust et Python.
- Bugs trouvés et corrigés :
  - **Phase de pick** : le bot sortait `HULK` mais ne consommait pas les
    `entityCount` lignes d'entités → désalignement au tour suivant. Corrigé
    dans les 4 langages (lecture des entités avant le test `roundType < 0`).
  - C : oubli de la ligne `oppGold` ; Rust : `gold`/`oppGold` lus sur une seule
    ligne ; C++ : `continue` sur notre propre HERO sautait la consommation de
    ses 9 champs supplémentaires. Le C++ lit désormais chaque ligne entière
    (`getline` + `istringstream`) et ignore les champs excédentaires.
- Test game réel (adversaire `DoubleHulk`, Wood 6) : **victoire en 48 tours**,
  scores `0=0`, `1=-100000000`, par élimination du héros ennemi. Aucune commande
  invalide ni timeout.
- Conclusion : `ATTACK <id>` suffit en Wood 6 (le moteur rapproche
  automatiquement). Les ligues supérieures (2 héros, sorts, objets, or) exigeront
  une gestion des sorts et des cibles multiples."})
