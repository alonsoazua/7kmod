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
int ModFryhtan::can_train_in_lair(int firmRecno, int showMsg)
{
	FryhtanLair* lairPtr = get_lair(firmRecno);

	if( !lairPtr || firm_array.is_deleted(firmRecno) )
		return 0;

	Firm* firmPtr = firm_array[firmRecno];

	if( firmPtr->under_construction || !firmPtr->nation_recno )
		return 0;

	if( lairPtr->train_count >= LAIR_MAX_TRAIN_QUEUE )
		return 0;

	if( nation_array[firmPtr->nation_recno]->food < LAIR_TRAIN_FOOD )
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
int ModFryhtan::add_lair_train(int firmRecno)
{
	if( !can_train_in_lair(firmRecno) )
		return 0;

	FryhtanLair* lairPtr = get_lair(firmRecno);
	Firm* firmPtr = firm_array[firmRecno];

	nation_array[firmPtr->nation_recno]->consume_food( (float) LAIR_TRAIN_FOOD );

	lairPtr->train_count++;
	return 1;
}
//----------- End of function ModFryhtan::add_lair_train -----------//


//--------- Begin of function ModFryhtan::cancel_lair_train ---------//
//
// Remove the last Fryhtan from the training queue, returning its food.
//
int ModFryhtan::cancel_lair_train(int firmRecno)
{
	FryhtanLair* lairPtr = get_lair(firmRecno);

	if( !lairPtr || lairPtr->train_count <= 0 )
		return 0;

	Firm* firmPtr = firm_array[firmRecno];

	nation_array[firmPtr->nation_recno]->add_food( (float) LAIR_TRAIN_FOOD );

	if( --lairPtr->train_count == 0 )
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

	if( ++lairPtr->train_days < LAIR_TRAIN_DAYS )
		return;

	if( !add_lair_soldier(firmPtr, lairPtr->monster_id) )
		return;		// no space next to the lair, try again the next day

	lairPtr->train_days = 0;
	lairPtr->train_count--;
}
//----------- End of function ModFryhtan::next_day_lair -----------//


//--------- Begin of function ModFryhtan::add_lair_soldier ---------//
//
// Add a Fryhtan soldier (combat LAIR_TRAIN_COMBAT) of the given species next
// to a lair. It goes into the lair if there is room.
//
// return: <int> the unit recno, 0 if there is no space for it
//
int ModFryhtan::add_lair_soldier(Firm* firmPtr, int monsterId)
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
	unitPtr->set_combat_level( LAIR_TRAIN_COMBAT );
	unitPtr->hit_points = (float) unitPtr->max_hit_points;

	//------ it goes into the lair if there is room ------//

	if( firmPtr->worker_count < MAX_WORKER )
		firmPtr->assign_unit(unitRecno);

	return unitRecno;
}
//----------- End of function ModFryhtan::add_lair_soldier -----------//


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

	if( !filePtr->file_put_short( (int16_t) lairCount ) )
		return 0;

	if( lairCount > 0 &&
		 !filePtr->file_put_short_array( (int16_t*) lair_array, lairCount * (sizeof(FryhtanLair)/sizeof(int16_t)) ) )
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

	if( lairCount < 0 )
		return 0;

	if( lairCount > 0 )
	{
		set_lair( lairCount, 1 );		// allocate the array

		if( !filePtr->file_get_short_array( (int16_t*) lair_array, lairCount * (sizeof(FryhtanLair)/sizeof(int16_t)) ) )
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

	int reward = KILL_REWARD_UNIT;

	if( targetUnit->rank_id == RANK_KING )
		reward = KILL_REWARD_KING;

	else if( targetUnit->rank_id == RANK_GENERAL )
		reward = KILL_REWARD_GENERAL;

	Nation* nationPtr = nation_array[nationRecno];

	nationPtr->add_income( INCOME_TREASURE, (float) reward );
	nationPtr->add_food( (float) reward );
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

	//------- the tribute, at the start of each month -------//

	if( info.game_day == 1 )
		nation_array[nationRecno]->add_income( INCOME_TAX, (float) town_tribute(townPtr) );

	return 1;
}
//----------- End of function ModFryhtan::next_day_town -----------//


//--------- Begin of function ModFryhtan::town_tribute ---------//
//
int ModFryhtan::town_tribute(Town* townPtr)
{
	return townPtr->population * TRIBUTE_PER_PERSON;
}
//----------- End of function ModFryhtan::town_tribute -----------//
