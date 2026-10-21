/*
 * Seven Kingdoms: Ancient Adversaries
 *
 * Copyright 2026 7kmod
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

//Filename    : OHONORS.CPP
//Description : 7kmod - honors of the generals (rewards for their
//              contribution) and loyalty breakdowns
//
// A general becomes unhappy when his contribution to the nation (one point
// for each hit to the enemy) is more than twice the money spent on his
// rewards. The loyalty penalty is (contribution - rewards*2)/2, 50 points
// at most (see Unit::update_loyalty()). Each reward costs REWARD_COST and
// covers REWARD_COST*2 points of contribution.

#include <ALL.h>
#include <OSTR.h>
#include <OMOUSE.h>
#include <KEY.h>
#include <OVGA.h>
#include <vga_util.h>
#include <OFONT.h>
#include <OHELP.h>
#include <OINFO.h>
#include <OSYS.h>
#include <OUNIT.h>
#include <OFIRM.h>
#include <OTOWN.h>
#include <ONATION.h>
#include <ORACERES.h>
#include <OREMOTE.h>
#include <ONEWS.h>
#include <OWORLDMT.h>
#include <ConfigAdv.h>
#include <OHONORS.h>
#include "gettext.h"

HonorsRequest honors_request;

//------- Begin of function honors_unrewarded -------//
//
// The contribution of the general that is not covered by his rewards.
//
int honors_unrewarded(Unit* unitPtr)
{
	if( unitPtr->rank_id != RANK_GENERAL || !unitPtr->race_id )
		return 0;

	int unrewarded = unitPtr->nation_contribution - unitPtr->total_reward*2;

	return unrewarded > 0 ? unrewarded : 0;
}
//-------- End of function honors_unrewarded --------//


//------- Begin of function honors_penalty -------//
//
// The loyalty penalty of the general for his unrewarded contribution, the
// same as in Unit::update_loyalty().
//
int honors_penalty(Unit* unitPtr)
{
	return MIN( MAX_HONORS_PENALTY, honors_unrewarded(unitPtr)/2 );
}
//-------- End of function honors_penalty --------//


//------- Begin of function honors_reward_count -------//
//
// The no. of rewards needed for removing the penalty of the general. Each
// reward covers REWARD_COST*2 points of contribution.
//
int honors_reward_count(Unit* unitPtr)
{
	if( honors_penalty(unitPtr) == 0 )
		return 0;

	int unrewarded = honors_unrewarded(unitPtr) - 1;		// the penalty is 0 with 1 point of unrewarded contribution
	int perReward  = REWARD_COST*2;

	return (unrewarded + perReward - 1) / perReward;
}
//-------- End of function honors_reward_count --------//


//------- Begin of function honors_cost -------//
//
int honors_cost(Unit* unitPtr)
{
	return honors_reward_count(unitPtr) * REWARD_COST;
}
//-------- End of function honors_cost --------//


//------- Begin of function is_honors_general -------//
//
// Whether the unit is a general of the player (the honors functions are
// only for them).
//
int is_honors_general(Unit* unitPtr)
{
	return unitPtr->rank_id == RANK_GENERAL && unitPtr->race_id &&
			 nation_array.player_recno && unitPtr->nation_recno == nation_array.player_recno &&
			 !unitPtr->spy_recno;
}
//-------- End of function is_honors_general --------//


//------- Begin of function pay_honors -------//
//
// Give the rewards needed for removing the penalty of the general, as many
// as the cash of the nation allows. It uses the normal rewards of the game
// (the general's reward button), so it works in multiplayer games.
//
// return: <int> the no. of rewards given
//
int pay_honors(Unit* unitPtr)
{
	if( !is_honors_general(unitPtr) )
		return 0;

	Nation* nationPtr   = nation_array[unitPtr->nation_recno];
	int     rewardCount = honors_reward_count(unitPtr);
	int     affordable  = (int) (nationPtr->cash / REWARD_COST);

	rewardCount = MIN( rewardCount, affordable );

	for( int i=0 ; i<rewardCount ; i++ )
	{
		if( unitPtr->unit_mode == UNIT_MODE_OVERSEE )
		{
			firm_array[unitPtr->unit_mode_para]->reward(0, COMMAND_PLAYER);		// 0-the commander of the firm
		}
		else if( !remote.is_enable() )
		{
			unitPtr->reward(unitPtr->nation_recno);
		}
		else
		{
			// packet structure : <unit no> + <rewarding nation recno>
			short *shortPtr = (short *)remote.new_send_queue_msg(MSG_UNIT_REWARD, sizeof(short)*2);
			shortPtr[0] = unitPtr->sprite_recno;
			shortPtr[1] = unitPtr->nation_recno;
		}
	}

	return rewardCount;		// Unit::reward() removes the request for honors when the penalty is cleared
}
//-------- End of function pay_honors --------//


//------- Begin of function check_honors_request -------//
//
// Called when the contribution of a unit increases or it is promoted to
// general. When a general of the player reaches the maximum penalty, he asks
// for honors.
//
// <int> justPromoted - 1 if the unit has just been promoted to general (a
//                      soldier may already have a lot of contribution)
//
void check_honors_request(Unit* unitPtr, int justPromoted)
{
	if( !is_honors_general(unitPtr) )
		return;

	if( justPromoted ? honors_penalty(unitPtr) >= MAX_HONORS_PENALTY :
		 honors_unrewarded(unitPtr) == MAX_HONORS_PENALTY*2 )		// the contribution increases one point at a time, so it reaches this value exactly
	{
		news_array.remove(NEWS_GENERAL_HONORS, unitPtr->sprite_recno);		// only one request per general
		news_array.general_honors(unitPtr->sprite_recno);
	}
}
//-------- End of function check_honors_request --------//


//------- Begin of function remove_honors_request -------//
//
// Remove the request for honors of the unit, and close its window if it is
// open.
//
void remove_honors_request(Unit* unitPtr)
{
	news_array.remove(NEWS_GENERAL_HONORS, unitPtr->sprite_recno);

	if( honors_request.is_active() && honors_request.unit_recno == unitPtr->sprite_recno )
		honors_request.close();
}
//-------- End of function remove_honors_request --------//


//------- Begin of static function add_line -------//
//
static void add_line(String& detail, const char* label, int value)
{
	if( detail.len() > 0 )
		detail += "\n";

	detail += label;
	detail += " ";

	if( value >= 0 )
		detail += "+";

	detail += value;
}
//-------- End of static function add_line --------//


//------- Begin of function unit_loyalty_breakdown -------//
//
// The components of the target loyalty of a general or a soldier on the map,
// calculated as in Unit::update_loyalty().
//
// return: <int> 1 - the breakdown is available, 0 - not available
//
int unit_loyalty_breakdown(Unit* unitPtr, String& title, String& detail)
{
	if( !unitPtr->nation_recno || unitPtr->rank_id == RANK_KING || !unitPtr->race_id )
		return 0;

	Nation* ownNation = nation_array[unitPtr->nation_recno];

	detail = "";

	if( unitPtr->rank_id == RANK_GENERAL )
	{
		//------ the power of the general ------//

		int villagePower=0, troopPower=0;

		if( unitPtr->unit_mode == UNIT_MODE_OVERSEE )
		{
			Firm* firmPtr = firm_array[unitPtr->unit_mode_para];

			if( firmPtr->firm_id == FIRM_CAMP )
			{
				for( int i=firmPtr->linked_town_count-1 ; i>=0 ; i-- )
				{
					if( firmPtr->linked_town_enable_array[i] == LINK_EE )
					{
						Town* townPtr = town_array[firmPtr->linked_town_array[i]];

						villagePower += townPtr->population / townPtr->linked_active_camp_count();
					}
				}

				troopPower = firmPtr->worker_count*3;
			}
			else if( firmPtr->firm_id == FIRM_BASE )
			{
				troopPower = 60;
			}
		}
		else if( unitPtr->team_info )
		{
			troopPower = unitPtr->team_info->member_count*3;
		}

		if( unitPtr->unit_mode == UNIT_MODE_OVERSEE && firm_array[unitPtr->unit_mode_para]->firm_id == FIRM_BASE )
			add_line( detail, _("Seat of Power"), troopPower );
		else
		{
			add_line( detail, _("Linked villages"), villagePower );
			add_line( detail, _("Troops"), troopPower );
		}

		add_line( detail, _("Same race as king"), ownNation->race_id == unitPtr->race_id ? 20 : 0 );
		add_line( detail, _("Reputation"), (int)ownNation->reputation/4 );
		add_line( detail, _("King's leadership"), ownNation->king_unit_recno ? unit_array[ownNation->king_unit_recno]->skill.skill_level/4 : 0 );

		if( honors_penalty(unitPtr) > 0 )
			add_line( detail, _("Unrewarded"), -honors_penalty(unitPtr) );
	}
	else if( unitPtr->rank_id == RANK_SOLDIER )
	{
		int hasLeader = config_adv.unit_loyalty_require_local_leader ?
			unitPtr->is_leader_in_range() : unitPtr->leader_unit_recno;

		if( !hasLeader )
		{
			detail = _("No general leads this unit.\nIts loyalty decreases little by little.");
		}
		else
		{
			Unit* leaderUnit = unit_array[unitPtr->leader_unit_recno];

			add_line( detail, _("Base"), 30 );
			add_line( detail, _("General's leadership"), leaderUnit->skill.get_skill(SKILL_LEADING) );
			add_line( detail, _("Combat"), -unitPtr->skill.combat_level/2 );

			if( unitPtr->skill.skill_level > 0 )
				add_line( detail, _("Own leadership"), -unitPtr->skill.skill_level );

			if( leaderUnit->rank_id == RANK_KING )
				add_line( detail, _("Led by the king"), 20 );

			add_line( detail, _("Same race as general"), race_res.is_same_race(unitPtr->race_id, leaderUnit->race_id) ? 20 : 0 );
		}
	}
	else
	{
		return 0;
	}

	title  = _("Target loyalty");
	title += ": ";
	title += unitPtr->target_loyalty;

	return 1;
}
//-------- End of function unit_loyalty_breakdown --------//


//------- Begin of function worker_loyalty_breakdown -------//
//
// The components of the target loyalty of a soldier in a fort, calculated as
// in Worker::target_loyalty().
//
int worker_loyalty_breakdown(Worker* workerPtr, int firmRecno, String& title, String& detail)
{
	if( workerPtr->town_recno || !workerPtr->race_id )
		return 0;

	Firm* firmPtr = firm_array[firmRecno];

	if( !firmPtr->overseer_recno )
		return 0;

	Unit* overseerUnit  = unit_array[firmPtr->overseer_recno];
	int   overseerSkill = overseerUnit->skill.get_skill(SKILL_LEADING);

	detail = "";

	add_line( detail, _("Base"), 30 );
	add_line( detail, _("General's leadership"), overseerSkill/2 );
	add_line( detail, _("Combat"), -workerPtr->combat_level/2 );

	if( workerPtr->skill_level > overseerSkill )
		add_line( detail, _("More leadership than general"), -(workerPtr->skill_level - overseerSkill) );

	if( overseerUnit->rank_id == RANK_KING )
		add_line( detail, _("Led by the king"), 20 );

	add_line( detail, _("Same race as general"), race_res.is_same_race(workerPtr->race_id, overseerUnit->race_id) ? 20 : 0 );

	title  = _("Target loyalty");
	title += ": ";
	title += workerPtr->target_loyalty(firmRecno);

	return 1;
}
//-------- End of function worker_loyalty_breakdown --------//


//------- Begin of function set_unit_loyalty_help -------//
//
// Set the help of a loyalty field to the loyalty breakdown of the unit.
// Only for the units of the player.
//
void set_unit_loyalty_help(Unit* unitPtr, int x1, int y1, int x2, int y2)
{
	if( !mouse.in_area(x1, y1, x2, y2) )
		return;

	if( !nation_array.player_recno || unitPtr->nation_recno != nation_array.player_recno )
		return;

	static String title, detail;

	if( unit_loyalty_breakdown(unitPtr, title, detail) )
		help.set_custom_help( x1, y1, x2, y2, title, detail );
}
//-------- End of function set_unit_loyalty_help --------//


//------- Begin of function set_worker_loyalty_help -------//
//
void set_worker_loyalty_help(Worker* workerPtr, int firmRecno, int x1, int y1, int x2, int y2)
{
	if( !mouse.in_area(x1, y1, x2, y2) )
		return;

	if( !nation_array.player_recno || firm_array[firmRecno]->nation_recno != nation_array.player_recno )
		return;

	static String title, detail;

	if( worker_loyalty_breakdown(workerPtr, firmRecno, title, detail) )
		help.set_custom_help( x1, y1, x2, y2, title, detail );
}
//-------- End of function set_worker_loyalty_help --------//


//---------- Define the window of the request ----------//

enum { HONORS_WIN_WIDTH  = 330,
		 HONORS_WIN_HEIGHT = 152,
		 HONORS_WIN_X1 = ZOOM_X1 + (ZOOM_X2-ZOOM_X1+1-HONORS_WIN_WIDTH)/2,
		 HONORS_WIN_Y1 = ZOOM_Y1 + (ZOOM_Y2-ZOOM_Y1+1-HONORS_WIN_HEIGHT)/2,
		 HONORS_WIN_X2 = HONORS_WIN_X1 + HONORS_WIN_WIDTH - 1,
		 HONORS_WIN_Y2 = HONORS_WIN_Y1 + HONORS_WIN_HEIGHT - 1 };

enum { HONORS_BUTTON_WIDTH  = 100,
		 HONORS_BUTTON_HEIGHT = 22,
		 HONORS_BUTTON_Y1 = HONORS_WIN_Y2 - HONORS_BUTTON_HEIGHT - 8,
		 HONORS_ACCEPT_X1 = HONORS_WIN_X1 + 45,
		 HONORS_REJECT_X1 = HONORS_WIN_X2 - 45 - HONORS_BUTTON_WIDTH + 1 };


//------- Begin of function HonorsRequest::open -------//
//
void HonorsRequest::open(int unitRecno, int unitNameId)
{
	unit_recno   = unitRecno;
	unit_name_id = unitNameId;
	active_flag  = 1;

	if( !is_valid() )		// the general is no longer valid, remove his request
	{
		active_flag = 0;
		news_array.remove(NEWS_GENERAL_HONORS, unitRecno);
	}
}
//-------- End of function HonorsRequest::open --------//


//------- Begin of function HonorsRequest::is_valid -------//
//
// Whether the general who made the request is still a general of the player.
//
int HonorsRequest::is_valid()
{
	if( unit_array.is_deleted(unit_recno) )
		return 0;

	Unit* unitPtr = unit_array[unit_recno];

	return is_honors_general(unitPtr) && unitPtr->name_id == unit_name_id;
}
//-------- End of function HonorsRequest::is_valid --------//


//------- Begin of function HonorsRequest::disp -------//
//
// Display the window on the active buffer (the back buffer).
//
void HonorsRequest::disp()
{
	if( !active_flag )
		return;

	if( !is_valid() )
	{
		close();
		return;
	}

	Unit* unitPtr = unit_array[unit_recno];
	VgaBuf* vgaBuf = Vga::active_buf;

	vgaBuf->d3_panel_up( HONORS_WIN_X1, HONORS_WIN_Y1, HONORS_WIN_X2, HONORS_WIN_Y2 );
	vgaBuf->d3_panel_down( HONORS_WIN_X1+6, HONORS_WIN_Y1+6, HONORS_WIN_X2-6, HONORS_WIN_Y1+28 );

	font_san.center_put( HONORS_WIN_X1+6, HONORS_WIN_Y1+6, HONORS_WIN_X2-6, HONORS_WIN_Y1+28, _("Request for Honors") );

	//------- the request -------//

	int x=HONORS_WIN_X1+14, y=HONORS_WIN_Y1+36;
	String str;

	// TRANSLATORS: General <Name> asks to be honored for his service to the kingdom.
	snprintf( str, MAX_STR_LEN+1, _("General %s asks to be honored for his service to the kingdom."), unitPtr->unit_name(0) );

	font_san.put_paragraph( x, y, HONORS_WIN_X2-14, y+30, str );

	y += 34;

	str  = _("Loyalty");
	str += ": ";
	str += unitPtr->loyalty;
	str += "      ";
	str += _("Penalty");
	str += ": -";
	str += honors_penalty(unitPtr);

	font_san.put( x, y, str );

	y += 16;

	int cost = honors_cost(unitPtr);

	str  = _("Honors");
	str += ": $";
	str += cost;

	int canPay = nation_array[unitPtr->nation_recno]->cash >= cost;

	if( !canPay )
	{
		str += "   (";
		str += _("Not enough money");
		str += ")";
	}

	font_san.put( x, y, str );

	//------- the buttons -------//

	vgaBuf->d3_panel_up( HONORS_ACCEPT_X1, HONORS_BUTTON_Y1, HONORS_ACCEPT_X1+HONORS_BUTTON_WIDTH-1, HONORS_BUTTON_Y1+HONORS_BUTTON_HEIGHT-1 );
	font_san.center_put( HONORS_ACCEPT_X1, HONORS_BUTTON_Y1, HONORS_ACCEPT_X1+HONORS_BUTTON_WIDTH-1, HONORS_BUTTON_Y1+HONORS_BUTTON_HEIGHT-1, _("Accept") );

	if( !canPay )
	{
		vgaBuf->adjust_brightness( HONORS_ACCEPT_X1, HONORS_BUTTON_Y1, HONORS_ACCEPT_X1+HONORS_BUTTON_WIDTH-1,
			HONORS_BUTTON_Y1+HONORS_BUTTON_HEIGHT-1, -6 );
	}

	vgaBuf->d3_panel_up( HONORS_REJECT_X1, HONORS_BUTTON_Y1, HONORS_REJECT_X1+HONORS_BUTTON_WIDTH-1, HONORS_BUTTON_Y1+HONORS_BUTTON_HEIGHT-1 );
	font_san.center_put( HONORS_REJECT_X1, HONORS_BUTTON_Y1, HONORS_REJECT_X1+HONORS_BUTTON_WIDTH-1, HONORS_BUTTON_Y1+HONORS_BUTTON_HEIGHT-1, _("Reject") );
}
//-------- End of function HonorsRequest::disp --------//


//------- Begin of function HonorsRequest::detect -------//
//
// return: <int> 1 - the mouse or keyboard event is used by the window
//
int HonorsRequest::detect()
{
	if( !active_flag )
		return 0;

	if( !is_valid() )
	{
		close();
		return 0;
	}

	Unit* unitPtr = unit_array[unit_recno];

	//-------- Accept --------//

	if( mouse.single_click( HONORS_ACCEPT_X1, HONORS_BUTTON_Y1, HONORS_ACCEPT_X1+HONORS_BUTTON_WIDTH-1,
		 HONORS_BUTTON_Y1+HONORS_BUTTON_HEIGHT-1 ) )
	{
		if( nation_array[unitPtr->nation_recno]->cash >= honors_cost(unitPtr) )
		{
			pay_honors(unitPtr);
			remove_honors_request(unitPtr);		// also closes the window
		}
		return 1;
	}

	//-------- Reject or Esc --------//

	if( mouse.single_click( HONORS_REJECT_X1, HONORS_BUTTON_Y1, HONORS_REJECT_X1+HONORS_BUTTON_WIDTH-1,
		 HONORS_BUTTON_Y1+HONORS_BUTTON_HEIGHT-1 ) ||
		 (mouse.is_key_event() && mouse.key_code == KEY_ESC) )
	{
		close();
		return 1;
	}

	//---- clicks on the window don't go to the map below ----//

	if( mouse.any_click( HONORS_WIN_X1, HONORS_WIN_Y1, HONORS_WIN_X2, HONORS_WIN_Y2, 0 ) ||
		 mouse.any_click( HONORS_WIN_X1, HONORS_WIN_Y1, HONORS_WIN_X2, HONORS_WIN_Y2, 1 ) )
	{
		return 1;
	}

	return 0;
}
//-------- End of function HonorsRequest::detect --------//
