# Mad Pod Racing

- id: `mad-pod-racing`
- page: https://www.codingame.com/multiplayer/bot-programming/mad-pod-racing
- IDE: https://www.codingame.com/ide/puzzle/mad-pod-racing
- Current league: **Wood 1** (promoted from Wood 2 with submission `41768447`).
- Current submission: `41768678` (drift-compensated bot with BOOST).

## Physics used by the bot

- Map 16000x9000, checkpoint radius 600, 100-turn checkpoint timeout.
- One turn: rotate at most 18° toward the target, add thrust to velocity,
  move, multiply velocity by 0.85, round position and truncate velocity.
- BOOST is a one-off acceleration of 650 (shared by both pods from Gold on).
- In Wood, only the next checkpoint is known; heading is recovered from
  `nextCheckpointAngle`, speed from the position delta of the previous frame.

## Bot

`bot.py` (Python 3):

- target = `checkpoint - 5 * last_displacement` (anti-drift; the literature
  commonly uses 3, local tuning on 40 maps preferred 5 here);
- `thrust = 100 * clamp(1 - angle/90) * clamp(distance / (3 * 600))`;
- BOOST after 15 turns, only when `angle < 5°`, `distance > 3500`, `speed > 200`.

`sim.py` is a local physics simulator used to tune constants offline.
`analyze.py` summarizes saved CodinGame test logs.

## Test results (online, vs league bosses)

- First heuristic `bot.py`: 3/3 wins vs Boss 2 (9 legs each).
- Drift-corrected heuristic without BOOST: 3/3 wins vs Boss 2, and slightly
  faster than the first heuristic on identical seeds.
- BOOST on turn 1: 1/3 vs Boss 3 — wasting the fixed 650 acceleration at zero
  speed is bad; waiting 15 turns is much better.
- Final constants (`drift=5`, `dist=3`, boost delay 15): 3/3 wins on the same
  three maps as earlier variants, then 4/4 wins on fresh random maps vs Boss 3.

## Web research: what actually works

Sources read:

- Tanguy Courreges, Bronze→Legend walkthrough:
  https://tcourreges.github.io/coders-strike-back
- Gold 3rd-place C++ solution:
  https://github.com/zeroblitz36/MadPodRacing
- Physics/collision guide (Magus):
  https://files.magusgeek.com/csb/csb_en.html
- Genetic-algorithm write-up by the contest winner Jeff06:
  https://www.codingame.com/blog/genetic-algorithms-coders-strike-back-game/
- Evolutionary-trajectory write-up by pb4608:
  https://www.codingame.com/blog/evolutionary-trajectory-optimization/
- Inoryy starter with exact simulator and search:
  https://github.com/Inoryy/csb-ai-starter

Confirmed strategy by league:

1. **Wood/Bronze/Silver**: aim at `cp - h * velocity` (h around 3-5), taper
   thrust with misalignment and distance, BOOST on a long aligned stretch.
   Collisions/shield matter in Silver but can mostly be ignored while racing.
2. **Gold/Legend**: the protocol changes (all checkpoints known from the start,
   two pods, absolute angle, velocity given directly). Strong bots rewrite for
   an exact simulator plus rolling-horizon stochastic search:
   - horizon 4-6 turns, population 6-10, always warm-start from the previous
     best plan shifted by one turn;
   - mutate one move at a time, biasing rotations to -18/0/+18 and thrust to
     0/max, with rare shield/boost flips;
   - evaluate `30000 * checkpoints_passed - distance_to_checkpoint`, plus a
     racer/blocker split in Gold;
   - spend part of the budget predicting or evolving the opponent;
   - collision rebound has a minimum impulse of 120 and velocities are
     truncated, not floored.

Next step: re-read the statement after each promotion and rewrite the bot for
the Gold protocol when reached.
