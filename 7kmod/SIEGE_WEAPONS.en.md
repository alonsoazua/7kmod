# Siege weapons

**English** | [Español](SIEGE_WEAPONS.md)

Reference on how siege weapons work in Seven Kingdoms: Ancient Adversaries 2.15.7 and in
7kmod. The values come from the game data (`RESOURCE/STD.SET`: the UNIT, UNITATTK,
SACTION and SPRITE tables) and from the code (`src/OUNIT.cpp`, `src/OUNITAT.cpp`,
`src/OBULLET.cpp`, `src/OB_FLAME.cpp`, `src/OU_CART.cpp`, `src/OF_CAMP.cpp`,
`src/OF_WAR.cpp`). Damage per second was calculated with the rules of
`tools/combat_sim.py` (see `COMBAT_ANALYSIS.md`, Spanish only).

None of the changes of 7kmod or 7kmod-gameplay modify siege weapons: everything in this
document is from the original game. The only thing that changes in gameplay is the
armor of some soldiers (see `CHANGELOG-gameplay.en.md`), which affects the damage they
take.

## 1. Costs

The building cost in the War Factory is the same as the yearly upkeep cost
(`build_cost = year_cost`).

| Weapon | Build | Upkeep / year | Days to build |
|---|---|---|---|
| Porcupine | $80 | $80 | 60 |
| Catapult | $100 | $100 | 80 |
| Ballista | $100 | $100 | 80 |
| Spitfire | $110 | $110 | 90 |
| Unicorn | $110 | $110 | 90 |
| Cannon | $120 | $120 | 100 |

- Upkeep is charged every day: 1/365 of the yearly cost (a Cannon costs about $0.33 a
  day).
- It doesn't change with the weapon's version: a Catapult III costs the same as a I.
- It's paid the same inside a fort (`FirmCamp::pay_weapon_expense`) as on the map
  (`Unit::pay_expense`).

Other non-human units, for reference: caravan $10, Trader and Transport ships $50,
Caravel $120 and Galleon $150 a year.

### Without money

If the treasury is at $0 or less, each weapon (and each ship) loses **1 health point per
day** instead of paying. When it reaches 0 it's destroyed and the news that it wore out
appears. Weapons have no loyalty, so they don't desert.

Caravans are the exception: they stay in your service even if they aren't paid.

**Invoked Fryhtans:** their yearly cost is $0, so with money they cost nothing, but
without money they fall under the same rule: on the map they lose 1 health point per day
and die without a news message. In gameplay they regenerate 1 point every 10 days, which
doesn't make up for it (about −0.9 per day). Inside a fort or a Seat of Power they lose
nothing, because the fort's charge only checks weapons.

## 2. Stats

Weapons always have combat 100, so their health is that of the data and doesn't go up.
The version (I, II or III) depends on the technology researched in the Tower of Science
when it's built and changes the attacks, not the health.

| Weapon | Health | Speed | Range (I/II/III) | Base damage (I/II/III) | Attack frames | Delay (I/II/III) | Projectile radius (I/II/III) | Fire |
|---|---|---|---|---|---|---|---|---|
| Catapult | 60 | 3 | 7 | 12 / 15 / 18 | 16 | 30 | 30 / 46 / 62 | II and III |
| Ballista | 60 | 4 | 7 | 10 (two alternating attacks) | 6 | 20 / 16 / 12 | 30 | no |
| Cannon | 80 | 3 | 6 / 7 / 8 | 16 / 20 / 24 | 4 | 30 | 30 / 30 / 46 | yes |
| Spitfire | 60 | 5 | 2 | 2 per hit, ~11 hits per blast | 3 | 25 / 20 / 15 | 30 | yes |
| Unicorn, ranged | 60 | 4 | 7 | 12 / 14 / 14 | 4 | 6 / 6 / 4 | 30 | no |
| Unicorn, melee | | | 1 | 8 / 8 / 10 | 4 | 4 | — | no |
| Porcupine | 40 | 4 | — | 50 when exploding | — | — | 3×3 | yes |

For reference, human soldiers have 200 health with combat 100 (100 with combat 50) and
speed 4 (Zulu 5 in gameplay).

## 3. Combat damage

Damage per second against a unit with armor 4 (the normal one), in version
I / II / III. The damage of a hit is (base + random(3)) / 4, minus armor / 4 (1 point
with armor 4). If it ends up less than 0.25 above the armor, the hit does at most 0.25.

| Weapon | Damage/s against armor 4 | Raw damage/s (no armor) |
|---|---|---|
| Catapult | 0.8 / 1.1 / 1.4 | 1.2 / 1.5 / 1.8 |
| Ballista | 0.9 / 1.1 / 1.4 | 1.5 / 1.7 / 2.2 |
| Cannon | 1.3 / 1.7 / 2.0 | 1.7 / 2.0 / 2.4 |
| Spitfire | 1.3 / 1.6 / 2.1 | 3.9 / 4.7 / 6.2 |
| Unicorn, ranged | 3.0 / 3.7 / 4.7 | 4.3 / 5.0 / 6.4 |
| Unicorn, melee | 2.1 / 2.1 / 3.0 | 3.8 / 3.8 / 4.7 |

Soldiers for reference (against armor 4):

| Soldier | Combat 50 | Combat 100 |
|---|---|---|
| Viking (melee) | 2.2 | 2.6 |
| Mayan (melee) | 2.2 | 2.8 |
| Chinese (melee) | 2.1 | 3.2 |
| Norman (melee) | 1.2 | 1.7 |
| Persian (ranged) | 1.1 | 1.5 |
| Egyptian (ranged) | 0.8 | 1.5 |

**Conclusion:** at version I the weapons hit like a soldier of combat 50, and at
version III like one of combat 100, but with a third or less of his health. Their
advantage is range (7–8 tiles against the archers' 4–6) and area damage. The Unicorn is
the exception: it hits harder than any soldier.

### Area damage

The projectile radius is in pixels (a tile is 32). Damage goes down from 100% at the
center to 50% at the edge of the radius (`Bullet::attenuated_damage`).

- **Radius 30:** only the target's tile.
- **Radius 46** (Catapult II, Cannon III): also the 4 neighboring tiles (up, down, left
  and right) with 65% of the damage.
- **Radius 62** (Catapult III): the 4 neighbors with 74% and the 4 diagonals with 64%.

Against tight groups, a Catapult III makes up to 9 hits per shot.

### Spitfire

The blast doesn't travel: it appears on the target's tile and hits on every frame of its
animation (12 frames, about 11 hits, `BulletFlame::process_idle`). Each hit does
(2 + random(3)) / 4 ≈ 0.75, and against armor 4 it stays at the minimum of 0.25, so
against units it does about 2.75 per blast regardless of their armor. Its weakness is
range 2, which forces it into the fight with only 60 health.

### Unicorn

It fires every 10–12 frames, against 33–45 for the Catapult and the Cannon, and it also
has a melee attack when the enemy is right next to it. Even at version I it beats almost
any combat 100 soldier. It's the clearest candidate for a balance adjustment.

### Porcupine

It doesn't attack: it's detonated (by the player's order or if a projectile of its own
kingdom hits it). It does 50 damage (49 after armor 4) to everything in the 3×3 around
it, own units included, and 50/3 to buildings. It kills any soldier with combat below 25
and leaves one with combat 50 at half health. It also makes other Porcupines within 2
tiles explode, in a chain.

### Fire

The Catapult II and III, the Cannon, the Spitfire and the Porcupine leave fire on the
ground. From what the code shows, fire doesn't damage units directly: it burns plants
and makes idle units move away.

## 4. Shields against projectiles

Some soldiers cover themselves with their shield when they see a projectile coming and
take **2.5 damage** off each one (10 / 4, `Bullet::hit_target`), before armor is
applied:

| Race | Shield from combat |
|---|---|
| Greek | always |
| Norman | 30 |
| Mughul | 40 |
| Zulu | 50 |

The shield only works if the soldier faces the projectile (±45°) and is standing still
or moving toward it. A soldier who is attacking doesn't cover himself, except the Zulu,
who also covers himself during the wait between attacks. So shields mostly protect units
that are advancing or waiting, not those already fighting in melee.

Effect per hit against a soldier with a shield and armor 4:

| Weapon | Normal hit | With shield |
|---|---|---|
| Ballista | 1.75 | 0.25 |
| Unicorn II/III, ranged | 2.75 | 0.25 |
| Catapult III | 3.75 | 1.25 |
| Cannon III | 5.25 | 2.75 |
| Spitfire (each hit of the blast) | 0.25 | 0 |

The shield completely cancels the Spitfire, leaves the Ballista and the ranged Unicorn
almost without damage, and only lets the high version Cannon and Catapult be effective.
The Unicorn's melee attack isn't a projectile, so the shield doesn't stop it. This also
affects human archers.

## 5. Against buildings

Buildings have no armor: they take the raw damage ÷ 3 (`Unit::hit_firm`, "so that it
takes longer to destroy a firm"). This applies to all units, not only weapons. Damage
per second against buildings, version I / II / III:

| Weapon | Damage/s to buildings |
|---|---|
| Catapult | 0.40 / 0.49 / 0.59 |
| Ballista | 0.49 / 0.58 / 0.74 |
| Cannon | 0.55 / 0.68 / 0.81 |
| Spitfire | 1.31 / 1.60 / 2.05 |
| Unicorn, ranged | 1.44 / 1.66 / 2.13 |

For reference, a Chinese soldier with combat 100 does about 1.66 per second to a
building, a Mayan 1.43 and a Viking 1.32.

**Conclusion:** the Catapult, the Ballista and the Cannon don't destroy buildings faster
than a good soldier. Their advantage against buildings is attacking from outside the
range of the defenses. The Spitfire and the Unicorn do knock buildings down quickly, if
they get there alive.
