# Classement 1 → N des modèles OpenCode **gratuits**

> Snapshot du **20 novembre 2026**. Catalogue : `tools.opencode.models()` → 38 modèles,
> dont **9 gratuits** (prix entrée/sortie = 0). Benchmarks : BenchLM et LLMBenchmark,
> données du 19–20 novembre 2026.

## ⚠️ Changement de catalogue depuis hier

Le modèle qui était premier dans [MODELS.md](MODELS.md) n'est **plus gratuit** :

| Modèle | Ancien statut | Statut actuel |
|---|---|---|
| `deepseek-v4.1-flash-free` → `opencode/deepseek-v4.1-flash` | gratuit | **payant** (0,30 $/M entrée, 1,20 $/M sortie) |
| `opencode/nemotron-3.5-lightning-free` | gratuit | **retiré du catalogue** |

Ils sont donc exclus du classement ci-dessous. À titre de référence, DeepSeek V4.1 Flash
aurait gardé la 1ʳᵉ place dans les deux disciplines (recherche : LiveBench Research 78.6,
HLE outils 63.9, GPQA-D 90.9 / code : BenchLM Coding 76.4, #17 global).

---

## 🥇 Classement DEEP SEARCH (recherche web + sources + synthèse)

Critères utilisés : BrowseComp, HLE avec outils, MCP Atlas, APEX-Agents, LiveBench
Research/Browser, web-research BenchLM, AA-LCR, AA-Omniscience (fiabilité).

| # | Modèle OpenCode | Preuves clés | Preuve | Verdict |
|---|---|---|---|---|
| **1** | **`opencode/longcat-2.5-free`** | **BrowseComp 91,9 % (4ᵉ mondial)** ; HLE outils 63,1 ; MCP Atlas 83,1 ; AutomationBench 51,8 ; GDPval 84,8 ; AA via labo | Auto-déclaré (26 benchmarks) | **Le plus fort de loin** pour trouver et citer des sources web. |
| **2** | **`opencode/hy4-flash-free`** | HLE outils 55,2 ; APEX-Agents 63,1 ; OSWorld 2.1 68,1 ; HLE 55,2 ; legal research Vals 38,5 ; valsai_index 50,8 | Auto-déclaré (11 benchmarks) | Bon chercheur outillé, derrière LongCat. |
| **3** | **`opencode/nemotron-3.5-super-vl-free`** | LiveBench Research **67,1** ; web-research 78,0 ; AA-LCR 69,0 ; HLE 30,7 ; Omniscience −2,9 | **Indépendant** ( BenchLM + LLMBenchmark) | Meilleur score de recherche **indépendant** ; browse faible (21,6). |
| **4** | **`opencode/mimo-v2.6-flash-free`** | AA-LCR **74,3** ; AA-HLE 35,1 ; AA Intelligence 26,7 ; legal research 38,0 ; **Omniscience −10,8** | Indépendant | Bon raisonnement long, mais hallucine trop pour de la recherche factuelle. |
| **5** | **`opencode/beam-free`** | web-research 76,8 ; AA Intelligence 17,7 ; overall 47,3 (#106) | Indépendant (couverture partielle) | Correct sur le web, pas de recherche profonde mesurée. |
| **6** | **`opencode/big-pickle`** (modèle actuel) | SWE Atlas Codebase QnA 50,8 % (recherche *dans le code*) | Indépendant, 1 seul benchmark | Pas de mesure de recherche web. Classé par défaut. |
| **7** | **`opencode/flint-40b-free`** | Aucune mesure de recherche ; IFEval 95,1 seulement | Aucune | Non évalué pour la recherche. |
| **8** | **`opencode/tidepool-alpha-free`** | Aucune donnée publique (stealth, 18/11/2026) | Aucune | **Non classable** — rang arbitraire. |
| **9** | **`opencode/iceman-free`** | Aucune donnée publique (stealth, 06/11/2026) | Aucune | **Non classable** — rang arbitraire. |

**→ Deep search : `longcat-2.5-free`.** Si tu veux une mesure indépendante plutôt
qu'auto-déclarée, prends `nemotron-3.5-super-vl-free`.

---

## 🥇 Classement CODE (SWE + génération + agent terminal)

Critères utilisés : SWE-bench Verified, DeepSWE, LiveCodeBench v6, Terminal-Bench,
SWE-Atlas Codebase QnA, NL2Repo, ProgramBench, BenchLM Coding, Vibe Code.

| # | Modèle OpenCode | Preuves clés | Preuve | Verdict |
|---|---|---|---|---|
| **1** | **`opencode/flint-40b-free`** | **SWE-bench Verified 74,2** ; **LiveCodeBench v6 80,3** ; Terminal-Bench 2.1 54,9 ; IFEval 95,1 ; Pro v2 89,7 (labo) | Auto-déclaré (5 benchmarks) | **Le plus fort en code pur** (fix/génération). |
| **2** | **`opencode/longcat-2.5-free`** | **Terminal-Bench 2.1 88,4** ; DeepSWE 64,6 ; Codebase QnA 58,3 ; NL2Repo 60,5 ; ProgramBench 25,5 | Auto-déclaré (6 benchmarks code) | **Le plus fort en code agentique / terminal.** |
| **3** | **`opencode/mimo-v2.6-flash-free`** | BenchLM Coding **66,7** ; Terminal-Bench 2.1 87,6 ; DeepSWE **67,9** ; Vibe 79,0 ; **ProgramBench 0,5** ; TB 4.0 24,2 | Indépendant + labo | Très bon sur les tâches classiques, s'effondre sur ProgramBench. |
| **4** | **`opencode/hy4-flash-free`** | DeepSWE 64,6 ; Vibe 74,8 ; ProgramBench 16,0 ; Terminal-Bench 4.0 19,2 ; gamebench failure 12,5 % | Auto-déclaré + indépendant | Solide DeepSWE, faible sur les benchmarks récents. |
| **5** | **`opencode/big-pickle`** (modèle actuel) | Codebase QnA 50,8 % ; mini-swe-agent **63/124 tâches** (11/08/2026) | Indépendant (ORPT-Bench, SWE Atlas) | Bonne compréhension de dépôt, données limitées. |
| **6** | **`opencode/nemotron-3.5-super-vl-free`** | BenchLM Coding 46,5 ; LiveBench Coding 54,1 ; **Terminal-Bench 4.1 = 0 %** ; FrontierCode 6,1 | Indépendant | Faible en code malgré un bon LiveBench Coding. |
| **7** | **`opencode/beam-free`** | BenchLM Coding 39,2 ; Arena coding Elo 1 475 ; Terminal-Bench 4.1 6,4 % | Indépendant | Dernier des modèles réellement évalués en code. |
| **8** | **`opencode/tidepool-alpha-free`** | Aucune donnée publique | Aucune | **Non classable** — rang arbitraire. |
| **9** | **`opencode/iceman-free`** | Aucune donnée publique | Aucune | **Non classable** — rang arbitraire. |

**→ Code : `flint-40b-free` pour le code pur, `longcat-2.5-free` pour le code agentique
(terminal / outils).** Les deux sont au coude-à-coude et leurs chiffres sont
auto-déclarés : n'hésite pas à les tester tous les deux sur une vraie tâche.

---

## Résumé express

| Besoin | 1ᵉʳ choix | 2ᵉ choix |
|---|---|---|
| Deep search / web | `longcat-2.5-free` | `hy4-flash-free` |
| Code | `flint-40b-free` | `longcat-2.5-free` |
| Les deux à la fois | `longcat-2.5-free` | `hy4-flash-free` |
| Généraliste équilibré | `mimo-v2.6-flash-free` | `big-pickle` |

## Méthode et limites

- Les classements 1–2 des deux tableaux reposent sur des **chiffres auto-déclarés**
  par les laboratoires (BenchLM les affiche mais ne les intègre pas au score global).
  Les rangs 3–7 s'appuient sur des mesures indépendantes (LLMBenchmark, Vals AI,
  Artificial Analysis, BenchLM aligné).
- Les écarts de 1–2 points ne sont **pas significatifs** : les intervalles 90 % se
  chevauchent largement (ex. MiMo-V2.6-Flash : 52,9–73,6).
- `longcat`, `flint` et `hy4-flash` sont **non classés** chez BenchLM (26, 5 et 11
  benchmarks seulement) : leur score global n'existe pas, d'où l'usage de leurs
  mesures brutes.
- Les variantes `-free` peuvent avoir des limites de débit différentes des modèles
  payants équivalents. Les scores publiés ne garantissent pas le comportement exact
  du point d'accès gratuit.
- `tidepool-alpha-free` et `iceman-free` sont des modèles "stealth" sans fiche ni
  benchmark public : leurs rangs (8 et 9) sont arbitraires, pas mesurés.

## Sources

- Catalogue OpenCode : `tools.opencode.models()` (20/11/2026)
- https://benchlm.ai/ — classement global et catégories (19/11/2026)
- https://benchlm.ai/best/web-research · https://benchlm.ai/best/for/research · https://benchlm.ai/best/for/coding
- https://benchlm.ai/api/data/leaderboard?format=json&mode=bench-align-v5&limit=1200
- https://benchlm.ai/models/longcat-2-5 · /flint-40b-a2-5b · /hy4-flash · /reflection-beam · /nemotron-3-5-super-vl-120b-a12b · /mimo-v2-6-flash
- https://llmbenchmark.ai/data/dataset.json (40 258 mesures, 20/11/2026)
- https://github.com/rosspeoples/orpt-bench · https://github.com/PhillipChaffee/big-pickle-swe-atlas

## Utilisation

Je ne peux pas changer **moi-même** le modèle de cette session. Pour utiliser l'un de
ceux-ci, demande-le explicitement, par exemple :

```
utilise opencode/longcat-2.5-free pour chercher les stratégies Legend de Mad Pod Racing
utilise opencode/flint-40b-free pour écrire le bot C++ Gold
```

Sans demande explicite, je continue avec le modèle configuré (`opencode/big-pickle`).
