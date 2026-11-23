#!/usr/bin/env bash
# Push main with retries (DNS / network blips). Usage: bash bot-programming/tools/push.sh
for i in $(seq 1 20); do
  git push -q origin main 2>/dev/null && { echo "pushed $(git log --oneline -1)"; exit 0; }
  sleep 15
done
echo "push failed after retries" >&2
exit 1
