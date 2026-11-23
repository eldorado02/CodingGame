---
name: cg-research-opencode
description: Run deep-search research for a CodinGame game through OpenCode free models (deepseek-v4.1-flash-free -> hy4-flash-free -> nemotron-3.5-super-vl-free) instead of Claude subagents, then verify and merge the findings into research.md. Use whenever research on referee rules, top-player strategies or papers is needed in this repo.
---

# Research through OpenCode

The user requires research sub-agents to be OpenCode, never Claude Agent
subagents, and never coding agents.

## Run
```bash
bash bot-programming/tools/research.sh <pretty_id> "<question>"   # prints report path
```
- Long questions: launch with Bash `run_in_background: true` and keep coding.
- Fallback chain is automatic; if every model fails, research manually with
  WebSearch / WebFetch / GitHub API (`gh api search/code -f q=...`).
- `longcat-2.5-free` is not in `opencode models` (checked 2026-11-23).

## Good questions (one per run)
- "Trouve le referee officiel de <jeu> et donne les constantes et l'ordre de
  résolution d'un tour, avec URL."
- "Quelles stratégies utilisent les bots publics qui ont atteint <ligue> ?"
- "Comment battre un boss qui fait <comportement observé dans le log> ?"

## Verify before use
- Every referee fact must be read in the source by you (WebFetch the raw URL)
  or confirmed by a test-game log. Unverified claims stay tagged
  `[NON VÉRIFIÉ]` in `research.md`.
- Copy all URLs into the challenge `research.md` and note the model used.
- Strategies from public repos are hypotheses: validate with test games.
