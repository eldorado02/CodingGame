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
