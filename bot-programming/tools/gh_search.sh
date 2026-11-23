#!/usr/bin/env bash
# GitHub + GitLab repository search (no auth needed; GitHub code search needs a token we do not have).
# Usage: bash bot-programming/tools/gh_search.sh "<query>" [n]
# Prints: stars, full_name, language, description, html_url — most stars first.
Q="${1:?query}"; N="${2:-15}"
curl -s -G "https://api.github.com/search/repositories" \
  --data-urlencode "q=$Q" --data-urlencode "sort=stars" --data-urlencode "per_page=$N" |
python3 -c '
import json, sys
try:
    d = json.load(sys.stdin)
except ValueError:
    print("API: empty/invalid answer (anonymous rate limit: 10 searches/min)"); sys.exit(0)
for r in d.get("items", []):
    print("%5d  %-45s %-10s %s  %s" % (r["stargazers_count"], r["full_name"], r.get("language") or "-",
          (r.get("description") or "")[:70], r["html_url"]))
if "message" in d: print("API:", d["message"])
'
echo "--- gitlab"
curl -s -G "https://gitlab.com/api/v4/projects" \
  --data-urlencode "search=$Q" --data-urlencode "order_by=star_count" --data-urlencode "per_page=$N" |
python3 -c '
import json, sys
try:
    d = json.load(sys.stdin)
except ValueError:
    sys.exit(0)
if isinstance(d, dict):
    print("API:", d.get("message") or d.get("error")); sys.exit(0)
for r in d:
    print("%5d  %-45s %s  %s" % (r.get("star_count", 0), r["path_with_namespace"], (r.get("description") or "")[:70], r["web_url"]))
'
