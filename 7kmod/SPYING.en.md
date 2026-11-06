# Spying

**English** | [Español](SPYING.md)

Reference on how spying works in Seven Kingdoms: Ancient Adversaries 2.15.7. The values
come from the code (`src/OSPY.cpp`, `src/OSPY2.cpp`, `src/OSPYA.cpp`,
`src/OFIRMIF3.cpp`, `src/OUNIT.cpp`, `src/OSYS2.cpp`, `src/ONATIONB.cpp`).

None of the changes of 7kmod, 7kmod-gameplay or 7kmod-pathing modify spying: everything
in this document is from the original game.

## 1. How to get a spy

- **Train one in a town**, like any other skill. It comes out with spying skill 20 (the
  level of any training, `TRAIN_SKILL_LEVEL`).
- **Bribe an enemy unit** from a spy who is already inside its building (see section 5).
  The bribed unit becomes your spy with skill 10.

Each spy has:

- **Skill (0–100):** it goes up 1 point every 40 days while active, or every 80 days if
  sleeping.
- **Loyalty** toward your kingdom.
- **Disguise:** the color of the kingdom it pretends to belong to.

## 2. Cost

- **$100 a year** always, wherever it is (`SPY_YEAR_SALARY`).
- **Food:** if it's on the map or in your buildings, it eats from your stock. If it's
  infiltrated, it eats and is paid by the enemy kingdom; you only pay the $100.
- Without money it loses 1 loyalty point per day. Without food (when it eats yours) it
  loses 1 point every 5 days.

## 3. Disguise and infiltration

- On the map, a spy can change its color to another kingdom's, but **only if there are
  no enemy units within 5 tiles** (`SPY_ENEMY_RANGE`). Going back to its own color is
  always possible.
- Disguised, it can enter towns and buildings of that kingdom as if it were one of
  theirs. Inside, it can't change its disguise until it leaves.
- A spy in a town or building gives you sight of that place even under the fog of war.

## 4. Modes inside a town or building

They are changed with the spy's button. The effect is applied every 30 days.

| Mode | Where | What it does |
|---|---|---|
| **Sleep** (Idle) | everywhere | Nothing. It can't be caught (see section 6). |
| **Sow dissent** | towns | Lowers the loyalty of the people of its own race: skill ÷ 5 ÷ population of that race, every month. In an independent town it lowers its resistance to your kingdom. |
| **Sow dissent** | buildings | Sometimes takes 1 loyalty from the general if he's of its race (less likely with generals of high leadership) and from the workers of its race. |
| **Sabotage** | buildings, except forts | Lowers the building's productivity: skill ÷ 5 per spy, down to −100. |

With several spies in the same place, the effects add up.

## 5. Special actions

### Bribe

A spy can bribe a worker or the general of the building it's in. You pay what you offer
(up to $4,000) even if the bribe fails. The chance is:

> spy's skill − target's loyalty − command power (if it's a general)
> \+ your reputation + 200 × amount ÷ 4000
> \+ 10 if the spy is of its race + 10 if your king is of its race

Also, whatever the target's loyalty is above 60, 70, 80 and 90 is subtracted. With
loyalty 100 it's impossible. Bribing an enemy spy always fails. If the bribe works, the
unit becomes your spy with skill 10 and loyalty between 30 and 100 (the chance the bribe
had).

### Assassinate

A spy can try to kill the general or the king of the building it's in.

- **Attack:** skill + spy's health ÷ 2, plus skill ÷ 4 of each of your other spies in the
  building.
- **Defense:** target's health ÷ 2, +50 if it's the king, + the target's skill if it's
  also a spy, + 4 + health ÷ 30 for each normal worker, + skill ÷ 2 for each spy of the
  kingdom that owns the building.

If the attack is greater than or equal, the target dies. If there are also other
defenders and the difference is below 80, your spy dies too. If the attack is lower,
your spy dies.

### Capture a building

- If your spy is the **general** of an enemy fort, it can take the fort. The soldiers who
  don't obey it leave and attack it. Each soldier obeys with a chance of leadership ÷ 2
  + your reputation ÷ 2, +50 if he's of the spy's race. Those who obey are left with a
  loyalty of at least 30.
- In buildings without a general (mines, factories, etc.) it can only be captured if
  **all** the workers are your spies.

### View the enemy kingdom's secrets

A spy inside an enemy town or building can open that kingdom's reports. Each one needs a
minimum skill:

| Report | Minimum skill |
|---|---|
| Towns | 20 |
| Economy | 30 |
| Trade | 30 |
| Kingdom | 40 |
| Technology | 40 |
| Military | 50 |
| Spies | 90 |

When the report is closed the spy may be **exposed**, and then it dies the next day (see
section 6). See the original game's bug in section 9.

## 6. Catching spies

- Every 30 days, each town and each building tries to catch **one** enemy spy that isn't
  sleeping. The chance is 1 in (100 + spy's skill − sum of the skill of your spies in
  that place), with a minimum of 1 in (skill ÷ 10). A sleeping spy is never caught.
- If all the place's inhabitants are enemy spies, nobody is caught.
- That's why putting your own spies in your towns and buildings works as
  counterespionage: the more skill they add up, the easier they catch enemies.
- An **exposed** spy dies the next day, unless it's wearing its own kingdom's color.

## 7. Loyalty and betrayal

- Every 60 days a spy's loyalty moves 1 point toward: 50 + reputation ÷ 4 + your
  kingdom's rating ÷ 4, +20 if it's of your king's race (maximum 100).
- You can reward it: $30, +10 loyalty.
- If its loyalty goes below 30 and the kingdom it pretends to serve attracts it more (its
  reputation, +30 if it's of its race), **it stops being a spy** and stays with that
  kingdom.

## 8. A spy who inherits the throne

When the king of an AI kingdom dies and the successor is your spy:

- If its loyalty is at least 90 + that kingdom's power − yours, **it hands you the whole
  kingdom** (the kingdom surrenders to you).
- If not, it betrays you and stays as king.
- It never happens with a human kingdom. If the spy is yours and inherits your own
  throne, it simply stops being a spy.

## 9. Bug of the original game: exposure when viewing secrets

When a secret report is closed, the game decides whether the spy is exposed
(`Sys::set_view_mode`, `src/OSYS2.cpp`):

```cpp
int needViewSecretSkill = spy_array.needed_view_secret_skill(info.viewing_spy_recno);
int escapeChance = spyPtr->spy_skill - needViewSecretSkill;

if( escapeChance > 0 )
{
	if( misc2.random( escapeChance/15 )==0 )
		killFlag = 1;
}
```

**The bug:** `needed_view_secret_skill()` expects the report type (Towns, Military...),
but it gets the spy's internal number (`viewing_spy_recno`). Report types are the numbers
1 to 7, so for almost every spy it returns 0 and the report's minimum skill isn't
subtracted (for spies numbered 1 to 7 it subtracts that of some report, not the one
viewed). The chance of being exposed depends only on the spy's skill, whatever the
report:

| Skill | escapeChance ÷ 15 | Exposed |
|---|---|---|
| 20–29 | 1 | always |
| 30–44 | 2 | 1 in 2 |
| 45–59 | 3 | 1 in 3 |
| 60–74 | 4 | 1 in 4 |
| 75–89 | 5 | 1 in 5 |
| 90–100 | 6 | 1 in 6 |

A newly trained spy (skill 20) is **always** exposed and dies the next day if it views
any report, even the easiest one (Towns, which needs 20).

**Even with the parameter fixed**, the formula has an odd edge: a spy with exactly the
skill for the report (escapeChance = 0) is never exposed, while one with 1 to 14 spare
points (escapeChance ÷ 15 = 0, and `random(0)` always gives 0) is always exposed. For
example, for the Military report (50), a spy with 50 would never be caught and one with
60 always would.

The code also has an older commented-out version (`src/OINFO.cpp`) in which the spy was
exposed on average in 5 to 15 days while viewing the secrets.

Possible fixes, if it's decided to do it:

- Pass the report type instead of the spy's number.
- Use a chance that goes down continuously with the spare skill, for example 1 in (2 +
  spare skill ÷ 15), so that more skill is always better.
