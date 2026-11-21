# Recherche — Olymbits / Summer Challenge 2024 (2024-06-11)

## Sources
- Énoncé officiel : `statement.md`.
- Stub officiel : `cg.py stub summer-challenge-2024-olymbits`.
- Moteur officiel : `https://github.com/CodinGame/SummerChallenge2024Olymbits`
  (`src/main/java/com/codingame/game/mini/HurdleRace.java`, `Game.java`,
  `mini/MiniGame.java`).

## Mécanique confirmée par le source
- Piste de 30 cases, départ en 0, arrivée en 29 (`maxX = map.length() - 1`).
- Actions : `LEFT` +1, `DOWN` +2, `RIGHT` +3, `UP` +2 en ignorant **la première**
  case seulement (`jump = false` après chaque pas).
- Heurter une haie (`#`) pose le coureur **sur** la haie et déclenche un étourdissement
  de `STUN_DURATION = 2` tours (l'énoncé dit 3, le code dit 2 : le code fait foi).
- `EARLY_RACE_END = true` : la course s'arrête dès que **un** joueur atteint 29.
- Le classement est alors calculé sur les positions (ex aequo autorisés) ;
  un joueur disqualifié reçoit `-1`. Médailles : or/argent/bronze, score final
  `nb_argent + 3 * nb_or`.
- Le registre `GPU` vaut `"GAME_OVER"` pendant le tour de réinitialisation :
  l'action est ignorée mais doit tout de même être fournie.

## Générateur de piste
`startStretch = 3 + rand(5)` points, puis `3 + rand(4)` motifs `#...` ou `#....`,
puis complétion à 30 caractères. Deux haies sont donc toujours espacées d'au moins
3 cases : il est toujours possible d'éviter toutes les haies.

## Stratégie retenue
BFS sur les positions 0..29 avec transitions `LEFT/DOWN/RIGHT/UP`, en écartant
toute transition qui atterrirait sur une haie (un étourdissement coûte 2 tours et
ne peut jamais être rentable). On rejoue le BFS à chaque tour ; le premier pas du
plus court chemin est envoyé. Temps optimal observé : 12 tours quelle que soit la
piste générée, contre 15..20 tours pour les adversaires qui avancent en `RIGHT`
et se cognent aux haies.

## Risques
| Risque | Mitigation |
|---|---|
| Confondre la case d'arrivée (27/28/29) | Arrivée = `len(GPU) - 1`, confirmée par le source. |
| Croire que `UP` saute toutes les haies | Un seul pas ignoré, reproduit exactement dans les 4 langages. |
| Se faire étourdir | Transitions interdisant les cases `#`. |
| Tour de reset sans entrée valide | Action `LEFT` inoffensive si `GPU == "GAME_OVER"`. |
| Décalage de lecture des registres | 3 lignes de score puis `nbGames` lignes, lues à chaque tour. |

## Recherche externe Wood 1 (21/11/2026)
Dépôts publics consultés :
- `JakubDotPy/codingame_olymbits` (`master`) : en Bronze/Silver, les 4 machines
  sont 4 mini-jeux **différents** (Hurdle, Archery, Roller, Diving) avec des
  registres de sens différents, et le score final est le **produit** des 4 scores.
  Le bot pondère le vote de chaque jeu selon ses médailles déjà obtenues.
- `UAKobzar/Olymbits` (`master`) : énumère 4^8 séquences d'actions et minimise
  le jeu le plus faible (index déterminé par `nb_gold*3 + nb_silver`).

⚠️ Ces stratégies visent les ligues **supérieures**. L'énoncé Wood 1 lu via
l'API confirme que les 4 machines y jouent **la même course d'obstacles** ;
les registres `reg0..reg5` sont position/stun des 3 joueurs. Le vrai levier
Wood 1 est donc le **classement relatif** dans chaque course : deux joueurs à
égalité reçoivent tous deux l'or, le dernier reçoit le bronze, et un score
0 dans un seul jeu annule le produit entier.

## Résultat de l'énumération de séquences (21/11/2026)
Comparaison menée contre `5507122` (BOSS 2) et `5507574` :
| Approche | Victoires / 5 |
|---|---|
| toujours `UP` (référence) | 0/5 |
| BFS classement relatif + pondération médailles | 2/5 |
| look-ahead 3 cases + vote majoritaire | 2/5 |
| énumération horizon 6 | 0/5 (timeout, `scores: 0=-1`) |
| énumération horizon 4 | 1/5 |
| **énumération horizon 5** | **5/5 + 5/5** |

Le point clé est que l'action est **partagée** entre les 4 machines : il faut
donc évaluer une séquence sur toutes les pistes simultanément, et non choisir
le meilleur coup de chaque piste. Le modèle d'étourdissement (3 tours d'immobilité
après collision) est déterminant : le boss joue `UP` en boucle et reste
récurrentément étourdi sur les haies de la deuxième case.

## Spécificité Bronze (21/11/2026)
L'énoncé Wood 1 décrit une seule course d'obstacles répétée ; la ligue Bronze
active **4 mini-games simultanés** (course d'obstacles, archerie, roller,
plongée) avec une action partagée. Le bot Wood 1 reste compétitif contre le boss
Bronze à condition de ne pas planter sur les registres signés de l'archerie.

Références externes consultées :
- `JakubDotPy/codingame_olymbits` (`bronze_league.py`, `dynamic_weights.py`,
  `silver_league.py`) — stratégies par mini-game et vote pondéré ;
- `UAKobzar/Olymbits/Player.cs` — énumération de 4^8 séquences et simulation des
  4 mini-games (trop lent en Python sous la limite de 50 ms, mais confirme que
  l'action partagée est le cœur du problème) ;
- `CGjupoulton/SummerChallenge2024Olymbits` — source Java du referee, utilisée
  pour confirmer que l'archerie stocke des coordonnées signées dans les
  registres.

Décision : conserver l'énumération 4^5 déjà validée et corriger uniquement la
gestion des positions négatives, plutôt que de réécrire le bot pour 4 mini-games
(les prototypes de simulation multi-jeux ont obtenu 3/8 contre le boss, contre
13/13 pour la version corrigée).
