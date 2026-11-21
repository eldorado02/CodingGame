# Critique — Back Track King (Wood)

## Avant implémentation
| Risque | Mitigation |
|---|---|
| Objectif de ligue mal identifié | Énoncé + sondage : une seule connexion active suffit, le boss passe son tour. |
| `AUTOPLACE` mal compris | Observation de replays : construction progressive, interruptions quand le budget de 3 paint est dépassé, reprise au même appel. |
| Paire de villes invalide | On ne tire que des paires explicitement présentes dans `desiredConnections`. |
| Dépassement des 100 tours | Bornes du plateau : le chemin le plus cher reste très inférieur à 300 points. |
| Processus tué entre les tours | Lecture jeton par jeton avec tampon, boucle `while true`, jamais de `read()` global. |
| Sorties divergentes | Même clé `(Manhattan, u, v)` dans les 4 langages. |

## Après implémentation
- `tests/run.sh` : **10/10 cas déterministes + 200/200 cas de fuzz identiques**
  entre Python, C, C++ et Rust, sans avertissement de compilation.
- Trois bugs de lecture corrigés :
  1. **Python** : la première version supposait `myId`, `width`, `height` sur
     trois lignes distinctes ; une partie officielle a renvoyé
     `ValueError: not enough values to unpack (expected 2, got 1)` et un timeout.
     Le bot lit désormais un flux de jetons avec tampon.
  2. **Rust** : deux erreurs de compilation successives (emprunt d’une chaîne
     temporaire, puis en-tête supposé sur une seule ligne). Le bot Rust lit
     maintenant les jetons depuis un `BufRead` avec `VecDeque<String>`.
  3. **Générateur** : il écrivait l’en-tête sur deux lignes ; il émet maintenant
     `myId width height` sur une seule ligne, ce qui reproduit le cas qui a fait
     échouer la première version Python.
- Test games officiels contre le boss Wood `6767604` : **20/20 victoires**
  (4 langages × 5 parties, plus une partie Rust de confirmation après une
  limite API 429). Scores `0 – -1` à chaque fois, objectif atteint en 3 à 5 tours.
- Limites connues : cette solution est strictement calibrée sur l’objectif de
  la ligue 1. Elle ignore le score, le routing précis de `AUTOPLACE`, les
  collisions, la disruption (`DISRUPT`) et les POI. En ligue supérieure il
  faudra construire une vraie optimisation de chemins, contester les tracks
  adverses et gérer l’instabilité des régions.
