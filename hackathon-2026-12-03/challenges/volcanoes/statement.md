# The Goal

The goal of the game is to create a chain of volcanoes that connects a tile in the northern hemisphere to it's corresponding tile in the southern hemisphere.

Opposing tiles are conveniently numbered so that `N16` (north 16) is opposite `S16` (south 16).

Since the board is a 2D representation of a [Pentakis Icosidodecahedron](https://en.wikipedia.org/wiki/Pentakis_icosidodecahedron) (an 80-faced sphere-ish shape), tiles connect around the edges of the board (e.g., `N40` connects to `N21` and `S17` connects to `S18`).

# Rules

On your turn you may perform one of the following actions.

1. Place a new level 1 volcano of your color on any empty tile.
2. Increase the level of one of your existing volcanoes by one (up to a maximum level of 4).

Play progresses as follows.

1. Blue's turn
2. Orange's turn
3. Growth (every volcano on the board gains one level)
4. Orange's turn
5. Blue's turn
6. Growth

This cycle repeats until a player wins by successfully creating an unbroken chain of volcanoes from one side of the board to the other.

When a volcano advances to level 4 (during a player's turn or a growth turn), it erupts with the following effects.

- The level 4 volcano becomes dormant (it stops growing)
- Adjacent empty tiles become level 1 volcanoes for the team who owns the erupting volcano.
- Adjacent volcanoes on the same team as the erupting volcano are increased by one level.
- Adjacent volcanoes on the opposite team as the erupting volcano are destroyed and leave behind empty tiles.

### Victory Conditions

Win by connecting any northern tile to its corresponding southern tile with an unbroken chain of volcanoes.

# Expert Rules

Since the connections between tiles are tedious to set up, they are provided to you in the initialization inputs.

Eruptions can trigger further eruptions. If an infinite chain of eruptions is detected then the game is considered a draw.

It is possible for both players to complete a path at the same time during the growth phase. If this happens then the game is declared a draw.

Game concept by [Simon Dorfman](https://simondorfman.com/volcanoes1).

Code is available on [GitHub](https://github.com/skotz/codingame-volcanoes).

# Game Input

### Input (first move only)

Line 1:  `numberOfTiles`, the total number of tiles on the board

Next `numberOfTiles` lines:  `4` space-separated values:

- `tileName`, the name of the tile at the given index
- `neighbor1`, the index of a neighboring tile
- `neighbor2`, the index of a neighboring tile
- `neighbor3`, the index of a neighboring tile

### Input (each turn)

Line 1:  `currentPosition`, a space-separated list of the volcano levels of all `numberOfTiles` tiles on the board, with positive values for your volcanoes and negative values for your opponent's volcanoes

Line 2:  `validMoves`, a space-separated list of valid moves in the current position

### Output

Line 1:  `move`, the name of the tile to play on or `RANDOM` to play a random move

### Constraints

`numberOfTiles` = `80`

Bots must respond within `100` milliseconds.
{
  "stub_generator": "read numberOfTiles:int\nloop numberOfTiles read tileName:word(3) neighbor1:int neighbor2:int neighbor3:int\ngameloop\nread position:string(1000)\nread moves:string(1000)\nwrite random\n\nSTATEMENT\nWin by building an unbroken chain of volcanoes from one side of the board to the other.\n\nINPUT\nnumberOfTiles : Number of tiles on the board\ntileName: Name of the tile at this index (e.g., N01 or S25)\nneighbor1: Index of a neighboring tile\nneighbor2: Index of a neighboring tile\nneighbor3: Index of a neighboring tile\nposition: Space-separated list of the volcano levels for every tile on the board (in index order); value will be positive for your volcanoes, negative for your opponent's volcanoes, or 0 if empty\nmoves: Space-separate list of all valid moves in this position\n\nOUTPUT\nEither RANDOM or a tile name (e.g., N12 or S34)",
  "syntax_doc": "https://www.codingame.com/api/v1/docs/stub-generator",
  "limits": {
    "requests": {
      "name": "requests_1m",
      "limit": 120,
      "used": 1,
      "remaining": 119,
      "window_seconds": 60,
      "resets_in_seconds": 60
    }
  }
}
