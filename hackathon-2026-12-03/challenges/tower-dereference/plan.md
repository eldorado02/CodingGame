# tower-dereference — plan

1. Protocole : voir README ; une ligne d'actions séparées par `;` (ou `PASS`), flushée.
2. Invariant : seule la carte et le chemin (cases du canyon sur un plus court chemin entre les colonnes 0 et W-1) sont gardés ; tours et argent relus à chaque tour.
3. Algorithme (glouton, entier) : pour chaque case du chemin, poids `1 + GLUE_BONUS·[couverte par notre colle]` et cadence canon cumulée. Options : construire un canon (valeur = 60·min(5,15)/5 · Σ poids dans le rayon 3), construire une colle (valeur = 3 · Σ cadence des cases non collées dans le rayon 3), améliorer DAMAGE/RANGE/RELOAD d'un canon existant (différence de valeur). Valeur des constructions × 90 % sur le demi-terrain adverse. On achète l'option de meilleur gain/coût (produit en croix) tant qu'elle est abordable et positive, en recalculant le modèle après chaque achat.
4. Départage : `>` strict ; ordre canon, colle, améliorations (ordre d'entrée des tours, propriétés DAMAGE RANGE RELOAD) ; cases en ordre ligne par ligne.
5. Fuzz : cartes aléatoires (canyon gauche-droite + impasses), tours aléatoires des deux joueurs (4 types, niveaux aléatoires, virgule décimale parfois), attaquants ignorés.
6. Hypothèse : couvrir le chemin + colle bat les bots « canons partout ». Réfutation : défaite par manque de cibles simultanées (vagues de 8) → modéliser la capacité de tir.
