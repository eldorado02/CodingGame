-- languages: Bash, C, C#, C++, Clojure, D, Dart, F#, Go, Groovy, Haskell, Java, Javascript, Kotlin, Lua, ObjectiveC, OCaml, Pascal, Perl, PHP, Python3, Ruby, Rust, Scala, Swift, TypeScript, VB.NET

> Source: `python3 bot-programming/tools/cg.py show ataxx` (official CodinGame statement, fetched 2026-11-21).

This is a port of the arcade game [Ataxx](https://en.wikipedia.org/wiki/Ataxx).
Contribution originally made by [struct](https://www.codingame.com/profile/dfb886f869fc14dc31fc9191a1b87a608740451).

# Rules

The game is played on a 7x7 board.
The board is chosen from one of the 596 possible [configurations](https://www.pressibus.org/ataxx/gen/gbblocs.html).
The game goes on until both players can no longer move or a player has no more pieces.

**Board coordinates**

- Columns are labeled `a` through `g` from left to right.
- Rows are labeled `1` throught `7` from bottom to top.
- Coordinates are written as a column followed by a row. For example, `a1` is the bottom-left corner of the board and `g7` is the top-right corner.

**Legal actions**

- Player `0` always goes first.
- Players take turns performing one of the two available actions, Spawn and Jump.
- Spawn: A player may spawn a piece on an empty square adjacent (vertically, horizonally or diagonally) to one of their existing pieces.
- Jump: A player may move one of their pieces to an empty square that is two squares away in any direction (vertically, horizontally or diagonally).
- After a Spawn or Jump action, all adjacent opponent pieces are converted to the color of the player who moved.

**Output**

- A player must output either:

  - the coordinates of the square for a Spawn (e.g. b7), or
  - the coordinates of the source and the destination squares for a Jump (e.g. a7c7).
- If the coordinates are outside the board, the game ends and the other player wins.
- If the coordinates are inside the board but the target square is occupied or a wall, the game ends and the other player wins.

# Expert Rules

The game ends in a draw if it reaches 300 turns, or if the same position occurs three times (the same board configuration and the same player to move).

### Victory Conditions

- Capture all of your opponent's pieces.
- Have more pieces than your opponent when the game ends.

### Loss Conditions

- You have no pieces.
- You do not respond in time or you output an unrecognized command.

# Game Protocol

### Initial input

First line: `id`: your player ID (`0` or `1`).

### Input for one game turn

`7` lines: A string `line` of 7 characters representing one horizontal row of the grid, from bottom to top. `.`: empty square, `0`: player 0's piece, `1`: player 1's piece, `#`: wall.
Next line: A string `lastAction` for the last action made by the opponent. `null` if it is the first turn, `pass` if the opponent passed. Multiple actions are separated by `;`.
Next line: An integer `actionsCount` for the number of legal actions for this turn.
Next `actionsCount` lines: A string `action` representing a legal action.

### Output for one game turn

A single line containing either:

- the coordinates where you want to jump from and to, e.g. `a7c7`, or
- the coordinates where you want to spawn a piece, e.g. `b7`, or
- `random`.

You can also print a message after the action, e.g. `a7c7 message`.

### Constraints

Response time first turn is ≤ `1000` ms.
Response time per turn is ≤ `100` ms.
