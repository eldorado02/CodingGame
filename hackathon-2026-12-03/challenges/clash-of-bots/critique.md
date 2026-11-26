# clash-of-bots — critique et journal des tentatives

Format : constat / hypothèse / sources / action / résultat / leçon.

### 2026-11-26 — Clash of Bots — v1 règles locales par robot
- Constat : jeu non soumis ; vision 5x5 sans position absolue
- Hypothèse : les mouvements précèdent les attaques : l'embuscade sur la case d'approche punit l'adversaire
- Sources : referee stivens/Clash-of-Bots (Interpreter.kt, MoveGraph.kt)
- Action : règles : explosion >=2 tués, attaque adjacente, embuscade distance 2, approche, dérive
- Résultat : fuzz 200/200 ; 4/4 contre rang 276/550 ; soumis
- Leçon : lire MoveGraph avant toute règle de déplacement groupé
