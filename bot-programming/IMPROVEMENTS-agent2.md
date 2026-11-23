# IMPROVEMENTS — journal de l'agent 2 (à fusionner dans IMPROVEMENTS.md)

### 2026-11-23 — Hex — v1 MCTS + RAVE soumise
- Constat : jeu jamais joué (ligue unique Wood, 125 joueurs) ; l'énoncé ne précise pas la diagonale voisine.
- Hypothèse : MCTS + RAVE à remplissage aléatoire (sans terme d'exploration) bat le milieu de tableau.
- Sources : énoncé, test games sondes (`probe3` : chemin gagnant adverse `(5,4)→(6,3)` → voisinage HexWiki), HexWiki.
- Action : 4 langages (tableaux statiques, RNG xorshift64 + multiplication-décalage), fuzz 200/200 ; budget 2600 itérations après mesure CG (3000 = 44 ms).
- Résultat : 3/3 contre le rang 61 ; soumission `41796234` (rang 57 provisoire à 14 %).
- Leçon : vérifier la géométrie par une partie réelle ; en MCTS, expansion après k visites, sinon la création de nœuds domine.
