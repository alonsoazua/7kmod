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
- **Lairs** (phase 2):
  - The Fryhtan king and generals have **Build → Lair**: a lair of their species that
    works like a fort (one general and up to 8 soldiers). It costs **$500** and **$100
    a year**.
  - **Train** button in the lair: a Fryhtan of the lair's species for **20 food**, in
    **20 days**, with combat 20. Queue of up to 8; right click removes one and gives the
    food back. It can't go over the population cap.
  - **Fryhtan population:** each Fryhtan of the kingdom, king included, uses 1 (small)
    or 2 (large). The cap is 18. It shows in the Train help and in the Kingdoms report
    ("Fryhtan Population").
  - **Promote:** a Fryhtan soldier can be promoted to general (Ordo) with leadership 20.
  - Fryhtans train their combat in the lair with the general and gain experience when
    fighting.
  - The lair has no Reward or Recruit; its Fryhtans show loyalty "N/A".
- **Economy** (phase 3):
  - **Spoils:** killing a human unit gives **$5 and 5 food** (soldier, villager or town
    defender), **$20 and 20** (general) or **$100 and 100** (king). In the Economy
    report it shows as "Spoils of War and Treasure".
  - **Human buildings** that pass to the Fryhtan kingdom (for example, if a kingdom
    surrenders to it) are **destroyed**, with whoever is inside.

### Compatibility
- **Its own folder:** `%APPDATA%\7kfans.com\7kaa-fryhtan\` (saved games, `CONFIG.DAT`,
  `7KMOD.DAT` and `config.txt`), apart from the mod's other executables.
- **Saved games:** version 310 (`GAME_VERSION_7KMOD_FRYHTAN`): it saves the species of
  each kingdom, the species of each lair and its Train queue. It loads gameplay saved games (version 300 or older). The mod's other
  executables don't load this branch's saved games.
- **Multiplayer:** the Fryhtan option doesn't exist in multiplayer.
