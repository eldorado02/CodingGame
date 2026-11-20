# Recherche — Coders of the Caribbean (2017-04-14)

## Sources

- Statement officiel : `statement.md` (API `/api/v1/puzzles/coders-of-the-caribbean`).
- Socle DSA : `../../dsa/README.md` (géométrie).
- Aucun code tiers copié.

## Mécanique (Wood 3)

Grille hexadécimale 23 × 21, un seul navire de 3 cases. Chaque tour :
rhum −1, commande appliquée, navire avance de `speed` cases (0–2), puis
tourne. Ramasser un baril sur le chemin ajoute 10–20 rhum (plafond 100).
`MOVE x y` calcule automatiquement le chemin hex ; `SLOWER` réduit la
vitesse, `WAIT` conserve cap et vitesse. Pas de collisions en Wood 3.

Victoire : adversaire à court de rhum, ou plus de rhum après 200 tours.

## Décisions de conception

- Le moteur gérant le déplacement hex, on ne choisit que la **cible**.
- Cible = baril le plus proche (distance euclidienne sur les coordonnées
  fournies, suffisante pour départager en Wood).
- Si aucun baril, `WAIT`.
- On n’utilise pas `SLOWER` en Wood 3 : plus vite = moins de tours donc
  moins de rhum consommé.
- **Idiomes** : C tableaux, C++ `vector`, Rust `Vec`, Python listes.

## Risques

| Risque | Mitigation |
|---|---|
| Rhum insuffisant pour atteindre le baril | Le baril le plus proche minimise le temps de trajet. |
| Dépé passer le baril | `MOVE` recalcule chaque tour vers la cible. |
| Timeout 50 ms | 26 barils → trivial. |
| Divergence entre langages | Même distance entière, fuzz différentiel. |
