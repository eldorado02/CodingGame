# Goal

Shobu is played on four small boards at once. Every turn you move two of your stones: a quiet move at home, then the same move somewhere else, where it can shove an enemy stone off the edge. **Push all four of your opponent's stones off any one board to win.**

*Shobu was designed by Manolis Vranas and Jamie Sajdak of Seesaw Games, and published by Smirk & Laughter in 2019. This adaptation adds a two-round match, a pie rule and a turn limit.*

Source code of the referee and viewer: [github.com/Waffle3z/CG-Shobu](https://github.com/Waffle3z/CG-Shobu)

# Rules

### The boards

There are four 4×4 boards, given to you as one 8×8 grid of `8` input lines: `x` is the column (`0` to `7`, left to right) and `y` the row (`0` to `7`, top to bottom). The start position, exactly as your program reads it:

|  | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |  |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0 | w | w | w | w | w | w | w | w | white's home boards |
| 1 | . | . | . | . | . | . | . | . |  |
| 2 | . | . | . | . | . | . | . | . |  |
| 3 | b | b | b | b | b | b | b | b |  |
| 4 | w | w | w | w | w | w | w | w | black's home boards |
| 5 | . | . | . | . | . | . | . | . |  |
| 6 | . | . | . | . | . | . | . | . |  |
| 7 | b | b | b | b | b | b | b | b |  |
|  | dark boards | light boards |  |  |  |  |  |  |  |

- The left column of boards (`x` < 4) is **dark**, the right column (`x` ≥ 4) is **light**.
- **Black**'s home boards are the two bottom ones (`y` ≥ 4), **white**'s the two top ones (`y` < 4).
- Each player starts with four stones on every board: black on row 3 of each board (`y` = 3 and 7), white on row 0 (`y` = 0 and 4).
- Stones never leave their board, except by being pushed off it.

### A turn: two moves

**Black moves first.** On your turn you make a **passive** move and then an **aggressive** move with the same displacement.

1. **Passive move.** Pick one of your stones on one of your two *home* boards and move it `1` or `2` squares in any of the `8` directions (orthogonal or diagonal). Every square it passes over or lands on must be empty: a passive move never touches another stone.
2. **Aggressive move.** Then move one of your stones on a board of the *other color* (dark ↔ light), on your side or your opponent's, by exactly the same direction and distance. This stone may **push**:

   - it may push **one** enemy stone that is in its path or on its landing square;
   - the pushed stone ends up one square past where your stone lands; if that square is off the board, the stone is **removed** from the game;
   - your stone may not pass over or land on one of your own stones, push two stones, or push a stone onto another stone.

Both moves are made in one turn and given as one **action**. An action is legal only when both of its moves are legal; the list of legal actions is in your input.

### End of a round

- You win the round as soon as one board has **no stones of your opponent**.
- A player with no legal action loses the round.
- A round ends after `110` turns in total (a turn spent on `SWAP` does not count). If the `110`th turn empties an enemy board, its player wins; otherwise the round is a draw.

### Pie rule

Moving first can be an advantage, and some first actions are much stronger than others. So in every round, on white's **first** turn, white may answer `SWAP` instead of playing two moves: the players exchange colors for the rest of the round.

- The player who swapped now plays **black**, with black's first action already on the board.
- The stones do not change: they stay where they are and keep their letters in the input. The player who swapped now moves the `b` stones and receives `myColor` = `0`.
- The other player now plays **white** and plays next.
- So black's best first action is a balanced one: a first action that is too strong just gets taken by the opponent.

`SWAP` is offered only on that one turn, once per round: when it is available it is the last line of your list of legal actions. The player who becomes white after a swap cannot swap back. Anywhere else `SWAP` is an invalid action. A turn spent on a swap does not count towards the `110`-turn limit.

### Match

A match is **two rounds**: you start as black (moving first) in one round and as white in the other, and the pie rule applies in both. A won round is worth `2` points, a draw `1`, whichever color you ended up playing. The player with more points wins the match; equal points are a draw.

A **timeout or an invalid action loses the whole match**, including any round you already won.

### Victory Conditions

- You win a **round** when one of the boards has no stones of your opponent left, or when your opponent has no legal action.
- You win the **match** with more points than your opponent after the two rounds (`2` per round won, `1` per draw).

### Defeat Conditions

- You lose a round when your opponent wins it.
- You lose the whole match if your program does not respond in time, or outputs anything that is not one of the listed legal actions.

A round reaching the `110`-turn limit is a draw, and so is a match with equal points.

# Example

- **First turn.** Black plays `3 7 4 3 0 -2`. Passive: the stone on (3,7), on black's dark home board, moves two squares up to (3,5) over empty squares. Aggressive: the stone on (4,3), on white's light board (light, so the other color), moves two squares up to (4,1). Nothing is pushed: white's stone on (4,0) is beyond the landing square.
- **Push off.** Later, with that black stone still on (4,1) and the white stone on (4,0), an aggressive move of `0 -1` (one up) lands on (4,0) and pushes the white stone to (4,-1): off the board, so it is removed. Here black plays `3 7 4 1 0 -1`:
  ![Passive move 3 7 to 3 6 on black's dark home board, then the aggressive move 4 1 to 4 0 on white's light board pushes the white stone off](https://cdn-games.codingame.com/community/3564009-1792211741326/2852be045d350ac0381b90a43df357a3eb62a4f01671fcd3c28d47ed89ce6981.png)
- **Illegal push.** A black stone on (4,3) cannot move `0 -1` if white stones stand on both (4,2) and (4,1): that would push two stones in a row.

The input white receives in round `1` after black's first action above, `3 7 4 3 0 -2` (`210` legal actions, then `SWAP` because it is white's first turn):

1 1
wwwwwwww
....b...
........
bbbb.bbb
wwwwwwww
...b....
........
bbb.bbbb
3 7 4 3 0 -2
211
0 0 4 0 1 1
0 0 5 0 1 1
0 0 6 0 1 1
(... 207 more legal actions ...)
SWAP

# Game Input/Output

### Input for one game turn

Line 1: `round` (`1` or `2`) and `myColor`, the color you play this turn (`0` black, `1` white). It can change during a round, after a swap.
Next `8` lines: the rows of the grid, top to bottom, `8` characters each: `.` empty, `b` black stone, `w` white stone.
Next line: your opponent's last action as `px` `py` `ax` `ay` `dx` `dy`, `SWAP` if your opponent just swapped, or `-` at the start of a round.
Next line: `actionCount`, the number of legal actions.
Next `actionCount` lines: a legal action `px` `py` `ax` `ay` `dx` `dy`. On white's first turn of a round the last of these lines is `SWAP`.

### Output for one game turn

A single line: `px` `py` `ax` `ay` `dx` `dy` — move your stone at (`px`,`py`) by (`dx`,`dy`) as the passive move, then your stone at (`ax`,`ay`) by the same (`dx`,`dy`) as the aggressive move. (`dx`,`dy`) is one or two steps in one of 8 directions, for example `0 -2` (two up) or `-1 1` (one diagonally down-left). Anything after the six numbers is shown as a message (its first `40` characters).
Or, on white's first turn of a round only: `SWAP` (optionally followed by a message).

### Constraints

Response time for your first turn of the match ≤ `1000` ms (only once, not every round)
Response time for one turn ≤ `100` ms
`actionCount` ≤ `1025`

# Leagues

Each league has a boss to beat:

- **Wood 4**: a greedy bot. It takes a winning action if it sees one, otherwise the action that removes the most material, and never looks at your reply.
- **Wood 3**: Monte Carlo tree search with short random playouts. It also recognizes positions it has fully solved.
- **Wood 2**: a plain three-turn-deep minimax with alpha-beta pruning, counting stones.

The bosses never swap. **Wood 1** is the top league.
