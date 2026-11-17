# 7kmod-fryhtan changelog (experimental)

**English** | [Español](CHANGELOG-fryhtan.md)

Experimental branch to play as a **Fryhtan kingdom**. It's based on `7kmod-gameplay` and
includes everything in that branch and in `7kmod` (see `CHANGELOG-gameplay.en.md` and
`CHANGELOG.en.md`). Its executable is `7kaa-fryhtan.exe`.

This file is the English translation of `CHANGELOG-fryhtan.md`; both are updated
together.

---

## [Unreleased]

### Added
- **Life Points:** the currency of Fryhtan kingdoms. They show on the top bar, in place
  of the reputation, with their own icon (a green and gold yin-yang sphere).
  - You start with 150, 300, 500 or 700 by the Treasure of New Game (Small to Very
    Large).
  - **Earned by killing:** a soldier gives its combat level / 10 (1 with combat 10, 10
    with combat 100); a villager or town defender half that; a general x1.25; a king
    x1.5; always at least 1. A weapon gives 3 and a ship 5. Enemy Fryhtans count as
    soldiers. Villagers killed in an attacked town, by Cull or by Taskmaster give 1
    each.
  - **Tribute of life:** each enslaved village gives 1 a month for every 10 villagers,
    with the gold tribute.
  - **Spent on Fryhtans**, besides the gold and food: training a soldier costs 10, an
    Ordo 15, and promoting a soldier to Ordo 5. Cancelling a training gives them back.
  - Next to the number, their change over the last 365 days, as for food and cash:
    "302 (+12)".
  - Training costs depend on the species: 7 (Ick, Doink, Broosken), 8 (Deezboanz,
    Rattus, Pfith), 9 (Karrotten, Holgh, Droog) or 10 (the others). An Ordo costs 5
    more.
  - Building a lair costs 30 (besides the $500); the starting lair doesn't.
  - The tribute of life gives 1 more if the village has 15 villagers or more.
- **Lair technologies:** the **Research** button (second place of the second page of the
  lair's buttons) opens the list of the kingdom's technologies. Choosing one asks to
  confirm, with its cost in gold and Life Points. The progress belongs to the kingdom:
  it goes on while there is any lair.
  - **Alchemist Lair** ($1,000 and 30 Life Points, 6 months): lairs can be built on raw
    material sites. With an Ordo or the king, every 15 days they turn 50 of the site's
    raw material into 5 Life Points. On the map they read "(Alchemist Lair)", and the ">"
    button of their panel shows the raw material left, the conversion and the next one.
  - **Empowerment** ($1,000 and 50 Life Points, 6 months): Fryhtans trained in the lairs
    get +10 combat (soldier 30, Ordo 40).
  - The menu is like the Tower of Science's: the list of the technologies left, the
    confirmation with its cost and the progress screen (with Change and Back). The cost
    is paid once, to unlock it; you can switch technologies and each keeps its progress.
  - A finished research shows a message and plays the "research complete" sound.
- **Lair places by species:** species with many hit points fit fewer: Rokken 4; Wyrm,
  Droog, Haubudam and Sauroid 5; Holgh, Karrotten and Pfith 6; the others 8. A full lair
  has the hit points of a full human fort.
- **Portrait of ranked Fryhtans:** generals, Ordos and the king have the soldier
  portrait of their species flipped, to tell them apart.
- **Games with a Fryhtan kingdom:** all kingdoms start on the largest land, as Fryhtans
  can't move by sea. Land Mass must be at least medium: choosing Fryhtans in New Game
  moves Small to Medium.
- **Overpopulation:** an enslaved town with 30 people or more loses 1 loyalty every 5
  days and rebels in the end unless it drops below 30.
- **Cull Population** (sword button in the enslaved town, only with an Ordo inside):
  kills 3 or 4 villagers and gives $2 and 5 food for each one; it needs at least 5
  people. Its help: "Large villages tend to rebel. Cull them when their population gets
  too big (30 or more)."
- **Subjugate** culls on its own when the town reaches 30 people, to prevent rebellions
  (with spoils, without news). It checks every 5 days.
- **Who destroys the last linked lair** of an enslaved town decides what happens:
  - **rebels:** the town declares its independence at once and the rebels nearby go
    back into it as villagers (it stays a normal independent town, not one controlled
    by rebels);
  - **an enemy kingdom:** the town goes to that kingdom at once;
  - **wild Fryhtans**, selling it or anything else: the usual 3-month grace.
- **Grace without rebellion:** an enslaved town that goes to a human kingdom can't rebel
  for 3 months.
- **Nobody migrates on their own to an enslaved town**, neither villagers of other towns
  nor workers of buildings. The migration the player orders between their own towns is
  unchanged.

### Fixed
- **Lair places:** with the soldiers outside (for example, after sending them to fight)
  training was sometimes blocked although there was room: it also counted the leader
  and the ones that died or went into another lair. Now it counts what the darkened
  places show.
- **Selecting a Fort or a lair again** opens it on its main menu and the first page of
  buttons; it used to stay on the menu (research, alchemist detail) or page you left.
- **Music when loading a Fryhtan game:** the menu's music kept playing, because the
  Fryhtan king has no race. Now it plays the same as a new game.

### Changed
- **Rokken and Holgh** move at speed 4 (was 3), like the others.
- **Gold price:** all small Fryhtans cost $100 and all large ones $150.
- **Build lair button:** it shows the lair of the builder's species on the stone of the
  build buttons (was the human fort with the Fryhtan's portrait). The Alchemist Lair icon
  is that lair with the mine in small.
- **Full lair:** you can train even without room; the new soldier stays outside. The
  places the species doesn't have read "This lair only has N places.".
- **AI against Fryhtan kingdoms:** an AI at war with a Fryhtan kingdom that hasn't
  attacked it for a while relaxes its conditions. After 6 months it asks half the
  surplus of troops to go to war and goes with 110% of the lair's defense (was
  125–150%); after a year it asks no surplus (its towns stay protected) and goes with
  100%. Attacking starts the count again.
- **More aggressive AI against Fryhtan kingdoms (test):** when picking whom to declare
  war on, a human AI sees a Fryhtan kingdom as if it had 70% of its strength, so it
  prefers it as a target; not if the Fryhtan kingdom is over 1.5 times as strong as it.
- **Taskmaster** no longer gives gold: only food, doubled (+10 to +40 a year per
  villager by leadership). The forced labor kills a villager every 30 to 90 days.

---

## [0.1.0] - 2026-11-14

Included in **New Adversaries 0.4.0** (gameplay 0.3.0, 7kmod 0.4.0). The
`7kmod-fryhtan` branch goes on with the experimental adjustments of the Fryhtan kingdom.

### Added
- **Fryhtan kingdom** (phase 1; single player games only):
  - In New Game, basic page, two text tabs above the races: **Humans** (the 10 races)
    and **Fryhtans** (the 13 species in the same place, 10 per page, with ▲ ▼
    arrows to change the page); the chosen one is underlined. Below the
    name it shows how the kingdom will be called, for example "Fryhtan kingdom: Al
    Rokken Horde". The tab and the species are remembered.
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
  - **Promote / Demote:** a Fryhtan soldier is promoted to **Ordo** (general) with
    Promote and gets a **name of its own** (for example "Kzorr"); it keeps its combat and
    leadership. Demote turns it back into a soldier. Soldiers start with **leadership 20**
    (shown in their panel) and learn it in the lair as humans do (only those with
    potential).
  - An Ordo's leadership rises **×1.5** as fast as a human general's.
  - **Train Ordo** in the lair: an Ordo for the same gold as a soldier of its species and
    40 food, in 15 or 10 days, with leadership and combat 30.
  - **Price by species** (soldier and Ordo): **$100** Ick, Broosken and Doink; **$150**
    Deezboanz, Rattus and Pfith (Pfith counts as small: population 2 and 10 days);
    **$200** the large ones.
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
    a minimum that depends on the general's leadership: **60** with leadership 30 or less
    down to **45** with 100 (the best lair counts). The leadership also decides how fast. Taking it to 0 requires fighting.
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
- **Ordo inside an enslaved town:** right clicking an enslaved town sends an Ordo into
  it (one per town). The town's spy button (the eye) opens its view, where the **Role**
  row with the **>** button switches between three actions:
  - **Subjugate:** the loyalty doesn't drop below the Ordo's leadership (50 at most).
  - **Kidnap:** **2 new villagers** of the town's races arrive, on average every 8
    months with leadership 30 and every 4 with 100 (never over 12 months), with the news
    "Your Ordo … has kidnapped 2 villagers into your village of …".
  - **Taskmaster:** each villager gives +$0.5 to +$2 more a month and +5 to +20 more food
    a year, by the leadership.
  - Inside, the Ordo **gains leadership and combat** as a general in its lair (every 5
    villagers count as one soldier, up to 8) and recovers hit points.
  - **Leave village** takes it out. If the town changes owner, the Ordo comes out.
  - With an Ordo selected, the enter arrow shows over the town.
  - The town's name on the map and in the Villages report shows the role, for example
    "Rouen (Subjugate)".
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
  - **Spoils:** killing a human unit gives gold by its **combat level**: a soldier $2
    with combat 10 up to **$45** with combat 100 (in a straight line); a town defender
    half; a general ×3 ($135 with combat 100); a king ×22 (**$990** with combat 100), and
    the next kings of the same kingdom half. A villager gives $2. The food is fixed: 5
    (soldier, villager or defender), 20 (general) or 100 (king). In the Economy report it
    shows as "Spoils of War and Treasure".
  - **Human buildings** that pass to the Fryhtan kingdom (for example, if a kingdom
    surrenders to it) are **destroyed**, with whoever is inside.

### Compatibility
- **Its own folder:** `%APPDATA%\7kfans.com\7kaa-fryhtan\` (saved games, `CONFIG.DAT`,
  `7KMOD.DAT` and `config.txt`), apart from the mod's other executables.
- **Saved games:** version 310 (`GAME_VERSION_7KMOD_FRYHTAN`): it saves the species of
  each kingdom, the species of each lair and its Train queue. It loads gameplay saved games (version 300 or older). The mod's other
  executables don't load this branch's saved games.
- **Multiplayer:** the Fryhtan option doesn't exist in multiplayer.
