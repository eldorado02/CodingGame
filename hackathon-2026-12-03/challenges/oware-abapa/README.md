# oware-abapa — guide de reprise

- Énoncé : `statement.md` ; referee lu : github.com/MaximeCheramy/cg-oware (Referee.java). Ligues Wood 2 → Wood 1. Boss Wood 2 : `2469367`.
- Règles exactes (referee) : maison jouable si non vide et (adversaire a des graines ou le semis l'atteint : graines ≥ 6 − i) ; semis saute la maison d'origine ; captures en remontant tant que 2 ou 3 côté adverse ; grand chelem = capture annulée ; score > 24 gagne ; sans coup légal le joueur prend tout ; 200 tours max.
- Protocole : 12 entiers par tour, nos 6 maisons d'abord ; sortie = index ABSOLU (joueur 1 → 6..11) ; id déduit du 1er plateau (tout à 4 = joueur 0) ; scores reconstitués (graines disparues = captures adverses).
- Recherche : marchete/CGZero (AlphaZero Oware sur CodinGame), joansalasoler/oware (Aalina, moteur fort), blackprince001/oware.
- Bot soumis : **v1** `41796595` — negamax αβ ID ; éval = 4 × écart de score + graines de notre côté − graines adverses ; 60k nœuds/tour (CG ≈ 10 ms, prof. 9–10).
- Bench v1 : 4/4 contre le boss Wood 2 (25–7).
- Prochaine étape : budget ×3 (limite 50 ms), TT, éval Aalina (maisons à 1–2 graines vulnérables) pour Wood 1.
- Fuzz : `bash tests/run.sh` (flux auto-joués, 40 s).
