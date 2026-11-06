# Advanced options (`config.txt`)

**English** | [Español](ADVANCED_CONFIG.md)

Seven Kingdoms 2.15.7 has about 45 advanced options that don't appear in any menu. They
are set in a text file, `config.txt`. This document explains each one based on the
source code (`ConfigAdv.cpp` and the places in the game that use each option).

---

## 1. How to use it

### Where the file goes
The game looks for `config.txt` in this order and uses the first one it finds:
1. **The executable's settings folder.** For the mod's executables it's
   `%APPDATA%\7kfans.com\7kaa-mod\` (the same folder as the saved games and `CONFIG.DAT`).
   For the official game it's `%APPDATA%\7kfans.com\7kaa\`.
2. **The folder the game is run from** (for example `Desktop\7kaa`).

The file doesn't exist by default: it has to be created. It's only read when the game
starts, so any change requires restarting it.

### Format
- One option per line: `name=value`. Spaces around the `=` are allowed.
- Lines that start with `#` are comments.
- **Yes/no values:** `true` or `false` (not `1`/`0`).
- **Numbers:** integers, like `6`.
- If a line has an invalid name or value, the game shows "Error in config.txt at line N"
  and **discards the whole file**, going back to the default values.

### Example
```
# Each kingdom starts with the scroll of its race
nation_start_god_level=1
# Mines that never run out
mine_unlimited_reserve=true
# Play in a 1280x960 window
vga_full_screen=false
vga_window_width=1280
vga_window_height=960
```

### Multiplayer, saved games and scenarios
Each option has one of these marks in the tables:

- **[Same]**: it changes the game, so **all the players of a multiplayer game must have the same value**. Otherwise the game won't let them join (it compares a checksum). It's also recorded in replays.
- **[Host]**: in multiplayer the host's value is used and sent to the others. It's also **saved in the game**: when a saved game is loaded, the values it was saved with are used, not those of your `config.txt`.
- **[Local]**: it only affects your computer (screen, keys, language, etc.).
- **[Unsynced]**: it changes the game but **is neither checked nor sent** in multiplayer. If the players have different values, the game may go out of sync; use it only in single player games or make sure everyone has the same value.

**Scenarios and tutorials:** if a scenario has its own `config.txt` in its folder, the game
applies it while that scenario is played and then restores yours (see `scenario_config`).
For example, the `TUTORIAL` folder has one with `monster_attack_divisor=6` and
`town_loyalty_qol=false`.

---

## 2. Game options

### Game start

| Option | Default | Values | What it does | Mark |
|---|---|---|---|---|
| `nation_start_god_level` | `0` | `0`, `1`, `2` | Scrolls at the start. `0`: none. `1`: each kingdom has the scroll of its race (it can build its Seat of Power). `2`: all kingdoms have all the scrolls. It applies to all kingdoms, including the AI. | Unsynced |
| `nation_start_tech_inc_all_level` | `0` | `0`, `1`, `2` | Raises **all** technologies of all kingdoms by that many levels at the start (catapults, ballistas, etc. already available). | Unsynced |
| `race_random_list` | all | `original` | With `original`, random races (AI kingdoms, independent towns, inn mercenaries) only come from the 7 original races: Normans, Mayans, Greeks, Vikings, Persians, Chinese and Japanese. Without the option, Egyptians, Indians and Zulus also appear. | Host |

### Fryhtans (monsters)

| Option | Default | Values | What it does | Mark |
|---|---|---|---|---|
| `monster_attack_divisor` | `4` | `1` to `6` | How often Fryhtans attack towns and buildings. With the original curve, there is on average **one attack every N months**, no matter how many lairs there are. Higher = fewer attacks. Attacks start about 3 years after the start. | Host |
| `monster_alternate_attack_curve` | `false` | `true`/`false` | Alternative curve: attacks increase with the number of lairs. Every 6 months: 1 attack with 0 lairs, 2 with 5, 3 with 10, 5 with 20, 7 with 40, 11 with 80. It ignores `monster_attack_divisor`. | Host |

### AI kingdoms

| Option | Default | Values | What it does | Mark |
|---|---|---|---|---|
| `nation_ai_defeat_when_no_towns` | `false` | `true`/`false` | An AI kingdom without towns (population 0) surrenders or is defeated, even if it has units left. Meant for simulation games. | Same |
| `nation_ai_no_treaty_with_biggest` | `true` | `true`/`false` | Large AI kingdoms don't make friendship treaties with their strongest rival. With `false` they can. | Same |
| `nation_ai_unite_min_relation_level` | `neutral` | `hostile`, `tense`, `neutral`, `friendly`, `alliance`, `off` | When the AI is aggressive and all AI kingdoms are far behind the humans, they surrender to the strongest AI kingdom to unite against the player. This option is the minimum relation needed to surrender to it. `off` disables the union. | Host |
| `fix_ai_consider_trade_treaty` | `true` | `true`/`false` | Fix: the AI really evaluates trade treaties. With `false` the original bug comes back: the AI always accepts them. | Same |
| `firm_ai_enable_think_spy_capture` | `false` | `true`/`false` | AI kingdoms take a building (anyone's, including the player's) when their spies inside it are enough to capture it, and then send troops against the nearby forts of the previous owner. | Same |

### Towns

| Option | Default | Values | What it does | Mark |
|---|---|---|---|---|
| `town_migration` | `true` | `true`/`false` | Town people move to other, more attractive towns. With `false` there is no migration between towns. | Same |
| `town_loyalty_qol` | `true` | `true`/`false` | The town's quality of life affects loyalty: from −17 to +17 (quality of life 0 to 100). With `false` quality of life doesn't affect loyalty. | Host |
| `town_ai_emerge_nation_pop_limit` | `420` | number | An independent town can only become a new kingdom if the total population of all kingdoms is below this number. | Host |
| `town_ai_emerge_town_pop_limit` | `1000` | number | New independent towns only appear if the total population is below this number (and there are fewer than 10 independents). | Host |
| `firm_migrate_stricter_rules` | `true` | `true`/`false` | Stricter rules for workers to move to another town: they don't move to towns where their race has loyalty below 40, and the new town must be clearly more attractive. With `false` they move more easily. | Same |
| `rebel_think_town_action` | `false` | `true`/`false` | Rebels who settle in a town can, over time, become a normal independent town or form a new kingdom (if the town has 20 people or more). | Same |

### Units

| Option | Default | Values | What it does | Mark |
|---|---|---|---|---|
| `unit_loyalty_require_local_leader` | `true` | `true`/`false` | A soldier only gets his general's loyalty bonus if the general is **nearby**. With `false` it's enough to have a general assigned, wherever he is. | Same |
| `unit_ai_team_help` | `true` | `true`/`false` | AI and independent units that are idle or going to a building help a teammate under attack (if they have more than 15 health). | Same |
| `unit_finish_attack_move` | `true` | `true`/`false` | If the target dies or disappears while the unit walks toward it, the unit finishes its step before stopping, instead of stopping halfway. | Same |
| `unit_spy_fixed_target_loyalty` | `false` | `true`/`false` | Original behavior: spies' loyalty is always between 70 and 90, which gave them away. With `false` (default) their loyalty behaves like a normal unit's. | Same |
| `unit_target_move_range_cycle` | `false` | `true`/`false` | Units with several equivalent ranged attacks alternate them when they get in range. | Same |
| `unit_allow_path_power_mode` | `false` | `true`/`false` | Turns on a pathfinding mode that takes the kingdoms' territories into account. It's off by default. | Same |
| `firm_mobilize_civilian_aggressive` | `false` | `true`/`false` | By default, builders and civilian workers who leave a building are in non-aggressive mode (they don't attack on their own). With `true` they leave in aggressive mode. AI units and soldiers with leadership don't change. | Same |

### Bug fixes

| Option | Default | Values | What it does | Mark |
|---|---|---|---|---|
| `fix_path_blocked_by_team` | `true` | `true`/`false` | Better handling when a unit is blocked by an idle teammate of the same group. With `false` they swap their destinations, as in the original. | Same |
| `fix_recruit_dec_loyalty` | `true` | `true`/`false` | Fixes the calculation of the loyalty lost when recruiting without training: the accumulated penalty is counted consistently, and the estimate used by the AI is rounded up. | Same |
| `fix_sea_travel_final_move` | `true` | `true`/`false` | AI units traveling by ship continue to their final destination after landing, instead of staying on the coast. | Same |
| `fix_town_unjob_worker` | `true` | `true`/`false` | When a town needs a worker to leave his job, it doesn't take out spies by mistake and doesn't fail if the worker can't quit. | Same |
| `game_file_patching` | `true` | `true`/`false` | Fixes very old saved games (version below 200): resets the restock type of the player's markets, which wasn't initialized in those versions. | Same |

### Resources and building

| Option | Default | Values | What it does | Mark |
|---|---|---|---|---|
| `mine_unlimited_reserve` | `false` | `true`/`false` | Mines never run out: their reserve never goes below 1,500. | Same |
| `wall_building_allowed` | `false` | `true`/`false` | Turns on the processing of walls on the map (existing walls grow and decay over time). **It doesn't add a button to build them**: the game has no button that gives the order to build walls, so in practice it only affects maps or scenarios that already have them. | Same |

### Speed and notices

| Option | Default | Values | What it does | Mark |
|---|---|---|---|---|
| `game_new_default_frame_speed` | `12` | number | Game speed (frames per second) when starting a new game. | Same |
| `game_load_default_frame_speed` | `9` | number | Game speed when loading a saved game or a scenario. | Same |
| `news_notify_complete` | `false` | `true`/`false` | Shows a news message when a building is finished, a unit finishes training, or a ship or weapon is built. | Local |

---

## 3. Screen, key and network options

| Option | Default | Values | What it does |
|---|---|---|---|
| `vga_full_screen` | `true` | `true`/`false` | Full screen. With `false` the game opens in a window. (The `-win` command line parameter does the same.) |
| `vga_full_screen_desktop` | `true` | `true`/`false` | "Desktop" full screen: uses your monitor's resolution and scales the picture. With `false` it switches the monitor to 800×600, and the window is also fixed at 800×600. |
| `vga_keep_aspect_ratio` | `true` | `true`/`false` | When scaling, keeps the 4:3 ratio with black bars. With `false` it stretches the picture to fill the screen. |
| `vga_window_width` / `vga_window_height` | automatic | number | Window size in window mode. If not given, it's chosen automatically (1024×768 if it fits). Both must be given. |
| `vga_allow_highdpi` | `false` | `true`/`false` | Allows high density display mode (HiDPI monitors / Windows scaling). |
| `vga_pause_on_focus_loss` | `false` | `true`/`false` | Pauses the game when the window loses focus (single player games only). |
| `fix_world_warp_slop` | `2` | number | Margin in pixels from the edge of the screen to scroll the map with the mouse when the mouse is captured by the window. |
| `bindkey` | — | `EVENT,key` | Changes the key of an action. It can be repeated for several actions. Example: `bindkey=KEYEVENT_TOWN_TRAIN,shift+t`. The key uses SDL names (`a`, `F5`, `Space`…) and accepts `shift+`, `ctrl+` or `alt+`. Available events: see the list below. |
| `locale` | (system) | language code | Game language, like `es`. **In the mod's executables it has no effect on the texts**, because they are built without translation support. |
| `mp_service_addr` | `battle.7kfans.com` | address | Server to find multiplayer games over the internet. |
| `remote_compare_random_seed` | `true` | `true`/`false` | In multiplayer, compares every frame the random number generator of all players to detect desync. |
| `remote_compare_object_crc` | `true` | `true`/`false` | In multiplayer, also compares a checksum of the game objects. |
| `scenario_config` | `true` | `true`/`false` | Lets scenarios and tutorials apply their own `config.txt`. |

**Events for `bindkey`:**
`KEYEVENT_FIRM_BUILD`, `KEYEVENT_FIRM_PATROL`, `KEYEVENT_TOWN_RECRUIT`, `KEYEVENT_TOWN_TRAIN`,
`KEYEVENT_UNIT_BUILD`, `KEYEVENT_UNIT_RETURN`, `KEYEVENT_UNIT_SETTLE`, `KEYEVENT_UNIT_UNLOAD`,
`KEYEVENT_UNIT_CANCEL`, `KEYEVENT_BUILD_BASE`, `KEYEVENT_BUILD_CAMP`, `KEYEVENT_BUILD_FACTORY`,
`KEYEVENT_BUILD_HARBOR`, `KEYEVENT_BUILD_INN`, `KEYEVENT_BUILD_MARKET`, `KEYEVENT_BUILD_MINE`,
`KEYEVENT_BUILD_MONSTER`, `KEYEVENT_BUILD_RESEARCH`, `KEYEVENT_BUILD_WAR_FACTORY`,
`KEYEVENT_MAP_MODE_CYCLE`, `KEYEVENT_MAP_MODE0`, `KEYEVENT_MAP_MODE1`, `KEYEVENT_MAP_MODE2`,
`KEYEVENT_REPORT_OPAQUE_TOGGLE`, `KEYEVENT_CLEAR_NEWS`, `KEYEVENT_OPEN_DIPLOMATIC_MSG`,
`KEYEVENT_OPEN_OPTION_MENU`, `KEYEVENT_TUTOR_PREV`, `KEYEVENT_TUTOR_NEXT`, `KEYEVENT_SAVE_GAME`,
`KEYEVENT_LOAD_GAME`, `KEYEVENT_OBJECT_PREV`, `KEYEVENT_OBJECT_NEXT`,
`KEYEVENT_NATION_OBJECT_PREV`, `KEYEVENT_NATION_OBJECT_NEXT`, `KEYEVENT_GOTO_RAW`,
`KEYEVENT_GOTO_KING`, `KEYEVENT_GOTO_GENERAL`, `KEYEVENT_GOTO_SPY`, `KEYEVENT_GOTO_SHIP`,
`KEYEVENT_GOTO_CAMP`, `KEYEVENT_GOTO_SELECTED`, `KEYEVENT_CHEAT_ENABLE1` … `_3`, `KEYEVENT_MANUF_QUEUE_UP`,
`KEYEVENT_MANUF_QUEUE_DOWN`, `KEYEVENT_MANUF_QUEUE_ADD`, `KEYEVENT_MANUF_QUEUE_ADD_BATCH`,
`KEYEVENT_MANUF_QUEUE_REMOVE`, `KEYEVENT_MANUF_QUEUE_REMOVE_BATCH`,
`KEYEVENT_SEL_GROUP_1` … `KEYEVENT_SEL_GROUP_9`, `KEYEVENT_SET_GROUP_1` … `KEYEVENT_SET_GROUP_9`.

The key of each action and what it does are in `KEYBINDINGS.en.md`.

---

## 4. Special files

Besides `config.txt`, the code checks whether certain files exist (empty ones work) in the
folder the game is run from:

- `SYNC1.SYS` / `SYNC2.SYS`: turn on the multiplayer sync checks, just like `remote_compare_random_seed` and `remote_compare_object_crc`.
- `TECHGOD.SYS`: in multiplayer only, human kingdoms start with 3 extra levels in all technologies and with all the scrolls. It's a testing tool of the original developers; it isn't checked between players.
