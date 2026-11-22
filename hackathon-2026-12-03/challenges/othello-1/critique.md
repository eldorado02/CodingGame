# Othello — critique

## Avant implémentation

- Boss Wood 2 `6797560` : joueur aléatoire (seed 13), sensiblement plus faible
  qu'un moteur alpha-beta même simple.
- Aucun historique de challenge : dossier créé de zéro.

## Après implémentation

### Tentatives

- `v1` (depth 5 fixe, évaluation avec mobilité) : timeout au 5ᵉ coup.
- `v2` (depth 4, évaluation sans mobilité) : 1V/5 — trop de matériel en milieu
  de partie.
- `v3` (budget temps 115 ms, **sans** élagage à la racine) : 2V/3D/5.
- `v4` (id. **avec** élagage à la racine) : **9V/1D/10**, soumis `41787330`.
- Budgets de nœuds 1200/1500/2000 et profondeurs fixes 4–6 (moteur tableau) :
  timeout ou 1V/5. Le coût variable de `evaluate` empêche un budget déterministe.
- `v5` (bitboard, profondeur fixe 6) : 10 000 positions validées, max 34,9 ms,
  **5V/5** contre le boss. Retenue.

## Leçons

- L'élagage à la racine divise le temps par ~3 et est indispensable sur le
  moteur tableau.
- Un budget temps n'est pas reproductible d'un langage à l'autre : le fuzz
  différentiel 200/200 échoue (16 divergences C/C++/Rust).
- Les bitboards rendent l'évaluation ~20× moins coûteuse : une profondeur fixe
  devient possible, ce qui réconcilie force, déterminisme et fuzz 4 langages.
- Ne pas confondre `x` (colonne) et `y` (ligne) : bit = `y*8+x`.

### Wood 1 — tentative profondeur 7 (rejetée)

- Constat : en Wood 1, la profondeur 6 perd 0/5 contre le rank 1 `6886490`
  (défaites d'un ou deux disques, sans timeout).
- Hypothèse : une profondeur supplémentaire améliore le résultat.
- Action : optimisation du moteur bitboard (table `PS` de sommes positionnelles
  16 bits, `evaluate(P,O,mP)` réutilisant les coups déjà calculés) pour faire
  passer la profondeur 7 de 108 ms à 85 ms sur le fuzz local.
- Résultat : 0/5 contre `6886490`, dont **1 défaite par timeout** sur CG.
  Le gain de profondeur ne compense pas le coût ; la version profondeur 6
  reste soumise (`41787653`).
- Leçon : les timings locaux sous-estiment les serveurs CodinGame d'un facteur
  ~1,5–2 ; garder une marge d'au moins 2× sous la limite de 150 ms.
