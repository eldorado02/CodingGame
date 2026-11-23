# tower-dereference — critique et journal des tentatives

Format : constat / hypothèse / sources / action / résultat / leçon.

### Checklist plan (PLAYBOOK §3)
- Actions multiples par tour séparées par `;`, argent jamais dépassé. VALIDÉ.
- Amélioration d'une tour construite le même tour impossible → seules les tours existantes sont améliorées. VALIDÉ.
- Conflit de construction non bloquant (message) mais coûteux (case perdue). Corrigé en v1c.
- Byte-identité : uniquement des entiers ; niveaux déduits des valeurs imprimées (tolérance 1e-9, virgule acceptée). VALIDÉ (fuzz 200/200).

### v1 — canons seuls (Python, test de protocole)
- Constat : 0/1 contre le rang 197 (`ChuckTheNinja`), perte de toutes les vies.
- Hypothèse : couvrir le chemin suffit.
- Sources : logs `probe-py-*`, `diag-*` (argent/vies par tour).
- Résultat : l'adversaire gagne avec une tour de colle près de ses canons.
- Leçon : la colle (ralentissement ×4 pendant 8 tours) multiplie l'exposition, elle vaut plus qu'un canon.

### v1b — colle valorisée
- Action : poids 1 + 3 pour les cases collées ; option colle valorisée par la cadence qu'elle amplifie ; tours planifiées du tour comptées dans le modèle.
- Résultat : 2/2 rang 197 ; 0/3 rang 113 (case (14,5) perdue en conflit au tour 1, puis surinvestissement dans un seul canon).

### v1c — plafond de dégâts + décote du demi-terrain adverse, soumission `41796833`
- Hypothèse : 30 dégâts contre 18 PV = gaspillage ; les conflits sont perdus sur le demi-terrain adverse.
- Action : `DAMAGE_CAP = 15`, `CONTESTED_PERCENT = 90` (4 langages), fuzz 200/200.
- Résultat : 2/3 contre le rang 113.
- Leçon : dans un jeu de placement simultané, lire la règle de conflit et l'intégrer au choix des cases.
