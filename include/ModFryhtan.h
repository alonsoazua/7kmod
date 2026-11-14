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

enum { FRYHTAN_LARGE_MIN_LEVEL = 6 };		// monster level from which a Fryhtan is large (FRYHTAN_LARGE_POPULATION)
enum { FRYHTAN_BASE_POPULATION = 20 };
enum { FRYHTAN_SMALL_POPULATION = 2,		// population taken by a small Fryhtan of the kingdom
		 FRYHTAN_LARGE_POPULATION = 3 };		// and by a large one
enum { FRYHTAN_SPECIES_COUNT = 13 };			// no. of records in MONSTER.DBF
enum { FRYHTAN_DEFAULT_MONSTER_ID = 6 };		// Rokken, the default choice in the new game screen		// population of Fryhtans allowed at the start
enum { FRYHTAN_START_SOLDIER_COUNT = 4 };		// soldiers in the starting lair
enum { FRYHTAN_START_TOWN_POPULATION = 20 };		// people of the starting enslaved town

enum { FRYHTAN_KING_COMBAT = 60,				// combat and leadership of the starting All High
		 FRYHTAN_KING_LEADERSHIP = 60 };

enum { LAIR_SETUP_COST = 500,						// cost of building a lair
		 LAIR_YEAR_COST  = 100 };					// yearly upkeep of a lair

enum { LAIR_TRAIN_FOOD = 20,						// food used for training a Fryhtan in a lair
		 LAIR_TRAIN_DAYS = 20,						// days needed for training it
		 LAIR_TRAIN_COMBAT = 20,					// combat level of a trained Fryhtan
		 LAIR_MAX_TRAIN_QUEUE = 8 };

enum { LAIR_ORDO_FOOD = 40,						// food used for training an Ordo (general) in a lair
		 LAIR_ORDO_DAYS = 40,						// days needed for training it
		 LAIR_ORDO_COMBAT = 30,						// combat level of a trained Ordo
		 LAIR_ORDO_LEADERSHIP = 30 };				// leadership of a trained Ordo

enum { LAIR_TRAIN_SOLDIER, LAIR_TRAIN_ORDO, LAIR_TRAIN_TYPE_COUNT };		// what a lair trains

enum { ORDO_NAME_ID_BASE = 60000 };

enum { FRYHTAN_HEADER_MARK = 0x7F };				// mark of the species in the header of a saved game				// Unit::name_id of an Ordo with a name: ORDO_NAME_ID_BASE + the name index (1-based)

enum { KILL_GOLD_UNIT		= 2,					// gold for killing a human soldier, villager or town defender
		 KILL_GOLD_GENERAL	= 10,					// a human general
		 KILL_GOLD_KING		= 100,				// a human king
		 KILL_FOOD_UNIT		= 5,					// food for killing a human soldier, villager or town defender
		 KILL_FOOD_GENERAL	= 20,					// a human general
		 KILL_FOOD_KING		= 100 };				// a human king

enum { LOST_LAIR_GRACE_DAYS = 90 };			// an enslaved town without a linked lair becomes independent after this

//--- raiding an independent town from a linked lair ---//

enum { RAID_INTERVAL_DAYS = 30,				// a town can be raided once in this time
		 RAID_MAX_LOOT = 30,						// at most this much gold and food ($1 and 1 food per person)
		 RAID_PEOPLE_PER_DEFENDER = 10,			// one defender attacks the lair for each this many people
		 RAID_MIN_RESISTANCE_DEC = 5,			// resistance decrease when no defenders come out
		 RAID_MAX_RESISTANCE_DEC = 10,
		 RAID_MIN_DEFEND_RESISTANCE = 30 };		// MIN_INDEPENDENT_DEFEND_LOYALTY: no defenders come out below this
enum { ENSLAVE_MIN_RESISTANCE = 45,			// the lairs lower the resistance of an independent town down to this
		 ENSLAVED_TOWN_LOYALTY  = 50,			// fixed loyalty of the people of an enslaved town with an Ordo or the king in a linked lair
		 ENSLAVED_TOWN_NO_COMMANDER_LOYALTY = 0,	// its target loyalty when no Ordo or king commands a linked lair
		 TRIBUTE_PER_PERSON		= 5 };			// monthly tribute of an enslaved town for each person

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
	// days needed for training; small Fryhtans take half the time
	static int	train_days(int trainType, int monsterId)
					{ int days = trainType==LAIR_TRAIN_ORDO ? LAIR_ORDO_DAYS : LAIR_TRAIN_DAYS;
					  return is_large_monster(monsterId) ? days : days/2; }

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

	int32_t	raid_date(int townRecno);
	void		set_raid_date(int townRecno, int32_t raidDate);
	void		reset_town_dates(int townRecno);		// a new town or a town with a new owner

	int	raid_lair(Town* townPtr, int nationRecno);		// the linked lair with the best general, 0 if none
	int	can_see_raid(Town* townPtr, int nationRecno);	// whether the Raid button is shown
	int	raid_days_left(Town* townPtr);						// 0 if it can be raided now
	int	raid_defender_count(Town* townPtr, int nationRecno);	// about how many defenders will come out
	int	raid_town(Town* townPtr, int nationRecno);
	void	reward_town_villager(int nationRecno);			// a villager killed in a town attacked by a Fryhtan kingdom
	//------ economy (phase 3) ------//

	void	reward_kill(int nationRecno, Unit* targetUnit);
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

	ModFryhtan()		{ lair_array=NULL; lair_array_size=0; offer_array=NULL; offer_array_size=0; lost_lair_date_array=NULL; lost_lair_date_array_size=0; raid_date_array=NULL; raid_date_array_size=0; capturing_lair=0; reset(); }

	int	write_file(File* filePtr);
	int	read_file(File* filePtr);
};

extern ModFryhtan mod_fryhtan;

#endif
