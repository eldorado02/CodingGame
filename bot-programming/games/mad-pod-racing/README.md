# Mad Pod Racing

- id: `mad-pod-racing`
- page: https://www.codingame.com/multiplayer/bot-programming/mad-pod-racing
- IDE: https://www.codingame.com/ide/puzzle/mad-pod-racing
- 1v1 race, map 16000x9000, checkpoint radius 600, thrust 0-100, one BOOST per race.
- Physics: rotate max 18°/turn toward the target, accelerate thrust/10, friction 0.85,
  heading is recoverable from `nextCheckpointAngle`.
- Current league: Bronze — score 6.71 vs league boss 13.32 (rank 91).
- The IDE still holds the default C++ template, so `bot.py` is the only real bot.

## Bot

`bot.py` (Python 3) — checkpoint chaser:
- records the checkpoints in order and, once the loop is known, aims slightly past
  the checkpoint along the next leg to keep speed;
- thrust by absolute angle (100 / 70 / 35 / 12 / 0);
- uses BOOST on a straight with speed > 560 and distance > 3500.

## Test results

- `logs/test-20261120-175417.log`: win vs Boss 4 (2-1), 288 turns.
- Weakness: cornering at full speed (turn radius ~2100 at max speed vs 600 checkpoint
  radius), so legs are slow. Next step: lookahead (a few turns of simulation) to pick
  the entry point and thrust, plus a braking rule before tight corners.
