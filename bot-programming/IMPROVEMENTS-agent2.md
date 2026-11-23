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
