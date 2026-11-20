# Choix des modèles disponibles dans cet environnement

> Snapshot du **20 novembre 2026**. Catalogue vérifié avec `all: true` :
> **40 modèles, dont 11 gratuits**. Benchmarks lus le 19–20 novembre 2026.
>
> ✅ **Catalogue corrigé :** une première version excluait à tort
> `deepseek-v4.1-flash-free` et `nemotron-3.5-lightning-free` : l'outil les
> masquait par défaut (`all: false`). Vérifié avec `all: true` : les deux
> existent et sont **gratuits**. `deepseek-v4.1-flash` (sans `-free`) est bien
> payant et reste exclu. Pour le classement 1→N détaillé (deep search / code),
> voir [MODELS-RANKING.md](MODELS-RANKING.md), qui fait foi.

## TL;DR

| Usage | Meilleur **gratuit** | Meilleur **tout modèles confondus** |
|---|---|---|
| Recherche générale / faits | **`opencode/deepseek-v4.1-flash-free`** | **`opencode/claude-fable-5-2`** |
| Recherche web / sources | **`opencode/longcat-2.5-free`** | **`opencode/gemini-4-neon`** |
| Code / agents SWE | **`opencode/deepseek-v4.1-flash-free`** | **`opencode/claude-fable-5-2`** = **`opencode/gpt-6.2-astra`** |
| Code économique | `opencode/deepseek-v4.1-flash-free` | `opencode/gpt-6.2-sol`, `opencode/claude-sonnet-5-5` |

Nuance importante : `longcat-2.5-free` est le meilleur gratuit pour **trouver et citer des sources sur le web** (BrowseComp 91.9, 4ᵉ de tous les modèles), tandis que `deepseek-v4.1-flash-free` est le meilleur gratuit pour la **recherche "connaissance + raisonnement"** avec des mesures indépendantes (LiveBench Research 78.6, HLE avec outils 63.9, GPQA-D 90.9, AA-LCR 84.0).

## Modèles gratuits

| Modèle OpenCode | Modèle d'origine | Date | Preuve publique | Recherche | Code |
|---|---|---|---|---|---|
| `tidepool-alpha-free` | inconnu (stealth, 18/11/2026) | 2026-11-18 | aucune — juste le catalogue | ? | ? |
| `hy4-flash-free` | Tencent Hy4 flash | 2026-11-16 | 11 lignes BenchLM, non classé | HLE w/ tools 55.2 ; APEX-Agents 63.1 | DeepSWE 64.6 ; ProgramBench 16.0 ; Vibe Code 74.8 |
| `iceman-free` | inconnu (stealth, 06/11/2026) | 2026-11-06 | aucune — juste le catalogue | ? | ? |
| `longcat-2.5-free` | Meituan LongCat-2.5 | 2026-11-02 | 26 lignes BenchLM, non classé | **BrowseComp 91.9 (#4 global)** ; HLE w/ tools 63.1 ; MCP Atlas 83.1 | DeepSWE 64.6 ; Terminal-Bench 2.1 88.4 ; NL2Repo 60.5 |
| `flint-40b-free` | Recursive Flint 40B A2.5B | 2026-10-21 | 5 lignes BenchLM, non classé | — | **SWE-bench Verified 74.2** ; LiveCodeBench v6 80.3 ; IFEval 95.1 |
| `deepseek-v4.1-flash-free` | DeepSeek V4.1 Flash | 2026-10-17 | 69 lignes BenchLM, **#23 global** | LiveBench Research 78.6 ; HLE w/ tools 63.9 ; GPQA-D 90.9 ; AA-LCR 84.0 | **Coding 76.4 (#17)** ; LiveBench Coding 80.4 ; Vibe Code 87.7 ; Codeforces 3471 |
| `nemotron-3.5-super-vl-free` | NVIDIA Nemotron 3.5 Super VL 120B A12B | 2026-10-15 | #112 global | Web research 78.0 ; LiveBench Research 67.1 | Coding 46.5 ; LiveBench Coding 54.1 |
| `beam-free` | Reflection Beam 1.0 501B A23B | 2026-10-05 | #106 global | Web research 76.8 ; AA Intelligence 17.7 | Coding 39.2 |
| `mimo-v2.6-flash-free` | Xiaomi MiMo-V2.6-Flash | 2026-09-22 | #43 global | AA-HLE 35.1 ; Omniscience −10.8 | Coding 66.7 ; DeepSWE 67.9 ; Vibe Code 78.96 |
| `nemotron-3.5-lightning-free` | NVIDIA Nemotron 3.5 Lightning 30B A3B | 2026-08-11 | #206 global | GPQA-D 75.4 ; MMLU-Pro 81.9 | SWE-bench Verified 51.6 |
| `big-pickle` (modèle actuel) | stealth ; communauté → GLM-4.6 (non confirmé) | 2025-10-17 | SWE Atlas Codebase QnA 50.8 % | — | 63/124 tâches résolues (mini-swe-agent, 11/08/2026) |

Les scores des modèles `flint`, `longcat`, `hy4-flash` et `deepseek` sont **auto-déclarés** par les laboratoires. Les scores BenchLM / LLMBenchmark (classement_global, LiveBench, Vals AI, Artificial Analysis) sont indépendants. Les variantes `-free` peuvent être servies avec des limites de débit et, pour certaines, une conservation des données ; un chiffre publié pour le modèle payant équivalent n'est donc pas une garantie absolue pour la variante gratuite.

## Modèles payants disponibles — meilleurs choix

| Usage | Modèle | Chiffres clés |
|---|---|---|
| Recherche (générale) | `claude-fable-5-2` | BenchLM "best for research" #1 : AA-Omniscience **66.8** ; LiveBench Research 94.5 ; HLE 59.7 ; Intelligence AA 64.6 |
| Recherche web | `gemini-4-neon` | LiveBench Browser **95.6** ; LiveBench Research 94.5 ; AA-Omniscience 54.8 |
| Code | `claude-fable-5-2` / `gpt-6.2-astra` | BenchLM Coding **92.3** ex aequo ; Fable #1 overall (93.2) |
| Code pas cher | `gpt-6.2-sol` / `claude-sonnet-5-5` | Coding 86.9 / 86.5 à 10 $/M tokens en sortie |

Rappel : `claude-opus-5-5` (coding 88.3) et `gpt-6.2-sol` (86.9) sont aussi disponibles ; `gemini-4-carbon` (coding 91.8, meilleur rapport qualité/prix selon BenchLM) **n'est pas** dans le catalogue de cet environnement.

## Méthode et sources

- **Recherche** = AA-Omniscience Index (faits + retenue à ne pas répondre), HLE / HLE avec outils, BrowseComp, LiveBench Research/Browser, AA-LCR, GPQA-D.
- **Code** = BenchLM Coding Score (agrège SWE-bench Pro, DeepSWE, LiveCodeBench v6, NL2Repo, FrontierCode, Terminal-Bench, Bug Hunt Bench…), SWE-bench Verified, LiveBench Coding, Codeforces.
- Sources :
  - https://benchlm.ai/ (classement global et catégories, 19/11/2026)
  - https://benchlm.ai/best/for/research · https://benchlm.ai/best/for/coding · https://benchlm.ai/best/web-research
  - https://benchlm.ai/api/data/leaderboard?format=json&mode=bench-align-v5&limit=300 (scores par catégorie)
  - https://llmbenchmark.ai/data/dataset.json (40 258 mesures, 20/11/2026)
  - https://benchlm.ai/models/deepseek-v4-1-flash · /longcat-2-5 · /flint · /hy4-flash · /mimo-v2-6-flash
  - https://github.com/rosspeoples/orpt-bench et https://github.com/PhillipChaffee/big-pickle-swe-atlas pour `big-pickle`
- Un écart de 1–2 points n'est **pas** une preuve de supériorité : les intervalles 90 % se chevauchent souvent (ex. Fable 5.2 : 88.4–97.9).

## Utilisation

Je ne peux pas changer **moi-même** le modèle de cette session. En revanche, si tu demandes explicitement un modèle, je peux lancer un sous-agent dessus, par exemple :

```
utilise opencode/longcat-2.5-free pour chercher les stratégies Legend de Mad Pod Racing
```

Sans demande explicite de ta part, je continue avec le modèle configuré par le harness (`opencode/big-pickle`).
