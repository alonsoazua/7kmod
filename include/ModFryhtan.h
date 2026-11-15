/*
 * Seven Kingdoms: Ancient Adversaries
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

//Filename    : ModFryhtan.h
//Description : 7kmod-fryhtan: Fryhtan kingdoms. The player can play as a
//              kingdom of Fryhtans. See 7kmod/TODO-fryhtan.md.

#ifndef __MODFRYHTAN_H
#define __MODFRYHTAN_H

#include <stdint.h>
#include <GAMEDEF.h>

class File;

//------- Define constant -------//

enum { FRYHTAN_LARGE_MIN_LEVEL = 6 };
enum { FRYHTAN_BROOSKEN = 3, FRYHTAN_PFITH = 5, FRYHTAN_DOINK = 7, FRYHTAN_ICK = 10 };		// monster ids. (MONSTER.DBF) with their own size or price: Pfith is small, the other three are cheaper		// monster level from which a Fryhtan is large (FRYHTAN_LARGE_POPULATION)
enum { FRYHTAN_BASE_POPULATION = 20 };
enum { FRYHTAN_POPULATION_LIMIT = 0 };		// 1: training is limited by the Fryhtan population cap (off for now)
enum { FRYHTAN_SMALL_POPULATION = 2,		// population taken by a small Fryhtan of the kingdom
		 FRYHTAN_LARGE_POPULATION = 3 };		// and by a large one
enum { FRYHTAN_SPECIES_COUNT = 13 };			// no. of records in MONSTER.DBF
enum { FRYHTAN_DEFAULT_MONSTER_ID = 6 };		// Rokken, the default choice in the new game screen		// population of Fryhtans allowed at the start
enum { FRYHTAN_START_SOLDIER_COUNT = 4 };		// soldiers in the starting lair
enum { FRYHTAN_START_TOWN_POPULATION = 20 };		// people of the starting enslaved town

enum { FRYHTAN_KING_COMBAT = 60,				// combat and leadership of the starting All High
		 FRYHTAN_KING_LEADERSHIP = 100 };

enum { LAIR_SETUP_COST = 500,						// cost of building a lair
		 LAIR_YEAR_COST  = 100 };					// yearly upkeep of a lair

enum { LAIR_TRAIN_FOOD = 20,						// food used for training a Fryhtan in a lair
		 LAIR_TRAIN_DAYS = 15,						// days needed for training it (a large one)
		 LAIR_SMALL_TRAIN_DAYS = 10,				// days needed for training a small one
		 LAIR_SMALL_TRAIN_COST = 150,			// gold for training a small Fryhtan, soldier or Ordo
		 LAIR_LARGE_TRAIN_COST = 200,			// gold for training a large one
		 LAIR_CHEAP_TRAIN_COST = 100,			// gold for training an Ick, a Broosken or a Doink
		 LAIR_TRAIN_COMBAT = 20,					// combat level of a trained Fryhtan
		 LAIR_TRAIN_LEADERSHIP = 20,				// leadership of a trained Fryhtan (as a human soldier)
		 LAIR_SHOW_TRAIN_ORDO = 1,				// 1: the lair has the Train Ordo button (hidden: the soldiers are promoted)
		 LAIR_MAX_TRAIN_QUEUE = 8 };

enum { LAIR_ORDO_FOOD = 40,						// food used for training an Ordo (general) in a lair
		 LAIR_ORDO_DAYS = 15,						// days needed for training it (a large one), as a soldier
		 LAIR_SMALL_ORDO_DAYS = 10,				// days needed for training a small one
		 LAIR_ORDO_COMBAT = 30,						// combat level of a trained Ordo
		 LAIR_ORDO_LEADERSHIP = 30 };				// leadership of a trained Ordo

enum { LAIR_TRAIN_SOLDIER, LAIR_TRAIN_ORDO, LAIR_TRAIN_TYPE_COUNT };		// what a lair trains

enum { ORDO_NAME_ID_BASE = 60000 };

enum { FRYHTAN_HEADER_MARK = 0x7F };				// mark of the species in the header of a saved game				// Unit::name_id of an Ordo with a name: ORDO_NAME_ID_BASE + the name index (1-based)

enum { KILL_GOLD_UNIT		= 2,					// gold for killing a human villager (a soldier: ModFryhtan::kill_gold())
		 KILL_GOLD_MIN_COMBAT	= 10,				// a soldier gives KILL_GOLD_UNIT with this combat level...
		 KILL_GOLD_MAX			= 45,				// ...and this with combat 100, in a straight line between them
		 KILL_GOLD_DEFENDER_PERCENT = 50,		// a town defender gives this % of a soldier of its combat
		 KILL_GOLD_GENERAL_MULTIPLY = 3,		// a general gives this many times a soldier of its combat
		 KILL_GOLD_KING_MULTIPLY = 22,			// a king, this many times (about $1000 with combat 100)...
		 KILL_GOLD_NEXT_KING_PERCENT = 50,		// ...and this % for the next kings of the same kingdom
		 KILL_FOOD_UNIT		= 5,					// food for killing a human soldier, villager or town defender
		 KILL_FOOD_GENERAL	= 20,					// a human general
		 KILL_FOOD_KING		= 100 };				// a human king

enum { LOST_LAIR_GRACE_DAYS = 90 };

enum { FRYHTAN_REPAIR_DAYS = 5 };				// the buildings of a Fryhtan kingdom recover 1 hit point in this time, like a construction worker of skill 20			// an enslaved town without a linked lair becomes independent after this

//--- raiding an independent town from a linked lair ---//

enum { RAID_INTERVAL_DAYS = 60,				// a town can be raided once in this time
		 RAID_LOOT_PER_PERSON = 2,				// gold and food stolen for each person, x (100 + leadership) / 100...
		 RAID_MAX_LOOT = 60,						// ...from at most this much before the leadership (30 people)
		 RAID_PEOPLE_PER_DEFENDER = 10,			// one defender attacks the lair for each this many people, plus one...
		 RAID_RESISTANCE_EXTRA1 = 50,			// ...one more above this resistance...
		 RAID_RESISTANCE_EXTRA2 = 80,			// ...and two more above this one
		 RAID_RESISTANCE_DEC = 5,					// each raid lowers the resistance by this, after the defenders come out
		 RAID_MIN_RESISTANCE = 30,				// MIN_INDEPENDENT_DEFEND_LOYALTY: no defenders come out below this, the town can't be raided
		 RAID_LEADER_COMBAT_BONUS = 20,			// the leader of the defenders is stronger
		 RAID_KILL_VILLAGERS = 0 };				// 1: a raid kills villagers by the size of the town (off for now)
enum { ENSLAVE_MIN_RESISTANCE = 45,			// the lairs lower the resistance of an independent town down to this with a general of leadership 100...
		 ENSLAVE_MIN_RESISTANCE_LOW = 60,		// ...and down to this with leadership 30 or less (ModFryhtan::enslave_min_resistance())
		 ENSLAVED_TOWN_LOYALTY  = 50,			// fixed loyalty of the people of an enslaved town with an Ordo or the king in a linked lair
		 ENSLAVED_TOWN_NO_COMMANDER_LOYALTY = 0,	// its target loyalty when no Ordo or king commands a linked lair
		 TRIBUTE_PER_PERSON		= 3 };			// monthly tribute of an enslaved town for each person

class Firm;
class Unit;
class Town;

//------- Define struct FryhtanLair -------//
//
// The lairs of a Fryhtan kingdom are Forts (FirmCamp) drawn with the lair of
// a Fryhtan species. This keeps their species and their training queue,
// indexed by firm recno.

struct FryhtanLair
{
	int16_t	monster_id;			// 0 if the firm is not a lair of a Fryhtan kingdom
	int16_t	train_count;		// no. of Fryhtans queued for training
	int16_t	train_days;			// days spent training the first one in the queue
	int16_t	ordo_mask;			// bit i set: the Fryhtan i of the queue (0-the first) is an Ordo

	int	train_type(int i)		{ return (ordo_mask >> i) & 1; }
	int	type_count(int trainType);
};

//------- Define struct LairOffer -------//
//
// Phase 5: the offer of a wild lair (FirmMonster) linked to a lair of the
// Fryhtan kingdom. Paying it captures the lair with a simulated Ordo and
// soldiers. Indexed by the firm recno of the wild lair.

enum { OFFER_MAX_SOLDIER = 8 };

enum { OFFER_ORDO_MIN_LEADERSHIP = 30, OFFER_ORDO_MAX_LEADERSHIP = 60,
		 OFFER_SOLDIER_MIN_COMBAT  = 20, OFFER_SOLDIER_MAX_COMBAT  = 40,
		 OFFER_COST_PER_COMBAT	  = 10,		// the base tribute: the sum of the combat levels x 10
		 OFFER_MIN_LEADERSHIP	  = 30,		// the tribute is x3 with this leadership of the best linked general...
		 OFFER_MAX_LEADERSHIP	  = 100,		// ...and x1 with this one
		 LAIR_CAPTURE_DISTANCE	  = 8 };		// a wild lair is linked to a lair of the kingdom within this distance (as a town to a Fort)

struct LairOffer
{
	int16_t	monster_id;			// the species of the wild lair, 0 if there is no offer
	int16_t	ordo_combat;
	int16_t	ordo_leadership;
	int16_t	soldier_count;
	int16_t	soldier_combat[OFFER_MAX_SOLDIER];
};

class FirmMonster;

//------- Define struct TownOrdo -------//
//
// An Ordo sent into an enslaved town. The unit is removed from the map and
// kept here, indexed by town recno, until it leaves the town.

enum { ORDO_ACTION_SUBJUGATE = 1,			// the loyalty of the town doesn't drop below the leadership of the Ordo
		 ORDO_ACTION_KIDNAP,						// a villager is added to the town now and then
		 ORDO_ACTION_TASKMASTER };				// more tribute and food from each villager

enum { ORDO_TASK_MIN_LEADERSHIP = 30,		// the leadership range of the effects of the actions...
		 ORDO_TASK_MAX_LEADERSHIP = 100,		// ...(below and above it, as at the ends)
		 SUBJUGATE_MAX_LOYALTY = 50,			// the highest loyalty Subjugate keeps (an upgrade could raise it)
		 KIDNAP_DAYS_LOW = 240,					// the average days between kidnaps with the lowest leadership...
		 KIDNAP_DAYS_HIGH = 120,					// ...and with the highest one; each one comes between 1/2 and 3/2 of it
		 KIDNAP_VILLAGERS = 2,					// villagers brought by each kidnap
		 ORDO_TOWN_VILLAGERS_PER_SOLDIER = 5,	// inside a town, these villagers count as one soldier for the leadership of the Ordo...
		 ORDO_LEADERSHIP_PERCENT = 150,		// ...whose leadership rises faster than a human general's (also in a lair)
		 TASKMASTER_GOLD_LOW = 50,				// extra tribute a month for each villager, in 1/100 $, with the lowest leadership...
		 TASKMASTER_GOLD_HIGH = 200,			// ...and with the highest one
		 TASKMASTER_FOOD_LOW = 5,				// extra food a year for each villager, with the lowest leadership...
		 TASKMASTER_FOOD_HIGH = 20,				// ...and with the highest one
		 TASKMASTER_DEATH_DAYS = 60,			// Taskmaster kills a villager every this many days...
		 TASKMASTER_DEATH_DAYS_RANGE = 60,		// ...plus up to these at random (90 on average)
		 OVERPOPULATED_TOWN_POPULATION = 30,	// an enslaved town with this many people loses loyalty...
		 OVERPOPULATED_LOYALTY_DAYS = 5,		// ...1 point every this many days, and rebels in the end
		 CULL_MIN_POPULATION = 5,				// Cull Population needs this many people in the town...
		 CULL_MIN_KILL = 3,						// ...and kills 3 or 4 of them
		 CULL_MAX_KILL = 4 };

struct TownOrdo
{
	int16_t	unit_id;				// 0 if there is no Ordo in the town
	uint16_t	name_id;
	int8_t	nation_recno;
	int8_t	action;				// ORDO_ACTION_???
	uint8_t	combat_level;
	uint8_t	leadership;
	int16_t	hit_points;
	int32_t	next_kidnap_date;
	uint8_t	skill_potential;
	uint8_t	leadership_minor;	// the fractions of the leadership and the combat level (Skill)
	uint8_t	combat_minor;
	int32_t	next_death_date;	// Taskmaster: when it kills a villager next
};

//------- Define class ModFryhtan -------//

class ModFryhtan
{
public:
	int16_t	nation_monster_id[MAX_NATION];		// the Fryhtan species (monster id.) of each nation, 0 for human nations

public:
	void	reset();
	void	init_nation(int nationRecno);

	void	set_nation_monster(int nationRecno, int monsterId);
	int	monster_id(int nationRecno);
	int	is_fryhtan(int nationRecno)		{ return monster_id(nationRecno) != 0; }

	int	nation_unit_id(int nationRecno);		// the unit id. of the species of the nation

	int	create_start_king(int nationRecno);	// the only unit of a Fryhtan kingdom at the start of a game

	//------ lairs ------//

	FryhtanLair*	lair_array;
	int				lair_array_size;

	void	set_lair(int firmRecno, int monsterId);
	void	del_lair(int firmRecno);
	FryhtanLair*	get_lair(int firmRecno);		// NULL if the firm is not a lair
	int	lair_monster_id(int firmRecno);
	int	is_lair(Firm* firmPtr);

	int	lair_unit_id(int firmRecno);
	int	lair_train_population(int firmRecno);	// population needed for training one more Fryhtan in this lair
	int	can_train_in_lair(int firmRecno, int trainType=LAIR_TRAIN_SOLDIER);
	int	add_lair_train(int firmRecno, int trainType=LAIR_TRAIN_SOLDIER);
	int	cancel_lair_train(int firmRecno, int trainType=LAIR_TRAIN_SOLDIER);
	void	next_day_lair(int firmRecno);
	int	add_lair_soldier(Firm* firmPtr, int monsterId, int combatLevel=LAIR_TRAIN_COMBAT);
	int	add_lair_ordo(Firm* firmPtr, int monsterId);
	static int	train_food(int trainType)	{ return trainType==LAIR_TRAIN_ORDO ? LAIR_ORDO_FOOD : LAIR_TRAIN_FOOD; }
	// days needed for training, by the size of the species
	static int	train_days(int trainType, int monsterId)
					{ if( trainType==LAIR_TRAIN_ORDO ) return is_large_monster(monsterId) ? LAIR_ORDO_DAYS : LAIR_SMALL_ORDO_DAYS;
					  return is_large_monster(monsterId) ? LAIR_TRAIN_DAYS : LAIR_SMALL_TRAIN_DAYS; }

	// gold for training a soldier or an Ordo, by the size of the species
	static int	train_cost(int monsterId);		// the gold for training a soldier or an Ordo of the species

	//------ names of the Ordos ------//

	static const char*	ordo_name(Unit* unitPtr);		// NULL if the unit has no Ordo name
	static const char*	ordo_name_by_id(int nameId);		// NULL if the name id. is not an Ordo name
	int	new_ordo_name_id(int nationRecno);

	//------ succession and defeat (phase 7) ------//

	int	think_succeed_king(int nationRecno);
	int	has_lair_or_unit(int nationRecno);
	void	draw_lair(Firm* firmPtr, int displayLayer);

	static int	firm_setup_cost(int firmId, int nationRecno);
	static int	firm_year_cost(Firm* firmPtr);

	//------ capturing wild lairs (phase 5) ------//

	LairOffer*	offer_array;
	int			offer_array_size;
	char			capturing_lair;		// 1 while a wild lair is being replaced (no gold coins when it's deleted)

	int	is_linked_wild_lair(FirmMonster* firmPtr, int nationRecno);
	LairOffer*	get_offer(FirmMonster* firmPtr);		// NULL if there is no offer
	LairOffer*	think_offer(FirmMonster* firmPtr, int nationRecno);		// create the offer if the lair is linked
	void	del_offer(int firmRecno);
	static int	offer_cost(LairOffer* offerPtr);		// the base cost
	int	is_linked_lair(Firm* lairPtr, FirmMonster* monsterFirm);
	int	capture_leadership(FirmMonster* monsterFirm, int nationRecno);		// -1 if no Ordo or king is in a linked lair
	int	capture_cost(FirmMonster* monsterFirm, int nationRecno, LairOffer* offerPtr);
	int	pay_lair_tribute(int firmRecno, int nationRecno);

	//------ enslaved towns (phase 4) ------//

	int	is_enslaved_town(Town* townPtr);
	int	lair_leadership(Town* townPtr, int nationRecno);
	int	has_lair_commander(Town* townPtr);		// whether an Ordo or the king commands a lair linked to an enslaved town
	int	nearest_lair(Town* townPtr);
	float	resistance_decrease(Town* townPtr, int nationRecno, float resistance, float targetResistance);
	int	can_enslave_town(Town* townPtr, int nationRecno);
	int	next_day_town(Town* townPtr);
	int	town_tribute(Town* townPtr);
	int	in_lair_grace(Town* townPtr);		// whether an enslaved town has lost its lairs and is waiting to become independent

	//--- the day an enslaved town lost the link with all its lairs (0: linked), by town recno ---//

	int32_t*	lost_lair_date_array;
	int		lost_lair_date_array_size;

	int32_t	lost_lair_date(int townRecno);
	void		set_lost_lair_date(int townRecno, int32_t lostDate);

	//------ raiding independent towns, and looting the towns attacked ------//

	int32_t*	raid_date_array;		// the day each town was last raided (0: never), by town recno
	int		raid_date_array_size;

	char		king_kill_array[MAX_NATION][MAX_NATION];		// the kings each Fryhtan kingdom killed of each kingdom: [killer-1][kingdom-1]

	void		reset_king_kills(int nationRecno);		// a new kingdom

	int32_t	raid_date(int townRecno);
	void		set_raid_date(int townRecno, int32_t raidDate);
	void		reset_town_dates(int townRecno);		// a new town or a town with a new owner

	//------ an Ordo inside an enslaved town ------//

	TownOrdo*	town_ordo_array;		// by town recno
	int			town_ordo_array_size;

	TownOrdo*	town_ordo(int townRecno);			// NULL if there is no Ordo in the town
	int	can_ordo_enter_town(Unit* unitPtr, int townRecno);
	void	ordo_enter_town(Town* townPtr, Unit* unitPtr);
	int	ordo_leave_town(int townRecno, int forced=0);		// return: the recno of the unit, 0 if it couldn't come out (forced: the Ordo is lost then)
	void	set_ordo_action(int townRecno, int action);
	void	next_day_town_ordo(Town* townPtr);
	int	subjugate_loyalty(Town* townPtr);		// the lowest loyalty Subjugate keeps, 0 without it
	static int	ordo_task_level(int leadership);		// the leadership in ORDO_TASK_MIN/MAX_LEADERSHIP
	static int	kidnap_days(int leadership);			// the average days between kidnaps
	static int	taskmaster_gold(int leadership);		// extra tribute a month for each villager, in 1/100 $
	static int	taskmaster_food(int leadership);		// extra food a year for each villager
	int	taskmaster_town_gold(Town* townPtr);			// the extra tribute of the town this month, $
	static int	is_overpopulated(Town* townPtr);		// an enslaved town losing loyalty for its population
	void	next_day_town_overpopulation(Town* townPtr);
	int	can_cull_town(Town* townPtr);
	int	cull_town(Town* townPtr);					// Cull Population: kills 3 or 4 villagers, for the spoils; return: the villagers killed
	void	train_town_ordo(Town* townPtr, TownOrdo* ordoPtr);		// its leadership and combat rise, it recovers hit points
	static int	enslave_min_resistance(int leadership);	// how low the lairs lower the resistance of an independent town
	void	promote_ordo(Unit* unitPtr);			// a Fryhtan soldier of a Fryhtan kingdom becomes an Ordo
	void	demote_ordo(Unit* unitPtr);			// an Ordo becomes a soldier
	static const char*	ordo_action_name(int action);		// "Subjugate", "Kidnap" or "Taskmaster"
	const char*	town_role_name(Town* townPtr);		// the action of the Ordo in a town of the player, NULL if none

	int	raid_lair(Town* townPtr, int nationRecno);		// the linked lair with the best general, 0 if none
	int	can_see_raid(Town* townPtr, int nationRecno);	// whether the Raid panel is shown
	int	raid_days_left(Town* townPtr);						// 0 if it can be raided now
	int	raid_max_defender(Town* townPtr, int nationRecno);		// the most defenders that come out
	int	raid_defender_count(Town* townPtr, int nationRecno);	// about how many defenders will come out
	int	is_raid_weakened(Town* townPtr, int nationRecno);		// its resistance is too low to raid it
	int	raid_loot(Town* townPtr, int nationRecno);			// the gold (and food) a raid steals
	int	raid_town(Town* townPtr, int nationRecno);
	void	reward_town_villager(int nationRecno);			// a villager killed in a town attacked by a Fryhtan kingdom
	//------ economy (phase 3) ------//

	void	reward_kill(int nationRecno, Unit* targetUnit);
	static int	kill_gold(int combatLevel, int percent);		// the gold of a soldier of the combat level, x percent / 100
	int	should_destroy_firm(Firm* firmPtr);
	void	destroy_firm(Firm* firmPtr);

	//------ population of a Fryhtan kingdom ------//

	int	nation_population(int nationRecno);
	int	nation_population_cap(int nationRecno);

	static int	is_large_monster(int monsterId);
	static int	unit_monster_id(int unitId);		// the Fryhtan species of a unit type, 0 if it isn't a Fryhtan
	static int	monster_population(int monsterId);		// FRYHTAN_SMALL_POPULATION or FRYHTAN_LARGE_POPULATION
	static int	unit_population(int unitId);

	static const char*	monster_name(int monsterId);
	static const char*	king_title(int nationRecno);		// "<king name> The All High"

	//------ the saved game list ------//

	static void	set_header_species(char* playerName, int nameSize, int monsterId);
	static int	header_species(const char* playerName, int nameSize);
	static const char*	monster_size_text(int monsterId);

	//--- for the new game screen, where MonsterRes is not loaded yet ---//

	static const char*	menu_species_name(int monsterId);
	static const char*	menu_species_size_text(int monsterId);
	static int				menu_next_species(int monsterId, int dir);		// dir: 1 - next, -1 - previous, in alphabetical order
	static int				is_valid_species(int monsterId)	{ return monsterId >= 1 && monsterId <= FRYHTAN_SPECIES_COUNT; }

	ModFryhtan()		{ lair_array=NULL; lair_array_size=0; offer_array=NULL; offer_array_size=0; lost_lair_date_array=NULL; lost_lair_date_array_size=0; raid_date_array=NULL; raid_date_array_size=0; town_ordo_array=NULL; town_ordo_array_size=0; capturing_lair=0; reset(); }

	int	write_file(File* filePtr);
	int	read_file(File* filePtr);
};

extern ModFryhtan mod_fryhtan;

#endif
