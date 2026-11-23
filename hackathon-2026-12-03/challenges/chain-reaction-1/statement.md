Chain Reaction is a two-player strategy game. Place orbs on your cells, push them to critical mass and watch the explosions cascade across the board — a single well-placed move can flip the entire game.

Many thanks to [eulerscheZahl](https://www.codingame.com/profile/8374201b6f1d19eb99d61c80351465b65150051) for his help.

# Rules

The game is played on a `6×6` board. Rows are numbered `1-6` bottom to top, columns `a-f` left to right.

The board starts empty.

**Placing an orb**

- On your turn, choose any cell that is either **empty** or **already belongs to you**, and add one orb to it.
- You cannot play on a cell owned by your opponent.

**Critical mass & explosions**

- Each cell's critical mass equals its number of orthogonal neighbours: `2` for corners, `3` for edges, `4` for interior cells.
- When a cell reaches its critical mass it **explodes**: the cell empties, each orthogonal neighbour receives 1 orb and is captured by you, including its orbs.
- A neighbour that then reaches its critical mass also explodes — producing a **chain reaction**.

# Expert Rules

The game ends after `200` turns if no player has won. In that case the game is a draw.

**Understanding the viewer**: the animation breaks down each turn into frames. The first frame shows the orb being placed. If any cells have reached critical mass, they explode in the next frame: each exploding cell empties (becoming neutral) and sends one orb to each orthogonal neighbour — the current player captures those neighbours, even if they belonged to the opponent. Each subsequent frame repeats the process until there are no more critical cells or the opponent has no cells left.

### Victory Conditions

- Your opponent has no cells left on the board (after each player has made at least one move).

### Loss Conditions

- You have no cells left on the board (after each player has made at least one move).
- You play on a cell that belongs to your opponent.
- You do not respond in time or output an invalid move.

# Game Protocol

### Initial input

First line: `color`: your color — `r` (red, first player) or `b` (blue, second player).

### Input for one game turn

Line 1: `lastMove` — your opponent's last move in chess notation (e.g. `c4`), or `null` if you are the first player on the first turn.
Next `6` lines: a `12`-character string `cells`, encoding the full row with `2` characters per cell, row `6` first. Each 2-character token is `..` (empty), `r1`–`r3` (red cell with 1–3 orbs), or `b1`–`b3` (blue cell with 1–3 orbs). Cell at column `c` is at characters `2c` and `2c+1`.

### Output for one game turn

A single line: the cell in chess notation, e.g. `c4` — column letter (`a`–`f`, left to right) followed by row number (`1` at bottom, `6` at top).
You may append a message after the move, e.g. `c4 message`.
You may also send `random` to let the referee play a random valid move on your behalf.

### Constraints

Response time first turn is ≤ `1000` ms.
Response time per turn is ≤ `100` ms.
{
  "stub_generator": "read color:word(1)\n\ngameloop\nread lastMove:word(4)\nloop 6 read cells:string(12)\nwrite random\n\nINPUT\ncolor: your color (r=red, b=blue)\nlastMove: opponent's last move in chess notation (e.g. \"c4\"), or \"null\" if you go first\ncells: 12-char string, 2 chars per cell \u2014 \"..\" (empty), \"r1\"-\"r3\" (red), \"b1\"-\"b3\" (blue)\n\nOUTPUT\ne.g. c4 (place an orb on column c, row 4)\nor: random (let the referee pick a random valid move)\n",
  "syntax_doc": "https://www.codingame.com/api/v1/docs/stub-generator",
  "limits": {
    "requests": {
      "name": "requests_1m",
      "limit": 120,
      "used": 2,
      "remaining": 118,
      "window_seconds": 60,
      "resets_in_seconds": 60
    }
  }
}
