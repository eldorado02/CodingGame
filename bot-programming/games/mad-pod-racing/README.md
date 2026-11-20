# Mad Pod Racing

- id: `mad-pod-racing`
- page: https://www.codingame.com/multiplayer/bot-programming/mad-pod-racing
- IDE: https://www.codingame.com/ide/puzzle/mad-pod-racing
- Current league: **Bronze** (entered 2026-11-20T16:23Z, promoted by `41768678`).
- Current submission: `41768678` (drift-compensated heuristic with BOOST),
  re-ranking in Bronze: score climbed from 12.5 at 10% to ~32 in Wood.

## Statement changes by league

| League | Change | Input / output |
|---|---|---|
| Wood 3 → Wood 2 | none relevant | 6 ints + opponent `(x,y)` |
| Wood 2 → Wood 1 | BOOST allowed (once per race, acceleration 650) | thrust may be `BOOST` |
| Wood 1 → Bronze | pods collide (force field radius 400), extra maps | unchanged |

Bronze statement re-read on promotion: still one pod per player, 6 input ints,
opponent position on line 2, one output line `x y thrust|BOOST`, 75 ms/turn.

## Physics used by the bot

- Map 16000x9000, checkpoint radius 600, 100-turn checkpoint timeout.
- One turn: rotate at most 18° toward the target, add thrust to velocity,
  move, multiply velocity by 0.85, round position and truncate velocity.
- BOOST is a one-off acceleration of 650.
- In Wood/Bronze, only the next checkpoint is known; heading is recovered from
  `nextCheckpointAngle`, speed from the position delta of the previous frame.

## Bot

`bot.py` (Python 3), submitted:

- target = `checkpoint - 5 * last_displacement` (anti-drift; literature uses 3,
  local tuning on 40 maps preferred 5);
- `thrust = 100 * clamp(1 - angle/90) * clamp(distance / (3 * 600))`;
- BOOST after 15 turns, only when `angle < 5°`, `distance > 3500`, `speed > 200`.

`bot_gold.cpp` is a prepared (compiled, **not yet submitted**) rolling-horizon
genetic planner for the Gold/Legend protocol: 4-turn horizon, population 6,
warm start from the shifted previous best, biased mutations, exact simulator,
`30000 * checkpoints - distance` evaluation, racer/blocker split, shared boost.
Do not submit it before re-reading the Gold statement.

`sim.py` is a local physics simulator used to tune constants offline.
`analyze.py` summarizes saved CodinGame test logs.

## Online test results (vs league bosses)

- Wood 2, Boss 2: 3/3 wins with the exact-angle chaser; drift-compensated
  variant slightly faster on identical seeds.
- Wood 1, Boss 3: BOOST on turn 1 gave 1/3 (wasting the 650 acceleration at
  zero speed); delaying to 15 turns gave 2/3, then tuned constants
  (`drift=5`, `dist=3`) gave 3/3 on the same maps and 4/4 on fresh maps.
- Bronze, Boss 4: **4/4 wins** with the submitted `bot.py` (including a
  6-checkpoint map finished in 416 turns).
- Foreground simulation on 40 random maps: average 214.7 turns with
  `drift=5, dist=3, boost delay=15`.

## Web research: what actually works

Sources read:

- Tanguy Courreges, Bronze→Legend walkthrough:
  https://tcourreges.github.io/coders-strike-back
- Gold 3rd-place C++ solution: https://github.com/zeroblitz36/MadPodRacing
- Physics/collision guide (Magus): https://files.magusgeek.com/csb/csb_en.html
- Genetic-algorithm write-up by contest winner Jeff06:
  https://www.codingame.com/blog/genetic-algorithms-coders-strike-back-game/
- Evolutionary-trajectory write-up by pb4608:
  https://www.codingame.com/blog/evolutionary-trajectory-optimization/
- Inoryy starter with exact simulator and search:
  https://github.com/Inoryy/csb-ai-starter
- Legend post-mortem (`cp - 3v`): https://www.alexkorablev.com/coders-strike-back
- Curated index of top competitor write-ups: https://yduf.github.io/CG-coder-strike-back/

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
   - collision rebound has a **minimum** impulse of 120 and velocities are
     truncated, not floored.

Next step: re-read the statement after each promotion; switch to
`bot_gold.cpp` only once the Gold protocol is confirmed.
