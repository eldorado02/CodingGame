# Critique — Code Royale

## Avant implémentation

| Risque | Évaluation | Mitigation |
|---|---|---|
| Choisir ARCHER (n'attaque pas la reine) | Élevé | KNIGHT uniquement. |
| Reine ennemie détruit le barracks | Moyen | Cible à > 150 + plusieurs barracks. |
|trained barracks non détecté | Moyen | `param1 == 0`, trié. |
| TRAIN invalide (or insuffisant) | Moyen | Comptabilité locale du gold. |
| Timeout 50 ms | Faible | Parcours linéaire. |
| Divergence entre langages | Moyen | Score entier, fuzz. |

## Après implémentation

- `tests/run.sh` : 7 cas déterministes × 4 langages → OK.
- Fuzz différentiel : 200 configurations aléatoires (`random.seed(37)`) →
  **0 divergence** entre C, C++, Rust et Python.
  - Bug trouvé : Rust oubliait le filtre `distance < 250` sur la destruction
    des barracks ennemis, et choisissait un `MOVE` même pour une cible lointaine.
    Corrigé.
- Physique validée contre le referee officiel : KNIGHT = 4 unités / 80 or /
  5 tours de build / vitesse 100 / 30 PV / 1 dégât par tour à la reine ;
  ARCHER n'attaque jamais la reine (donc écarté).
- Test games réels (Wood 3) :
  - adversaire par défaut : **victoire en 98 tours**, scores `0=26`, `1=0` ;
  - **Duchess Boss** (boss Wood 3) : **victoire en 94 tours**, scores `0=24`,
    `1=0`.
  - Aucune commande invalide ni timeout ; la reine ennemie meurt dans les deux cas.
- Conclusion : le rush KNIGHT suffit en Wood 3. Les ligues supérieures (TOWER,
  MINE, GIANT, or réduit) demanderont une vraie gestion économique et défensive."})
