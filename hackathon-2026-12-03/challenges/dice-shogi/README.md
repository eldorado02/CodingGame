# dice-shogi — guide de reprise

- Énoncé : `statement.md` ; referee github.com/jdermont/DiceShogi. Minishogi + dé : la colonne d'arrivée est imposée par le dé (repli libre si aucun coup). Ligue unique `Wood` (81 joueurs).
- Protocole : identique à `minishogi` + une ligne `rolledDice` après les mains.
- Code : copie de `minishogi` avec DICE activé (Python `DICE = True`, C/C++ `#define DICE 1`, Rust `const DICE: usize = 1`).
- Bot soumis : **v1** `41796284` — même glouton que minishogi (captures, promotions, carte d'attaque).
- Bench v1 : 1/3 contre le rang 40 `6236067`.
- Prochaine étape : espérance sur le dé adverse (expectimax 1 pli : pour chaque colonne 1..5, meilleure réponse adverse), moteur complet partagé avec minishogi.
