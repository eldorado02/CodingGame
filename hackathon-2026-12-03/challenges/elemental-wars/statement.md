
-- languages: Bash, C, C#, C++, Clojure, D, Dart, F#, Go, Groovy, Haskell, Java, Javascript, Kotlin, Lua, ObjectiveC, OCaml, Pascal, Perl, PHP, Python3, Ruby, Rust, Scala, Swift, TypeScript, VB.NET
![](//cdn.codingame.com/smash-the-code/statement/league_wood_04.png)

This is a **league based** challenge.

For this challenge, multiple leagues for the same game are available. Once you have proven your worth against the Boss, you will access the higher league.

**NEW RULE:** Captured elementals are not lost forever! If one of your free elementals reaches the enemy camp, all your imprisoned allies are released.

## Goal

Control your elemental tribe, hunt your prey, avoid your predator, and capture more enemies than your opponents to win the battlefield!

## Rules

The game is played by **3 players** on a square grid. Each player controls a tribe of **elementals**.

Each tribe has one element:

- `W` : 💧 Water
- `F` : 🔥 Fire
- `P` : 🌿 Plant

The tribes follow a cyclic relationship:

- **💧 Water** captures **🔥 Fire**.
- **🔥 Fire** captures **🌿 Plant**.
- **🌿 Plant** captures **💧 Water**.

### 🗺️ Map

The map is a square grid made of the following cells:

- `.`: an empty walkable cell.
- `#`: a wall.
- `W`, `F`, `P`: the camps of the three tribes.

Camps are walkable cells. Walls are not walkable.

All border cells of the grid are walls.

### 🎬 Actions

Each turn, output at least one command. Commands are separated by semicolons (`;`).

- `id MOVE x y`: orders your elemental `id` to move one step toward the target cell (`x`, `y`).
- `id WAIT`: orders your elemental `id` to stay in place.
- `MSG [text]`: displays a global message under your score. It does not target an elemental. The latest message remains visible until it is replaced. If no text is provided, the message is cleared.

You may provide commands for only some of your elementals. Elementals without a command will wait.

If multiple commands target the same elemental in one turn, only the first is applied and the others are ignored.

### ↪️ Movement

A `MOVE x y` command gives a **target cell**. The game moves the elemental by **one step** toward that target using a shortest path.

Movement is 4-directional only (north, east, south, west); diagonal moves are not allowed.

If multiple shortest paths exist, the first step is chosen using this fixed priority order: north, then east, then south, then west.

If the target cannot be reached, the elemental stays in place.

Elementals can move through camps and through other elementals. Only walls block movement.

### ⚔️ Captures

After all players' moves have been processed, all captures are resolved **simultaneously**.

If an elemental shares a cell with one or more elementals of its **predator** tribe, it is captured.

A captured elemental becomes a **prisoner** and is moved to the camp of the tribe that captured it.

Each captured elemental gives **1 point** to the capturing tribe. Points are permanent, but a player who times out or outputs an invalid command loses the game regardless of the points they have earned.

Multiple elementals can stand on the same cell. A stack is not stronger than a single elemental: one predator is enough to capture all prey elementals on the cell.

If all three tribes are present on the same cell, the three tribes capture each other simultaneously.

Nothing special happens when a prey elemental enters an enemy camp: camps do not capture by themselves. Captures only occur when predator and prey elementals share the same cell.

### 🔓 Prisoners and releases

A prisoner cannot move.

Prisoners do not participate in captures. They are ignored during capture resolution.

To release your prisoners, move one of your free elementals onto the camp of your **predator** tribe. All your prisoners held there are released.

Released elementals stay on that camp cell and can be commanded again starting on the next turn.

Releasing prisoners never removes points already earned by captors. Released elementals can be captured again later, and each new capture gives another point.

Releases are resolved **after** captures. If your liberator is captured on the camp during the capture phase, no release happens.

### ⛔ Game End

The game ends when one of the following happens:

- At least one player has all of their elementals imprisoned after turn resolution.
- 200 turns have passed.
- A player times out or outputs an invalid command.

### Victory Conditions

- Have the highest score when the game ends.

### Defeat Conditions

- Your program times out, crashes, or outputs an invalid command.

## Technical Details

- All players receive the full map and the full list of elementals.
- All tribes have exactly one camp and the same number of elementals.
- Elemental identifiers are unique **within a tribe**.
- Several elementals may occupy the same cell.
- If a tribe is fully imprisoned, the game ends immediately and players are ranked by cumulative captures.
- In the settings, you can enable grid display to show cell coordinates on hover.

Click to expand
## Game Protocol

### Initialization Input

Line 1: `myTribe`, your tribe (`W`, `F` or `P`).

Line 2: `mapSize` (integer), the size of the square map.

Next `mapSize` lines: one row of the map, containing `mapSize` characters.

- `.`: empty walkable cell.
- `#`: wall.
- `W`, `F`, `P`: tribe camps.

### Input for One Game Turn

Line 1: three space-separated integers: `scoreW` `scoreF` `scoreP`, the current scores of Water (W), Fire (F), and Plant (P) tribes.

Line 2: `elementalsCount` (integer), the total number of elementals in the game.

Next `elementalsCount` lines: the following space-separated values for each elemental:

- `elementalId` (integer): the identifier of this elemental, unique within its tribe.
- `tribe`: the elemental's tribe (`W`, `F` or `P`).
- `x` (integer): X coordinate, `0` is the left edge.
- `y` (integer): Y coordinate, `0` is the top edge.
- `prisoner`: `1` if this elemental is prisoner, `0` otherwise.

### Output

A single line containing at least one command and at most `elementalsCount` commands.

- `id MOVE x y`: move elemental `id` one step toward the target cell (`x`, `y`).
- `id WAIT`: keep elemental `id` in place.
- `MSG [text]`: display a global message under your score. It does not target an elemental. The latest message remains visible until it is replaced. If no text is provided, the message is cleared.

Commands must be separated by semicolons (`;`). Each `MSG` counts as a command. Its text cannot include semicolons. An empty output line or malformed command is invalid.

Example:

`0 MOVE 5 3;1 WAIT;2 MOVE 7 4`

### Constraints

`mapSize` = `11`, `13` or `15`
`elementalsCount` = `6`, `9`, `12` or `15`

Response time for the first turn ≤ `1000`ms
Response time per turn ≤ `50`ms

# Story

In a world shaped by primal forces, three elemental tribes wage an endless war for dominance. The Water, Fire, and Plant clans are bound by an ancient cycle of power, where each element can overpower another but is also vulnerable in turn.

Driven by instinct and survival, these elementals roam the battlefield, capturing enemies and rescuing allies to strengthen their ranks. Only the most cunning leader will master this delicate balance of aggression and caution.

Will you harness the cycle… or be consumed by it?
