# minishogi — recherche

## Sources (URL, langage, idée retenue / rejetée et pourquoi)

## Faits du referee / contradictions avec l'énoncé

## Classe de problème (ALGO.md) et approche retenue


## Deep search opencode (hy4-flash-free) — 2026-11-23
Rapport complet : `research-opencode-20261123-205959.md` (non vérifié ligne à ligne ; à valider par test game).

Recommandation extraite :

> **Hypothèse centrale (testable) :** remplacer un alpha-beta/MCTS naïf par un **expectiminimax à budget temps avec modèle de dé exact**, puis n'ajouter que des termes d'évaluation mesurables. Le jeu est particulièrement adapté : le hasard n'a que **6 issues uniformes**, et le referee fournit déjà la liste des coups légaux au root (économie de génération).
