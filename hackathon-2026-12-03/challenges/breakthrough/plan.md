# breakthrough — plan (v3)

1. **Protocole** : ligne 1 = coup adverse ou `None` ; ligne 2 = N ; N lignes = coups légaux (`a2a3`). Pas d'init. Sortie : 1 ligne. Couleur : `None` au 1er tour → Blanc (rangs 1–2, monte), sinon Noir.
2. **État** : bitboards `w`, `b` (bit = rang*8 + colonne) tenus à jour avec le coup adverse puis notre coup. On ne joue qu'un coup présent dans la liste fournie (repli : premier coup listé).
3. **Algorithme** : negamax αβ, iterative deepening, budget nœuds (600k au tour 1, 60k ensuite ; garde-temps 700/75 ms). Ordre : coup gagnant (rang but) > capture > ordre de génération ; PV d'abord à la racine.
   - Règle des coureurs à chaque nœud non terminal : le camp au trait a une pièce à 1 pas du but → gain à ply+1 ; l'adversaire en a ≥2, ou 1 non capturable → perte à ply+2.
   - Éval (camp au trait) : 1000/pièce + avancement `ADV={0,4,10,20,36,60,110}` + 12 par pièce défendue + 18 par pièce sur la ligne de fond.
4. **Départage** : premier trouvé (`>` strict), tri par insertion stable, génération par case croissante puis colonne −1, 0, +1.
5. **Fuzz** : flux multi-tours auto-générés (réponses = référence Python, adversaire aléatoire seedé avec captures favorisées), listes de coups mélangées, 1–30 tours, Blanc/Noir.
6. **Hypothèse de victoire** : les bots de milieu de tableau perdent sur les coureurs non vus à prof. 4 ; la règle exacte gagne 2 plis. Réfutation : défaite par percée adverse visible dans le log (coureur en rang 3/6).
