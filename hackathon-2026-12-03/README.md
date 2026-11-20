# Préparation Hackathon CodinGame — 3 décembre 2026

Sujet exact inconnu à ce jour. Stratégie : traiter à l’avance une série de challenges
CodinGame passés (2016 → 2026), un par un, dans l’ordre chronologique, et produire
**4 implémentations idiomatiques par challenge** : **C, C++, Rust, Python**.

> Classement des modèles et chaîne de repli :
> [`../bot-programming/MODELS-RANKING.md`](../bot-programming/MODELS-RANKING.md).
> Deep search → `deepseek-v4.1-flash-free` ; code → `deepseek-v4.1-flash-free`.
> En cas de quota : descendre dans la chaîne de repli du fichier.

## Règles de quality

- Chaque langue suit **ses propres conventions** (nommage, structuration, gestion
  d’erreurs idiomatique). Pas de traduction mot à mot d’une langue vers une autre.
- C : structs + fonctions, gestions explicite des erreurs et de la mémoire.
- C++ : RAII, STL, types forts, `const correcteness`.
- Rust : ownership/borrowing, `Result`, pas d’`unwrap` dans la logique métier.
- Python : typing, dataclasses, `if __name__ == "__main__"`.
- Toute solution doit être testable en local avec un jeu de tests minimal contenu
  dans le dossier du challenge (`tests/`).

## Structure

```
hackathon-2026-12-03/
├── README.md                 ← ce fichier (pilotage + suivi)
├── dsa/                      ← Phase 1 : socle DSA commun
│   ├── README.md             ← structures couvertes + conventions par langue
│   ├── C/
│   ├── CPP/
│   ├── Rust/
│   └── Python/
└── challenges/               ← Phase 2 : un dossier par challenge
    └── YYYY-<pretty-id>/
        ├── statement.md      ← énoncé/contexte
        ├── research.md       ← résultats du deep search
        ├── plan.md           ← plan de résolution
        ├── critique.md       ← critique du plan avant code
        ├── solutions/
        │   ├── main.c
        │   ├── main.cpp
        │   ├── main.rs
        │   └── main.py
        └── tests/
```

## Phase 1 — Socle DSA commun (préalable obligatoire)

- [ ] Deep search : dépôts de référence pour les templates DSA dans les 4 langages
      (`krahets/hello-algo`, etc.)
- [ ] Définir la liste commune des structures couvertes
- [ ] Implémenter le socle dans les 4 langages, même niveau de qualité
- [ ] Documenter `dsa/README.md`

## Phase 2 — Challenges 2016 → 2026 (séquentiel, jamais en parallèle)

Pour chaque challenge :

1. Deep search via sous-agent avec le modèle du ranking → `research.md`
2. Rédaction du plan de résolution → `plan.md`
3. Critique du plan (avant tout code) → `critique.md`
4. Implémentation par un sous-agent dédié, 4 langages, base DSA de la Phase 1
   → `solutions/` + `tests/`
5. Validation locale, puis passage au challenge suivant

### Suivi

| # | Année | Challenge | Recherche | Plan | Critique | 4 langages | Testé |
|---|---|---|---|---|---|---|---|
| — | — | *(en attente de la Phase 1)* | | | | | |

## Progression

- **État actuel :** Phase 1 lancée.
