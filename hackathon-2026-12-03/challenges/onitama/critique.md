# onitama — critique et journal des tentatives

Format : constat / hypothèse / sources / action / résultat / leçon.

### 2026-11-23 — Onitama — v1 αβ soumise
- Constat : jeu jamais joué ; cartes orientées selon le prochain utilisateur
- Hypothèse : αβ prof. 5–6 avec cartes normalisées dans un repère unique bat le boss
- Sources : énoncé, sonde (orientation), referee eulerscheZahl/onitama, chareca/onitama-ai (αβ public)
- Action : 4 langages, fuzz 200/200 (cartes aléatoires)
- Résultat : 3/3 contre le boss `6797555` ; soumission `41796749`
- Leçon : normaliser toute donnée orientée (cartes, directions) dans le repère du joueur 0 évite les erreurs de signe
