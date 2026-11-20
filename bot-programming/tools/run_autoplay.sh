#!/bin/sh
# Launch the autonomous driver detached from this session.
# It keeps running if the agent session ends or the model rate-limits.
cd "$(dirname "$0")/.." || exit 1
mkdir -p games/mad-pod-racing/logs
setsid nohup python3 tools/autoplay.py --supervise \
  >> games/mad-pod-racing/logs/autoplay.out 2>&1 < /dev/null &
echo $! > /tmp/opencode/autoplay.pid
echo "autoplay started, pid $(cat /tmp/opencode/autoplay.pid)"
