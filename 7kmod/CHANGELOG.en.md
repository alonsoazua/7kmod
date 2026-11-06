# 7kmod changelog

**English** | [Español](CHANGELOG.md)

Changes of the `7kmod` branch (executable `7kaa-mod.exe`) compared to the original
**Seven Kingdoms: Ancient Adversaries 2.15.7** (tag `v2.15.7`).

The `7kmod` branch has quality of life and interface improvements and a few balance
adjustments. Changes that alter how the game plays are in the `7kmod-gameplay` branch,
which includes everything in `7kmod` and documents its own changes in
`CHANGELOG-gameplay.en.md`. The scenario editor (since 0.2.0) is described in
`TODO-editor.md` (Spanish only).

## How this file is used

- Each new change is first added under **Unreleased**.
- When a version is released, that section is renamed with the number and the date, and
  the tag `7kmod-X.Y.Z` is created on the matching commit.
- Versions: the third number goes up with fixes, the second with new features or balance
  changes, and the first stays at 0 while the mod is still being tested.
- Sections of each version: **Added**, **Changed**, **Fixed**, **Compatibility**.
- This file is the English translation of `CHANGELOG.md`; both are updated together.

---

## [Unreleased]

---

## [0.3.0] - 2026-11-06

More editor features (kingdoms, kings, spies, blank map) and fixes.

### Added
- **Editor: add and remove kingdoms.** In the Kingdoms window, **Add Kingdom** creates an
  AI kingdom without a king and **Remove Kingdom** removes one together with its towns,
  buildings and units; the color of a kingdom can also be changed. The king is placed
  with the **King** entry of the Units tab, and Delete now removes kings. Test and Save
  warn if any kingdom has no king. Details in `TODO-editor.md`.
- **Editor: blank map.** **Base** option in the New Map window: Generated, **Land** or
  **Water** (the whole map is grass or water, with nothing on it and no kings).
- **Editor: spies.** **Spy** entry in the Units tab, with the options Race, Spying and
  Cloak. Placed on a town or building (of any kingdom) it goes in as a spy; on the map it
  stays as a disguised unit. When selected, its true kingdom is shown and its skill and
  disguise can be changed; towns and buildings show how many spies they have, and towns
  send them out with **Spies out**. Details in `TODO-editor.md`.

### Fixed
- **Selection box over units of other kingdoms:** sometimes some units in the box were
  missing from the list and didn't show their health bar, and if the chosen unit was one
  of them, there was no list. The list checks that each unit is still the same one by
  comparing its name, and it stored the name number in a variable that turned numbers
  above 32,767 negative (`inspect_name_id_array`): those units never matched and were
  removed. It depends on each unit's name, which is why it didn't always happen.
- **Help of the Mughul, Egyptian and Zulu Greater Beings' powers:** their buttons had no
  help text on mouse over, because the game's help file (`RESOURCE/HELP.RES`) doesn't
  have their keys (`GODMUGL`, `GODEGYPT`, `GODZULU`). It's a bug of the original game.
  They now show "Lower Enemy Loyalty", "Increase Population" and "Increase Leadership"
  with their explanation, like the other powers (`UnitGod::power_help`).
- **Fryhtan portraits in the list of selected units:** they showed a Persian portrait,
  because the game data has no small Fryhtan portraits (the original game never shows
  them). They are now made when the game loads by shrinking each Fryhtan's large
  portrait, for all their ranks (soldier, Ordo and All High) (`make_small_icon`,
  `src/OUNITRES.cpp`).

### Compatibility
- No changes to saved games or multiplayer.

---

## [0.2.0] - 2026-10-28

Adds the **scenario editor** (formerly in the `7kmod-editor` branch). Its design, phases
and technical details are in `TODO-editor.md`.

### Added
- **Contribution and honors of generals:** a general becomes unhappy when his contribution (1 point for each hit on the enemy) is more than twice what he has been given in rewards; he loses up to 50 loyalty. The original game didn't show this anywhere. Now:
  - **Military report (F5) with tabs** in the bottom panel:
    - **Units:** what it showed before (units by type).
    - **Contributions:** the list of your generals, sorted by risk, with Loyalty (current and where it's heading), Contrib., Status (Content, Unrewarded, or At risk! if his target loyalty is below 30 and he may betray you), Penalty (how much loyalty he loses) and Honors (what it costs to remove the penalty, at $30 per reward).
    - **Pay selected** pays the honors of the selected general and **Pay all** those of everyone, in order of risk, until the money runs out. They use the game's normal reward. Double click on a general goes to him.
  - **Request for honors:** when one of your generals reaches the maximum penalty (−50), the news "Your general, <name>, asks to be honored for his service: $<cost>" appears. Clicking it moves the camera to the general and selects him (or his fort, if he is inside), and opens the kingdoms report (F1), just like a diplomatic proposal, with your kingdom selected and no tab selected. The request appears below, for example "General Itzcoatl asks $150 to be honored for his service to the kingdom.", with the options **Accept.** (pays the honors) and **Reject.** (doesn't pay). If you don't have enough money, a second line says so and only Reject appears. After choosing, the news is removed and you go back to the previous view. The game keeps running while it's open. If the general dies, changes kingdom or stops being a general, the news is removed and the request is closed. Paying his honors some other way also removes the news.
- **Loyalty breakdown:** when the mouse is over the loyalty of a general or a soldier (in the unit, fort or Seat of Power panel), it shows what makes up his target loyalty: linked towns, troops, race, reputation, king's leadership and the penalty for missing rewards for generals; general's leadership, combat and race for soldiers. Only for your units. Like all the game's help, it appears in single player games with help turned on.
- **Selected units:** when several units are selected, the number of selected units appears at the right of the top bar of the unit panel.
- **Attack power and armor in the unit panel:** below combat (and above contribution) it shows:
  - **Melee pwr:** the power of the melee attack: the average damage per minute before the enemy's armor, at normal game speed, with the same calculation as the game. It combines the damage of each hit and the attack speed. Against a real enemy, armor removes armor ÷ 4 from each hit (1 with the normal armor), so the actual damage is lower, especially for units with weak hits. It's shown without armor so bonuses read exactly: a +100% bonus doubles the power.
  - **Ranged pwr:** the same for the ranged attack, if the unit has one.
  - **General's bonus:** if the unit's general is 10 tiles away or closer, the power includes his bonus and is shown with an up arrow, like loyalty, and the percentage: for example "62 ↑ (+100%)". The percentage is the general's leadership and is exactly how much the power goes up.
  - **Armor:** the unit's armor. In `7kaa-mod.exe` it's 4 for every unit, as in the original game.
  - It doesn't include special attacks that use power. It isn't shown for civilians or your own spies (the spy menu needs that space).
  - The panel grows with the lines it shows and the buttons move down.
- **Fryhtan panel:** when a Fryhtan is selected, its combat, attack power and armor are shown. Before, it showed no data.
- **Documentation:** `GREATER_BEINGS.md` explains in detail how each Greater Being works (how they are obtained, costs, powers and exact effects), based on the game's code and data. It includes what the Phoenix reveals (the information of nearby enemy units and buildings) and that its spy detection is disabled in the original code.
- **Documentation:** `ADVANCED_CONFIG.md` explains the hidden advanced options of the original game's `config.txt`: where the file goes, its format, what each option does and how it behaves in multiplayer and saved games.
- **Documentation:** `KEYBINDINGS.md` lists all the keys and mouse combinations of the game and the mod: generals, map, groups, unit, building and town panels, production queues, menus and cheats, with the name of each action to change it with `bindkey`.
- **Documentation:** `SIEGE_WEAPONS.md` explains siege weapons: building and upkeep costs, what happens without money, stats by version, damage against units and buildings, area damage, shields against projectiles, and details of the Spitfire, the Unicorn and the Porcupine.
- **"Game Paused" notice:** while the game is paused, "Game Paused" appears centered at the top of the map, whether you paused it (`Space` or `0`) or the game paused itself when the window lost focus. It disappears when the game continues. In multiplayer everyone sees it, because the pause is shared. Before, there was no notice.
- **Units outside the fort:** the fort panel keeps showing the units that left it, darkened, so you can see who the fort belongs to and where its troops are:
  - **General (or king):** if the fort is left without a general, the darkened portrait of the general who left appears in his place, with his name and "Outside of the fort". If another general enters the fort, the new one is shown as usual.
  - **Soldiers:** those who left appear darkened, with their health bar, in the empty slots of the soldier list (up to 8). They don't count as soldiers of the fort.
  - Clicking one moves the camera to that unit and selects it.
  - They are only shown while they are on the map: if they die, enter another building or change kingdom, they no longer appear. Only in your forts.
- **Withdraw button in the fort:** on page 1 of the fort's buttons, in the place of Alert / At ease. It makes all the units that left the fort (general or king, soldiers, weapons and Fryhtans) return to it, with the game's own "return to fort" order. It's disabled if nobody is outside. The **Alert / At ease button moves to page 2**, in the first place; that's why your forts now always have the page arrows. When there are spies, their button goes in the second place of page 2.
- **List of selected units:** with several units selected, a **">"** button appears next to their number (at the right of the panel's name bar). It replaces the rest of the panel with a list of the units, like the fort's soldier list: portrait, health bar and combat level (or skill level for civilians). The button changes to **"<"** to go back. The order is king and generals first, then by unit type and combat. Left click on a unit selects only that unit and opens its panel; right click removes it from the selection. On mouse over its name appears. With more than 32 units there are pages with arrows. The list closes by itself if one unit is left selected.
- **Selection box over units of other kingdoms:** before, a box with none of your units selected nothing. Now, if there are units of other kingdoms, wild Fryhtans, rebels or independents inside, the most important one is selected (king, then general, then the one with the most health; combat isn't used so it isn't revealed) and its panel is shown. As with your units, if there is any military unit in the box only military units are taken (with `Alt`, all of them). Since the profession of other kingdoms' units isn't visible (so spies aren't given away), all their human units count as military; only caravans and Trader and Transport ships count as civilians. The number and the ">" button show the list of all the units in the box (up to 100), grouped by kingdom (first the chosen unit's kingdom, at the end those without a kingdom), each with its kingdom's color in a corner and **without combat level**. They don't take orders: left click opens the panel of one (the list is kept and ">" shows it again) and right click removes it from the list. While the panel shows one of them, all those in the list show their health bar over them on the map, like selected units.
- **Selection with military priority:** when selecting with the box, if there is at least one military unit inside (soldiers, generals, king, siege weapons, Caravel and Galleon warships, Fryhtans and Greater Beings), only the military units are selected and civilians are ignored (builders, miners, workers, researchers, priests, caravans and Trader and Transport ships). Your spies count according to the skill they pretend to have: those who look like soldiers (no skill or leadership) are selected with the army, and those who pretend to have a civilian trade are left out. If there are only civilians, they are selected as before. With `Alt` pressed the box selects all units, as in the original game. It also applies with `Shift` to add to the selection.
- **King's portrait in the kingdoms report (F1):** in the list of kingdoms, after each kingdom's color, the portrait of its king appears (for example, the Japanese king's if the king is Japanese). The race of the current king is used, because the successor may be of another race; if the kingdom is without a king for a moment, the king of the kingdom's race is shown. On mouse over the portrait, the king's race appears (like all the game's help, in single player games with help turned on).

- **Scenario editor, phase 0 (MVP):**
  - **Access:** Single Player → Scenario → **New** button (above "Your Name"). It goes to
    the New Game options screen; with Start, the map opens in the editor. The **Edit**
    button is disabled for now.
  - **Editor mode:** the game stays paused (the speed keys do nothing) and the whole map
    is visible. Explored areas don't change: when testing the scenario, the chosen fog
    and map are used. Units can't be given orders; only the keys F11 (screenshot), F12
    (test) and Esc (clear the selection) work.
  - **Top bar** instead of the report buttons: **Scenario**, **Kingdoms**, **Goals** and
    **Save** (darkened, for the next phases), **Test (F12)** and **Exit**.
  - **Right panel:** **Units**, **Towns**, **Buildings** and **Nature** tabs (for now
    only with a text of what's coming) and what is selected on the map (name and
    kingdom). Right click or Esc clears the selection.
  - **Test (F12):** saves a copy (`EDITOR.TMP`, in the game's settings folder) and plays
    the scenario normally. **F12** or **Return to Editor** (in the place of "Training" in
    the game menu) go back to the editor as it was before testing.
  - **Exit:** asks and returns to the main menu.
  - In the editor, the game menu disables Save Game, Load Game, Training and Retire.
    While testing, Load Game is disabled.
- **Scenario editor, phase 1 (save):**
  - The editor's **Save** button opens the **Save Scenario** window: name, difficulty
    (1 to 10), bonus (0 to 500), description (6 lines) and the file that will be used.
  - It saves the scenario in the `SCENMOD` folder of the game's settings folder
    (`%APPDATA%\7kfans.com\7kaa-mod\SCENMOD\`): the game (`.SCN`), the text (`.SCT`) and
    the picture (`.SCP`), which is the middle strip of the map view when saving.
  - The file name comes from the scenario name (`My Scenario` → `MY-SCENARIO`). If it
    already exists, it asks before replacing it.
  - Saved scenarios appear in Single Player → Scenario with "(mod)" after the name and
    are played like the others.
- **Scenario editor, phases 2 and 3 (placing, editing and windows):**
  - **Right panel:** at the top, the kingdom of what is placed (`< ■ Kingdom >`, includes
    Independent). Tabs with a palette:
    - **Units:** Soldier and General (race and combat level), weapons and ships.
    - **Towns:** town (race and population); independent if the kingdom is Independent.
    - **Buildings:** Fort, Mine, Factory, Inn, Market, Tower of Science, War Factory and
      Harbor, finished, empty and free.
    - **Nature:** Clay, Copper, Iron (reserve) and Fryhtan lairs (type).
  - **Placing:** click on the palette and click on the map, with the game's marker
    (colored means it can be placed, black means it can't). Right click or Esc stops
    placing.
  - **The selection:** name, kingdom, one property with `< >` (combat, population or
    reserve) and the **Set Owner** and **Delete** (or Del) buttons.
  - **Top bar:** **Scenario** (name, difficulty, bonus and description without saving),
    **Kingdoms** (money, food, reputation, technologies and relations of each kingdom)
    and **Goals** (the New Game goals).
- **Scenario editor, phases 2b and 4:**
  - **Moving:** with units selected, right click on the map puts them there instantly (a
    group spreads around the point; ships only on water).
  - **Putting in:** right click on a fort, building or town of their kingdom puts them in,
    with the game's rules. Placing a Soldier, General or Worker from the palette on a
    building of its kingdom puts it in directly.
  - **Taking out:** when a building is selected, the panel shows its general and its
    soldiers or workers, with the **All out** and **General out** buttons.
  - **Worker** in the Units palette, with skill (Mining, Manufacture, Research,
    Construction) and level.
  - **Edit** in the Scenario screen: opens the selected scenario in the editor with its
    name, difficulty, bonus and description. An original scenario is saved as a copy in
    SCENMOD.
  - Clearing the selection: Esc or left click on an empty spot (right click now moves).
- **Scenario editor, phases 5a and 5b (terrain):** **Terrain** tab with a 1×1, 3×3 or 5×5
  brush:
  - **Trees** (trees of the ground type), **Clear** (removes trees, rocks and dirt
    patches), **Small rock** and **Large rock**.
  - **Dark grass**, **Light grass** and **Dirt**: paint the ground with its automatic
    transitions. Coasts, hills, buildings, towns and resources aren't painted, so water
    and regions don't change.
- **Scenario editor, new map:**
  - **New** in the Scenario screen no longer shows the New Game screen: it opens the
    editor with a new map with the settings of the last game and a random Map I.D.
  - **New** button in the editor bar: opens the **New Map** window with all the New Game
    options (basic and advanced, the name and the Map I.D., with **Random Map I.D.**) and
    the calculated difficulty. **Create** asks (unsaved changes are lost) and generates
    the map; **Cancel** closes without changing anything. Goals are still edited in
    **Goals**.
  - The new map starts with no scenario name or description, like a new one.

- **Scenario editor, changing race:** **Race** row in the selection:
  - **Town:** the jobless villagers change to the new race (those working in buildings
    and spies keep theirs).
  - **Units** (one or several): soldiers, generals, workers and villagers change race,
    keeping kingdom, rank, combat, leadership or skill, loyalty and equipment. They get a
    name of the new race.
  - **King:** also changes the kingdom's race, and takes a name of the new race (so does
    the kingdom), except with Lock Kingdom Name. With Seats of Power Unlocked, the
    kingdom knows the Seat of its new race.
  - Buildings have no race row (the Seat of Power's is chosen when placing it).
  - The unit stays on the same tile.
  - The selection box grows upward with the rows it needs.
- **Scenario editor, mine:** when a mine is selected, **Mineral** (None, Clay, Copper,
  Iron) and **Reserve** (from 1,000 to 20,000). A mine without a mineral can get one: the
  mineral site is created under it with the reserve of the Nature tab. Changing the
  mineral keeps the reserve and empties what was already mined; None removes the site.
- **Scenario editor, bar:** with a window open, another button of the bar switches to its
  window and the same button closes it.

- **Scenario editor, phase 5c (water, land and hills):** **Water**, **Land** and **Hill**
  entries in the Terrain tab. Water paints sea with its shores, Land paints land (also
  over the sea) and Hill raises a plateau surrounded by hills; strokes join, so high
  paths and plateaus of any shape can be made, with automatic ramps where they fit
  (straight edges). The **Ramp** entry adds or removes a ramp. Afterwards the coasts,
  what can be walked and sailed, the map's regions and those of towns, buildings,
  harbors and the AI are recalculated. Nothing is painted where there are units,
  buildings, towns, resources, walls or rocks.

- **Editor, Seat of Power:** in the Buildings palette, with the Race option. When it's
  placed, the kingdom gets the scroll of its race; when it's removed or changes kingdom,
  the kingdom loses the scroll if it no longer has a Seat of that race.

- **Editor, names:** **Re-roll** button for units and towns (another name of their race;
  buildings with the town's name change with it) and **Rename** for kings. In the
  Kingdoms window, **Lock Kingdom Name** keeps the king's name from changing on re-roll
  or when changing his race.
- **Editor, Workers:** Skill option with **Praying** (Seat of Power) and **None**
  (villager without a trade).

### Changed
- **Version on the main menu:** instead of "7kmod-test", the mod's version is shown, for example "7kmod 0.2.0". The version is in `include/ModVersion.h`.

- **Editor, palette:** Tower of Science is shown as "Twr. Science" because it didn't fit.
- **Editor, Hill with dirt (5d):** Hill paints the plateau's dirt with any brush (like the
  old Dirt) and the hills appear by themselves on its outline; the plateau follows the
  painted shape, with corners and steps, and narrow parts are left as a wall. The
  **Dirt** entry was removed. **Land** lowers or trims plateaus; **Clear** only removes
  trees, rocks and dirt patches; **Water**, Dark grass and Light grass don't change
  plateaus.
- **Editor, drag painting:** in the Terrain tab, holding the button paints wherever the
  mouse passes (all brush entries; rocks and Ramp are still one click). What can't be
  painted in the middle of a stroke is skipped without a warning.
- **Buildings tab:** the Race option was removed; in the game buildings have no race.
  The fort no longer has the Race row (its general's race) in the selection either.
- **Lowering the population** of a town in the editor no longer plays the villagers'
  scream.

### Fixed
- **Units outside the fort:** clicking a darkened soldier of the fort no longer leaves remains of the fort list's health bars over the soldier's panel.
- **Background of the new panels:** the background of the unit list got a bit lighter every time the list changed (for example, when a unit died), and that of the general outside the fort every time it changed. The panel was painted again over itself, and since it isn't opaque it lightened what was already lightened. Now the whole interface is painted again, as the game does.
- **List of units of other kingdoms:** sometimes units that weren't in the box appeared, like a soldier leaving a fort on the other side of the map. When a unit dies or enters a building, the game gives its internal number to the next unit created; the list now checks that each number is still the same unit (type, name and kingdom).
- **Documentation:** in `ADVANCED_CONFIG.md`, the list of actions for `bindkey` was incomplete; `KEYEVENT_CHEAT_ENABLE1` to `3`, `KEYEVENT_SEL_GROUP_7` to `9` and `KEYEVENT_SET_GROUP_1` to `9` were missing.
- **Repeated names in the news:** in messages with two names, such as the one about a spy of a kingdom being exposed and executed, the unit's name came out the same as the king's. The function that builds the names used a single shared buffer, and with the compiler of the mod's executables the second name overwrote the first. It's a bug of the original game that doesn't show in the official version because of the order in which its compiler evaluates the names.

- **Crash when closing the game after a scenario:** after playing any scenario, the game crashed when closing ("double free"). When a scenario starts, a copy of the advanced settings is made to restore them at the end, and the copy shared the `mp_service_addr` text with the original, which was freed twice. It's a bug of the original game; it didn't affect games.

- **Editor, changing a unit's race:** it no longer moves the unit one tile down, and the
  kingdom's units are counted correctly.
- **Editor, yearly expenses:** placing a building no longer adds its cost to the
  kingdom's expenses of the year (the money was already restored).
- **Weapons placed with the editor:** sometimes they had no graphics, didn't attack or
  crashed the game, because they were left without a version when the kingdom didn't
  have its technology. Now every placed weapon has a version (**Version** option in the
  new **Weapons** tab, and also in the selection).
- **Soldiers placed with the editor:** they came out as Peasant because they had no
  leadership skill. Now Soldier and General have the **Leadership** option (20 by
  default) besides Combat (20 by default), and the selection shows both.

### Compatibility
- No changes to saved games or multiplayer. The editor's scenarios are normal saved
  games (plus their text and picture) in the `SCENMOD` folder.

---

## [0.1.0] - 2026-10-18

First documented version. It gathers all the changes made so far on top of 2.15.7.

### Added

#### General
- **Mark on the main menu:** the text "7kmod-test" appears next to the version, to tell the mod apart from the original game.
- **Its own settings and saved games folder (`7kaa-mod`):** the mod doesn't mix its saved games or settings with those of the official game.

#### Fort: recruiting and training from the fort
- **Recruit button in the fort** (Train icon):
  - If several of your towns are linked to the fort, it shows the list to choose one; if there is only one, it goes straight to it.
  - It opens the town in "fort mode", with only three buttons: Recruit, Train and "Back to Fort" in one row. Tax and Grant are hidden.
- **Recruit in fort mode:** the unit of the chosen race goes straight into the fort.
- **Train in fort mode:** queues a Leadership soldier without going through the training menu. Shift + click adds 8.
- **Soldiers trained from the fort go into the fort by themselves** when finished, if it's still yours and has room. If not, they stay next to the town as usual.
- **Counter on the Train button:** shows how many soldiers are being trained for that fort.
- **Button pages for the spy:** if you have spies in your own fort, "<" ">" arrows appear under the row of buttons to switch to the page with the spy button.

#### Game options
- **Seats of Power Unlocked?** (Advanced Options II page, single player game):
  - With "Yes", your kingdom starts knowing how to build the Seat of Power of its race, as if it had the scroll.
  - It only applies to the player's kingdom, not to the AI, and doesn't work in multiplayer.
  - The choice is saved in `7KMOD.DAT`; `CONFIG.DAT` keeps the original format.

### Changed

#### Independent towns
- **Balanced starting population:** all the starting independent towns have 15 to 29 people, no matter how many races they have. Before, mixed ones were smaller (2 races: 12–22, 3 races: 11–19).
- **Lower resistance in mixed towns:** −8 with 2 races and −12 with 3 or more, with a minimum of 10. It applies to starting towns and to those that appear later.

#### Interface
- The Cancel button of the town list is taller, so the text fits.

### Compatibility
- **Saved games:** game version **290** (adds the destination fort of units in training). The mod loads saved games of the official game 2.15.x; the official game doesn't load the mod's saved games.
- **Multiplayer:** training for the fort uses the new network message `MSG_TOWN_TRAIN_FOR_FIRM`. All players need the same executable of the mod.
