# 7k-mod-IA changelog (experimental)

**English** | [Español](CHANGELOG-ia.md)

Experimental branch to test changes to the AI of the computer kingdoms. It's based on
`7kmod-pathing` and includes everything in that branch, in `7kmod-gameplay` and in
`7kmod` (see `CHANGELOG-pathing.en.md`, `CHANGELOG-gameplay.en.md` and
`CHANGELOG.en.md`). Its executable is `7kaa-ia.exe`.

If the changes work well in games, they will be moved to `7kmod-gameplay`.

This file is the English translation of `CHANGELOG-ia.md`; both are updated together.

---

## [Unreleased]

---

## [0.1.0] - 2026-11-06

First released version. Based on **7kmod-pathing 0.7.1** (gameplay 0.2.0, 7kmod 0.3.0).

### Added
- **Its own folder:** `7kaa-ia.exe` keeps its saved games, `CONFIG.DAT` and `config.txt`
  in `%APPDATA%\7kfans.com\7kaa-ia\`, apart from the mod's other executables
  (`7kaa-mod`), so its `config.txt` and saved games don't get mixed with theirs.
- **AI type (`ai_type` in `config.txt`):** `mixed` (default in this branch), `7kmod` or
  `original`.
  - `original`: the game's AI.
  - `7kmod`: all AI kingdoms use the experimental AI.
  - `mixed`: kingdoms with an even number use it (with the player as kingdom 1, half of
    the AI kingdoms), to compare it with the original in the same game.
  - In the kingdoms report, those that use it have "(7kmod)" next to their name (only if
    the option isn't `original`). Long names are cut off.
  - It's saved in the game: when it's loaded, the value it was saved with is used (and
    stays that way until the game is restarted). In multiplayer it's compared between
    players (like the options marked Same in `ADVANCED_CONFIG.en.md`).
- **Experimental AI, villager rule:** the towns of a kingdom with the experimental AI
  don't give people (automatic hiring by buildings, fort recruiting, AI orders, training
  units) if the kingdom would be left with less than 30% jobless villagers among all its
  people: towns, soldiers in forts and Seats, and units (the same ones that eat). The
  first 8 workers of each town and the first 8 people of the kingdom outside the towns
  are always allowed (`Town::mod_ai_keep_peasants`). It's so the AI doesn't run out of
  food: only jobless villagers produce (30 a year) and everyone eats (10).

- **Per-kingdom limits** (`config.txt`, saved in the game; 0 = no limit):
  - `nation_max_town_population` (**600** by default): the people living in the
    kingdom's towns. When it's reached, there are no births in its towns. Conquered
    towns, towns that join, and people who arrive at or return to the towns can go over
    it.
  - `nation_max_military_units` (**300** by default): soldiers and generals with
    leadership (not the king or spies), siege weapons, warships (Caravel and Galleon)
    and the kingdom's Fryhtans, also those being trained or built. When it's reached,
    Leadership (or Promote) can't be trained, weapons or warships can't be built,
    Fryhtans can't be invoked and mercenaries with leadership can't be hired: the queues
    wait, the buttons are disabled with the reason in their help, and a message appears
    when you try. Units gained by capture, surrender or conversion can go over it.
  - The AI follows the same limits.
  - In the kingdoms report (Information), "Your Population" and "Your Military Units"
    are shown with their limit.

- **Tax per person** (`town_tax_per_person` in `config.txt`, **5** by default, as in the
  original; saved in the game): what each person gives when collecting taxes in a town
  and in the automatic yearly tax (`Town::collect_tax`, `Town::collect_yearly_tax`).

### Changed
- **Soldiers are trained, no longer recruited** (for everyone: the player and all AIs).
  - Only people with the leadership profession (soldiers, generals, kings) and spies go
    into a fort. Villagers and workers with another profession don't: they have to be
    trained in Leadership in a town (Train, $30 and 5 days) (`Unit::firm_can_assign`).
  - Forts no longer pull villagers from towns: neither with the click on the town-fort
    link nor by the AI (`Firm::pull_town_people`).
  - In the mode for recruiting from a fort, **Recruit** is disabled; **Train** still
    works and the trained soldier goes into the fort by himself. Recruit in the town's
    normal mode still takes out villagers.
  - Weapons, Fryhtans and what is placed with the editor don't change.
  - **The AI** fills its forts by training soldiers in its towns (the order to take
    workers to a fort no longer takes out villagers: it trains them,
    `Nation::ai_assign_worker`, `FirmCamp::ai_recruit`). It trains one at a time per
    town, and follows the experimental AI's villager rule.

### Compatibility
- **Saved games:** version 303: 301 saves `ai_type` (`GAME_VERSION_7KMOD_AI_TYPE`), 302
  the per-kingdom limits (`GAME_VERSION_7KMOD_NATION_LIMITS`) and 303 the tax per person
  (`GAME_VERSION_7KMOD_TOWN_TAX`). Older saved games use the values of `config.txt`. It
  loads gameplay and pathing saved games (version 300 or older) with the original AI.
  The mod's other executables don't load this branch's saved games.
