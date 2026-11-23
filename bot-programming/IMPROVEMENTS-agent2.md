# IMPROVEMENTS — journal de l'agent 2 (à fusionner dans IMPROVEMENTS.md)

### 2026-11-23 — Hex — v1 MCTS + RAVE soumise
- Constat : jeu jamais joué (ligue unique Wood, 125 joueurs) ; l'énoncé ne précise pas la diagonale voisine.
- Hypothèse : MCTS + RAVE à remplissage aléatoire (sans terme d'exploration) bat le milieu de tableau.
- Sources : énoncé, test games sondes (`probe3` : chemin gagnant adverse `(5,4)→(6,3)` → voisinage HexWiki), HexWiki.
- Action : 4 langages (tableaux statiques, RNG xorshift64 + multiplication-décalage), fuzz 200/200 ; budget 2600 itérations après mesure CG (3000 = 44 ms).
- Résultat : 3/3 contre le rang 61 ; soumission `41796234` (rang 57 provisoire à 14 %).
- Leçon : vérifier la géométrie par une partie réelle ; en MCTS, expansion après k visites, sinon la création de nœuds domine.

### 2026-11-23 — Yavalath — v1 αβ + coups forcés soumise
- Constat : jeu jamais joué (ligue unique Wood, 256 joueurs).
- Hypothèse : αβ + extension des blocages forcés + éval fenêtres de 4 suffit pour le milieu de tableau.
- Sources : referee MultiStruct/Yavalath (lu), sonde protocole (vol autorisé seulement au 1er tour du joueur 2).
- Action : 4 langages, fuzz 200/200, 14000 nœuds/tour.
- Résultat : 3/3 rang 133, 3/3 rang 61 ; soumission `41796345`.
- Leçon : lire le referee quand le dépôt est cité dans l'énoncé (règle « ligne la plus longue » ; vol à `turn == 1`).

### 2026-11-23 — TwixT-PP — v1 distance de cavalier + 2 plis soumise
- Constat : jeu jamais joué (ligue unique Wood, 128 joueurs), referee introuvable.
- Hypothèse : éval distance en pegs (BFS 0-1, segments coupés par les liens adverses) + 2 plis.
- Sources : énoncé, bot public TidyMaze/Twixt-PP (protocole), test games.
- Action : 4 langages, fuzz 200/200, 30 ms au pire sur CG.
- Résultat : 6/6 contre rangs 65/49 (bots stub invalides), 0/5 contre rangs 25/9 ; soumission `41796478`.
- Leçon : éval de connexion plate → égalités départagées par l'index → agglutinement au bord ; ajouter un départage central.

### 2026-11-23 — Paper Soccer — v1b αβ tours complets soumise
- Constat : jeu jamais joué (ligue unique Wood, 241 joueurs).
- Hypothèse : αβ sur tours complets (DFS des rebonds) + éval rangs/distance BFS (Lecorbio, rang 2) bat le milieu de tableau.
- Sources : referee jdermont/CodinGame-paper-soccer (lu), Lecorbio/paper-soccer-strategy-engine `docs/algorithms.md`, rapport OpenCode (37 URL).
- Action : 4 langages, fuzz 200/200 (flux multi-tours, réponses par la référence Python) ; garde-temps 150 → 80 ms vérifié à chaque nœud après 153 ms observés.
- Résultat : 9/9 contre rangs 97, 49, 33 ; soumission `41796586`.
- Leçon : coups composés = coût par nœud très variable → garde-temps à chaque nœud.

### 2026-11-23 — Domain Expansion — v1 αβ + Voronoi soumise
- Constat : jeu jamais joué (ligue unique Wood, 92 joueurs), aucun bot public.
- Hypothèse : αβ sur (déplacement, mur) + Voronoi + fin exacte bat le milieu de tableau.
- Sources : referee Mortis66666/CG-Domain-Expansion (Kotlin, lu ; chemins de 4 pas autorisés).
- Action : 4 langages, fuzz 200/200, 10000 nœuds (28 ms CG).
- Résultat : 5/6 (2/3 rang 43, 3/3 rang 25) ; soumission `41796695`.
- Leçon : lire le BFS du referee, l'énoncé simplifie la règle de déplacement.

### 2026-11-23 — Bandas — v1 αβ profondeur 7–8 soumise
- Constat : jeu jamais joué (ligue unique Wood, 649 joueurs).
- Hypothèse : branching 4 → negamax profond + éval matériel/distance de chute.
- Sources : referee Oli8/CG-bandas (port exact des poussées et du rognage).
- Action : 4 langages, fuzz 200/200, 10000 nœuds (29 ms CG).
- Résultat : 6/6 contre rangs 302 et 130, 0/2 contre rang 44 ; soumission `41796751`.
- Leçon : pour un petit branching, la vitesse du moteur (bitboards) décide du rang au-dessus du milieu de tableau.

### 2026-11-23 — Tower Dereference — v1c glouton couverture + colle soumise
- Constat : jeu jamais joué (ligue unique Wood, 425 joueurs) ; v1 canons seuls perd contre le rang 197.
- Hypothèse : la colle multiplie l'exposition ; les dégâts au-delà des PV sont perdus ; les conflits de construction se perdent sur le demi-terrain adverse.
- Sources : referee eulerscheZahl/TowerDefense (constantes, ordre du tour, règle de conflit), logs diagnostics argent/vies.
- Action : glouton gain/coût entier (canon, colle, améliorations), poids colle ×4, plafond 15 dégâts, décote 10 % côté adverse ; 4 langages, fuzz 200/200.
- Résultat : v1b 2/2 rang 197 ; v1c 2/3 rang 113 ; soumission `41796833`.
- Leçon : premier test en Python (langage CG Python3) = validation du protocole avant de porter dans 3 langages.
