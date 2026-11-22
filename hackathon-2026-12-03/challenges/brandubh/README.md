# Brandubh

État : **en cours** — prototype Python, aucune soumission.

## Règles essentielles

- Plateau 7×7, 8 attaquants, 4 défenseurs, 1 roi. Le roi commence en `d4` (trône).
- Le joueur `0` contrôle les attaquants et joue en premier ; le joueur `1` contrôle
  les défenseurs et le roi.
- Déplacement orthogonal de toute distance, sans saut. Le trône et les coins sont
  traversables par les non-rois mais ne sont pas des cases d'arrivée.
- Capture : une pièce est prise si elle est encadrée sur deux côtés opposés par
  des « hostiles ». Les coins sont toujours hostiles ; le trône est hostile aux
  attaquants, et aux défenseurs seulement si le roi n'y est pas.
- Victoire attaquants : roi capturé ou défenseurs encerclés. Victoire défenseurs :
  roi dans un coin. Égalité après 200 tours.
- Limite de réponse : 1000 ms au premier tour, **50 ms** ensuite.

## Prototype retenu (en test)

`solutions/python/bot.py` — alpha-beta 2 plis (1 pli côté défenseur pour rester
sous la limite de 50 ms), évaluation basée sur :

- distance BFS du roi au coin le plus proche (pièces = murs) ;
- mobilité du roi ;
- attaquants adjacents au roi ;
- différence de matériel.

## Sources

- Énoncé officiel : `statement.md`.
- Referee : `n3l-ctrl/brandubh-codingame`, `Referee.java` et `Board.java`.

## Tests

Les test games se lancent avec :

```
python3 bot-programming/tools/cg.py test brandubh <fichier> --language Python3 --opponents 6913632
```

`6913632` (`jacek`) est le meilleur joueur Wood listé ; ce jeu n'a pas de boss
déclaré. Critère de validation : au moins 3 victoires sur 5.

## Historique court

- Premier prototype : timeout au 3e frame (le programme lisait un seul tour).
- Boucle `while True` ajoutée : timeout corrigé, mais 0/5 (fuite du roi).
- Évaluation sans BFS puis BFS avec murs : en cours de validation.
