Tu es le sous-agent de DEEP SEARCH de la Phase 1 d'une préparation de hackathon CodinGame (le hackathon est le 3 décembre 2026, sujet inconnu). Réponds et écris en FRANÇAIS.

OBJECTIF : trouver les dépôts de référence fiables pour des templates/implémentations de structures de données et algorithmes (DSA) dans 4 langages : C, C++, Rust, Python. Point de départ suggéré : https://github.com/krahets/hello-algo . Trouve aussi des ressources spécifiques par langue (repos de competitive programming, docs officielles, crates utiles). Si l'outil websearch renvoie 402, utilise webfetch ou DuckDuckGo lite via fetch.

À FAIRE :
1. Évaluer chaque source : couverture, qualité du code, licence, maintenabilité, pertinence pour CodinGame (runtime serré, pas de dépendances lourdes).
2. Proposer une LISTE COMMUNE priorisée de structures à couvrir par le socle DSA (mêmes structures dans les 4 langages). Prioriser ce qui sert vraiment à CodinGame : graphes (BFS/DFS, Dijkstra, A*, SCC, topologique), files et files de priorité, union-find, segment tree / Fenwick, trie, math modulaire, géométrie, DP, tri/recherche, tables de hachage. Justifier brièvement chaque item.
3. Pour chaque structure, indiquer l'approche idiomatique dans chaque langue (ex. BinaryHeap + Reverse en Rust, priority_queue en C++, heapq en Python, tri manuel en C).

FICHIER À ÉCRIRE (chemin absolu) :
/home/arafaram/Projects/codingGame/hackathon-2026-12-03/dsa/research.md
Contenu : a) tableau des sources (URL, langues, licence, ce qu'on en tire), b) liste commune priorisée, c) tableau « structure × approche par langue », d) recommandation finale et estimation du travail.

Ne modifie aucun autre fichier. Ne lance pas d'autre sous-agent. Cite les URLs et distingue le vérifié du suggéré.
