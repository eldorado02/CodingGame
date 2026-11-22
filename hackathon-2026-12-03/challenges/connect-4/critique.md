# Connect 4 — critique

## Avant implémentation

- Le boss public `6797564` est aléatoire dans `config/Boss.java`, mais le vrai
  boss de ligue peut être plus fort : viser une recherche complète plutôt qu'une
  heuristique de surface.
- `STEAL` est gratuit au premier tour du second joueur : le refuser reviendrait
  à laisser l'adversaire jouer deux pions d'affilée.
- Le plateau 7×9 exclut les tables de solveur 7×6 classiques ; 63 cellules et
  100 ms par tour imposent une recherche compacte et déterministe.

## Après implémentation

### Tentatives

- `c4_v1` (alpha-beta itératif avec minuterie mal placée) : **0/5**, timeout au
  tour 0 ; la racine ne consultait l'horloge que tous les 1024 nœuds et
  poursuivait jusqu'à la profondeur 14.
- `c4_v2` (même moteur, délai 85 ms, vérification à chaque coup racine) :
  **4/5** contre `6797564`.
- `c4_fixed` (profondeur fixe 6) : **3/5**.
- `c4_fixed7` (profondeur fixe 7) : **0/5**.
- `c4_center` (bonus center-top `±2` au lieu du bonus de colonne) : **4/5**
  puis **5/5** sur une nouvelle série ; la version du dépôt a été reverifiée
  **5/5**.
- `c4_full` (ordre des coups corrigé) : non mesuré, quota épuisé ; non retenu
  pour éviter de remplacer une version validée par une variante non validée.

## Leçons

- Une minuterie approximative peut être pire qu'une profondeur fixe : dans ce
  challenge, la profondeur 6 déterministe est à la fois plus sûre et plus forte
  que la recherche itérative avec budget de 85 ms.
- Les séries de 5 test games sont bruitées (4/5 puis 5/5 pour le même binaire)
  : rejouer la version exacte du dépôt avant de soumettre.
- Le fuzz différentiel 4 langages a validé la lecture du protocole et la table
  de transpositions ; aucun bug de port n'a été détecté.
