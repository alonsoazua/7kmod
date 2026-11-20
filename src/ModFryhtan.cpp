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

//Filename    : ModFryhtan.cpp
//Description : 7kmod-fryhtan: Fryhtan kingdoms.

#include <string.h>
#include <ALL.h>
#include <OFILE.h>
#include <OMONSRES.h>
#include <OUNITRES.h>
#include <OF_CAMP.h>
#include <OUNIT.h>
#include <OREBEL.h>
#include <OSPRTRES.h>
#include <OSKILL.h>
#include <OTOWN.h>
#include <OF_MONS.h>
#include <OWORLD.h>
#include <OSITE.h>
#include <OCONFIG.h>
#include <OREGION.h>
#include <ONATION.h>
#include <OFIRM.h>
#include <OFIRMA.h>
#include <OFIRMID.h>
#include <OFIRMRES.h>
#include <OGAME.h>
#include <OIMGRES.h>
#include <OMOUSE.h>
#include <OFONT.h>
#include <COLCODE.h>
#include <OVGA.h>
#include <OWORLDMT.h>
#include <OSERES.h>
#include <ORACERES.h>
#include <OINFO.h>
#include <ONEWS.h>
#include <OSYS.h>
#include <OF_BASE.h>

//------ the dates kept by town recno (grace period, raids) ------//

static void set_town_date(int32_t*& dateArray, int& arraySize, int townRecno, int32_t newDate);
static int  write_town_dates(File* filePtr, int32_t* dateArray, int arraySize);
static int  read_town_dates(File* filePtr, int32_t*& dateArray, int& arraySize);
#include <OTOWN.h>
#include <ModFryhtan.h>
#include "gettext.h"

ModFryhtan mod_fryhtan;

//--------- Begin of function ModFryhtan::reset ---------//
//
void ModFryhtan::reset()
{
	memset( nation_monster_id, 0, sizeof(nation_monster_id) );

	if( lair_array )
		memset( lair_array, 0, sizeof(FryhtanLair) * lair_array_size );

	if( offer_array )
		memset( offer_array, 0, sizeof(LairOffer) * offer_array_size );

	if( lost_lair_date_array )
		memset( lost_lair_date_array, 0, sizeof(int32_t) * lost_lair_date_array_size );

	if( raid_date_array )
		memset( raid_date_array, 0, sizeof(int32_t) * raid_date_array_size );

	memset( king_kill_array, 0, sizeof(king_kill_array) );
	memset( life_points_array, 0, sizeof(life_points_array) );
	memset( ai_last_attack_date, 0, sizeof(ai_last_attack_date) );
	memset( tech_state_array, 0, sizeof(tech_state_array) );
	memset( tech_days_array, 0, sizeof(tech_days_array) );
	start_region_id = 0;
	memset( cur_year_life_points_change, 0, sizeof(cur_year_life_points_change) );
	memset( last_year_life_points_change, 0, sizeof(last_year_life_points_change) );

	if( town_ordo_array )
		memset( town_ordo_array, 0, sizeof(TownOrdo) * town_ordo_array_size );

	capturing_lair = 0;
}
//----------- End of function ModFryhtan::reset -----------//


//--------- Begin of function ModFryhtan::init_nation ---------//
//
// Called when a nation is created; nations are human by default.
//
void ModFryhtan::init_nation(int nationRecno)
{
	if( nationRecno >= 1 && nationRecno <= MAX_NATION )
	{
		nation_monster_id[nationRecno-1] = 0;
		life_points_array[nationRecno-1] = 0;
		cur_year_life_points_change[nationRecno-1] = 0;
		last_year_life_points_change[nationRecno-1] = 0;

		for( int i=0 ; i<MAX_NATION ; i++ )
		{
			ai_last_attack_date[nationRecno-1][i] = 0;
			ai_last_attack_date[i][nationRecno-1] = 0;
		}

		for( int t=0 ; t<FRYHTAN_TECH_COUNT ; t++ )
		{
			tech_state_array[nationRecno-1][t] = FRYHTAN_TECH_NONE;
			tech_days_array[nationRecno-1][t]  = 0;
		}
	}
}
//----------- End of function ModFryhtan::init_nation -----------//


//--------- Begin of function ModFryhtan::set_nation_monster ---------//
//
void ModFryhtan::set_nation_monster(int nationRecno, int monsterId)
{
	if( nationRecno < 1 || nationRecno > MAX_NATION )
		return;

	if( monsterId && !nation_monster_id[nationRecno-1] )		// a new Fryhtan kingdom: its starting Life Points
		life_points_array[nationRecno-1] = start_life_points(nationRecno);

	nation_monster_id[nationRecno-1] = monsterId;
}
//----------- End of function ModFryhtan::set_nation_monster -----------//


//--------- Begin of function ModFryhtan::monster_id ---------//
//
// Return the Fryhtan species of the nation, or 0 if it is a human nation.
//
int ModFryhtan::monster_id(int nationRecno)
{
	if( nationRecno < 1 || nationRecno > MAX_NATION )
		return 0;

	return nation_monster_id[nationRecno-1];
}
//----------- End of function ModFryhtan::monster_id -----------//


//--------- Begin of function ModFryhtan::nation_unit_id ---------//
//
int ModFryhtan::nation_unit_id(int nationRecno)
{
	int monsterId = monster_id(nationRecno);

	return monsterId ? monster_res[monsterId]->unit_id : 0;
}
//----------- End of function ModFryhtan::nation_unit_id -----------//


//--------- Begin of function ModFryhtan::create_start_king ---------//
//
// A Fryhtan kingdom starts with its king (an All High of its species) in a
// lair with its first soldiers, and an enslaved town of a random race linked
// to the lair, at a place chosen like the starting town of a human kingdom.
//
// return: <int> the unit recno of the king, 0 if there is no space.
//
int ModFryhtan::create_start_king(int nationRecno)
{
	int monsterId = monster_id(nationRecno);

	if( !monsterId )
		return 0;

	int unitId = monster_res[monsterId]->unit_id;

	//------ locate a place like the starting town of a kingdom -----//

	int xLoc, yLoc;

	if( !town_array.think_town_loc(MAX_WORLD_X_LOC*MAX_WORLD_Y_LOC, xLoc, yLoc) )
		return 0;

	//---- the enslaved town, where a human kingdom has its starting town ----//

	int townRaceId = misc.random(MAX_RACE)+1;

	int townRecno = town_array.add_town(nationRecno, townRaceId, xLoc, yLoc);

	town_array[townRecno]->init_pop( townRaceId, FRYHTAN_START_TOWN_POPULATION, ENSLAVED_TOWN_LOYALTY, 0, 1 );		// last 1-first initialization at the beginning of the game
	town_array[townRecno]->auto_set_layout();

	//------ build the starting lair next to it, where a human kingdom has its starting fort (a finished Fort, free of charge) ------//

	xLoc += 6;

	if( !world.can_build_firm(xLoc, yLoc, FIRM_CAMP) )
		return 0;

	Nation* nationPtr = nation_array[nationRecno];

	int firmRecno = firm_array.create_firm(FIRM_CAMP);

	firm_array[firmRecno]->init( xLoc, yLoc, nationRecno, FIRM_CAMP, race_res[nationPtr->race_id]->code );		// Firm::init() marks it as a lair of the kingdom's species, and links it to the town

	Firm* firmPtr = firm_array[firmRecno];

	firmPtr->complete_construction();

	//---------- create the king next to the lair ----------//

	SpriteInfo* spriteInfo = sprite_res[ unit_res[unitId]->sprite_id ];

	xLoc = firmPtr->loc_x1;
	yLoc = firmPtr->loc_y1;

	if( !world.locate_space( &xLoc, &yLoc, firmPtr->loc_x2, firmPtr->loc_y2, spriteInfo->loc_width, spriteInfo->loc_height,
									 unit_res[unitId]->mobile_type ) )
	{
		return 0;
	}

	int unitRecno = unit_array.add_unit( unitId, nationRecno, RANK_KING, 100, xLoc, yLoc );

	if( !unitRecno )
		return 0;

	Unit* unitPtr = unit_array[unitRecno];

	unitPtr->set_monster_id(monsterId);
	unitPtr->skill.set_skill(SKILL_LEADING);
	unitPtr->skill.skill_level = FRYHTAN_KING_LEADERSHIP;
	unitPtr->set_combat_level(FRYHTAN_KING_COMBAT);
	unitPtr->hit_points = (float) unitPtr->max_hit_points;

	nationPtr->set_king(unitRecno, 1);		// 1-the first king of the nation

	//------ the king commands the lair, with its first soldiers ------//

	firmPtr->assign_overseer(unitRecno);

	for( int i=0 ; i<FRYHTAN_START_SOLDIER_COUNT ; i++ )
		add_lair_soldier(firmPtr, monsterId);

	return unitRecno;
}
//----------- End of function ModFryhtan::create_start_king -----------//


//--------- Begin of function ModFryhtan::set_lair ---------//
//
// Mark a Fort of a Fryhtan kingdom as a lair of the given species.
//
void ModFryhtan::set_lair(int firmRecno, int monsterId)
{
	if( firmRecno < 1 )
		return;

	if( firmRecno > lair_array_size )
	{
		int newSize = firmRecno + 50;

		lair_array = (FryhtanLair*) mem_resize( lair_array, sizeof(FryhtanLair) * newSize );
		memset( lair_array+lair_array_size, 0, sizeof(FryhtanLair) * (newSize-lair_array_size) );
		lair_array_size = newSize;
	}

	FryhtanLair* lairPtr = lair_array+firmRecno-1;

	memset( lairPtr, 0, sizeof(FryhtanLair) );
	lairPtr->monster_id = monsterId;
}
//----------- End of function ModFryhtan::set_lair -----------//


//--------- Begin of function ModFryhtan::del_lair ---------//
//
void ModFryhtan::del_lair(int firmRecno)
{
	if( firmRecno >= 1 && firmRecno <= lair_array_size )
		memset( lair_array+firmRecno-1, 0, sizeof(FryhtanLair) );
}
//----------- End of function ModFryhtan::del_lair -----------//


//--------- Begin of function ModFryhtan::get_lair ---------//
//
FryhtanLair* ModFryhtan::get_lair(int firmRecno)
{
	if( firmRecno < 1 || firmRecno > lair_array_size )
		return NULL;

	FryhtanLair* lairPtr = lair_array+firmRecno-1;

	return lairPtr->monster_id ? lairPtr : NULL;
}
//----------- End of function ModFryhtan::get_lair -----------//


//--------- Begin of function ModFryhtan::unit_monster_id ---------//
//
// return: <int> the Fryhtan species of the given unit type, 0 if it isn't a
//               Fryhtan of a species
//
int ModFryhtan::unit_monster_id(int unitId)
{
	for( int monsterId=1 ; monsterId<=monster_res.monster_count ; monsterId++ )
	{
		if( monster_res[monsterId]->unit_id == unitId )
			return monsterId;
	}

	return 0;
}
//----------- End of function ModFryhtan::unit_monster_id -----------//


//--------- Begin of function ModFryhtan::lair_monster_id ---------//
//
int ModFryhtan::lair_monster_id(int firmRecno)
{
	FryhtanLair* lairPtr = get_lair(firmRecno);

	return lairPtr ? lairPtr->monster_id : 0;
}
//----------- End of function ModFryhtan::lair_monster_id -----------//


//--------- Begin of function ModFryhtan::is_lair ---------//
//
int ModFryhtan::is_lair(Firm* firmPtr)
{
	return firmPtr->firm_id == FIRM_CAMP && lair_monster_id(firmPtr->firm_recno) != 0;
}
//----------- End of function ModFryhtan::is_lair -----------//


//--------- Begin of function ModFryhtan::lair_unit_id ---------//
//
int ModFryhtan::lair_unit_id(int firmRecno)
{
	int monsterId = lair_monster_id(firmRecno);

	return monsterId ? monster_res[monsterId]->unit_id : 0;
}
//----------- End of function ModFryhtan::lair_unit_id -----------//


//--------- Begin of function ModFryhtan::lair_train_population ---------//
//
int ModFryhtan::lair_train_population(int firmRecno)
{
	int monsterId = lair_monster_id(firmRecno);

	return monsterId ? monster_population(monsterId) : 0;
}
//----------- End of function ModFryhtan::lair_train_population -----------//


//--------- Begin of function ModFryhtan::can_train_in_lair ---------//
//
// Whether one more Fryhtan can be queued for training in the lair: enough
// food, free population and room in the queue.
//
int ModFryhtan::can_train_in_lair(int firmRecno, int trainType)
{
	FryhtanLair* lairPtr = get_lair(firmRecno);

	if( !lairPtr || firm_array.is_deleted(firmRecno) )
		return 0;

	Firm* firmPtr = firm_array[firmRecno];

	if( firmPtr->under_construction || !firmPtr->nation_recno )
		return 0;

	if( lairPtr->train_count >= LAIR_MAX_TRAIN_QUEUE )
		return 0;

	// a soldier is trained even if the lair is full: it stays outside, next to the lair (add_lair_soldier)

	if( nation_array[firmPtr->nation_recno]->food < train_food(trainType) )
		return 0;

	if( nation_array[firmPtr->nation_recno]->cash < train_cost(lairPtr->monster_id, firmPtr->nation_recno) )
		return 0;

	if( life_points(firmPtr->nation_recno) < train_life_points(trainType, lairPtr->monster_id, firmPtr->nation_recno) )
		return 0;

	if( FRYHTAN_POPULATION_LIMIT &&
		 nation_population(firmPtr->nation_recno) + lair_train_population(firmRecno) >
		 nation_population_cap(firmPtr->nation_recno) )
	{
		return 0;
	}

	return 1;
}
//----------- End of function ModFryhtan::can_train_in_lair -----------//


//--------- Begin of function ModFryhtan::add_lair_train ---------//
//
// Queue one Fryhtan for training in the lair. The gold and the food are paid now.
//
int ModFryhtan::add_lair_train(int firmRecno, int trainType)
{
	if( !can_train_in_lair(firmRecno, trainType) )
		return 0;

	FryhtanLair* lairPtr = get_lair(firmRecno);
	Firm* firmPtr = firm_array[firmRecno];

	nation_array[firmPtr->nation_recno]->consume_food( (float) train_food(trainType) );
	nation_array[firmPtr->nation_recno]->add_expense( EXPENSE_TRAIN_UNIT, (float) train_cost(lairPtr->monster_id, firmPtr->nation_recno) );
	add_life_points( firmPtr->nation_recno, -train_life_points(trainType, lairPtr->monster_id, firmPtr->nation_recno) );

	if( trainType == LAIR_TRAIN_ORDO )
		lairPtr->ordo_mask |= (1 << lairPtr->train_count);

	lairPtr->train_count++;
	return 1;
}
//----------- End of function ModFryhtan::add_lair_train -----------//


//--------- Begin of function ModFryhtan::cancel_lair_train ---------//
//
// Remove the last Fryhtan of the given type (soldier or Ordo) from the
// training queue, returning its gold and food.
//
int ModFryhtan::cancel_lair_train(int firmRecno, int trainType)
{
	FryhtanLair* lairPtr = get_lair(firmRecno);

	if( !lairPtr || lairPtr->train_count <= 0 )
		return 0;

	//------ the last one of this type in the queue ------//

	int i;

	for( i=lairPtr->train_count-1 ; i>=0 ; i-- )
	{
		if( lairPtr->train_type(i) == trainType )
			break;
	}

	if( i<0 )
		return 0;

	Firm* firmPtr = firm_array[firmRecno];

	nation_array[firmPtr->nation_recno]->add_food( (float) train_food(trainType) );
	nation_array[firmPtr->nation_recno]->add_expense( EXPENSE_TRAIN_UNIT, (float) -train_cost(lairPtr->monster_id, firmPtr->nation_recno) );		// the gold back
	add_life_points( firmPtr->nation_recno, train_life_points(trainType, lairPtr->monster_id, firmPtr->nation_recno) );		// the Life Points back

	//------ remove it, the ones after it move one place forward ------//

	int lowBits = lairPtr->ordo_mask & ((1 << i) - 1);

	lairPtr->ordo_mask = (int16_t) ( lowBits | ((lairPtr->ordo_mask >> (i+1)) << i) );

	lairPtr->train_count--;

	if( i==0 )		// the one being trained
		lairPtr->train_days = 0;

	return 1;
}
//----------- End of function ModFryhtan::cancel_lair_train -----------//


//--------- Begin of function ModFryhtan::next_day_lair ---------//
//
// Called by FirmCamp::next_day() of a lair. When a Fryhtan has finished its
// training it comes out next to the lair and goes into it if there is room.
//
void ModFryhtan::next_day_lair(int firmRecno)
{
	FryhtanLair* lairPtr = get_lair(firmRecno);

	if( !lairPtr )
		return;

	Firm* firmPtr = firm_array[firmRecno];

	next_day_alchemist(firmPtr);		// a lair on a raw material site

	if( lairPtr->train_count <= 0 )
		return;

	if( firmPtr->under_construction )
		return;

	int trainType = lairPtr->train_type(0);

	if( ++lairPtr->train_days < train_days(trainType, lairPtr->monster_id) )
		return;

	if( trainType == LAIR_TRAIN_ORDO )
	{
		if( !add_lair_ordo(firmPtr, lairPtr->monster_id, lair_train_combat(firmPtr->nation_recno, trainType)) )
			return;		// no space next to the lair, try again the next day
	}
	else
	{
		if( !add_lair_soldier(firmPtr, lairPtr->monster_id, lair_train_combat(firmPtr->nation_recno, trainType)) )
			return;
	}

	//--- the "ready" sound of the human units (the Fryhtans don't have one), also played when a war factory finishes a weapon ---//

	if( firmPtr->nation_recno == nation_array.player_recno )
		se_res.far_sound( firmPtr->center_x, firmPtr->center_y, 1, 'F', FIRM_WAR_FACTORY, "FINS" );

	lairPtr->train_days = 0;
	lairPtr->train_count--;
	lairPtr->ordo_mask = (int16_t) ( (lairPtr->ordo_mask >> 1) & 0x7FFF );
}
//----------- End of function ModFryhtan::next_day_lair -----------//


//--------- Begin of function ModFryhtan::add_lair_soldier ---------//
//
// Add a Fryhtan soldier (combat LAIR_TRAIN_COMBAT by default) of the given
// species next to a lair. It goes into the lair if there is room.
//
// return: <int> the unit recno, 0 if there is no space for it
//
int ModFryhtan::add_lair_soldier(Firm* firmPtr, int monsterId, int combatLevel)
{
	//------ locate a space next to the lair ------//

	int unitId = monster_res[monsterId]->unit_id;
	SpriteInfo* spriteInfo = sprite_res[ unit_res[unitId]->sprite_id ];

	int xLoc=firmPtr->loc_x1, yLoc=firmPtr->loc_y1;

	if( !world.locate_space( &xLoc, &yLoc, firmPtr->loc_x2, firmPtr->loc_y2,
									 spriteInfo->loc_width, spriteInfo->loc_height, unit_res[unitId]->mobile_type ) )
	{
		return 0;
	}

	//------ add the Fryhtan ------//

	int unitRecno = unit_array.add_unit( unitId, firmPtr->nation_recno, RANK_SOLDIER, 100, xLoc, yLoc );

	if( !unitRecno )
		return 0;

	Unit* unitPtr = unit_array[unitRecno];

	unitPtr->set_monster_id( monsterId );
	unitPtr->set_combat_level( combatLevel );
	unitPtr->hit_points = (float) unitPtr->max_hit_points;
	unitPtr->skill.set_skill( SKILL_LEADING );		// it can be promoted to Ordo
	unitPtr->skill.skill_level = LAIR_TRAIN_LEADERSHIP;

	//------ it goes into the lair if there is room ------//

	if( firmPtr->worker_count < firmPtr->max_worker_count() )
		firmPtr->assign_unit(unitRecno);

	return unitRecno;
}
//----------- End of function ModFryhtan::add_lair_soldier -----------//


//--------- Begin of function ModFryhtan::add_lair_ordo ---------//
//
// Add an Ordo (a general, leadership LAIR_ORDO_LEADERSHIP and combat
// LAIR_ORDO_COMBAT) of the given species next to a lair, with a name of the
// list. It stays outside the lair.
//
// return: <int> the unit recno, 0 if there is no space for it
//
int ModFryhtan::add_lair_ordo(Firm* firmPtr, int monsterId, int combatLevel)
{
	int unitId = monster_res[monsterId]->unit_id;
	SpriteInfo* spriteInfo = sprite_res[ unit_res[unitId]->sprite_id ];

	int xLoc=firmPtr->loc_x1, yLoc=firmPtr->loc_y1;

	if( !world.locate_space( &xLoc, &yLoc, firmPtr->loc_x2, firmPtr->loc_y2,
									 spriteInfo->loc_width, spriteInfo->loc_height, unit_res[unitId]->mobile_type ) )
	{
		return 0;
	}

	int unitRecno = unit_array.add_unit( unitId, firmPtr->nation_recno, RANK_SOLDIER, 100, xLoc, yLoc );

	if( !unitRecno )
		return 0;

	Unit* unitPtr = unit_array[unitRecno];

	unitPtr->set_monster_id( monsterId );
	unitPtr->set_rank( RANK_GENERAL );
	unitPtr->skill.set_skill( SKILL_LEADING );
	unitPtr->skill.skill_level = LAIR_ORDO_LEADERSHIP;
	unitPtr->set_combat_level( combatLevel );
	unitPtr->hit_points = (float) unitPtr->max_hit_points;
	unitPtr->name_id = (uint16_t) new_ordo_name_id( firmPtr->nation_recno );

	return unitRecno;
}
//----------- End of function ModFryhtan::add_lair_ordo -----------//


//--------- Begin of function FryhtanLair::type_count ---------//
//
// No. of Fryhtans of the given type (soldiers or Ordos) in the queue.
//
int FryhtanLair::type_count(int trainType)
{
	int count = 0;

	for( int i=0 ; i<train_count ; i++ )
	{
		if( train_type(i) == trainType )
			count++;
	}

	return count;
}
//----------- End of function FryhtanLair::type_count -----------//


//------- the names of the Ordos (7kmod/FRYHTAN_NAMES.md) -------//

static const char* ordo_name_array[] =
{
	"Akhrul", "Bazgor", "Brakkas", "Drovuk", "Durmog",
	"Ekzarr", "Feskul", "Ghaddrin", "Gharvok", "Ghulmar",
	"Gorrath", "Grevvuk", "Hrozzul", "Ikthar", "Izzrak",
	"Jarvuk", "Kaggoth", "Karzul", "Khavrek", "Kolgrim",
	"Krathos", "Kruvak", "Kzorr", "Lughaz", "Malvok",
	"Mordrek", "Murzak", "Naggol", "Nekrath", "Nurvog",
	"Oggrath", "Orzuk", "Phazzar", "Qorrak", "Rakthul",
	"Razzug", "Rhovok", "Rukmar", "Sarvog", "Shagrul",
	"Skorra", "Skulvar", "Sorgath", "Thrakk", "Thulzar",
	"Tograk", "Trezzul", "Ubrakk", "Ulgrath", "Urzog",
	"Vakkor", "Varguul", "Vazzrik", "Vorrak", "Vruzzol",
	"Waggrul", "Xarvok", "Xoggul", "Yazzrak", "Yurgoth",
	"Zagrath", "Zarkul", "Zhovrek", "Zogmar", "Zurrak",
};

enum { ORDO_NAME_COUNT = sizeof(ordo_name_array) / sizeof(ordo_name_array[0]) };


//--------- Begin of function ModFryhtan::ordo_name ---------//
//
const char* ModFryhtan::ordo_name(Unit* unitPtr)
{
	if( unitPtr->race_id || unitPtr->rank_id != RANK_GENERAL )
		return NULL;

	return ordo_name_by_id(unitPtr->name_id);
}


const char* ModFryhtan::ordo_name_by_id(int nameId)
{
	int nameIndex = nameId - ORDO_NAME_ID_BASE;

	if( nameIndex < 1 || nameIndex > ORDO_NAME_COUNT )
		return NULL;

	return ordo_name_array[nameIndex-1];
}
//----------- End of function ModFryhtan::ordo_name -----------//


//--------- Begin of function ModFryhtan::new_ordo_name_id ---------//
//
// A random name of the list that no Ordo of the kingdom is using (any name if
// all are in use).
//
// return: <int> the Unit::name_id for it
//
int ModFryhtan::new_ordo_name_id(int nationRecno)
{
	char usedArray[ORDO_NAME_COUNT];

	memset( usedArray, 0, sizeof(usedArray) );

	int usedCount = 0;

	for( int i=unit_array.size() ; i>0 ; i-- )
	{
		if( unit_array.is_deleted(i) )
			continue;

		Unit* unitPtr = unit_array[i];

		if( unitPtr->nation_recno != nationRecno || !ordo_name(unitPtr) )
			continue;

		int nameIndex = unitPtr->name_id - ORDO_NAME_ID_BASE;

		if( !usedArray[nameIndex-1] )
		{
			usedArray[nameIndex-1] = 1;
			usedCount++;
		}
	}

	for( int t=1 ; t<=town_ordo_array_size ; t++ )		// the Ordos inside enslaved towns
	{
		TownOrdo* ordoPtr = town_ordo(t);

		if( !ordoPtr || ordoPtr->nation_recno != nationRecno )
			continue;

		int nameIndex = ordoPtr->name_id - ORDO_NAME_ID_BASE;

		if( nameIndex >= 1 && nameIndex <= ORDO_NAME_COUNT && !usedArray[nameIndex-1] )
		{
			usedArray[nameIndex-1] = 1;
			usedCount++;
		}
	}

	if( usedCount >= ORDO_NAME_COUNT )		// all in use
		return ORDO_NAME_ID_BASE + 1 + misc.random(ORDO_NAME_COUNT);

	int pick = misc.random(ORDO_NAME_COUNT - usedCount);

	for( int i=0 ; i<ORDO_NAME_COUNT ; i++ )
	{
		if( usedArray[i] )
			continue;

		if( pick-- == 0 )
			return ORDO_NAME_ID_BASE + 1 + i;
	}

	return ORDO_NAME_ID_BASE + 1;
}
//----------- End of function ModFryhtan::new_ordo_name_id -----------//


//--------- Begin of function ModFryhtan::draw_lair ---------//
//
// Draw a lair of a Fryhtan kingdom: the lair of its species, centered on
// the 3x3 Fort (the lair graphics are 4x4), with the flags of the kingdom.
// While it is being built only the ground of the lair is shown.
//
void ModFryhtan::draw_lair(Firm* firmPtr, int displayLayer)
{
	MonsterInfo* monsterInfo = monster_res[ lair_monster_id(firmPtr->firm_recno) ];
	FirmBuild*   firmBuild   = firm_res.get_build( firm_res[FIRM_MONSTER]->get_build_id(monsterInfo->firm_build_code) );

	int absX = firmPtr->loc_x1 * ZOOM_LOC_WIDTH - ZOOM_LOC_WIDTH/2;
	int absY = firmPtr->loc_y1 * ZOOM_LOC_HEIGHT - ZOOM_LOC_HEIGHT/2;

	char* colorRemapTable = game.get_color_remap_table(firmPtr->nation_recno, firm_array.selected_recno == firmPtr->firm_recno);

	if( firmBuild->ground_bitmap_recno )
		firm_res.get_bitmap(firmBuild->ground_bitmap_recno)->draw_at(absX, absY, NULL, displayLayer);

	//------- the flags of the kingdom at the top corners -------//

	#define LAIR_FLAG_WIDTH 9

	char *flagBitmapPtr = image_spict.get_ptr("FLAG-S0");
	int drawY = firmPtr->loc_y1 * ZOOM_LOC_HEIGHT - World::view_top_y + ZOOM_Y1;
	int drawX = firmPtr->loc_x1 * ZOOM_LOC_WIDTH - World::view_top_x + ZOOM_X1;

	world.zoom_matrix->put_bitmap_remap_clip(drawX, drawY, flagBitmapPtr, colorRemapTable, 1);	// 1-the bitmap is compressed

	drawX = (firmPtr->loc_x2+1) * ZOOM_LOC_WIDTH - LAIR_FLAG_WIDTH - World::view_top_x + ZOOM_X1;
	world.zoom_matrix->put_bitmap_remap_clip(drawX, drawY, flagBitmapPtr, colorRemapTable, 1);

	//------- the lair -------//

	if( firmPtr->under_construction )
		return;

	FirmBitmap* firmBitmap = firm_res.get_bitmap( firmBuild->first_bitmap(1) );

	if( firmBitmap )
		firmBitmap->draw_at(absX, absY, colorRemapTable, displayLayer);
}
//----------- End of function ModFryhtan::draw_lair -----------//


//--------- Begin of static function closest_color ---------//
//
// The color of the game palette closest to the given one (not the
// transparent one), weighting green the most, as the eye does.
//
static int closest_color(int red, int green, int blue)
{
	int bestColor = 0, bestDistance = 0x7FFFFFFF;

	for( int i=0 ; i<TRANSPARENT_CODE ; i++ )
	{
		int dr = vga.get_game_pal()[i].r - red;
		int dg = vga.get_game_pal()[i].g - green;
		int db = vga.get_game_pal()[i].b - blue;
		int distance = dr*dr*3 + dg*dg*4 + db*db*2;

		if( distance < bestDistance )
		{
			bestDistance = distance;
			bestColor = i;
		}
	}

	return bestColor;
}
//----------- End of static function closest_color -----------//


//--------- Begin of function ModFryhtan::lair_icon ---------//
//
// The lair of a species as it is drawn on the map (its ground and its first
// frame, in the colors of the nation), scaled down to fit in
// LAIR_ICON_MAX_WIDTH x LAIR_ICON_MAX_HEIGHT, for the icons of the
// interface. It is drawn once on the map area of the screen over a color
// marked as transparent, read back and scaled; the map is drawn again on the
// next frame. Kept for each species.
//
// return: <char*> a bitmap (<short> width, <short> height, pixels), 255 is
//                 transparent; NULL if the species has no lair graphics
//
char* ModFryhtan::lair_icon(int monsterId, int nationRecno)
{
	enum { ICON_BUF_SIZE = 2*sizeof(short) + LAIR_ICON_MAX_WIDTH*LAIR_ICON_MAX_HEIGHT };

	static char iconBufArray[FRYHTAN_SPECIES_COUNT][ICON_BUF_SIZE];
	static char iconDoneArray[FRYHTAN_SPECIES_COUNT];

	if( monsterId < 1 || monsterId > FRYHTAN_SPECIES_COUNT || monsterId > monster_res.monster_count )
		return NULL;

	char* iconBuf = iconBufArray[monsterId-1];

	if( iconDoneArray[monsterId-1] )
		return iconBuf;

	//------ the bitmaps of the lair and the box around them ------//

	MonsterInfo* monsterInfo = monster_res[monsterId];
	FirmBuild*   firmBuild   = firm_res.get_build( firm_res[FIRM_MONSTER]->get_build_id(monsterInfo->firm_build_code) );

	FirmBitmap* bitmapArray[20];
	int bitmapCount = 0;

	if( firmBuild->ground_bitmap_recno )
		bitmapArray[bitmapCount++] = firm_res.get_bitmap(firmBuild->ground_bitmap_recno);

	for( int i=0 ; i<firmBuild->bitmap_count_array[0] && bitmapCount<20 ; i++ )
		bitmapArray[bitmapCount++] = firm_res.get_bitmap( firmBuild->first_bitmap(1) + i );

	int minX=0x7FFF, minY=0x7FFF, maxX=-0x7FFF, maxY=-0x7FFF;

	for( int i=0 ; i<bitmapCount ; i++ )
	{
		FirmBitmap* bitmapPtr = bitmapArray[i];

		minX = MIN( minX, bitmapPtr->offset_x );
		minY = MIN( minY, bitmapPtr->offset_y );
		maxX = MAX( maxX, bitmapPtr->offset_x + bitmapPtr->width );
		maxY = MAX( maxY, bitmapPtr->offset_y + bitmapPtr->height );
	}

	int width  = maxX - minX;
	int height = maxY - minY;

	if( bitmapCount==0 || width <= 0 || height <= 0 || width > ZOOM_WIDTH || height > ZOOM_HEIGHT )
		return NULL;

	//------ draw it on the map area over the transparent color and read it back ------//

	int x1 = ZOOM_X1, y1 = ZOOM_Y1;
	int x2 = x1+width-1, y2 = y1+height-1;

	char* readBuf = mem_add( 2*sizeof(short) + width*height );
	char* colorRemapTable = game.get_color_remap_table(nationRecno, 0);

	mouse.hide_area( x1, y1, x2, y2 );

	vga_front.bar( x1, y1, x2, y2, TRANSPARENT_CODE );

	for( int i=0 ; i<bitmapCount ; i++ )
	{
		FirmBitmap* bitmapPtr = bitmapArray[i];

		vga_front.put_bitmap_trans_remap_decompress( x1 + bitmapPtr->offset_x - minX, y1 + bitmapPtr->offset_y - minY,
			bitmapPtr->bitmap_ptr, colorRemapTable );
	}

	vga_front.read_bitmap( x1, y1, x2, y2, readBuf );

	mouse.show_area();

	//------ scale it down to fit, keeping its proportions ------//

	int iconWidth  = width, iconHeight = height;

	if( iconWidth > LAIR_ICON_MAX_WIDTH || iconHeight > LAIR_ICON_MAX_HEIGHT )
	{
		if( width * LAIR_ICON_MAX_HEIGHT > height * LAIR_ICON_MAX_WIDTH )
		{
			iconWidth  = LAIR_ICON_MAX_WIDTH;
			iconHeight = MAX( 1, height * LAIR_ICON_MAX_WIDTH / width );
		}
		else
		{
			iconHeight = LAIR_ICON_MAX_HEIGHT;
			iconWidth  = MAX( 1, width * LAIR_ICON_MAX_HEIGHT / height );
		}
	}

	*(short*) iconBuf = iconWidth;
	*((short*) iconBuf + 1) = iconHeight;

	//--- each pixel of the icon averages the colors of its block of the lair, so the edges stay solid; ---//
	//--- the shadow is dithered (one pixel of SHADOW_CODE out of two), it is made a solid dark gray    ---//

	unsigned char* srcPtr  = (unsigned char*) readBuf + 2*sizeof(short);
	unsigned char* destPtr = (unsigned char*) iconBuf + 2*sizeof(short);

	int shadowColor = closest_color(98, 95, 89);		// the stone of the buttons at half its brightness

	for( int y=0 ; y<iconHeight ; y++ )
	{
		int srcY1 = y*height/iconHeight, srcY2 = MAX( srcY1+1, (y+1)*height/iconHeight );

		for( int x=0 ; x<iconWidth ; x++ )
		{
			int srcX1 = x*width/iconWidth, srcX2 = MAX( srcX1+1, (x+1)*width/iconWidth );
			int totalCount=0, colorCount=0, shadowCount=0, red=0, green=0, blue=0;

			for( int sy=srcY1 ; sy<srcY2 ; sy++ )
			{
				for( int sx=srcX1 ; sx<srcX2 ; sx++ )
				{
					int pixel = srcPtr[sy*width + sx];

					totalCount++;

					if( pixel == TRANSPARENT_CODE )
						continue;

					if( pixel == SHADOW_CODE )
					{
						shadowCount++;
						continue;
					}

					colorCount++;
					red   += vga.get_game_pal()[pixel].r;
					green += vga.get_game_pal()[pixel].g;
					blue  += vga.get_game_pal()[pixel].b;
				}
			}

			//--- covered: the colors, plus the shadow counted twice (it's dithered), at least half of the block ---//

			int coverCount = colorCount + MIN( totalCount-colorCount, shadowCount*2 );
			int pixel = TRANSPARENT_CODE;

			if( coverCount*2 >= totalCount )
			{
				if( colorCount*2 >= coverCount )
					pixel = closest_color( red/colorCount, green/colorCount, blue/colorCount );
				else
					pixel = shadowColor;
			}

			destPtr[y*iconWidth + x] = (unsigned char) pixel;
		}
	}

	mem_del( readBuf );

	iconDoneArray[monsterId-1] = 1;

	return iconBuf;
}
//----------- End of function ModFryhtan::lair_icon -----------//


//--------- Begin of function ModFryhtan::disp_lair_button ---------//
//
// A 99x60 button picture, as the build buttons: the frame of the Fort build
// button, darkened, with the lair of the kingdom's species (lair_icon()) in
// the middle, the label at the bottom and, with <withMine>, the Mine build
// button at a third of its size in the top left corner (Alchemist Lair,
// without a label).
//
void ModFryhtan::disp_lair_button(int x, int y, int nationRecno, int monsterId, const char* label, int withMine)
{
	enum { BUTTON_WIDTH = 99, BUTTON_HEIGHT = 60 };

	static char mineBuf[2*sizeof(short) + BUTTON_WIDTH*BUTTON_HEIGHT];
	static char smallMineBuf[2*sizeof(short) + (BUTTON_WIDTH/3)*(BUTTON_HEIGHT/3)];
	static int  smallMineDone = 0;

	char* remapTable = game.get_color_remap_table(nationRecno, 0);
	String str;

	//--- the Mine button at a third of its size, made once: drawn here, read back and scaled down ---//

	if( withMine && !smallMineDone )
	{
		str  = "F-";
		str += FIRM_MINE;

		mouse.hide_area( x, y, x+BUTTON_WIDTH-1, y+BUTTON_HEIGHT-1 );
		vga_front.put_bitmap_trans_remap_decompress( x, y, image_button.get_ptr(str), remapTable );
		vga_front.read_bitmap( x, y, x+BUTTON_WIDTH-1, y+BUTTON_HEIGHT-1, mineBuf );
		mouse.show_area();

		int smallWidth = BUTTON_WIDTH/3, smallHeight = BUTTON_HEIGHT/3;

		*(short*) smallMineBuf = smallWidth;
		*((short*) smallMineBuf + 1) = smallHeight;

		char* srcPtr  = mineBuf + 2*sizeof(short);
		char* destPtr = smallMineBuf + 2*sizeof(short);

		for( int j=0 ; j<smallHeight ; j++ )
		{
			for( int i=0 ; i<smallWidth ; i++ )
				destPtr[j*smallWidth+i] = srcPtr[(j*3+1)*BUTTON_WIDTH + i*3+1];
		}

		smallMineDone = 1;
	}

	//--- the Fort button, its picture and its name covered with its plain stone ---//
	//--- (a patch of its bottom right part, repeated over the inside)           ---//

	str  = "F-";
	str += FIRM_CAMP;

	mouse.hide_area( x, y, x+BUTTON_WIDTH-1, y+BUTTON_HEIGHT-1 );

	vga_front.put_bitmap_trans_remap_decompress( x, y, image_button.get_ptr(str), remapTable );

	enum { PATCH_X = 69, PATCH_Y = 23, PATCH_SIZE = 24,		// a plain part of the Fort button
			 INSIDE_X1 = 4, INSIDE_Y1 = 4, INSIDE_X2 = BUTTON_WIDTH-5, INSIDE_Y2 = BUTTON_HEIGHT-5 };

	char patchBuf[2*sizeof(short) + PATCH_SIZE*PATCH_SIZE];

	vga_front.read_bitmap( x+PATCH_X, y+PATCH_Y, x+PATCH_X+PATCH_SIZE-1, y+PATCH_Y+PATCH_SIZE-1, patchBuf );

	for( int py=INSIDE_Y1 ; py<=INSIDE_Y2 ; py+=PATCH_SIZE )
	{
		for( int px=INSIDE_X1 ; px<=INSIDE_X2 ; px+=PATCH_SIZE )
		{
			int areaX2 = MIN( PATCH_SIZE-1, INSIDE_X2-px );
			int areaY2 = MIN( PATCH_SIZE-1, INSIDE_Y2-py );

			vga_front.put_bitmap_area( x+px, y+py, patchBuf, 0, 0, areaX2, areaY2 );
		}
	}

	//--- the lair of the kingdom, in the middle ---//

	if( !monsterId )
		monsterId = mod_fryhtan.nation_monster_id[nationRecno-1];

	char* lairIcon = lair_icon( monsterId, nationRecno );

	if( lairIcon )
	{
		int iconWidth  = *(short*) lairIcon;
		int iconHeight = *((short*) lairIcon + 1);

		vga_front.put_bitmap_trans( x+(BUTTON_WIDTH-iconWidth)/2, y+(BUTTON_HEIGHT-iconHeight)/2, lairIcon );
	}

	//--- the Mine in small, in the top left corner, with a frame ---//

	if( withMine )
	{
		vga_front.put_bitmap( x+4, y+4, smallMineBuf );
		vga_front.rect( x+3, y+3, x+4+BUTTON_WIDTH/3, y+4+BUTTON_HEIGHT/3, 1, V_BLACK );
	}

	if( label && label[0] )
		font_san.put( x+8, y+BUTTON_HEIGHT-20, label );

	mouse.show_area();
}
//----------- End of function ModFryhtan::disp_lair_button -----------//


//--------- Begin of function ModFryhtan::firm_setup_cost ---------//
//
int ModFryhtan::firm_setup_cost(int firmId, int nationRecno)
{
	if( firmId == FIRM_CAMP && mod_fryhtan.is_fryhtan(nationRecno) )
		return LAIR_SETUP_COST;

	return firm_res[firmId]->setup_cost;
}
//----------- End of function ModFryhtan::firm_setup_cost -----------//


//--------- Begin of function ModFryhtan::firm_year_cost ---------//
//
int ModFryhtan::firm_year_cost(Firm* firmPtr)
{
	if( mod_fryhtan.is_lair(firmPtr) )
		return LAIR_YEAR_COST;

	return firm_res[firmPtr->firm_id]->year_cost;
}
//----------- End of function ModFryhtan::firm_year_cost -----------//


//--------- Begin of function ModFryhtan::nation_population ---------//
//
// The Fryhtan population of a nation: each small Fryhtan counts 1 and each
// large one 2, including the king, the Fryhtans in its lairs and those in
// the training queues.
//
int ModFryhtan::nation_population(int nationRecno)
{
	int population = 0;
	int i;

	for( i=unit_array.size() ; i>0 ; i-- )
	{
		if( unit_array.is_deleted(i) )
			continue;

		Unit* unitPtr = unit_array[i];

		if( unitPtr->nation_recno == nationRecno &&
			 unit_res[unitPtr->unit_id]->unit_class == UNIT_CLASS_MONSTER )
		{
			population += unit_population(unitPtr->unit_id);
		}
	}

	for( i=firm_array.size() ; i>0 ; i-- )
	{
		if( firm_array.is_deleted(i) )
			continue;

		Firm* firmPtr = firm_array[i];

		if( firmPtr->nation_recno != nationRecno || firmPtr->firm_id != FIRM_CAMP )
			continue;

		for( int j=0 ; j<firmPtr->worker_count ; j++ )
		{
			int unitId = firmPtr->worker_array[j].unit_id;

			if( unit_res[unitId]->unit_class == UNIT_CLASS_MONSTER )
				population += unit_population(unitId);
		}

		FryhtanLair* lairPtr = get_lair(i);

		if( lairPtr )
			population += lairPtr->train_count * monster_population(lairPtr->monster_id);
	}

	for( i=1 ; i<=town_ordo_array_size ; i++ )		// the Ordos inside enslaved towns
	{
		TownOrdo* ordoPtr = town_ordo(i);

		if( ordoPtr && ordoPtr->nation_recno == nationRecno )
			population += unit_population(ordoPtr->unit_id);
	}

	return population;
}
//----------- End of function ModFryhtan::nation_population -----------//


//--------- Begin of function ModFryhtan::nation_population_cap ---------//
//
// The Fryhtan population allowed: 18, plus 1 for each person living in the
// enslaved towns of the nation (phase 4).
//
int ModFryhtan::nation_population_cap(int nationRecno)
{
	//--- each person of its enslaved towns raises the cap by 1 ---//

	int cap = FRYHTAN_BASE_POPULATION;

	for( int i=town_array.size() ; i>0 ; i-- )
	{
		if( town_array.is_deleted(i) )
			continue;

		Town* townPtr = town_array[i];

		if( townPtr->nation_recno == nationRecno )
			cap += townPtr->population;
	}

	return cap;
}
//----------- End of function ModFryhtan::nation_population_cap -----------//


//--------- Begin of function ModFryhtan::is_large_monster ---------//
//
int ModFryhtan::is_large_monster(int monsterId)
{
	if( monsterId == FRYHTAN_PFITH )		// small, although its level is 6
		return 0;

	return monster_res[monsterId]->level >= FRYHTAN_LARGE_MIN_LEVEL;
}
//----------- End of function ModFryhtan::is_large_monster -----------//


//--------- Begin of function ModFryhtan::lair_max_soldiers ---------//
//
// The places for soldiers in a lair, by the hit points of the species, so
// that a full lair has about the hit points of a full fort of humans
// (MAX_WORKER x 200): LAIR_PLACES_HIT_POINTS / hit points, rounded, at most
// MAX_WORKER. Rokken (400) 4; Wyrm, Droog, Haubudam, Sauroid 5; Holgh,
// Karrotten, Pfith 6; the others 8. The leader doesn't take a place.
//
int ModFryhtan::lair_max_soldiers(int monsterId)
{
	if( monsterId < 1 || monsterId > monster_res.monster_count )
		return MAX_WORKER;

	int hitPoints = unit_res[ monster_res[monsterId]->unit_id ]->hit_points;

	if( hitPoints <= 0 )
		return MAX_WORKER;

	int places = (LAIR_PLACES_HIT_POINTS + hitPoints/2) / hitPoints;

	return MAX( 1, MIN( (int) MAX_WORKER, places ) );
}
//----------- End of function ModFryhtan::lair_max_soldiers -----------//


//--------- Begin of function ModFryhtan::train_cost ---------//
//
// The gold for training a soldier or an Ordo of the species: $100 for the
// small ones, $150 for the large ones.
//
int ModFryhtan::train_cost(int monsterId, int nationRecno)
{
	return brood_mastery_price( is_large_monster(monsterId) ? LAIR_LARGE_TRAIN_COST : LAIR_SMALL_TRAIN_COST, nationRecno );
}
//----------- End of function ModFryhtan::train_cost -----------//


//--------- Begin of function ModFryhtan::promote_life_points ---------//
//
int ModFryhtan::promote_life_points(int nationRecno)
{
	return brood_mastery_price( LIFE_POINTS_PROMOTE, nationRecno );
}
//----------- End of function ModFryhtan::promote_life_points -----------//


//--------- Begin of function ModFryhtan::brood_mastery_price ---------//
//
// A cost of training or promoting with Brood Mastery: BROOD_MASTERY_PERCENT
// of it, rounded (10 -> 8, 9 -> 7, 8 -> 6, 7 -> 6, 5 -> 4, $150 -> $120).
//
int ModFryhtan::brood_mastery_price(int value, int nationRecno)
{
	if( !mod_fryhtan.has_tech(nationRecno, FRYHTAN_TECH_BROOD_MASTERY) )
		return value;

	return (value * BROOD_MASTERY_PERCENT + 50) / 100;
}
//----------- End of function ModFryhtan::brood_mastery_price -----------//


//--------- Begin of function ModFryhtan::train_life_points ---------//
//
// The Life Points for training a Fryhtan in a lair, by species: 7 for Ick,
// Doink and Broosken, 8 for Deezboanz, Rattus and Pfith, 9 for Karrotten,
// Holgh and Droog, 10 for the others. An Ordo costs 5 more. Less with Brood
// Mastery.
//
int ModFryhtan::train_life_points(int trainType, int monsterId, int nationRecno)
{
	static const char speciesLifePoints[] =		// by monster id (MONSTER.DBF), 1 to 13
	{
		8,		// Deezboanz
		8,		// Rattus
		7,		// Broosken
		10,	// Haubudam
		8,		// Pfith
		10,	// Rokken
		7,		// Doink
		10,	// Wyrm
		9,		// Droog
		7,		// Ick
		10,	// Sauroid
		9,		// Karrotten
		9,		// Holgh
	};

	int lifePoints = LIFE_POINTS_SOLDIER;

	if( monsterId >= 1 && monsterId <= (int) sizeof(speciesLifePoints) )
		lifePoints = speciesLifePoints[monsterId-1];

	if( trainType == LAIR_TRAIN_ORDO )
		lifePoints += LIFE_POINTS_ORDO_EXTRA;

	return brood_mastery_price( lifePoints, nationRecno );
}
//----------- End of function ModFryhtan::train_life_points -----------//


//--------- Begin of function ModFryhtan::monster_population ---------//
//
int ModFryhtan::monster_population(int monsterId)
{
	return is_large_monster(monsterId) ? FRYHTAN_LARGE_POPULATION : FRYHTAN_SMALL_POPULATION;
}
//----------- End of function ModFryhtan::monster_population -----------//


//--------- Begin of function ModFryhtan::unit_population ---------//
//
int ModFryhtan::unit_population(int unitId)
{
	MonsterInfo* monsterInfo = monster_res.get_monster_by_unit_id(unitId);

	return monsterInfo ? monster_population(monsterInfo->monster_id) : 1;
}
//----------- End of function ModFryhtan::unit_population -----------//


//--------- Begin of function ModFryhtan::monster_name ---------//
//
const char* ModFryhtan::monster_name(int monsterId)
{
	return _(monster_res[monsterId]->name);
}
//----------- End of function ModFryhtan::monster_name -----------//


//--------- Begin of function ModFryhtan::monster_size_text ---------//
//
const char* ModFryhtan::monster_size_text(int monsterId)
{
	return is_large_monster(monsterId) ? _("Large: $200 per unit") : _("Small: $150 per unit");
}
//----------- End of function ModFryhtan::monster_size_text -----------//


//------- the species for the new game screen -------//
//
// The same names and levels as MONSTER.DBF, indexed by monster id. The new
// game screen is displayed before MonsterRes is loaded.

static const char* menu_species_name_array[FRYHTAN_SPECIES_COUNT] =
{
	N_("Deezboanz"), N_("Rattus"), N_("Broosken"), N_("Haubudam"), N_("Pfith"), N_("Rokken"), N_("Doink"),
	N_("Wyrm"), N_("Droog"), N_("Ick"), N_("Sauroid"), N_("Karrotten"), N_("Holgh"),
};

static const char menu_species_level_array[FRYHTAN_SPECIES_COUNT] = { 5, 3, 5, 7, 6, 9, 5, 7, 7, 4, 7, 6, 6 };

static const char menu_species_order[FRYHTAN_SPECIES_COUNT] =		// monster ids in alphabetical order
{
	3,		// Broosken
	1,		// Deezboanz
	7,		// Doink
	9,		// Droog
	4,		// Haubudam
	13,	// Holgh
	10,	// Ick
	12,	// Karrotten
	5,		// Pfith
	2,		// Rattus
	6,		// Rokken
	11,	// Sauroid
	8,		// Wyrm
};

//--------- Begin of function ModFryhtan::menu_species_name ---------//
//
const char* ModFryhtan::menu_species_name(int monsterId)
{
	if( !is_valid_species(monsterId) )
		return "";

	return _(menu_species_name_array[monsterId-1]);
}
//----------- End of function ModFryhtan::menu_species_name -----------//


//--------- Begin of function ModFryhtan::menu_species_size_text ---------//
//
const char* ModFryhtan::menu_species_size_text(int monsterId)
{
	if( !is_valid_species(monsterId) )
		return "";

	return menu_species_level_array[monsterId-1] >= FRYHTAN_LARGE_MIN_LEVEL && monsterId != FRYHTAN_PFITH ?
			 _("Large: $200 per unit") : _("Small: $150 per unit");
}
//----------- End of function ModFryhtan::menu_species_size_text -----------//


//--------- Begin of function ModFryhtan::menu_next_species ---------//
//
int ModFryhtan::menu_next_species(int monsterId, int dir)
{
	int i;

	for( i=0 ; i<FRYHTAN_SPECIES_COUNT ; i++ )
	{
		if( menu_species_order[i] == monsterId )
			break;
	}

	if( i==FRYHTAN_SPECIES_COUNT )
		return FRYHTAN_DEFAULT_MONSTER_ID;

	i = (i + dir + FRYHTAN_SPECIES_COUNT) % FRYHTAN_SPECIES_COUNT;

	return menu_species_order[i];
}
//----------- End of function ModFryhtan::menu_species_next -----------//


//--------- Begin of function ModFryhtan::write_file ---------//
//
int ModFryhtan::write_file(File* filePtr)
{
	if( !filePtr->file_put_short_array( nation_monster_id, MAX_NATION ) )
		return 0;

	//------- the lairs -------//

	int lairCount = 0;		// write up to the last lair

	for( int i=lair_array_size ; i>0 ; i-- )
	{
		if( lair_array[i-1].monster_id )
		{
			lairCount = i;
			break;
		}
	}

	//--- the lair format with the Ordos in the queue: a -1 mark first (the lair count is never negative in the older format) ---//

	if( !filePtr->file_put_short( -1 ) || !filePtr->file_put_short( (int16_t) lairCount ) )
		return 0;

	if( lairCount > 0 &&
		 !filePtr->file_put_short_array( (int16_t*) lair_array, lairCount * (sizeof(FryhtanLair)/sizeof(int16_t)) ) )
	{
		return 0;
	}

	//------- the offers of the wild lairs (phase 5) -------//

	int offerCount = 0;		// write up to the last offer

	for( int i=offer_array_size ; i>0 ; i-- )
	{
		if( offer_array[i-1].monster_id )
		{
			offerCount = i;
			break;
		}
	}

	if( !filePtr->file_put_short( (int16_t) offerCount ) )
		return 0;

	if( offerCount > 0 &&
		 !filePtr->file_put_short_array( (int16_t*) offer_array, offerCount * (sizeof(LairOffer)/sizeof(int16_t)) ) )
	{
		return 0;
	}

	//------- the enslaved towns waiting to become independent: town recno and the day they lost their lairs -------//

	if( !write_town_dates( filePtr, lost_lair_date_array, lost_lair_date_array_size ) )
		return 0;

	//------- the day each town was last raided -------//

	if( !write_town_dates( filePtr, raid_date_array, raid_date_array_size ) )
		return 0;

	//------- the kings killed of each kingdom -------//

	if( !filePtr->file_write( king_kill_array, sizeof(king_kill_array) ) )
		return 0;

	//------- the Ordos inside enslaved towns -------//

	int ordoCount = 0;
	int i;

	for( i=1 ; i<=town_ordo_array_size ; i++ )
	{
		if( town_ordo(i) )
			ordoCount++;
	}

	if( !filePtr->file_put_short( -2 ) )		// -2: the records have the potential, the fractions of leadership and combat (-1 up to here) and the Taskmaster death date
		return 0;

	if( !filePtr->file_put_short( (int16_t) ordoCount ) )
		return 0;

	for( i=1 ; i<=town_ordo_array_size ; i++ )
	{
		TownOrdo* ordoPtr = town_ordo(i);

		if( !ordoPtr )
			continue;

		filePtr->file_put_short( (int16_t) i );
		filePtr->file_put_short( ordoPtr->unit_id );
		filePtr->file_put_unsigned_short( ordoPtr->name_id );
		filePtr->file_put_char( ordoPtr->nation_recno );
		filePtr->file_put_char( ordoPtr->action );
		filePtr->file_put_char( (int8_t) ordoPtr->combat_level );
		filePtr->file_put_char( (int8_t) ordoPtr->leadership );
		filePtr->file_put_short( ordoPtr->hit_points );

		filePtr->file_put_long( ordoPtr->next_kidnap_date );
		filePtr->file_put_char( (int8_t) ordoPtr->skill_potential );
		filePtr->file_put_char( (int8_t) ordoPtr->leadership_minor );

		filePtr->file_put_char( (int8_t) ordoPtr->combat_minor );

		if( !filePtr->file_put_long( ordoPtr->next_death_date ) )
			return 0;
	}

	//------- the Life Points of each kingdom -------//

	for( i=0 ; i<MAX_NATION ; i++ )
	{
		if( !filePtr->file_put_long( life_points_array[i] ) )
			return 0;
	}

	//------- their yearly change -------//

	for( i=0 ; i<MAX_NATION ; i++ )
	{
		if( !filePtr->file_put_long( cur_year_life_points_change[i] ) ||
			 !filePtr->file_put_long( last_year_life_points_change[i] ) )
		{
			return 0;
		}
	}

	//------- the last attacks of the AI against Fryhtan kingdoms -------//

	for( i=0 ; i<MAX_NATION ; i++ )
	{
		for( int j=0 ; j<MAX_NATION ; j++ )
		{
			if( !filePtr->file_put_long( ai_last_attack_date[i][j] ) )
				return 0;
		}
	}

	//------- the technologies of each kingdom -------//

	if( !filePtr->file_put_short( FRYHTAN_TECH_COUNT ) )
		return 0;

	for( i=0 ; i<MAX_NATION ; i++ )
	{
		for( int t=0 ; t<FRYHTAN_TECH_COUNT ; t++ )
		{
			if( !filePtr->file_put_short( tech_state_array[i][t] ) ||
				 !filePtr->file_put_short( tech_days_array[i][t] ) )
			{
				return 0;
			}
		}
	}

	return 1;
}
//----------- End of function ModFryhtan::write_file -----------//


//--------- Begin of function ModFryhtan::read_file ---------//
//
int ModFryhtan::read_file(File* filePtr)
{
	reset();

	if( !filePtr->file_get_short_array( nation_monster_id, MAX_NATION ) )
		return 0;

	//--- saved games from before the Life Points: the Fryhtan kingdoms get the starting ones (read below in newer games) ---//

	for( int n=0 ; n<MAX_NATION ; n++ )
		life_points_array[n] = nation_monster_id[n] ? FRYHTAN_START_LIFE_POINTS : 0;

	//------- the lairs -------//

	if( filePtr->file_pos() >= filePtr->file_size() )		// a saved game of phase 1, without lairs
		return 1;

	int lairCount = filePtr->file_get_short();
	int withOrdo  = 0;

	if( lairCount == -1 )		// the format with the Ordos in the queue (since the Train Ordo button)
	{
		withOrdo  = 1;
		lairCount = filePtr->file_get_short();
	}

	if( lairCount < 0 )
		return 0;

	if( lairCount > 0 )
	{
		set_lair( lairCount, 1 );		// allocate the array

		if( withOrdo )
		{
			if( !filePtr->file_get_short_array( (int16_t*) lair_array, lairCount * (sizeof(FryhtanLair)/sizeof(int16_t)) ) )
				return 0;
		}
		else		//--- older saved games: monster_id, train_count and train_days, all soldiers ---//
		{
			for( int i=0 ; i<lairCount ; i++ )
			{
				lair_array[i].monster_id  = filePtr->file_get_short();
				lair_array[i].train_count = filePtr->file_get_short();
				lair_array[i].train_days  = filePtr->file_get_short();
				lair_array[i].ordo_mask   = 0;
			}
		}
	}

	//------- the offers of the wild lairs (phase 5) -------//

	if( filePtr->file_pos() >= filePtr->file_size() )		// a saved game from before phase 5
		return 1;

	int offerCount = filePtr->file_get_short();

	if( offerCount < 0 )
		return 0;

	if( offerCount > 0 )
	{
		offer_array = (LairOffer*) mem_resize( offer_array, sizeof(LairOffer) * offerCount );
		offer_array_size = offerCount;

		if( !filePtr->file_get_short_array( (int16_t*) offer_array, offerCount * (sizeof(LairOffer)/sizeof(int16_t)) ) )
			return 0;
	}

	//------- the enslaved towns waiting to become independent -------//

	if( filePtr->file_pos() >= filePtr->file_size() )		// a saved game from before the grace period
		return 1;

	if( !read_town_dates( filePtr, lost_lair_date_array, lost_lair_date_array_size ) )
		return 0;

	//------- the day each town was last raided -------//

	if( filePtr->file_pos() >= filePtr->file_size() )		// a saved game from before the raids
		return 1;

	if( !read_town_dates( filePtr, raid_date_array, raid_date_array_size ) )
		return 0;

	//------- the kings killed of each kingdom -------//

	if( filePtr->file_pos() >= filePtr->file_size() )		// a saved game from before the gold by combat level
		return 1;

	if( !filePtr->file_read( king_kill_array, sizeof(king_kill_array) ) )
		return 0;

	//------- the Ordos inside enslaved towns -------//

	if( filePtr->file_pos() >= filePtr->file_size() )		// a saved game from before the Ordos in towns
		return 1;

	int ordoCount = filePtr->file_get_short();
	int hasMinor  = 0;
	int hasDeath  = 0;

	if( ordoCount == -1 || ordoCount == -2 )		// the records have the potential and the fractions (and the death date with -2)
	{
		hasMinor  = 1;
		hasDeath  = ordoCount == -2;
		ordoCount = filePtr->file_get_short();
	}

	for( int i=0 ; i<ordoCount ; i++ )
	{
		int townRecno = filePtr->file_get_short();

		if( townRecno < 1 || townRecno > 10000 )
			return 0;

		if( townRecno > town_ordo_array_size )
		{
			int newSize = townRecno + 20;

			town_ordo_array = (TownOrdo*) mem_resize( town_ordo_array, sizeof(TownOrdo) * newSize );
			memset( town_ordo_array + town_ordo_array_size, 0, sizeof(TownOrdo) * (newSize - town_ordo_array_size) );
			town_ordo_array_size = newSize;
		}

		TownOrdo* ordoPtr = town_ordo_array + townRecno - 1;

		ordoPtr->unit_id          = filePtr->file_get_short();
		ordoPtr->name_id          = filePtr->file_get_unsigned_short();
		ordoPtr->nation_recno     = filePtr->file_get_char();
		ordoPtr->action           = filePtr->file_get_char();
		ordoPtr->combat_level     = (uint8_t) filePtr->file_get_char();
		ordoPtr->leadership       = (uint8_t) filePtr->file_get_char();
		ordoPtr->hit_points       = filePtr->file_get_short();
		ordoPtr->next_kidnap_date = filePtr->file_get_long();

		if( hasMinor )
		{
			ordoPtr->skill_potential  = (uint8_t) filePtr->file_get_char();
			ordoPtr->leadership_minor = (uint8_t) filePtr->file_get_char();
			ordoPtr->combat_minor     = (uint8_t) filePtr->file_get_char();
		}

		if( hasDeath )
			ordoPtr->next_death_date = filePtr->file_get_long();
	}

	//------- the Life Points of each kingdom -------//

	if( filePtr->file_pos() >= filePtr->file_size() )		// a saved game from before the Life Points
		return 1;

	for( int n=0 ; n<MAX_NATION ; n++ )
		life_points_array[n] = filePtr->file_get_long();

	//------- their yearly change -------//

	if( filePtr->file_pos() >= filePtr->file_size() )		// a saved game from before the yearly change
		return 1;

	for( int n=0 ; n<MAX_NATION ; n++ )
	{
		cur_year_life_points_change[n]  = filePtr->file_get_long();
		last_year_life_points_change[n] = filePtr->file_get_long();
	}

	//------- the last attacks of the AI against Fryhtan kingdoms -------//

	if( filePtr->file_pos() >= filePtr->file_size() )		// a saved game from before
		return 1;

	for( int n=0 ; n<MAX_NATION ; n++ )
	{
		for( int m=0 ; m<MAX_NATION ; m++ )
			ai_last_attack_date[n][m] = filePtr->file_get_long();
	}

	//------- the technologies of each kingdom -------//

	if( filePtr->file_pos() >= filePtr->file_size() )		// a saved game from before
		return 1;

	int techCount = filePtr->file_get_short();		// saved, so that technologies can be added later

	for( int n=0 ; n<MAX_NATION ; n++ )
	{
		for( int t=0 ; t<techCount ; t++ )
		{
			int techState = filePtr->file_get_short();
			int techDays  = filePtr->file_get_short();

			if( t < FRYHTAN_TECH_COUNT )
			{
				tech_state_array[n][t] = (char) techState;
				tech_days_array[n][t]  = (int16_t) techDays;
			}
		}
	}

	return 1;
}
//----------- End of function ModFryhtan::read_file -----------//


//--------- Begin of function ModFryhtan::reward_kill ---------//
//
// 7kmod-fryhtan: a Fryhtan kingdom gets gold and food for each human unit
// it kills. The gold goes by the combat level (ModFryhtan::kill_gold()): a
// villager $2; a soldier $2 with combat 10 to $45 with combat 100; a town
// defender half that; a general x3; a king x22 (about $1000 with combat 100),
// and half for the next kings of the same kingdom. The food is fixed.
//
// <int>   nationRecno - the nation of the attacker
// <Unit*> targetUnit  - the unit killed
//
void ModFryhtan::reward_kill(int nationRecno, Unit* targetUnit)
{
	if( !nationRecno || !is_fryhtan(nationRecno) || nation_array.is_deleted(nationRecno) )
		return;

	if( !targetUnit->race_id || targetUnit->nation_recno == nationRecno )		// only human units of other nations
		return;

	int combatLevel = targetUnit->skill.combat_level;
	int gold, food = KILL_FOOD_UNIT;

	if( targetUnit->rank_id == RANK_KING )
	{
		int percent = 100 * KILL_GOLD_KING_MULTIPLY;
		int targetNationRecno = targetUnit->nation_recno;

		if( targetNationRecno >= 1 && targetNationRecno <= MAX_NATION )
		{
			char& killCount = king_kill_array[nationRecno-1][targetNationRecno-1];

			if( killCount > 0 )		// the next kings of the same kingdom give less
				percent = percent * KILL_GOLD_NEXT_KING_PERCENT / 100;

			if( killCount < 100 )
				killCount++;
		}

		gold = kill_gold(combatLevel, percent);
		food = KILL_FOOD_KING;
	}
	else if( targetUnit->rank_id == RANK_GENERAL )
	{
		gold = kill_gold(combatLevel, 100 * KILL_GOLD_GENERAL_MULTIPLY);
		food = KILL_FOOD_GENERAL;
	}
	else if( targetUnit->unit_mode == UNIT_MODE_DEFEND_TOWN )		// a town defender
	{
		gold = kill_gold(combatLevel, KILL_GOLD_DEFENDER_PERCENT);
	}
	else if( targetUnit->is_civilian() && combatLevel < 20 )		// a villager (as in Unit::hit_target())
	{
		gold = KILL_GOLD_UNIT;
	}
	else
	{
		gold = kill_gold(combatLevel, 100);
	}

	Nation* nationPtr = nation_array[nationRecno];

	nationPtr->add_income( INCOME_TREASURE, (float) gold );
	nationPtr->add_food( (float) food );
}
//----------- End of function ModFryhtan::reward_kill -----------//


//--------- Begin of function ModFryhtan::kill_gold ---------//
//
// The gold for killing a soldier of the combat level: KILL_GOLD_UNIT with
// KILL_GOLD_MIN_COMBAT, KILL_GOLD_MAX with 100, in a straight line; x percent
// / 100, rounded, at least 1.
//
int ModFryhtan::kill_gold(int combatLevel, int percent)
{
	combatLevel = MAX( (int) KILL_GOLD_MIN_COMBAT, MIN(100, combatLevel) );

	float gold = (float) KILL_GOLD_UNIT + (float) (combatLevel - KILL_GOLD_MIN_COMBAT) *
					 (KILL_GOLD_MAX - KILL_GOLD_UNIT) / (100 - KILL_GOLD_MIN_COMBAT);

	return MAX( 1, (int) (gold * percent / 100 + 0.5f) );
}
//----------- End of function ModFryhtan::kill_gold -----------//


//--------- Begin of function ModFryhtan::ai_wait_relax_level ---------//
//
// How long an AI kingdom has been at war with a Fryhtan kingdom without
// sending troops against it, since the war started or since its last
// attack: 0 - less than 6 months, 1 - 6 months to 1 year, 2 - 1 year or
// more. The AI then asks less to attack (see AI_RELAX_DAYS_1).
//
int ModFryhtan::ai_wait_relax_level(int aiNationRecno, int fryhtanNationRecno)
{
	if( aiNationRecno < 1 || aiNationRecno > MAX_NATION ||
		 fryhtanNationRecno < 1 || fryhtanNationRecno > MAX_NATION ||
		 aiNationRecno == fryhtanNationRecno ||
		 nation_array.is_deleted(aiNationRecno) || nation_array.is_deleted(fryhtanNationRecno) ||
		 !is_fryhtan(fryhtanNationRecno) )
	{
		return 0;
	}

	NationRelation* nationRelation = nation_array[aiNationRecno]->get_relation(fryhtanNationRecno);

	if( nationRelation->status != NATION_HOSTILE )
		return 0;

	int sinceDate = MAX( nationRelation->last_change_status_date, (int) ai_last_attack_date[aiNationRecno-1][fryhtanNationRecno-1] );
	int waitDays  = info.game_date - sinceDate;

	if( waitDays >= AI_RELAX_DAYS_2 )
		return 2;

	if( waitDays >= AI_RELAX_DAYS_1 )
		return 1;

	return 0;
}
//----------- End of function ModFryhtan::ai_wait_relax_level -----------//


//--------- Begin of function ModFryhtan::ai_relax_level ---------//
//
int ModFryhtan::ai_relax_level(int aiNationRecno)
{
	int relaxLevel = 0;

	for( int i=nation_array.size() ; i>0 ; i-- )
	{
		if( !nation_array.is_deleted(i) && is_fryhtan(i) )
			relaxLevel = MAX( relaxLevel, ai_wait_relax_level(aiNationRecno, i) );
	}

	return relaxLevel;
}
//----------- End of function ModFryhtan::ai_relax_level -----------//


//--------- Begin of function ModFryhtan::ai_attack_sent ---------//
//
// An AI kingdom has just sent troops against a target of a nation.
//
void ModFryhtan::ai_attack_sent(int aiNationRecno, int targetNationRecno)
{
	if( aiNationRecno < 1 || aiNationRecno > MAX_NATION ||
		 targetNationRecno < 1 || targetNationRecno > MAX_NATION ||
		 !is_fryhtan(targetNationRecno) )
	{
		return;
	}

	ai_last_attack_date[aiNationRecno-1][targetNationRecno-1] = info.game_date;
}
//----------- End of function ModFryhtan::ai_attack_sent -----------//


//--------- Begin of function ModFryhtan::lair_train_combat ---------//
//
// The combat of a Fryhtan trained in a lair: LAIR_TRAIN_COMBAT for a
// soldier, LAIR_ORDO_COMBAT for an Ordo; EMPOWERMENT_COMBAT more with the
// technology Empowerment.
//
int ModFryhtan::lair_train_combat(int nationRecno, int trainType)
{
	int combatLevel = trainType == LAIR_TRAIN_ORDO ? LAIR_ORDO_COMBAT : LAIR_TRAIN_COMBAT;

	if( has_tech(nationRecno, FRYHTAN_TECH_EMPOWERMENT) )
		combatLevel += EMPOWERMENT_COMBAT;

	return combatLevel;
}
//----------- End of function ModFryhtan::lair_train_combat -----------//


//--------- Begin of function ModFryhtan::tech_name ---------//
//
const char* ModFryhtan::tech_name(int techId)
{
	switch( techId )
	{
		case FRYHTAN_TECH_EMPOWERMENT:			return _("Empowerment");
		case FRYHTAN_TECH_BROOD_MASTERY:		return _("Brood Mastery");
		case FRYHTAN_TECH_DOMINION:			return _("Dominion");
		case FRYHTAN_TECH_SACRIFICIAL_RITES:	return _("Sacrificial Rites");
	}

	return _("Alchemist Lair");
}
//----------- End of function ModFryhtan::tech_name -----------//


//--------- Begin of function ModFryhtan::tech_desc ---------//
//
const char* ModFryhtan::tech_desc(int techId)
{
	static char descStr[200];

	if( techId == FRYHTAN_TECH_EMPOWERMENT )
	{
		// TRANSLATORS: Fryhtans trained in your lairs get +<Number> combat.
		snprintf( descStr, sizeof(descStr), _("Fryhtans trained in your lairs get +%d combat."), (int) EMPOWERMENT_COMBAT );
		return descStr;
	}

	if( techId == FRYHTAN_TECH_BROOD_MASTERY )
	{
		// TRANSLATORS: Training Fryhtans in your lairs costs <Number>% less gold and Life Points.
		snprintf( descStr, sizeof(descStr), _("Training Fryhtans in your lairs costs %d%% less gold and Life Points."), 100 - (int) BROOD_MASTERY_PERCENT );
		return descStr;
	}

	if( techId == FRYHTAN_TECH_DOMINION )
	{
		// TRANSLATORS: Your enslaved towns get +<Number> loyalty, and overpopulation starts at <Number> people.
		snprintf( descStr, sizeof(descStr), _("Your enslaved towns get +%d loyalty, and overpopulation starts at %d people."),
			(int) DOMINION_LOYALTY, (int) OVERPOPULATED_TOWN_POPULATION + DOMINION_OVERPOPULATION );
		return descStr;
	}

	if( techId == FRYHTAN_TECH_SACRIFICIAL_RITES )
		return _("When a town reaches overpopulation, your Ordo will sacrifice humans. Sacrifices give additional Life Points.");

	return _("Lairs can be built on raw material sites to turn the raw material into Life Points. Requires an Ordo.");
}
//----------- End of function ModFryhtan::tech_desc -----------//


//--------- Begin of function ModFryhtan::tech_cost ---------//
//
int ModFryhtan::tech_cost(int techId)
{
	switch( techId )
	{
		case FRYHTAN_TECH_EMPOWERMENT:			return EMPOWERMENT_TECH_COST;
		case FRYHTAN_TECH_BROOD_MASTERY:		return BROOD_MASTERY_TECH_COST;
		case FRYHTAN_TECH_DOMINION:			return DOMINION_TECH_COST;
		case FRYHTAN_TECH_SACRIFICIAL_RITES:	return RITES_TECH_COST;
	}

	return ALCHEMIST_TECH_COST;
}


//--------- Begin of function ModFryhtan::tech_life_points ---------//
//
int ModFryhtan::tech_life_points(int techId)
{
	switch( techId )
	{
		case FRYHTAN_TECH_EMPOWERMENT:			return EMPOWERMENT_TECH_LIFE_POINTS;
		case FRYHTAN_TECH_BROOD_MASTERY:		return BROOD_MASTERY_TECH_LIFE_POINTS;
		case FRYHTAN_TECH_DOMINION:			return DOMINION_TECH_LIFE_POINTS;
		case FRYHTAN_TECH_SACRIFICIAL_RITES:	return RITES_TECH_LIFE_POINTS;
	}

	return ALCHEMIST_TECH_LIFE_POINTS;
}


//--------- Begin of function ModFryhtan::tech_total_days ---------//
//
int ModFryhtan::tech_total_days(int techId)
{
	switch( techId )
	{
		case FRYHTAN_TECH_EMPOWERMENT:			return EMPOWERMENT_TECH_DAYS;
		case FRYHTAN_TECH_BROOD_MASTERY:		return BROOD_MASTERY_TECH_DAYS;
		case FRYHTAN_TECH_DOMINION:			return DOMINION_TECH_DAYS;
		case FRYHTAN_TECH_SACRIFICIAL_RITES:	return RITES_TECH_DAYS;
	}

	return ALCHEMIST_TECH_DAYS;
}


//--------- Begin of function ModFryhtan::tech_required ---------//
//
// The chains: Brood Mastery -> Empowerment, Dominion -> Sacrificial Rites.
//
int ModFryhtan::tech_required(int techId)
{
	switch( techId )
	{
		case FRYHTAN_TECH_EMPOWERMENT:			return FRYHTAN_TECH_BROOD_MASTERY;
		case FRYHTAN_TECH_SACRIFICIAL_RITES:	return FRYHTAN_TECH_DOMINION;
	}

	return -1;
}


//--------- Begin of function ModFryhtan::tech_list_order ---------//
//
// The order of the list of the lairs, by branch: Alchemist Lair; Brood
// Mastery, Empowerment; Dominion, Sacrificial Rites.
//
int ModFryhtan::tech_list_order(int i)
{
	static const char listOrder[FRYHTAN_TECH_COUNT] =
	{
		FRYHTAN_TECH_ALCHEMIST_LAIR,
		FRYHTAN_TECH_BROOD_MASTERY,
		FRYHTAN_TECH_EMPOWERMENT,
		FRYHTAN_TECH_DOMINION,
		FRYHTAN_TECH_SACRIFICIAL_RITES,
	};

	return listOrder[i];
}


//--------- Begin of function ModFryhtan::tech_available ---------//
//
// Whether the technology is in the list: its required technology is
// researched. One already unlocked or researched before the chains (a saved
// game) stays available.
//
int ModFryhtan::tech_available(int nationRecno, int techId)
{
	int requiredId = tech_required(techId);

	return requiredId < 0 || has_tech(nationRecno, requiredId) ||
			 tech_state(nationRecno, techId) != FRYHTAN_TECH_NONE;
}
//----------- End of function ModFryhtan::tech_available -----------//


//--------- Begin of function ModFryhtan::tech_state ---------//
//
int ModFryhtan::tech_state(int nationRecno, int techId)
{
	if( nationRecno < 1 || nationRecno > MAX_NATION || techId < 0 || techId >= FRYHTAN_TECH_COUNT )
		return FRYHTAN_TECH_NONE;

	return tech_state_array[nationRecno-1][techId];
}
//----------- End of function ModFryhtan::tech_state -----------//


//--------- Begin of function ModFryhtan::tech_days ---------//
//
int ModFryhtan::tech_days(int nationRecno, int techId)
{
	if( nationRecno < 1 || nationRecno > MAX_NATION || techId < 0 || techId >= FRYHTAN_TECH_COUNT )
		return 0;

	return tech_days_array[nationRecno-1][techId];
}
//----------- End of function ModFryhtan::tech_days -----------//


//--------- Begin of function ModFryhtan::can_start_research ---------//
//
int ModFryhtan::can_start_research(int nationRecno, int techId)
{
	if( !is_fryhtan(nationRecno) )
		return 0;

	int techState = tech_state(nationRecno, techId);

	if( techState == FRYHTAN_TECH_UNLOCKED )		// already paid
		return 1;

	if( techState != FRYHTAN_TECH_NONE || !tech_available(nationRecno, techId) )
		return 0;

	Nation* nationPtr = nation_array[nationRecno];

	return nationPtr->cash >= tech_cost(techId) &&
			 life_points(nationRecno) >= tech_life_points(techId);
}
//----------- End of function ModFryhtan::can_start_research -----------//


//--------- Begin of function ModFryhtan::start_research ---------//
//
// Research a technology, as in the Tower of Science: the first time it is
// unlocked, paying its gold and Life Points; after that it is free. The
// kingdom researches one at a time: the one being researched is paused
// (unlocked), keeping its progress.
//
int ModFryhtan::start_research(int nationRecno, int techId)
{
	if( !can_start_research(nationRecno, techId) )
		return 0;

	if( tech_state(nationRecno, techId) == FRYHTAN_TECH_NONE )		// unlock it
	{
		nation_array[nationRecno]->add_expense( EXPENSE_FIRM, (float) tech_cost(techId) );		// the research of the lairs
		add_life_points( nationRecno, -tech_life_points(techId) );

		tech_days_array[nationRecno-1][techId] = 0;
	}

	//--- pause the one being researched ---//

	for( int t=0 ; t<FRYHTAN_TECH_COUNT ; t++ )
	{
		if( tech_state_array[nationRecno-1][t] == FRYHTAN_TECH_RESEARCHING )
			tech_state_array[nationRecno-1][t] = FRYHTAN_TECH_UNLOCKED;
	}

	tech_state_array[nationRecno-1][techId] = FRYHTAN_TECH_RESEARCHING;

	return 1;
}
//----------- End of function ModFryhtan::start_research -----------//


//--------- Begin of function ModFryhtan::nation_lair_count ---------//
//
int ModFryhtan::nation_lair_count(int nationRecno)
{
	int lairCount = 0;

	for( int i=firm_array.size() ; i>0 ; i-- )
	{
		if( firm_array.is_deleted(i) )
			continue;

		Firm* firmPtr = firm_array[i];

		if( firmPtr->nation_recno == nationRecno && !firmPtr->under_construction && is_lair(firmPtr) )
			lairCount++;
	}

	return lairCount;
}
//----------- End of function ModFryhtan::nation_lair_count -----------//


//--------- Begin of function ModFryhtan::next_day_research ---------//
//
// The research of a Fryhtan kingdom goes on while it has a finished lair,
// even if the lair where it started has been destroyed.
//
void ModFryhtan::next_day_research(int nationRecno)
{
	if( nationRecno < 1 || nationRecno > MAX_NATION || !is_fryhtan(nationRecno) )
		return;



	int lairCountChecked = 0, hasLair = 0;

	for( int t=0 ; t<FRYHTAN_TECH_COUNT ; t++ )
	{
		if( tech_state_array[nationRecno-1][t] != FRYHTAN_TECH_RESEARCHING )
			continue;

		if( !lairCountChecked )
		{
			hasLair = nation_lair_count(nationRecno) > 0;
			lairCountChecked = 1;
		}

		if( !hasLair )
			return;

		if( ++tech_days_array[nationRecno-1][t] >= tech_total_days(t) )
		{
			tech_state_array[nationRecno-1][t] = FRYHTAN_TECH_DONE;

			if( nationRecno == nation_array.player_recno )
			{
				news_array.lair_tech_researched(t);

				//--- the "research complete" sound of the Tower of Science, at a lair of the kingdom ---//

				for( int i=firm_array.size() ; i>0 ; i-- )
				{
					if( firm_array.is_deleted(i) )
						continue;

					Firm* firmPtr = firm_array[i];

					if( firmPtr->nation_recno == nationRecno && !firmPtr->under_construction && is_lair(firmPtr) )
					{
						se_res.far_sound( firmPtr->center_x, firmPtr->center_y, 1, 'F', FIRM_RESEARCH, "FINS",
							'S', unit_res[UNIT_CATAPULT]->sprite_id );
						break;
					}
				}
			}
		}
	}
}
//----------- End of function ModFryhtan::next_day_research -----------//


//--------- Begin of function ModFryhtan::alchemist_site_recno ---------//
//
// The raw material site under a lair, 0 if there is none.
//
int ModFryhtan::alchemist_site_recno(Firm* firmPtr)
{
	for( int yLoc=firmPtr->loc_y1 ; yLoc<=firmPtr->loc_y2 ; yLoc++ )
	{
		for( int xLoc=firmPtr->loc_x1 ; xLoc<=firmPtr->loc_x2 ; xLoc++ )
		{
			Location* locPtr = world.get_loc(xLoc, yLoc);

			if( locPtr->has_site() && site_array[locPtr->site_recno()]->site_type == SITE_RAW )
				return locPtr->site_recno();
		}
	}

	return 0;
}
//----------- End of function ModFryhtan::alchemist_site_recno -----------//


//--------- Begin of function ModFryhtan::can_build_lair_on_site ---------//
//
// With Alchemist Lair, a Fryhtan kingdom can build a lair on a raw material
// site that has no mine (World::can_build_firm).
//
int ModFryhtan::can_build_lair_on_site(Location* locPtr, int builderUnitRecno)
{
	if( !builderUnitRecno || unit_array.is_deleted(builderUnitRecno) )
		return 0;

	int nationRecno = unit_array[builderUnitRecno]->nation_recno;

	if( !nationRecno || !has_tech(nationRecno, FRYHTAN_TECH_ALCHEMIST_LAIR) )
		return 0;

	Site* sitePtr = site_array[locPtr->site_recno()];

	return sitePtr->site_type == SITE_RAW && !sitePtr->has_mine;
}
//----------- End of function ModFryhtan::can_build_lair_on_site -----------//


//--------- Begin of function ModFryhtan::is_alchemist_lair ---------//
//
int ModFryhtan::is_alchemist_lair(Firm* firmPtr)
{
	return is_lair(firmPtr) && has_tech(firmPtr->nation_recno, FRYHTAN_TECH_ALCHEMIST_LAIR) &&
			 alchemist_site_recno(firmPtr) != 0;
}
//----------- End of function ModFryhtan::is_alchemist_lair -----------//


//--------- Begin of function ModFryhtan::next_day_alchemist ---------//
//
// An Alchemist Lair with an Ordo or the king as its leader turns
// ALCHEMIST_RAW_QTY of the raw material of its site into
// ALCHEMIST_LIFE_POINTS every ALCHEMIST_DAYS days (less of the last ones, in
// proportion). When the site is exhausted, it is a normal lair.
//
void ModFryhtan::next_day_alchemist(Firm* firmPtr)
{
	if( firmPtr->under_construction || !firmPtr->nation_recno ||
		 info.game_date % ALCHEMIST_DAYS != firmPtr->firm_recno % ALCHEMIST_DAYS )
	{
		return;
	}

	if( !has_tech(firmPtr->nation_recno, FRYHTAN_TECH_ALCHEMIST_LAIR) )
		return;

	int siteRecno = alchemist_site_recno(firmPtr);

	if( !siteRecno )
		return;

	Site* sitePtr = site_array[siteRecno];

	if( !sitePtr->has_mine )		// the site is taken, no mine can be built on it
	{
		sitePtr->has_mine = 1;
		site_array.untapped_raw_count--;
	}

	//--- it needs a Fryhtan leader: an Ordo or the king ---//

	if( !firmPtr->overseer_recno || unit_array[firmPtr->overseer_recno]->race_id )
		return;

	int rawQty = MIN( (int) ALCHEMIST_RAW_QTY, sitePtr->reserve_qty );

	if( rawQty <= 0 )
		return;

	sitePtr->reserve_qty -= rawQty;

	add_life_points( firmPtr->nation_recno, MAX( 1, ALCHEMIST_LIFE_POINTS * rawQty / ALCHEMIST_RAW_QTY ) );

	//--- the site is exhausted, as with a mine ---//

	if( sitePtr->reserve_qty <= 0 )
	{
		site_array.untapped_raw_count++;		// del_site() decreases it again
		site_array.del_site(siteRecno);
	}
}
//----------- End of function ModFryhtan::next_day_alchemist -----------//


//--------- Begin of function ModFryhtan::release_alchemist_site ---------//
//
// The lair is deleted: its site, if any, can have a mine again.
//
void ModFryhtan::release_alchemist_site(Firm* firmPtr)
{
	int siteRecno = alchemist_site_recno(firmPtr);

	if( !siteRecno )
		return;

	Site* sitePtr = site_array[siteRecno];

	if( sitePtr->has_mine )
	{
		sitePtr->has_mine = 0;
		site_array.untapped_raw_count++;
	}
}
//----------- End of function ModFryhtan::release_alchemist_site -----------//


//--------- Begin of function ModFryhtan::largest_land_region_id ---------//
//
// The region id of the largest land of the map, 0 if there is none.
//
int ModFryhtan::largest_land_region_id()
{
	for( int i=1 ; i<=region_array.region_info_count ; i++ )		// sorted by size, the largest first
	{
		RegionInfo* regionInfo = region_array.get_sorted_region(i);

		if( regionInfo->region_type == REGION_LAND )
			return regionInfo->region_id;
	}

	return 0;
}
//----------- End of function ModFryhtan::largest_land_region_id -----------//


//--------- Begin of function ModFryhtan::town_tribute_life_points ---------//
//
// The monthly tribute of life of an enslaved town: 1 for every 10 people, and
// 1 more with 15 people or more (10-14: 1, 15-19: 2, 20-29: 3, 30-39: 4).
//
int ModFryhtan::town_tribute_life_points(int population)
{
	int lifePoints = population / LIFE_POINTS_TRIBUTE_PEOPLE;

	if( population >= LIFE_POINTS_TRIBUTE_EXTRA_PEOPLE )
		lifePoints++;

	return lifePoints;
}
//----------- End of function ModFryhtan::town_tribute_life_points -----------//


//--------- Begin of function ModFryhtan::firm_setup_life_points ---------//
//
int ModFryhtan::firm_setup_life_points(int firmId, int nationRecno)
{
	if( firmId == FIRM_CAMP && nationRecno && mod_fryhtan.is_fryhtan(nationRecno) )
		return LIFE_POINTS_LAIR;

	return 0;
}
//----------- End of function ModFryhtan::firm_setup_life_points -----------//


//--------- Begin of function ModFryhtan::start_life_points ---------//
//
// The Life Points a Fryhtan kingdom starts with, by the Treasure of the new
// game: Small 150, Medium 300, Large 500, Very Large 700 (the AI with its own
// Treasure).
//
int ModFryhtan::start_life_points(int nationRecno)
{
	int startUpCash = config.start_up_cash;

	if( nationRecno != nation_array.player_recno && !nation_array.is_deleted(nationRecno) &&
		 nation_array[nationRecno]->nation_type == NATION_AI )
	{
		startUpCash = config.ai_start_up_cash;
	}

	switch( startUpCash )
	{
		case OPTION_LOW:			return 150;
		case OPTION_HIGH:			return 500;
		case OPTION_VERY_HIGH:	return 700;
		default:						return FRYHTAN_START_LIFE_POINTS;		// Medium: 300
	}
}
//----------- End of function ModFryhtan::start_life_points -----------//


//--------- Begin of function ModFryhtan::life_points ---------//
//
int ModFryhtan::life_points(int nationRecno)
{
	if( nationRecno < 1 || nationRecno > MAX_NATION )
		return 0;

	return life_points_array[nationRecno-1];
}
//----------- End of function ModFryhtan::life_points -----------//


//--------- Begin of function ModFryhtan::add_life_points ---------//
//
// Only Fryhtan kingdoms have Life Points. They never go below 0.
//
void ModFryhtan::add_life_points(int nationRecno, int points)
{
	if( nationRecno < 1 || nationRecno > MAX_NATION || !is_fryhtan(nationRecno) )
		return;

	int32_t& lifePoints = life_points_array[nationRecno-1];
	int32_t  oldPoints  = lifePoints;

	lifePoints = MAX( 0, MIN( 999999, lifePoints + points ) );

	cur_year_life_points_change[nationRecno-1] += lifePoints - oldPoints;
}
//----------- End of function ModFryhtan::add_life_points -----------//


//--------- Begin of function ModFryhtan::life_points_change_365days ---------//
//
// The net change of the Life Points in the last 365 days: this year's and
// the matching part of last year's, as NationBase::food_change_365days().
//
int ModFryhtan::life_points_change_365days(int nationRecno)
{
	if( nationRecno < 1 || nationRecno > MAX_NATION )
		return 0;

	return (int) ( (float) last_year_life_points_change[nationRecno-1] * (365-info.year_day) / 365 +
		cur_year_life_points_change[nationRecno-1] );
}
//----------- End of function ModFryhtan::life_points_change_365days -----------//


//--------- Begin of function ModFryhtan::next_year_life_points ---------//
//
void ModFryhtan::next_year_life_points(int nationRecno)
{
	if( nationRecno < 1 || nationRecno > MAX_NATION )
		return;

	last_year_life_points_change[nationRecno-1] = cur_year_life_points_change[nationRecno-1];
	cur_year_life_points_change[nationRecno-1]  = 0;
}
//----------- End of function ModFryhtan::next_year_life_points -----------//


//--------- Begin of function ModFryhtan::kill_life_points ---------//
//
// The Life Points for killing a unit: its combat level / 10 (1 with combat
// 10, 10 with combat 100); a villager or a town defender half that, a
// general x1.25, a king x1.5; rounded, at least 1. Weapons and ships, which
// have no combat level, give a fixed amount. Fryhtans count as soldiers.
//
int ModFryhtan::kill_life_points(Unit* targetUnit)
{
	char unitClass = unit_res[targetUnit->unit_id]->unit_class;

	if( unitClass == UNIT_CLASS_WEAPON )
		return LIFE_POINTS_WEAPON;

	if( unitClass == UNIT_CLASS_SHIP )
		return LIFE_POINTS_SHIP;

	int percent = 100;

	if( targetUnit->rank_id == RANK_KING )
		percent = LIFE_POINTS_KING_PERCENT;
	else if( targetUnit->rank_id == RANK_GENERAL )
		percent = LIFE_POINTS_GENERAL_PERCENT;
	else if( targetUnit->race_id && (targetUnit->is_civilian() || targetUnit->unit_mode == UNIT_MODE_DEFEND_TOWN) )
		percent = LIFE_POINTS_CIVILIAN_PERCENT;

	int combatLevel = MAX( 0, MIN(100, (int) targetUnit->skill.combat_level) );

	// rounded: (combat x percent / 100) / 10, the .5 up
	int points = (combatLevel * percent + LIFE_POINTS_KILL_COMBAT*100/2) / (LIFE_POINTS_KILL_COMBAT*100);

	return MAX( 1, points );
}
//----------- End of function ModFryhtan::kill_life_points -----------//


//--------- Begin of function ModFryhtan::reward_life_points ---------//
//
// A Fryhtan kingdom gets Life Points for each unit of another nation (or
// independent) that its units kill: humans, Fryhtans, weapons and ships.
//
// <int>   nationRecno - the nation of the attacker
// <Unit*> targetUnit  - the unit killed
//
void ModFryhtan::reward_life_points(int nationRecno, Unit* targetUnit)
{
	if( !nationRecno || !is_fryhtan(nationRecno) || nation_array.is_deleted(nationRecno) )
		return;

	if( targetUnit->nation_recno == nationRecno )
		return;

	add_life_points( nationRecno, kill_life_points(targetUnit) );
}
//----------- End of function ModFryhtan::reward_life_points -----------//


//--------- Begin of function ModFryhtan::reset_king_kills ---------//
//
// A new kingdom in the nation recno: no kings of it have been killed.
//
void ModFryhtan::reset_king_kills(int nationRecno)
{
	if( nationRecno < 1 || nationRecno > MAX_NATION )
		return;

	for( int i=0 ; i<MAX_NATION ; i++ )
	{
		king_kill_array[i][nationRecno-1] = 0;
		king_kill_array[nationRecno-1][i] = 0;
	}
}
//----------- End of function ModFryhtan::reset_king_kills -----------//


//--------- Begin of function ModFryhtan::should_destroy_firm ---------//
//
// 7kmod-fryhtan: a Fryhtan kingdom can't use human buildings, the ones it
// captures (for example when a kingdom surrenders to it) are destroyed.
// Its lairs are the only buildings it keeps.
//
int ModFryhtan::should_destroy_firm(Firm* firmPtr)
{
	if( !firmPtr->nation_recno || !is_fryhtan(firmPtr->nation_recno) )
		return 0;

	return !is_lair(firmPtr);
}
//----------- End of function ModFryhtan::should_destroy_firm -----------//


//--------- Begin of function ModFryhtan::destroy_firm ---------//
//
// Destroy a human building captured by a Fryhtan kingdom. The humans inside
// (general, workers and builder) die with it, as the kingdom has no use for
// them. The caller deletes the firm afterwards.
//
void ModFryhtan::destroy_firm(Firm* firmPtr)
{
	if( firmPtr->builder_recno )
	{
		firmPtr->kill_builder(firmPtr->builder_recno);
		firmPtr->builder_recno = 0;
	}

	if( firmPtr->overseer_recno )
		firmPtr->kill_overseer();

	if( firmPtr->worker_array && firmPtr->worker_count > 0 )
		firmPtr->kill_all_worker();

	se_res.sound( firmPtr->center_x, firmPtr->center_y, 1, 'F', firmPtr->firm_id, "DEST" );
}
//----------- End of function ModFryhtan::destroy_firm -----------//


//--------- Begin of function ModFryhtan::king_title ---------//
//
// 7kmod-fryhtan: the title of the king of a Fryhtan kingdom, "<king name> The
// All High" (for example "Al The All High"). The unit name of the king is
// still "All High <species>".
//
const char* ModFryhtan::king_title(int nationRecno)
{
	static char str[NationBase::NATION_NAME_LEN+40];

	// TRANSLATORS: <King name> The All High, the king of a Fryhtan kingdom
	snprintf( str, sizeof(str), _("%s The All High"), nation_array[nationRecno]->king_name() );

	return str;
}
//----------- End of function ModFryhtan::king_title -----------//


//--------- Begin of function ModFryhtan::is_enslaved_town ---------//
//
// 7kmod-fryhtan: the towns of a Fryhtan kingdom are enslaved towns.
//
int ModFryhtan::is_enslaved_town(Town* townPtr)
{
	return townPtr->nation_recno && is_fryhtan(townPtr->nation_recno);
}
//----------- End of function ModFryhtan::is_enslaved_town -----------//


//--------- Begin of function ModFryhtan::lair_leadership ---------//
//
// The best leadership of the generals (or the king) in the lairs of the
// given Fryhtan kingdom that have an active link with the town.
//
// return: <int> the leadership, 0 if there is no such lair
//
int ModFryhtan::lair_leadership(Town* townPtr, int nationRecno)
{
	int bestLeadership = 0;

	for( int i=0 ; i<townPtr->linked_firm_count ; i++ )
	{
		if( townPtr->linked_firm_enable_array[i] != LINK_EE )
			continue;

		Firm* firmPtr = firm_array[ townPtr->linked_firm_array[i] ];

		if( firmPtr->nation_recno != nationRecno || !firmPtr->overseer_recno || !is_lair(firmPtr) )
			continue;

		int leadership = unit_array[firmPtr->overseer_recno]->skill.get_skill(SKILL_LEADING);

		if( leadership > bestLeadership )
			bestLeadership = leadership;
	}

	return bestLeadership;
}
//----------- End of function ModFryhtan::lair_leadership -----------//


//--------- Begin of function ModFryhtan::resistance_decrease ---------//
//
// How much the resistance of an independent town drops toward a Fryhtan
// kingdom in one update (every 5 days). The lairs lower it toward
// ENSLAVE_MIN_RESISTANCE, at the speed of a Fort (the gap / 30, at least 1)
// times the leadership / 100 of the best general.
//
float ModFryhtan::resistance_decrease(Town* townPtr, int nationRecno, float resistance, float targetResistance)
{
	float decValue = (resistance - targetResistance) / 30;

	decValue = MAX(1, decValue);

	int leadership = MAX( 10, lair_leadership(townPtr, nationRecno) );

	return decValue * leadership / 100;
}
//----------- End of function ModFryhtan::resistance_decrease -----------//


//--------- Begin of function ModFryhtan::can_enslave_town ---------//
//
// A Fryhtan kingdom can only enslave a town that is linked to one of its
// lairs.
//
int ModFryhtan::can_enslave_town(Town* townPtr, int nationRecno)
{
	return townPtr->has_linked_camp(nationRecno, 0);
}
//----------- End of function ModFryhtan::can_enslave_town -----------//


//--------- Begin of function ModFryhtan::next_day_town ---------//
//
// Daily processing of an enslaved town.
//
// return: <int> 1 if the town is still enslaved, 0 if it has become independent
//
int ModFryhtan::next_day_town(Town* townPtr)
{
	int nationRecno = townPtr->nation_recno;

	//---- if it has lost the link with all the lairs, it becomes independent after a grace period ----//

	if( !townPtr->has_linked_camp(nationRecno, 0) )
	{
		int32_t lostDate = lost_lair_date(townPtr->town_recno);

		if( !lostDate )
		{
			set_lost_lair_date(townPtr->town_recno, info.game_date);
			return 1;
		}

		if( info.game_date < lostDate + LOST_LAIR_GRACE_DAYS )
			return 1;		// no tribute: no Ordo or king commands it

		set_lost_lair_date(townPtr->town_recno, 0);

		make_town_independent(townPtr);
		return 0;
	}

	set_lost_lair_date(townPtr->town_recno, 0);		// linked again: the grace period is over

	//------- the tribute, at the start of each month (only with an Ordo or the king in a linked lair) -------//

	if( info.game_day == 1 && has_lair_commander(townPtr) )
	{
		nation_array[nationRecno]->add_income( INCOME_TAX, (float) town_tribute(townPtr) );
		add_life_points( nationRecno, town_tribute_life_points(townPtr->population) );
	}

	return 1;
}
//----------- End of function ModFryhtan::next_day_town -----------//


//--------- Begin of function ModFryhtan::town_tribute ---------//
//
int ModFryhtan::town_tribute(Town* townPtr)
{
	//--- the leadership of the best general (or king) of a linked lair raises it: x2 with leadership 100 ---//

	int leadership = lair_leadership(townPtr, townPtr->nation_recno);

	return townPtr->population * TRIBUTE_PER_PERSON * (100 + leadership) / 100;
}
//----------- End of function ModFryhtan::town_tribute -----------//


//--------- Begin of function ModFryhtan::set_header_species ---------//
//
// 7kmod-fryhtan: the species of a Fryhtan kingdom in the header of a saved
// game, for the list of saved games (which doesn't load the whole game). The
// header has a fixed size, so it goes in the last two bytes of the king's
// name, after its end: a mark and the monster id. The game ignores them.
// It isn't stored if the name is too long for it.
//
// <char*> playerName - GameFile::player_name, with its terminating zero
// <int>   nameSize   - the size of the array
// <int>   monsterId  - the species
//
void ModFryhtan::set_header_species(char* playerName, int nameSize, int monsterId)
{
	if( !monsterId || (int) strlen(playerName) > nameSize-3 )
		return;

	playerName[nameSize-2] = FRYHTAN_HEADER_MARK;
	playerName[nameSize-1] = (char) monsterId;
}
//----------- End of function ModFryhtan::set_header_species -----------//


//--------- Begin of function ModFryhtan::header_species ---------//
//
// return: <int> the species of the Fryhtan kingdom of a saved game, 0 if it
//               is a human kingdom
//
int ModFryhtan::header_species(const char* playerName, int nameSize)
{
	if( playerName[nameSize-2] != FRYHTAN_HEADER_MARK || (int) strlen(playerName) > nameSize-3 )
		return 0;

	int monsterId = (unsigned char) playerName[nameSize-1];

	return is_valid_species(monsterId) ? monsterId : 0;
}
//----------- End of function ModFryhtan::header_species -----------//


//--------- Begin of function ModFryhtan::has_lair_commander ---------//
//
// Whether an Ordo or the king of the kingdom is in a lair with an active link
// with the enslaved town. Without one, the town pays no tribute and its
// loyalty drops.
//
int ModFryhtan::has_lair_commander(Town* townPtr)
{
	for( int i=0 ; i<townPtr->linked_firm_count ; i++ )
	{
		if( townPtr->linked_firm_enable_array[i] != LINK_EE )
			continue;

		Firm* firmPtr = firm_array[ townPtr->linked_firm_array[i] ];

		if( firmPtr->nation_recno == townPtr->nation_recno && firmPtr->overseer_recno && is_lair(firmPtr) )
			return 1;
	}

	return 0;
}
//----------- End of function ModFryhtan::has_lair_commander -----------//


//--------- Begin of function ModFryhtan::in_lair_grace ---------//
//
// An enslaved town that has lost the link with all its lairs doesn't rebel
// while it waits to become independent: the rebels would have no lair to
// attack.
//
int ModFryhtan::in_lair_grace(Town* townPtr)
{
	return is_enslaved_town(townPtr) && !townPtr->has_linked_camp(townPtr->nation_recno, 0);
}
//----------- End of function ModFryhtan::in_lair_grace -----------//


//--------- Begin of function ModFryhtan::make_town_independent ---------//
//
// An enslaved town declares its independence: a new resistance toward all the
// kingdoms (60 to 99), so that it doesn't surrender at once to a kingdom with
// an old zero resistance, and 100 toward its old Fryhtan kingdom, which has to
// fight it again to enslave it.
//
void ModFryhtan::make_town_independent(Town* townPtr)
{
	int nationRecno = townPtr->nation_recno;

	for( int i=0 ; i<MAX_RACE ; i++ )
	{
		for( int j=0 ; j<MAX_NATION ; j++ )
			townPtr->race_resistance_array[i][j] = (float) 60 + misc.random(40);

		if( nationRecno )
			townPtr->race_resistance_array[i][nationRecno-1] = (float) 100;
	}

	if( nationRecno == nation_array.player_recno )
		news_array.town_rebel(townPtr->town_recno, 0);		// 0 rebels: "Your village of ... has declared independence."

	townPtr->set_nation(0);
}
//----------- End of function ModFryhtan::make_town_independent -----------//


//--------- Begin of function ModFryhtan::lair_orphan_towns ---------//
//
// The enslaved towns linked to the lair and to no other lair of its kingdom:
// they lose their last lair when it is destroyed (Unit::hit_firm()).
//
// return: <int> the number of towns put in townRecnoArray (at most MAX_LINKED_FIRM_TOWN)
//
int ModFryhtan::lair_orphan_towns(Firm* firmPtr, short* townRecnoArray)
{
	if( !firmPtr->nation_recno || !is_fryhtan(firmPtr->nation_recno) || !is_lair(firmPtr) )
		return 0;

	int townCount = 0;

	for( int i=0 ; i<firmPtr->linked_town_count ; i++ )
	{
		Town* townPtr = town_array[ firmPtr->linked_town_array[i] ];

		if( townPtr->nation_recno != firmPtr->nation_recno )
			continue;

		//--- another lair of the kingdom linked to the town? ---//

		int otherLair = 0;

		for( int j=0 ; j<townPtr->linked_firm_count ; j++ )
		{
			Firm* otherFirm = firm_array[ townPtr->linked_firm_array[j] ];

			if( otherFirm != firmPtr && otherFirm->nation_recno == firmPtr->nation_recno && is_lair(otherFirm) )
			{
				otherLair = 1;
				break;
			}
		}

		if( !otherLair )
			townRecnoArray[townCount++] = (short) townPtr->town_recno;
	}

	return townCount;
}
//----------- End of function ModFryhtan::lair_orphan_towns -----------//


//--------- Begin of function ModFryhtan::lair_destroyed ---------//
//
// A lair of a Fryhtan kingdom has been destroyed (after Unit::hit_firm()
// deleted it). Its enslaved towns without another linked lair:
//
// - destroyed by rebels: the town declares its independence at once, and the
//   rebels nearby go back into it as villagers;
// - destroyed by an enemy kingdom: the town goes to that kingdom at once (a
//   normal town of a human kingdom, an enslaved town of a Fryhtan one);
// - destroyed by wild Fryhtans: the grace period of ModFryhtan::next_day_town()
//   as with any other loss of the lair.
//
void ModFryhtan::lair_destroyed(short* townRecnoArray, int townCount, Unit* attackUnit, int attackNationRecno, int lairNationRecno)
{
	int byRebels = attackUnit && attackUnit->unit_mode == UNIT_MODE_REBEL;
	int byEnemy  = !byRebels && attackNationRecno && attackNationRecno != lairNationRecno &&
						!nation_array.is_deleted(attackNationRecno);

	if( !byRebels && !byEnemy )
		return;

	for( int i=0 ; i<townCount ; i++ )
	{
		if( town_array.is_deleted(townRecnoArray[i]) )
			continue;

		Town* townPtr = town_array[ townRecnoArray[i] ];

		if( townPtr->nation_recno != lairNationRecno || townPtr->has_linked_camp(lairNationRecno, 0) )
			continue;

		if( byRebels )
		{
			set_lost_lair_date(townPtr->town_recno, 0);
			make_town_independent(townPtr);

			if( !town_array.is_deleted(townRecnoArray[i]) )
				rebels_return_to_town(townPtr);
		}
		else
		{
			set_lost_lair_date(townPtr->town_recno, 0);

			if( townPtr->nation_recno == nation_array.player_recno || attackNationRecno == nation_array.player_recno )
				news_array.town_surrendered(townPtr->town_recno, attackNationRecno);

			townPtr->set_nation(attackNationRecno);
		}
	}
}
//----------- End of function ModFryhtan::lair_destroyed -----------//


//--------- Begin of function ModFryhtan::rebels_return_to_town ---------//
//
// The rebels near a town that has won its independence by destroying its
// lair go back into it as villagers (the town stays a normal independent
// town, not one controlled by rebels). The leaders go last, so that a group
// doesn't pick a new leader or join a kingdom halfway (Rebel::process_leader_quit()).
//
void ModFryhtan::rebels_return_to_town(Town* townPtr)
{
	for( int leaderPass=0 ; leaderPass<2 ; leaderPass++ )
	{
		for( int unitRecno=unit_array.size() ; unitRecno>0 ; unitRecno-- )
		{
			if( unit_array.is_deleted(unitRecno) )
				continue;

			Unit* unitPtr = unit_array[unitRecno];

			if( unitPtr->unit_mode != UNIT_MODE_REBEL || !unitPtr->race_id || !unitPtr->is_visible() )
				continue;

			if( rebel_array.is_deleted(unitPtr->unit_mode_para) )
				continue;

			int isLeader = rebel_array[unitPtr->unit_mode_para]->leader_unit_recno == unitRecno;

			if( isLeader != leaderPass )
				continue;

			if( misc.points_distance( unitPtr->next_x_loc(), unitPtr->next_y_loc(),
											  townPtr->center_x, townPtr->center_y ) > REBELS_RETURN_DISTANCE )
			{
				continue;
			}

			if( townPtr->population >= MAX_TOWN_POPULATION )
				return;

			int raceId = unitPtr->race_id;

			rebel_array.drop_rebel_identity(unitRecno);		// leaves the rebel group
			townPtr->inc_pop(raceId, 0, 0);
			unit_array.del(unitRecno);
		}
	}
}
//----------- End of function ModFryhtan::rebels_return_to_town -----------//


//--------- Begin of function ModFryhtan::lost_lair_date ---------//
//
int32_t ModFryhtan::lost_lair_date(int townRecno)
{
	if( townRecno < 1 || townRecno > lost_lair_date_array_size )
		return 0;

	return lost_lair_date_array[townRecno-1];
}
//----------- End of function ModFryhtan::lost_lair_date -----------//


//--------- Begin of static function set_town_date ---------//
//
// Set a date in an array of dates by town recno, growing it if needed.
//
static void set_town_date(int32_t*& dateArray, int& arraySize, int townRecno, int32_t newDate)
{
	if( townRecno < 1 )
		return;

	if( townRecno > arraySize )
	{
		if( !newDate )
			return;

		int newSize = townRecno + 20;

		dateArray = (int32_t*) mem_resize( dateArray, sizeof(int32_t) * newSize );
		memset( dateArray + arraySize, 0, sizeof(int32_t) * (newSize - arraySize) );
		arraySize = newSize;
	}

	dateArray[townRecno-1] = newDate;
}
//----------- End of static function set_town_date -----------//


//--------- Begin of static function write_town_dates ---------//
//
// Write the non-zero dates of an array by town recno: their count, then the
// town recno and the date of each one.
//
static int write_town_dates(File* filePtr, int32_t* dateArray, int arraySize)
{
	int count = 0;

	for( int i=0 ; i<arraySize ; i++ )
	{
		if( dateArray[i] )
			count++;
	}

	if( !filePtr->file_put_short( (int16_t) count ) )
		return 0;

	for( int i=0 ; i<arraySize ; i++ )
	{
		if( dateArray[i] &&
			 ( !filePtr->file_put_short( (int16_t) (i+1) ) || !filePtr->file_put_long( dateArray[i] ) ) )
		{
			return 0;
		}
	}

	return 1;
}
//----------- End of static function write_town_dates -----------//


//--------- Begin of static function read_town_dates ---------//
//
static int read_town_dates(File* filePtr, int32_t*& dateArray, int& arraySize)
{
	int count = filePtr->file_get_short();

	if( count < 0 )
		return 0;

	for( int i=0 ; i<count ; i++ )
	{
		int townRecno = filePtr->file_get_short();
		int32_t townDate = filePtr->file_get_long();

		set_town_date( dateArray, arraySize, townRecno, townDate );
	}

	return 1;
}
//----------- End of static function read_town_dates -----------//


//--------- Begin of function ModFryhtan::set_lost_lair_date ---------//
//
void ModFryhtan::set_lost_lair_date(int townRecno, int32_t lostDate)
{
	set_town_date( lost_lair_date_array, lost_lair_date_array_size, townRecno, lostDate );
}
//----------- End of function ModFryhtan::set_lost_lair_date -----------//


//--------- Begin of function ModFryhtan::raid_date ---------//
//
int32_t ModFryhtan::raid_date(int townRecno)
{
	if( townRecno < 1 || townRecno > raid_date_array_size )
		return 0;

	return raid_date_array[townRecno-1];
}
//----------- End of function ModFryhtan::raid_date -----------//


//--------- Begin of function ModFryhtan::set_raid_date ---------//
//
void ModFryhtan::set_raid_date(int townRecno, int32_t raidDate)
{
	set_town_date( raid_date_array, raid_date_array_size, townRecno, raidDate );
}
//----------- End of function ModFryhtan::set_raid_date -----------//


//--------- Begin of function ModFryhtan::reset_town_dates ---------//
//
// A new town, or a town that has changed owner: no grace period or raid
// left from before (the town recnos are reused).
//
void ModFryhtan::reset_town_dates(int townRecno)
{
	set_lost_lair_date(townRecno, 0);
	set_raid_date(townRecno, 0);

	if( townRecno >= 1 && townRecno <= town_ordo_array_size )		// no Ordo left from a deleted town (ModFryhtan::ordo_leave_town() takes it out before)
		memset( town_ordo_array + townRecno - 1, 0, sizeof(TownOrdo) );
}
//----------- End of function ModFryhtan::reset_town_dates -----------//


//--------- Begin of function ModFryhtan::town_ordo ---------//
//
// return: <TownOrdo*> the Ordo inside the town, NULL if there is none
//
TownOrdo* ModFryhtan::town_ordo(int townRecno)
{
	if( townRecno < 1 || townRecno > town_ordo_array_size || !town_ordo_array[townRecno-1].unit_id )
		return NULL;

	return town_ordo_array + townRecno - 1;
}
//----------- End of function ModFryhtan::town_ordo -----------//


//--------- Begin of function ModFryhtan::can_ordo_enter_town ---------//
//
// An Ordo (a Fryhtan general, not the All High) can go into an enslaved town
// of its kingdom without an Ordo.
//
int ModFryhtan::can_ordo_enter_town(Unit* unitPtr, int townRecno)
{
	if( unitPtr->race_id || unitPtr->rank_id != RANK_GENERAL || unitPtr->spy_recno ||
		 unit_res[unitPtr->unit_id]->unit_class != UNIT_CLASS_MONSTER ||
		 !unitPtr->nation_recno || !is_fryhtan(unitPtr->nation_recno) )
	{
		return 0;
	}

	if( town_array.is_deleted(townRecno) || town_array[townRecno]->nation_recno != unitPtr->nation_recno )
		return 0;

	return town_ordo(townRecno) == NULL;
}
//----------- End of function ModFryhtan::can_ordo_enter_town -----------//


//--------- Begin of function ModFryhtan::ordo_enter_town ---------//
//
// The Ordo goes into the town (Town::assign_unit()): the unit is removed from
// the map and kept in town_ordo_array. It starts with Subjugate.
//
void ModFryhtan::ordo_enter_town(Town* townPtr, Unit* unitPtr)
{
	int townRecno = townPtr->town_recno;

	if( !can_ordo_enter_town(unitPtr, townRecno) )
	{
		unitPtr->stop2();
		return;
	}

	//------- grow the array if needed -------//

	if( townRecno > town_ordo_array_size )
	{
		int newSize = townRecno + 20;

		town_ordo_array = (TownOrdo*) mem_resize( town_ordo_array, sizeof(TownOrdo) * newSize );
		memset( town_ordo_array + town_ordo_array_size, 0, sizeof(TownOrdo) * (newSize - town_ordo_array_size) );
		town_ordo_array_size = newSize;
	}

	TownOrdo* ordoPtr = town_ordo_array + townRecno - 1;

	memset( ordoPtr, 0, sizeof(TownOrdo) );

	ordoPtr->unit_id      = unitPtr->unit_id;
	ordoPtr->name_id      = unitPtr->name_id;
	ordoPtr->nation_recno = (int8_t) unitPtr->nation_recno;
	ordoPtr->action       = ORDO_ACTION_SUBJUGATE;
	ordoPtr->combat_level = (uint8_t) unitPtr->skill.combat_level;
	ordoPtr->leadership   = (uint8_t) unitPtr->skill.get_skill(SKILL_LEADING);
	ordoPtr->hit_points   = (int16_t) MAX( 1, (int) unitPtr->hit_points );
	ordoPtr->skill_potential  = unitPtr->skill.skill_potential;
	ordoPtr->leadership_minor = unitPtr->skill.skill_level_minor;
	ordoPtr->combat_minor     = unitPtr->skill.combat_level_minor;

	//------- remove the unit from the map -------//

	unitPtr->set_rank(RANK_SOLDIER);		// not a general any more, so its death isn't reported (as Town::assign_unit() does)

	unit_array.disappear_in_town(unitPtr->sprite_recno, townRecno);
}
//----------- End of function ModFryhtan::ordo_enter_town -----------//


//--------- Begin of function ModFryhtan::ordo_leave_town ---------//
//
// The Ordo comes out of the town as an Ordo of its kingdom, next to the
// town.
//
// <int> townRecno - the town
// [int] forced    - 1 if the town is lost or destroyed: the Ordo is removed
//                   from it even if it can't come out (default: 0)
//
// return: <int> the recno of the unit, 0 if it couldn't come out
//
int ModFryhtan::ordo_leave_town(int townRecno, int forced)
{
	TownOrdo* ordoPtr = town_ordo(townRecno);

	if( !ordoPtr )
		return 0;

	TownOrdo ordo = *ordoPtr;

	if( sys.signal_exit_flag || !ordo.nation_recno || nation_array.is_deleted(ordo.nation_recno) ||
		 town_array.is_deleted(townRecno) )
	{
		memset( ordoPtr, 0, sizeof(TownOrdo) );
		return 0;
	}

	//------- a place for it next to the town -------//

	Town*		  townPtr = town_array[townRecno];
	UnitInfo*	  unitInfo = unit_res[ordo.unit_id];
	SpriteInfo* spriteInfo = sprite_res[unitInfo->sprite_id];

	int xLoc=townPtr->loc_x1, yLoc=townPtr->loc_y1;
	int unitRecno = 0;

	if( world.locate_space( &xLoc, &yLoc, townPtr->loc_x2, townPtr->loc_y2,
									spriteInfo->loc_width, spriteInfo->loc_height, unitInfo->mobile_type ) )
	{
		unitRecno = unit_array.add_unit( ordo.unit_id, ordo.nation_recno, RANK_SOLDIER, 100, xLoc, yLoc );
	}

	if( !unitRecno )
	{
		if( forced )
			memset( ordoPtr, 0, sizeof(TownOrdo) );

		return 0;
	}

	memset( ordoPtr, 0, sizeof(TownOrdo) );

	//------- the Ordo as it went in -------//

	Unit* unitPtr = unit_array[unitRecno];

	unitPtr->set_monster_id( unit_monster_id(ordo.unit_id) );
	unitPtr->set_rank( RANK_GENERAL );
	unitPtr->skill.set_skill( SKILL_LEADING );
	unitPtr->skill.skill_level = ordo.leadership;
	unitPtr->set_combat_level( ordo.combat_level );
	unitPtr->hit_points = (float) MAX( 1, MIN( (int) ordo.hit_points, (int) unitPtr->max_hit_points ) );
	unitPtr->name_id = ordo.name_id;
	unitPtr->skill.skill_potential    = ordo.skill_potential;
	unitPtr->skill.skill_level_minor  = ordo.leadership_minor;
	unitPtr->skill.combat_level_minor = ordo.combat_minor;

	return unitRecno;
}
//----------- End of function ModFryhtan::ordo_leave_town -----------//


//--------- Begin of function ModFryhtan::set_ordo_action ---------//
//
void ModFryhtan::set_ordo_action(int townRecno, int action)
{
	TownOrdo* ordoPtr = town_ordo(townRecno);

	if( !ordoPtr || ordoPtr->action == action )
		return;

	ordoPtr->action = (int8_t) action;

	if( action == ORDO_ACTION_KIDNAP )		// the first kidnap comes a whole period later
	{
		int days = kidnap_days(ordoPtr->leadership);
		ordoPtr->next_kidnap_date = info.game_date + days/2 + misc.random(days+1);
	}

	if( action == ORDO_ACTION_TASKMASTER )	// the first death too
		ordoPtr->next_death_date = info.game_date + TASKMASTER_DEATH_DAYS + misc.random(TASKMASTER_DEATH_DAYS_RANGE+1);
}
//----------- End of function ModFryhtan::set_ordo_action -----------//


//--------- Begin of function ModFryhtan::next_day_town_ordo ---------//
//
// The Ordo inside an enslaved town: it eats and costs as outside, and does
// its action.
//
void ModFryhtan::next_day_town_ordo(Town* townPtr)
{
	int townRecno = townPtr->town_recno;
	TownOrdo* ordoPtr = town_ordo(townRecno);

	if( !ordoPtr )
		return;

	if( ordoPtr->nation_recno != townPtr->nation_recno )		// shouldn't happen: Town::set_nation() takes it out
	{
		ordo_leave_town(townRecno, 1);
		return;
	}

	Nation* nationPtr = nation_array[ordoPtr->nation_recno];
	int leadership = ordoPtr->leadership;

	//------- it eats and costs as outside (Unit::pay_expense()) -------//

	if( nationPtr->food > 0 )
		nationPtr->consume_food( fryhtan_food_year(ordoPtr->unit_id, ordoPtr->nation_recno) / 365 );

	if( nationPtr->cash > 0 )
		nationPtr->add_expense( EXPENSE_MOBILE_UNIT, (float) unit_res[ordoPtr->unit_id]->year_cost / 365, 1 );

	//------- Sacrificial Rites: whatever its order, it sacrifices villagers of an overpopulated town -------//

	if( is_overpopulated(townPtr) && has_tech(ordoPtr->nation_recno, FRYHTAN_TECH_SACRIFICIAL_RITES) )
	{
		int lifePoints = 0, sacrificeCount;

		for( sacrificeCount=0 ; sacrificeCount<RITES_VILLAGERS && townPtr->population > 1 ; sacrificeCount++ )
		{
			townPtr->kill_town_people(0, 0);		// a random race, by its people; no attacker: no loyalty loss
			reward_town_villager(ordoPtr->nation_recno);		// the spoils and the Life Points of a villager killed...

			int extraPoints = RITES_LIFE_POINTS_MIN + misc.random(RITES_LIFE_POINTS_MAX - RITES_LIFE_POINTS_MIN + 1);
			add_life_points( ordoPtr->nation_recno, extraPoints );		// ...and more
			lifePoints += LIFE_POINTS_VILLAGER + extraPoints;
		}

		if( sacrificeCount && townPtr->nation_recno == nation_array.player_recno )
			news_array.ordo_sacrifice( townRecno, ordoPtr->name_id, sacrificeCount, lifePoints );
	}

	//------- Subjugate: it culls an overpopulated town, to prevent rebellions (no news) -------//

	if( ordoPtr->action == ORDO_ACTION_SUBJUGATE && info.game_date%SUBJUGATE_CULL_DAYS == townRecno%SUBJUGATE_CULL_DAYS &&
		 is_overpopulated(townPtr) )
		cull_town(townPtr);

	//------- its leadership and combat rise, as a general in a lair -------//

	if( info.game_date%15 == townRecno%15 )
		train_town_ordo(townPtr, ordoPtr);

	//------- Kidnap: villagers of the races of the town, by its people -------//

	if( ordoPtr->action == ORDO_ACTION_KIDNAP && info.game_date >= ordoPtr->next_kidnap_date )
	{
		int days = kidnap_days(leadership);
		ordoPtr->next_kidnap_date = info.game_date + days/2 + misc.random(days+1);

		int kidnapCount = 0;

		for( int k=0 ; k<KIDNAP_VILLAGERS ; k++ )
		{
			if( townPtr->population <= 0 || townPtr->population >= MAX_TOWN_POPULATION )
				break;


			int pick = misc.random(townPtr->population);
			int raceId = 0;

			for( int i=0 ; i<MAX_RACE ; i++ )
			{
				if( pick < townPtr->race_pop_array[i] )
				{
					raceId = i+1;
					break;
				}

				pick -= townPtr->race_pop_array[i];
			}

			if( !raceId )
				break;

			townPtr->inc_pop( raceId, 0, (int) townPtr->race_loyalty_array[raceId-1] );
			kidnapCount++;
		}

		if( kidnapCount && townPtr->nation_recno == nation_array.player_recno )
			news_array.ordo_kidnap( townRecno, ordoPtr->name_id, kidnapCount );
	}

	//------- Taskmaster: more food every day, more tribute every month -------//

	else if( ordoPtr->action == ORDO_ACTION_TASKMASTER )
	{
		nationPtr->add_food( (float) townPtr->population * taskmaster_food(leadership) / 365 );

		//--- the forced labor kills a villager now and then (no news) ---//

		if( !ordoPtr->next_death_date )		// a saved game from before the deaths
			ordoPtr->next_death_date = info.game_date + TASKMASTER_DEATH_DAYS + misc.random(TASKMASTER_DEATH_DAYS_RANGE+1);

		if( info.game_date >= ordoPtr->next_death_date )
		{
			ordoPtr->next_death_date = info.game_date + TASKMASTER_DEATH_DAYS + misc.random(TASKMASTER_DEATH_DAYS_RANGE+1);

			if( townPtr->population > 1 )		// never the last one
			{
				townPtr->kill_town_people(0, 0);
				add_life_points( ordoPtr->nation_recno, LIFE_POINTS_VILLAGER );
			}
		}
	}
}
//----------- End of function ModFryhtan::next_day_town_ordo -----------//


//--------- Begin of function ModFryhtan::train_town_ordo ---------//
//
// Every 15 days, as FirmCamp::train_unit() for a general: the leadership of
// the Ordo rises as with one soldier for each ORDO_TOWN_VILLAGERS_PER_SOLDIER
// villagers (at most MAX_WORKER), ORDO_LEADERSHIP_PERCENT as fast as a human
// general; its combat rises as a general's; and it recovers 1 hit point
// (FirmCamp::recover_hit_point()).
//
void ModFryhtan::train_town_ordo(Town* townPtr, TownOrdo* ordoPtr)
{
	UnitInfo* unitInfo = unit_res[ordoPtr->unit_id];
	int maxHitPoints = MAX( 1, (int) unitInfo->hit_points * ordoPtr->combat_level / 100 );		// as Unit::set_combat_level()
	int hitPoints    = MIN( (int) ordoPtr->hit_points, maxHitPoints );
	int potential    = 100 + ordoPtr->skill_potential*2;

	//------- leadership -------//

	int soldierCount = MIN( (int) MAX_WORKER, townPtr->population / ORDO_TOWN_VILLAGERS_PER_SOLDIER );

	if( soldierCount > 0 && ordoPtr->leadership < 100 )
	{
		int incValue = 5 * soldierCount * hitPoints / maxHitPoints * potential / 100
							* ORDO_LEADERSHIP_PERCENT / 100;
		int levelMinor = ordoPtr->leadership_minor + incValue;

		while( levelMinor >= 100 && ordoPtr->leadership < 100 )
		{
			levelMinor -= 100;
			ordoPtr->leadership++;
		}

		ordoPtr->leadership_minor = (uint8_t) MIN( 99, levelMinor );
	}

	//------- combat -------//

	if( ordoPtr->combat_level < 100 )
	{
		int incValue = 20 * hitPoints / maxHitPoints * potential / 100;
		int levelMinor = ordoPtr->combat_minor + incValue;

		while( levelMinor >= 100 && ordoPtr->combat_level < 100 )
		{
			levelMinor -= 100;
			ordoPtr->combat_level++;
		}

		ordoPtr->combat_minor = (uint8_t) MIN( 99, levelMinor );
	}

	//------- hit points -------//

	maxHitPoints = MAX( 1, (int) unitInfo->hit_points * ordoPtr->combat_level / 100 );

	if( ordoPtr->hit_points < maxHitPoints )
		ordoPtr->hit_points++;
}
//----------- End of function ModFryhtan::train_town_ordo -----------//


//--------- Begin of function ModFryhtan::enslave_min_resistance ---------//
//
// How low the lairs lower the resistance of an independent town toward the
// kingdom, by the leadership of the general of the lair: 60 with leadership
// 30 or less, 45 with 100.
//
int ModFryhtan::enslave_min_resistance(int leadership)
{
	int level = ordo_task_level(leadership) - ORDO_TASK_MIN_LEADERSHIP;

	return ENSLAVE_MIN_RESISTANCE_LOW - level * (ENSLAVE_MIN_RESISTANCE_LOW - ENSLAVE_MIN_RESISTANCE)
			 / (ORDO_TASK_MAX_LEADERSHIP - ORDO_TASK_MIN_LEADERSHIP);
}
//----------- End of function ModFryhtan::enslave_min_resistance -----------//


//--------- Begin of function ModFryhtan::promote_ordo ---------//
//
// A Fryhtan soldier of a Fryhtan kingdom becomes an Ordo, with a name of its
// own. It keeps its combat and leadership.
//
void ModFryhtan::promote_ordo(Unit* unitPtr)
{
	if( unitPtr->skill.skill_id != SKILL_LEADING )		// a soldier from before the leadership of the soldiers
	{
		unitPtr->skill.set_skill( SKILL_LEADING );
		unitPtr->skill.skill_level = LAIR_TRAIN_LEADERSHIP;
	}

	unitPtr->set_rank(RANK_GENERAL);
	unitPtr->name_id = (uint16_t) new_ordo_name_id( unitPtr->nation_recno );

	if( unit_array.selected_recno == unitPtr->sprite_recno )		// show its new name
		info.disp();
}


//--------- Begin of function ModFryhtan::demote_ordo ---------//
//
// An Ordo becomes a soldier, with a series no. as name (Unit::init()).
//
void ModFryhtan::demote_ordo(Unit* unitPtr)
{
	unitPtr->set_rank(RANK_SOLDIER);

	if( unitPtr->nation_recno )
		unitPtr->name_id = ++nation_array[unitPtr->nation_recno]->last_unit_name_id_array[unitPtr->unit_id-1];

	if( unit_array.selected_recno == unitPtr->sprite_recno )
		info.disp();
}
//----------- End of function ModFryhtan::demote_ordo -----------//


//--------- Begin of function ModFryhtan::is_overpopulated ---------//
//
// An enslaved town with OVERPOPULATED_TOWN_POPULATION people or more loses
// loyalty bit by bit (next_day_town_overpopulation()) and rebels in the end.
//
int ModFryhtan::is_overpopulated(Town* townPtr)
{
	return townPtr->population >= overpopulated_population(townPtr->nation_recno);
}


//--------- Begin of function ModFryhtan::overpopulated_population ---------//
//
// The people an enslaved town of the kingdom is overpopulated with:
// OVERPOPULATED_TOWN_POPULATION, DOMINION_OVERPOPULATION more with Dominion.
//
int ModFryhtan::overpopulated_population(int nationRecno)
{
	if( mod_fryhtan.has_tech(nationRecno, FRYHTAN_TECH_DOMINION) )
		return OVERPOPULATED_TOWN_POPULATION + DOMINION_OVERPOPULATION;

	return OVERPOPULATED_TOWN_POPULATION;
}
//----------- End of function ModFryhtan::overpopulated_population -----------//


//--------- Begin of function ModFryhtan::next_day_town_overpopulation ---------//
//
// The loyalty of an overpopulated enslaved town drops 1 point every
// OVERPOPULATED_LOYALTY_DAYS days (Town::update_target_loyalty() keeps it
// from rising back meanwhile).
//
void ModFryhtan::next_day_town_overpopulation(Town* townPtr)
{
	if( !is_overpopulated(townPtr) || info.game_date % OVERPOPULATED_LOYALTY_DAYS != townPtr->town_recno % OVERPOPULATED_LOYALTY_DAYS )
		return;

	for( int i=0 ; i<MAX_RACE ; i++ )
	{
		if( townPtr->race_pop_array[i] > 0 )
		{
			townPtr->race_loyalty_array[i] = MAX( 0.0f, townPtr->race_loyalty_array[i] - 1 );

			//--- the target goes down with it, so that Town::update_loyalty() doesn't raise it back ---//

			townPtr->race_target_loyalty_array[i] = MIN( (int) townPtr->race_target_loyalty_array[i],
																		(int) townPtr->race_loyalty_array[i] );
		}
	}
}


//--------- Begin of function ModFryhtan::can_cull_town ---------//
//
int ModFryhtan::can_cull_town(Town* townPtr)
{
	return is_enslaved_town(townPtr) && town_ordo(townPtr->town_recno) &&		// only with an Ordo inside
			 townPtr->population >= CULL_MIN_POPULATION;
}


//--------- Begin of function ModFryhtan::cull_town ---------//
//
// Cull Population: kills 3 or 4 villagers of the enslaved town (of its
// races, by its people), with the spoils of a villager killed for each one
// (reward_town_villager()). No effect on the loyalty of the others.
//
// return: <int> the villagers killed
//
int ModFryhtan::cull_town(Town* townPtr)
{
	if( !can_cull_town(townPtr) )
		return 0;

	int killCount = CULL_MIN_KILL + misc.random(CULL_MAX_KILL - CULL_MIN_KILL + 1);
	int nationRecno = townPtr->nation_recno;
	int i;

	for( i=0 ; i<killCount && townPtr->population > 1 ; i++ )
	{
		townPtr->kill_town_people(0, 0);		// a random race, by its people; no attacker: no loyalty loss
		reward_town_villager(nationRecno);
	}

	return i;
}
//----------- End of function ModFryhtan::cull_town -----------//


//--------- Begin of function ModFryhtan::subjugate_loyalty ---------//
//
// The lowest loyalty Subjugate keeps in the town: the leadership of the Ordo,
// at most SUBJUGATE_MAX_LOYALTY. 0 without an Ordo doing Subjugate.
//
int ModFryhtan::subjugate_loyalty(Town* townPtr)
{
	TownOrdo* ordoPtr = town_ordo(townPtr->town_recno);

	if( !ordoPtr || ordoPtr->action != ORDO_ACTION_SUBJUGATE )
		return 0;

	return MIN( (int) ordoPtr->leadership, (int) SUBJUGATE_MAX_LOYALTY );
}
//----------- End of function ModFryhtan::subjugate_loyalty -----------//


//--------- Begin of function ModFryhtan::ordo_task_level ---------//
//
int ModFryhtan::ordo_task_level(int leadership)
{
	return MAX( (int) ORDO_TASK_MIN_LEADERSHIP, MIN( (int) ORDO_TASK_MAX_LEADERSHIP, leadership ) );
}


//--------- Begin of function ModFryhtan::kidnap_days ---------//
//
// The average days between kidnaps: 120 with leadership 30, 60 with 100.
//
int ModFryhtan::kidnap_days(int leadership)
{
	int level = ordo_task_level(leadership) - ORDO_TASK_MIN_LEADERSHIP;

	return KIDNAP_DAYS_LOW - level * (KIDNAP_DAYS_LOW - KIDNAP_DAYS_HIGH) / (ORDO_TASK_MAX_LEADERSHIP - ORDO_TASK_MIN_LEADERSHIP);
}


//--------- Begin of function ModFryhtan::taskmaster_food ---------//
//
// Extra food a year for each villager: 5 with leadership 30, 20 with 100.
//
int ModFryhtan::taskmaster_food(int leadership)
{
	int level = ordo_task_level(leadership) - ORDO_TASK_MIN_LEADERSHIP;

	return TASKMASTER_FOOD_LOW + level * (TASKMASTER_FOOD_HIGH - TASKMASTER_FOOD_LOW) / (ORDO_TASK_MAX_LEADERSHIP - ORDO_TASK_MIN_LEADERSHIP);
}


//--------- Begin of function ModFryhtan::fryhtan_food_year ---------//
//
// The food a Fryhtan of the nation eats a year: as an invoked one
// (invoked_fryhtan_food_year()), FRYHTAN_KINGDOM_FOOD_PERCENT of it in a
// Fryhtan kingdom.
//
float ModFryhtan::fryhtan_food_year(int unitId, int nationRecno)
{
	float food = (float) invoked_fryhtan_food_year(unitId);

	if( nationRecno && mod_fryhtan.is_fryhtan(nationRecno) )
		food = food * FRYHTAN_KINGDOM_FOOD_PERCENT / 100;

	return food;
}
//----------- End of function ModFryhtan::fryhtan_food_year -----------//


//--------- Begin of function ModFryhtan::ordo_action_name ---------//
//
const char* ModFryhtan::ordo_action_name(int action)
{
	switch( action )
	{
		case ORDO_ACTION_KIDNAP:		return _("Kidnap");
		case ORDO_ACTION_TASKMASTER:	return _("Taskmaster");
		default:								return _("Subjugate");
	}
}
//----------- End of function ModFryhtan::ordo_action_name -----------//


//--------- Begin of function ModFryhtan::town_role_name ---------//
//
// The action (role) of the Ordo inside a town of the player, shown next to
// its name on the map and in the Villages report. NULL without an Ordo or
// for the towns of other kingdoms.
//
const char* ModFryhtan::town_role_name(Town* townPtr)
{
	if( !nation_array.player_recno || townPtr->nation_recno != nation_array.player_recno )
		return NULL;

	TownOrdo* ordoPtr = town_ordo(townPtr->town_recno);

	return ordoPtr ? ordo_action_name(ordoPtr->action) : NULL;
}
//----------- End of function ModFryhtan::town_role_name -----------//


//--------- Begin of function ModFryhtan::raid_lair ---------//
//
// return: <int> the firm recno of the lair of the kingdom linked to the town
//               with the general of the highest leadership, 0 if there is
//               no linked lair with a general
//
int ModFryhtan::raid_lair(Town* townPtr, int nationRecno)
{
	int bestRecno = 0, bestLeadership = -1;

	for( int i=0 ; i<townPtr->linked_firm_count ; i++ )
	{
		Firm* firmPtr = firm_array[ townPtr->linked_firm_array[i] ];

		if( firmPtr->nation_recno != nationRecno || !is_lair(firmPtr) || !firmPtr->overseer_recno )
			continue;

		int leadership = unit_array[firmPtr->overseer_recno]->skill.skill_level;

		if( leadership > bestLeadership )
		{
			bestLeadership = leadership;
			bestRecno = firmPtr->firm_recno;
		}
	}

	return bestRecno;
}
//----------- End of function ModFryhtan::raid_lair -----------//


//--------- Begin of function ModFryhtan::raid_loot ---------//
//
// The gold (and the same food) a raid steals: RAID_LOOT_PER_PERSON per person,
// at most RAID_MAX_LOOT, x (100 + leadership) / 100 with the leadership of the
// general of the lair that raids (x2 with leadership 100).
//
int ModFryhtan::raid_loot(Town* townPtr, int nationRecno)
{
	int loot = MIN( townPtr->population * (int) RAID_LOOT_PER_PERSON, (int) RAID_MAX_LOOT );
	int lairRecno = raid_lair(townPtr, nationRecno);
	int leadership = 0;

	if( lairRecno && firm_array[lairRecno]->overseer_recno )
		leadership = unit_array[ firm_array[lairRecno]->overseer_recno ]->skill.get_skill(SKILL_LEADING);

	return loot * (100 + leadership) / 100;
}
//----------- End of function ModFryhtan::raid_loot -----------//


//--------- Begin of function ModFryhtan::can_see_raid ---------//
//
// The Raid button is shown on an independent town linked to a lair of the
// Fryhtan kingdom with a general.
//
int ModFryhtan::can_see_raid(Town* townPtr, int nationRecno)
{
	return nationRecno && is_fryhtan(nationRecno) && townPtr->nation_recno == 0 &&
			 townPtr->population > 0 && raid_lair(townPtr, nationRecno);
}
//----------- End of function ModFryhtan::can_see_raid -----------//


//--------- Begin of function ModFryhtan::raid_days_left ---------//
//
int ModFryhtan::raid_days_left(Town* townPtr)
{
	int32_t lastDate = raid_date(townPtr->town_recno);

	if( !lastDate || info.game_date >= lastDate + RAID_INTERVAL_DAYS )
		return 0;

	return lastDate + RAID_INTERVAL_DAYS - info.game_date;
}
//----------- End of function ModFryhtan::raid_days_left -----------//


//--------- Begin of function ModFryhtan::raid_max_defender ---------//
//
// The most defenders that come out in a raid: one for each
// RAID_PEOPLE_PER_DEFENDER people plus one, and one more with a resistance
// above RAID_RESISTANCE_EXTRA1 or two more above RAID_RESISTANCE_EXTRA2.
//
int ModFryhtan::raid_max_defender(Town* townPtr, int nationRecno)
{
	int resistance = townPtr->average_resistance(nationRecno);
	int maxCount = townPtr->population / RAID_PEOPLE_PER_DEFENDER + 1;

	if( resistance > RAID_RESISTANCE_EXTRA2 )
		maxCount += 2;
	else if( resistance > RAID_RESISTANCE_EXTRA1 )
		maxCount += 1;

	return maxCount;
}
//----------- End of function ModFryhtan::raid_max_defender -----------//


//--------- Begin of function ModFryhtan::is_raid_weakened ---------//
//
// A town with a resistance of RAID_MIN_RESISTANCE or less toward the
// kingdom can't be raided: no defenders would come out. It's ready to be
// attacked directly.
//
int ModFryhtan::is_raid_weakened(Town* townPtr, int nationRecno)
{
	return townPtr->average_resistance(nationRecno) <= RAID_MIN_RESISTANCE;
}
//----------- End of function ModFryhtan::is_raid_weakened -----------//


//--------- Begin of function ModFryhtan::raid_defender_count ---------//
//
// About how many defenders come out to attack the lair: up to
// raid_max_defender(), if the resistance toward the kingdom is high enough
// (the rule of Town::mobilize_defender(), on the average).
//
int ModFryhtan::raid_defender_count(Town* townPtr, int nationRecno)
{
	int maxCount = raid_max_defender(townPtr, nationRecno);

	float resistance = (float) townPtr->average_resistance(nationRecno);
	int   count = 0;

	while( count < maxCount && resistance >= RAID_MIN_RESISTANCE && resistance >= count*5 )
	{
		resistance -= resistance / MAX(1, townPtr->population - count);
		count++;
	}

	return count;
}
//----------- End of function ModFryhtan::raid_defender_count -----------//


//--------- Begin of function ModFryhtan::raid_town ---------//
//
// Raid an independent town from a linked lair: steal a little gold and
// food, call out its defenders (led by a stronger one) to attack the lair,
// and lower its resistance by RAID_RESISTANCE_DEC. Killing the defenders
// lowers the resistance of the town; the ones that go back to the town give
// it back. A town with a resistance of RAID_MIN_RESISTANCE or less can't be
// raided: it's ready to be attacked directly.
//
// return: <int> 1 if the raid has been done
//
int ModFryhtan::raid_town(Town* townPtr, int nationRecno)
{
	if( !can_see_raid(townPtr, nationRecno) || raid_days_left(townPtr) || is_raid_weakened(townPtr, nationRecno) )
		return 0;

	int lairRecno = raid_lair(townPtr, nationRecno);
	Firm* lairPtr = firm_array[lairRecno];

	if( lairPtr->worker_count == 0 )		// it needs soldiers in the lair
		return 0;

	int townRecno = townPtr->town_recno;
	int townNameId = townPtr->town_name_id;
	int population = townPtr->population;

	set_raid_date(townRecno, info.game_date);

	//------- the loot -------//

	Nation* nationPtr = nation_array[nationRecno];

	int loot = raid_loot(townPtr, nationRecno);

	nationPtr->add_income( INCOME_TREASURE, (float) loot );
	nationPtr->add_food( (float) loot );

	//------- villagers killed, by the size of the town (off for now) -------//

	int killCount = 0;

	if( RAID_KILL_VILLAGERS )
	{
		if( population < 20 )
			killCount = misc.random(4)==0 ? 1 : 0;		// small: unlikely
		else if( population < 40 )
			killCount = 1;										// medium
		else
			killCount = 1 + misc.random(2);				// large: 1 or 2

		killCount = MIN( killCount, population-1 );		// the raid never empties the town

		for( int i=0 ; i<killCount ; i++ )
			townPtr->kill_town_people(0, nationRecno);

		if( town_array.is_deleted(townRecno) )
			return 1;
	}

	//------- the defenders come out and attack the lair -------//

	townPtr->set_hostile_nation(nationRecno);

	int maxDefender = raid_max_defender(townPtr, nationRecno);
	int defenderCount = 0;

	while( defenderCount < maxDefender )
	{
		int unitRecno = townPtr->mobilize_defender(nationRecno);

		if( !unitRecno )
			break;

		Unit* unitPtr = unit_array[unitRecno];

		//--- the first one is the leader of the defense: stronger, and it carries twice the resistance ---//

		if( defenderCount == 0 )
		{
			unitPtr->set_combat_level( MIN( 100, unitPtr->skill.combat_level + RAID_LEADER_COMBAT_BONUS ) );
			unitPtr->hit_points = (float) unitPtr->max_hit_points;

			int carried = unitPtr->skill.skill_level;		// the resistance it took away (Town::mobilize_defender())

			for( int i=0 ; i<MAX_NATION ; i++ )
			{
				float& resistance = townPtr->race_resistance_array[unitPtr->race_id-1][i];
				resistance = MAX( (float) 0, resistance - carried );
			}

			unitPtr->skill.skill_level = (char) MIN( 100, carried*2 );
		}

		unitPtr->attack_firm( lairPtr->loc_x1, lairPtr->loc_y1 );
		defenderCount++;
	}

	//------- every raid lowers the resistance a bit -------//

	for( int i=0 ; i<MAX_RACE ; i++ )
	{
		if( townPtr->race_pop_array[i] == 0 )
			continue;

		float& resistance = townPtr->race_resistance_array[i][nationRecno-1];
		resistance = MAX( (float) 0, resistance - RAID_RESISTANCE_DEC );
	}

	if( nationRecno == nation_array.player_recno )
		news_array.town_raided(townRecno, townNameId, loot, killCount, defenderCount);

	return 1;
}
//----------- End of function ModFryhtan::raid_town -----------//


//--------- Begin of function ModFryhtan::reward_town_villager ---------//
//
// A villager killed in a town attacked by a Fryhtan kingdom gives the same
// gold and food as killing a villager (not when the town is raided).
//
void ModFryhtan::reward_town_villager(int nationRecno)
{
	if( !nationRecno || !is_fryhtan(nationRecno) || nation_array.is_deleted(nationRecno) )
		return;

	Nation* nationPtr = nation_array[nationRecno];

	nationPtr->add_income( INCOME_TREASURE, (float) KILL_GOLD_UNIT );
	nationPtr->add_food( (float) KILL_FOOD_UNIT );

	add_life_points( nationRecno, LIFE_POINTS_VILLAGER );
}
//----------- End of function ModFryhtan::reward_town_villager -----------//


//--------- Begin of function ModFryhtan::nearest_lair ---------//
//
// return: <int> the firm recno of the lair of the town's kingdom nearest to
//               the town, 0 if there is none
//
int ModFryhtan::nearest_lair(Town* townPtr)
{
	int bestRecno = 0, bestDistance = 0x7FFF;

	for( int i=firm_array.size() ; i>0 ; i-- )
	{
		if( firm_array.is_deleted(i) )
			continue;

		Firm* firmPtr = firm_array[i];

		if( firmPtr->nation_recno != townPtr->nation_recno || !is_lair(firmPtr) )
			continue;

		int distance = misc.points_distance( townPtr->center_x, townPtr->center_y, firmPtr->center_x, firmPtr->center_y );

		if( distance < bestDistance )
		{
			bestDistance = distance;
			bestRecno = i;
		}
	}

	return bestRecno;
}
//----------- End of function ModFryhtan::nearest_lair -----------//


//--------- Begin of function ModFryhtan::is_linked_wild_lair ---------//
//
// 7kmod-fryhtan, phase 5: a wild lair is linked to the kingdom when one of
// its finished lairs is within LAIR_CAPTURE_DISTANCE of it.
//
int ModFryhtan::is_linked_wild_lair(FirmMonster* monsterFirm, int nationRecno)
{
	if( !nationRecno || !is_fryhtan(nationRecno) || monsterFirm->nation_recno )
		return 0;

	for( int i=firm_array.size() ; i>0 ; i-- )
	{
		if( firm_array.is_deleted(i) )
			continue;

		Firm* firmPtr = firm_array[i];

		if( firmPtr->nation_recno == nationRecno && is_linked_lair(firmPtr, monsterFirm) )
			return 1;
	}

	return 0;
}
//----------- End of function ModFryhtan::is_linked_wild_lair -----------//


//--------- Begin of function ModFryhtan::is_linked_lair ---------//
//
// Whether a finished lair of a Fryhtan kingdom is linked to a wild lair:
// within LAIR_CAPTURE_DISTANCE of it.
//
int ModFryhtan::is_linked_lair(Firm* lairPtr, FirmMonster* monsterFirm)
{
	if( lairPtr->under_construction || !lairPtr->nation_recno || !is_lair(lairPtr) || monsterFirm->nation_recno )
		return 0;

	return misc.rects_distance( monsterFirm->loc_x1, monsterFirm->loc_y1, monsterFirm->loc_x2, monsterFirm->loc_y2,
										 lairPtr->loc_x1, lairPtr->loc_y1, lairPtr->loc_x2, lairPtr->loc_y2 ) <= LAIR_CAPTURE_DISTANCE;
}
//----------- End of function ModFryhtan::is_linked_lair -----------//


//--------- Begin of function ModFryhtan::capture_leadership ---------//
//
// The best leadership of the Ordos (or the king) in the lairs of the kingdom
// linked to a wild lair.
//
// return: <int> the leadership, -1 if no Ordo or king is in a linked lair
//
int ModFryhtan::capture_leadership(FirmMonster* monsterFirm, int nationRecno)
{
	int bestLeadership = -1;

	for( int i=firm_array.size() ; i>0 ; i-- )
	{
		if( firm_array.is_deleted(i) )
			continue;

		Firm* firmPtr = firm_array[i];

		if( firmPtr->nation_recno != nationRecno || !firmPtr->overseer_recno || !is_linked_lair(firmPtr, monsterFirm) )
			continue;

		int leadership = unit_array[firmPtr->overseer_recno]->skill.get_skill(SKILL_LEADING);

		if( leadership > bestLeadership )
			bestLeadership = leadership;
	}

	return bestLeadership;
}
//----------- End of function ModFryhtan::capture_leadership -----------//


//--------- Begin of function ModFryhtan::capture_cost ---------//
//
// The tribute for capturing a wild lair: the base cost (the sum of the combat
// levels x OFFER_COST_PER_COMBAT) with the leadership of the best linked
// general: x1 with leadership 100, x3 with 30 or less, gradual in between.
//
// return: <int> the cost, 0 if no Ordo or king is in a linked lair (it can't be captured)
//
int ModFryhtan::capture_cost(FirmMonster* monsterFirm, int nationRecno, LairOffer* offerPtr)
{
	int leadership = capture_leadership(monsterFirm, nationRecno);

	if( leadership < 0 )
		return 0;

	leadership = MAX( OFFER_MIN_LEADERSHIP, MIN( OFFER_MAX_LEADERSHIP, leadership ) );

	int range = OFFER_MAX_LEADERSHIP - OFFER_MIN_LEADERSHIP;		// 70

	return offer_cost(offerPtr) * ( range + 2 * (OFFER_MAX_LEADERSHIP-leadership) ) / range;
}
//----------- End of function ModFryhtan::capture_cost -----------//


//--------- Begin of function ModFryhtan::get_offer ---------//
//
LairOffer* ModFryhtan::get_offer(FirmMonster* monsterFirm)
{
	int firmRecno = monsterFirm->firm_recno;

	if( firmRecno < 1 || firmRecno > offer_array_size )
		return NULL;

	LairOffer* offerPtr = offer_array+firmRecno-1;

	if( !offerPtr->monster_id || offerPtr->monster_id != monsterFirm->monster_id )		// the recno may have been reused
		return NULL;

	return offerPtr;
}
//----------- End of function ModFryhtan::get_offer -----------//


//--------- Begin of function ModFryhtan::think_offer ---------//
//
// Return the offer of a wild lair, creating it the first time the lair is
// linked to the kingdom:
//
// - an Ordo with the combat of the best general of the wild lair (its king
//   if it has none) and a leadership of 30-60;
// - 4-6 soldiers of a large species or 6-8 of a small one, with combat 20-40.
//
// The numbers use their own random generator, so looking at the lair doesn't
// change the game's random sequence.
//
// return: <LairOffer*> the offer, NULL if the lair isn't linked
//
LairOffer* ModFryhtan::think_offer(FirmMonster* monsterFirm, int nationRecno)
{
	LairOffer* offerPtr = get_offer(monsterFirm);

	if( offerPtr )
		return offerPtr;

	if( !is_linked_wild_lair(monsterFirm, nationRecno) )
		return NULL;

	//------ the best general of the wild lair ------//

	int bestCombat = 0;

	for( int i=0 ; i<monsterFirm->monster_general_count ; i++ )
		bestCombat = MAX( bestCombat, (int) monsterFirm->monster_general_array[i].combat_level );

	if( !bestCombat && monsterFirm->monster_king.monster_id )
		bestCombat = monsterFirm->monster_king.combat_level;

	if( !bestCombat )
		bestCombat = OFFER_SOLDIER_MAX_COMBAT;

	//------ allocate it ------//

	int firmRecno = monsterFirm->firm_recno;

	if( firmRecno > offer_array_size )
	{
		int newSize = firmRecno + 50;

		offer_array = (LairOffer*) mem_resize( offer_array, sizeof(LairOffer) * newSize );
		memset( offer_array+offer_array_size, 0, sizeof(LairOffer) * (newSize-offer_array_size) );
		offer_array_size = newSize;
	}

	offerPtr = offer_array+firmRecno-1;
	memset( offerPtr, 0, sizeof(LairOffer) );

	//------ simulate it ------//

	uint32_t seed = (uint32_t) firmRecno * 2654435761u + (uint32_t) info.game_date * 40503u + 12345u;

	#define OFFER_RANDOM(n)		( seed = seed * 1103515245u + 12345u, (int) ((seed >> 16) % (uint32_t)(n)) )

	int monsterId = monsterFirm->monster_id;

	offerPtr->monster_id		  = monsterId;
	offerPtr->ordo_combat	  = bestCombat;
	offerPtr->ordo_leadership = OFFER_ORDO_MIN_LEADERSHIP + OFFER_RANDOM(OFFER_ORDO_MAX_LEADERSHIP-OFFER_ORDO_MIN_LEADERSHIP+1);

	if( is_large_monster(monsterId) )
		offerPtr->soldier_count = 4 + OFFER_RANDOM(3);		// 4-6
	else
		offerPtr->soldier_count = 6 + OFFER_RANDOM(3);		// 6-8

	for( int i=0 ; i<offerPtr->soldier_count ; i++ )
		offerPtr->soldier_combat[i] = OFFER_SOLDIER_MIN_COMBAT + OFFER_RANDOM(OFFER_SOLDIER_MAX_COMBAT-OFFER_SOLDIER_MIN_COMBAT+1);

	#undef OFFER_RANDOM

	return offerPtr;
}
//----------- End of function ModFryhtan::think_offer -----------//


//--------- Begin of function ModFryhtan::del_offer ---------//
//
void ModFryhtan::del_offer(int firmRecno)
{
	if( firmRecno >= 1 && firmRecno <= offer_array_size )
		memset( offer_array+firmRecno-1, 0, sizeof(LairOffer) );
}
//----------- End of function ModFryhtan::del_offer -----------//


//--------- Begin of function ModFryhtan::offer_cost ---------//
//
int ModFryhtan::offer_cost(LairOffer* offerPtr)
{
	int totalCombat = offerPtr->ordo_combat;

	for( int i=0 ; i<offerPtr->soldier_count ; i++ )
		totalCombat += offerPtr->soldier_combat[i];

	return totalCombat * OFFER_COST_PER_COMBAT;
}
//----------- End of function ModFryhtan::offer_cost -----------//


//--------- Begin of function ModFryhtan::pay_lair_tribute ---------//
//
// Pay the offer of a wild lair: the wild lair is replaced by a lair of the
// kingdom of the same species, with the Ordo as its general and the soldiers
// inside. The wild Fryhtans in the lair disappear; the ones outside stay
// wild. The kingdom can go over its population cap this way.
//
// return: <int> the recno of the new lair, 0 if it couldn't be paid
//
int ModFryhtan::pay_lair_tribute(int firmRecno, int nationRecno)
{
	if( firm_array.is_deleted(firmRecno) || firm_array[firmRecno]->firm_id != FIRM_MONSTER )
		return 0;

	FirmMonster* monsterFirm = (FirmMonster*) firm_array[firmRecno];
	LairOffer*	 offerPtr	 = get_offer(monsterFirm);

	if( !offerPtr || !is_linked_wild_lair(monsterFirm, nationRecno) )
		return 0;

	Nation* nationPtr = nation_array[nationRecno];
	int cost = capture_cost(monsterFirm, nationRecno, offerPtr);

	if( !cost || nationPtr->cash < cost )		// no Ordo or king in a linked lair, or not enough money
		return 0;

	LairOffer offer = *offerPtr;		// it is deleted with the wild lair
	int xLoc = monsterFirm->loc_x1, yLoc = monsterFirm->loc_y1;

	nationPtr->add_expense( EXPENSE_TRIBUTE, (float) cost, 1 );

	//---- the wild Fryhtans inside disappear (not mobilized), then delete the wild lair ----//

	memset( &monsterFirm->monster_king, 0, sizeof(monsterFirm->monster_king) );
	monsterFirm->monster_general_count = 0;
	monsterFirm->waiting_soldier_count = 0;

	capturing_lair = 1;		// no gold coins
	firm_array.del_firm(firmRecno);
	capturing_lair = 0;

	//------ build the lair of the kingdom in its place (3x3 in the 4x4 place) ------//

	if( !world.can_build_firm(xLoc, yLoc, FIRM_CAMP) )
		return 0;

	int lairRecno = firm_array.create_firm(FIRM_CAMP);

	firm_array[lairRecno]->init( xLoc, yLoc, nationRecno, FIRM_CAMP, race_res[nationPtr->race_id]->code );

	Firm* lairPtr = firm_array[lairRecno];

	lairPtr->complete_construction();
	set_lair( lairRecno, offer.monster_id );		// the species of the captured lair, not the kingdom's

	//------ the Ordo ------//

	int ordoRecno = add_lair_ordo( lairPtr, offer.monster_id );

	if( ordoRecno )
	{
		Unit* ordoPtr = unit_array[ordoRecno];

		ordoPtr->skill.skill_level = offer.ordo_leadership;
		ordoPtr->set_combat_level( offer.ordo_combat );
		ordoPtr->hit_points = (float) ordoPtr->max_hit_points;

		lairPtr->assign_overseer( ordoRecno );
	}

	//------ the soldiers ------//

	for( int i=0 ; i<offer.soldier_count ; i++ )
	{
		if( !add_lair_soldier( lairPtr, offer.monster_id, offer.soldier_combat[i] ) )
			break;
	}

	return lairRecno;
}
//----------- End of function ModFryhtan::pay_lair_tribute -----------//


//--------- Begin of function ModFryhtan::think_succeed_king ---------//
//
// 7kmod-fryhtan, phase 7: when the All High of a Fryhtan kingdom dies, the
// Ordo with the best combat (on the map or commanding a lair) becomes the new
// All High. Without Ordos, the Fryhtan with the best combat on the map, and
// without those, the one with the best combat in a lair (it comes out).
//
// The new All High keeps his Ordo name (or gets one of the list), and the
// kingdom is called by it ("<name> <species> Horde"). The name is kept as the
// player's name (NationArray::human_name_array), so all the places that show
// the name of the kingdom or of its king keep working.
//
// return: <int> 1 if there is a new All High
//
int ModFryhtan::think_succeed_king(int nationRecno)
{
	Nation* nationPtr = nation_array[nationRecno];

	if( nationPtr->king_unit_recno || !is_fryhtan(nationRecno) )
		return 0;

	//------ the best Ordo, or the best Fryhtan on the map ------//

	int bestUnitRecno=0, bestRank= -1, bestCombat= -1;
	int i;

	for( i=unit_array.size() ; i>0 ; i-- )
	{
		if( unit_array.is_deleted(i) )
			continue;

		Unit* unitPtr = unit_array[i];

		if( unitPtr->nation_recno != nationRecno || unit_res[unitPtr->unit_id]->unit_class != UNIT_CLASS_MONSTER ||
			 !unitPtr->is_visible() && unitPtr->unit_mode != UNIT_MODE_OVERSEE )
		{
			continue;
		}

		if( unitPtr->hit_points <= 0 || unitPtr->cur_action == SPRITE_DIE )
			continue;

		int rank = unitPtr->rank_id==RANK_GENERAL ? 1 : 0;
		int combat = unitPtr->skill.combat_level;

		if( rank > bestRank || (rank==bestRank && combat > bestCombat) )
		{
			bestRank = rank;
			bestCombat = combat;
			bestUnitRecno = i;
		}
	}

	//------ none on the map: the best soldier of a lair comes out ------//

	if( !bestUnitRecno )
	{
		Firm* bestFirm = NULL;
		int bestWorkerId = 0;

		for( i=firm_array.size() ; i>0 ; i-- )
		{
			if( firm_array.is_deleted(i) )
				continue;

			Firm* firmPtr = firm_array[i];

			if( firmPtr->nation_recno != nationRecno || !is_lair(firmPtr) )
				continue;

			for( int j=0 ; j<firmPtr->worker_count ; j++ )
			{
				if( firmPtr->worker_array[j].combat_level > bestCombat )
				{
					bestCombat = firmPtr->worker_array[j].combat_level;
					bestFirm = firmPtr;
					bestWorkerId = j+1;
				}
			}
		}

		if( bestFirm )
			bestUnitRecno = bestFirm->mobilize_worker( bestWorkerId, COMMAND_AUTO );
	}

	if( !bestUnitRecno || unit_array.is_deleted(bestUnitRecno) )
		return 0;

	//------ his name ------//

	Unit* kingUnit = unit_array[bestUnitRecno];

	if( !ordo_name_by_id(kingUnit->name_id) )
		kingUnit->name_id = (uint16_t) new_ordo_name_id(nationRecno);

	char nameStr[HUMAN_NAME_LEN+1];

	strncpy( nameStr, ordo_name_by_id(kingUnit->name_id), HUMAN_NAME_LEN );
	nameStr[HUMAN_NAME_LEN] = '\0';

	nation_array.set_human_name( nationRecno, nameStr );

	//------ the succession (news, set_king) ------//

	nationPtr->succeed_king( bestUnitRecno );
	return 1;
}
//----------- End of function ModFryhtan::think_succeed_king -----------//


//--------- Begin of function ModFryhtan::has_lair_or_unit ---------//
//
// 7kmod-fryhtan, phase 7: a Fryhtan kingdom is defeated when it has no lairs
// and no units left.
//
int ModFryhtan::has_lair_or_unit(int nationRecno)
{
	int i;

	for( i=firm_array.size() ; i>0 ; i-- )
	{
		if( !firm_array.is_deleted(i) && firm_array[i]->nation_recno == nationRecno && is_lair(firm_array[i]) )
			return 1;
	}

	for( i=unit_array.size() ; i>0 ; i-- )
	{
		if( unit_array.is_deleted(i) )
			continue;

		Unit* unitPtr = unit_array[i];

		if( unitPtr->nation_recno == nationRecno && unit_res[unitPtr->unit_id]->unit_class == UNIT_CLASS_MONSTER &&
			 unitPtr->hit_points > 0 )
		{
			return 1;
		}
	}

	return 0;
}
//----------- End of function ModFryhtan::has_lair_or_unit -----------//
