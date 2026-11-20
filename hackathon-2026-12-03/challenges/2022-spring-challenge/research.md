# Recherche — Spring Challenge 2022 (2022-04-21)

## Sources
- Énoncé officiel : `statement.md`.
- Stub officiel : `cg.py stub spring-challenge-2022`.

## Mécanique (Wood 4 = première ligue)
- Carte `17630 × 9000`, deux bases aux coins opposés, 3 héros par joueur.
- Chaque tour, un héros peut `WAIT` ou `MOVE x y` (déplacement maximal de 800).
- Après le déplacement, tout monstre à ≤ 800 d'un héros perd 2 PV.
- Les monstres avancent de 400 par tour ; à ≤ 5000 d'une base ils la ciblent
  puis foncent vers elle ; à ≤ 300 ils infligent 1 dégât et disparaissent.
- victoire si la base adverse est détruite, ou santé supérieure après 220 tours.
- `threatFor == 1` identifie les monstres qui menacent **notre** base.

## Stratégie retenue
1. Pour chaque monstre menaçant notre base, estimer sa position au prochain tour :
   - s'il est déjà en approche (`nearBase == 1`) : avancer de 400 vers notre base ;
   - sinon : ajouter le vecteur `(vx, vy)` fourni.
2. Trier les menaces par `(distance de la position estimée à notre base, pv, x, y)`.
3. Assigner chaque héros à la menace non encore prise la plus proche de lui.
4. Si le héros est déjà à ≤ 800 de la cible, rester sur place (`MOVE` sur sa
   propre position) ; sinon avancer vers la position estimée.

## Risques
| Risque | Mitigation |
|---|---|
| Plusieurs héros sur la même cible | Table `used` partagée, une cible par héros. |
| Monstres ciblant l'adversaire | Filtre strict sur `threatFor == 1`. |
| Divergence 4 langages | Même prédiction et mêmes départages + fuzz différentiel. |
| Timeout 50 ms | Tri et affectation en `O(n log n)`, sans recherche d'arbre. |
