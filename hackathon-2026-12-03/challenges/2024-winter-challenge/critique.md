# Critique — Cellularena (Wood)

## Avant implémentation
| Risque | Mitigation |
|---|---|
| Type `BASIC` confondu avec la protéine `B` | Comparaison exacte du type dans les 4 langages. |
| Croissance hors voisinage rejetée | Le BFS ne renvoie que des cibles adjacentes à un organe possédé. |
| Stocks insuffisants | On n'émet un `GROW` que si `myA > 0`. |
| Grille relue une seule fois | Boucle `while true` avec lecture de `entityCount` lignes par tour. |
| Score mal compris | Source Java : score = nombre d'organes, fin dès plus de protéines exploitables. |

## Après implémentation
- `tests/run.sh` : 10 scénarios déterministes + 200 cas de fuzz, **210/210 cas
  identiques** entre Python, C, C++ et Rust.
- Trois divergences réelles trouvées et corrigées :
  1. **C** : `kind[0] >= 'A' && kind[0] <= 'D'` classait les organes `BASIC`
     comme des sources `B` ; le bot réessayait indéfiniment de pousser sur le
     même organe (`cannot grow onto organ`). Correction : exiger `kind[1] == '\0'`.
  2. **C++ / Rust** : une case libre non-source pouvait être retenue avant
     qu'une source ne soit trouvée (sentinelle `-1`/`None` mélangée aux clés).
     Correction : deux passes explicites, sources d'abord, cases libres ensuite.
  3. **Générateur de tests** : il n'émettait que des `ROOT` et inversait `owner`.
     Correction : organes `BASIC` générés et `owner = 1` pour le joueur local.
     C'est ce générateur renforcé qui a révélé les deux bugs ci-dessus.
- Test games officiels contre le boss Wood `5736694` : **19/19 victoires**,
  score `15 – 14` à chaque fois, dans les 4 langages (5 parties par langage,
  une requête 429 absorbée par le délai de retry). Les autres boss Wood
  (`5738801`, `6501284`, `5737299`, `5736741`) sont battus `47–1` à `53–1`.
- Limites : le bot ne considère ni les organes adverses comme obstacles stratégiques
  ni les collisions de growth (une seule action par tour en ligue Wood). Ce choix
  est suffisant ici car la partie s'arrête dès qu'un joueur n'a plus de protéines,
  et capturer une source vaut toujours +2 A nets. En ligue supérieure, il faudra
  gérer `HARVESTER`, `TENTACLE`, `SPORER`, les attaques et les quatre familles de
  protéines.

## Montée en ligue Wood 3 — HARVESTER (soumission `41776467`)
L'énoncé générique affirme que seul `BASIC` existe dans les ligues Wood, mais
les métadonnées des test games prouvent que **Wood 3 active le HARVESTER**
(`growthTypes_harvester_0=1`, `harvestedProteins_A_1=2`). La défaite 16–17
venait d'un détail précis : le bot absorbait lui-même la source que son propre
HARVESTER récoltait — le `GROW BASIC` vers une source plus lointaine posait le
premier organe **sur** la source harvesée (`gained 3A from absorption` au lieu
du `gained 1A from harvest` répété du boss).

Correction décisive : les cases sources d'un HARVESTER sont **bloquées dans le
BFS** (et pas seulement exclues de la liste des objectifs). Le chemin vers une
autre source ne peut plus traverser la source récoltée.

Validation : **9/9 parties gagnées** contre le boss Wood 3 `5736695` (scores
18–17 et 19–18 ; deux erreurs réseau `gaierror` exclues). Le prototype précédent
sans blocage BFS plafonnait à 3/5. **210/210 cas différentiels** identiques dans
4 langages, avec un générateur étendu aux organes HARVESTER, aux 4 types de
protéines et aux stocks C/D non nuls. Soumission officielle : `41776467`.
