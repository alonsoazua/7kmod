/*
 * Seven Kingdoms: Ancient Adversaries
 *
 * Copyright 1997,1998 Enlight Software Ltd.
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

//Filename    : OF_BASE2.CPP
//Description : Firm Base - part 2, the AI functions

#include <ONATION.h>
#include <OTOWN.h>
#include <OF_BASE.h>
#include <OF_CAMP.h>
#include <OUNIT.h>
#include <OUNITRES.h>

//--------- Begin of function FirmBase::process_ai ---------//

void FirmBase::process_ai()
{
	think_assign_unit();

	think_invoke_god();

	think_invoke_fryhtan();		// 7kmod-gameplay
}
//----------- End of function FirmBase::process_ai -----------//


//--------- Begin of function FirmBase::think_assign_unit ---------//

void FirmBase::think_assign_unit()
{
	Nation* nationPtr = nation_array[nation_recno];

	//-------- assign overseer ---------//

	if( info.game_date%15==firm_recno%15 )		// do not call too often because when an AI action is queued, it will take a while to carry it out
	{
		if( !overseer_recno )
			nationPtr->add_action(loc_x1, loc_y1, -1, -1, ACTION_AI_ASSIGN_OVERSEER, FIRM_BASE);
	}

	//------- recruit workers ---------//

	if( info.game_date%15==firm_recno%15 )
	{
		if( worker_count < MAX_WORKER )
			ai_recruit_worker();
	}
}
//----------- End of function FirmBase::think_assign_unit -----------//


//--------- Begin of function FirmBase::think_invoke_god ---------//

void FirmBase::think_invoke_god()
{
	if( pray_points < MAX_PRAY_POINTS || !can_invoke() )
		return;

	invoke_god();
}
//----------- End of function FirmBase::think_invoke_god -----------//


//------- 7kmod-gameplay: the AI invokes Fryhtans ---------//
//
// The AI invokes the Fryhtan of its Seats of Power (invoke_fryhtan()) and
// handles them like siege weapons (Unit::think_fryhtan_action()): they go
// into its Forts and go out with their attacks. The rules:
//
// - the Greater Being goes first: only when it is on the map; before that
//   the pray points are kept for it, as in the original game.
// - at least AI_FRYHTAN_RESERVE_PRAY_POINTS are left after paying for the
//   Fryhtan, so the Greater Being stays alive and can use its power.
// - each Seat thinks about it every AI_FRYHTAN_INTERVAL_DAYS days, so it
//   invokes at most one in that time.
// - at most AI_FRYHTAN_MAX_PER_NATION Fryhtans per kingdom.
// - its food isn't going down (each Fryhtan eats 20 to 50 a year).
// - it is at war with a kingdom.
//
enum { AI_FRYHTAN_RESERVE_PRAY_POINTS = 100,
		 AI_FRYHTAN_INTERVAL_DAYS = 90,
		 AI_FRYHTAN_MAX_PER_NATION = 20 };

static int nation_fryhtan_count(int nationRecno);


//--------- Begin of function FirmBase::think_invoke_fryhtan ---------//
//
void FirmBase::think_invoke_fryhtan()
{
	if( info.game_date % AI_FRYHTAN_INTERVAL_DAYS != firm_recno % AI_FRYHTAN_INTERVAL_DAYS )
		return;

	//---- the Greater Being first, and keep the reserve of pray points ----//

	if( !god_unit_recno || unit_array.is_deleted(god_unit_recno) )
		return;

	if( !can_invoke_fryhtan() ||
		 pray_points - invoke_fryhtan_pray_points() < AI_FRYHTAN_RESERVE_PRAY_POINTS )
	{
		return;
	}

	//---- at war, with food, below the maximum ----//

	Nation* nationPtr = nation_array[nation_recno];

	if( nationPtr->food <= 0 || nationPtr->food_change_365days() < 0 )
		return;

	int atWar = 0;

	for( int i=nation_array.size() ; i>0 ; i-- )
	{
		if( i != nation_recno && !nation_array.is_deleted(i) &&
			 nationPtr->get_relation_status(i) == NATION_HOSTILE )
		{
			atWar = 1;
			break;
		}
	}

	if( !atWar )
		return;

	if( nation_fryhtan_count(nation_recno) >= AI_FRYHTAN_MAX_PER_NATION )
		return;

	invoke_fryhtan(COMMAND_AI);
}
//----------- End of function FirmBase::think_invoke_fryhtan -----------//


//--------- Begin of static function nation_fryhtan_count ---------//
//
// The Fryhtans of a kingdom: on the map and in its Forts.
//
static int nation_fryhtan_count(int nationRecno)
{
	int count=0;

	for( int i=unit_array.size() ; i>0 ; i-- )
	{
		if( unit_array.is_deleted(i) )
			continue;

		Unit* unitPtr = unit_array[i];

		if( unitPtr->nation_recno == nationRecno && unit_res[unitPtr->unit_id]->unit_class == UNIT_CLASS_MONSTER )
			count++;
	}

	for( int i=firm_array.size() ; i>0 ; i-- )
	{
		if( firm_array.is_deleted(i) )
			continue;

		Firm* firmPtr = firm_array[i];

		if( firmPtr->nation_recno != nationRecno || firmPtr->firm_id != FIRM_CAMP )
			continue;

		for( int j=0 ; j<firmPtr->worker_count ; j++ )
		{
			if( unit_res[firmPtr->worker_array[j].unit_id]->unit_class == UNIT_CLASS_MONSTER )
				count++;
		}
	}

	return count;
}
//----------- End of static function nation_fryhtan_count -----------//
