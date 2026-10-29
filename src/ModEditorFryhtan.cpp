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


//Filename    : ModEditorFryhtan.cpp
//Description : 7kmod-gameplay: the invoked Fryhtans in the scenario editor
//              (the "Fryhtan" entry of the Units tab). They are made as the
//              Seat of Power makes them (FirmBase::invoke_fryhtan()): a unit
//              of a kingdom with the species of a Fryhtan. Kept apart from
//              ModEditorPanel.cpp, which comes from 7kmod.

#include <ModEditor.h>
#include <OUNIT.h>
#include <OUNITRES.h>
#include <OMONSRES.h>
#include <OF_BASE.h>
#include "gettext.h"

//------- the Fryhtans that can be invoked, in the order of the races --------//

static short fryhtan_unit_array[] =
{
	UNIT_HEADLESS,			// Holgh (Normans)
	UNIT_SKELETON,			// Deezboanz (Mayans)
	UNIT_LIZARDMAN,		// Sauroid (Greeks)
	UNIT_GIANT_ETTIN,		// Haubudam (Vikings)
	UNIT_ROCKMAN,			// Rokken (Persians)
	UNIT_FIREKIN,			// Wyrm (Chinese)
	UNIT_LYW,				// Rattus (Japanese)
	UNIT_MAN,				// Karrotten (Egyptians)
	UNIT_GREMJERM,			// Doink (Mughuls)
	UNIT_HOBGLOBLIN,		// Broosken (Zulus)
};

enum { FRYHTAN_COUNT = sizeof(fryhtan_unit_array)/sizeof(fryhtan_unit_array[0]) };


//-------- Begin of function editor_next_fryhtan --------//
//
// <int> unitId - the current Fryhtan (0: the first one)
// <int> dir    - 1 next, -1 previous
//
// return: <int> the unit id of the next or previous Fryhtan
//
int editor_next_fryhtan(int unitId, int dir)
{
	int i;

	for( i=0 ; i<FRYHTAN_COUNT && fryhtan_unit_array[i] != unitId ; i++ );

	if( i == FRYHTAN_COUNT )
		return fryhtan_unit_array[0];

	return fryhtan_unit_array[ (i + dir + FRYHTAN_COUNT) % FRYHTAN_COUNT ];
}
//----------- End of function editor_next_fryhtan -----------//


//-------- Begin of function editor_fryhtan_name --------//
//
const char* editor_fryhtan_name(int unitId)
{
	MonsterInfo* monsterInfo = monster_res.get_monster_by_unit_id(unitId);

	return monsterInfo ? _(monsterInfo->name) : "";
}
//----------- End of function editor_fryhtan_name -----------//


//-------- Begin of function editor_init_fryhtan --------//
//
// A new unit of a Fryhtan becomes an invoked Fryhtan, as in
// FirmBase::invoke_fryhtan(), with the given combat level.
//
void editor_init_fryhtan(Unit* unitPtr, int combatLevel)
{
	MonsterInfo* monsterInfo = monster_res.get_monster_by_unit_id(unitPtr->unit_id);

	if( monsterInfo )
		unitPtr->set_monster_id( monsterInfo->monster_id );		// normally set by the Fryhtan lair

	unitPtr->set_combat_level( combatLevel );
	unitPtr->hit_points = (float) unitPtr->max_hit_points;
}
//----------- End of function editor_init_fryhtan -----------//


//-------- Begin of function editor_is_invoked_fryhtan --------//
//
int editor_is_invoked_fryhtan(Unit* unitPtr)
{
	return unit_res[unitPtr->unit_id]->unit_class == UNIT_CLASS_MONSTER && unitPtr->nation_recno;
}
//----------- End of function editor_is_invoked_fryhtan -----------//
