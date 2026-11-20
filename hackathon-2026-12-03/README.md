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

- [x] Deep search : dépôts de référence pour les templates DSA dans les 4 langages
      (`krahets/hello-algo`, etc.) → `dsa/research.md`
- [x] Définir la liste commune des structures couvertes
- [x] Implémenter le socle dans les 4 langages, même niveau de qualité
- [x] Documenter `dsa/README.md`
- [x] Tests locaux des 4 langages → `dsa/run_tests.sh` (C, C++, Python, Rust 6/6)

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

## Superviseur (exécution autonome)

Le plan entier est piloté par `tools/supervise.py`, lancé détaché via
`./tools/run_supervisor.sh`. Il ne dépend pas de la session agent.

```sh
./tools/run_supervisor.sh          # démarrer / redémarrer en arrière-plan
tail -f logs/supervisor.log        # suivre l'exécution
cat STATUS.md                      # état lisible
touch supervisor.stop              # arrêt propre
cat logs/phase1-research-*.out     # sortie d'une étape
```

**Résilience :**
- Chaque étape tourne dans une session OpenCode dédiée avec un modèle explicite.
- L'état est écrit atomiquement dans `supervisor_state.json` après chaque événement.
- Quota épuisé / token coupé / contexte plein / `kill -9` → reprise exacte de
  l'étape courante dans la même session (ou session neuve avec passation si le
  contexte est saturé).
- En cas de quota, descente automatique dans la chaîne de repli de
  `supervisor.json` (calquée sur `MODELS-RANKING.md`), puis attente de 30 min
  avant un nouveau cycle — donc reprise automatique quand le quota revient.

**Passation de contexte (limite 250 K) :** chaque étape écrit un `HANDOFF.md`
de 3 000 caractères maximum. Le superviseur n'injecte que ce fichier et la liste
des étapes terminées dans le prompt de l'étape suivante : jamais l'historique
complet d'une session n'est rechargé.

## Progression

- **État actuel :** **Phase 1 terminée localement** dans la session `big-pickle`
  (socle DSA complet + tests verts dans les 4 langages). Le superviseur reste
  arrêté : toute nouvelle session OpenCode renvoie `Free usage limit reached`,
  y compris avec `big-pickle`. Son état a été avancé à `phase2.index` : il
  suffira de le relancer avec `./tools/run_supervisor.sh` quand le quota sera
  rétabli pour enchaîner sur la liste des challenges 2016→2026.
