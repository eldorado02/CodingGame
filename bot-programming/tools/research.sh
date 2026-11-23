#!/usr/bin/env bash
# Deep-search sub-agent via OpenCode (free models only, fallback chain).
# Usage: bash bot-programming/tools/research.sh <pretty_id> "<question>" [out_file]
# Writes the agent's report to out_file (default: the challenge's
# research-opencode-<timestamp>.md) and prints its path.
# The agent may browse (webfetch, GitHub API) but must NOT edit code.
set -uo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
ID="${1:?pretty_id}"
Q="${2:?question}"
DIR="$ROOT/hackathon-2026-12-03/challenges/$ID"
mkdir -p "$DIR"
OUT="${3:-$DIR/research-opencode-$(date +%Y%m%d-%H%M%S).md}"
CHAIN=(deepseek-v4.1-flash-free hy4-flash-free nemotron-3.5-super-vl-free)
PROMPT="Tu es un agent de RECHERCHE pour le jeu CodinGame bot-programming '$ID'.
Interdit : modifier ou créer des fichiers de code, lancer des builds, lire ~/.config/codingame.
Autorisé : webfetch, API GitHub (https://api.github.com/search/code?q=..., repositories?q=...),
raw.githubusercontent.com, arXiv, Semantic Scholar, post-mortems CodinGame/forum.
Ordre des sources : referee officiel (souvent github.com/CodinGame/<jeu> ou eulerscheZahl/RefereeCollection)
> code de bots publics > post-mortems > articles.
Question : $Q
Réponds en Markdown, en français, avec : 1) faits du referee (constantes, ordre de résolution,
départages) avec URL exacte ; 2) stratégies des meilleurs joueurs (URL + idée + pourquoi ça marche) ;
3) recommandation concrète et testable pour battre le boss de la ligue ; 4) liste de toutes les URL lues.
Marque [NON VÉRIFIÉ] tout ce que tu n'as pas lu dans une source."
for M in "${CHAIN[@]}"; do
  echo "research: $M" >&2
  if timeout 900 opencode run -m "opencode/$M" --archived --auto "$PROMPT" >"$OUT.tmp" 2>&1 \
     && ! grep -qiE 'usage limit|rate limit|quota|insufficient' "$OUT.tmp" \
     && [ "$(wc -c <"$OUT.tmp")" -gt 400 ]; then
    { echo "<!-- opencode model: $M, $(date -Is) -->"; sed 's/\x1b\[[0-9;]*m//g' "$OUT.tmp"; } >"$OUT"
    rm -f "$OUT.tmp"
    echo "$OUT"
    exit 0
  fi
  echo "research: $M failed, fallback" >&2
done
rm -f "$OUT.tmp"
echo "research: all models failed — continue research manually" >&2
exit 1
