# Critique — Ghost in the Cell

## Avant implémentation

| Risque | Évaluation | Mitigation |
|---|---|---|
| Vider une usine puis la perdre | Élevé | Réserve de 5 sur la source. |
| Sous-estimer une garnison | Élevé | Forces effectives avec troupes en approche. |
| Action unique mal choisie | Moyen | Score global sur tous les couples. |
| Défense tardive | Moyen | Troupes en approche incluses dans `force`. |
| Timeout 50 ms | Faible | Floyd-Warshall en init seulement. |

## Après implémentation

- `tests/run.sh` : 6 cas déterministes × 4 langages → OK.
- Fuzz différentiel : 200 parties aléatoires (`random.seed(17)`) →
  **0 divergence** entre C, C++, Rust et Python.
  - Bugs trouvés :
    - Rust : `factoryCount` et `linkCount` lus sur une seule ligne (ils sont
      sur deux lignes distinctes) → index hors bornes.
    - C/C++/Rust : la force d’une troupe était appliquée pendant la lecture
      des entités ; si une `FACTORY` était lue après une `TROOP` visant la
      même usine, elle écrasait le renfort. Corrigé : stocker les troupes puis
      les appliquer après toutes les usines.
- Test game réel : **victoire 346–5** contre l’adversaire Wood, sans timeout.
- Conclusion : la stratégie « forces effectives + une action globale » suffit
  largement en Wood 3. Les ligues supérieures demanderont plusieurs actions
  par tour, bombs et upgrade de production.

## 2026-11-21 —Strong bot (soumission `41778890`)

Bot précédent : force effective locale, une seule action par tour, scores 0–227
contre le boss Wood 4 `1079501` (0 victoire sur 7). Deux prototypes intermédiaires
(`gic_v2` multi-ordres + prédiction des troupes, `gic_v3` réserve 1 + jusqu'à 5
attaques par usine) n'ont donné que 1 victoire sur 7 et 1 sur 9.

Nouvelle stratégie (inspirée du post-mortem d'Agade) :
- `floyd` + centralité (nombre de plus courts chemins passant par chaque usine) ;
- projection temporelle de chaque usine sur 6 tours : production, troupes
  arrivantes, bombes et suppression de production, combats résolus tour par tour ;
- renforts d'abord (usine la plus précieuse en premier, « spare » = stock −
  troupes déjà envoyées − besoin de défense), puis attaques notées
  `valeur / ((ETA+0,35)² × coût^0,7)` pour les neutres, `valeur / ((ETA+0,35)²×8)`
  pour les ennemies, avec mise à jour de l'état après chaque ordre ;
- bombes tôt sur les usines à forte production que l'ennemi va capturer, puis
  sur les gros stocks ennemis ; jamais deux bombes sur la même cible.

Validation : prototype Python **4 victoires / 5** en vérification indépendante
(boss à 0 dans les victoires), 5/5 annoncés par la recherche initiale, puis
portage 4 langages et fuzz **206/206** identiques. Soumission C++ `41778890`.

### Correctif sortie C++ (2026-11-21)
La première soumission C++timeoutait dès le frame 3 (`$0 has not provided 1 lines
in time`) alors que Python et le fuzz passaient : `std::cout` avec
`sync_with_stdio(false)` ne vide pas son tampon sur `'\n'`. Ajout de
`<< std::flush` et resoumission (`41778911` / `41778912`). Test game C++
Olymbits repassé de `-1` à `7280`.

## 2026-11-21 — Promotion Bronze : nouvelle règle INC

- Promotion de Wood 1 vers Bronze confirmée par `/puzzles/ghost-in-the-cell/progress`.
- Énoncé relu : nouveauté Bronze, `INC factory` sacrifie 10 cyborgs pour
  augmenter durablement la production d'une usine (max 3/tour).
- Le bot validé en Wood 1 n'émet jamais `INC` : il faut ajouter une décision
  d'investissement avant de viser la Ligue Silver.
- Pistes : inciter les usines à production 0/1/2 dès que le stock dépasse un
  seuil et qu'aucune attaque immédiate n'est prioritaire ; ne jamais sacrifier
  sous 10 unités ni sur une usine frontalière menacée.

## 2026-11-21 — Bronze : tentatives INC (toutes rejetées)

Boss Bronze : `Bossaki` (agent `1094665`, score 26.06). Bot Wood 1 conservé.

| variante | politique | résultats vs boss |
|---|---|---|
| v1 | 2 INC/tour, spare ≥ 25, tours ≤ 100, usines prod 0–3 | 3 victoires / 10 |
| v2 | 1 INC/tour, tours ≤ 20, spare ≥ 20 | 2 / 9 |
| v3 | 1 INC/tour, arrière-ligne (distance ennemie max), prod 1–2, spare 15, horizon défense 10, pas d'INC si avance de prod ≥ 2 | 6 / 17 |
| v4a | v3 avec score d'INC pondéré par `value[i]` | 0 / 6 |
| v5 | v3 avec `value = prod² + …` | 1 / 7 |
| v6 | v3 + réserve de 8 unités par usine lors des attaques | 0 / 9 |
| v7 | v3 + une seule cible d'attaque par tour (concentration) | 2 / 5 |

Le rejeu avec seed fixe `729611684` et traces `stderr` montre que nos usines
restent à 3 unités (leur production) parce que leursUnités sont en transit ; ce
n'est donc pas une simple gaspillage de garnison. Le véritable écart vient de la
concentration adverse (22 unités sur une seule usine productrice dès le tour 2)
et de notre dispersion en plusieurs petits MOVE. Les variantes qui réduisent la
dispersion (v6, v7) n'ont pas amélioré le taux de victoire sur ces échantillons.

Décision : ne pas soumettre de nouvelle version pour l'instant ; journaliser et
passer au jeu suivant. Le bot validé Wood 1 (`41778912`) reste en place.
