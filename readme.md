# Futureshock

## Overview

This is an alpha version of Futureshock(formerly Corporate Wars), a strategic card game.
Pass-and-play two-player mode is currently the only way to play the game and decks cannot be modified yet.

## Plans that span centuries
Text-based interface

Basic combat and movement

Card deployment and decks

Basic trigger-event mechanics and Tactic cards

--- **WE ARE HERE** --- 

Use less and better pointers

TUI overhaul using ncurses

Client-server support

LAN multiplayer

Graphic interface

Deck editor

Advanced card mechanics

Basic parametrized AI

???

## Build guide

Console TUI edition
- Generate a  makefile: ```cmake -G "Unix Makefiles"``` for linux, ```MinGW Makefiles``` or ```NMake Makefiles``` for Windows)
- Run ```make test``` to build and launch the application. (For Windows use ```mingw32-make``` or ```nmake``` depending on your previous choice)
- Run the application once it has been compiled: ```./test```

# Foundational rules
## Win condition
Take control of **capture zone** - 2-tile wide, 6-tile long area in the middle of playing field, in order to earn points; The first player to score *10* points wins.
Points are awarded to players at the beginning of their respective turns if they have more units placed within the capture zone than their opponents.

## Resource management
Players start the game with **6 cards**. At the beginning of their turn, players draw **one card**, gain basic income of **2 credits**, then gain **1 credit** for each **contract** they have in play.

If player has to draw one or more cards but has no cards left in their deck, it is refreshed by shuffling that player's *discard pile*. If both are empty (e.g. when all cards owned by the player are either on the field or in hand), the action is skipped.

## Card types
Each card belongs to one of three types. 

- **Units** can move on the grid and engage in combat with others; Their main parameter is **combat power**. This card type is essential for controlling the field and scoring points.

- **Contracts** boost your income for a certain number of turns and provide other advantages, yielding more credits than you spend on them in long-term perspective. 

- **Tactics** allow you to perform a certain action to gain immediate advantage. They cannot be placed on board and are discarded immediately after their effect is resolved.

## Actions
### Deploy card
In order to play a card, you must have sufficient *credits* to pay its cost. *Units* require an unoccupied tile to be deployed, while *Contracts* and *Tactics* don't.

Units can be played on either:
- *Your deploy zone*: six tiles adjacent to the border on your side of the grid.
- Any tile that is not in opponent's deploy zone, that has:
  - At least one *Friendly* unit on *adjacent* tiles (4-neighbourhood)
  - *No enemy units* on *surrounding* tiles (8-neighbourhood)

### Move
Units can move to any *adjacent* tile (also known as *4-neighbourhood*: up, down, left, right, but *not diagonally*) **once** per turn and **before** attacking.
A player can make two controlled adjacent units exchange places, if they both are eligible to move. (*NOT IMPLEMENTED YET*)

### Attack
Units can attack any target on *surrounding* tiles (known as *8-neighbourhood*: neighbouring tiles in all directions, *including diagonals*) and only **once** per turn.

In combat, the unit with **least** combat power is *destroyed*, while the other has the first's power subtracted from its own. The defending unit is marked as **Overwhelmed** until the beginning of its owner's turn - this makes it more vulnerable to subsequent attacks *(see below)*

**Advantage** is a frequently used mechanic that makes units inflict bonus damage *before* the resolution of combat. If one or both units engaged in a fight have nonzero advantage, the one with **least** advantage is dealt the difference in both units' advantage values before basic combat rules are resolved. Units that do not have advantage as basic abitily are treated as if they have 0 as its value.

If a unit that has already been marked as *Overwhelmed* is attacked, the attacker gains **1 bonus advantage** until end of combat. This makes attacking with multiple units simultaneously, starting with the weakest ones, the most favorable strategy.

## Deck design

it should have more than around 30 cards and copies will be limited or something, and it will be really hard to build a crappy one because i don't want people to gravitate towards three and a half best-performing decks like they do in m:tg or hearthstone
