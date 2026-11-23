# brandubh — guide de reprise

- Énoncé : `statement.md`. Ligue unique `Wood` (27 joueurs) : seul le rang compte.
- Referee lu : `github.com/n3l-ctrl/brandubh-codingame` `Referee.java` (règles exactes dans l'en-tête de `solutions/python/bot.py`).
- Protocole : `role` (0 attaquant, 1 défenseur), `lastAction`, 7 lignes de plateau (rangée 7 en haut). Sortie `d2d4`. 1000 ms puis **50 ms**.
- Bot soumis : **v9** `41795979` (C++) — template board-2p, αβ ID, budget 12k nœuds/tour (CG ~370k nœuds/s), filtre anti-répétition (3e répétition = défaite du joueur qui la crée).
- Résultats v9 (attaquant, toujours agent 0 en test) : 0/3 contre le rang 15 `6872530`. L'ancien v8 battait le rang 23 `6879824` (adversaire qui plante).
- Prochaine étape : moteur plus rapide (prof. ≥4), éval défenseur dédiée, étude des parties perdues `bot-programming/games/brandubh/logs/v9*-*.log`.
- Commandes : fuzz `bash tests/run.sh` (CG_NODES=1500, 40 s) ; bench `python3 bot-programming/tools/bench.py brandubh <cpp> 6872530 --n 3`.
