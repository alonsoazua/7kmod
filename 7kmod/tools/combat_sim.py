#!/usr/bin/env python3
"""
Combat simulator for Seven Kingdoms: Ancient Adversaries (7kmod).

Reads the unit data from RESOURCE/STD.SET and reproduces the combat rules of
the game code (src/OUNITAT.cpp, src/OUNITAT2.cpp, src/OSPRITE2.cpp):

  - Max hit points    = base HP * combat level / 100
  - Damage per hit    = basic + random(3)
                        + pierce * random(combat - attack level) / (100 - attack level)
                        (+ leadership % if a general is in range), then / 4
  - The target loses  damage - 1 (fixed armor 4 / 4), or at most 0.25 if the
                      damage is below 1.25. The ARMOR value of the data is not used.
  - Attack cycle      = animation frames of the attack + attack delay (frames)
  - Power             = combat + 50 at most, +5 per game day (10 frames),
                        special attacks need MIN_POWER and consume CONSUME_P.
  - Close attacks are forced when the target is adjacent; otherwise the unit
    uses the strongest attack that reaches the target.

By default it uses the rules of 7kmod-gameplay, including its balance changes
(MOD_ARMOR, MOD_ATTACK_LEVEL and MOD_SPEED below, the same as in the code).

Usage:
  python3 combat_sim.py [path/to/STD.SET] [--original] [--armor] [--seed N]

  --original  rules of the original game (armor 4 for all, original data)
  --armor     use the ARMOR value of the game data for every unit

Without arguments it prints all the tables of 7kmod/COMBAT_ANALYSIS.md.
"""
import random, struct, sys, os

# ------------------------------------------------------------ data loading --

def load_set(path):
    d = open(path, 'rb').read()
    n = struct.unpack('<H', d[:2])[0]
    ents = []
    for i in range(n + 1):
        o = 2 + i * 13
        name = d[o:o + 9].split(b'\0')[0].decode('latin1')
        off = struct.unpack('<I', d[o + 9:o + 13])[0]
        ents.append((name, off))
    return {ents[i][0]: d[ents[i][1]:ents[i + 1][1]] for i in range(n)}

def dbf(b):
    nrec, hlen, rlen = struct.unpack('<IHH', b[4:12])
    fields = []; o = 32
    while b[o] != 0x0d:
        fields.append((b[o:o + 11].split(b'\0')[0].decode(), b[o + 16])); o += 32
    rows = []
    for r in range(nrec):
        p = hlen + r * rlen + 1; row = {}
        for nm, fl in fields:
            row[nm] = b[p:p + fl].decode('latin1').strip(); p += fl
        rows.append(row)
    return rows

def ii(x): return int(x) if x else 0

UNITS = {}

# --- 7kmod-gameplay balance changes (src/OUNITAT.cpp mod_armor(),
#     src/OUNITRES.cpp apply_mod_attack_balance(), src/OSPRTRES.cpp) ---

def MOD_ARMOR(name, combat):
    if name == 'Norman': return 5
    if name == 'Greek': return 6
    if name == 'Maya': return 3 if combat < 50 else 4
    return 4

MOD_ATTACK_LEVEL = {('Chinese', 1): 40, ('Egyptian', 1): 30, ('Japanese', 1): 50, ('Mughul', 2): 50}
MOD_SPEED = {'Zulu': 1}

ORIGINAL = False

def load_units(path, original=False):
    global ORIGINAL
    ORIGINAL = original
    UNITS.clear()
    s = load_set(path)
    sa = dbf(s['SACTION']); atk_db = dbf(s['UNITATTK']); unit_db = dbf(s['UNIT'])
    speed = {r['NAME']: int(r['SPEED']) for r in dbf(s['SPRITE'])}
    frames = {}
    for r in sa:
        if r['ACTION'].startswith('A'):
            frames[(r['SPRITE'], r['ACTION'])] = int(r['FRAMECOUNT'])
    for r in unit_db:
        if r['CLASS'] not in ('HUMAN', 'MONSTER'):
            continue
        fa = int(r['FIRSTATTK']); c = int(r['ATTKCOUNT']); atk = []
        for k in range(c):
            x = atk_db[fa - 1 + k]
            atk.append(dict(lvl=ii(x['COMBAT_LVL']), delay=ii(x['ATTK_DELAY']), rng=ii(x['ATTK_RANGE']),
                            dmg=ii(x['BASIC_DMG']), pierce=ii(x['PIERCE_DMG']), minp=ii(x['MIN_POWER']),
                            cons=ii(x['CONSUME_P']), nxt=ii(x['NEXT_ATKID']), out=ii(x['OUT_FRAME']),
                            frames=frames[(r['SPRITE'], 'A%d' % (k + 1))]))
        if not original:
            for k, a in enumerate(atk):
                a['lvl'] = MOD_ATTACK_LEVEL.get((r['NAME'], k), a['lvl'])
        spd = speed[r['SPRITE']] + (0 if original else MOD_SPEED.get(r['NAME'], 0))
        UNITS[r['NAME']] = dict(hp=int(r['HIT_PT']), armor=int(r['ARMOR']), atk=atk, cls=r['CLASS'], speed=spd)

# --------------------------------------------------------------- the rules --

def rnd(n): return random.randrange(n) if n > 0 else 0

class Fighter:
    def __init__(s, name, combat, armor_on=False, leadership=0):
        u = UNITS[name]
        s.name = name; s.c = combat; s.atk = u['atk']
        s.maxhp = u['hp'] * combat / 100; s.hp = s.maxhp
        s.maxp = combat + 50; s.p = s.maxp
        s.cur = None; s.frame = 1; s.wait = 0
        s.armor = u['armor'] if armor_on else (4 if ORIGINAL else MOD_ARMOR(name, combat))
        s.speed = u['speed']
        s.lead = leadership

    def can(s, a): return s.c >= a['lvl'] and s.p >= a['minp']

    def usable(s, a, dist):
        return s.can(a) and a['rng'] >= dist and not (dist == 1 and a['rng'] != 1)

    def choose(s, dist):
        best = None
        for i, a in enumerate(s.atk):
            if s.usable(a, dist) and (best is None or a['dmg'] > s.atk[best]['dmg']):
                best = i
        return best

    def damage(s, target):
        a = s.atk[s.cur]
        d = a['dmg'] + rnd(3) + a['pierce'] * rnd(s.c - a['lvl']) // (100 - a['lvl'])
        d += d * s.lead // 100
        x = d / 4; ar = target.armor / 4
        return x - ar if x >= ar + 0.25 else min(x, 0.25)

    def step(s, target, frame_no, dist):
        """One game frame, in the order of SpriteArray::process():
        1. the attack delay counts down,
        2. Unit::attack_target() hits when the animation reaches the hit frame
           (the last frame for close attacks, OUT_FRAME for range attacks),
        3. Sprite::process_attack() advances the animation, except while
           waiting for the delay on the first frame.
        Returns False if the fighter has no attack that reaches the target."""
        if frame_no % 10 == 0:
            s.p = min(s.maxp, s.p + 5)
        if s.wait > 0:
            s.wait -= 1
        if s.cur is None or (s.frame == 1 and not s.usable(s.atk[s.cur], dist)):
            s.cur = s.choose(dist); s.frame = 1
            if s.cur is None:
                return False
        a = s.atk[s.cur]
        hit_frame = a['frames'] if a['rng'] == 1 else min(a['out'] or a['frames'], a['frames'])
        if s.frame == hit_frame and s.can(a):
            target.hp -= s.damage(target); s.p -= a['cons']; s.wait = a['delay']
        if s.wait > 0 and s.frame == 1:
            return True
        s.frame += 1
        if s.frame > a['frames']:                  # attack finished: Unit::cycle_eqv_attack()
            s.frame = 1
            if a['nxt'] > 0:
                n = s.cur
                for _ in range(5):
                    n = s.atk[n]['nxt'] - 1
                    if s.usable(s.atk[n], dist): break
                s.cur = n
        return True

class Dummy:
    def __init__(s): s.hp = 1e9; s.armor = 4

def dps(name, combat, ranged=False, armor_on=False, runs=20, frames=1200):
    """Damage per second (12 frames/s) against a target with armor 4."""
    f = Fighter(name, combat, armor_on)
    dist = max([a['rng'] for a in f.atk if combat >= a['lvl']] or [1]) if ranged else 1
    if ranged and dist == 1:
        return 0.0
    tot = 0
    for _ in range(runs):
        f = Fighter(name, combat, armor_on); t = Dummy()
        for fr in range(1, frames + 1):
            if not f.step(t, fr, dist): return 0.0
        tot += 1e9 - t.hp
    return tot / runs / frames * 12

def duel(a, ca, b, cb, n=300, start=6, armor_on=False, lead_a=0, lead_b=0):
    """Win rate of a over b. They start 'start' tiles apart; a unit that can't
    reach walks 'speed' pixels per frame (tiles of 32 px)."""
    win = 0
    for _ in range(n):
        x = Fighter(a, ca, armor_on, lead_a); y = Fighter(b, cb, armor_on, lead_b)
        dist = start; fr = 0; mv = 0
        while x.hp > 0 and y.hp > 0 and fr < 20000:
            fr += 1
            order = [(x, y), (y, x)] if random.random() < .5 else [(y, x), (x, y)]
            walk = 0
            for p, q in order:
                if p.hp > 0 and q.hp > 0 and not p.step(q, fr, dist):
                    walk += p.speed           # pixels walked this frame
            if walk:
                mv += walk
                if mv >= 32: dist -= 1; mv -= 32
        if x.hp > 0 >= y.hp: win += 1
        elif x.hp > 0 and y.hp > 0: win += .5
    return win / n

def break_even(race, fryhtan, fry_combat=50, n=300):
    """Combat level the soldier needs to win at least half of the melee duels."""
    if duel(race, 100, fryhtan, fry_combat, n, start=1) < .5: return '>100'
    lo, hi = 10, 100
    while hi - lo > 2:
        m = (lo + hi) // 2
        if duel(race, m, fryhtan, fry_combat, n, start=1) >= .5: hi = m
        else: lo = m
    return str(hi)

# ---------------------------------------------------------------- reports --

RACES = ['Norman', 'Maya', 'Greek', 'Viking', 'Persian', 'Chinese', 'Japanese', 'Egyptian', 'Mughul', 'Zulu']
FRYHTANS = ['Rattus', 'Broosken', 'Deezboanz', 'Pfith', 'Karrotten', 'Holgh', 'Sauroid', 'Haubudam', 'Wyrm', 'Rokken']
INVOKE_COST = dict(Rattus=40, Broosken=40, Deezboanz=50, Pfith=55, Karrotten=60, Holgh=60,
                   Sauroid=65, Haubudam=65, Wyrm=70, Rokken=80)

def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--') and not a.isdigit()]
    armor_on = '--armor' in sys.argv
    seed = 1
    if '--seed' in sys.argv: seed = int(sys.argv[sys.argv.index('--seed') + 1])
    random.seed(seed)
    path = args[0] if args else os.path.join(os.path.dirname(__file__), '..', '..', 'data', 'RESOURCE', 'STD.SET')
    load_units(path, '--original' in sys.argv)

    print('## Soldiers: HP and damage/s (melee / ranged)')
    print('| Race | ' + ' | '.join('Combat %d' % c for c in (20, 50, 100)) + ' |')
    for r in RACES:
        cells = []
        for c in (20, 50, 100):
            hp = UNITS[r]['hp'] * c / 100
            cells.append('%.0f HP, %.2f / %.2f' % (hp, dps(r, c, False, armor_on), dps(r, c, True, armor_on)))
        print('| %s | %s |' % (r, ' | '.join(cells)))

    print('\n## Fryhtans at combat 50 and 100: HP, melee damage/s, HP x damage/s')
    for f in FRYHTANS + ['Droog', 'Ick']:
        for c in (50, 100):
            d = dps(f, c, False, armor_on)
            print('| %s | %d | %.0f | %.2f | %.0f |' % (f, c, UNITS[f]['hp'] * c / 100, d, d * UNITS[f]['hp'] * c / 100))

    print('\n## Race vs race, average win rate against the other 9 races')
    for c in (20, 50, 100):
        line = []
        for a in RACES:
            line.append('%s %.0f%%' % (a, 100 * sum(duel(a, c, b, c, 200, armor_on=armor_on) for b in RACES if b != a) / 9))
        print('combat %d: ' % c + ', '.join(line))

    print('\n## Combat level a soldier needs to beat a combat-50 Fryhtan in melee')
    print('| Fryhtan | ' + ' | '.join(RACES) + ' |')
    for f in FRYHTANS:
        print('| %s (%d) | %s |' % (f, INVOKE_COST[f], ' | '.join(break_even(r, f) for r in RACES)))

if __name__ == '__main__':
    main()
