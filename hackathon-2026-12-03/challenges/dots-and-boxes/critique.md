# dots-and-boxes — critique et journal des tentatives

Format : constat / hypothèse / sources / action / résultat / leçon.

### 2026-11-23 — Dots and Boxes — v1 αβ soumise
- Constat : jeu jamais joué ; état complet fourni à chaque tour
- Hypothèse : αβ avec tour supplémentaire à la fermeture d'une boîte résout le 2×2 de Wood 2
- Sources : énoncé, loicpetit/codingame-dots-and-boxes (gh_search.sh)
- Action : 4 langages, fuzz 200/200 (tailles 2–5)
- Résultat : 4/4 (4–0) contre le boss `6797561` ; soumission `41796861`
- Leçon : un tour supplémentaire se modélise sans changer le signe dans le negamax
