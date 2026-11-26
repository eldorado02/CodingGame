# yinsh — critique et journal des tentatives

Format : constat / hypothèse / sources / action / résultat / leçon.

### 2026-11-26 — Yinsh — v1 glouton 1 pli sur les actions légales
- Constat : jeu non soumis ; format de grille non documenté
- Hypothèse : demander les coups légaux (yes) évite d'écrire le générateur ; simuler + évaluer suffit
- Sources : sonde stdin brut ; referee MultiStruct/Yinsh
- Action : parse colonnes a..k, simulation (déplacement, retournement, retrait), score fenêtres de 5
- Résultat : fuzz 200/200 ; 4/4 contre rang 71/142 ; soumis
- Leçon : quand le referee fournit les actions légales, un 1-pli simulé est la v1 la plus rapide
