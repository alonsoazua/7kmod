# 7kmod-pathing changelog (experimental)

**English** | [Español](CHANGELOG-pathing.md)

Experimental branch with pathfinding changes. It's based on `7kmod-gameplay` and
includes everything in that branch (see `CHANGELOG-gameplay.en.md` and
`CHANGELOG.en.md`). Its executable is `7kaa-pathing.exe`.

If the changes work well in games, they will be moved to `7kmod-gameplay` (or to
`7kmod`).

This file is the English translation of `CHANGELOG-pathing.md`; both are updated
together.

---

## [Unreleased]

---

## [0.7.1] - 2026-11-06

First released version. Based on **7kmod-gameplay 0.2.0** (7kmod 0.3.0). The earlier
versions of this branch weren't released; each change says in parentheses which version
it came in.

### Added
- **Hurrying while gathering (0.7.0):** while the group gathers before marching, the
  units going to their spot in the gathering move 50% faster (an extra movement step
  every 2 frames, with the same checks as a normal step; the units' speed isn't
  changed). When marching they go back to their normal speed.
- **Destination visible during the gathering (0.7.1):** while a selected group gathers,
  a marker is shown at the destination on the map (the waypoint icon) and an animated
  line from the center of the gathering to the destination on the mini map. When
  marching the real paths are shown, as always. In 0.7.0 they were straight lines per
  unit on the map, which at medium and long distances were far from the real path.
- **Gathering before marching (0.6.0)**, like in Age of Empires II: if a scattered group
  (the case of the new formation) is sent to a destination more than 15 tiles away, it
  first gathers in formation **where it is** (centered on the group's center, facing the
  destination) and then marches to the destination keeping the formation.
  - Since 0.6.1 the gathering is looser: a unit counts as ready if it's within 2 tiles
    of its spot, and the group marches when 80% are ready, or after 45 frames (about 2
    seconds; 150 in 0.6.1 and 65 in 0.7.0) even if they aren't. The missing ones get the
    march order anyway. In 0.6.0 it waited for all of them to reach their exact spot (up
    to 400 frames).
  - Units that get another order, die or leave don't count.
  - Grouped units or those with a nearby destination move as before.
  - It isn't saved in the game: if you save while a group is gathering, when loading it
    stays gathered where it was and has to be sent again.

### Changed
- **Waiting for slow units turned off (0.7.0):** in testing it misbehaved. Each unit of
  the group moves at its own speed again, as in the original game. The code stays
  disabled in case it's picked up again.
- **Group speed: wait only when far ahead (0.5.0).** Before (0.4.0) the fast units of a
  group paused all the time to go at the speed of the slowest one, and a single slow
  unit far away slowed down the whole selection. Now each unit moves at its own speed,
  and only waits for the slowest unit that was **within 10 tiles** of it when the order
  was given:
  - It compares how far it has moved since the order with how far the slow one has. If
    it moved more than 3 tiles further, it stops until the difference goes down to 1
    tile.
  - It stops waiting for it if the slow one arrived, stopped, got another order or died,
    or if it has been waiting about 60 frames (the slow one may be blocked); in that case
    it doesn't wait for it again during that move.
  - As before, it stops applying when arriving, when getting another order or when
    entering combat, and it isn't saved in the game.

### Added
- **New formation for scattered groups (0.4.0):** if the group isn't together (it doesn't
  fit in a 10×10 square or its units fill less than half of the square, like the soldiers
  who have just left a fort and are spread around it), its shape is no longer copied: a
  new formation is made at the destination, centered on the click.
  - Rows perpendicular to the direction of movement, facing the destination: 1 row up to
    4 units, 2 rows up to 12, and from 13 on, at least 3 rows of at most 10 units (since
    0.4.1; in 0.4.0 it was always 3 rows, which made large groups very wide). For
    example, 30 units go in 3 rows of 10, 50 in 5 rows and 100 in 10 rows.
    Units side by side. If the movement is diagonal, the formation is diagonal too (in a
    staircase).
  - Order by type, from front to back: melee, ranged units, siege weapons and caravans,
    and at the end the general or the king.
  - Within each row the units are arranged according to their current position, so they
    don't cross paths.
  - If you click very close to the center of the scattered group (less than 3 tiles),
    they gather around the click as in the original.
  - In narrow passages the shape isn't controlled: each unit goes its own way and the
    group forms again at the destination.
- **The group moves at the speed of the slowest unit (0.4.0):** when moving a group of
  land units, the faster ones pause now and then to move at the pace of the slowest one
  (for example, Zulus with siege weapons). The units' speed isn't changed (it's data
  shared by unit type). It stops applying when the unit arrives, gets another order or
  enters combat. It isn't saved in the game: when loading, the units that were on their
  way continue at their normal speed.
- **Swapping places in the formation (0.4.0):** when a unit bumps into a teammate of the
  same group who has already reached his spot and is of the same type and rank
  (soldiers), they swap their places instead of pushing him out of the formation: the
  teammate goes on to the unit's spot and the unit takes his. It's the original game's
  behavior for that case. With units of another type the teammate is still pushed aside,
  as before.
- **Moving in formation, "magic box" (0.3.0):** when moving a group of land units, if the
  group is together (it fits in a 10×10 tile square) and you click outside the area it
  occupies, each unit goes to the clicked point plus its position relative to the
  group's center, so the group arrives in the same shape it had. Each unit searches its
  own path to its spot (they don't share a path), so they don't cross. If a unit's spot
  can't be walked, is occupied or is in another region, the nearest free tile is used. If
  you click inside the group's area, if the group is scattered, if there are more than
  100 units or if any of them isn't 1×1 in size, they gather around the click as in the
  original game.
  - The units move each at its own speed (not at the slowest one's): speed is data
    shared by unit type and the game uses it in many arrival and attack checks, so
    changing it per unit is risky. Since most soldiers have speed 4, in practice the
    formation holds; the ones that do split up are mixes with siege weapons, Zulus (5)
    or Fryhtans.

### Fixed
- **Detours on short moves (0.2.1):** with the new heuristic, moving a unit a few tiles
  in a straight line sometimes made a detour. Since a diagonal step costs the same as a
  straight one, a path that strays to one side and comes back tied with the straight
  one, and the search could pick it. Now the estimate goes up a bit when the path moves
  away from the line to the destination (the smaller of the two distances adds
  (smaller+1)/2), so in a tie the straight path wins.

### Added
- **Sidestep on collision (0.2.0):** when a land unit bumps into another one that is
  moving or waiting (in a crowd or a line), before it only waited its turn. Now, after
  waiting a moment, it tries to pass through one of the two tiles diagonally ahead (45°
  to each side of its direction) and continues to the next point of its path. It picks
  the side that leaves it closer to its path, and with the same distance half of the
  units try one side first and the other half the other, so the crowd opens to both
  sides instead of forming a single file. It only does so if the tiles up to the next
  point of the path can be walked, and not within 2 tiles of the destination, so units
  settle normally when arriving (`Unit::try_sidestep` in `src/OUNITHB.cpp`).
- **Looking for another lane when stuck (0.2.0):** when a unit has been blocked for a
  while and searches for a path again (`Unit::search_or_wait`), the search now counts
  tiles with units as more expensive: +2 for each idle, waiting or fighting unit and +1
  for each moving unit, up to +4 per block. It doesn't forbid them. Before, the units of
  its own group more than 3 tiles away counted as free, so the new path went through the
  same line again; now it looks for a parallel lane if there is one.

### Changed
- **Pathfinding heuristic:** the search (A* over 2×2 tile blocks, `src/OSPATH.cpp`)
  estimated the distance to the destination as the **squared** distance, much greater
  than the real cost of the path (1 or 2 per block). That made it a "greedy" search that
  went straight to the destination: facing concave obstacles (a U-shaped lake, a forest
  with a side entrance, a bay) it went to the bottom and used up all its nodes there, and
  the paths weren't the shortest. Now it uses the number of blocks to the destination
  counting diagonals (Chebyshev distance) with a weight of 3/2, so it goes around
  obstacles without becoming slow.
- **Search budget, 8 times larger:** the original game allowed 2,400 nodes per frame
  shared among all units and 1,600 per search, and didn't start new searches with less
  than 400 available. Now they are 19,200, 12,800 and 3,200. There are fewer delays when
  giving orders to many units and fewer searches that fail by running out of nodes on
  large maps.

### Fixed
- **Nearest point when there is no path:** if a search doesn't reach the destination, the
  unit should walk to the explored point closest to it. The original code only compared
  two nodes (the first of the open list and the first of the closed one, sorted by total
  cost and not by distance), so sometimes the unit went to a far point or a dead end. Now
  the node closest to the destination is searched among all the nodes of the search (and,
  among those equally close, the one closest to the starting point).

### Compatibility
- **Saved games:** same format as `7kmod-gameplay`; they can be loaded in both. Paths
  already calculated are saved as lists of points.
- **Multiplayer:** pathfinding is part of the game logic, so **everyone must use
  `7kaa-pathing.exe`**. Mixing it with `7kaa-gameplay.exe` in a game would make the game
  go out of sync.
- **Replays:** those recorded with another executable won't match.
