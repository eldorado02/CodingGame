# Recherche — Fantastic Bits (2016-11-26)

## Sources

- Statement officiel : `statement.md` (API CodinGame `/api/v1/puzzles/fantastic-bits`).
- Socle DSA : `../../dsa/README.md` (géométrie entière).
- Aucun code tiers copié.

## Mécanique (Wood 2)

Terrain 16001 × 7501. Buts centres (0,3750) pour team 0 (notre gauche) et
(16000,3750) pour team 1. Pôles distants de 4000 (donc y=1750 et y=5750),
rayon 300. 2 sorciers par équipe, rayon 400. Snaffles rayon 150, 5 ou 7,
placés symétriquement.

- `MOVE x y thrust` (0–150) : force vers (x,y), vitesse cumulée, inertie.
- `THROW x y power` (0–500) : le tour **suivant** un grab, lance le Snaffle.
  Sinon le Snaffle est lâché.
- Un sorcier attrape un Snaffle si son **centre** entre dans son rayon 400.
- Rebonds sur les bords ; collision entre toutes entités sauf Snaffle↔sorcier.
- Fin : 200 tours ou majorité des points.

## Décisions de conception

- **Rôles** : un sorcier « attaquant » cherche le Snaffle le plus proche,
  l’autre « défenseur » reste entre notre but et le Snaffle le plus proche
  de notre camp (interception).
- **Approche** : viser la position **anticipée** du Snaffle :
  `pos + v * k` avec k ≈ 3 (le Snaffle dérive tout seul). Thrust 150 loin,
  réduit à 60 à moins de 1200 pour ne pas dépasser.
- **Grab puis THROW** : dès que `state==1`, lancer vers le **centre du but
  adverse** avec power 500. Le Snaffle file droit au but.
- **Défense** : si un Snaffle est dans notre moitié et se dirige vers notre
  but, le défenseur vise sa position anticipée pour l’intercepter.
- **Idiomes** : C tableaux + structs, C++ `vector<Entity>`, Rust `Vec<Entity>`,
  Python listes de dicts. Parsing des entités avec le type en **string**.

## Risques

| Risque | Mitigation |
|---|---|
| Dépasser le Snaffle (inertie) | Thrust réduit à proximité, visée anticipée. |
| Lancer dans le mauvais but | But = (16000,3750) si team 0, (0,3750) si team 1. |
| Oublier le THROW après grab | Dès `state==1`, priorité au THROW. |
| Timeout | 13 entités × 2 sorciers → trivial. |
| Divergence entre langages | Même logique entière, fuzz différentiel. |
