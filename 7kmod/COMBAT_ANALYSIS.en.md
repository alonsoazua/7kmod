# Combat analysis: soldiers and Fryhtans

[Español](COMBAT_ANALYSIS.md) | **English**

How combat between units works, with the numbers for each race and each Fryhtan.
Sections 1 to 6 describe the rules and data of the **original game** (without the
mod's balance) and serve as a reference for comparison. Section 8 describes the
**race balance of `7kmod-gameplay`** and its results.

The rules come from the code (`src/OUNITAT.cpp`, `src/OUNITAT2.cpp`,
`src/OSPRITE2.cpp`, `src/OSPRITEA.cpp`) and the numbers from the game data
(`RESOURCE/STD.SET`, tables `UNIT`, `UNITATTK` and `SACTION`). The duel results are
simulations; the simulator is in `7kmod/tools/combat_sim.py` and can be run again
after any change (see the last section).

---

## 1. Combat rules

### Hit points

- **Maximum hit points = base hit points × combat ÷ 100.**
- All humans have base hit points of 200: a soldier with combat 20 has 40 hit points,
  with 50 has 100 and with 100 has 200.
- Fryhtans have base hit points from 150 to 400 (table in section 2).
- The combat level is what changes hit points the most. Damage changes much less.

### Damage of each hit

```
damage = basic damage + random(0 to 2)
         + piercing damage × random(0 to combat − attack level − 1) ÷ (100 − attack level)
```

1. **General's bonus:** if the unit's general is nearby, the damage rises by a
   percentage equal to his leadership (a general with 60 leadership gives +60%). It also
   applies to Fryhtans that leave the fort with their general. In `7kmod-gameplay`, the
   general also gets his own leadership as a bonus and +1 armor if he has at least 4 of
   his soldiers within 10 tiles (the king too). The bonus is applied before the armor
   reduction, so the real damage reaching the target rises more than the percentage.
2. **General reduction:** the damage is divided by 4 (`ATTACK_SLOW_DOWN`), so that
   battles last longer.
3. **Armor:** 1 is subtracted from the result. If the divided damage is less than 1.25,
   the hit takes at most 0.25.

On average, piercing adds half its value with combat 100 and almost nothing with low
combat. That is why **combat raises damage little**: a Maya does 8 + 1 + 1.5 = 10.5 per
hit with combat 50 (1.6 hit points to the target) and 8 + 1 + 3 = 12 with combat 100
(2 hit points).

### The armor in the data is not used

Each unit has an `ARMOR` value in the data (from 3 to 8), but **the code does not read
it**. It always uses a fixed armor of 4 for everyone (the "−1" above). It has been like
this since the original game; it is not a change of the mod. Consequences:

- The Rokken (armor 8) and the Sauroid (7) do not withstand more hits than a Rattus (4).
- Normans (7) and Greeks (6), which in the data seem designed as tough units with low
  damage, lose that advantage.
- Mayas and Persians (3) do not suffer from their low armor.

Section 4 compares the results if armor worked. `7kmod-gameplay` uses its own armor for
some races (section 8).

### Attack speed

- **Cycle of an attack = frames of the animation + attack delay − 1** (if the delay is
  0, only the animation).
- The game runs at 12 frames per second at normal speed: a 12-frame cycle is one hit per
  second.
- Melee attacks hit on the last frame of the animation. Ranged attacks fire on the
  `OUT_FRAME` frame and the projectile takes a moment to arrive.

### Special attacks and power

- Each unit has **power**: maximum combat + 50, recovering 5 per game day (every 10
  frames, less than 1 second).
- Some attacks need a minimum combat level and an amount of power, and spend power when
  used.
- **Choice of attack:** if the target is adjacent, the unit must use a melee attack, the
  one with the highest basic damage it can use. If it is far, it uses the attack with
  the highest damage that reaches it. If none reaches it, it walks towards it.
- Chained attacks (column `NEXT_ATKID`) alternate: after an attack, the unit moves on to
  the next one in the chain it can use.

### Fryhtan combat levels

| Fryhtan | Combat |
|---|---|
| Summoned from the Seat of Power (mod) | 50, fixed; does not train |
| King of a lair | 100 |
| Generals of a lair | 40 to 69 |
| Soldiers of a lair | from half of their general's combat to almost the same (20 to 68) |

Summoned Fryhtans also regenerate 1 hit point every 10 days and are healed by the
Persian Greater Being (see `CHANGELOG-gameplay.en.md`).

---

## 2. Data of each unit

Attacks in **basic + piercing** format. The cycle is in frames (12 = 1 second).

### Soldiers

| Race | Base hit points | Armor (not used) | Attacks |
|---|---|---|---|
| Norman | 200 | 7 | 5+5, melee, cycle 7<br>7+3, range 6, cycle 13, from combat 50 |
| Maya | 200 | 3 | 8+6, melee, cycle 8 |
| Greek | 200 | 6 | 5+3, melee, cycle 6<br>4+4, melee, cycle 3, from combat 30, power 10 (spends 8)<br>4+4, melee, cycle 3, from combat 60, power 25 (spends 12) |
| Viking | 200 | 4 | 6+6, melee, cycle 9<br>40+0, melee, cycle 12, from combat 50, power 90 (spends 60) |
| Persian | 200 | 3 | 3+3, melee, cycle 8<br>6+6, range 6, cycle 11 |
| Chinese | 200 | 5 | 6+10, melee, cycle 7<br>5+3, range 6, cycle 11, from combat 30 |
| Japanese | 200 | 5 | 5+3, melee, cycle 7<br>32+0, melee, cycle 10, from combat 55, power 100 (spends 45) |
| Egyptian | 200 | 4 | 5+4, melee, cycle 8<br>6+3, range 6, cycle 14, from combat 40<br>25+0, range 6, cycle 14, from combat 70, power 90 (spends 60) |
| Mughul | 200 | 4 | 5+6, melee, cycle 8<br>5+5, melee, cycle 4, from combat 30, power 70 (spends 10)<br>30+0, melee, cycle 7, from combat 60, power 60 (spends 50) |
| Zulu | 200 | 4 | 4+3, melee, cycle 9<br>10+8, range 7, cycle 22, from combat 30 |

The Viking's special attack needs 90 power and the Japanese's 100, so in practice they
start being used with combat 50 and 55, and only every so often.

### Fryhtans

| Fryhtan | Ally of (mod) | Base hit points | Armor (not used) | Attacks |
|---|---|---|---|---|
| Rattus | Japanese | 150 | 4 | 7+9, melee, cycle 7 |
| Broosken | Zulus | 150 | 3 | 15+5, melee, cycle 22 |
| Deezboanz | Mayas | 200 | 5 | 8+8, melee, cycle 13 |
| Pfith | Mughuls | 250 | 4 | 5+4, melee, cycle 8<br>8+4, melee, cycle 11, power 50 (spends 40) |
| Karrotten | Egyptians | 270 | 3 | 7+6, melee, cycle 11 |
| Holgh | Normans | 280 | 4 | 7+8, melee, cycle 10<br>10+5, melee, cycle 14, from combat 10, power 50 (spends 40) |
| Sauroid | Greeks | 300 | 7 | 7+8, melee, cycle 9 |
| Haubudam | Vikings | 300 | 4 | 7+6, melee, cycle 5<br>7+6, melee, cycle 10 (alternating) |
| Wyrm | Chinese | 330 | 5 | 6+9, melee, cycle 8<br>7+12, melee, cycle 12, from combat 20, power 50 (spends 40) |
| Rokken | Persians | 400 | 8 | 8+4, melee, cycle 12<br>15+0, melee, cycle 18, from combat 10, power 50 (spends 40) |
| Droog | — (wild only) | 320 | 6 | 6+6, melee, cycle 10<br>8+8, melee, cycle 12, from combat 10, power 50 (spends 40) |
| Ick | — (wild only) | 160 | 3 | 3+5, melee, cycle 9<br>6+4, melee, cycle 12, from combat 20, power 50 (spends 40) |
| Doink | — (wild only) | 180 | 3 | 5+5, range 5, cycle 15 |

Fryhtan special attacks need 50 power and spend 40. With combat 50 they have 100
maximum power and recover 5 per day, so in a fight they use the special at the start
and then about every 8 game days.

---

## 3. Damage per second and hit points

Damage per second against a target, without a general, at normal speed. "Range" is the
damage shooting from afar (0 if the race does not have a ranged attack yet).

### Soldiers

| Race | Combat 20 (40 hit points) | Combat 50 (100 hit points) | Combat 100 (200 hit points) |
|---|---|---|---|
| Norman | 0.86 | 1.19 · range 1.09 | 1.70 · range 1.36 |
| Maya | **1.94** | **2.24** | 2.80 |
| Greek | 1.00 | 1.21 | 1.74 |
| Viking | 1.05 | **2.20** | 2.66 |
| Persian | 0.38 · range 0.86 | 0.42 · range 1.09 | 0.54 · range 1.48 |
| Chinese | 1.49 | 2.13 · range 0.53 | **3.19** · range 0.82 |
| Japanese | 0.84 | 0.99 | 2.20 |
| Egyptian | 0.75 | 0.93 · range 0.75 | 1.30 · range 1.50 |
| Mughul | 0.80 | 1.24 | 2.47 |
| Zulu | 0.44 | 0.51 · range 1.05 | 0.70 · range 1.44 |

### Fryhtans

"Strength" is hit points × damage per second: a measure of how much total damage a unit
does before dying, useful to compare units with different profiles.

| Fryhtan | Cost (points) | Combat 50: hit points / damage/s / strength | Combat 100: hit points / damage/s / strength |
|---|---|---|---|
| Rattus | 40 | 75 / **2.43** / 182 | 150 / 3.42 / 513 |
| Broosken | 40 | 75 / 1.73 / 130 | 150 / 1.88 / 282 |
| Deezboanz | 50 | 100 / 1.50 / 150 | 200 / 1.95 / 390 |
| Pfith | 55 | 125 / **1.01** / 127 | 250 / 1.39 / 348 |
| Karrotten | 60 | 135 / 1.37 / 184 | 270 / 1.74 / 471 |
| Holgh | 60 | 140 / 1.65 / 230 | 280 / 2.17 / 608 |
| Sauroid | 65 | 150 / 1.84 / 276 | 300 / 2.49 / 747 |
| Haubudam | 65 | 150 / 1.99 / 298 | 300 / 2.57 / 772 |
| Wyrm | 70 | 165 / 1.69 / 280 | 330 / 2.58 / 851 |
| Rokken | 80 | **200** / 1.54 / 308 | 400 / 1.72 / 688 |
| Droog | — | 160 / 1.27 / 204 | 320 / 1.74 / 558 |
| Ick | — | 80 / 0.51 / 41 | 160 / 0.82 / 132 |

For comparison, a soldier with combat 50 has a strength of 224 (Maya), 220 (Viking),
121 (Greek) or 119 (Norman).

---

## 4. Soldiers against soldiers

One-on-one duels between soldiers of the same combat level, starting 6 tiles apart so
that archers get to shoot. Average percentage of wins against the other 9 races:

| Race | Combat 20 | Combat 50 | Combat 100 |
|---|---|---|---|
| Maya | 100% | 95% | 86% |
| Viking | 76% | 88% | 81% |
| Mughul | 36% | 58% | 67% |
| Chinese | 89% | 41% | 56% |
| Japanese | 48% | 31% | 56% |
| Egyptian | 24% | 13% | 39% |
| Persian | 0% | 37% | 37% |
| Greek | 69% | 50% | 37% |
| Zulu | 11% | 24% | 24% |
| Norman | 49% | 65% | 20% |

Wins of the row's race against the column's, with combat 100:

| | Nor | May | Gre | Vik | Per | Chi | Jap | Egy | Mug | Zul |
|---|---|---|---|---|---|---|---|---|---|---|
| Norman | — | 0 | 62 | 0 | 0 | 100 | 0 | 0 | 0 | 10 |
| Maya | 100 | — | 100 | 71 | 100 | 0 | 100 | 100 | 100 | 100 |
| Greek | 30 | 0 | — | 0 | 100 | 0 | 0 | 100 | 0 | 100 |
| Viking | 100 | 32 | 100 | — | 100 | 0 | 100 | 100 | 97 | 100 |
| Persian | 99 | 0 | 0 | 0 | — | 100 | 0 | 41 | 0 | 86 |
| Chinese | 0 | 100 | 100 | 100 | 0 | — | 100 | 0 | 100 | 0 |
| Japanese | 100 | 0 | 100 | 0 | 100 | 0 | — | 100 | 0 | 100 |
| Egyptian | 100 | 0 | 0 | 0 | 57 | 100 | 0 | — | 0 | 94 |
| Mughul | 100 | 0 | 100 | 2 | 100 | 0 | 100 | 100 | — | 100 |
| Zulu | 94 | 0 | 0 | 0 | 10 | 100 | 0 | 4 | 0 | — |

**How to read it:**

- The duels are very rock-paper-scissors: almost all end 100 to 0. The averages say more
  than each matchup.
- **Mayas and Vikings are the strongest** at all levels. The Maya has the best basic
  attack in the game and the Viking a special of 40 from combat 50.
- **The Chinese depend on distance:** in melee they are the strongest with combat 100,
  but from combat 30 they prefer to shoot weak arrows when the enemy is not adjacent, and
  they lose against other archers.
- **The Japanese** are weak until combat 55, when their special kicks in.
- **Normans, Egyptians, Greeks and Zulus** fall behind. Normans and Greeks have weak
  attacks and their high armor does not count. Egyptians have arrows until combat 40 and
  their strong attack until combat 70.
- **Persians and Zulus** are archers: they do badly in duels because the enemy quickly
  reaches melee, but in real battles, shooting behind a line of soldiers, they perform
  better than this table shows.

### If armor worked

Same duels, but using each unit's `ARMOR` value instead of the fixed 4 (each point above
4 takes 0.25 more per hit and each point below takes 0.25 less):

| Race | Armor | Combat 20 | Combat 50 | Combat 100 | Change with 100 |
|---|---|---|---|---|---|
| Norman | 7 | 89% | 80% | 88% | +68 |
| Viking | 4 | 44% | 87% | 84% | +3 |
| Japanese | 5 | 56% | 44% | 71% | +15 |
| Chinese | 5 | 88% | 73% | 56% | 0 |
| Mughul | 4 | 32% | 33% | 54% | −13 |
| Greek | 6 | 90% | 77% | 46% | +9 |
| Maya | 3 | 67% | 56% | 35% | −51 |
| Egyptian | 4 | 23% | 0% | 33% | −6 |
| Zulu | 4 | 11% | 33% | 22% | −2 |
| Persian | 3 | 0% | 16% | 12% | −25 |

With armor, the Normans become among the strongest and Mayas and Persians fall a lot.
The data seems designed for armor, but enabling it as is unbalances the game in another
direction: it would leave Persians and Egyptians even weaker.

---

## 5. Fryhtans against soldiers

Combat level a soldier of each race needs to win at least half of the melee duels
against a summoned Fryhtan (combat 50), without generals:

| Fryhtan (cost) | Nor | May | Gre | Vik | Per | Chi | Jap | Egy | Mug | Zul |
|---|---|---|---|---|---|---|---|---|---|---|
| Rattus (40) | 68 | 43 | 68 | 50 | >100 | 46 | 55 | 80 | 60 | >100 |
| Broosken (40) | 55 | 32 | 53 | 50 | >100 | 37 | 55 | 63 | 52 | 97 |
| Deezboanz (50) | 60 | 37 | 60 | 50 | >100 | 40 | 55 | 69 | 57 | >100 |
| Pfith (55) | 53 | 32 | 52 | 49 | >100 | 35 | 55 | 63 | 52 | 94 |
| Karrotten (60) | 68 | 43 | 68 | 50 | >100 | 46 | 55 | 79 | 60 | >100 |
| Holgh (60) | 79 | 52 | 79 | 52 | >100 | 53 | 61 | 92 | 60 | >100 |
| Sauroid (65) | 88 | 60 | 88 | 61 | >100 | 60 | 69 | >100 | 66 | >100 |
| Haubudam (65) | 92 | 63 | 92 | 66 | >100 | 63 | 74 | >100 | 69 | >100 |
| Wyrm (70) | 89 | 60 | 89 | 61 | >100 | 61 | 71 | >100 | 66 | >100 |
| Rokken (80) | 94 | 64 | 94 | 68 | >100 | 64 | 77 | >100 | 71 | >100 |

Persians, Zulus and Egyptians look so high because this table is melee; as archers they
can damage the Fryhtan before it arrives.

### Fryhtans withstand a lot and deal little damage

- **Hit points grow with the Fryhtan's size, damage does not.** Their base hit points go
  from 150 to 400, but their damage per hit is 5 to 8, the same as humans'.
- **Example:** a Rokken with combat 50 has twice the hit points of a soldier with combat
  50, but hits less than a Maya. In a duel, the Rokken takes about 65 seconds to kill a
  Maya with combat 50 and the Maya about 89 to kill the Rokken.
- **Raising their combat does not fix it:** combat multiplies hit points but barely
  raises damage. A Rokken with combat 100 has twice the hit points and only 12% more
  damage.
- **Their specials barely count** in long fights, because of the power they need.
- **Regeneration** (1 hit point every 10 days) makes them even tougher in long fights.
- **Exceptions:** the Rattus and the Broosken are the other way around, with few hit
  points (75 with combat 50). The Rattus deals the most damage per second of all
  Fryhtans.

### Summoning cost against strength

The current costs were calculated with base hit points only. With strength (hit points ×
damage per second, combat 50):

| Fryhtan | Cost | Strength | Strength per point |
|---|---|---|---|
| Rattus | 40 | 182 | 4.6 |
| Haubudam | 65 | 298 | 4.6 |
| Sauroid | 65 | 276 | 4.2 |
| Wyrm | 70 | 280 | 4.0 |
| Rokken | 80 | 308 | 3.9 |
| Holgh | 60 | 230 | 3.8 |
| Broosken | 40 | 130 | 3.3 |
| Karrotten | 60 | 184 | 3.1 |
| Deezboanz | 50 | 150 | 3.0 |
| Pfith | 55 | 127 | **2.3** |

The Pfith is the worst per point and among the weakest overall. The Rattus costs 40 and
has the same strength as the Karrotten, which costs 60.

---

## 6. Limits of the analysis

- These are one-on-one duels, on flat terrain and with full hit points. They do not
  include generals (unless stated), groups, formations, buildings or Greater Beings.
- Projectiles are assumed to always hit. In the game, an arrow can miss if the target
  moves.
- Each number comes from hundreds of duels with randomness, so it can vary by about ±3
  points between runs.
- In real battles, archers (Persians, Zulus, Egyptians, Normans and Chinese with high
  combat) perform better than in these tables if a melee line protects them.

## 7. How to generate the numbers again

The simulator reads the game data and applies the same rules as the code:

```
python3 7kmod/tools/combat_sim.py                       # 7kmod-gameplay rules, data/RESOURCE/STD.SET
python3 7kmod/tools/combat_sim.py --original            # original game rules (sections 3 to 5)
python3 7kmod/tools/combat_sim.py path/to/STD.SET       # another data file
python3 7kmod/tools/combat_sim.py --armor               # using the armor from the data
python3 7kmod/tools/combat_sim.py --seed 5              # another random seed
```

It prints the tables of damage per second, duels between races and Fryhtans against
soldiers. The mod's balance changes are at the top of the simulator (`MOD_ARMOR`,
`MOD_ATTACK_LEVEL` and `MOD_SPEED`), with the same values as the code. If the rules are
changed in the code, the same change has to be applied there or in the `Fighter` class.

---

## 8. Race balance of 7kmod-gameplay

### Changes

| Race | Change | Original |
|---|---|---|
| Norman | Armor 4; 5 from combat 50 | 4 (fixed for everyone) |
| Greek | Armor 4; 5 from combat 30; 6 from combat 70 | 4 |
| Maya | Armor 3 while their combat is below 50; 4 from 50 | 4 |
| Chinese | Ranged attack from combat 40 | 30 |
| Egyptian | Ranged attack from combat 30 | 40 |
| Japanese | Special attack from combat 50 | 55 |
| Mughul | Second special attack (30 damage) from combat 50 | 60 |
| Zulu | Movement speed 5 (1.9 tiles per second) | 4 (1.5) |

- **Armor:** the armor applies to all units of that race (soldiers, generals, king,
  civilians and spies). The other races, Fryhtans, weapons and ships stay at 4. Each
  point above 4 takes 0.25 more from each hit received, and a hit that does not exceed
  the armor + 0.25 takes at most 0.25.
- **Attacks:** the levels are changed when the data is loaded
  (`UnitRes::apply_mod_attack_balance()`), without modifying the game files.
- **Speed:** changed when the sprite data is loaded. Rain and snow still reduce it the
  same way.

### Effect of armor on each hit

Average damage of a melee hit from an attacker with combat 20, after the reduction:

| Attacker | Against armor 4 | Against armor 5 | Against armor 6 |
|---|---|---|---|
| Maya | 1.39 | 1.14 | 0.89 |
| Chinese | 0.99 | 0.74 | 0.49 |
| Viking | 0.89 | 0.64 | 0.39 |
| Norman | 0.62 | 0.37 | 0.25 |

With low combat, hits are weak and armor 6 takes more than half of them. With high
combat hits are stronger and armor weighs less.

### Soldiers against soldiers with the mod's balance

Average percentage of wins against the other 9 races (same method as section 4),
compared with the original:

| Race | Combat 20 | Combat 30 | Combat 40 | Combat 50 | Combat 100 |
|---|---|---|---|---|---|
| Maya | 95 (100) | 87 (100) | 88 (100) | 95 (93) | 86 (86) |
| Viking | 75 (75) | 66 (76) | 66 (78) | 87 (88) | 81 (81) |
| Mughul | 36 (35) | 53 (55) | 54 (58) | **67** (57) | 65 (67) |
| Greek | 68 (68) | **80** (64) | **78** (62) | 46 (50) | **57** (37) |
| Chinese | 95 (89) | **98** (67) | 67 (56) | 39 (40) | 56 (56) |
| Norman | 48 (48) | 48 (49) | 47 (48) | 44 (64) | **46** (20) |
| Japanese | 47 (48) | 29 (31) | 28 (27) | **56** (31) | 42 (56) |
| Persian | 0 (0) | 15 (15) | 32 (32) | 32 (35) | 26 (37) |
| Egyptian | 24 (24) | 5 (24) | 16 (16) | 11 (14) | 28 (38) |
| Zulu | 11 (11) | 19 (19) | 23 (23) | 24 (25) | 14 (23) |

In parentheses, the original result. Wins with combat 100, row against column:

| | Nor | May | Gre | Vik | Per | Chi | Jap | Egy | Mug | Zul |
|---|---|---|---|---|---|---|---|---|---|---|
| Norman | — | 0 | 0 | 0 | 100 | 100 | 14 | 100 | 0 | 93 |
| Maya | 100 | — | 100 | 70 | 100 | 0 | 100 | 100 | 100 | 100 |
| Greek | 100 | 0 | — | 0 | 100 | 0 | 100 | 100 | 12 | 100 |
| Viking | 100 | 26 | 100 | — | 100 | 0 | 100 | 100 | 96 | 100 |
| Persian | 0 | 0 | 0 | 0 | — | 100 | 0 | 44 | 0 | 88 |
| Chinese | 0 | 100 | 100 | 100 | 0 | — | 100 | 0 | 100 | 0 |
| Japanese | 82 | 0 | 0 | 0 | 100 | 0 | — | 100 | 0 | 100 |
| Egyptian | 0 | 0 | 0 | 0 | 53 | 100 | 0 | — | 0 | 92 |
| Mughul | 100 | 1 | 88 | 2 | 100 | 0 | 100 | 100 | — | 100 |
| Zulu | 6 | 0 | 0 | 0 | 14 | 100 | 0 | 8 | 0 | — |

**Reading:**

- **Normans and Greeks** improve a lot with combat 100 (Normans from 20% to 46%, Greeks
  from 37% to 57%).
- **Armor rises with combat:** with low combat hits are weak and a high armor almost
  cancels them. With armor 6 from the start, Greeks won 96% to 100% of the duels with
  combat 20 to 40. With the stepped armor they stay at 68% to 80%: strong, but not
  unbeatable. Normans do not change until combat 50.
- **Mayas:** armor 3 takes away the total advantage they had at the start (from 100% to
  78–83%) without touching them from combat 50.
- **Japanese and Mughuls** improve with combat 50 thanks to their earlier specials
  (Japanese from 31% to 55%, Mughuls from 57% to 67%).
- **Chinese:** with combat 30 they no longer swap their strong melee attack for weak
  arrows (from 67% to 87%); from 40 they stay as before.
- **Egyptians:** they do not improve in duels. With arrows from combat 30 they keep
  shooting weak arrows instead of closing in for melee, and lose against other archers.
  In battles with a melee line in front, shooting earlier does help them; duels do not
  show it.
- **Zulus:** speed barely matters in duels, because they fight at range and barely walk.
  It helps them catch up, flee and reach the battle earlier. They drop with combat 100
  because they now lose against the Normans.
- **Persians, Egyptians and Zulus** are still the weakest in duels. They are archers:
  their value is in group battles, which this simulator does not measure.

### Fryhtans against soldiers with the mod's balance

Combat a soldier needs to win half of the melee duels against a summoned Fryhtan
(combat 50):

| Fryhtan (cost) | Nor | May | Gre | Vik | Per | Chi | Jap | Egy | Mug | Zul |
|---|---|---|---|---|---|---|---|---|---|---|
| Rattus (40) | 60 | 49 | 50 | 50 | >100 | 46 | 52 | 79 | 50 | >100 |
| Broosken (40) | 52 | 35 | 47 | 50 | >100 | 37 | 50 | 63 | 50 | 97 |
| Deezboanz (50) | 52 | 41 | 46 | 50 | >100 | 40 | 50 | 69 | 50 | >100 |
| Pfith (55) | 43 | 41 | 32 | 49 | >100 | 37 | 50 | 63 | 50 | 94 |
| Karrotten (60) | 58 | 50 | 47 | 50 | >100 | 46 | 52 | 79 | 50 | >100 |
| Holgh (60) | 69 | 52 | 60 | 52 | >100 | 53 | 60 | 92 | 57 | >100 |
| Sauroid (65) | 77 | 60 | 66 | 61 | >100 | 60 | 69 | >100 | 64 | >100 |
| Haubudam (65) | 80 | 63 | 68 | 66 | >100 | 63 | 74 | >100 | 69 | >100 |
| Wyrm (70) | 77 | 60 | 66 | 61 | >100 | 61 | 71 | >100 | 68 | >100 |
| Rokken (80) | 85 | 64 | 75 | 68 | >100 | 64 | 77 | >100 | 71 | >100 |

This table was calculated with the first version of the balance (Normans with armor 5
and Greeks with 6 at all levels). With the stepped armor, the values for Normans and
Greeks below combat 50 would be somewhat higher; those of 50 or more do not change.
Normans and Greeks need about 10 to 25 fewer combat points than before; Japanese and
Mughuls, about 5. Mayas with low combat need a bit more because of their armor 3.

### Summoned Fryhtans with the mod's balance

The previous table (Fryhtans against soldiers) is without these changes: it only
includes the race balance. Changes to summoned Fryhtans:

- **+25% damage** for summoned Fryhtans (those of a kingdom); wild ones do not change.
  Applied before the general's bonus, in `Unit::actual_damage()`.
- **Mughuls summon the Doink** instead of the Pfith. It is the only Fryhtan with a ranged
  attack; its range rises from 5 to 6, the same as human archers.
- **Broosken:** speed 4 (was 3), the normal one.
- **Costs** by strength (hit points × damage per second with combat 50 and the +25%),
  from 40 to 80 rounded to 5, with −5 for the slow ones (speed 3). The Haubudam ends at
  75 and the Doink, a weak archer, at 30.

| Race | Fryhtan | Cost original → mod | Speed | Hit points (combat 50) | Damage/s with +25% |
|---|---|---|---|---|---|
| Mughul | Pfith → **Doink** | 55 → 30 | 5 | 90 | ~1.2 (range 6, +1 base damage) |
| Zulu | Broosken | 40 → 40 | 3 → 4 | 75 | 2.26 |
| Maya | Deezboanz | 50 → 45 | 4 | 100 | 1.99 |
| Japanese | Rattus | 40 → 55 | 5 | 75 | 3.32 |
| Egyptian | Karrotten | 60 → 50 | 4 | 135 | 1.88 |
| Norman | Holgh | 60 → 60 | 3 | 140 | 2.20 |
| Greek | Sauroid | 65 → 80 | 5 | 150 | 2.48 |
| Chinese | Wyrm | 70 → 75 | 4 | 165 | 2.36 |
| Viking | Haubudam | 65 → 75 | 4 | 150 | 2.75 |
| Persian | Rokken | 80 → 90 (armor 5) | 3 | 200 | 2.06 |

- The damage per second with +25% comes from the same simulation as section 3, with the
  bonus applied. The Doink's is calculated by hand from its data.
- **The bonus yields more than 25% in damage per second** (30% to 45%), because it is
  applied before the fixed armor reduction. Those with low basic damage, like the Rattus,
  gain the most; that is why its cost rises more.
- Combat a Maya needs to win half of the duels (previous simulation): Rattus 49 → 55,
  Holgh 52 → 66, Rokken 64 → 80.
- **Doink:** with +25% it deals damage similar to a Persian or Zulu archer with combat
  50, with fewer hit points. Its role is cheap and fast support from behind the line.

**Adjustment with duels:** duels of each summoned Fryhtan against Greeks and Mayas of
combat 30 to 100 were then simulated, with and without a general. Highest soldier combat
each one beats without generals:

| Tier | Fryhtans | Cost | Beats Greeks up to | Beats Mayas up to |
|---|---|---|---|---|
| Strong | Haubudam, Wyrm | 75 | 80 | 70 |
| Strong | Sauroid (fast) | 80 | 80 | 70 |
| Tank | Rokken (armor 5) | 90 | 100 | 80 |
| Medium-high | Holgh | 60 | 70 | 60 |
| Medium | Rattus | 55 | 60 | 50 |
| Medium | Karrotten | 50 | 60 | 50 |
| Medium-low | Deezboanz | 45 | 50–60 | 40 |
| Low | Broosken | 40 | 50 | 40 |
| Support | Doink | 30 | 30 | — |

- With a general of 60 leadership on the soldier's side, almost all lose from combat
  40–50; the Rokken with armor 5 holds up to 60 against Greeks and 50 against Mayas. The
  general is the factor that decides these duels the most.
- The Doink is not meant for duels: its value is shooting from behind a line.

The numbers in sections 3 and 5 are from the original game; to recalculate them with
these changes, run the simulator without `--original`.
