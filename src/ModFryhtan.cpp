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
#include <ModFryhtan.h>
#include "gettext.h"

ModFryhtan mod_fryhtan;

//--------- Begin of function ModFryhtan::reset ---------//
//
void ModFryhtan::reset()
{
	memset( nation_monster_id, 0, sizeof(nation_monster_id) );
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

	SpriteInfo* spriteInfo = sprite_res[ unit_res[unitId]->sprite_id ];

	xLoc += STD_TOWN_LOC_WIDTH/2;
	yLoc += STD_TOWN_LOC_HEIGHT/2;

	if( !world.locate_space( &xLoc, &yLoc, xLoc, yLoc, spriteInfo->loc_width, spriteInfo->loc_height,
									 unit_res[unitId]->mobile_type ) )
	{
		return 0;
	}

	//---------- create the king ----------//

	int unitRecno = unit_array.add_unit( unitId, nationRecno, RANK_KING, 100, xLoc, yLoc );

	if( !unitRecno )
		return 0;

	Unit* unitPtr = unit_array[unitRecno];

	unitPtr->set_monster_id(monsterId);
	unitPtr->skill.set_skill(SKILL_LEADING);
	unitPtr->skill.skill_level = FRYHTAN_KING_LEADERSHIP;
	unitPtr->set_combat_level(FRYHTAN_KING_COMBAT);
	unitPtr->hit_points = (float) unitPtr->max_hit_points;

	nation_array[nationRecno]->set_king(unitRecno, 1);		// 1-the first king of the nation

	return unitRecno;
}
//----------- End of function ModFryhtan::create_start_king -----------//


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
	return filePtr->file_put_short_array( nation_monster_id, MAX_NATION );
}
//----------- End of function ModFryhtan::write_file -----------//


//--------- Begin of function ModFryhtan::read_file ---------//
//
int ModFryhtan::read_file(File* filePtr)
{
	return filePtr->file_get_short_array( nation_monster_id, MAX_NATION );
}
//----------- End of function ModFryhtan::read_file -----------//
