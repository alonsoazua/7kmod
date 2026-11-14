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
    Rokken Horde (Large: 3 population)". The tab and the species are remembered.
  - The species names, the tabs and the "Seats of Power Unlocked?" label use smooth
    anti-aliased text (TeX Gyre Pagella, a Palatino clone), pre-rendered with
    `7kmod/tools/make_smooth_text.py`.
  - The kingdom starts with its king, an All High of the species with combat
    60 and leadership 100.
  - It's called "<name> <species> Horde", also in the news.
  - **No reputation** (it stays at 0) and its Fryhtans have **no loyalty**.
  - **No human buildings:** its units have no build menu.
  - Villagers of independent towns don't join a Fryhtan kingdom.
- **Lairs** (phase 2):
  - The Fryhtan king and generals have **Build → Lair**: a lair of their species that
    works like a fort (one general and up to 8 soldiers). It costs **$500** and **$100
    a year**.
  - **Train** button in the lair: a Fryhtan of the lair's species for **$200** (**$150**
    for a small species) and **20 food**, in **15 days** (10 for a small species), with
    combat 20. Queue of up to 8; right click removes one and gives the gold and food back.
  - **Fryhtan population** (being tested, **no limit**): each Fryhtan of the kingdom, king
    included, uses 2 (small) or 3 (large), and the Kingdoms report shows how many there
    are ("Fryhtan Population"). The cap (base 20 plus the enslaved people) is turned off
    in the code (`FRYHTAN_POPULATION_LIMIT`); instead, training costs gold.
  - **Train Ordo:** a second button trains a general (Ordo) of the lair's species:
    **$200** (**$150** for a small species) and **40 food**, **30 days** (20 for a small
    species), leadership and combat **30**. It comes out next to the lair
    with a **name of its own** (for example "Kzorr"). Soldiers can no longer be promoted
    (there is no Promote or Demote).
  - Fryhtans train their combat in the lair with the general and gain experience when
    fighting.
  - The lair has no Reward or Recruit; its Fryhtans show loyalty "N/A".
- **Lairs by species:** the lair an Ordo builds is of that Ordo's species (for example,
  the one of a captured wild lair), and Train trains the lair's species.
- There are **no wild lairs of the player's species** on the map.
- When a Fryhtan finishes training, the **"ready"** sound of the human units plays.
- **Start:** the kingdom starts with the king as general of an already built lair and 4
  soldiers of its species, and with an **enslaved town of 20 people** of a random race,
  linked to the lair (where a human kingdom has its starting town). With the king in
  the lair, it pays tribute from the first month; the population cap starts at 40.
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
  - The enslaved town doesn't recruit, train or pay taxes: it pays a **tribute of $3 per
    person a month** ("Tribute from Enslaved Towns"), which goes up with the leadership
    of the best general of the linked lairs (double with leadership 100). It produces food, grows, doesn't
    rebel and its people don't leave.
  - Each enslaved person raises the Fryhtan **population cap** by 1.
  - **Grace period:** during the first month after it's enslaved, the town can't rebel,
    so that the general has time to go back to the lair (the loyalty starts at 30 and
    drops without a general).
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
- **Looting towns:** when attacking a town, each villager that dies gives $2 and 5 food,
  like killing a villager.
- **Raiding independent towns:** on a town linked to a lair with a general, the **Raid**
  button (Collect Tax icon, with a waiting bar; dark and with the reason in its help when
  it can't be done) opens a view with the main race, the population, the resistance,
  what the raid gives and the defenders, with the **Raid village** and **Cancel**
  buttons. Once every 2 months per town:
  - It steals **$2 and 2 food per person** (up to 60, that is 30 people), times
    (100 + leadership) / 100 of the general of the lair that raids: double with
    leadership 100. It lowers the resistance by 5.
  - Up to 1 defender per 10 people plus 1 comes out (+1 with a resistance over 50, +2
    over 80) and attacks the lair of the best general. The first one is the leader:
    combat +20, and it carries twice the resistance. Each defender killed lowers the
    resistance of the town; the ones that go back return it.
  - With a resistance of 30 or less it can't be done: "This village is weakened and
    ready to be attacked directly."
  - For now it doesn't kill villagers.
  - A news message with the loot and the defenders.
- The **starting All High** has leadership **100** (60 before).
- **The kingdom's buildings repair themselves**, as if they had a construction worker
  of skill 20: 1 hit point every 5 days, except while under attack or with negative
  gold.
- **No human unit joins the Fryhtan kingdom:**
  - Human spies can't cloak as a Fryhtan kingdom (neither in their menu nor the AI's,
    which doesn't send them to enslaved towns either). Before, an AI spy came out of a
    town cloaked as your kingdom with the message that a unit had joined.
  - An enemy spy who was in a town you enslaved leaves it as an independent unit, and if
    he stops being a spy he stays as a villager.
  - Betraying units, leaderless rebels and propaganda villagers don't go over to a
    Fryhtan kingdom, and the villagers of an enslaved town don't hand it a human
    building (capture by workers).
- **Fixed:** when an Ordo died, the news showed garbage text instead of the name. Now
  it says "Your Ordo, <name>, has been slain."; the name in the assassination, betrayal
  and honors news was fixed too.
- **Enslaved towns without a lair:** an enslaved town left without any linked lair
  stays in the kingdom for **3 months** (no tribute, no rebellion); if it's linked to a
  lair again in that time, it stays enslaved. Then it becomes independent with the news
  "Your village of … has declared independence."
- **Fixed:** a town that became independent surrendered at once to another kingdom,
  because it kept an old zero resistance toward it. Now it gets the resistance of a new
  independent town (60 to 100) toward every kingdom, and 100 toward the Fryhtan kingdom.
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
