# Connect 4

État : **Wood 2**, soumission C++ `41786612` (en cours de classement).

## Stratégie

Plateau 7×9, alpha-beta à profondeur fixe 6, avec table de transpositions
(16 entrées par clé), ordre des coups centre → bords, évaluation par fenêtres
de 4 cases avec poids `1,2,4,8`, bonus `+2` pour le center-top, détection
immédiate des victoires et des parties nulles. Le second joueur joue toujours
`STEAL` quand l'action est disponible.

```
score = Σ_fenetres (avec p uniquement : 1 << (nb_p - 1))
      - Σ_fenetres (avec adv uniquement : 1 << (nb_adv - 1))
      + 2 * (case centrale du haut == p) - 2 * (case centrale du haut == adv)
```

## Fichiers

- `solutions/python/bot.py` — référence.
- `solutions/c/bot.c`, `solutions/cpp/bot.cpp`, `solutions/rust/src/main.rs`.
- `tests/run.sh` — fuzz différentiel 200 cas.
- `statement.md`, `research.md`, `plan.md`, `critique.md`.

## Résultats

- Fuzz : **200/200** identiques dans 4 langages.
- Test games C++ : **5/5** (`scores: 0=10 1=0`) contre le boss officiel `6797564`.
- Soumission C++ `41786612`.

## Commandes

```
python3 bot-programming/tools/cg.py test connect-4 \
  hackathon-2026-12-03/challenges/connect-4/solutions/cpp/bot.cpp \
  --language C++ --opponents 6797564
python3 bot-programming/tools/cg.py submit connect-4 \
  hackathon-2026-12-03/challenges/connect-4/solutions/cpp/bot.cpp --language C++
```
