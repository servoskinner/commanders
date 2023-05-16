# Corporate wars

## Overview

This is an alpha version of Corporate Wars, a strategic card game.
Pass-and-play two-player mode with standard rules is currently the only one available and decks cannot be modified yet.

## Build guide

Console TUI edition
- Generate a makefile: ```cmake -G "Unix Makefiles"``` ("MinGW Makefiles" or "NMake Makefiles" for Windows)
- Run ```make test``` to build and launch the application. (Windows: mingw32-make, nmake)
- Run the application once it's been built: ```./test```

# Rules
## Win condition
Take control of **capture zone** - the 2-tile wide, 6-tile long area in the middle of grid, in order to earn points; The first player to score *10* points wins.
Points are given to a player at the beginning of their turn if they have more units placed within the capture zone than the opponent.

## Resources
You draw a total of **6 cards**, including your first draw, at the beginning of round. Every time a player's turn starts, they draw **one card**, gain basic income of **2 credits** and extra **1 credit** for each **contract** they have in play.

If a player is forced to draw and has no cards in their deck, it is refreshed by shuffling that player's *discard*. If both are empty, the draw is skipped.

## Card types

- **Units** can move on the grid and engage in combat with others; Their main parameter is **combat power**. This card type is essential for controlling the field and scoring points.

- **Contracts** boost your income for a certain number of turns, yielding more credits than you spend on them in long-term perspective. 

- **Tactics** allow you to perform a certain action to gain quick advantage. They cannot be placed on board and are discarded immediately after being played.

## Actions
### Deploy card
In order to put a card in play, you must have sufficient *credits* to pay its cost. Units require a vacant tile to be deployed, while Contracts and Tactics don't.

Units can be played on either:
- *Your deploy zone*: six tiles adjacent to the border on your side of the grid.
- Any tile that is not in opponent's deploy zone, that has:
  - At least one *Friendly* unit on *adjacent* tiles (4-neighbourhood)
  - *No enemy units* on *surrounding* tiles (8-neighbourhood)

### Move
Units can move on any *adjacent* tile (one tile in any direction, *except for diagonals*) **once** per turn and **before** attacking.

### Attack
Units can attack any target on *surrounding* tiles (3x3 region with the card in its center) and only **once** per turn.

When combat between two units is resolved, the one with **least** combat power is *destroyed*, while the **other** has its power value *reduced* by that of former. The defending unit is marked as *Overwhelmed* until the beginning of its owner's turn.

**Advantage** is a frequently used mechanic that makes units inflict bonus damage *before* the resolution of combat. If one or both units engaged in a fight have this property, the one with **least** advantage is dealt the difference in both units' advantage values before basic combat rules are applied. Units that do not have advantage as basic abitily are treated as if they have 0 as its value.

If a unit that has already been marked as *Overwhelmed* is attacked, the attacker gains **1** advantage bonus that wears off after the combat, meaning that attacking a single target with multiple units yields you extra difference in combat power.


