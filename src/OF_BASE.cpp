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

//Filename    : OF_BASE.CPP
//Description : Firm Base

#include <OINFO.h>
#include <OVGA.h>
#include <vga_util.h>
#include <ODATE.h>
#include <OMOUSE.h>
#include <OIMGRES.h>
#include <OHELP.h>
#include <OSTR.h>
#include <OFONT.h>
#include <OCONFIG.h>
#include <OUNIT.h>
#include <OSPY.h>
#include <OGAME.h>
#include <OBUTT3D.h>
#include <ONATION.h>
#include <ORACERES.h>
#include <OWALLRES.h>
#include <OGODRES.h>
#include <OPOWER.h>
#include <OTOWN.h>
#include <OWORLD.h>
#include <OF_BASE.h>
#include <OHONORS.h>
#include <OREMOTE.h>
#include <OSERES.h>
#include <OSE.h>
#include <OUNITRES.h>
#include <OSPRTRES.h>
#include <OMONSRES.h>
#include "gettext.h"
#include <ModLimits.h>

//----------- Define static vars -------------//

static Button3D   button_invoke, button_reward;
static Button3D   button_invoke_fryhtan;		// 7kmod-gameplay
static Button3D	button_vacate_firm;

//--------- Begin of function FirmBase::FirmBase ---------//
//
FirmBase::FirmBase()
{
	firm_skill_id = SKILL_PRAYING;
}
//----------- End of function FirmBase::FirmBase -----------//


//--------- Begin of function FirmBase::~FirmBase ---------//
//
FirmBase::~FirmBase()
{
	err_when( race_id<1 || race_id>MAX_RACE );

	nation_array[nation_recno]->base_count_array[race_id-1]--;

	err_when( nation_array[nation_recno]->base_count_array[race_id-1] < 0 );
}
//----------- End of function FirmBase::~FirmBase -----------//


//--------- Begin of function FirmBase::init_derived ---------//
//
void FirmBase::init_derived()
{
	pray_points = (float) 0;

	//------ increase NationBase::base_count_array[] -----//

	err_when( race_id<1 || race_id>MAX_RACE );

	nation_array[nation_recno]->base_count_array[race_id-1]++;

	//---------- set the god id. ----------//

	god_id = 0 ;
	god_unit_recno = 0;

	for( int i=1 ; i<=god_res.god_count ; i++ )
	{
		if( god_res[i]->race_id == race_id &&
			 god_res[i]->is_nation_know(nation_recno) )
		{
			god_id = i;
			break;
		}
	}

	err_when( god_id==0 );
}
//----------- End of function FirmBase::init_derived -----------//


//--------- Begin of function FirmBase::assign_unit ---------//
//
void FirmBase::assign_unit(int unitRecno)
{
	Unit* unitPtr = unit_array[unitRecno];

	//### begin alex 18/9 ###//
	/*//------ only assign units of the right race ------//

	if( unitPtr->race_id != race_id )
		return;*/
	//#### end alex 18/9 ####//

	//------- if this is a construction worker -------//

	if( unitPtr->skill.skill_id == SKILL_CONSTRUCTION )
	{
		set_builder(unitRecno);
		return;
	}

	//### begin alex 18/9 ###//
	//------ only assign units of the right race ------//

	if( unitPtr->race_id != race_id )
		return;
	//#### end alex 18/9 ####//

	//-------- assign the unit ----------//

	int rankId = unit_array[unitRecno]->rank_id;

	if( rankId == RANK_GENERAL || rankId==RANK_KING )
	{
		assign_overseer(unitRecno);
	}
	else
	{
		assign_worker(unitRecno);
	}
}
//----------- End of function FirmBase::assign_unit -----------//


//--------- Begin of function FirmBase::assign_overseer ---------//
//
void FirmBase::assign_overseer(int overseerRecno)
{
	//---- reset the team member count of the general ----//

	if( overseerRecno )
	{
		Unit* unitPtr = unit_array[overseerRecno];

		err_when( !unitPtr->race_id );
		err_when( unitPtr->rank_id != RANK_GENERAL && unitPtr->rank_id != RANK_KING );
		err_when( !unitPtr->team_info );

		unitPtr->team_info->member_count = 0;
	}

	//----- assign the overseer now -------//

	Firm::assign_overseer(overseerRecno);
}
//----------- End of function FirmBase::assign_overseer -----------//


//--------- Begin of function FirmBase::put_info ---------//
//
void FirmBase::put_info(int refreshFlag)
{
	disp_basic_info(INFO_Y1, refreshFlag);

	if( !should_show_info() )
		return;

	disp_base_info(INFO_Y1+54, refreshFlag);
	disp_worker_list(INFO_Y1+104, refreshFlag);
	disp_worker_info(INFO_Y1+168, refreshFlag);
	disp_god_info(INFO_Y1+226, refreshFlag);

	//------ display button -------//

	int x, y=INFO_Y1+279;

	if( own_firm() )
	{
		if( refreshFlag==INFO_REPAINT )
		{
			button_invoke.paint( INFO_X1, y, 'A', "INVOKE" );
			button_reward.paint( INFO_X1 + BUTTON_ACTION_WIDTH, y, 'A', "REWARDSP" );
			button_vacate_firm.paint(INFO_X1 + BUTTON_ACTION_WIDTH * 2, y, 'A', "RECRUIT");
			button_vacate_firm.set_help_code("MOBILIZE");
		}

		if( can_invoke() )
			button_invoke.enable();
		else
			button_invoke.disable();

		if( nation_array[nation_recno]->cash >= REWARD_COST &&
			 ( (overseer_recno && unit_array[overseer_recno]->rank_id != RANK_KING)
			  || selected_worker_id ) )
		{
			button_reward.enable();
		}
		else
		{
			button_reward.disable();
		}

		if( have_own_workers() )
			button_vacate_firm.enable();
		else
			button_vacate_firm.disable();

		x=INFO_X1+BUTTON_ACTION_WIDTH * 3;

		//--- 7kmod-gameplay: the Invoke Fryhtan button uses the last slot of the row. ---//
		//
		// The row is full, so when the player has spies in this Seat of Power the
		// slot is used by the spy menu button instead, as in the original game.

		if( player_spy_count == 0 )
		{
			if( refreshFlag==INFO_REPAINT )
			{
				button_invoke_fryhtan.paint( x, y, 'A', "HIREUNIT" );
				button_invoke_fryhtan.set_help_code("");		// use the custom help below instead of the help of the Inn's hire button
			}

			if( can_invoke_fryhtan() )
				button_invoke_fryhtan.enable();
			else
				button_invoke_fryhtan.disable();

			disp_invoke_fryhtan_cost();
		}
		else if( refreshFlag==INFO_REPAINT )
		{
			button_invoke_fryhtan.reset();
		}
	}
	else
	{
		x=INFO_X1;

		if( refreshFlag==INFO_REPAINT )
			button_invoke_fryhtan.reset();
	}

	disp_spy_button(x, y, refreshFlag);
}
//----------- End of function FirmBase::put_info -----------//


//--------- Begin of function FirmBase::detect_info ---------//
//
int FirmBase::detect_info()
{
	if( detect_basic_info() )
		return 1;

	if( !should_show_info() )
		return 0;

	//------ detect the overseer button -----//

	int rc = mouse.any_click(INFO_X1+6, INFO_Y1+58, INFO_X1+5+UNIT_LARGE_ICON_WIDTH, INFO_Y1+57+UNIT_LARGE_ICON_HEIGHT, LEFT_BUTTON) ? 1 
		: mouse.any_click(INFO_X1+6, INFO_Y1+58, INFO_X1+5+UNIT_LARGE_ICON_WIDTH, INFO_Y1+57+UNIT_LARGE_ICON_HEIGHT, RIGHT_BUTTON) ? 2 : 0;

	if( rc==1 )		// display this overseer's info
	{
		selected_worker_id = 0;
		disp_base_info(INFO_Y1+54, INFO_UPDATE);
		disp_worker_list(INFO_Y1+104, INFO_UPDATE);
		disp_worker_info(INFO_Y1+168, INFO_UPDATE);
		return 1;
	}

	//--------- detect soldier info ---------//

	if( detect_worker_list() )
	{
		disp_base_info(INFO_Y1+54, INFO_UPDATE);
		disp_worker_list(INFO_Y1+104, INFO_UPDATE);
		disp_worker_info(INFO_Y1+168, INFO_UPDATE);
		return 1;
	}

	//---------- detect spy button ----------//

	if( detect_spy_button() )
		return 1;

	if( !own_firm() )
		return 0;

	//------ detect the overseer button -----//

	if( rc==2 )
	{
		if(remote.is_enable())
		{
			// packet structure : <firm recno>
			short *shortPtr=(short *)remote.new_send_queue_msg(MSG_FIRM_MOBL_OVERSEER, sizeof(short));
			shortPtr[0] = firm_recno;
		}
		else
		{
			assign_overseer(0);		// the overseer quits the camp
		}
		return 1;
	}

	//----------- detect invoke -----------//

	if( button_invoke.detect() )
	{
		if(remote.is_enable())
		{
			// ##### begin Gilbert 14/10 #######//
			// packet structure : <firm recno>
			short *shortPtr=(short *)remote.new_send_queue_msg(MSG_F_BASE_INVOKE_GOD, sizeof(short));
			shortPtr[0] = firm_recno;
			// ##### end Gilbert 14/10 #######//
		}
		else
		{
			invoke_god();
		}
		return 1;
	}

	//-------- 7kmod-gameplay: detect invoke Fryhtan --------//

	mod_military_limit_help( nation_recno, button_invoke_fryhtan, "", _("Invoke Fryhtan") );		// 7k-mod-IA

	if( player_spy_count == 0 && button_invoke_fryhtan.detect() )
	{
		if( invoke_fryhtan(COMMAND_PLAYER) )
			se_ctrl.immediate_sound("TURN_ON");

		return 1;
	}

	//----------- detect reward -----------//

	if( button_reward.detect() )
	{
		reward(selected_worker_id, COMMAND_PLAYER);
		// ##### begin Gilbert 26/9 ########//
		se_ctrl.immediate_sound("TURN_ON");
		// ##### end Gilbert 26/9 ########//
		return 1;
	}

	//-------- detect mobilize button ----------//

	if( button_vacate_firm.detect() )
	{
		mobilize_all_workers(COMMAND_PLAYER);
		return 1;
	}

	return 0;
}
//----------- End of function FirmBase::detect_info -----------//


//--------- Begin of function FirmBase::disp_base_info ---------//
//
void FirmBase::disp_base_info(int dispY1, int refreshFlag)
{
	//---------------- paint the panel --------------//

	if( refreshFlag == INFO_REPAINT )
		vga_util.d3_panel_up( INFO_X1, dispY1, INFO_X2, dispY1+46);

	if( !overseer_recno )
		return;

	//------------ display overseer info -------------//

	Unit* overseerUnit = unit_array[overseer_recno];

	int x=INFO_X1+6, y=dispY1+4, x1=x+UNIT_LARGE_ICON_WIDTH+8;

	if( selected_worker_id == 0 )
	{
		vga_front.rect( x-2, y-2, x+UNIT_LARGE_ICON_WIDTH+1, y+UNIT_LARGE_ICON_HEIGHT+1, 2, V_YELLOW );
	}
	else
	{
		vga_util.blt_buf( x-2, y-2, x+UNIT_LARGE_ICON_WIDTH+1, y-1, 0 );
		vga_util.blt_buf( x-2, y+UNIT_LARGE_ICON_HEIGHT+1, x+UNIT_LARGE_ICON_WIDTH+1, y+UNIT_LARGE_ICON_HEIGHT+2, 0 );
		vga_util.blt_buf( x-2, y-2, x-1, y+UNIT_LARGE_ICON_HEIGHT+2, 0 );
		vga_util.blt_buf( x+UNIT_LARGE_ICON_WIDTH, y-2, x+UNIT_LARGE_ICON_WIDTH+1, y+UNIT_LARGE_ICON_HEIGHT+2, 0 );
	}

	//-------------------------------------//

	if( refreshFlag == INFO_REPAINT )
	{
		vga_front.put_bitmap(x, y, unit_res[overseerUnit->unit_id]->get_large_icon_ptr(overseerUnit->rank_id) );
	}

	//-------- set help parameters --------//

	if( mouse.in_area(x, y, x+UNIT_LARGE_ICON_WIDTH+3, y+UNIT_LARGE_ICON_HEIGHT+3) )
		help.set_unit_help( overseerUnit->unit_id, overseerUnit->rank_id, x, y, x+UNIT_LARGE_ICON_WIDTH+3, y+UNIT_LARGE_ICON_HEIGHT+3);

	//-------------------------------------//

	if( overseerUnit->rank_id == RANK_KING )
	{
		if( refreshFlag == INFO_REPAINT )
			font_san.put( x1, y, _("King") );

		y+=14;
	}

	if( refreshFlag == INFO_REPAINT )
		font_san.put( x1, y, overseerUnit->unit_name(0), 0, INFO_X2-2 );		// 0-ask unit_name() not to return the title of the unit

	y+=14;

	//------- display leadership -------//

	String str;

	str  = _("Leadership");
	str += ": ";
	str += overseerUnit->skill.get_skill(SKILL_LEADING);

	font_san.disp( x1, y, str, INFO_X2-10 );
	y+=14;

	//--------- display loyalty ----------//

	if( overseerUnit->rank_id != RANK_KING )
	{
		str  = _("Loyalty");
		str += ":";
		x1 = font_san.put( x1, y, str );

		int x2 = info.disp_loyalty( x1, y-1, x1, overseerUnit->loyalty, overseerUnit->target_loyalty, nation_recno, refreshFlag );

		set_unit_loyalty_help( overseerUnit, INFO_X1+4, y-1, INFO_X2-2, y+13 );		// 7kmod: loyalty breakdown

		if( overseerUnit->spy_recno )
		{
			//------ if this is the player's spy -------//

			if( overseerUnit->true_nation_recno() == nation_array.player_recno )
			{
				vga_front.put_bitmap( x2+5, y+1, image_icon.get_ptr("U_SPY") );
				x2 += 15;
			}
		}

		vga_util.blt_buf( x2, y-1, INFO_X2-2, dispY1+44, 0 );
	}
}
//----------- End of function FirmBase::disp_base_info -----------//


//--------- Begin of function FirmBase::next_day ---------//
//
void FirmBase::next_day()
{
	//----- call next_day() of the base class -----//

	Firm::next_day();

	//--------------------------------------//

	calc_productivity();

	//--------------------------------------//

	if( info.game_date%15 == firm_recno%15 )			// once a week
	{
		train_unit();
		recover_hit_point();
	}

	//------- increase pray points --------//

	if( overseer_recno && pray_points < MAX_PRAY_POINTS )
	{
		// ###### patch begin Gilbert 21/1 #######//
		if( config.fast_build )
			pray_points += productivity/10;
		else
			pray_points += productivity/100;
		// ###### patch end Gilbert 21/1 #######//

		if( pray_points > MAX_PRAY_POINTS )
			pray_points = (float) MAX_PRAY_POINTS;
	}

	//------ validate god_unit_recno ------//

	if( god_unit_recno )
	{
		if( unit_array.is_deleted(god_unit_recno) )
			god_unit_recno = 0;

	#ifdef DEBUG
		if( god_unit_recno )
		{
			err_when( !unit_array[god_unit_recno]->is_visible() );
			err_when( unit_array[god_unit_recno]->nation_recno != nation_recno );
		}
	#endif
	}
}
//----------- End of function FirmBase::next_day -----------//


//------- Begin of function FirmBase::change_nation ---------//
//
void FirmBase::change_nation(int newNationRecno)
{
	//--- update the UnitInfo vars of the workers in this firm ---//

	for( int i=0 ; i<worker_count ; i++ )
		unit_res[ worker_array[i].unit_id ]->unit_change_nation(newNationRecno, nation_recno, worker_array[i].rank_id );

	//------ update base_count_array[] --------//

	err_when( race_id<1 || race_id>MAX_RACE );

	nation_array[nation_recno]->base_count_array[race_id-1]--;

	err_when( nation_array[nation_recno]->base_count_array[race_id-1] < 0 );

	nation_array[newNationRecno]->base_count_array[race_id-1]++;

	//----- change the nation recno of the god invoked by the base if there is any ----//

	if( god_unit_recno && !unit_array.is_deleted(god_unit_recno) )
		unit_array[god_unit_recno]->change_nation(newNationRecno);

	//-------- change the nation of this firm now ----------//

	Firm::change_nation(newNationRecno);
}
//-------- End of function FirmBase::change_nation ---------//


//------- Begin of function FirmBase::train_unit -------//
//
// Increase the praying skills of the prayers.
//
void FirmBase::train_unit()
{
	if( !overseer_recno )
		return;

	Unit* overseerUnit = unit_array[overseer_recno];
	int 	overseerSkill = overseerUnit->skill.skill_level;
	int	incValue;

	//------- increase the commander's leadership ---------//

	if( worker_count > 0 && overseerUnit->skill.skill_level < 100 )
	{
		//-- the more soldiers this commander has, the higher the leadership will increase ---//

		incValue = 3 * worker_count
					  * (int) overseerUnit->hit_points / overseerUnit->max_hit_points
					  * (100+overseerUnit->skill.skill_potential*2) / 100;

		overseerUnit->inc_minor_skill_level(incValue);
	}

	//------- increase the prayer's skill level ------//

	int	  levelMinor;
	Worker* workerPtr = worker_array;

	for( int i=0 ; i<worker_count ; i++, workerPtr++ )
	{
		//------- increase prayer skill -----------//

		if( workerPtr->skill_level < overseerSkill )
		{
			incValue = MAX(20, overseerSkill-workerPtr->skill_level)
						  * workerPtr->hit_points / workerPtr->max_hit_points()
						  * (100+workerPtr->skill_potential*2) / 100;

			levelMinor = workerPtr->skill_level_minor + incValue;		// with random factors, resulting in 75% to 125% of the original

			while( levelMinor >= 100 )
			{
				levelMinor -= 100;
				workerPtr->skill_level++;
			}

			workerPtr->skill_level_minor = levelMinor;
		}
	}
}
//-------- End of function FirmBase::train_unit --------//


//------- Begin of function FirmBase::recover_hit_point -------//
//
// Prayers recover their hit points.
//
// No need to recover the hit points of the general here as
// this is taken care in the Unit class function of the general.
//
void FirmBase::recover_hit_point()
{
	Worker* workerPtr = worker_array;

	for( int i=0 ; i<worker_count ; i++, workerPtr++ )
	{
		//------- increase worker hit points --------//

		if( workerPtr->hit_points < workerPtr->max_hit_points() )
			workerPtr->hit_points++;
	}
}
//------- End of function FirmBase::recover_hit_point -------//

//--------- Begin of function FirmBase::disp_god_info ---------//
//
void FirmBase::disp_god_info(int dispY1, int refreshFlag)
{
	//---------------- paint the panel --------------//

	if( refreshFlag == INFO_REPAINT )
		vga_util.d3_panel_up( INFO_X1, dispY1, INFO_X2, dispY1+50 );

	//-------- display the icon of the mythical creature -------//

	int 		 x=INFO_X1+4, y=dispY1+4;
	UnitInfo* unitInfo = unit_res[ god_res[god_id]->unit_id ];

	if( refreshFlag == INFO_REPAINT )
	{
		vga_util.d3_panel_down( x, y, x+UNIT_LARGE_ICON_WIDTH+3, y+UNIT_LARGE_ICON_HEIGHT+3, 2 );
		vga_front.put_bitmap( x+2, y+2, unitInfo->get_large_icon_ptr(0) );

		//----------- display text ------------//

		x += UNIT_LARGE_ICON_WIDTH+10;

		font_san.put( x, y+2, _(unitInfo->name) );
	}
	else
	{
		x += UNIT_LARGE_ICON_WIDTH+10;
	}

	vga_front.indicator( 0x00, x-3, y+18, pray_points, (float) MAX_PRAY_POINTS, 0 );

	//--- 7kmod-gameplay: display the pray points as a number ---//

	String str;

	str  = _("Points");
	str += ": ";
	str += (int) pray_points;
	str += " / ";
	str += MAX_PRAY_POINTS;

	font_san.disp( x, y+31, str, INFO_X2-4 );
}
//----------- End of function FirmBase::disp_god_info -----------//


//--------- Begin of function FirmBase::disp_invoke_fryhtan_cost ---------//
//
// 7kmod-gameplay: display the pray points cost on the Invoke Fryhtan button,
// and the name of the Fryhtan and its cost in the help of the button.
//
void FirmBase::disp_invoke_fryhtan_cost()
{
	Button3D& b = button_invoke_fryhtan;

	if( !b.init_flag )
		return;

	int cost = invoke_fryhtan_pray_points();

	//------ the cost on the button -------//

	vga_front.bar( b.x1+8, b.y2-19, b.x2-12, b.y2-6, V_WHITE );
	vga_front.rect( b.x1+8, b.y2-19, b.x2-12, b.y2-6, 1, V_BLACK );

	font_mid.center_put( b.x1+8, b.y2-19, b.x2-12, b.y2-6, misc.format(cost) );

	//------ the help of the button -------//

	int fryhtanUnitId = invoke_fryhtan_unit_id();

	if( !fryhtanUnitId )
		return;

	static String title, detail;

	title  = _("Invoke");
	title += " ";
	title += _(monster_res.get_monster_by_unit_id(fryhtanUnitId)->name);

	detail  = _("Cost");
	detail += ": ";
	detail += cost;
	detail += " ";
	detail += _("points");
	detail += ". ";
	detail += _("You have");
	detail += " ";
	detail += (int) pray_points;
	detail += ".";

	help.set_custom_help( b.x1, b.y1, b.x2, b.y2, title, detail );
}
//----------- End of function FirmBase::disp_invoke_fryhtan_cost -----------//


//--------- Begin of function FirmBase::invoke_god ---------//
//
// Invoke God.
//
void FirmBase::invoke_god()
{
	god_unit_recno = god_res[god_id]->invoke(firm_recno, center_x, center_y);
}
//----------- End of function FirmBase::invoke_god -----------//


//--------- Begin of function FirmBase::can_invoke ---------//
//
int FirmBase::can_invoke()
{
	//----- if the base's god creature has been destroyed -----//

	if( god_unit_recno && unit_array.is_deleted(god_unit_recno) )
		god_unit_recno = 0;

	//---------------------------------------------------------//

	return !god_unit_recno &&		// one base can only support one god
			 overseer_recno &&
			 pray_points >= MAX_PRAY_POINTS/10;		// there must be at least 10% of the maximum pray points to cast a creature
}
//----------- End of function FirmBase::can_invoke -----------//


//--------- Begin of function FirmBase::can_invoke_fryhtan ---------//
//
// 7kmod-gameplay: whether the Seat of Power can invoke a Fryhtan now.
//
int FirmBase::can_invoke_fryhtan()
{
	return !under_construction && invoke_fryhtan_unit_id() &&
			 pray_points >= invoke_fryhtan_pray_points() &&
			 !mod_military_limit_reached(nation_recno);		// 7k-mod-IA
}
//----------- End of function FirmBase::can_invoke_fryhtan -----------//


//--------- Begin of function FirmBase::invoke_fryhtan_unit_id ---------//
//
// 7kmod-gameplay: the Fryhtan allied with the race of this Seat of Power.
// All of them cost the same.
//
// return: <int> unit id. of the Fryhtan, 0 if the race has none
//
int FirmBase::invoke_fryhtan_unit_id()
{
	switch( race_id )
	{
		case RACE_NORMAN:		return UNIT_HEADLESS;		// Holgh
		case RACE_MAYA:		return UNIT_SKELETON;		// Deezboanz
		case RACE_GREEK:		return UNIT_LIZARDMAN;		// Sauroid
		case RACE_VIKING:		return UNIT_GIANT_ETTIN;	// Haubudam
		case RACE_PERSIAN:	return UNIT_ROCKMAN;			// Rokken
		case RACE_CHINESE:	return UNIT_FIREKIN;			// Wyrm
		case RACE_JAPANESE:	return UNIT_LYW;				// Rattus
		case RACE_EGYPTIAN:	return UNIT_MAN;				// Karrotten
		case RACE_INDIAN:		return UNIT_GREMJERM;		// Doink
		case RACE_ZULU:		return UNIT_HOBGLOBLIN;		// Broosken
	}

	return 0;
}
//----------- End of function FirmBase::invoke_fryhtan_unit_id -----------//


//--------- Begin of function FirmBase::invoke_fryhtan_pray_points ---------//
//
// 7kmod-gameplay: the pray points needed for invoking the Fryhtan of this
// Seat of Power. They are based on the strength of each Fryhtan (hit points
// x damage per second at combat 50, with the damage bonus of the invoked
// Fryhtans), from 40 to 80 and rounded to a multiple of 5, with -5 for the
// slow ones (speed 3). The Haubudam is limited to 75 and the Doink, a weak
// archer, costs 30. Adjusted after simulating duels against soldiers: the
// Rokken (with more armor) costs 90, the Sauroid (fast) 80 and the Karrotten
// 50. See 7kmod/COMBAT_ANALYSIS.md.
//
int FirmBase::invoke_fryhtan_pray_points()
{
	switch( invoke_fryhtan_unit_id() )
	{
		case UNIT_ROCKMAN:		return 90;		// Rokken (slow, armor 5)
		case UNIT_FIREKIN:		return 75;		// Wyrm
		case UNIT_LIZARDMAN:		return 80;		// Sauroid (fast)
		case UNIT_GIANT_ETTIN:	return 75;		// Haubudam
		case UNIT_HEADLESS:		return 60;		// Holgh (slow)
		case UNIT_MAN:				return 50;		// Karrotten
		case UNIT_LYW:				return 55;		// Rattus
		case UNIT_SKELETON:		return 45;		// Deezboanz
		case UNIT_HOBGLOBLIN:	return 40;		// Broosken
		case UNIT_GREMJERM:		return 30;		// Doink
	}

	return MAX_PRAY_POINTS/10;
}
//----------- End of function FirmBase::invoke_fryhtan_pray_points -----------//


//--------- Begin of function invoked_fryhtan_food_year ---------//
//
// 7kmod-gameplay: the food that a Fryhtan of a nation (invoked) eats in a
// year, from 20 to 50, in proportion to its invoke cost. A person eats
// PERSON_FOOD_YEAR_CONSUMPTION (10).
//
int invoked_fryhtan_food_year(int unitId)
{
	switch( unitId )
	{
		case UNIT_ROCKMAN:		return 50;		// Rokken
		case UNIT_LIZARDMAN:		return 45;		// Sauroid
		case UNIT_FIREKIN:		return 40;		// Wyrm
		case UNIT_GIANT_ETTIN:	return 40;		// Haubudam
		case UNIT_HEADLESS:		return 35;		// Holgh
		case UNIT_LYW:				return 30;		// Rattus
		case UNIT_MAN:				return 30;		// Karrotten
		case UNIT_SKELETON:		return 25;		// Deezboanz
		case UNIT_HOBGLOBLIN:	return 25;		// Broosken
		case UNIT_GREMJERM:		return 20;		// Doink
	}

	return 30;		// other Fryhtans
}
//----------- End of function invoked_fryhtan_food_year -----------//


//--------- Begin of function FirmBase::invoke_fryhtan ---------//
//
// 7kmod-gameplay: invoke a Fryhtan that belongs to the nation of this Seat
// of Power. It costs invoke_fryhtan_pray_points() and the Fryhtan is
// permanent, it doesn't depend on the Seat of Power after it is
// invoked.
//
// return: <int> recno of the Fryhtan unit, 0 if it can't be invoked now or
//               the message has been sent to the other players
//
int FirmBase::invoke_fryhtan(char remoteAction)
{
	if( !can_invoke_fryhtan() )
		return 0;

	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <firm recno>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_F_BASE_INVOKE_FRYHTAN, sizeof(short));
		shortPtr[0] = firm_recno;
		return 0;
	}

	//------- locate a space next to the Seat of Power ------//

	int fryhtanUnitId = invoke_fryhtan_unit_id();

	SpriteInfo* spriteInfo = sprite_res[unit_res[fryhtanUnitId]->sprite_id];

	int xLoc=loc_x1, yLoc=loc_y1;		// xLoc & yLoc are used for returning results

	if( !world.locate_space( &xLoc, &yLoc, loc_x2, loc_y2, spriteInfo->loc_width, spriteInfo->loc_height ) )
		return 0;

	//------------ add the Fryhtan unit -------------//

	int unitRecno = unit_array.add_unit( fryhtanUnitId, nation_recno, RANK_SOLDIER, 0, xLoc, yLoc );

	if( !unitRecno )
		return 0;

	Unit* unitPtr = unit_array[unitRecno];

	unitPtr->set_monster_id( monster_res.get_monster_by_unit_id(fryhtanUnitId)->monster_id );	// normally set by the Fryhtan lair
	unitPtr->set_combat_level( INVOKE_FRYHTAN_COMBAT_LEVEL );
	unitPtr->hit_points = (float) unitPtr->max_hit_points;

	//------------ consume the pray points ----------//

	pray_points -= invoke_fryhtan_pray_points();

	if( pray_points < 0 )
		pray_points = (float) 0;

	if( firm_array.selected_recno == firm_recno )
		info.disp();

	return unitRecno;
}
//----------- End of function FirmBase::invoke_fryhtan -----------//
