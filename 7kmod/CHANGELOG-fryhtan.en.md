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
  - In New Game, basic page, two text tabs above the races: **Humans** (the 10 races)
    and **Fryhtans** (the 13 species in the same place, 10 per page, with ▲ ▼
    arrows to change the page); the chosen one is underlined. Below the
    name it shows how the kingdom will be called, for example "Fryhtan kingdom: Al
    Rokken Horde (Large: 2 population)". The tab and the species are remembered.
  - The species names, the tabs and the "Seats of Power Unlocked?" label use smooth
    anti-aliased text (TeX Gyre Pagella, a Palatino clone), pre-rendered with
    `7kmod/tools/make_smooth_text.py`.
  - The kingdom starts with its king, an All High of the species with combat
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
  - **Train Ordo:** a second button trains a general (Ordo) of the lair's species: **40
    food**, **40 days**, leadership and combat **30**. It comes out next to the lair
    with a **name of its own** (for example "Kzorr"). Soldiers can no longer be promoted
    (there is no Promote or Demote).
  - Fryhtans train their combat in the lair with the general and gain experience when
    fighting.
  - The lair has no Reward or Recruit; its Fryhtans show loyalty "N/A".
- **Start:** the kingdom starts with the king as general of an already built lair and 4
  soldiers of its species.
- **The king** is called "<name> The All High" in his panel and in the lair's (for
  example "Al The All High"), over "All High <species>".
- The king and the Ordos have the **king or general icon** over the unit.
- In the **saved game list**, a Fryhtan game shows the portrait of the All High of its
  species and "<name> The All High".
- A Fryhtan kingdom **can't grant money** to independent towns.
- **Progress bar** of the Fryhtan in training on the Train button. With the king out,
  the lair says "Outside of the lair".
- **Enslaved towns** (phase 4):
  - A lair with a general linked to an independent town lowers its resistance down to
    **45**; the general's leadership decides how fast. Taking it to 0 requires fighting.
  - A town of a human kingdom is enslaved by fighting the loyalty of its people down to
    0.
  - In both cases a lair linked to the town is needed.
  - The enslaved town doesn't recruit, train or pay taxes: it pays a **tribute of $5 per
    person a month** ("Tribute from Enslaved Towns"), which goes up with the leadership
    of the best general of the linked lairs (double with leadership 100). It produces food, grows, doesn't
    rebel and its people don't leave.
  - Each enslaved person raises the Fryhtan **population cap** by 1.
  - If it loses the link with all the lairs, it becomes independent.
  - Destroying or selling a lair no longer shows a human Fort.
- **Closing** (phase 7):
  - **Succession:** if the All High dies, the Ordo with the best combat becomes the new
    All High (without Ordos, the Fryhtan with the best combat) and the kingdom takes
    his name ("Zhovrek The All High", "Zhovrek Wyrm Horde").
  - **Defeat:** the kingdom only loses when it has no lairs and no Fryhtans left.
  - End of game with "Your Horde has …" texts and its own pictures (the Fryhtans if you
    win, the humans celebrating if you lose). The Hall of Fame shows "<name> The All
    High".
  - Reports: "Lair" with its cost in Villages, "In Lair" in Military, and the Fryhtans
    count in the population score of the Ranking.
  - When placing a lair, the **lines to the wild lairs** it would be linked to are
    shown.
- **Diplomacy and wild Fryhtans** (phase 6):
  - With a Fryhtan kingdom there is no trade, friendship, alliance, embargo, food
    purchase or surrender. War, ceasefire, tribute, aid and technology are allowed.
  - AI kingdoms don't surrender to a Fryhtan kingdom.
  - **Wild Fryhtans don't attack** the Fryhtan kingdom (they do defend if you attack
    them).
- **Enslaved towns without a general:** if no linked lair has an Ordo or the king
  inside, the town pays no tribute and its loyalty drops. If it reaches the rebellion
  point, the rebels go out to attack the nearest lair.
- **Capturing wild lairs** (phase 5):
  - A wild lair of any species within 8 tiles of a lair of the kingdom shows an
    **offer**: an Ordo with the combat of the lair's best Ordo and leadership 30–60, and
    4–6 soldiers (large species) or 6–8 (small) with combat 20–40.
  - The base tribute is the sum of the combat levels × $10, adjusted by the leadership
    of the best general of your linked lairs: ×1 with leadership 100 and ×3 with 30 or
    less. Without an Ordo or the All High in a linked lair it can't be paid.
  - With **Pay Tribute** the lair passes to the kingdom with that Ordo and those
    soldiers; the wild Fryhtans inside leave and the ones outside stay wild. The
    population cap can be exceeded.
  - A **link line** joins your lairs with the linked wild lairs.
  - Each captured lair trains its own species.
- **Economy** (phase 3):
  - **Spoils:** killing a human unit gives **$2 and 5 food** (soldier, villager or town
    defender), **$10 and 20** (general) or **$100 and 100** (king). In the Economy
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
