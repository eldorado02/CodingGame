# impasse — guide de reprise

- Énoncé : `statement.md` ; referee github.com/acatai/CG_Impasse ; règles Mark Steere (singles avancent en diagonale, doubles reculent, transposition, bear-off en rangée proche, couronnement en rangée lointaine, impasse = retirer un pion). Ligue unique `Wood` (115 joueurs). 300 tours max.
- Protocole (sonde) : couleur ; 8 lignes plateau (rangée 8 en haut) ; `lastMove` ; N coups légaux (1 à 3 coordonnées). Blanc : singles montent vers la rangée 8, doubles descendent vers la rangée 1 (bear-off).
- Bot soumis : **v1** `41796185` — glouton 1 coup sur la liste légale (impasse 50 > double en rangée proche 25 > transposition 12/+20 > 3 par rangée parcourue, +8 single en rangée lointaine, +2 couronnement).
- Bench v1 : 3/3 contre le rang 71 `5032434` (12–4).
- Prochaine étape : moteur complet (referee acatai) + αβ ; bench rangs 29/15.
- Fuzz : `bash tests/run.sh` (plateaux et chaînes aléatoires, ~10 s).
