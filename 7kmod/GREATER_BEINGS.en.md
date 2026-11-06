# Greater Beings

**English** | [Español](GREATER_BEINGS.md)

How the Greater Beings of Seven Kingdoms: Ancient Adversaries 2.15.7 work, based on the
source code (`OU_GOD.cpp`, `OGODRES.cpp`, `OF_BASE.cpp`, `OU_MONS.cpp`, `OSITE.cpp`) and
the game data (the `GOD`, `UNIT` and `UNITATTK` tables of `RESOURCE/STD.SET`).

**Changes of the mod:**
- **7kmod:** the "Seats of Power Unlocked?" option (how the scroll is obtained at the
  start) and the help of the power buttons of the Djini, Isis and uNkulunkulu, which had
  none.
- **7kmod-gameplay:** the Norman Dragon throws a fireball with a button instead of
  attacking; the Chinese Jing Nung attacks at half the cost; the Mughul Djini also
  converts a soldier; the Greek Phoenix costs less and has a power to expose spies. They
  are marked with *(gameplay)* below. In `7kmod` all four work as in the original.

---

## 1. How they are obtained

### The scroll (Scroll of Power)
- Each race has a Greater Being. To invoke it, your kingdom needs the **scroll of that race**, which allows building its **Seat of Power**.
- **When a Fryhtan king is killed** (the monster of king rank), a scroll is left on the ground near where it died.
  - The scroll's race is that of the most numerous units within 10 tiles of the king. If there are no units with a race nearby (for example, only siege weapons), the race is random.
  - It can only be picked up by a unit **of the same race as the scroll**. When it's picked up, its kingdom learns to build that Seat of Power and a news message appears.
  - Killing a Fryhtan general leaves gold, not scrolls.
- **At the start of the game:**
  - With the 7kmod option **Seats of Power Unlocked?** = Yes, the player's kingdom starts with the scroll of its race (single player games only).
  - The original game also has the advanced setting `nation_start_god_level` in `config.txt`: 0 = none (default), 1 = each kingdom starts with the scroll of its race, 2 = all the scrolls. This setting applies to **all** kingdoms, including the AI. It isn't synced in multiplayer: all players must have the same value or the game may go out of sync. See `ADVANCED_CONFIG.en.md`.

### The Seat of Power
- It only accepts an overseer and workers (priests) **of the Seat's race**.
- **Prayer points:**
  - They build up every day, only if there is an overseer: **productivity / 100** per day. With "fast build" it's productivity / 10.
  - Productivity is the sum of the workers' skill, adjusted by their health, divided by 8. With 8 priests of skill 100 at full health it's 100, that is **1 point per day**.
  - The overseer slowly raises the workers' skill, as in the fort.
  - Maximum: **400 points**.
- **Invoke:**
  - It requires at least **40 points** (10% of the maximum) and that the Seat doesn't already have an invoked being. **Each Seat keeps only one being at a time**; to have several you need several Seats.

### Rules common to all beings
- **Their health is their Seat's prayer points.** If the Seat loses points (for existing, using powers or attacking), the being loses health.
- **Damage doesn't add up.** Taking damage doesn't take points from the Seat: on every game frame the being's health is set back to the Seat's points. It only dies from damage if, at a single moment, it takes as much damage as it has points. In practice it's almost invulnerable while it has many points and fragile when it has few left (for example, right after using its power).
- **Existing costs points every day.** The cost depends on the being (see the table). If the points reach 0, the being dies.
- **If the Seat is destroyed, the being dies instantly.** If the Seat changes owner, so does the being.
- **They all fly** and have armor 8.
- **Powers:** each use costs the points shown in the table, and the button is only enabled if the being has at least that many points (health).
- **Attack:** only the two dragons attack. The others can't attack and only move or use their power. *(gameplay)*: only the Chinese Jing Nung attacks; the Norman Dragon uses its fireball with a button.

---

## 2. Summary

| Race | Being | Type | Cost per day | Cost per use | Power range | Sight |
|---|---|---|---|---|---|---|
| Norman | Dragon | Attack (fire). *(gameplay)*: fireball with a button | 1 | 10 per attack. *(gameplay)*: 100 per fireball | — *(gameplay: area of radius 4)* | 9 |
| Chinese | Jing Nung | Attack (3 attacks) | 1.5 | 10 per attack. *(gameplay)*: 5 | — | 9 |
| Greek | Phoenix | Scout, reveals information. *(gameplay)*: power to expose spies | 2.5. *(gameplay)*: 1.5 | — *(gameplay: 100)* | — *(gameplay: one of your towns)* | 10 |
| Mayan | Kukulcan | Power: more combat | 1.5 | 100 | Area (radius 5) | 9 |
| Persian | Lord of Healing | Power: heal | 1.5 | 100 | Area (radius 5) | 9 |
| Japanese | Mind Turner | Power: lower loyalty | 1.5 | 100 | Area (radius 5) | 9 |
| Indian | Djini | Power: lower loyalty. *(gameplay)*: and convert a soldier | 1.5 | 100 | Area (radius 5) | 10 |
| Viking | Thor | Powers: rain and tornado | 1.5 | 100 | Map / in front of Thor | 9 |
| Egyptian | Isis | Power: more population | 1.5 | 100 | One tile (town) | 9 |
| Zulu | uNkulunkulu | Power: more leadership | 1.5 | 100 | One tile | 9 |

**Notes on the table:**
- The "cost per day" comes from the `GOD` table (existence points ÷ 20).
- With 400 points and no praying, a being with cost 1.5 lasts about 266 days; the Phoenix, about 160. A Seat with good productivity (up to 1 point per day) makes up for part of the cost.
- **Area of radius 5:** a zone of about 9×9 tiles around the chosen point, trimmed into a flattened diamond shape (the top and bottom rows are shorter).
- **Effect on buildings:** the effect reaches the overseer and the workers who live inside (not those of buildings whose workers live in towns). It's split among the tiles the building occupies, so a building fully inside the area gets about the whole effect; one partly inside, only a part.
- **Ships:** the units inside your ships get the effect of friendly powers, divided by the size of the ship.

---

## 3. Each being in detail

### Normans — Dragon
- **What it does:** attacks with fire at range.
- **Attack:** base damage 15, range 6 tiles, delay between attacks 6.
- **Cost:** each attack uses 10 prayer points, which are also its health; attacking a lot weakens it.
- **Use:** the strongest direct combat unit early on, but it uses up its points quickly.

- *(gameplay)* **Fireball with a button:** it no longer attacks. Its power button (the
  icon is its fireball) lets you choose a point on the map; it flies there and throws the
  fireball.
  - **Cost:** 100 points per fireball.
  - **Base damage 150** (its attack had 15), in a radius of 109 pixels (about 3.4 tiles;
    45 before) that goes down from 100% at the center to 50% at the edge: against armor
    4, about 37 health at the center, 31 at one tile, 26 at two and 20 at three. It only
    damages kingdoms you are at war with. Buildings take the damage ÷3, once each.
  - **Fire** on the tiles within 5 of the center (3 before).
  - The AI throws it when there are 5 or more enemy units together.

### Chinese — Jing Nung (Chinese dragon)
- **What it does:** attacks at range with a sequence of 3 attacks.
- **Attack:** three attacks of base damage 18 each, range 6.
- **Cost:** 10 points for each attack.
- **Use:** more damage per sequence than the Norman Dragon, at the same cost per attack.

- *(gameplay)* **Cost:** 5 points per attack: with the same points it attacks twice as much.

### Greeks — Phoenix
- **What it does:** it doesn't attack and has no powers. It's a flying scout with the **highest sight (10)** of all the beings.
- **Reveals information:** everything within 10 tiles of the Phoenix shows its full information when selected, as if you had a spy inside:
  - **Enemy units:** their information panel (combat, skill, loyalty) and their real profession.
  - **Enemy buildings:** the inside panel, with the list of workers and their levels, production, research, etc.
- **It doesn't detect spies.** The original code had a function to mark enemy spies with skill below 40 inside your buildings when the Phoenix flew over them, but it's disabled (commented out) in the code.
- **Cost:** it's the most expensive to keep (2.5 per day). *(gameplay)*: 1.5 per day.
- *(gameplay)* **Power: expose spies.** Power button (the spies' eye icon): you choose a
  **town of your kingdom** and the Phoenix flies to it. If there are spies of other
  kingdoms, it **exposes and executes** the one with the lowest spying skill, with the
  normal news message.
  - **Cost:** 100 points, even if there are no spies. On something that isn't one of
    your towns it does nothing and uses no points.
  - **Effect:** a ring of fire over the town (the Mind Turner's, in orange), with the
    sound of Isis's power.
  - The AI uses it on its towns with foreign spies (the game knows where they are).
- **Use:** exploring the map and spying on the inside of enemy buildings without risking spies; the AI sends it to fly to random points of the map.

### Mayans — Kukulcan
- **Power (combat increase button):** the **Mayan units of your kingdom** in the area gain **+15 to +24 combat** instantly, up to a maximum of 100.
  - It works on units on the map and on Mayan workers in your buildings.
  - The units' current health doesn't change, even though their combat goes up.
- **It doesn't affect** units of other races, even if they are yours.
- **Use:** permanently improving a group of Mayan soldiers before a battle.

### Persians — Lord of Healing
- **Power (heal):** heals the **human units of your kingdom** in the area, of any race.
  - On the map: they recover between 1/9 and 1/6 of their maximum health (minimum 10).
  - Inside buildings: workers recover between 1/7 and 1/4 of their maximum health.
- **It doesn't heal** siege weapons, ships or units of other kingdoms.
- **Use:** recovering a wounded army without waiting for it to heal in the fort.

### Japanese — Mind Turner
- **Power:** lowers the loyalty of everything **not of your kingdom** in the area:
  - Units and workers with a race: **−7 to −15 loyalty**.
  - Towns of other kingdoms: lowers the loyalty of each race by about 7–15 in total if the town is fully inside the area.
  - Independent towns: lowers their resistance toward your kingdom by the same amount, which helps to conquer them.
- **Watch out:** the code doesn't tell enemies from allies apart; it affects any kingdom that isn't yours, allies included.
- **Use:** weakening the loyalty of an enemy army or town; with very low loyalty units may desert or rebel, as in the normal game.

### Indians — Djini
- **Power:** lowers **−20 to −30 loyalty** of the visible units in the area that belong to kingdoms you are **at war** with or to nobody (independents).
- **It doesn't affect** workers inside buildings, towns, or kingdoms you aren't at war with.
- **Use:** stronger than the Japanese one against armies in the field, but it only works against declared enemies.

- *(gameplay)* **It also converts a soldier:** on each cast, the soldier with the lowest
  combat in the area (soldier rank, not general or king; only people, no spies) among
  the units whose loyalty it lowers joins your kingdom, like a unit that changes sides
  (with its news message and its loyalty). Ties: the one with the lowest loyalty.

### Vikings — Thor
It has **two powers**, each costing 100 points:
- **Rain:** triggers instantly, without choosing a target.
  - Heavy rain for 10 days and a thunderstorm for 7 days.
  - Rain puts out fires.
  - Lightning strikes **random points all over the map** and damages whoever is there (units, buildings and towns of any kingdom, yours included), and it can start fires.
- **Tornado:** a tornado appears right in front of Thor, in the direction he is facing.
  - It lasts 60 days (600 frames) and the wind blows 10 days in that direction, so the tornado moves that way.
  - It damages the units it passes over, of any kingdom; flying units take double damage.
- **Use:** the tornado works against grouped armies if you turn Thor toward them; rain is more useful to put out fires than as an attack, because the lightning is random.

### Egyptians — Isis
- **Power:** cast on **a town of your kingdom** (range of a single tile), it adds **up to 5 people** of the races that already live there, without going over the growth limit of 60.
  - The new people arrive with the current loyalty of their race in that town.
- **It does nothing** to units, buildings or towns of others.
- **Use:** speeding up a town's growth, for example to have more people to recruit or to pay taxes.

### Zulus — uNkulunkulu
- **Power:** on a single tile, gives **+30 skill (leadership)** to a **Zulu unit of your kingdom that is a general or king**, up to a maximum of 100.
  - If cast on a building, the overseer gets it if he is a Zulu general.
  - It doesn't affect soldiers or workers.
- **Use:** quickly turning a Zulu general into a great leader, which improves his soldiers' training and loyalty.

---

## 4. How the AI uses Greater Beings

- The AI builds Seats of Power and invokes their beings when it has the scroll.
- **Dragons:** look for enemy targets to attack. *(gameplay)*: the Jing Nung; the Dragon throws its fireball at groups of 5 or more enemies.
- **Thor:** casts the tornado against enemy targets.
- **Lord of Healing:** looks for its own fort with the most wounded and heals it.
- **Phoenix:** flies to random points. *(gameplay)*: first it exposes the spies in its towns if it has the points.
- **The other beings:** look for targets according to their power.
