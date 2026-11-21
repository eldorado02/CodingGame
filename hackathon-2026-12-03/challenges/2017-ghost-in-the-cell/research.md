# Recherche — Ghost in the Cell (2017-02-25)

## Sources

- Statement officiel : `statement.md` (API `/api/v1/puzzles/ghost-in-the-cell`).
- Socle DSA : `../../dsa/README.md` (Floyd-Warshall, file).
- Aucun code tiers copié.

## Mécanique (Wood 3)

7–15 usines, 21–105 liens, distances 1–20 tours. Chaque joueur possède une
usine initiale avec 15–30 cyborgs ; les autres sont neutres avec garnison.
**Une seule action par tour** en Wood 3 (`MOVE src dst n` ou `WAIT` ; les
actions multiples arrivent en ligue supérieure).

Ordre de résolution : déplacement des troupes → ordres → production (0–3 par
usine non neutre) → combats → conditions de fin. Une troupe en transit ne
combattant pas, une usine qui envoie tout reste à 0.

Victoire : plus de cyborgs après 200 tours, ou adversaire sans usine ni
troupe.

## Décisions de conception

- **Distances** : Floyd-Warshall sur le graphe des usines, une fois à l’init.
- **Forces effectives** : `cyborgs + alliés_en_approche - ennemis_en_approche`.
  On raisonne sur cette force pour ne pas envoyer une usine qui sera prise.
- **Choix de l’action** :
  1. défense prioritaire si une de nos usines a une force effective négative ;
  2. sinon capture de l’usine **neutre** la plus rentable (production × poids,
     distance faible, coût faible), en envoyant `garnison + 1` ;
  3. sinon attaque de l’usine ennemie la plus faible atteignable avec excédent ;
  4. sinon `WAIT`.
- **Idiomes** : C matrices statiques, C++ `vector`, Rust `Vec`, Python listes.

## Risques

| Risque | Mitigation |
|---|---|
| Envoyer trop (usine vidée puis prise) | On garde une réserve de 5 sur l’usine source. |
| Sous-estimer une défense | Forces effectives incluent les troupes en approche. |
| Action unique gaspillée | On choisit le meilleur couple source/cible global. |
| Timeout 50 ms | 15³ Floyd-Warshall en init, 15² par tour → trivial. |

### 2026-11-21 — recherche approfondie Wood 4
Sources consultées et retenues :
- [Agade — Ghost in the Cell Postmortem](https://github.com/Agade09/Agade-Ghost-in-the-Cell-Postmortem/blob/master/Agade_GitC_Postmortem.md) :
  garder assez d'unités pour 5 tours de défense, noter `valeur/(ETA² × coût)`,
  réévaluer l'état après chaque ordre, bombarder tôt les usines productives ;
- [devYaoYH — README #13 Legend](https://github.com/devYaoYH/CodinGame_GitC/blob/master/README.md) :
  simulation future, Floyd–Warshall, ordre renfort → attaque → redistribution ;
- [Lif3line — pythonBasic.py](https://github.com/Lif3line/ghostInTheCell/blob/master/pythonBasic.py) :
  coût de capture incluant les renforts ennemis et la production pendant le trajet.
Décision : remplacer l'heuristique locale par la projection temporelle et les
multi-ordres ; les variantes « défense serrée » testées en cours de recherche
ont donné 2/6 et ont été rejetées.

## 2026-11-21 — recherche approfondie Bronze (règle INC)

Sources consultées :
- Referee officiel : https://github.com/dreignier/cg-referee-ghost-in-the-cell/blob/master/src/main/java/Referee.java
- Agade (postmortem du vainqueur) : https://github.com/Agade09/Agade-Ghost-in-the-Cell-Postmortem/blob/master/Agade_GitC_Postmortem.md
- Kodle / devYaoYH (`viable_upgrade`, horizon 10) : https://github.com/devYaoYH/CodinGame_GitC/blob/master/parsimonyLegend.py
- Tbigman (1 INC/tour, spare > 10) : https://github.com/Tbigman/CG-GitC/blob/master/main.cpp
- RoboStac (#22, backline) : https://github.com/RoboStac/ghost-in-the-cell
- martin (#18, hub arrière) : forum https://forum.codingame.com/t/ghost-in-the-cell-feedback-strategy/2634
- eulerscheZahl (#6, distance hostile ≥ 3) : même fil
- PED (#3, pas d'INC quand on a ≥2 production d'avance) : même fil
- dyatlov (#19, blitzkrieg sans INC) : même fil
- Churchill & Buro, *Build Order Optimization in StarCraft*, AIIDE 2011 : https://davechurchill.ca/publications/pdf/aiide11-bo.pdf

Mécanique confirmée par le referee : `INC f` coûte 10 unités **après** les MOVE,
augmente la production de 1 (plafond 3), s'applique dès le tour courant, et est
silencieusement ignorée si stock < 10 ou production déjà maximale. Plusieurs INC
par tour sont légaux. INC sur une usine non possédée = défaite immédiate.

Économie : coût immédiat net −9 unités, gain +1/tour → rentabilité en ~10 tours.
Règles retenues (intersection des sources) : un seul INC par tour, usine amère
avec production 1–2, stock ≥ 15 après MOVE et défense à 10 tours, aucune usine
ennemie à moins de 3 tours, pas d'INC si notre production dépasse celle de
l'adversaire de ≥2, pas d'INC sur l'usine 0 en début de partie.
