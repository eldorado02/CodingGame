# Recherche — CodeBusters (2016-06-25)

## Sources

- Statement officiel : `statement.md` (API CodinGame `/api/v1/puzzles/codebusters`).
- Socle DSA local : `../../dsa/README.md` (graphes, file, géométrie entière).
- Aucun code tiers copié. Les règles ci-dessous viennent de l’énoncé.

## Mécanique du jeu

Carte 16001 × 9001, origine en haut à gauche. Deux équipes, bases aux
corners (0,0) pour l’équipe 0 et (16000,9000) pour l’équipe 1. Brouillard :
un fantôme ou un buster adverse n’est visible que s’il est à ≤ 2200 d’un de
nos busters.

Actions par buster et par tour (100 ms) :

- `MOVE x y` : avance de **exactement 800** vers la cible (arrondi à l’entier
  le plus proche). Ne pas viser un point à moins de 800, sinon on dépasserait
  et on perdrait un tour.
- `BUST id` : capture si 900 ≤ distance ≤ 1760. En dessous de 900, le fantôme
  s’échappe (il faut d’abord reculer). Un seul fantôme transporté par buster.
- `RELEASE` : rapporte le fantôme si distance ≤ 1600 à la base. Sinon on perd
  un point — ne jamais relâcher ailleurs.

Fantômes : immobiles puis, dès qu’un buster est à ≤ 2200, ils fuient de 400
**par tour** du buster le plus proche (ou de la position moyenne en cas
d’égalité). Ils ne bougent pas pendant qu’ils sont en train d’être capturés.

Contestation : si plusieurs busters visent un fantôme, l’équipe ayant le plus
de busters gagne ; à égalité entre équipes, personne ne le capture. Au sein
de l’équipe gagnante, le buster le plus proche captura. Un buster qui tente de
capturer un autre fantôme laisse échapper celui qu’il porte.

Fin : tous les fantômes capturés ou 250 tours. Le score est le nombre de
fantômes dans chaque base.

## Décisions de conception

- **Équipage par buster** : chaque buster poursuit son fantôme le plus proche,
  mais on garde une **réservation globale** des fantômes ciblés pour éviter que
  deux busters gaspillent leur tour sur le même.
- **Bande de capture** : si distance < 900, `MOVE` vers un point à ~1300 du
  fantôme (recul) ; si 900 ≤ d ≤ 1760, `BUST` ; sinon `MOVE` vers le fantôme
  mais **en s’arrêtant à 1400** (pas 800 de plus pour ne pas le dépasser).
- **Retour base** : dès qu’un buster porte un fantôme, `MOVE` vers la base
  jusqu’à être à ≤ 800, puis `RELEASE`.
- **Anticipation de fuite** : le fantôme bouge de 400 par tour quand un buster
  est à ≤ 2200. On vise donc sa position **actuelle** uniquement si elle est
  dans la bande ; sinon on estime sa position future en le prolongeant de 400
  sur l’axe opposé au buster le plus proche.
- **Fog** : on ne poursuit que les fantômes visibles ; si aucun n’est visible,
  on patrouille vers le centre ou vers la dernière position connue (pas de
  supposition hors brouillard).
- **Idiomes** : C tableaux fixes + boucles, C++ `vector`/`struct`, Rust
  `Vec`/`Option`, Python listes de dicts. Même logique, pas de traduction
  mot à mot.

## Risques

| Risque | Mitigation |
|---|---|
| Dépasser la cible en MOVE | Viser un point à exactement 800 si d > 800, sinon ne pas bouger (ou reculer si d < 900). |
| Capturer un fantôme déjà visé par l’adversaire | On ne sait pas ce que l’adversaire vise ; privilégier les fantômes les plus proches de notre base et les plus proches de nos busters. |
| Fantôme qui fuit | Anticipation 400, bande 900–1760 large. |
| Timeout 100 ms | Au plus 5 busters × 28 fantômes → trivial. |
