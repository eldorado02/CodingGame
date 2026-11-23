#!/usr/bin/env bash
# Scaffold a challenge folder from a template.
# Usage: bash hackathon-2026-12-03/templates/new_challenge.sh <pretty_id> [template=board-2p]
# Creates hackathon-2026-12-03/challenges/<pretty_id>/ with:
#   README.md research.md plan.md critique.md statement.md (official, via cg.py)
#   solutions/{python,c,cpp,rust}  tests/{gen_fuzz.py,run.sh}
# Never overwrites an existing folder.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
ID="${1:?pretty_id required}"
TPL="${2:-board-2p}"
SRC="$ROOT/hackathon-2026-12-03/templates/$TPL"
DST="$ROOT/hackathon-2026-12-03/challenges/$ID"
[ -d "$SRC" ] || { echo "unknown template $TPL"; exit 1; }
[ -e "$DST" ] && { echo "$DST already exists"; exit 1; }
mkdir -p "$DST/solutions"
for l in python c cpp rust; do cp -r "$SRC/$l" "$DST/solutions/"; done
cp -r "$SRC/tests" "$DST/tests"
sed -i "s/^name = \"bot\"/name = \"$(echo "$ID" | tr '-' '_')_bot\"/" "$DST/solutions/rust/Cargo.toml"
python3 "$ROOT/bot-programming/tools/cg.py" show "$ID" > "$DST/statement.md"
python3 "$ROOT/bot-programming/tools/cg.py" stub "$ID" >> "$DST/statement.md" || true
DATE=$(date +%Y-%m-%d)
cat > "$DST/README.md" <<EOF
# $ID — guide de reprise

- Énoncé : \`statement.md\` (archivé le $DATE, relire après chaque promotion).
- Template de départ : \`templates/$TPL\` (voir \`bot-programming/ALGO.md\`).
- Ligue actuelle : à remplir après \`python3 bot-programming/tools/status.py --only $ID\`.
- Bot soumis : aucun.
- Adversaires de référence (boss / top) : à remplir (\`cg.py opponents $ID\`).
- Commandes :
  - fuzz : \`bash hackathon-2026-12-03/challenges/$ID/tests/run.sh\`
  - test : \`python3 bot-programming/tools/cg.py test $ID hackathon-2026-12-03/challenges/$ID/solutions/cpp/bot.cpp --language C++ --opponents <id>\`
  - submit : \`python3 bot-programming/tools/cg.py submit $ID hackathon-2026-12-03/challenges/$ID/solutions/cpp/bot.cpp --language C++\`
EOF
cat > "$DST/research.md" <<EOF
# $ID — recherche

## Sources (URL, langage, idée retenue / rejetée et pourquoi)

## Faits du referee / contradictions avec l'énoncé

## Classe de problème (ALGO.md) et approche retenue
EOF
cat > "$DST/plan.md" <<EOF
# $ID — plan

1. Protocole exact (lignes d'init, ordre des champs, lignes de sortie par tour) :
2. Invariant d'état (recalculé / mémorisé) :
3. Algorithme, complexité, budget (nœuds, ms) :
4. Départage déterministe :
5. Matrice de tests (fuzz) :
6. Hypothèse de victoire et signal de réfutation :
EOF
cat > "$DST/critique.md" <<EOF
# $ID — critique et journal des tentatives

Format : constat / hypothèse / sources / action / résultat / leçon.
EOF
echo "scaffolded $DST"
