# onitama — guide de reprise

- Énoncé : `statement.md` ; referee github.com/eulerscheZahl/onitama. 5×5, 1 maître + 4 élèves, 5 cartes (2 par joueur + centre). Victoire : capturer le maître adverse ou poser son maître sur le sanctuaire adverse. 200 tours max. Ligues Wood 2 → Wood 1. Boss Wood 2 : `6797555`.
- Protocole (sonde) : id ; 5 lignes plateau (1re = rangée 5) ; 5 lignes cartes `owner id dx1 dy1 … dx4 dy4` (orientées pour leur prochain utilisateur : nos cartes et la carte centrale pour nous) ; N actions `cardId A1A2` / `cardId PASS`. Joueur 0 en bas (rangée 1), dy > 0 = vers le haut. 50 ms.
- Recherche : chareca/onitama-ai (αβ Python, 48e mondial) ; referee eulerscheZahl.
- Bot soumis : **v1** `41796749` — cartes normalisées dans le repère du joueur 0, negamax αβ ID, éval 100/élève − 4 × distance maître→sanctuaire, captures d'abord ; 30k nœuds (CG 7–13 ms, prof. 5–6).
- Bench v1 : 3/3 contre le boss Wood 2.
- Prochaine étape : TT, éval de contrôle (cases attaquées autour des maîtres), budget ×2.
- Fuzz : `bash tests/run.sh` (cartes aléatoires, 20 s).
