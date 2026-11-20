# Recherche — Botters of the Galaxy (2018-03-02)

## Sources

- Statement officiel : `statement.md` (API `/api/v1/puzzles/botters-of-the-galaxy`).
- Socle DSA : `../../dsa/README.md` (géométrie).
- Aucun code tiers copié.

## Mécanique (Wood 6)

Carte 1920 × 750, 2 équipes, **un seul héros** par joueur, pas d'objets ni
de sorts (ajoutés en ligues supérieures). picks : `roundType < 0` → sortir le
nom d'un héros ; `roundType > 0` → une ligne d'action par héros vivant.

Point clé de l'énoncé : « If a target is out of range, your hero will first
move closer towards the target and then try to attack it. » Un simple
`ATTACK unitId` gère donc le déplacement ET l'attaque.

Choix : **HULK** (1450 PV, 80 dégâts, vitesse 200, portée 95) — le plus
résistant, idéal pour un rush.

## Décisions de conception

1. Pick : `HULK`.
2. Chaque tour :
   - cible prioritaire = héros ennemi visible (`team != mon team`, `HERO`) ;
   - sinon tour ennemie (`TOWER`) ;
   - sinon unité ennemie (`UNIT`) ;
   - sinon `WAIT`.
3. Action : `ATTACK <unitId>` (le moteur rapproche automatiquement).

## Risques

| Risque | Mitigation |
|---|---|
| Héros ennemi invisible | `isVisible` ignoré : on attaque tout héros adverse connu. |
| Cible déjà morte | On ne garde que les entités du tour courant. |
| Tour trop résistante | HULK maximise les PV ; on teste en conditions réelles. |
| Timeout 50 ms | ≤ 30 entités, parcours linéaire. |
| Divergence entre langages | Choix par priorité + id croissant, fuzz. |
