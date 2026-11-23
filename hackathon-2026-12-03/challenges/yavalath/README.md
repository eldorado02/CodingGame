# yavalath — guide de reprise

- Énoncé : `statement.md` (archivé le 2026-11-23). Ligue unique `Wood` (256 joueurs) : pas de promotion, seul le rang compte.
- Referee : https://github.com/MultiStruct/Yavalath (lu : `HexGrid.java`, `Referee.java`, `Action.java`).
- Protocole : `myId` (1 ou 2) seulement au 1er tour du joueur ; chaque tour `9`, 9 lignes de chiffres (1 = moi, 2 = adversaire, **vue normalisée**), puis `x y` du dernier coup (`-1 -1` au tout début). Sortie `x y` = index dans la ligne, ligne. 1000 ms au 1er tour, 100 ms ensuite.
- Règle : la plus longue ligne passant par le coup ≥ 4 → victoire (même si une ligne de 3 est créée en même temps), = 3 → défaite. Plateau plein → nul (0-0).
- Vol (steal) : le joueur 2 peut rejouer la case du 1er coup à son 1er tour (vérifié en test game : `ThePopSpider` vole). Notre règle : on vole si la pierre est à distance ≤ 2 du centre.
- Géométrie : axial `Q = x - min(y,4) + 4`, directions E/O, bas-droite `(0,+1)`, bas-gauche `(-1,+1)`.
- Template : `templates/board-2p`. Classe ALGO.md : **B**.
- Bot soumis : **v1** `41796345` (C++) — αβ + ID, blocages forcés sans coût de profondeur, éval fenêtres de 4 à 2 pierres + différence de coups sûrs ; 14000 nœuds/tour (140000 au 1er).
- Résultats v1 : **3/3** contre le rang 133 `2706303`, **3/3** contre le rang 61 `6763457`.
- Budget CG mesuré : 10000 nœuds = 28 ms, 14000 = 48 ms (pire tour).
- Prochaine étape : bench contre le top 25 (`2466216`), table de transposition, bitboards (61 cases → u64) pour doubler la profondeur.
- Commandes :
  - fuzz : `bash hackathon-2026-12-03/challenges/yavalath/tests/run.sh` (CG_NODES=1500, ~30 s)
  - bench : `python3 bot-programming/tools/bench.py yavalath hackathon-2026-12-03/challenges/yavalath/solutions/cpp/bot.cpp 6763457 --n 3`
  - submit : `python3 bot-programming/tools/cg.py submit yavalath hackathon-2026-12-03/challenges/yavalath/solutions/cpp/bot.cpp --language C++`
