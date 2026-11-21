# Brandubh

An ancient Celtic game of strategy. A small group of defenders must help their King move to safety, while a larger force of attackers tries to capture him.

# The Goal

Play either the **Attackers** or the **Defenders** in a game of Brandubh (a variant of Hnefatafl). You must fulfill your side's victory conditions before your opponent does.

*Note: The Defenders control the King as well as the regular defending pieces.*

# Rules

*For complete details, you can consult the [official rules PDF](https://github.com/n3l-ctrl/brandubh-codingame/blob/master/docs/brandubh2_rules_en.pdf). Note the following deviations for this CodinGame adaptation:*

- The official rules use the terms "black" and "white" instead of "Attackers" and "Defenders".
- Point 10 (a full game is played in two sets) is irrelevant. Every CodinGame match is a single game.
- The concept of a turn limit doesn't exist in the official rules but is used here to prevent infinite games.

**Board & Coordinates:** The game is played on a 7x7 board. Every game starts from the standard opening position as described in the official rules. The board uses standard chess notation, where columns are `a` to `g` (left to right) and rows are `1` to `7` (bottom to top). The bottom-left square is `a1`.

**Movement:** The Attackers always move first. All pieces move orthogonally (like a rook in chess) any number of empty squares. Pieces cannot jump over other pieces.

**The Throne and Corners:** The center square (3,3) is the Throne. The 4 corner squares are the escape zones. Only the King can stop on the Throne or the corners. Other pieces can pass *over* the Throne if it is empty, but cannot land on it.

**Hostile Pieces:** The opponent's pieces are considered hostile pieces. The Throne and the corners *also* act as hostile pieces for the purpose of capturing.

**The Throne's Hostility:** The Throne is **always** hostile to Attackers. However, it is only hostile to Defenders if the King is **not** currently standing on it. The Throne is never hostile to the King.

**Capturing:** A piece (attacker or defender) is captured if it is flanked on two opposite sides by hostile pieces.

**Capturing the King:** The King is captured like any other piece, *except* when he is on the Throne. If the King is on the Throne, he must be surrounded on all 4 sides to be captured.

**Game End:** The game ends immediately when a victory or defeat condition is met. If neither side wins after **200 turns** (100 rounds), a draw is declared.

**Source Code:** The source code for this game (Referee) is available on [GitHub](https://github.com/n3l-ctrl/brandubh-codingame/blob/master/src/main/java/com/codingame/game/Referee.java).

### Victory Conditions

- **Attackers:** The King is captured, or the King and all his remaining defenders are encircled and cannot move.
- **Defenders:** The King reaches one of the 4 corner squares.

### Defeat Conditions

- You have no valid moves available.
- You repeat the exact same board position 3 times (3-fold repetition rule).
- Your program provides an invalid move.
- Your program times out.

# Game Input

### Input for one game turn

Line 1: `role` - an integer representing your role (`0` for Attackers, `1` for Defenders).

Line 2: `lastAction` - a string representing the opponent's last move (e.g., `d2d4`). It is `NONE` on the very first turn of the game.

Next 7 lines: A `7`-character string representing the board state (from top to bottom, row 7 to 1). Characters can be:

- `.`: Empty square
- `A`: Attacker piece
- `D`: Defender piece
- `K`: The King

### Output for one game turn

A single line containing your move in chess notation: `start_square``end_square`.

- `start_square`: The starting square of the piece you want to move (e.g. `d2`).
- `end_square`: The destination square for that piece (e.g. `d4`).

For example: `d6d7` to move the piece from d6 to d7.

### Constraints

Allotted response time to output is ≤ `1000` ms for the first turn, and ≤ `50` ms for subsequent turns.
