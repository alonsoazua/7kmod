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
//Description : 7kmod-fryhtan: Fryhtan kingdoms. See 7kmod/TODO-fryhtan.md.

#include <string.h>
#include <ALL.h>
#include <OFILE.h>
#include <OMONSRES.h>
#include <OUNITRES.h>
#include <OUNIT.h>
#include <OSPRTRES.h>
#include <OSKILL.h>
#include <OTOWN.h>
#include <OF_MONS.h>
#include <OWORLD.h>
#include <ONATION.h>
#include <OFIRM.h>
#include <OFIRMA.h>
#include <OFIRMID.h>
#include <OFIRMRES.h>
#include <OGAME.h>
#include <OIMGRES.h>
#include <OWORLDMT.h>
#include <OSERES.h>
#include <ORACERES.h>
#include <OINFO.h>
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
		nation_monster_id[nationRecno-1] = 0;
}
//----------- End of function ModFryhtan::init_nation -----------//


//--------- Begin of function ModFryhtan::set_nation_monster ---------//
//
void ModFryhtan::set_nation_monster(int nationRecno, int monsterId)
{
	if( nationRecno >= 1 && nationRecno <= MAX_NATION )
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
// A Fryhtan kingdom starts with its king (an All High of its species) only,
// at a place chosen like the starting town of a human kingdom.
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

	//------ build the starting lair there (a finished Fort, free of charge) ------//

	if( !world.can_build_firm(xLoc, yLoc, FIRM_CAMP) )
		return 0;

	Nation* nationPtr = nation_array[nationRecno];

	int firmRecno = firm_array.create_firm(FIRM_CAMP);

	firm_array[firmRecno]->init( xLoc, yLoc, nationRecno, FIRM_CAMP, race_res[nationPtr->race_id]->code );		// Firm::init() marks it as a lair of the kingdom's species

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

	if( nation_array[firmPtr->nation_recno]->food < train_food(trainType) )
		return 0;

	if( nation_population(firmPtr->nation_recno) + lair_train_population(firmRecno) >
		 nation_population_cap(firmPtr->nation_recno) )
	{
		return 0;
	}

	return 1;
}
//----------- End of function ModFryhtan::can_train_in_lair -----------//


//--------- Begin of function ModFryhtan::add_lair_train ---------//
//
// Queue one Fryhtan for training in the lair. The food is paid now.
//
int ModFryhtan::add_lair_train(int firmRecno, int trainType)
{
	if( !can_train_in_lair(firmRecno, trainType) )
		return 0;

	FryhtanLair* lairPtr = get_lair(firmRecno);
	Firm* firmPtr = firm_array[firmRecno];

	nation_array[firmPtr->nation_recno]->consume_food( (float) train_food(trainType) );

	if( trainType == LAIR_TRAIN_ORDO )
		lairPtr->ordo_mask |= (1 << lairPtr->train_count);

	lairPtr->train_count++;
	return 1;
}
//----------- End of function ModFryhtan::add_lair_train -----------//


//--------- Begin of function ModFryhtan::cancel_lair_train ---------//
//
// Remove the last Fryhtan of the given type (soldier or Ordo) from the
// training queue, returning its food.
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

	if( !lairPtr || lairPtr->train_count <= 0 )
		return;

	Firm* firmPtr = firm_array[firmRecno];

	if( firmPtr->under_construction )
		return;

	int trainType = lairPtr->train_type(0);

	if( ++lairPtr->train_days < train_days(trainType) )
		return;

	if( trainType == LAIR_TRAIN_ORDO )
	{
		if( !add_lair_ordo(firmPtr, lairPtr->monster_id) )
			return;		// no space next to the lair, try again the next day
	}
	else
	{
		if( !add_lair_soldier(firmPtr, lairPtr->monster_id) )
			return;
	}

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

	//------ it goes into the lair if there is room ------//

	if( firmPtr->worker_count < MAX_WORKER )
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
int ModFryhtan::add_lair_ordo(Firm* firmPtr, int monsterId)
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
	unitPtr->set_combat_level( LAIR_ORDO_COMBAT );
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

	int nameIndex = (int) unitPtr->name_id - ORDO_NAME_ID_BASE;

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
	return monster_res[monsterId]->level >= FRYHTAN_LARGE_MIN_LEVEL;
}
//----------- End of function ModFryhtan::is_large_monster -----------//


//--------- Begin of function ModFryhtan::monster_population ---------//
//
int ModFryhtan::monster_population(int monsterId)
{
	return is_large_monster(monsterId) ? 2 : 1;
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
	return is_large_monster(monsterId) ? _("Large: 2 population") : _("Small: 1 population");
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

	return menu_species_level_array[monsterId-1] >= FRYHTAN_LARGE_MIN_LEVEL ?
			 _("Large: 2 population") : _("Small: 1 population");
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

	return 1;
}
//----------- End of function ModFryhtan::read_file -----------//


//--------- Begin of function ModFryhtan::reward_kill ---------//
//
// 7kmod-fryhtan: a Fryhtan kingdom gets gold and food for each human unit
// it kills: a soldier or a villager, a general or a king.
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

	int gold = KILL_GOLD_UNIT, food = KILL_FOOD_UNIT;

	if( targetUnit->rank_id == RANK_KING )
	{
		gold = KILL_GOLD_KING;
		food = KILL_FOOD_KING;
	}
	else if( targetUnit->rank_id == RANK_GENERAL )
	{
		gold = KILL_GOLD_GENERAL;
		food = KILL_FOOD_GENERAL;
	}

	Nation* nationPtr = nation_array[nationRecno];

	nationPtr->add_income( INCOME_TREASURE, (float) gold );
	nationPtr->add_food( (float) food );
}
//----------- End of function ModFryhtan::reward_kill -----------//


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

	//---- if it has lost the link with all the lairs, it becomes independent ----//

	if( !townPtr->has_linked_camp(nationRecno, 0) )
	{
		for( int i=0 ; i<MAX_RACE ; i++ )		// the kingdom has to fight it again to enslave it
			townPtr->race_resistance_array[i][nationRecno-1] = (float) 100;

		townPtr->set_nation(0);
		return 0;
	}

	//------- the tribute, at the start of each month (only with an Ordo or the king in a linked lair) -------//

	if( info.game_day == 1 && has_lair_commander(townPtr) )
		nation_array[nationRecno]->add_income( INCOME_TAX, (float) town_tribute(townPtr) );

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
