# Brandubh — plan

## Objectif

Obtenir au moins 3 victoires sur 5 contre `jacek` (`6913632`) avec le bot
attaquant (rôle 0), puis porter en 4 langages, fuzz 200 cas identiques et
soumettre en C++.

## Étapes

1. Reproduire exactement le referee dans le prototype Python :
   - génération des coups (trône/coins traversables par les non-rois) ;
   - captures (hostilité du trône, coins, roi sur le trône) ;
   - conditions de victoire (roi capturé, coin, encerclement).
2. Évaluation :
   - BFS d'évasion du roi vers un coin (pièces = murs) ;
   - mobilité du roi, attaquants adjacents, matériel.
3. Alpha-beta 2 plis côté attaquant, 1 pli côté défenseur (limite 50 ms).
4. Test games contre `6913632` ; ajuster un seul paramètre à la fois.
5. Une fois ≥3/5 : ports C, C++, Rust, fuzz différentiel 200 cas, soumission C++.

## Pièges identifiés

- Le bot doit rester en boucle `while True` : CodinGame garde le processus vivant
  entre les tours.
- `sys.stdin.read()` bloque ; lire avec `input()`.
- Ne pas repousser les attaquants loin du roi ( terme de proximité mal signé ).
- Le BFS d'évasion ne doit pas traverser les défenseurs.
