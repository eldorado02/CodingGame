This is a port of the **Texas hold'em** version of Poker.

The source code of this game is available [on this GitHub repository](https://github.com/wala-fr/CodingamePoker).

# RULES

The rules of **Texas hold 'em** can be found all over the [web](https://www.google.com/search?q=texas+hold'em+rules). For example : [wikipedia](https://en.wikipedia.org/wiki/Texas_hold_'em#Rules) with the [hand ranking](https://en.wikipedia.org/wiki/Texas_hold_'em#Hand_values).

## **Game parameters**

The `number of players` may vary from `2` to `4` . The action moves **clockwise** around the table. The `player 0` is at the `top` .

The total of all buy-ins is always `$4800` . So for 2 players the `buy-in` is `$2400` , for 3 players it's `$1600` and for 4 players it's `$1200` .

The value of the `small blind` / `big blind` are `$5` / `$10` . Then every `10` hands, the **level** will increase and the blinds (small and big) are multiplied by `2` (the first hand is `1` so to simplify the blinds will increase at hands `10` , `20` ,...).

The `first big blind player` is random.

These values are given as game inputs.

## **End game**

The game ends :

- when one player owns all the chips.
- after `600` rounds and if the **last hand** is not over, it's **cancelled**.

The players are ranked by elimination round and stack.

## **Timeout**

When a player timeouts :

1. He is eliminated.
2. He is considered folded for the current hand.
3. The chips from his stack are removed from the game.

## **Heads-up**

In heads-up :

- `Player 0` is on the `left` .
- The dealer / button is the small blind.

# CARDS

The card's abbreviations are `2D` , `KH` ... ([complete list](https://github.com/wala-fr/CodingamePoker/blob/main/CodingamePoker/config/md/CARDS.md)).

| **Hand examples** | `AS_AD` |  |
| --- | --- | --- |
|  | `TC_5H` |  |
|  |  |  |
| **Board examples** | `X_X_X_X_X` | PREFLOP |
|  | `4H_6H_JS_X_X` | FLOP |
|  | `4H_6H_JS_AC_X` | TURN |
|  | `4H_6H_JS_AC_AD` | RIVER |
|  |  |  |
| **Showdown examples** | `QH_3S_E_E_5C_H_7D_KC` | player0's cards, followed by player1's cards ... player1 was eliminated ( `E` ) at the start of the hand. |

## **Showdown**

At the end of each hand, you will know the cards of all the players (**even the folded ones**).

# ACTIONS

## **Possible actions**

There are `5` possible actions / outputs :

- `CALL`
- `CHECK`
- `BET amount` , where `amount` is the number of chips that you add to the pot.
- `ALL-IN`
- `FOLD`

A list of all possible actions is given in your inputs. `BET_240` means 240 is the minimum raise. You can bet more. If you bet less it will be replaced by `CALL` or `BET 240` (depending on your amount).

In the inputs, `BET` actions have an **underscore**. In the outputs, you can use a space.

## **Bet / Raise**

A summary of some betting rules (with examples) can be found [here](https://github.com/wala-fr/CodingamePoker/blob/main/CodingamePoker/config/md/RAISE.md) .

## **Action replacement**

In some cases, if you give an "invalid" action, it will be **automatically replaced** by a "legal" one.

For example :

- `BET 500` with a stack of $400 will be replaced by `ALL-IN` .
- `TOTO` or other invalid action will be replaced by `FOLD` .

However if you `FOLD` when you could just `CHECK` , it won't be replaced.

The replacement informations are written in the output console and a complete list can be found [here](https://github.com/wala-fr/CodingamePoker/blob/main/CodingamePoker/config/md/ACTION_REPLACEMENT.md) .

## **Other players actions**

The actions of all the players are given as well as the round, hand's number, playerId and board cards when each action was done.

The action can be :

- `CALL`
- `CHECK`
- `BET_500` , with an underscore after BET.
- `ALL-IN`
- `FOLD`
- `NONE` , if no action during the hand (for example: if all players are directly all-in). Then `playerId` is `-1` .
- `TIMEOUT` , the player has timeout

The first action given is always your last action.

# GRAPHICS

Only the **big wins** (more than 2 * BB * number of alive players) and level's changes are marked on the **scrollbar**.

There's a **debug mode** with "bigger" cards and buttons to show / hide the **opponents cards** and the **folded cards**.

The winning probabilities are calculated with a java version of [kennethshackleton's SKPokerEval](https://github.com/kennethshackleton/SKPokerEval/tree/develop) (a Texas Hold'em 7-card hand evaluator).

### Victory Conditions

- You own all the chips.
- After 600 rounds you own the biggest stack.

### Loss Conditions

- You've lost all your chips.
- You do not respond in time.

# Game Input

### Initialization input

Line 1 : `smallBlind`
Line 2 : `bigBlind` the initial values of the blinds.
Line 3 : `handNbByLevel` number of hands to play to reach the next level.
Line 4 : `levelBlindMultiplier` blinds are multiply by this coefficient when the level changes.
Line 5 : `buyIn` initial stack for each player.
Line 6 : `firstBigBlindId` id of the first big blind player .
Line 7 : `playerNb` number of players ( `2` to `4` ).
Line 8 : `playerId` your id ( `0` to `3` ).

### Input for one game turn

Line 1 : `round` (starts at `1` , game ends when it reaches `600` ).
Line 2 : `handNb` hand's number (starts at `1` ).
Next `playerNb` lines :

- `stack` number of chips in the player's stack.
- `chipInPot` number of player's chips in the pot. The sum of all the chipInPot values is equal to the pot.

Next line : `boardCards` board cards (example : `AD_QH_2S_X_X` ).
Next line : `playerCards` your cards (example : `TC_JH` ).
Next line : `actionNb` number of actions since your last turn.
Next `actionNb` lines :

- `actionRound` round of the action.
- `actionHandNb` hand number of the action.
- `actionPlayerId` player id of the action ( `-1` if action is `NONE` ).
- `action` action (examples : `BET_200` , `FOLD` , `NONE` ...).
- `actionBoardCards` board cards when the action is done (example : `AD_QH_2S_4H_X` ).

Next line : `showDownNb` number of hands that ended since your last turn.
Next `showDownNb` lines :

- `showDownHandNb` hand's number.
- `showDownBoardCards` board cards at the showdown.
- `showDownPlayerCards` players cards (example : `QH_3S_E_E_5C_H_7D_KC` player0's cards, followed by player1's cards ...).

Next line : `possibleActionNb` number of actions you can do.
Next `possibleActionNb` lines :  `possibleAction` your possible action ( `BET_240` means 240 is the minimum raise but you can bet more).

### Output

A single line containing the player action. Example: `BET 200`
You can also add a message : `FOLD;MESSAGE`

### Constraints

`2` ≤ number of players ≤ `4`
Response time first turn is ≤ `1000` ms.
Response time per turn is ≤ `50` ms.
`1` ≤ round ≤ `600`
{
  "stub_generator": "read smallBlind:int\nread bigBlind:int\nread handNbByLevel:int\nread levelBlindMultiplier:int\nread buyIn:int\nread firstBigBlindId:int\nread playerNb:int\nread playerId:int\n\ngameloop\nread round:int\nread handNb:int\nloop playerNb\n\tread stack:int chipInPot:int\nread boardCards:word(14)\nread playerCards:word(5)\nread actionNb:int\nloop actionNb\n\tread actionRound:int actionHandNb:int actionPlayerId:int action:word(10) actionBoardCards:word(14)\nread showDownNb:int\nloop showDownNb\n\tread showDownHandNb:int showDownBoardCards:word(14) showDownPlayerCards:word(23)\nread possibleActionNb:int\nloop possibleActionNb\n\tread possibleAction:word(10)\nwrite CALL;MESSAGE\n\nINPUT\nsmallBlind:initial small blind's value\nbigBlind:initial big blind's value\nhandNbByLevel:number of hands to play to reach next level\nlevelBlindMultiplier:blinds are multiply by this coefficient when the level changes\nbuyIn:initial stack for each player\nfirstBigBlindId:id of the first big blind player \nplayerNb:number of players (2 to 4)\nplayerId:your id\nround:round (starts at 1-game ends when it reaches 600)\nhandNb:hand number (starts at 1)\nstack:number of chips in the player's stack\nchipInPot:number of player's chips in the pot\nboardCards:board cards (example : AD_QH_2S_X_X)\nplayerCards:your cards (example : TC_JH)\nactionNb:number of actions since your last turn\nactionRound:round of the action\nactionHandNb:hand number of the action\nactionPlayerId:player id of the action\naction:action (examples : BET_200, FOLD, NONE...)\nactionBoardCards:board cards when the action is done (example : AD_QH_2S_4H_X)\nshowDownNb:number of hands that ended since your last turn\nshowDownHandNb:hand number\nshowDownBoardCards:board cards at the showdown\nshowDownPlayerCards:players cards (example : QH_3S_E_E_5C_H_7D_KC player0's cards, followed by player1's cards ...)\npossibleActionNb:number of actions you can do\npossibleAction:your possible action (BET_240 means 240 is the minimum raise but you can bet more)",
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
