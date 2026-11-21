# Recherche — Tron Battle (CodinGame bot-programming)

Deep search réalisé dans la session principale le 2026-11-21 (sous-agent
`opencode/hy4-flash-free`, 3ᵉ de la chaîne après épuisement des quotas
`deepseek-v4.1-flash-free` et `longcat-2.5-free`).

## Sources

- Énoncé officiel : `statement.md` (`GET /api/v1/puzzles/tron-battle`).
- Stub : `GET /api/v1/puzzles/tron-battle/stub-generator`.
- Statistiques : `GET /api/v1/puzzles/tron-battle/stats` — 8 281 joueurs,
  Wood 1/2, Bronze, Silver, Gold, Legend.
- Boss de la ligue courante : `GET /puzzles/tron-battle/progress` →
  `.league_boss` ; Wood 2 = `JarvBoss`, `agent_id 630143`. Les boss des ligues
  supérieures ne sont pas exposés tant que le compte n'y est pas (ils peuvent
  être des copies de soumissions de joueurs ; `agent_id` non stable).

### Implémentations publiques
- `FabianPetersen/TRON_MCST` — MCTS pur.
- `xavierfeltin/tron_battle` — MCTS local, coefficients réglés, **Legend**.
- `nathanWolo/tron-codingame` — alpha-beta itératif + Voronoi par bitboards,
  recherche « paranoid mixe » 3–4 joueurs, harnais SPRT. Voir
  `writeup/tronzero-notes.html` et `legacy/alpha-beta/README.md`.
- `nicolaspi/codingame-tron` — alpha-beta, score 48 (top 10).
- `vafin-mk/tron_battle` — top 57 Gold, notes survie / 3–4 joueurs.
- `kvas-it/tron_battle`, `Oreshnik/TronTester`, `jasonrute/CG-Tron-Simulator`,
  `Agade09/CG-Tron-Arena` — simulateurs locaux.
- Pascal van Kooten, « AI Challenge in 78 lines » — https://vks.ai/2016-09-07-ai-challenge-in-78-lines
  (Voronoi « qui atteint le plus de cases en premier », top 5 %).
- a1k0n, Google AI Challenge post-mortem — https://www.a1k0n.net/2010/03/04/google-ai-postmortem.html
  (Voronoi, wall-hugging, tree of chambers, greedy primé).
- `yduf.github.io/CG-tron/` — compendium de ressources.
- Thread officiel : https://forum.codingame.com/t/tron-battle-aka-line-racing-multiplayer-challenge-discussion/49

### Littérature (arXiv)
- `arXiv:1110.3211` — Tron est PSPACE-complet : justifie l'heuristique.
- `arXiv:1412.4100` — Tron pondéré sur arbres.
- `arXiv:1201.4995` — méta-théorèmes de complexité.
- MCTS multi-joueurs : `arXiv:2303.09449`, `arXiv:2506.13249`,
  `arXiv:2112.07761`, `arXiv:1805.09218`, `arXiv:2212.05208`,
  `arXiv:2104.04278`, `arXiv:1901.04274`.
- P. Nijssen, *Monte-Carlo Tree Search for Multi-Player Games* (2013),
  DOI 10.26481/dis.20131202jn — max^n, pooled values.

## Mécanique exacte

- Grille 30×20, **tours séquentiels** (P0 puis P1…), pas de simultanéité.
- Entrée : `N P` puis `N` lignes `X0 Y0 X1 Y1` = (queue, tête). Joueur mort :
  les 4 valeurs valent `-1` **et toute sa bande disparaît**.
- Sortie : une seule ligne `UP`/`DOWN`/`LEFT`/`RIGHT`.
- 100 ms/tour (le 1ᵉʳ tour ≈ 1000 ms en pratique).
- La bande d'un joueur mort doit être effacée du tracker, sinon divergence
  avec l'arbitre (piège n°1 documenté par `tronzero-notes`).
- Ne jamais appliquer sa propre sortie au tracker : le tour suivant contient
  déjà la nouvelle tête.
- Élimination immédiate si collision, sortie de grille, timeout ou sortie
  invalide.

## Décision stratégique

Fait observé (`tronzero-notes`) : celui qui a le plus de territoire au premier
« cut » gagne ~96–99 % des duels ; « a better evaluation heuristic will always
beat deeper minimax » (a1k0n). Pour commencer en Wood/Bronze (2 joueurs, 100 ms
en Python) on privilégie donc :

1. **Tracker d'état** : reconstruire les bandes à partir des (queue, tête),
   mémoriser les têtes, effacer la bande du mort.
2. **Évaluation Voronoi multi-source** : BFS par vagues depuis chaque tête
   vivante dans l'ordre des tours ; case au joueur strictement le plus proche,
   égalité = contestée. Score `10^7·mes_cases − 10^5·cases_ennemies +
   somme_distances_ennemis` (78-lignes).
3. **Fin de partie / isolement** : greedy primé (essayer chaque 1ᵉʳ coup puis
   remplir gloutonnement), wall-hugging (choisir le coup qui retire le moins
   d'arêtes), éviter les points d'articulation, borne dam checker.
4. **Recherche** : 1v1 → alpha-beta à approfondissement itératif en C++/Rust ;
   Python restera sur Voronoi 1-ply + greedy (3 playouts/100 ms seulement).
   Silver/Gold (3–4 joueurs) → paranoid mixte ou max^n/PUCT.

## Pièges classiques
- Tours séquentiels, pas collisions frontales (les bots du Google AI Challenge
  2010 sont faux ici).
- Bande du mort effacée.
- `list.pop(0)` est O(n) en Python → `deque`.
- Negamax mal signé ≈ −35 Elo (SPRT du README nathanWolo).
- Avantage du siège : 70–83 % des mêmes ouvertures gagnées par le même siège →
  tester les deux sièges et plusieurs seeds.
- Battre le boss en test game ≠ promotion (mode `rank_above_boss`).
