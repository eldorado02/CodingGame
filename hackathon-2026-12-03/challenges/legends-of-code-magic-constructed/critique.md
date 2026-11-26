# legends-of-code-magic-constructed — critique et journal des tentatives

Format : constat / hypothèse / sources / action / résultat / leçon.

### 2026-11-26 — Legends of Code & Magic: Constructed — v1 deck valeur/mana + combat glouton
- Constat : jeu non soumis
- Hypothèse : un deck à bonne courbe + gardes puis tête bat le milieu
- Sources : referee1.5 ConstructPhase.java (CHOOSE = baseId)
- Action : deck 15x2, sac à dos des invocations, objets, attaques
- Résultat : fuzz 200/200 ; 4/4 contre rang 57/114 ; soumis
- Leçon : lire le parsing du referee pour l'identifiant attendu (baseId vs instanceId)
