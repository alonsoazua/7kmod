# 7kmod-fryhtan changelog (experimental)

**English** | [Español](CHANGELOG-fryhtan.md)

Experimental branch to play as a **Fryhtan kingdom**. It's based on `7kmod-gameplay` and
includes everything in that branch and in `7kmod` (see `CHANGELOG-gameplay.en.md` and
`CHANGELOG.en.md`). Its executable is `7kaa-fryhtan.exe`. The plan and the details of
each change are in `TODO-fryhtan.md` (Spanish only).

This file is the English translation of `CHANGELOG-fryhtan.md`; both are updated
together.

---

## [Unreleased]

Version **0.1.0** in development, based on **7kmod-gameplay 0.2.0** (7kmod 0.3.0).

### Added
- **Fryhtan kingdom** (phase 1 of `TODO-fryhtan.md`; single player games only):
  - In New Game, basic page, the **Fryhtan** button (right of Maya) and the species
    selector **< Rokken >** (right of Zulu), with the 13 species. Below the name it shows
    how the kingdom will be called, for example "Fryhtan kingdom: Al Rokken Horde
    (Large: 2 population)". Choosing a race goes back to a human kingdom.
  - The kingdom starts **with its king only**, an All High of the species with combat
    60 and leadership 60.
  - It's called "<name> <species> Horde", also in the news.
  - **No reputation** (it stays at 0) and its Fryhtans have **no loyalty**.
  - **No human buildings:** its units have no build menu.
  - Villagers of independent towns don't join a Fryhtan kingdom.
  - For now the kingdom can't do anything else: lairs come in phase 2.

### Compatibility
- **Its own folder:** `%APPDATA%\7kfans.com\7kaa-fryhtan\` (saved games, `CONFIG.DAT`,
  `7KMOD.DAT` and `config.txt`), apart from the mod's other executables.
- **Saved games:** version 310 (`GAME_VERSION_7KMOD_FRYHTAN`): it saves the species of
  each kingdom. It loads gameplay saved games (version 300 or older). The mod's other
  executables don't load this branch's saved games.
- **Multiplayer:** the Fryhtan option doesn't exist in multiplayer.
