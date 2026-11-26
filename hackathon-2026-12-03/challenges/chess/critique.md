# chess — critique et journal des tentatives

Format : constat / hypothèse / sources / action / résultat / leçon.

### 2026-11-26 — Chess — v1 1 pli sur coups légaux + pièces en prise
- Constat : jeu non soumis ; coups légaux fournis (input moves)
- Hypothèse : éviter les pièces en prise et prendre le matériel suffit contre le boss Wood 2
- Sources : énoncé (protocole fen/moves, roque Chess960)
- Action : simulation du coup sur la FEN, cartes d'attaque, perte maximale adverse
- Résultat : fuzz 200/200 ; 1 victoire 2 nuls contre boss ; soumis
- Leçon : les nuls viennent du manque de mat : v2 alpha-bêta Chess960
