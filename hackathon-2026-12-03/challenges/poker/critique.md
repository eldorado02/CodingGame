# poker — critique et journal des tentatives

Format : constat / hypothèse / sources / action / résultat / leçon.

### 2026-11-26 — Poker — v1 paliers -> v2 équité Monte-Carlo
- Constat : v1 0/4 : 33 FOLD, 77 CHECK, éliminé par un boss qui suit et mise
- Hypothèse : décider à l'équité contre la cote du pot au lieu de paliers fixes
- Sources : sonde stdin (format des actions)
- Action : Monte-Carlo déterministe (xorshift32 partagé), évaluateur 7 cartes
- Résultat : fuzz 200/200 ; v2 2/4 contre boss ; soumis
- Leçon : aléatoire reproductible : même PRNG entier dans les 4 langages = fuzz différentiel possible
