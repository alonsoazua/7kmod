# Seven Kingdoms: Ancient Adversaries keys and shortcuts

**English** | [Español](KEYBINDINGS.md)

Complete list of the keys and mouse combinations of the game 2.15.7 and of the mod's
executables (`7kaa-mod.exe` and `7kaa-gameplay.exe`), taken from the code. Shortcuts
added by the mod are marked **(mod)**.

## Before you start

- **Letter keys work alone, without Shift, Ctrl or Alt.** With a modifier held down,
  the game looks for the combination with that modifier, not the letter alone.
- **Letter keys don't work while you type** in the chat of the kingdoms report (F1) or
  in a text box.
- **Many keys change depending on what you have selected.** For example, `R` recruits
  in a town, sends the soldiers out of a fort and returns a unit to its fort.
- **Configurable keys:** the actions that have a `KEYEVENT_…` name in these tables can
  be changed with the `bindkey` option of `config.txt` (see `ADVANCED_CONFIG.en.md`),
  for example `bindkey=KEYEVENT_TOWN_TRAIN,shift+t`. The others are fixed in the code.
- **Keyboard layout:** symbol keys (`=`, `-`, `,`, `.`, `\`, `;`, `[`, `]`, `'`, `/`) are
  read according to the key that produces that symbol on your keyboard. On the Latin
  American layout `=` has no key of its own (it's Shift+0), so shortcuts with `=` can't
  be used as they are. In that case, change them to another key with `bindkey`. The
  format uses `+` to separate the modifier, so the `+` key can't be assigned.

---

## General

| Key | Action | Name for `bindkey` |
|---|---|---|
| `Esc` | Closes the open report and returns to the map. | (fixed) |
| `F1` | Kingdoms report (diplomacy, chat). Again to close it. | (fixed) |
| `F2` | Towns report. | (fixed) |
| `F3` | Economy report. | (fixed) |
| `F4` | Trade report. | (fixed) |
| `F5` | Military report. | (fixed) |
| `F6` | Technology report. | (fixed) |
| `F7` | Spy report. | (fixed) |
| `F8` | Kingdom rankings. | (fixed) |
| `F9` | News log. | (fixed) |
| `F10` | Game menu. | (fixed) |
| `F11` | Screenshot (`7K000000.BMP`, `7K000001.BMP`…, in the game's settings folder). | (fixed) |
| `F12` | Only with the scenario editor: in the editor, tests the scenario; while testing, returns to the editor. | (fixed) |
| `S` | Save game. | `KEYEVENT_SAVE_GAME` |
| `L` | Load game. | `KEYEVENT_LOAD_GAME` |
| `O` | Options menu. | `KEYEVENT_OPEN_OPTION_MENU` |
| `P` | Toggles the reports between opaque and transparent. | `KEYEVENT_REPORT_OPAQUE_TOGGLE` |
| `X` | Clears the news on screen. | `KEYEVENT_CLEAR_NEWS` |
| `D` | Opens the oldest pending diplomatic message. | `KEYEVENT_OPEN_DIPLOMATIC_MSG` |
| `,` / `.` | Tutorial: previous / next text. | `KEYEVENT_TUTOR_PREV` / `KEYEVENT_TUTOR_NEXT` |

### Window and mouse

| Key | Action |
|---|---|
| `Alt+Enter` | Full screen / window. |
| `Alt+F4` | Quit the game. |
| `Alt+Tab` | Minimizes the game (full screen only). |
| `Ctrl+G` | Captures or releases the mouse inside the window. |
| `Ctrl+M` | Changes the mouse mode (absolute / relative). |

### Game speed

| Key | Action |
|---|---|
| `1` to `8` | Speed from 3 to 24 frames per second (the key × 3). `4` is the normal speed (12). |
| `9` | Maximum speed. |
| `0` or `Space` | Pause / continue. |

### Mini map

| Key | Action | Name for `bindkey` |
|---|---|---|
| `Q` | Terrain map. | `KEYEVENT_MAP_MODE0` |
| `W` | Territory map. Again to show or hide the kingdoms' colors. | `KEYEVENT_MAP_MODE1` |
| `E` | Dot map (only shows units and buildings). | `KEYEVENT_MAP_MODE2` |
| (no key) | Switches to the next map mode. | `KEYEVENT_MAP_MODE_CYCLE` |

### Go to…

Each time you press the key, it goes to the next one of that kind, selects it and
centers the view on it.

| Key | Action | Name for `bindkey` |
|---|---|---|
| `K` | Your king. | `KEYEVENT_GOTO_KING` |
| `G` | Your generals. | `KEYEVENT_GOTO_GENERAL` |
| `F` | Your forts. | `KEYEVENT_GOTO_CAMP` |
| `Y` | Your spies on the map (not those inside buildings or towns). | `KEYEVENT_GOTO_SPY` |
| `H` | Your ships, including those in harbors. | `KEYEVENT_GOTO_SHIP` |
| `J` | A raw material site. | `KEYEVENT_GOTO_RAW` |
| (no key) | Centers the view on the selected unit (also done by clicking its name in the panel). | `KEYEVENT_GOTO_SELECTED` |

### Moving between objects of the same kind

Works with a unit, building, town or raw material site selected.

| Key | Action | Name for `bindkey` |
|---|---|---|
| `↑` / `↓` | Previous / next of the same kind, of any kingdom. | `KEYEVENT_OBJECT_PREV` / `KEYEVENT_OBJECT_NEXT` |
| `←` / `→` | Previous / next of the same kind, of the same kingdom. | `KEYEVENT_NATION_OBJECT_PREV` / `KEYEVENT_NATION_OBJECT_NEXT` |

In the training and production menus, `↑` and `↓` move the queue selection (see below).

### Unit groups

| Key | Action | Name for `bindkey` |
|---|---|---|
| `Ctrl+1` to `Ctrl+9` | Assigns the selected units to group 1–9. | `KEYEVENT_SET_GROUP_1` … `_9` |
| `Alt+1` to `Alt+9` | Selects group 1–9. | `KEYEVENT_SEL_GROUP_1` … `_9` |

---

## Mouse on the map

| Action | Result |
|---|---|
| Left click | Selects a unit, building or town. |
| Left click drag | Selects your units inside the box. **(mod)** If there is at least one military unit (soldiers, generals, king, weapons, warships, Fryhtans), only the military ones are selected; if there are only civilians, all are selected. |
| `Alt` + drag **(mod)** | Selects all your units inside the box, military and civilian, as in the original game. |
| Box without your units **(mod)** | If there are units of other kingdoms inside (or wild Fryhtans, rebels…), selects the most important one (king, general, then the one with the most health) and the panel's `>` button shows the list of all those in the box. If there are military units, only the military ones are taken (with `Alt`, all). |
| `Shift` + click or box | Adds or removes units from the current selection. |
| `Ctrl` + left click | Precise selection: only what is right under the cursor. Without Ctrl, the game also takes nearby large or moving units. |
| Right click on the ground | Moves the selected units. |
| Right click on an enemy | Attacks. |
| Right click on your own building or town | Assigns the units (work, garrison, live, board a ship, etc.). |
| Right click on your own unit | Selects its group, for example all the soldiers who left the same fort. |
| `Shift` + right click on your own building | Attacks your own building. |
| `Shift` + right click on your explosive cart | Attacks the cart to detonate it. |
| `Alt` + right click | Adds a waypoint to the route of the selected units. |
| Right click during an order | Cancels the current order, for example while choosing where to build. |

In the reports (trade, military, spy), **double click** on a row goes to that caravan,
ship, building, troop or spy.

---

## Selection panels

### Units

| Key | Action | Name for `bindkey` |
|---|---|---|
| `B` | Build menu. | `KEYEVENT_UNIT_BUILD` |
| `T` | Settle a town. | `KEYEVENT_UNIT_SETTLE` |
| `R` | Return to its fort. | `KEYEVENT_UNIT_RETURN` |
| `Del` | Dismiss the unit (doesn't work with the king). | (fixed) |
| (no key) | Cancel button of the build menu. | `KEYEVENT_UNIT_CANCEL` |

**Building to build inside the build menu:** in this executable they have no key, but
they can be assigned with `bindkey`:
`KEYEVENT_BUILD_CAMP` (fort), `KEYEVENT_BUILD_MINE`, `KEYEVENT_BUILD_FACTORY`,
`KEYEVENT_BUILD_MARKET`, `KEYEVENT_BUILD_RESEARCH`, `KEYEVENT_BUILD_WAR_FACTORY`,
`KEYEVENT_BUILD_INN`, `KEYEVENT_BUILD_HARBOR`, `KEYEVENT_BUILD_BASE` (Seat of Power),
`KEYEVENT_BUILD_MONSTER`.

### Ships

| Key | Action | Name for `bindkey` |
|---|---|---|
| `R` | Unload everyone. | (fixed) |

`KEYEVENT_UNIT_UNLOAD` exists in the `bindkey` list (with `R` by default), but the unload
button uses the fixed `R`, so changing that event has no effect.

### Buildings

| Key | Action | Name for `bindkey` |
|---|---|---|
| `Del` | Demolish the building, or cancel the construction if it isn't finished. For buildings that can be sold, sell it. | (fixed) |
| `R` in the fort | Send the soldiers out to patrol. | `KEYEVENT_FIRM_PATROL` |
| `R` in the market | Hire a caravan. | `KEYEVENT_FIRM_PATROL` |
| `R` in the inn | Hire the selected unit. | `KEYEVENT_FIRM_PATROL` |
| `R` in the harbor | Sail with the selected ship. | `KEYEVENT_FIRM_PATROL` |
| `B` in the Tower of Science | Choose what to research. | `KEYEVENT_FIRM_BUILD` |
| `B` in the War Factory | Choose which weapon to build. | `KEYEVENT_FIRM_BUILD` |
| `B` in the harbor | Choose which ship to build. | `KEYEVENT_FIRM_BUILD` |

Right click on the builder's button also removes the builder from the building.

### Towns

| Key | Action | Name for `bindkey` |
|---|---|---|
| `R` | Recruit. | `KEYEVENT_TOWN_RECRUIT` |
| `B` | Open the training menu. | `KEYEVENT_TOWN_TRAIN` |
| Right click on **Collect Tax** | Set up automatic tax collection. | — |
| Right click on **Grant** | Set up automatic granting. | — |

### Training and production queues

They work in the town's training menu, in the War Factory and in the harbor.

| Key / mouse | Action | Name for `bindkey` |
|---|---|---|
| `↑` / `↓` | Changes the selected profession, weapon or ship. | `KEYEVENT_MANUF_QUEUE_UP` / `KEYEVENT_MANUF_QUEUE_DOWN` |
| `=` | Adds one to the queue. | `KEYEVENT_MANUF_QUEUE_ADD` |
| `Shift+=` | Adds a batch. | `KEYEVENT_MANUF_QUEUE_ADD_BATCH` |
| `-` | Removes one from the queue. | `KEYEVENT_MANUF_QUEUE_REMOVE` |
| `Shift+-` | Removes a batch. | `KEYEVENT_MANUF_QUEUE_REMOVE_BATCH` |
| Left / right click on the button | Adds / removes one. | — |
| `Shift` + click | Adds or removes a batch. | — |

Batch size: **8** in towns, **10** in the War Factory and **5** in the harbor.

### Added by the mod

| Where | Key / mouse | Action |
|---|---|---|
| Town in fort mode, Train button **(mod, both branches)** | `Shift` + click | Queues 8 soldiers for the fort. |
| List of towns of the fort's Recruit button **(mod, both branches)** | Right click on the panel | Closes the list, just like Cancel. |
| Town, Train Captain button **(mod, gameplay)** | Left / right click | Adds / removes a captain from the queue. |
| Unit panel with several selected **(mod, both branches)** | `>` / `<` button at the right of the name bar | Shows / hides the list of selected units. With several pages, `<` and `>` at the bottom change the page. |
| List of selected units **(mod, both branches)** | Left / right click on a unit | Selects only that unit (opens its panel) / removes it from the selection. |
| Fort panel, darkened units (outside the fort) **(mod, both branches)** | Left click | Moves the camera to that unit and selects it. |
| Fort, Withdraw button **(mod, both branches)** | Click | All the units that left the fort return to it. Alert / At ease is on page 2 of the fort's buttons. |

The mod's other buttons (the fort's Recruit, harbor trade, recruiting in production
buildings, technologies, Invoke Fryhtan) have no key.

---

## Message boxes and menus

| Key | Action |
|---|---|
| `Enter` | Accept (message boxes, options menu). In the chat, sends the message. |
| `Esc` | Cancel or close the box or menu. |
| First letter of the button | In confirmation boxes, it's the same as that button. |
| Right click | In the load or save game windows and the scenario windows, it's the same as Cancel. |
| `O` / `S` / `L` / `Esc` | In the game menu (F10): options / save / load / back to the game. |
| `R` in the main menu | Plays back the last recorded multiplayer game (`NONAME.RPL` in the settings folder). |

---

## Cheats (single player only)

**To turn them on**, press `Shift+1` three times, `Shift+2` three times and `Shift+3`
three times, in that order (`KEYEVENT_CHEAT_ENABLE1` to `3`). The message "Cheat Mode
Enabled." appears. They don't work in multiplayer.

| Key | Effect |
|---|---|
| `C` | +$1000. |
| `\` | +1000 food. |
| `N` | All technologies and all Greater Beings available. |
| `/` | Reveals the whole map. |
| `;` | Adds people to the selected town. |
| `U` | Immortal king (on / off). |
| `Z` | Fast building (on / off). |
| `=` | Fills the prayer points of the selected Seat of Power. |
| `-` | Finishes the construction of the selected building or restores all its health. |
| `[` | +20 combat for the selected unit. |
| `]` | +20 skill for the selected unit. |
| `'` | +20 spying skill for the selected spy. |

With cheats on, `=` and `-` do the cheat and also their normal action in the production
queues.
