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

//Filename    : ModLimits.cpp
//Description : 7k-mod-IA: population limits of the kingdoms (see ModLimits.h)

#include <ModLimits.h>
#include <ConfigAdv.h>
#include <ONATION.h>
#include <OUNIT.h>
#include <OUNITRES.h>
#include <OFIRM.h>
#include <OF_WAR.h>
#include <OF_HARB.h>
#include <OSKILL.h>
#include <OBOX.h>
#include <OHELP.h>
#include <OBUTT3D.h>
#include <OSTR.h>
#include "gettext.h"


//--------- Begin of function mod_is_military_unit_id ---------//
//
// Units counted as military by their type: siege weapons, warships and
// Fryhtans. People are counted by their skill (leadership).
//
int mod_is_military_unit_id(int unitId)
{
	if( unitId < 1 || unitId > unit_res.unit_info_count )
		return 0;

	switch( unit_res[unitId]->unit_class )
	{
		case UNIT_CLASS_WEAPON:
		case UNIT_CLASS_MONSTER:
			return 1;
	}

	return unitId == UNIT_CARAVEL || unitId == UNIT_GALLEON;
}
//----------- End of function mod_is_military_unit_id -----------//


//--------- Begin of function mod_military_unit_count ---------//
//
// The military units of a kingdom: on the map, in buildings (the soldiers
// of the Forts, the generals and the ships in harbors are units too) and
// being trained or built.
//
int mod_military_unit_count(int nationRecno)
{
	int count=0;

	//------ units (also the ones being trained in towns) ------//

	for( int i=unit_array.size() ; i>0 ; i-- )
	{
		if( unit_array.is_deleted(i) )
			continue;

		Unit* unitPtr = unit_array[i];

		if( unitPtr->nation_recno != nationRecno || unitPtr->spy_recno )
			continue;

		if( unitPtr->race_id )
		{
			if( unitPtr->skill.skill_id == SKILL_LEADING && unitPtr->rank_id != RANK_KING )
				count++;
		}
		else if( mod_is_military_unit_id(unitPtr->unit_id) )
			count++;
	}

	//------ the soldiers, weapons and Fryhtans in the Forts, and the units being built ------//

	for( int i=firm_array.size() ; i>0 ; i-- )
	{
		if( firm_array.is_deleted(i) )
			continue;

		Firm* firmPtr = firm_array[i];

		if( firmPtr->nation_recno != nationRecno )
			continue;

		if( firmPtr->firm_id == FIRM_CAMP )
		{
			for( int j=0 ; j<firmPtr->worker_count ; j++ )
			{
				Worker* workerPtr = firmPtr->worker_array+j;

				if( workerPtr->spy_recno )
					continue;

				if( workerPtr->race_id ? workerPtr->skill_id == SKILL_LEADING :
					 mod_is_military_unit_id(workerPtr->unit_id) )
				{
					count++;
				}
			}
		}
		else if( firmPtr->firm_id == FIRM_WAR_FACTORY )
		{
			if( ((FirmWar*)firmPtr)->build_unit_id )
				count++;
		}
		else if( firmPtr->firm_id == FIRM_HARBOR )
		{
			if( mod_is_military_unit_id( ((FirmHarbor*)firmPtr)->build_unit_id ) )
				count++;
		}
	}

	return count;
}
//----------- End of function mod_military_unit_count -----------//


//--------- Begin of function mod_military_limit_reached ---------//
//
int mod_military_limit_reached(int nationRecno)
{
	if( !nationRecno || config_adv.nation_max_military_units <= 0 )
		return 0;

	return mod_military_unit_count(nationRecno) >= config_adv.nation_max_military_units;
}
//----------- End of function mod_military_limit_reached -----------//


//--------- Begin of function mod_town_population_limit_reached ---------//
//
int mod_town_population_limit_reached(int nationRecno)
{
	if( !nationRecno || nation_array.is_deleted(nationRecno) ||
		 config_adv.nation_max_town_population <= 0 )
	{
		return 0;
	}

	return nation_array[nationRecno]->total_population >= config_adv.nation_max_town_population;
}
//----------- End of function mod_town_population_limit_reached -----------//


//--------- Begin of function mod_check_military_limit ---------//
//
// For the orders of training or building military units: whether the
// kingdom is at its limit, telling the player.
//
// return: <int> 1 - at the limit, the order can't be done
//
int mod_check_military_limit(int nationRecno)
{
	if( !mod_military_limit_reached(nationRecno) )
		return 0;

	if( nationRecno == nation_array.player_recno )
	{
		String str;
		snprintf( str, MAX_STR_LEN+1, _("Your kingdom has reached its limit of %d military units."),
					 config_adv.nation_max_military_units );
		box.msg( str );
	}

	return 1;
}
//----------- End of function mod_check_military_limit -----------//


//--------- Begin of function mod_military_limit_help ---------//
//
// The help of a button disabled by the military limit, telling why. Call it
// before Button3D::detect(), which sets the help of the button.
//
// <char*> helpCode - the normal help code of the button, "" if it has a
//                    custom help (it is kept then)
//
void mod_military_limit_help(int nationRecno, Button3D& button, const char* helpCode, const char* helpTitle)
{
	if( !button.init_flag )
		return;

	if( !mod_military_limit_reached(nationRecno) )
	{
		if( helpCode[0] )
			button.set_help_code(helpCode);
		return;
	}

	button.set_help_code("");		// the custom help below is shown instead

	static String str;		// the help keeps the pointer
	snprintf( str, MAX_STR_LEN+1, _("Your kingdom has reached its limit of %d military units."),
				 config_adv.nation_max_military_units );

	help.set_custom_help( button.x1, button.y1, button.x2, button.y2, helpTitle, str );
}
//----------- End of function mod_military_limit_help -----------//
