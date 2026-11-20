# Recherche — Ghost in the Cell (2017-02-25)

## Sources

- Statement officiel : `statement.md` (API `/api/v1/puzzles/ghost-in-the-cell`).
- Socle DSA : `../../dsa/README.md` (Floyd-Warshall, file).
- Aucun code tiers copié.

## Mécanique (Wood 3)

7–15 usines, 21–105 liens, distances 1–20 tours. Chaque joueur possède une
usine initiale avec 15–30 cyborgs ; les autres sont neutres avec garnison.
**Une seule action par tour** en Wood 3 (`MOVE src dst n` ou `WAIT` ; les
actions multiples arrivent en ligue supérieure).

Ordre de résolution : déplacement des troupes → ordres → production (0–3 par
usine non neutre) → combats → conditions de fin. Une troupe en transit ne
combattant pas, une usine qui envoie tout reste à 0.

Victoire : plus de cyborgs après 200 tours, ou adversaire sans usine ni
troupe.

## Décisions de conception

- **Distances** : Floyd-Warshall sur le graphe des usines, une fois à l’init.
- **Forces effectives** : `cyborgs + alliés_en_approche - ennemis_en_approche`.
  On raisonne sur cette force pour ne pas envoyer une usine qui sera prise.
- **Choix de l’action** :
  1. défense prioritaire si une de nos usines a une force effective négative ;
  2. sinon capture de l’usine **neutre** la plus rentable (production × poids,
     distance faible, coût faible), en envoyant `garnison + 1` ;
  3. sinon attaque de l’usine ennemie la plus faible atteignable avec excédent ;
  4. sinon `WAIT`.
- **Idiomes** : C matrices statiques, C++ `vector`, Rust `Vec`, Python listes.

## Risques

| Risque | Mitigation |
|---|---|
| Envoyer trop (usine vidée puis prise) | On garde une réserve de 5 sur l’usine source. |
| Sous-estimer une défense | Forces effectives incluent les troupes en approche. |
| Action unique gaspillée | On choisit le meilleur couple source/cible global. |
| Timeout 50 ms | 15³ Floyd-Warshall en init, 15² par tour → trivial. |
