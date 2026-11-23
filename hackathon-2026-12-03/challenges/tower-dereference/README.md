# tower-dereference — guide de reprise

- Énoncé : `statement.md` (archivé le 2026-11-23). Ligue unique `Wood` (425 joueurs) : pas de promotion, seul le rang compte.
- Referee : https://github.com/eulerscheZahl/TowerDefense (Java, lu : `Constants.java`, `Tower.java`, `GunTower.java`, `Board.java` (vagues), `Referee.java` (actions), `Player.java` (score), `config/Boss.cs`).
- Protocole : init `playerId`, `width height`, carte (`.` canyon, `#` plateau) ; chaque tour argent/vies (moi, lui), tours (`type id owner x y damage range reload cooldown`, valeurs au format `0.#`), attaquants. Sortie : actions séparées par `;` (`BUILD x y TYPE`, `UPGRADE id DAMAGE|RANGE|RELOAD`, `PASS`). 50 ms par tour.
- Score = `100·vies + argent` ; un joueur sans vie perd. Les améliorations sont appliquées tout de suite, les constructions en fin de tour (une tour construite ce tour n'est pas améliorable). Conflit de construction : le joueur dont la base est la plus proche gagne la case (sinon message non bloquant).
- Template : `templates/board-2p` (structure gardée, code remplacé). Classe ALGO.md : **M** (gestion / placement).
- Bot soumis : **v1c** `41796833` (C++), sans état hors carte : chemin = cases du canyon sur un plus court chemin gauche-droite ; valeur d'une tour canon = Σ poids(case) × cadence ; poids = 1 + 3 si la case est couverte par notre colle ; achats gloutons par gain/coût (construire canon 100 / colle 70 sur la meilleure case libre, ou améliorer un canon) ; dégâts plafonnés à 15 (surpuissance) ; cases du demi-terrain adverse décotées de 10 % (conflits perdus).
- Résultats : v1 (canons seuls) 0/2 rang 197 ; v1b (+ colle) 2/2 rang 197 (Python), 0/3 rang 113 ; **v1c 2/3 rang 113** `6290431`.
- Prochaine étape : simuler les vagues (PV, vitesse, primes) pour valoriser le nombre de tours contre la cadence, tours de soin offensives.
- Commandes :
  - fuzz : `bash hackathon-2026-12-03/challenges/tower-dereference/tests/run.sh` (~15 s)
  - bench : `python3 bot-programming/tools/bench.py tower-dereference hackathon-2026-12-03/challenges/tower-dereference/solutions/cpp/bot.cpp 6290431 --n 3`
  - submit : `python3 bot-programming/tools/cg.py submit tower-dereference hackathon-2026-12-03/challenges/tower-dereference/solutions/cpp/bot.cpp --language C++`
