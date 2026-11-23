## The Goal

Tic-tac-toe is a turn-based game, where the objective is to get three in a row, but in this game, player who have greater count of three in a row(vertically, horizontally or diagonally), when the 10x10 board is full,wins the game.

## Rules

• The game is played on a 10x10 grid. You must output the coordinate of the cell you want to mark. The player who have greater count of three in a row(vertically, horizontally or diagonally), when the 10x10 board is full, wins the game.

• During a game, each player plays two matches with reversed colors in order to avoid the first player advantage and keep the game balanced. The final score is the sum of the points scored by each player during the two matches.

### Victory Conditions

- The winner is the player with the most **3 in a row**.

## Game Input

### Input for one game turn

Line 1: 2 space separated integers `opponentRow` and `opponentCol`, the opponent's last action (`-1` `-1` for the first turn).
Line 2: the number of valid actions for this turn, `validActionCount`.
Next `validActionCount` lines: 2 space separated integers `row` and `col`, the coordinates you're allowed to play at.

### Output for one game turn

Line 1: 2 space separated integers `row` and `col`.

### Constraints

Response time per turn ≤ `100`ms

### Source code

The game source code is available at: https://github.com/RezaSi/counting-tic-tac-toe.
{
  "stub_generator": "gameloop\nread opponentRow:int opponentCol:int\nread validActionCount:int\nloop validActionCount read row:int col:int\nwrite 0 0\n\nSTATEMENT\nGet greater count of three in a row!\n\nINPUT\nopponentRow: The coordinates of your opponent's last move\nvalidActionCount: the number of possible actions for your next move\nrow: The coordinates of a possible next move\n\nOUTPUT\n<row> <column>",
  "syntax_doc": "https://www.codingame.com/api/v1/docs/stub-generator",
  "limits": {
    "requests": {
      "name": "requests_1m",
      "limit": 120,
      "used": 28,
      "remaining": 92,
      "window_seconds": 60,
      "resets_in_seconds": 36
    }
  }
}
