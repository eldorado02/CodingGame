# Classement des modèles OpenCode — du plus performant au moins cher

> Snapshot du **20 novembre 2026**. Catalogue vérifié avec `all: true` (les modèles
> marqués « anciens » sont sinon masqués) : **40 modèles, dont 11 gratuits**.
> Benchmarks : BenchLM / LLMBenchmark, données du 19–20 novembre 2026.
>
> Règle de repli : si un modèle est indisponible ou hors quota, descendre d'un rang
> dans le classement correspondant à la tâche (deep search ou code).

## ⚠️ Correction du 20/11

Une première version de ce fichier excluait à tort `deepseek-v4.1-flash-free` et
`nemotron-3.5-lightning-free` : l'outil catalogue les masquait par défaut
(`all: false`). Vérifié avec `all: true` : les deux existent et sont **gratuits**
(0 $ entrée / sortie). `deepseek-v4.1-flash` (sans `-free`) est bien payant (0,30 $ /
1,20 $) et reste exclu. Les classements ci-dessous sont corrigés.

---

## 🥇 DEEP SEARCH — recherche web, sources, synthèse

| # | Modèle OpenCode | Preuves clés | Nature des preuves |
|---|---|---|---|
| **1** | **`opencode/deepseek-v4.1-flash-free`** | LiveBench Research **78,6** ; HLE outils **63,9** ; GPQA-D **90,9** ; AA-LCR **84,0** ; BenchLM global **#23** | Indépendantes (LLMBenchmark, Vals AI, AA, BenchLM) |
| **2** | **`opencode/longcat-2.5-free`** | **BrowseComp 91,9 (4ᵉ mondial)** ; HLE outils 63,1 ; MCP Atlas 83,1 ; GDPval 84,8 | Auto-déclarées (26 benchmarks, non classé) |
| **3** | **`opencode/hy4-flash-free`** | HLE outils 55,2 ; APEX-Agents 63,1 ; OSWorld 2.1 68,1 ; legal research 38,5 | Auto-déclarées (11 benchmarks) |
| **4** | **`opencode/nemotron-3.5-super-vl-free`** | LiveBench Research 67,1 ; web-research 78,0 ; AA-LCR 69,0 ; Omniscience −2,9 | Indépendantes |
| **5** | **`opencode/mimo-v2.6-flash-free`** | AA-LCR 74,3 ; AA-HLE 35,1 ; AA Intelligence 26,7 ; **Omniscience −10,8** | Indépendantes |
| **6** | **`opencode/beam-free`** | web-research 76,8 ; AA Intelligence 17,7 ; overall 47,3 (#106) | Indépendantes (couverture partielle) |
| **7** | **`opencode/big-pickle`** (session actuelle) | Codebase QnA 50,8 % (recherche dans le code uniquement) | Indépendante, 1 benchmark |
| **8** | **`opencode/flint-40b-free`** | Aucune mesure de recherche ; IFEval 95,1 | Aucune |
| **9** | **`opencode/nemotron-3.5-lightning-free`** | GPQA-D 75,4 ; MMLU-Pro 81,9 (pas de recherche web) | Indépendantes |
| **10** | **`opencode/tidepool-alpha-free`** | Aucune donnée publique (stealth, 18/11) | Aucune — rang arbitraire |
| **11** | **`opencode/iceman-free`** | Aucune donnée publique (stealth, 06/11) | Aucune — rang arbitraire |

** Choix deep search : `deepseek-v4.1-flash-free`.**
** Pour trouver/citer des sources web uniquement : `longcat-2.5-free`** (BrowseComp 91,9).
** Repli : `hy4-flash-free`, puis `nemotron-3.5-super-vl-free`.**

---

## 🥇 CODE — SWE, génération, agent terminal

| # | Modèle OpenCode | Preuves clés | Nature des preuves |
|---|---|---|---|
| **1** | **`opencode/deepseek-v4.1-flash-free`** | BenchLM Coding **76,4 (#17)** ; LiveBench Coding **80,4** ; Codeforces **3471** ; Vibe Code 87,7 ; DeepSWE 77 | Indépendantes |
| **2** | **`opencode/flint-40b-free`** | **SWE-bench Verified 74,2** ; **LiveCodeBench v6 80,3** ; Terminal-Bench 2.1 54,9 ; IFEval 95,1 | Auto-déclarées (5 benchmarks) |
| **3** | **`opencode/longcat-2.5-free`** | **Terminal-Bench 2.1 88,4** ; DeepSWE 64,6 ; Codebase QnA 58,3 ; NL2Repo 60,5 | Auto-déclarées |
| **4** | **`opencode/mimo-v2.6-flash-free`** | BenchLM Coding 66,7 ; Terminal-Bench 2.1 87,6 ; DeepSWE 67,9 ; **ProgramBench 0,5** | Indépendantes + labo |
| **5** | **`opencode/hy4-flash-free`** | DeepSWE 64,6 ; Vibe 74,8 ; ProgramBench 16,0 ; Terminal-Bench 4.0 19,2 | Auto-déclarées |
| **6** | **`opencode/big-pickle`** (session actuelle) | SWE Atlas Codebase QnA 50,8 % ; mini-swe-agent 63/124 tâches | Indépendantes (ORPT-Bench) |
| **7** | **`opencode/nemotron-3.5-lightning-free`** | SWE-bench Verified 51,6 ; GPQA-D 75,4 ; overall #206 | Indépendantes |
| **8** | **`opencode/nemotron-3.5-super-vl-free`** | BenchLM Coding 46,5 ; LiveBench Coding 54,1 ; **Terminal-Bench 4.1 = 0 %** | Indépendantes |
| **9** | **`opencode/beam-free`** | BenchLM Coding 39,2 ; Arena coding Elo 1 475 ; Terminal-Bench 4.1 6,4 % | Indépendantes |
| **10** | **`opencode/tidepool-alpha-free`** | Aucune donnée publique | Aucune — rang arbitraire |
| **11** | **`opencode/iceman-free`** | Aucune donnée publique | Aucune — rang arbitraire |

** Choix code : `deepseek-v4.1-flash-free`.**
** Alternative (code agentique / SWE pur) : `flint-40b-free`, puis `longcat-2.5-free`.**
** Modèles payants disponibles si besoin : `claude-fable-5-2` et `gpt-6.2-astra` (Coding 92,3 ex aequo).**

---

## Chaîne de repli recommandée (hackathon 3 décembre)

| Tâche | 1ᵉʳ choix | 2ᵉ | 3ᵉ | 4ᵉ |
|---|---|---|---|---|
| Deep search | `deepseek-v4.1-flash-free` | `longcat-2.5-free` | `hy4-flash-free` | `nemotron-3.5-super-vl-free` |
| Implémentation 4 langages | `deepseek-v4.1-flash-free` | `flint-40b-free` | `longcat-2.5-free` | `mimo-v2.6-flash-free` |

## Limites

- Les chiffres de `longcat`, `flint` et `hy4-flash` sont **auto-déclarés** : BenchLM
  les affiche mais ne leur donne pas de score global (26, 5 et 11 benchmarks couverts).
- Les écarts de 1–2 points ne sont pas significatifs (intervalles 90 % qui se chevauchent).
- Les modèles `stealth` (`tidepool`, `iceman`) n'ont aucune mesure publique : rangs 10
  et 11 arbitraires.
- Un modèle `-free` peut avoir des limites de débit différentes du modèle payant.

## Sources

- Catalogue : `tools.opencode.models()` avec `all: true` (20/11/2026)
- https://benchlm.ai/ (19/11/2026) · https://benchlm.ai/api/data/leaderboard?format=json&mode=bench-align-v5&limit=1200
- https://benchlm.ai/best/web-research · /best/for/research · /best/for/coding
- https://benchlm.ai/models/longcat-2-5 · /flint-40b-a2-5b · /hy4-flash · /deepseek-v4-1-flash · /reflection-beam · /nemotron-3-5-super-vl-120b-a12b · /mimo-v2-6-flash
- https://llmbenchmark.ai/data/dataset.json (40 258 mesures, 20/11/2026)
- https://github.com/rosspeoples/orpt-bench · https://github.com/PhillipChaffee/big-pickle-swe-atlas

## Utilisation par les sous-agents

Le modèle doit être passé explicitement lors de la création du sous-agent, sinon le
harness utilise `opencode/big-pickle`. En cas d'erreur de quota, descendre dans la
chaîne de repli ci-dessus.
