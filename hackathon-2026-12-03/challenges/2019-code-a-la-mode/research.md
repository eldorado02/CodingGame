# Recherche — Code a la Mode (2019-03-08)

## Sources

- Énoncé officiel : `statement.md`.
- Referee et modèles : https://github.com/csj/code-a-la-mode
  - `src/main/kotlin/com/codingame/game/Referee.kt`
  - `src/main/kotlin/com/codingame/game/Player.kt`
  - `src/main/kotlin/com/codingame/game/model/BoardFeatures.kt`
  - `src/main/kotlin/com/codingame/game/model/Dishes.kt`, `Items.kt`,
    `Customers.kt`, `Containers.kt`, `ItemsAndEquipment.kt`
  - `USE.md`, `src/test/starterkit/starterAI.py`
  - **Lecture seule, aucun code copié.**

## Mécanique (Wood 4 = league 1)

- Grille 11×7. 3 joueurs, 3 rounds de 200 tours, 2 joueurs actifs par round ;
  les points du round sont partagés entre les deux chefs.
- Un tour = une seule action pour le joueur désigné (alternance).
- Déplacement pondéré : voisin orthogonal coûte 2, diagonal coûte 3
  (`REACH_DISTANCE = 3`, `WALK_DISTANCE = 9`). Un `USE` hors de portée
  déclenche un rapprochement automatique.
- Toutes les cases non `.` sont des « tables » : `D`, `W`, `B`, `I`, `#`.
- Ingrédients illimités aux caisses `B` (BLUEBERRIES) et `I` (ICE_CREAM).
- `USE D` : sans rien → `DISH` ; avec un aliment → `DISH + aliment`.
- `USE caisse` avec `DISH` → ajoute l'aliment au plat.
- `USE table avec item` avec `DISH` → ajoute l'aliment au plat.
- `USE W` avec un plat complet → livraison, +points pour les deux,
  une nouvelle vaisselle apparaît.
- En league 1, les commandes ne contiennent que `ICE_CREAM` et
  `BLUEBERRIES` (2 ingrédients) : il faut toujours les deux.
- Le prix baisse de 1 par tour d'attente.

## Décisions de conception

1. Mémoriser la cuisine au premier tour (positions de `D`, `W`, `B`, `I`).
2. Machine à états, une seule commande `USE` par tour :
   - plat complet → `W` ;
   - plat partiel → caisse / table manquante ;
   - aliment seul → `D` (donne directement `DISH + aliment`) ;
   - rien → table avec plat/aliment, sinon `D`.
3. Comparer les contenus comme des ensembles (l'ordre de `Dish.describe()`
   dépend de l'ordre d'insertion).
4. Arithmétique entière, ordre de priorité identique dans les 4 langages.

## Risques

| Risque | Mitigation |
|---|---|
| `USE` hors de portée | Autorisé : le referee rapproche automatiquement. |
| Vaisselle épuisée | `USE D` invalide = `WAIT` ; on réessaie plus tard. |
| Partner sur la même cible | Points partagés ; travail dupliqué seulement. |
| Item sur table inconnue | On n'utilise que les tables annoncées. |
| Timeout 50 ms | 77 cases, recherche linéaire. |
