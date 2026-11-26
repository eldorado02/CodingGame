# start-up — plan

1. Protocole : par tour 11 entiers (id, playerCount, turn, income, cash, devs,
   sellers, managers, features, tests, bugs) puis playerCount lignes
   `id share(‰) reputation`. Sortie `devsToHire sellersToHire managersToHire
   maintenanceDevs competitiveSellers [targetId]`.
2. Invariant : aucun, tout est dans l'entrée.
3. Algorithme : économie à règles. Embauche max (2 par manager, plafond 6 par
   manager), +1 manager par tour, devs jusqu'à 12 puis vendeurs dès 10 features ;
   avant 10 features la moitié des devs en maintenance, ensuite 1 sur 6 développe.
   Réserve : 3 salaires (revenu compté).
4. Départage : rival le plus gros (puis id le plus petit) parmi ceux qu'on
   surpasse en réputation.
5. Fuzz : 200 flux de 1 à 40 tours aléatoires (seuil des 10 features, cash nul
   ou énorme, parts nulles, réputations).
6. Hypothèse : le marché libre (12 %/tour) va au premier vendeur. Réfutation :
   un rival à 80 % avant nous.
