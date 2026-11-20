#!/bin/sh
# Lance le superviseur du hackathon detached de cette session.
# Il survit a la fin de la session agent, aux quotas de tokens et aux redemarrages.
cd "$(dirname "$0")/.." || exit 1
mkdir -p logs /tmp/opencode
setsid nohup python3 tools/supervise.py --supervise \
  >> logs/supervisor.out 2>&1 < /dev/null &
echo $! > /tmp/opencode/hackathon-supervisor.pid
echo "hackathon supervisor started, pid $(cat /tmp/opencode/hackathon-supervisor.pid)"
