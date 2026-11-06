# 7kmod-gameplay changelog

**English** | [Español](CHANGELOG-gameplay.md)

Changes of the `7kmod-gameplay` branch (executable `7kaa-gameplay.exe`), which change how
the game plays. This branch includes everything in the `7kmod` branch, documented in
`CHANGELOG.en.md`; only this branch's own changes are listed here.

## How this file is used

- Each new change is first added under **Unreleased**.
- When a version is released, that section is renamed with the number and the date, and
  the tag `7kmod-gameplay-X.Y.Z` is created on the matching commit.
- Each version says which version of `7kmod` it's based on.
- Versions: the third number goes up with fixes, the second with new features or balance
  changes, and the first stays at 0 while the mod is still being tested.
- Sections of each version: **Added**, **Changed**, **Fixed**, **Compatibility**.
- This file is the English translation of `CHANGELOG-gameplay.md`; both are updated
  together. `COMBAT_ANALYSIS.md` and `TODO-espionaje.md` are in Spanish only.

---

## [Unreleased]

---

## [0.2.0] - 2026-11-06

Based on **7kmod 0.3.0**, which includes the scenario editor (see `CHANGELOG.en.md` and
`TODO-editor.md`).

### Added
- **Greater Beings** (details in `GREATER_BEINGS.en.md`):
  - **Norman Dragon: fireball.** It no longer attacks; its power button (with its
    fireball as the icon) throws a 100-point fireball with base damage 150 (15 before)
    and 2 more tiles of area: damage radius of 109 pixels (45 before), going down to 50%
    at the edge, and fire within 5 tiles (3 before). Against armor 4 it takes about 37
    health at the center and 20 at three tiles. Buildings take the damage ÷3. The AI
    throws it at groups of 5 or more enemies.
  - **Mughul Djini: converts a soldier.** Besides lowering loyalty, the soldier with the
    lowest combat in the area (not generals, kings or spies; ties: the one with the
    lowest loyalty) joins your kingdom, like a unit that changes sides.
  - **Greek Phoenix: expose spies.** Power button (the spies' eye): on a town of your
    kingdom, it exposes and executes the foreign spy with the lowest skill, with a ring
    of fire over the town (the Mind Turner's, in orange) and the sound of Isis's power.
    It costs 100 points, even if there are no spies; outside one of your towns it uses
    nothing. The AI uses it on its towns with spies.
- **Invoked Fryhtans in the scenario editor:** **Fryhtan** entry in the Units tab, with
  the options **Type** (the 10 Fryhtans that can be invoked, regardless of the kingdom's
  race) and **Combat** (50 by default, as when invoking; in steps of 10). It comes out as
  an invoked Fryhtan of the chosen kingdom (`FirmBase::invoke_fryhtan`): full health and
  everything invoked Fryhtans have, without using prayer points. On a fort of the
  kingdom it goes in as a soldier. When selected, its Combat can be changed. It needs a
  kingdom; wild Fryhtans still come from the lairs. Code in `ModEditorFryhtan.cpp`.
- **The AI invokes Fryhtans** in its Seats of Power and uses them as siege weapons: they
  go into its forts and come out with its attacks. It only invokes if its Greater Being
  is already on the map, if it has 100 points left after paying, if it's at war and if
  its food isn't in deficit; each Seat checks once every 90 days, and it has at most 20
  Fryhtans per kingdom (test numbers). If there is no room in its forts, they stay next
  to the Seat. Also, the Fryhtans of an AI kingdom no longer attack whatever is near them
  like wild ones do (`FirmBase::think_invoke_fryhtan`, `Unit::think_fryhtan_action`).
- **Icon with the kingdom's color on invoked Fryhtans.** Fryhtan sprites have no pixels
  in the kingdom's color, so one of a kingdom looked the same as a wild one. Now, where
  the rank icons go, they carry a 10×10 square in their kingdom's color with a Fryhtan's
  head in black. It's shown like the rank icons (when selected or pointed at, or always
  with the option to show all icons), but on the Fryhtans of every kingdom. Wild ones
  carry no icon (`Unit::draw_fryhtan_nation_icon`, `src/OUNITDRW.cpp`).
- **Ranged shots are no longer blocked** by buildings, walls, mountains or other obstacles (`BulletArray::bullet_path_possible`). Before, if the straight shot (arrows, javelins, etc.) passed through a tile that can't be walked, the archer had to move to another tile to shoot, and sometimes ended up right next to the enemy. Now every shot goes over everything, as the curved projectiles of siege weapons already did. It applies to all ranged units of every kingdom. Side effect: units behind walls or on the other side of a mountain can be shot if they are within range.
- **Spying: Propaganda in independent towns.** In an independent town the spy's "Sow Dissent" mode is called **"Propaganda"** and is much stronger (see `SPYING.en.md` and `TODO-espionaje.md`):
  - **Only each kingdom's most skilled spy counts** in the town (having more spies doesn't speed anything up): every 30 days the resistance toward your kingdom goes down by its skill ÷ 20, without dividing by the population (before, skill ÷ 5 ÷ population of its race). **Below 30 resistance the effect is halved.**
  - **Race matters:** full effect on the people of the spy's race and a quarter on the other races of the town (before, none). The idea is to hire a spy of the town's race at an Inn.
  - **Spies alone don't conquer:** without a fort of yours linked to the town and with a general, propaganda doesn't lower resistance below 30. An empty fort or one with the link turned off doesn't count.
  - **The linked fort's influence goes up:** best spy's skill ÷ 5 if he is of the race of the fort's general (÷ 20 if of another race), up to +20. The fort's target (100 − influence) goes down, so a general of average leadership no longer gets stuck.
  - **Villagers who join:** every 30 days, an independent town with your spies in Propaganda has a chance that **one** villager leaves it and joins your kingdom (at most 1 per town a month). Chance, by race: best spy's skill (one of another race counts a quarter) ÷ 12 × population of that race ÷ 30 %, **at most 20%** (a spy of 100 in a town of 60: about 2 villagers a year). The villager has the combat of the town's defenders ±10, a random skill (construction, mining, manufacturing, research or leadership) from 10 to 10 + the best spy's skill ÷ 2, and loyalty 60. The town is never left empty. A message appears.
  - **Independent towns no longer catch spies** (before, 1 in 100 + skill a month). There, you can only be caught by other kingdoms' counterspies.
  - **Counterespionage in independent towns:** new **"Counter-Spy"** mode (in an independent town the cycle is Sleep → Propaganda → Counter-Spy). Every 30 days your best counterspy in the town can catch and execute a spy of another kingdom who is in Propaganda: skill × 10 ÷ (100 + enemy's skill) % per spy (a spy of 50 against one of 50: 3.3% a month), at most one a month per town. Sleeping spies and counterspies can't be caught this way.
  - **Confrontation between counterspies:** if there are counterspies of two kingdoms in the town, each can find the other (skill × 5 ÷ (100 + the other's skill) % a month). The one who finds the other starts the confrontation and wins with (his skill + 30) ÷ (his skill + 30 + the other's skill); the loser is executed, even if he's the one who attacked. At most one confrontation a month per town. Messages for both sides. The AI doesn't use counterespionage.
  - **The AI** already used this mode in independent towns, so it uses propaganda. It now takes its spy out of the town when the resistance of its race reaches 30 and it has no linked fort with a general (before, it checked loyalty, which independent towns don't have).
- **Spying: accidents.** In any building with workers (not forts or the Seat of Power), of any kingdom, an accident can happen: 1 in 3,650 a day per building (1 every 10 years on average). In normal accidents 1 worker dies 90% of the time and 2 the other 10%; never a spy. Enemy spies in **Sabotage** make it much more likely according to their skill: the multiplier is 1 + the sum of their skill ÷ 10, up to ×16 (a spy of skill 100: ×11, about 1 accident a year in that building). The accidents they cause kill 1 or 2 workers (50% each). **A spy who causes an accident can't cause another for 180 days:** while waiting his skill doesn't count, and the next accident is credited to the most skilled one who isn't waiting. Limits per kingdom in 365 days: 1 normal accident and 3 caused by spies. Messages: "An accident occurred at your <building> near <town>..." and, if your spy caused it, "Your spy caused an accident at the <building> of <kingdom>...". The accidents your spies cause are listed in the Spy report, new **"Sabotage"** tab (date, kingdom, building, nearby town, deaths and the most skilled spy), with the total at the end; the last 30 are kept.
- **Spying: bribe success percentage.** Each button of the bribe menu shows the chance of success, for example "Offer $1,000 (35%)". If the target is a spy of another kingdom the bribe always fails, but the percentage is shown as for a normal unit, so he isn't given away.
- **Spying: intelligence reports** (they replace "Steal secrets", which was removed):
  - New spy mode, **"Gather Intel"**, in towns and buildings of other kingdoms. While gathering, the spy can be caught, as in the other active modes.
  - The report takes **2 to 6 months** depending on the sum of the skill of your spies in Gather Intel in that kingdom: 180 days with 20 or less, 60 days with 200 or more, proportional in between (for example 160 days with 1 spy of 50). One report of each kingdom is received at a time, no matter how many spies you have in it; it's made by the most skilled spy.
  - Sections according to the spy's skill: Towns (20), Economy (30), Trade (30), Kingdom (40), Technology (40), Military (50), Spies (90).
  - Numbers come out as a range with an error of ±(100 − skill) ÷ 2 % (exact with skill 100); the real value is at a random point in the range. Technologies and relations are exact.
  - They are viewed in the Spy report, new **"Intelligence"** tab: the list of reports (date, kingdom, spy, skill) and the details of the selected one, with the same translucent white background as the lists of the other report windows. The last 30 reports of each kingdom are kept.
  - When a report is finished a message appears. The AI doesn't use this mode.
  - The spy's exposure when closing secret reports no longer exists, nor its original bug (see `SPYING.en.md`, section 9).
- **Invoke Fryhtan:** new button in the Seat of Power (hire unit icon, last place in the row) that invokes a Fryhtan of your kingdom with combat 50. Each race has its allied Fryhtan, according to the Seat's race:
  - Normans → Holgh, Mayans → Deezboanz, Greeks → Sauroid, Vikings → Haubudam, Persians → Rokken.
  - Chinese → Wyrm, Japanese → Rattus, Egyptians → Karrotten, Mughuls → Doink, Zulus → Broosken.
  - **Cost in prayer points,** according to each Fryhtan's strength (health × damage per second with combat 50, see `COMBAT_ANALYSIS.md`), with −5 for the slow ones: Rokken 90; Sauroid 80; Wyrm and Haubudam 75; Holgh 60; Rattus 55; Karrotten 50; Deezboanz 45; Broosken 40; Doink 30. Adjusted with simulated duels against soldiers (see `COMBAT_ANALYSIS.md`). The bar has 400 points.
  - **Damage:** invoked Fryhtans do 25% more damage than wild ones, to make up for their low damage per hit for their size. It adds up with the general's bonus if they leave the fort with one.
  - The attack power shown in the Fryhtan's panel (see `CHANGELOG.en.md`) includes this bonus.
  - **Doink:** the only Fryhtan with a ranged attack. Its range goes up from 5 to 6, the same as human archers (also for wild Doinks). The invoked Doink has +1 base damage per hit (6 instead of 5), before the +25%.
  - **Rokken:** the invoked Rokken has armor 5 (wild ones and the other Fryhtans, 4). That makes it the Fryhtans' tank: it holds up better against soldiers with a general.
  - **Broosken:** its speed goes up from 3 to 4, the normal speed of the other units (also for wild ones).
  - They are permanent: they don't use points per day or die if the Seat is destroyed.
  - If you have your own spies inside the Seat, that place is taken by the spy button, as in the original game.
  - A kingdom's Fryhtans don't wander on their own, attack according to their aggressive mode, don't go into Fryhtan lairs, don't give reputation to whoever attacks them, and don't count toward the goal of destroying the Fryhtans.
  - **Regeneration:** they recover 1 health point every 10 game days (about 8 seconds at normal speed), even while moving or fighting. Inside the fort too, besides the soldiers' normal recovery.
  - **Healing of the Persian Greater Being:** its power now also heals your kingdom's Fryhtans, both on the map and inside forts. Before, it only healed human units.
  - **Visible cost:** the button shows the cost in points. On mouse over, the Fryhtan's name, its cost and the points you have appear.
  - Their graphics have no areas in the kingdom's color, so they look the same as wild Fryhtans.
  - Fryhtans have no selection or order sounds in the game data; they use their death sound instead.
  - **Their own voices:** if `MODSOUND\<code>.WAV` exists in the game folder, the Fryhtan uses it as its selection and order sound. The code is the Fryhtan's internal name, the same as its file in `SPRITE` (the Doink's is `GREMJERM`). The Rokken has its voice in `MODSOUND\ROCKMAN.WAV`; the mod's voice files are in `7kmod/MODSOUND` in the repository.
- **Seat of Power points:** the Seat's panel shows the prayer points as a number ("Points: 123 / 400") below the bar.
- **Documentation:** `COMBAT_ANALYSIS.md` (Spanish only) explains how combat works (health, damage formula, attack speed, power and special attacks) and compares the soldiers of each race and the Fryhtans with damage per second tables and simulated duels. It includes that the armor in the data isn't used in the code. The simulator is in `7kmod/tools/combat_sim.py` to recalculate the numbers after a change.
- **Fryhtans in the fort:** your kingdom's Fryhtans can go into your forts as soldiers, just like siege weapons, and come out keeping their species. The fort's panel shows their combat. They don't train with the general. When they leave the fort they have the return to fort button.
- **Small Fryhtan portrait:** the game data has no small portraits for Fryhtans (they used a Persian one). They are now made when the game starts by shrinking their large portrait, and they are shown in the fort's soldier list.
- **Building Repair technology:** new technology in Technologies of the Tower of Science. All your kingdom's buildings recover 1 health point every 5 days (a building of 200 takes about 1000 days to fill up), without needing a builder. It adds up with the builder's repair, if there is one. It keeps repairing during attacks, but stops if your treasury is negative. It doesn't repair walls. For now its research is very fast, for testing; the AI doesn't research it.
- **Fryhtan Slayers technology:** new technology in Technologies of the Tower of Science. Your kingdom's soldiers (human units: soldiers, generals and king) do 30% more damage to Fryhtans, both wild ones and those invoked by other kingdoms. The bonus is applied before armor, like the general's. It doesn't apply to siege weapons, ships or your invoked Fryhtans. For now its research is very fast, for testing; the AI doesn't research it.
- **Invoked Fryhtans eat:** they consume food per year according to their invocation cost: Rokken 50, Sauroid 45, Wyrm and Haubudam 40, Holgh 35, Rattus and Karrotten 30, Deezboanz and Broosken 25, Doink 20 (a person eats 10). They eat on the map and inside the fort. Without food they lose 1 health point per day; it adds up with the loss for lack of money (2 per day if both are missing). Wild Fryhtans don't eat. For now the consumption isn't shown in the interface.

- **Icons of the mod's technologies:** Elite Warriors, Elite Workers, Building Repair and Fryhtan Slayers have their own icon, with the frame and palette of the game's icons. They are shown in the Technologies menu of the Tower of Science (like those of War Machines) and in the panel while being researched (before, a box with the text "Tech").
- **Help in Technologies:** on mouse over a technology its name and description appear. The button no longer shows the description, which didn't fit next to the icon.

### Changed
- **Greater Beings:** the Chinese Jing Nung uses 5 points per attack (10 before) and the
  Phoenix costs 1.5 per day (2.5 before). The changes to the `GOD` table are applied in
  the code (`GodRes::apply_mod_changes`), also when loading a saved game.
- **The mod's technologies:** Building Repair and Fryhtan Slayers cost the same as Elite
  Warriors (before they were very cheap, for testing).
- **Race balance:**
  - **Armor by race:** the original game uses armor 4 for every unit and ignores the one in the data. Now armor goes up with combat for some races: Normans have 4 and go up to 5 with combat 50; Greeks have 4, go up to 5 with combat 30 and to 6 with combat 70; Mayans have 3 while their combat is below 50 and 4 from 50. The other races, Fryhtans, weapons and ships stay at 4. Each armor point takes 0.25 damage off each hit received. The unit panel shows its current armor.
  - **Chinese:** the ranged attack is learned with combat 40 (30 before).
  - **Egyptians:** the ranged attack is learned with combat 30 (40 before).
  - **Japanese:** the special attack is learned with combat 50 (55 before).
  - **Mughuls:** the second special attack is learned with combat 50 (60 before).
  - **Zulus:** movement speed 5 (4 before), 25% faster.
  - **General's bonus in battle:** a general on the map with at least 4 of his soldiers (the units he leads) within 10 tiles gets his own leadership as a damage bonus (for example +60% with 60 leadership) and +1 armor. It's the same range as the bonus he gives his soldiers. It also applies to the king with his troops. Only the units that have that general (or the king) as their leader count: those that leave the fort with the patrol button or in the fort's automatic defense; those you take out one by one with right click don't count. The general's panel shows the bonus with the up arrow, for example "Armor 5 ↑ (+1)".
  - The changes are applied when the data is loaded; the game files aren't modified. The effect on combat is in section 8 of `COMBAT_ANALYSIS.md`.
- **The Persian Greater Being (Lord of Healing) heals more in forts:** inside a fort or the Seat of Power, soldiers (and priests) recover between 1/5 and 1/3 of their maximum health (before, between 1/7 and 1/4), and the general in command heals like his soldiers (before, he healed as on the map, between 1/9 and 1/6). On the map it heals the same as before.
- **Towns of another kingdom that surrender:** they start with 15 more loyalty (55 for your king's race and 45 for the others, 40 and 30 before) and can't rebel for 90 days, the same wait there already was between two rebellions. Before, they were left right above the rebellion limit (29) and sometimes rebelled almost immediately. It doesn't apply to captured independent towns or when a whole kingdom surrenders. It applies to the AI too.
- **Version on the main menu:** the version of this branch and that of the `7kmod` it's based on are shown, for example "7kmod-gameplay 0.2.0 (7kmod 0.3.0)". The version is in `include/ModVersion.h`.

### Compatibility
- **Saved games:** version 300 (296 adds Building Repair, 297 Fryhtan Slayers, 298 spying: accident dates, gathering progress and intelligence reports, 299 the list of accidents caused by spies and 300 the spies' wait after an accident). Saved games of version 2.00 and later are still recognized as the same base format even though the version reaches 300. Older saved games still load (the new technologies stay unresearched and without intelligence reports). A kingdom's Fryhtans are saved as normal units.
- **Multiplayer:** invoking a Fryhtan uses the new network message `MSG_F_BASE_INVOKE_FRYHTAN`. All players need the same executable.
- **Race balance:** it doesn't change the format of saved games; when a saved game is loaded the new rules are applied. In multiplayer, everyone needs the same executable.

---

## [0.1.0] - 2026-10-18

Based on **7kmod 0.1.0**. First documented version; it gathers all the changes made so
far.

### Added

#### Harbor: raw material trade
- **Trade button** (tax icon) that switches to the trade panel:
  - The harbor works like a mine without workers: it produces 10 units every 3 days, with storage of up to 500.
  - **Rotate production:** switches between clay, copper and iron. What is stored is lost, as when changing a factory's production.
  - **Hire Shipment** (Grant icon): costs $500, is recorded as an import expense and adds 500 to the supply. Without supply there is no production.
  - Back button (ship icon).
- Linked markets and factories take raw materials both from mines and from harbors.

#### Recruiting in production buildings
- **Recruit button (Train icon)** in mines, factories, Towers of Science and War Factories, just like in the fort:
  - Recruit puts the unit straight into the building.
  - Train trains the building's profession (mining, manufacturing or research) and the unit goes in by itself when finished.
- **Automatic recruiting button** (defense icon) in those buildings of the player, to turn it on or off.
- "Back to <building>" is shortened to "Back" if the name doesn't fit.

#### Technology tree
- **Conduct Research** in the Tower of Science asks first: **Technologies** or **War Machines**.
- New technologies, researched in Technologies:
  - **Elite Warriors:** soldiers trained in Leadership start with 30 combat (20 before).
  - **Elite Workers:** units trained in the other professions start with 30 skill (20 before).
- The research time of both is 3/4 of that of the level 1 catapult.
- When finished a news message appears. The AI doesn't research them.

#### Towns: Train Captain
- **Train Captain button** (promote icon), in the second row of the town panel:
  - Trains a **general** of the king's race with 30 leadership, 20 combat and a random leadership potential from 50 to 100.
  - It costs $150 and 1 person, and takes 4 times as long as a normal soldier (20 days).
  - It only works in towns where the king's race is the majority.
  - Left click adds to the queue, right click removes one. The button shows how many are in training.

#### Documentation
- `TODO.md`: list of pending ideas and their status (Spanish only).

### Changed

#### Independent towns
- **At most 2 races** in independent towns (up to 3 before), both starting ones and those that appear later.
- **55% of the starting towns have 2 races** and 45% only one. The second race is always different from the first.

#### Production buildings
- **Automatic recruiting off by default** in the player's mines, factories, Towers of Science and War Factories. The AI keeps recruiting automatically.
- **Room for the spy button:**
  - Tower of Science and War Factory: the buttons moved up a bit and the spy button goes in a second row.
  - Factory: uses the pages with arrows, like the fort.

### Fixed
- The technologies' progress bar no longer goes outside the panel or leaves remains on the map.

### Compatibility
- **Saved games:** the mod loads saved games of earlier versions, including those of the official game 2.15.x and those of `7kmod`. The official game and `7kaa-mod.exe` don't load this branch's saved games.

  | Saved game version | Adds |
  |---|---|
  | 291 | Harbor trade |
  | 292 | Supply from hired shipments |
  | 293 | Automatic recruiting per building |
  | 294 | The mod's technologies |
  | 295 | Elite Workers |

- **Multiplayer:** each new action has its own network message (`MSG_F_HARBOR_TRADE_RAW`, `MSG_F_HARBOR_HIRE_SHIPMENT`, `MSG_FIRM_TOGGLE_AUTO_RECRUIT`, `MSG_F_RESEARCH_START_MOD_TECH`). All players need the same executable.
