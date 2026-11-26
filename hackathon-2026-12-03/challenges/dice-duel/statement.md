# The Goal

Capture more dice of your opponent than you lose.

# Rules

The game is played on an `8x8` grid. Each player has `8` dice to start with. Players move alternatingly.
In each turn a player chooses one of their own dice and rolls it exactly as many cells as the number on top of the die initially showed. The path is a sequence of neighboring cells (diagonals are not considered as neighbors), it's allowed to make turns within the path. It is however not allowed to visit the same cell twice.
The path may not cross any cells which are occupied by other dice. The last step can end on an opponent's die which will then be captured and is out of play.

# Expert Rules

You can find the source code of the game at https://github.com/eulerscheZahl/Dice-Duel.

You can zoom, rotate and pan the viewer by using the mouse wheel and holding the left or right mouse button.

# Game Input

### Input per turn

Line 1: `diceCount`, the number of dice on the board

Next `diceCount`lines: `owner` `cell` `top` `front` `bottom` `back` `left` `right`
`owner` is `0` if the die belongs to you, 1 otherwise.

`cell` is the location of the die. The `x`-coordinate is a letter from `A` to `H`, `y` goes from `1` to `8`.
The remaining values describe the rotation of the die. `top` is equal to the number of cells the die can move.

### Output

A single line `cell sequence`, e.g. `B5 URRUL`. `Cell` is the starting position of the move, the `sequence` can contain the letters `U`, `D`, `R` and `L` for the directions up, down, right, left.

### Constraints

Allotted response time to output is ≤ `50ms` per turn (`1s` for the first turn).

Assets:
Dice https://www.turbosquid.com/3d-models/3ds-dice/412639
Board https://www.turbosquid.com/3d-models/3d-model-chess-games-rook-1540311
Table https://www.turbosquid.com/3d-models/3d-small-dining-table-1161153
Background https://opengameart.org/content/elyvisions-skyboxes
{
  "stub_generator": "gameloop\nread diceCount:int\nloop diceCount read owner:int cell:word(2) top:int front:int bottom:int back:int left:int right:int\nwrite A1 URUL",
  "syntax_doc": "https://www.codingame.com/api/v1/docs/stub-generator",
  "limits": {
    "requests": {
      "name": "requests_1m",
      "limit": 120,
      "used": 3,
      "remaining": 117,
      "window_seconds": 60,
      "resets_in_seconds": 19
    }
  }
}
