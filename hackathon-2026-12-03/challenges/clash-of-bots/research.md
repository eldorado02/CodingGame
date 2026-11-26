# clash-of-bots — recherche

## Sources
- Referee : https://github.com/stivens/Clash-of-Bots (Config.kt, Interpreter.kt,
  MoveGraph.kt, InputGenerator.kt, Vector.kt), lu le 2026-11-26.

## Faits du referee
- Tore 15x15, 100 tours, PV 10, attaque 2, collision 1 (aux deux), explosion 4
  sur le 3x3, garde = dégâts / 2 arrondis bas ; vision 2.
- Ordre : garde, déplacements, attaques, explosions.
- Collision : case visée par >= 2 robots, échange de cases, ou robot immobile
  sur la case visée ; les chaînes de déplacements sont légales.
- UP = dy -1 = ligne précédente de la mini-carte.
