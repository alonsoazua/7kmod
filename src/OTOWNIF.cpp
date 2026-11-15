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

//Filename    : OTOWNIF.CPP
//Description : Town interface routines

#include <OINFO.h>
#include <OBOX.h>
#include <OVGA.h>
#include <vga_util.h>
#include <OSYS.h>
#include <OHELP.h>
#include <OSPY.h>
#include <OSTR.h>
#include <OFONT.h>
#include <OMOUSE.h>
#include <OVBROWIF.h>
#include <OGAME.h>
#include <ONATION.h>
#include <OBUTT3D.h>
#include <OBUTTON.h>
#include <OIMGRES.h>
#include <ORAWRES.h>
#include <ORACERES.h>
#include <OUNITRES.h>
#include <OWORLD.h>
#include <OUNIT.h>
#include <OTOWN.h>
#include <OREMOTE.h>
#include <OSE.h>
#include <OSERES.h>
#include <OBUTTCUS.h>
#include <ONEWS.h>
#include "gettext.h"
#include <ConfigAdv.h>
#include <OFIRM.h>
#include <OFIRMA.h>
#include <OFIRMID.h>
#include <OPOWER.h>
#include <ModTech.h>
#include <ModFryhtan.h>


//------------- Define coordinations -----------//

enum { RACE_BROWSE_X1 = INFO_X1,
		 RACE_BROWSE_Y1 = INFO_Y1+48,
		 RACE_BROWSE_X2 = INFO_X2,
		 RACE_BROWSE_Y2 = RACE_BROWSE_Y1+130,
	  };

enum { BUTTON_X1 = INFO_X1,
		 BUTTON_Y1 = RACE_BROWSE_Y2+28,
		 BUTTON_X2 = INFO_X2,
		 BUTTON_Y2 = BUTTON_Y1+50,
     };

//---------- Define constant ------------//

enum { TOWN_MENU_MAIN,
		 TOWN_MENU_TRAIN,
		 TOWN_MENU_SPY,
		 TOWN_MENU_VIEW_SECRET,
		 TOWN_MENU_SET_AUTO_COLLECT_TAX,
		 TOWN_MENU_SET_AUTO_GRANT,
		 TOWN_MENU_RAID,				// 7kmod-fryhtan: the details of a raid on an independent town
		 TOWN_MENU_ORDO,				// 7kmod-fryhtan: the Ordo inside an enslaved town
	  };

#define BUTTON_LOYALTY_COUNT	8
#define COUNT_BUTTON_OFFSET_X	165
#define COUNT_BUTTON_OFFSET_Y 5
#define COUNT_BUTTON_WIDTH		32
#define COUNT_BUTTON_HEIGHT	32

//----------- Define static variables ----------//

static VBrowseIF  browse_race, browse_spy;
static Button3D   button_recruit, button_train, button_tax, button_grant;
static Button3D   button_raid;		// 7kmod-fryhtan: the Raid button of an independent town linked to a lair
static Button     button_raid_village, button_raid_cancel;		// 7kmod-fryhtan: in the Raid menu
static Button     button_ordo;			// 7kmod-fryhtan: the Ordo inside an enslaved town, opens the Ordo menu
static Button     button_ordo_action[3], button_ordo_leave, button_ordo_back;		// 7kmod-fryhtan: in the Ordo menu
static Button3D   button_spy, button_cancel, button_spy_mobilize,
						button_spy_reward, button_spy_action, button_spy_view_secret;
static Button3D	button_cancel_training;
static ButtonCustom  button_cancel3;
static Button		button_loyalty_array[BUTTON_LOYALTY_COUNT];
static Button		button_loyalty_disabled;
static Button		button_cancel2;
static ButtonCustom button_skill[MAX_TRAINABLE_SKILL];
static ButtonCustom button_queue_skill[MAX_TRAINABLE_SKILL];
static int        queue_train_selected;
static short      browse_race_recno=1, browse_race_town_recno=0;		// the town which the browse_race displays its info
static short      recruit_race_count;
static short		spy_count;
static short      last_town_recno=0, last_rebel_recno=0;
static char 		last_has_linked_own_camp;
static char       town_menu_mode=TOWN_MENU_MAIN;
static char       disable_refresh=0;
static short 		action_spy_recno;	// recno of the spy that is doing the bribing or viewing secret reports of other nations
static ButtonCustom button_back_fort;		// 7kmod: return to the Fort in fort recruit mode
static char       last_fort_mode;		// 7kmod: whether the last displayed interface was in fort mode
static Button3D   button_train_captain;		// 7kmod-gameplay
static short      last_captain_train_count;
static short      last_fort_train_count;	// 7kmod: the last displayed number of units training for the Fort

//-------- Define static class member var ------//

short  Town::if_town_recno = 0;
short  Town::fort_recruit_firm_recno = 0;
short  Town::fort_recruit_town_recno = 0;

//----------- Define static functions ----------//

static int  race_filter(int recNo=0);
static const char* ordo_menu_name(TownOrdo* ordoPtr);		// 7kmod-fryhtan
static const char* ordo_action_name(int action);
static int  spy_filter(int recNo=0);
static int  ceilf_int(float f);
static void put_race_rec(int recNo, int x, int y, int refreshFlag);
static void put_spy_rec(int recNo, int x, int y, int refreshFlag);
// ###### begin Gilbert 12/9 ########//
static void i_disp_skill_button(ButtonCustom *button, int);
static void i_disp_queue_skill_button(ButtonCustom *button, int);
// ###### end Gilbert 12/9 ########//
static void i_disp_back_fort_button(ButtonCustom *button, int);


//--------- Begin of function Town::disp_info ---------//
//
void Town::disp_info(int refreshFlag)
{
	if_town_recno = town_recno;

	if( town_recno != last_town_recno ||
		 (refreshFlag==INFO_REPAINT && !disable_refresh) )
	{
		if( town_recno != last_town_recno )
		{
			browse_race_recno = 1;

			if( fort_recruit_town_recno != town_recno )		// 7kmod: switching to another town ends fort mode
				clear_fort_recruit();
		}

		town_menu_mode  = TOWN_MENU_MAIN;
		last_town_recno = town_recno;
	}

	//-----------------------------------------//

	int needRepaint=0;

	if( last_rebel_recno != rebel_recno )
	{
		last_rebel_recno = rebel_recno;
		needRepaint = 1;
	}

	if( last_has_linked_own_camp != has_linked_own_camp )
	{
		last_has_linked_own_camp = has_linked_own_camp;
		needRepaint = 1;
	}

	//--- 7kmod: end fort mode if the Fort is gone, captured or unlinked ---//

	if( fort_recruit_town_recno == town_recno && fort_recruit_firm_recno && !fort_recruit_valid() )
		clear_fort_recruit();

	char fortMode = (char) fort_recruit_valid();

	if( last_fort_mode != fortMode )
	{
		last_fort_mode = fortMode;
		needRepaint = 1;
	}

	if( needRepaint && refreshFlag == INFO_UPDATE )
	{
		info.disp();
		return;
	}

	//-----------------------------------//

	switch( town_menu_mode )
	{
		case TOWN_MENU_MAIN:
			disp_main_menu(refreshFlag);
			break;

		case TOWN_MENU_TRAIN:
			disp_train_menu(refreshFlag);
			break;

		case TOWN_MENU_SPY:
			disp_spy_menu(refreshFlag);
			break;

		case TOWN_MENU_VIEW_SECRET:
			spy_array.disp_view_secret_menu(action_spy_recno, refreshFlag);
			break;

		case TOWN_MENU_SET_AUTO_COLLECT_TAX:
			if( refreshFlag == INFO_REPAINT )
				disp_auto_menu(1);
			break;

		case TOWN_MENU_SET_AUTO_GRANT:
			if( refreshFlag == INFO_REPAINT )
				disp_auto_menu(0);
			break;

		case TOWN_MENU_RAID:
			disp_raid_menu(refreshFlag);		// 7kmod-fryhtan
			break;

		case TOWN_MENU_ORDO:
			disp_ordo_menu(refreshFlag);		// 7kmod-fryhtan
			break;
	}
}
//----------- End of function Town::disp_info -----------//


//--------- Begin of function Town::detect_info ---------//
//
void Town::detect_info()
{
	if_town_recno = town_recno;

	switch( town_menu_mode )
	{
		case TOWN_MENU_MAIN:
			if( detect_main_menu() )
				return;
			break;

		case TOWN_MENU_TRAIN:
			if( detect_train_menu() )
				return;
			break;

		case TOWN_MENU_SPY:
			if( detect_spy_menu() )
				return;
			break;

		case TOWN_MENU_VIEW_SECRET:
			if( spy_array.detect_view_secret_menu(action_spy_recno, nation_recno) )
			{
				town_menu_mode = TOWN_MENU_MAIN;
				info.disp();
				return;
			}
			break;

		case TOWN_MENU_SET_AUTO_COLLECT_TAX:
			if( detect_auto_menu(1) )
				return;
			break;

		case TOWN_MENU_SET_AUTO_GRANT:
			if( detect_auto_menu(0) )
				return;
			break;

		case TOWN_MENU_RAID:
			if( detect_raid_menu() )		// 7kmod-fryhtan
				return;
			break;

		case TOWN_MENU_ORDO:
			if( detect_ordo_menu() )		// 7kmod-fryhtan
				return;
			break;
	}

	if( ISKEY(KEYEVENT_OBJECT_PREV) )
	{
		town_array.disp_next(-1, 0);    // previous same object type of any nation
		return;
	}

	if( ISKEY(KEYEVENT_OBJECT_NEXT) )
	{
		town_array.disp_next(1, 0);     // next same object type of any nation
		return;
	}

	if( ISKEY(KEYEVENT_NATION_OBJECT_PREV) )
	{
		town_array.disp_next(-1, 1);    // prevous same object type of the same nation
		return;
	}

	if( ISKEY(KEYEVENT_NATION_OBJECT_NEXT) )
	{
		town_array.disp_next(1, 1);     // next same object type of the same nation
		return;
	}
}
//----------- End of function Town::detect_info -----------//


//--------- Begin of function Town::disp_main_menu ---------//
//
void Town::disp_main_menu(int refreshFlag)
{
	static short lastTownNationRecno;

	//--- if the town's owner nation has just been changed ---//

	if( lastTownNationRecno != nation_recno )
	{
		lastTownNationRecno = nation_recno;
		info.disp();
		return;
	}

	//--------- display basic info --------//

	disp_basic_info(refreshFlag);

	//---------- paint controls -----------//

	if( refreshFlag == INFO_REPAINT )
	{
		recruit_race_count = race_filter();

		//------ display browser field description -------//

		int x=RACE_BROWSE_X1+2;
		int y=RACE_BROWSE_Y1-23;

		vga_util.d3_panel_up( RACE_BROWSE_X1, y, RACE_BROWSE_X2, RACE_BROWSE_Y1-3 );

		font_san.put( x+2  , y+4, _("Population") );
		font_san.put( x+70 , y+4, _("Peasants") );

		if( nation_recno )      // only display loyalty if this town is controlled by a nation
			font_san.put( x+132, y+4, _("Loyalty") );
		else
		{
			#ifdef GERMAN
				font_san.put( x+128, y+4, "Resistance" );
			#else
				font_san.put( x+132, y+4, _("Resistance") );
			#endif
		}

		//------------ create browser ------------//

		browse_race.init( RACE_BROWSE_X1, RACE_BROWSE_Y1, RACE_BROWSE_X2, RACE_BROWSE_Y2,
								0, 25, recruit_race_count, put_race_rec );

		browse_race.open(browse_race_recno);

		browse_race_town_recno = town_recno;		// the town which browse_race displays

		//---------- paint total section ----------//

		vga_util.d3_panel_up( RACE_BROWSE_X1, RACE_BROWSE_Y2+3, RACE_BROWSE_X2, RACE_BROWSE_Y2+23 );

		font_san.put( RACE_BROWSE_X1+5, RACE_BROWSE_Y2+7, _("Total") );
		font_san.put( RACE_BROWSE_X1+128, RACE_BROWSE_Y2+7, _("Avg") );
	}
	else
	{
		//---------- update controls -----------//

		if( recruit_race_count != race_filter() )
		{
			info.disp();
			return;
		}

		browse_race.update();
	}

	browse_race_recno = browse_race.recno();

	//----------- display total -----------//

	font_mid.put( RACE_BROWSE_X1+52, RACE_BROWSE_Y2+6, population, 1 );
	font_mid.put( RACE_BROWSE_X1+94, RACE_BROWSE_Y2+6, jobless_population, 1 );

	if( nation_recno )
		font_mid.put( RACE_BROWSE_X1+165, RACE_BROWSE_Y2+6, average_loyalty(), 1 );
	else
		font_mid.put( RACE_BROWSE_X1+165, RACE_BROWSE_Y2+6, average_resistance(nation_array.player_recno), 1 );

	//------ if this town is controlled by a rebel group -----//

	int x=BUTTON_X1, y=BUTTON_Y1;

	if( rebel_recno )
	{
		if( refreshFlag == INFO_REPAINT )
			font_san.d3_put( BUTTON_X1, y-1, BUTTON_X2, y+19, _("Controlled by Rebels") );

		y+=23;
	}

	//--- 7kmod-fryhtan: an enslaved town has no buttons (no Recruit, Train, Collect Tax or Grant), only its tribute ---//

	if( nation_recno && nation_recno==nation_array.player_recno && mod_fryhtan.is_enslaved_town(this) )
	{
		if( refreshFlag == INFO_REPAINT )
		{
			button_recruit.reset();
			button_train.reset();
			button_tax.reset();
			button_grant.reset();
			button_train_captain.reset();
			button_back_fort.reset();

			char str[200];

			if( mod_fryhtan.has_lair_commander(this) )
			{
				snprintf( str, sizeof(str), _("Enslaved town. At the end of the month you will receive $%d as tribute."),
							 mod_fryhtan.town_tribute(this) );
			}
			else
			{
				snprintf( str, sizeof(str), "%s", _("No Ordo commands a linked lair: no tribute, and its loyalty is dropping.") );
			}

			vga_util.d3_panel_up( BUTTON_X1, y, BUTTON_X2, y+52 );
			font_san.put_paragraph( BUTTON_X1+6, y+4, BUTTON_X2-6, y+50, str );
		}

		y += 55;

		//--- the Ordo inside the town: its button opens the Ordo menu ---//

		static int lastOrdoKey = -1;
		TownOrdo* ordoPtr = mod_fryhtan.town_ordo(town_recno);
		int ordoKey = ordoPtr ? ordoPtr->action : 0;

		if( refreshFlag == INFO_REPAINT || ordoKey != lastOrdoKey )
		{
			lastOrdoKey = ordoKey;

			if( ordoPtr )
			{
				char str[100];
				// TRANSLATORS: Ordo <Name>: <Subjugate|Kidnap|Taskmaster>
				snprintf( str, sizeof(str), _("Ordo %s: %s"), ordo_menu_name(ordoPtr), ordo_action_name(ordoPtr->action) );
				button_ordo.paint_text( BUTTON_X1, y, BUTTON_X2, y+24, str );
			}
			else if( button_ordo.init_flag )
			{
				button_ordo.reset();

				if( refreshFlag != INFO_REPAINT )
				{
					info.disp();		// clear the button
					return;
				}
			}
		}
	}

	//----------- create the paint button ----------//

	else if( nation_recno==nation_array.player_recno )
	{
		int fortMode = fort_recruit_valid();		// 7kmod

		if( refreshFlag == INFO_REPAINT )
		{
			button_recruit.paint( BUTTON_X1, y, 'A', "RECRUIT" );

			if( fortMode )		// 7kmod: fort mode only has Recruit, Train and Back to Fort
			{
				button_train.paint( BUTTON_X1+BUTTON_ACTION_WIDTH, y, 'A', "TRAIN" );
				button_tax.reset();
				button_grant.reset();

				button_back_fort.paint( BUTTON_X1+BUTTON_ACTION_WIDTH*2, y, BUTTON_X2, y+BUTTON_ACTION_HEIGHT-1,
					i_disp_back_fort_button, ButtonCustomPara(NULL, fort_recruit_firm_recno) );
			}
			else if( has_linked_own_camp )
			{
				button_train.paint( BUTTON_X1+BUTTON_ACTION_WIDTH, y, 'A', "TRAIN" );
				button_tax.paint( BUTTON_X1+BUTTON_ACTION_WIDTH*2, y, 'A', "COLLTAX" );
				button_grant.paint( BUTTON_X1+BUTTON_ACTION_WIDTH*3, y, 'A', "GRANT" );

				disp_auto_loyalty();
			}
			else
			{
				button_train.reset();
				button_tax.reset();
				button_grant.reset();
			}

			#ifdef DEBUG
			if(debug2_enable_flag)
			{
				font_san.d3_put( INFO_X1, INFO_Y2-30, INFO_X2, INFO_Y2, "" );
				font_san.field( INFO_X1+10, INFO_Y2-20, " ", INFO_X1+20, town_recno, 1, INFO_X2-10, refreshFlag);
				font_san.field( INFO_X1+40, INFO_Y2-20, " ", INFO_X1+50, loc_x1, 1, INFO_X2-10, refreshFlag);
				font_san.field( INFO_X1+70, INFO_Y2-20, " ", INFO_X1+80, loc_y1, 1, INFO_X2-10, refreshFlag);
				font_san.field( INFO_X1+100, INFO_Y2-20, " ", INFO_X1+110, ai_link_checked, 1, INFO_X2-10, refreshFlag);
			}
			#endif
		}

		if( refreshFlag == INFO_REPAINT && !fortMode )
			button_back_fort.reset();		// 7kmod

		if( has_linked_own_camp || fortMode )		 // a whole row is used for displaying buttons, so additional buttons will be displayed in the next row
			y += BUTTON_ACTION_HEIGHT;
		else
			x += BUTTON_ACTION_WIDTH;	 // only one button "Recruit", new button displayed next to it.

		//---- 7kmod-gameplay: the Train Captain button, at the start of the second row ----//

		if( refreshFlag == INFO_REPAINT )
		{
			if( has_linked_own_camp && !fortMode )
			{
				button_train_captain.paint( x, y, 'A', "PROMOTE" );
				last_captain_train_count = 0;
			}
			else
				button_train_captain.reset();
		}

		if( button_train_captain.init_flag )
		{
			if( can_train_captain() )
				button_train_captain.enable();
			else
				button_train_captain.disable();

			disp_captain_train_count();

			x += BUTTON_ACTION_WIDTH;		// the spy button goes next to it
		}

		//-------- enable/disable the train button -----------//

		int raceId = race_filter(browse_race.recno());

		if( fortMode ? fort_recruit_can_pull(raceId) : can_recruit(raceId) )
			button_recruit.enable();
		else
			button_recruit.disable();

		if( button_train.init_flag )
		{
			if( can_train(raceId) )
				button_train.enable();
			else
				button_train.disable();

			if( fortMode )
				disp_fort_train_count();		// 7kmod
		}

		if( button_tax.init_flag )
		{
			// ###### patch begin Gilbert 5/8 ######//
			// if( average_loyalty() >= 1 )
			if( average_loyalty() > COLLECT_TAX_LOYALTY_DECREASE )
			// ###### end begin Gilbert 5/8 ######//
				button_tax.enable();
			else
				button_tax.disable();
		}

		if( button_grant.init_flag )
		{
			if( nation_recno && nation_array[nation_recno]->cash > 0 )
				button_grant.enable();
			else
				button_grant.disable();
		}

		//--------- display train info --------//

		if( train_unit_recno )			// display the progress of the current training process
			disp_train_info(refreshFlag);
	}

	//------ grant to an independent town ------//

	else if( nation_array.player_recno &&
				can_grant_to_non_own_town(nation_array.player_recno) )
	{
		if( refreshFlag == INFO_REPAINT )
			button_grant.paint( BUTTON_X1, y, 'A', "GRANT2" );

		if( button_grant.init_flag )
		{
			if( nation_array[nation_array.player_recno]->cash > 0 )
				button_grant.enable();
			else
				button_grant.disable();
		}

		x += BUTTON_ACTION_WIDTH;
	}

	//------ 7kmod-fryhtan: raid an independent town from a linked lair ------//

	else if( nation_array.player_recno &&
				mod_fryhtan.can_see_raid(this, nation_array.player_recno) )
	{
		disp_raid_button(refreshFlag);
		x += BUTTON_ACTION_WIDTH;
	}
	else if( refreshFlag == INFO_REPAINT )
	{
		button_raid.reset();
	}

	//---------- display the spy button ----------//

	int spyFlag = spy_filter() > 0;

	if( refreshFlag == INFO_REPAINT )
	{
		if( spyFlag )				// only display the spy button for non-player towns
			button_spy.paint( x, y, 'A', "SPYMENU" );
		else
			button_spy.reset();
	}
	else
	{
		if( spyFlag != button_spy.init_flag )		// if the button availability has just changed
		{
			if(spyFlag)		// only display the spy button for non-player towns
				button_spy.paint( x, y, 'A', "SPYMENU" );
			else				// remove the button from the screen
				button_spy.hide();
		}
	}

	//-------- display debug info ----------//

	if( sys.debug_session || sys.testing_session )
		disp_debug_resistance(refreshFlag);
}
//----------- End of function Town::disp_main_menu -----------//


//--------- Begin of function Town::disp_raid_button ---------//
//
// 7kmod-fryhtan: the Raid button of an independent town linked to a lair of
// the player's Fryhtan kingdom with a general. It's enabled when the town
// can be raided, and it opens the Raid menu; otherwise its help says why,
// with a bar for the time left until the next raid.
//
void Town::disp_raid_button(int refreshFlag)
{
	int nationRecno = nation_array.player_recno;
	int daysLeft    = mod_fryhtan.raid_days_left(this);
	int lairRecno   = mod_fryhtan.raid_lair(this, nationRecno);
	int hasSoldier  = lairRecno && firm_array[lairRecno]->worker_count > 0;
	int weakened    = mod_fryhtan.is_raid_weakened(this, nationRecno);
	int canRaid     = !daysLeft && hasSoldier && !weakened;

	static int lastTownRecno = -1, lastDaysLeft = -1, lastCanRaid = -1;

	if( refreshFlag == INFO_REPAINT || !button_raid.init_flag ||
		 town_recno != lastTownRecno || daysLeft != lastDaysLeft || canRaid != lastCanRaid )
	{
		button_raid.paint( BUTTON_X1, BUTTON_Y1 + (rebel_recno ? 23 : 0), 'A', "COLLTAX" );
		button_raid.set_help_code("");		// the custom help is set below

		if( canRaid )
			button_raid.enable();
		else
			button_raid.disable();

		//--- the time until the next raid, a bar at the bottom of the button ---//

		if( daysLeft )
		{
			vga_front.indicator( button_raid.x1+6, button_raid.y2-11, button_raid.x2-9, button_raid.y2-7,
										(float) (RAID_INTERVAL_DAYS - daysLeft), (float) RAID_INTERVAL_DAYS, VGA_LIGHT_GREEN, V_BLACK );
		}

		lastTownRecno = town_recno;
		lastDaysLeft  = daysLeft;
		lastCanRaid   = canRaid;
	}

	//------- help: why it can't be raided, or what a raid does -------//

	static char helpStr[300];

	if( weakened )
		snprintf( helpStr, sizeof(helpStr), "%s", _("This village is weakened and ready to be attacked directly.") );
	else if( daysLeft )
		snprintf( helpStr, sizeof(helpStr),
					 ngettext("You can raid this village again in %d day.", "You can raid this village again in %d days.", daysLeft),
					 daysLeft );
	else if( !hasSoldier )
		snprintf( helpStr, sizeof(helpStr), "%s", _("Your lair needs soldiers to raid this village.") );
	else
		snprintf( helpStr, sizeof(helpStr), "%s",
					 _("Raid this village: steal gold and food and call out its defenders to attack your lair. Killing them weakens the village. A village can be raided once every two months.") );

	help.set_custom_help( button_raid.x1, button_raid.y1, button_raid.x2, button_raid.y2, _("Raid"), helpStr );
}
//----------- End of function Town::disp_raid_button -----------//


//--------- Begin of function Town::disp_raid_menu ---------//
//
// 7kmod-fryhtan: the details of a raid on an independent town, in place of
// the list of races: the town (its main race, population and resistance),
// what the raid gives, the defenders that will attack the lair, and the
// Raid village and Cancel buttons.
//
void Town::disp_raid_menu(int refreshFlag)
{
	int nationRecno = nation_array.player_recno;

	//--- back to the main menu if the town can't be raided any more ---//

	if( !mod_fryhtan.can_see_raid(this, nationRecno) || mod_fryhtan.raid_days_left(this) ||
		 mod_fryhtan.is_raid_weakened(this, nationRecno) )
	{
		town_menu_mode = TOWN_MENU_MAIN;
		info.disp();
		return;
	}

	disp_basic_info(refreshFlag);

	//--- repaint when the town changes (once a day at most) ---//

	int defenderCount = mod_fryhtan.raid_defender_count(this, nationRecno);
	int resistance    = average_resistance(nationRecno);

	static int lastKey[4];
	int key[4] = { town_recno, (int) population, resistance, defenderCount };

	if( refreshFlag != INFO_REPAINT )
	{
		if( memcmp(key, lastKey, sizeof(key)) != 0 )
		{
			disable_refresh = 1;    // keep the Raid menu
			info.disp();
			disable_refresh = 0;
		}
		return;
	}

	memcpy( lastKey, key, sizeof(key) );

	//------- the town: its main race, population and resistance -------//

	int y = RACE_BROWSE_Y1-23;

	vga_util.d3_panel_up( INFO_X1, y, INFO_X2, y+56 );

	int raceId = majority_race();

	if( raceId )
	{
		UnitInfo* unitInfo = unit_res[ race_res[raceId]->basic_unit_id ];
		vga_front.put_bitmap( INFO_X1+6, y+4, unitInfo->get_large_icon_ptr(RANK_SOLDIER) );
	}

	int x1 = INFO_X1+6+UNIT_LARGE_ICON_WIDTH+10;
	char str[300];

	snprintf( str, sizeof(str), _("Population: %d"), (int) population );
	font_san.put( x1, y+10, str, 0, INFO_X2-4 );

	snprintf( str, sizeof(str), _("Resistance: %d"), resistance );
	font_san.put( x1, y+28, str, 0, INFO_X2-4 );

	//------- the raid -------//

	y += 60;

	int panelY2 = INFO_Y2;

	vga_util.d3_panel_up( INFO_X1, y, INFO_X2, panelY2 );

	int x = INFO_X1+6;
	int loot = mod_fryhtan.raid_loot(this, nation_array.player_recno);

	// TRANSLATORS: Raid <Town>?
	snprintf( str, sizeof(str), _("Raid %s?"), town_name() );
	font_san.center_put( INFO_X1, y+4, INFO_X2, y+20, str );

	// TRANSLATORS: You will obtain $<Gold> and <Food> food and lower its resistance by <Number>.
	snprintf( str, sizeof(str), _("You will obtain $%d and %d food and lower its resistance by %d."),
				 loot, loot, (int) RAID_RESISTANCE_DEC );
	font_san.put_paragraph( x, y+26, INFO_X2-6, y+72, str, 0 );

	int y2 = y+78;

	snprintf( str, sizeof(str),
				 ngettext("%d defender will attack your lair.", "%d defenders will attack your lair.", defenderCount),
				 defenderCount );
	font_san.put_paragraph( x, y2, INFO_X2-6, y2+30, str, 0 );

	int minCombat = MAX( 10, town_combat_level-10 );
	int maxCombat = MIN( 100, town_combat_level+10 );
	int leaderCombat = MIN( 100, (int) town_combat_level + RAID_LEADER_COMBAT_BONUS );

	// TRANSLATORS: Combat: <min>-<max> (leader: <combat>)
	snprintf( str, sizeof(str), _("Combat: %d-%d (leader: %d)"), minCombat, maxCombat, leaderCombat );
	font_san.put( x, y2+34, str, 0, INFO_X2-6 );

	//------- the buttons -------//

	int buttonY = panelY2-30;

	button_raid_village.paint_text( INFO_X1+8, buttonY, INFO_X1+110, buttonY+24, _("Raid village") );
	button_raid_cancel.paint_text( INFO_X1+118, buttonY, INFO_X2-8, buttonY+24, _("Cancel") );
}
//----------- End of function Town::disp_raid_menu -----------//


//--------- Begin of function Town::detect_raid_menu ---------//
//
int Town::detect_raid_menu()
{
	if( button_raid_village.detect() )
	{
		se_ctrl.immediate_sound("TURN_ON");

		int townRecno = town_recno;

		mod_fryhtan.raid_town(this, nation_array.player_recno);

		if( !town_array.is_deleted(townRecno) )
		{
			town_menu_mode = TOWN_MENU_MAIN;
			info.disp();
		}
		return 1;
	}

	if( button_raid_cancel.detect() )
	{
		se_ctrl.immediate_sound("TURN_OFF");

		town_menu_mode = TOWN_MENU_MAIN;
		info.disp();
		return 1;
	}

	return 0;
}
//----------- End of function Town::detect_raid_menu -----------//


//--------- Begin of static function ordo_menu_name ---------//
//
// 7kmod-fryhtan: the name of the Ordo inside a town: its Ordo name, or
// "<species> Ordo".
//
static const char* ordo_menu_name(TownOrdo* ordoPtr)
{
	const char* name = ModFryhtan::ordo_name_by_id(ordoPtr->name_id);

	return name ? name : _(unit_res[ordoPtr->unit_id]->name);
}


//--------- Begin of static function ordo_action_name ---------//
//
static const char* ordo_action_name(int action)
{
	switch( action )
	{
		case ORDO_ACTION_KIDNAP:		return _("Kidnap");
		case ORDO_ACTION_TASKMASTER:	return _("Taskmaster");
		default:								return _("Subjugate");
	}
}
//----------- End of static function ordo_action_name -----------//


//--------- Begin of function Town::disp_ordo_menu ---------//
//
// 7kmod-fryhtan: the Ordo inside an enslaved town of the player: its
// leadership and combat, the three actions (the current one pushed) with
// their effect, and the buttons to take it out of the town and to go back.
//
void Town::disp_ordo_menu(int refreshFlag)
{
	TownOrdo* ordoPtr = mod_fryhtan.town_ordo(town_recno);

	if( !ordoPtr || nation_recno != nation_array.player_recno )
	{
		town_menu_mode = TOWN_MENU_MAIN;
		info.disp();
		return;
	}

	disp_basic_info(refreshFlag);

	//--- repaint when the town or the Ordo change ---//

	static int lastKey[4];
	int key[4] = { town_recno, (int) population, ordoPtr->action, ordoPtr->leadership };

	if( refreshFlag != INFO_REPAINT )
	{
		if( memcmp(key, lastKey, sizeof(key)) != 0 )
		{
			disable_refresh = 1;    // keep the Ordo menu
			info.disp();
			disable_refresh = 0;
		}
		return;
	}

	memcpy( lastKey, key, sizeof(key) );

	//------- the Ordo -------//

	int y = RACE_BROWSE_Y1-23;
	char str[300];

	vga_util.d3_panel_up( INFO_X1, y, INFO_X2, y+56 );

	UnitInfo* unitInfo = unit_res[ordoPtr->unit_id];
	vga_front.put_bitmap( INFO_X1+6, y+4, unitInfo->get_large_icon_ptr(RANK_GENERAL) );

	int x1 = INFO_X1+6+UNIT_LARGE_ICON_WIDTH+10;

	font_san.put( x1, y+6, ordo_menu_name(ordoPtr), 0, INFO_X2-4 );

	snprintf( str, sizeof(str), _("Leadership: %d"), (int) ordoPtr->leadership );
	font_san.put( x1, y+22, str, 0, INFO_X2-4 );

	snprintf( str, sizeof(str), _("Combat: %d"), (int) ordoPtr->combat_level );
	font_san.put( x1, y+38, str, 0, INFO_X2-4 );

	//------- the actions -------//

	y += 60;

	int leadership = ordoPtr->leadership;
	int kidnapDays = ModFryhtan::kidnap_days(leadership);
	int taskGold   = ModFryhtan::taskmaster_gold(leadership);

	for( int i=0 ; i<3 ; i++ )
	{
		int action = ORDO_ACTION_SUBJUGATE + i;

		button_ordo_action[i].paint_text( INFO_X1+4, y, INFO_X2-4, y+22, ordo_action_name(action) );

		if( ordoPtr->action == action )		// the current action, framed in yellow
			vga_front.rect( INFO_X1+2, y-2, INFO_X2-2, y+24, 2, V_YELLOW );

		switch( action )
		{
			case ORDO_ACTION_SUBJUGATE:
				// TRANSLATORS: The loyalty of the village doesn't drop below <Number>.
				snprintf( str, sizeof(str), _("The loyalty of the village doesn't drop below %d."),
							 MIN( leadership, (int) SUBJUGATE_MAX_LOYALTY ) );
				break;

			case ORDO_ACTION_KIDNAP:
				// TRANSLATORS: A new villager every <Days> to <Days> days.
				snprintf( str, sizeof(str), _("A new villager every %d to %d days."),
							 kidnapDays/2, kidnapDays - kidnapDays/2 + kidnapDays );
				break;

			default:
				// TRANSLATORS: +$<Gold> a month and +<Food> food a year per villager.
				snprintf( str, sizeof(str), _("+$%d.%02d a month and +%d food a year per villager."),
							 taskGold/100, taskGold%100, ModFryhtan::taskmaster_food(leadership) );
				break;
		}

		vga_util.d3_panel_up( INFO_X1, y+24, INFO_X2, y+56 );
		font_san.put_paragraph( INFO_X1+6, y+26, INFO_X2-6, y+56, str, 0 );

		y += 60;
	}

	//------- the buttons -------//

	int buttonY = INFO_Y2-30;

	button_ordo_leave.paint_text( INFO_X1+8, buttonY, INFO_X1+110, buttonY+24, _("Leave village") );
	button_ordo_back.paint_text( INFO_X1+118, buttonY, INFO_X2-8, buttonY+24, _("Back") );
}
//----------- End of function Town::disp_ordo_menu -----------//


//--------- Begin of function Town::detect_ordo_menu ---------//
//
int Town::detect_ordo_menu()
{
	for( int i=0 ; i<3 ; i++ )
	{
		if( button_ordo_action[i].detect() )
		{
			se_ctrl.immediate_sound("TURN_ON");

			mod_fryhtan.set_ordo_action(town_recno, ORDO_ACTION_SUBJUGATE + i);

			disable_refresh = 1;    // keep the Ordo menu
			info.disp();
			disable_refresh = 0;
			return 1;
		}
	}

	if( button_ordo_leave.detect() )
	{
		int unitRecno = mod_fryhtan.ordo_leave_town(town_recno);

		if( unitRecno )
		{
			se_ctrl.immediate_sound("TURN_OFF");
			town_menu_mode = TOWN_MENU_MAIN;
			info.disp();
		}
		else
		{
			se_ctrl.immediate_sound("TURN_OFF");		// no room for it next to the town
		}
		return 1;
	}

	if( button_ordo_back.detect() )
	{
		se_ctrl.immediate_sound("TURN_OFF");

		town_menu_mode = TOWN_MENU_MAIN;
		info.disp();
		return 1;
	}

	return 0;
}
//----------- End of function Town::detect_ordo_menu -----------//


//------- Begin of function Town::open_fort_recruit -------//
//
// 7kmod: select the given town and display its interface in fort
// recruit mode for the given Fort.
//
void Town::open_fort_recruit(short firmRecno, short townRecno)
{
	power.reset_selection();

	fort_recruit_firm_recno = firmRecno;
	fort_recruit_town_recno = townRecno;

	town_array.selected_recno = townRecno;

	se_ctrl.immediate_sound("TURN_ON");

	info.disp();
}
//-------- End of function Town::open_fort_recruit --------//


//------- Begin of function Town::clear_fort_recruit -------//
//
void Town::clear_fort_recruit()
{
	fort_recruit_firm_recno = 0;
	fort_recruit_town_recno = 0;
}
//-------- End of function Town::clear_fort_recruit --------//


//------- Begin of function Town::fort_recruit_valid -------//
//
// Return 1 if this town's interface is in fort recruit mode and the
// Fort is still a player's Fort linked to this player's town.
//
int Town::fort_recruit_valid()
{
	if( !fort_recruit_firm_recno || fort_recruit_town_recno != town_recno )
		return 0;

	if( !nation_recno || nation_recno != nation_array.player_recno )
		return 0;

	if( firm_array.is_deleted(fort_recruit_firm_recno) )
		return 0;

	Firm* firmPtr = firm_array[fort_recruit_firm_recno];

	if( !firmPtr->is_recruit_firm() || firmPtr->nation_recno != nation_recno )		// 7kmod-gameplay: Forts, mines, factories and towers of science
		return 0;

	for( int i=0 ; i<firmPtr->linked_town_count ; i++ )
	{
		if( firmPtr->linked_town_array[i] == town_recno )
			return 1;
	}

	return 0;
}
//-------- End of function Town::fort_recruit_valid --------//


//------- Begin of function Town::fort_recruit_can_pull -------//
//
// Return 1 if a unit of the given race can be recruited directly into
// the Fort of the current fort recruit mode.
//
int Town::fort_recruit_can_pull(int raceId)
{
	if( !fort_recruit_valid() || !can_recruit(raceId) )
		return 0;

	Firm* firmPtr = firm_array[fort_recruit_firm_recno];

	return !firmPtr->under_construction && firmPtr->worker_array &&
			 firmPtr->worker_count < MAX_WORKER;
}
//-------- End of function Town::fort_recruit_can_pull --------//


//------- Begin of function Town::fort_train_count -------//
//
// 7kmod: return the number of units queued or under training in this
// town that go into the given Fort when trained.
//
int Town::fort_train_count(short firmRecno)
{
	int count=0;

	for( int i=0 ; i<train_queue_count ; i++ )
	{
		if( train_queue_firm_array[i] == firmRecno )
			count++;
	}

	if( train_unit_recno && train_unit_firm_recno == firmRecno )
		count++;

	return count;
}
//-------- End of function Town::fort_train_count --------//


//------- Begin of function Town::disp_fort_train_count -------//
//
// 7kmod: display on the Train button the number of units training for
// the Fort, in the same style as the auto tax/grant loyalty numbers.
//
void Town::disp_fort_train_count()
{
	int count = fort_train_count(fort_recruit_firm_recno);

	if( count > 0 )
	{
		vga_front.bar( button_train.x1+8, button_train.y1+10, button_train.x2-12, button_train.y2-15, V_WHITE );
		vga_front.rect( button_train.x1+8, button_train.y1+10, button_train.x2-12, button_train.y2-15, 1, V_BLACK );

		font_mid.center_put( button_train.x1+8, button_train.y1+10, button_train.x2-12, button_train.y2-15,
									misc.format(count) );
	}
	else if( last_fort_train_count > 0 )
	{
		button_train.paint();		// remove the number
	}

	last_fort_train_count = count;
}
//-------- End of function Town::disp_fort_train_count --------//


//------- Begin of function Town::can_train_captain -------//
//
// 7kmod-gameplay: whether a captain can be added to the training queue.
// Captains are only trained in towns where the race of the king is the
// majority, and they are of the king's race.
//
int Town::can_train_captain()
{
	if( !nation_recno )
		return 0;

	Nation* nationPtr = nation_array[nation_recno];
	int kingRaceId = nationPtr->race_id;

	return kingRaceId && majority_race() == kingRaceId &&
			 has_linked_own_camp && jobless_race_pop_array[kingRaceId-1] > 0 &&
			 nationPtr->cash > TRAIN_CAPTAIN_COST &&
			 train_queue_count + (train_unit_recno>0) < MAX_TRAIN_QUEUE;
}
//-------- End of function Town::can_train_captain --------//


//------- Begin of function Town::is_training_captain -------//
//
// 7kmod-gameplay: whether the unit under training is a captain. Only
// captains start the training with CAPTAIN_LEADERSHIP_LEVEL leadership.
//
int Town::is_training_captain()
{
	if( !train_unit_recno )
		return 0;

	Unit* unitPtr = unit_array[train_unit_recno];

	return unitPtr->skill.skill_id == SKILL_LEADING &&
			 unitPtr->skill.skill_level >= CAPTAIN_LEADERSHIP_LEVEL;
}
//-------- End of function Town::is_training_captain --------//


//------- Begin of function Town::captain_train_count -------//
//
// 7kmod-gameplay: the number of captains queued or under training.
//
int Town::captain_train_count()
{
	int count = is_training_captain();

	for( int i=0 ; i<train_queue_count ; i++ )
	{
		if( train_queue_skill_array[i] == TRAIN_CAPTAIN_ID )
			count++;
	}

	return count;
}
//-------- End of function Town::captain_train_count --------//


//------- Begin of function Town::train_total_days -------//
//
// The number of days needed for training the unit under training.
//
int Town::train_total_days()
{
	int totalDays = is_training_captain() ? TRAIN_CAPTAIN_DAYS : TOTAL_TRAIN_DAYS;		// 7kmod-gameplay

	if( config.fast_build && nation_recno==nation_array.player_recno )
		totalDays /= 2;

	return totalDays;
}
//-------- End of function Town::train_total_days --------//


//------- Begin of function Town::disp_captain_train_count -------//
//
// 7kmod-gameplay: display the number of captains in training on the
// Train Captain button.
//
void Town::disp_captain_train_count()
{
	int count = captain_train_count();

	if( count > 0 )
	{
		Button3D& b = button_train_captain;

		vga_front.bar( b.x1+8, b.y1+10, b.x2-12, b.y2-15, V_WHITE );
		vga_front.rect( b.x1+8, b.y1+10, b.x2-12, b.y2-15, 1, V_BLACK );

		font_mid.center_put( b.x1+8, b.y1+10, b.x2-12, b.y2-15, misc.format(count) );
	}
	else if( last_captain_train_count > 0 )
	{
		button_train_captain.paint();		// remove the number
	}

	last_captain_train_count = count;
}
//-------- End of function Town::disp_captain_train_count --------//


//-------- Begin of static function i_disp_back_fort_button --------//
//
// 7kmod: button for returning to the Fort being recruited for.
//
static void i_disp_back_fort_button(ButtonCustom *button, int repaintBody)
{
	int x1 = button->x1;
	int y1 = button->y1;
	int x2 = button->x2;
	int y2 = button->y2;

	if( button->pushed_flag )
	{
		vga_util.d3_panel2_down(x1, y1, x2, y2);
		x1++;
		y1++;
	}
	else
	{
		vga_util.d3_panel2_up(x1, y1, x2, y2);
		x2--;
		y2--;
	}

	//--- 7kmod-gameplay: show the type of the firm, as mines, factories and towers of science can also recruit ---//

	String str("Back to ");
	int firmRecno = button->custom_para.value;

	if( firmRecno && !firm_array.is_deleted(firmRecno) )
		str += _(firm_res[firm_array[firmRecno]->firm_id]->name);
	else
		str += "Fort";

	if( font_san.text_width(str) > x2-x1-4 )		// too long, such as "Back to Tower of Science"
		str = "Back";

	font_san.center_put( x1+2, y1+1, x2-2, y2-1, str );
}
//--------- End of static function i_disp_back_fort_button ---------//


//--------- Begin of function Town::disp_auto_loyalty ---------//
//
void Town::disp_auto_loyalty()
{
	if( auto_collect_tax_loyalty )
	{
		vga_front.bar( button_tax.x1+8, button_tax.y1+10, button_tax.x2-12, button_tax.y2-15, V_WHITE );
		vga_front.rect( button_tax.x1+8, button_tax.y1+10, button_tax.x2-12, button_tax.y2-15, 1, V_BLACK );

		font_mid.center_put( button_tax.x1+8, button_tax.y1+10, button_tax.x2-12, button_tax.y2-15,
									misc.format(auto_collect_tax_loyalty) );
	}

	if( auto_grant_loyalty )
	{
		vga_front.bar( button_grant.x1+8, button_grant.y1+10, button_grant.x2-12, button_grant.y2-15, V_WHITE );
		vga_front.rect( button_grant.x1+8, button_grant.y1+10, button_grant.x2-12, button_grant.y2-15, 1, V_BLACK );

		font_mid.center_put( button_grant.x1+8, button_grant.y1+10, button_grant.x2-12, button_grant.y2-15,
									misc.format(auto_grant_loyalty) );
	}
}
//----------- End of function Town::disp_auto_loyalty -----------//


//--------- Begin of function Town::detect_main_menu ---------//
//
int Town::detect_main_menu()
{
	//--- detect clicking on the name area to center the map on it ---//

	if( mouse.single_click(INFO_X1, INFO_Y1, INFO_X2, INFO_Y1+21) )
	{
		world.go_loc( center_x, center_y );
		return 1;
	}

	//-------- detect browsers ---------//

	if( browse_race.detect() )
	{
		browse_race_recno = browse_race.recno();
		// ##### begin patch Gilbert 21/1 #######//
		if( sys.debug_session || sys.testing_session )
			disp_debug_resistance(INFO_UPDATE);
		// ##### end patch Gilbert 21/1 #######//
		return 1;
	}

	if( button_spy.detect() )	// switch to the spy menu
	{
		town_menu_mode  = TOWN_MENU_SPY;
		disable_refresh = 1;    // static var for disp_info() only
		info.disp();
		disable_refresh = 0;
		return 1;
	}

	//----- 7kmod-fryhtan: open the menu of the Ordo inside an enslaved town ---//

	if( button_ordo.init_flag && mod_fryhtan.town_ordo(town_recno) &&
		 nation_recno == nation_array.player_recno && button_ordo.detect() )
	{
		se_ctrl.immediate_sound("TURN_ON");

		town_menu_mode  = TOWN_MENU_ORDO;
		disable_refresh = 1;    // static var for disp_info() only
		info.disp();
		disable_refresh = 0;
		return 1;
	}

	//----- 7kmod-fryhtan: detect raiding an independent town ---//

	if( nation_array.player_recno &&
		 mod_fryhtan.can_see_raid(this, nation_array.player_recno) &&
		 button_raid.init_flag && button_raid.detect() )
	{
		se_ctrl.immediate_sound("TURN_ON");

		town_menu_mode  = TOWN_MENU_RAID;		// show the details of the raid
		disable_refresh = 1;    // static var for disp_info() only
		info.disp();
		disable_refresh = 0;
		return 1;
	}

	//----- detect granting to an independent town ---//

	if( nation_array.player_recno &&
		 can_grant_to_non_own_town(nation_array.player_recno) )
	{
		if( button_grant.detect() )
		{
			se_ctrl.immediate_sound("TURN_ON");

			grant_to_non_own_town(nation_array.player_recno, COMMAND_PLAYER);
			return 1;
		}
	}

	//---------- buttons for player town only --------//

	if( nation_recno!=nation_array.player_recno )
		return 0;

	//------ update button status ------//

	if( browse_race.recno() > race_filter() )
		return 0;

	int raceId = race_filter(browse_race.recno());
	int fortMode = fort_recruit_valid();		// 7kmod

	if( fortMode ? fort_recruit_can_pull(raceId) : can_recruit(raceId) )
		button_recruit.enable();
	else
		button_recruit.disable();

	if( can_train(raceId) )
		button_train.enable();
	else
		button_train.disable();

	//------- 7kmod-gameplay: detect the Train Captain button -------//
	//
	// Left click adds a captain to the training queue, right click
	// removes one from it.

	if( !fortMode && button_train_captain.init_flag )
	{
		if( can_train_captain() )
			button_train_captain.enable();
		else
			button_train_captain.disable();

		int rc = button_train_captain.detect(0, 0, 1);

		if( rc==2 && captain_train_count()==0 )
			rc = 0;

		if( rc )
		{
			int kingRaceId = nation_array[nation_recno]->race_id;

			if( remote.is_enable() )
			{
				// packet structure : <town recno> <skill id> <race id> <amount>
				short *shortPtr = (short *)remote.new_send_queue_msg(MSG_TOWN_RECRUIT, 4*sizeof(short) );
				shortPtr[0] = town_recno;
				shortPtr[1] = TRAIN_CAPTAIN_ID;
				shortPtr[2] = rc==1 ? kingRaceId : -1;		// -1 for removing from the queue
				shortPtr[3] = 1;
			}
			else
			{
				if( rc==1 )
					add_queue(TRAIN_CAPTAIN_ID, kingRaceId, 1);
				else
					remove_queue(TRAIN_CAPTAIN_ID, 1);
			}

			se_ctrl.immediate_sound(rc==1 ? "TURN_ON" : "TURN_OFF");
			info.disp();
			return 1;
		}
	}

	//------- 7kmod: detect the button for returning to the Fort -------//

	if( fortMode && button_back_fort.init_flag && button_back_fort.detect() )
	{
		short firmRecno = fort_recruit_firm_recno;

		se_ctrl.immediate_sound("TURN_OFF");

		clear_fort_recruit();
		power.reset_selection();
		firm_array.selected_recno = firmRecno;
		firm_array[firmRecno]->sort_worker();
		info.disp();
		return 1;
	}

	//------- detect buttons --------//

	if( button_recruit.detect(GETKEY(KEYEVENT_TOWN_RECRUIT)) )
	{
		if( fortMode )		// 7kmod: recruit directly into the Fort
		{
			if( fort_recruit_can_pull(raceId) )
			{
				firm_array[fort_recruit_firm_recno]->pull_town_people(town_recno, COMMAND_PLAYER, raceId, 1);		// last 1-force pulling people from the town to the firm
				se_ctrl.immediate_sound("PULL_MAN");
			}
		}
		else
		{
			recruit(-1, 0, COMMAND_PLAYER);
		}
		return 1;
	}

	if( button_train.detect(GETKEY(KEYEVENT_TOWN_TRAIN)) )
	{
		if( fortMode )		// 7kmod: train a Leadership unit directly
		{
			if( can_train(raceId) )
			{
				int trainAmount = (mouse.event_skey_state & SHIFT_KEY_MASK) ? TOWN_TRAIN_BATCH_COUNT : 1;

				if( remote.is_enable() )
				{
					// packet structure : <town recno> <skill id> <race id> <amount> <firm recno>
					short *shortPtr = (short *)remote.new_send_queue_msg(MSG_TOWN_TRAIN_FOR_FIRM, 5*sizeof(short) );
					shortPtr[0] = town_recno;
					shortPtr[1] = firm_array[fort_recruit_firm_recno]->recruit_train_skill_id();
					shortPtr[2] = raceId;
					shortPtr[3] = (short)trainAmount;
					shortPtr[4] = fort_recruit_firm_recno;
				}
				else
				{
					add_queue(firm_array[fort_recruit_firm_recno]->recruit_train_skill_id(), raceId, trainAmount, fort_recruit_firm_recno);		// the trained units go into the firm
				}

				se_ctrl.immediate_sound("TURN_ON");
				info.disp();
			}
			return 1;
		}

		town_menu_mode = TOWN_MENU_TRAIN;
		disable_refresh = 1;    // static var for disp_info() only
		info.disp();
		disable_refresh = 0;
		return 1;
	}

	int rc;

	if( (rc=button_tax.detect(0, 0, 1)) > 0 )		// 1-detect right-clicking
	{
		disp_auto_loyalty();

		// ##### begin Gilbert 26/9 ########//
		se_ctrl.immediate_sound("TURN_ON");
		// ##### end Gilbert 26/9 ########//

		if( rc==1 )
		{
			town_array[town_recno]->collect_tax(COMMAND_PLAYER);
		}
		else if( rc==2 )		// right click
		{
			town_menu_mode  = TOWN_MENU_SET_AUTO_COLLECT_TAX;
			disable_refresh = 1;    // static var for disp_info() only
			info.disp();
			disable_refresh = 0;
		}
		return 1;
	}

	if( (rc=button_grant.detect(0, 0, 1)) > 0 )
	{
		disp_auto_loyalty();

		// ##### begin Gilbert 26/9 ########//
		se_ctrl.immediate_sound("TURN_ON");
		// ##### end Gilbert 26/9 ########//

		if( rc==1 )
		{
			town_array[town_recno]->reward(COMMAND_PLAYER);
		}
		else if( rc==2 ) 		// right click
		{
			town_menu_mode  = TOWN_MENU_SET_AUTO_GRANT;
			disable_refresh = 1;    // static var for disp_info() only
			info.disp();
			disable_refresh = 0;
		}
		return 1;
	}

	if(train_unit_recno)
	{
		if((rc = button_cancel_training.detect()))
		{
			if( !remote.is_enable() )
			{
				cancel_train_unit();
				info.disp();
			}
			else
			{
				short *shortPtr = (short *)remote.new_send_queue_msg(MSG_TOWN_SKIP_RECRUIT, sizeof(short));
				shortPtr[0] = town_recno;
			}
			return 1;
		}
	}

	return 0;
}
//----------- End of function Town::detect_main_menu -----------//


//--------- Begin of function Town::disp_basic_info ---------//
//
void Town::disp_basic_info(int refreshFlag)
{
	if( refreshFlag == INFO_REPAINT )
	{
		vga_util.d3_panel_up( INFO_X1, INFO_Y1, INFO_X2, INFO_Y1+21 );

		if( nation_recno )
		{
			font_san.center_put( INFO_X1+21, INFO_Y1, INFO_X2-2, INFO_Y1+21, town_name() );

			char *nationPict = image_button.get_ptr("V_COLCOD");

			vga_front.put_bitmap_trans_remap_decompress(INFO_X1+3, INFO_Y1+2, nationPict, game.get_color_remap_table(nation_recno, 0) );
		}
		else
		{
			font_san.center_put( INFO_X1, INFO_Y1, INFO_X2-2, INFO_Y1+21, town_name() );
		}
	}
}
//----------- End of function Town::disp_basic_info -----------//


//--------- Begin of function Town::disp_train_info ---------//
//
void Town::disp_train_info(int refreshFlag)
{
	if( !train_unit_recno || nation_recno!=nation_array.player_recno )
		return;

	int dispY1 = INFO_Y1+26;

	Unit* unitPtr = unit_array[train_unit_recno];
	int	x=MSG_X1+4, y=MSG_Y1+4;

	if( refreshFlag == INFO_REPAINT )
	{
		vga_util.d3_panel_up( MSG_X1, MSG_Y1, MSG_X2, MSG_Y2 );

		vga_util.d3_panel_down(x, y, x+RACE_ICON_WIDTH+3, y+RACE_ICON_HEIGHT+3 );
		vga_front.put_bitmap(x+2, y+2, race_res[unitPtr->race_id]->icon_bitmap_ptr );

		// vga_util.d3_panel_down(x+RACE_ICON_WIDTH+6, y, MSG_X2-4, MSG_Y2-4 );
	}

	int totalDays = train_total_days();		// 7kmod-gameplay: captains take longer

	vga_front.indicator( 0, x+RACE_ICON_WIDTH+6, y, float(sys.frame_count-start_train_frame_no),
		float(totalDays * FRAMES_PER_DAY), VGA_GRAY );

	button_cancel_training.paint(MSG_X2-27, MSG_Y1+2, "V_X-U", "V_X-D");
	button_cancel_training.set_help_code( "CANCELTRA" );
}
//----------- End of function Town::disp_train_info -----------//


//------ Begin of function Town::browse_selected_race_id ------//
//
// Return the id. of the race selected in the town's race browser.
//
int Town::browse_selected_race_id()
{
	err_when( town_recno != town_array.selected_recno );		// the current town must be the selected one when this function is called

	if( browse_race_town_recno != town_recno )		// the browser still hasn't displayed this town yet. This happens when the selected town is just changed, and this function is called before the town interface is refreshed
		return 0;

	if( browse_race.none_record )
		return 0;

	int raceCount = race_filter();

	if( raceCount != browse_race.total_rec() ||
		 raceCount != recruit_race_count )
	{
		info.disp();
	}

	err_when( browse_race.recno() < 1 );
	err_when( browse_race.recno() > raceCount );
	err_when( browse_race.recno() > browse_race.total_rec() );

	return race_filter(browse_race.recno());
}
//------ End of function Town::browse_selected_race_id ------//


//--------- Begin of function race_filter ---------//
//
// Filter those races that are in the town and can be trained.
//
static int race_filter(int recNo)
{
	err_when( recNo && recNo < 1 );
	err_when( recNo && recNo > recruit_race_count );
	err_when( recNo && recNo > browse_race.total_rec() );

	int 	i, recCount=0;
	Town* townPtr = town_array[Town::if_town_recno];

	for( i=0 ; i<MAX_RACE ; i++ )
	{
		if( townPtr->race_pop_array[i] > 0 )
			recCount++;

		if( recNo && recCount==recNo )
			return i+1;
	}

	err_when( recNo );

	return recCount;
}
//----------- End of function race_filter -----------//


//-------- Begin of static function put_race_rec --------//
//
static void put_race_rec(int recNo, int x, int y, int refreshFlag)
{
	//-------- display race icon -------//

	int       raceId   = race_filter(recNo);
	RaceInfo* raceInfo = race_res[raceId];

	if( refreshFlag == INFO_REPAINT )
	{
		vga_util.d3_panel_down(x+1, y+1, x+RACE_ICON_WIDTH+4, y+RACE_ICON_HEIGHT+4 );
		vga_front.put_bitmap(x+3, y+3, raceInfo->icon_bitmap_ptr);
	}

	//--------- set help parameters --------//

	if( mouse.in_area(x+1, y+1, x+RACE_ICON_WIDTH+4, y+RACE_ICON_HEIGHT+4) )
		help.set_unit_help( raceInfo->basic_unit_id, 0, x+1, y+1, x+RACE_ICON_WIDTH+4, y+RACE_ICON_HEIGHT+4 );

	//-------- display race name --------//

	Town* townPtr = town_array[Town::if_town_recno];

	font_mid.put( x+46, y+6, townPtr->race_pop_array[raceId-1],1, x+87 );
	font_mid.put( x+88, y+6, townPtr->jobless_race_pop_array[raceId-1], 1, x+129 );

	//---- only display loyalty if this town is controlled by a nation ----//

	int curLoyalty, targetLoyalty;
	int x2 = x+browse_race.rec_width-1;

	if( townPtr->nation_recno )
	{
		curLoyalty    = (int) townPtr->race_loyalty_array[raceId-1];
		targetLoyalty = (int) townPtr->race_target_loyalty_array[raceId-1];
	}
	else
	{
		curLoyalty    = (int) townPtr->race_resistance_array[raceId-1][nation_array.player_recno-1];
		targetLoyalty = (int) townPtr->race_target_resistance_array[raceId-1][nation_array.player_recno-1];

		if( targetLoyalty > curLoyalty )		// resistance only decrease, won't increase
			targetLoyalty = -1;					// don't display the decrease target
	}

	//---------- display loyalty/resistance ------------//

	int    dispArrow=0;
	String str;

	if( curLoyalty == targetLoyalty || targetLoyalty == -1 )					// only display up and down arrow for independent town's resistance
	{
		str = curLoyalty;
	}
	else
	{
		str  = curLoyalty;
		str += " ";
		str += targetLoyalty;

		dispArrow=1;
	}

	x2 = font_mid.center_put( x+110, y+6, x2, y+5+font_mid.height(), str, 1 );

	//--------- display up/down arrow -----------//

	if( dispArrow )
	{
		x = x2-font_mid.text_width( misc.format(targetLoyalty) ) - 8;

		if( (int) targetLoyalty > (int) curLoyalty )
			image_icon.put_join( x+1, y+9, "ARROWUP" );

		else if( (int) targetLoyalty < (int) curLoyalty )
			image_icon.put_join( x+1, y+9, "ARROWDWN" );
	}
}
//----------- End of static function put_race_rec -----------//


//--------- Begin of function Town::disp_train_menu ---------//
//
void Town::disp_train_menu(int refreshFlag)
{
	// ####### begin Gilbert 13/9 ########//
	if( refreshFlag == INFO_UPDATE )
	{
		for( int i=1; i<=MAX_TRAINABLE_SKILL; i++)
		{
			button_skill[i-1].paint(-1,0);
			// button_queue_skill[i] is called by automatically
		}
	}
	else if( refreshFlag == INFO_REPAINT )
	{
		font_san.d3_put( INFO_X1, INFO_Y1, INFO_X2, INFO_Y1+18, _("Train (Cost: $30, Skill: 20)") );
		int x=INFO_X1, y=INFO_Y1+24;

		for(int i=1; i<=MAX_TRAINABLE_SKILL; i++)
		{
			button_queue_skill[i-1].create(INFO_X1+COUNT_BUTTON_OFFSET_X,
			y+COUNT_BUTTON_OFFSET_Y,
			INFO_X1+COUNT_BUTTON_OFFSET_X+COUNT_BUTTON_WIDTH-1,
			y+COUNT_BUTTON_OFFSET_Y+COUNT_BUTTON_HEIGHT-1,
			i_disp_queue_skill_button, ButtonCustomPara(this,i));

			button_skill[i-1].paint(INFO_X1, y, 
			INFO_X2, y+BUTTON_ACTION_HEIGHT-1,
			i_disp_skill_button, ButtonCustomPara(&button_queue_skill[i-1],i) );

			y += BUTTON_ACTION_HEIGHT;
		}

		button_cancel3.paint( INFO_X1, y, INFO_X2, y+BUTTON_ACTION_HEIGHT*3/4-1,
		ButtonCustom::disp_text_button_func, ButtonCustomPara((void*)_("Done"),0) );
	}
	// ####### end Gilbert 13/9 ########//
}
//----------- End of function Town::disp_train_menu -----------//

// ######### begin Gilbert 13/9 #########//

//-------- Begin of function i_disp_skill_button --------//
//
static void i_disp_skill_button(ButtonCustom *button, int repaintBody)
{
	int x1 = button->x1;
	int y1 = button->y1;
	int x2 = button->x2;
	int y2 = button->y2;
	if( !button->pushed_flag )
	{
		if( repaintBody )
		{
			vga_util.blt_buf(x1, y1, x2, y2, 0);
			vga_util.d3_panel2_up( x1, y1, x2, y2, 1 );
		}
		x2--;
		y2--;
	}
	else
	{
		if( repaintBody )
		{
			vga_util.blt_buf(x1, y1, x2, y2, 0);
			vga_util.d3_panel2_down( x1, y1, x2, y2, 1 );
		}
		x1++;
		y1++;
	}

	ButtonCustom *queueButton = (ButtonCustom *)button->custom_para.ptr;
	if( repaintBody)
	{
		// display skill large icon
		short skillId = button->custom_para.value;
		char str[9] = "U_";
		strcat( str, Skill::skill_code_array[skillId-1] );
		char *bitmapPtr = image_button.get_ptr(str);

		vga_front.put_bitmap_trans_decompress(x1, y1+4, bitmapPtr);

		// put name
		String str2;

		str2 = "";

		if( skillId == queue_train_selected )
			str2 += ">";

		if( skillId == SKILL_MFT )
			str2 += _("Manufacturing");		// the string in skill_str_array[] is "Manufacture".
		else
			str2 += _(Skill::skill_str_array[skillId-1]);

		if( skillId == queue_train_selected )
			str2 += "<";

		font_bible.put(x1+50, y1+11, str2);
	}

	// display small button
	queueButton->paint(-1, repaintBody);
}
//--------- End of static function i_disp_skill_button ---------//

//-------- Begin of static function i_disp_queue_skill_button --------//
//
static void i_disp_queue_skill_button(ButtonCustom *button, int repaintBody)
{
	Town *townPtr= (Town *)button->custom_para.ptr;

	int x1 = button->x1;
	int y1 = button->y1;
	int x2 = button->x2;
	int y2 = button->y2;
	if( !button->pushed_flag )
	{
		if( repaintBody )
		{
			vga_util.blt_buf(x1, y1, x2, y2, 0);
			vga_util.d3_panel2_up( x1, y1, x2, y2, 1, 1);
		}
		x2--;
		y2--;
	}
	else
	{
		if( repaintBody )
		{
			vga_util.blt_buf(x1, y1, x2, y2, 0);
			vga_util.d3_panel2_down( x1, y1, x2, y2, 1, 1);
		}
		x1++;
		y1++;
	}

	//----- count the no. of units queued for this skill ------//

	short skillId = button->custom_para.value;
	int queuedCount=0;
	for( int i=0 ; i<townPtr->train_queue_count ; i++ )
	{
		if( townPtr->train_queue_skill_array[i] == skillId )
			queuedCount++;
	}
	if(townPtr->train_unit_recno)
	{
		Unit *unitPtr = unit_array[townPtr->train_unit_recno];
		// ##### begin Gilbert 10/10 #######//
		if((unitPtr->skill.skill_id==skillId && !townPtr->is_training_captain())		// 7kmod-gameplay: captains have their own button
			//### begin alex 17/3 ###//
			//|| (unitPtr->spy_recno && skillId == SKILL_SPYING) )
			|| (skillId == SKILL_SPYING && unitPtr->spy_recno && unitPtr->skill.skill_id == 0) ) // 0 for spying-training
			 //#### end alex 17/3 ####//
			queuedCount++;
		// ##### end Gilbert 10/10 #######//
	}

	font_mid.center_put( x1+3, y1+3, x2-3, y2-3, misc.format(queuedCount), 1);
}
//--------- End of static function i_disp_queue_skill_button ---------//
// ######### end Gilbert 13/9 #########//

//--------- Begin of function Town::detect_train_menu ---------//
//
int Town::detect_train_menu()
{
	int	x=INFO_X1+2, y=INFO_Y1+24, rc, quitFlag, waitFlag;

	waitFlag = 0;
	for(int b=1; b<=MAX_TRAINABLE_SKILL; ++b)
	{
		// ###### begin Gilbert 10/9 ########//
		//------ detect pressing on the small queue count button -------//
		rc = 0;
		if( (rc = button_queue_skill[b-1].detect(0,0,2)) != 0)
		{
			quitFlag = 0;		// don't quit the menu right after pressing the button
		}
		//------ detect pressing on the big button -------//
		// but defer to the queue button if clicked over that
		else if( !button_queue_skill[b-1].button_wait && ((rc= button_skill[b-1].detect(0,0,2)) != 0) )
		{
			quitFlag = 1;		// quit the menu right after pressing the button
		}
		// ###### end Gilbert 10/9 ########//

		if( button_queue_skill[b-1].button_wait || button_skill[b-1].button_wait )
			waitFlag = 1;

		int shiftPressed = mouse.event_skey_state & SHIFT_KEY_MASK;

		//------- process the action --------//
		if( rc > 0 )
		{
			// Holding shift will use batches of FIRMWAR_BUILD_BATCH_COUNT
			int trainCancelAmount = shiftPressed ? TOWN_TRAIN_BATCH_COUNT : 1;

			if( rc==1 )		// left button
			{
				if( remote.is_enable() )
				{
					// packet structure : <town recno> <skill id> <race id> <amount>
					short *shortPtr = (short *)remote.new_send_queue_msg(MSG_TOWN_RECRUIT, 4*sizeof(short) );
					shortPtr[0] = town_recno;
					shortPtr[1] = b;
					shortPtr[2] = race_filter(browse_race.recno());
					shortPtr[3] = (short)trainCancelAmount;
				}
				else
					add_queue(b, race_filter(browse_race.recno()), trainCancelAmount);
				// ##### begin Gilbert 26/9 ########//
				se_ctrl.immediate_sound("TURN_ON");
				// ##### end Gilbert 26/9 ########//
			}
			else 				// right button - remove queue
			{
				if( remote.is_enable() )
				{
					// packet structure : <town recno> <skill id> <race id> <amount>
					short *shortPtr = (short *)remote.new_send_queue_msg(MSG_TOWN_RECRUIT, 4*sizeof(short) );
					shortPtr[0] = town_recno;
					shortPtr[1] = b;
					shortPtr[2] = -1;		// -1 race_id represent remove queue
					shortPtr[3] = (short)trainCancelAmount;
				}
				else
					remove_queue(b, trainCancelAmount);
				// ##### begin Gilbert 26/9 ########//
				se_ctrl.immediate_sound("TURN_OFF");
				// ##### end Gilbert 26/9 ########//
			}

			if( quitFlag )
				info.disp();		// info.disp() will call put_info() which will switch mode back to the main menu mode
			// ####### begin Gilbert 10/9 ######//
			else
				info.update();
			// ####### end Gilbert 10/9 ######//

			return 1;
		}

		y += BUTTON_ACTION_HEIGHT;
	}
	//------ detect the cancel button --------//

	if( button_cancel3.detect() || (!waitFlag && mouse.any_click(1)) )		// press the cancel button or right click
	{
		// ##### begin Gilbert 26/9 ########//
		se_ctrl.immediate_sound("TURN_OFF");
		// ##### end Gilbert 26/9 ########//
      town_menu_mode = TOWN_MENU_MAIN;
		info.disp();
		return 1;
	}

	//------ detect production selecting hotkeys --------//

	if( ISKEY(KEYEVENT_MANUF_QUEUE_UP) )
	{
		queue_train_selected--;
		if( queue_train_selected <= 0 )
			queue_train_selected = MAX_TRAINABLE_SKILL;
		disp_train_menu(INFO_REPAINT);
		return 1;
	}

	if( ISKEY(KEYEVENT_MANUF_QUEUE_DOWN) )
	{
		queue_train_selected++;
		if( queue_train_selected > MAX_TRAINABLE_SKILL )
			queue_train_selected = 1;
		disp_train_menu(INFO_REPAINT);
		return 1;
	}

	if( queue_train_selected && ISKEY(KEYEVENT_MANUF_QUEUE_ADD) )
	{
		if( remote.is_enable() )
		{
			// packet structure : <town recno> <skill id> <race id> <amount>
			short *shortPtr = (short *)remote.new_send_queue_msg(MSG_TOWN_RECRUIT, 4*sizeof(short) );
			shortPtr[0] = town_recno;
			shortPtr[1] = queue_train_selected;
			shortPtr[2] = race_filter(browse_race.recno());
			shortPtr[3] = 1;
		}
		else
			add_queue(queue_train_selected, race_filter(browse_race.recno()), 1);
		se_ctrl.immediate_sound("TURN_ON");
		return 1;
	}

	if( queue_train_selected && ISKEY(KEYEVENT_MANUF_QUEUE_ADD_BATCH) )
	{
		if( remote.is_enable() )
		{
			// packet structure : <town recno> <skill id> <race id> <amount>
			short *shortPtr = (short *)remote.new_send_queue_msg(MSG_TOWN_RECRUIT, 4*sizeof(short) );
			shortPtr[0] = town_recno;
			shortPtr[1] = queue_train_selected;
			shortPtr[2] = race_filter(browse_race.recno());
			shortPtr[3] = TOWN_TRAIN_BATCH_COUNT;
		}
		else
			add_queue(queue_train_selected, race_filter(browse_race.recno()), TOWN_TRAIN_BATCH_COUNT);
		se_ctrl.immediate_sound("TURN_ON");
		return 1;
	}

	if( queue_train_selected && ISKEY(KEYEVENT_MANUF_QUEUE_REMOVE) )
	{
		if( remote.is_enable() )
		{
			// packet structure : <town recno> <skill id> <race id> <amount>
			short *shortPtr = (short *)remote.new_send_queue_msg(MSG_TOWN_RECRUIT, 4*sizeof(short) );
			shortPtr[0] = town_recno;
			shortPtr[1] = queue_train_selected;
			shortPtr[2] = -1;		// -1 race_id represent remove queue
			shortPtr[3] = 1;
		}
		else
			remove_queue(queue_train_selected, 1);
		se_ctrl.immediate_sound("TURN_OFF");
		return 1;
	}

	if( queue_train_selected && ISKEY(KEYEVENT_MANUF_QUEUE_REMOVE_BATCH) )
	{
		if( remote.is_enable() )
		{
			// packet structure : <town recno> <skill id> <race id> <amount>
			short *shortPtr = (short *)remote.new_send_queue_msg(MSG_TOWN_RECRUIT, 4*sizeof(short) );
			shortPtr[0] = town_recno;
			shortPtr[1] = queue_train_selected;
			shortPtr[2] = -1;		// -1 race_id represent remove queue
			shortPtr[3] = TOWN_TRAIN_BATCH_COUNT;
		}
		else
			remove_queue(queue_train_selected, TOWN_TRAIN_BATCH_COUNT);
		se_ctrl.immediate_sound("TURN_OFF");
		return 1;
	}

	return 0;
}
//----------- End of function Town::detect_train_menu -----------//


//--------- Begin of function Town::disp_auto_menu ---------//
//
void Town::disp_auto_menu(int modeCollectTax)
{
	int curAutoLoyalty;

	Nation* nationPtr = nation_array[nation_recno];

	if( modeCollectTax )
		curAutoLoyalty =	auto_collect_tax_loyalty;
	else
		curAutoLoyalty =	auto_grant_loyalty;

	//---------- paint buttons ------------//

	String headingStr;

	if( modeCollectTax )
		headingStr = _("Automatically Collect Tax from Villagers when their Loyalty reaches:");
	else
		headingStr = _("Automatically Grant Money to Villagers when their Loyalty drops below:");

	headingStr += "\n";
	headingStr += _("(Left-click below to apply to this village. Right-click below to apply to all your villages.)");

	vga_util.d3_panel_up( INFO_X1, INFO_Y1, INFO_X2, INFO_Y1+110 );

	font_san.put_paragraph( INFO_X1+7, INFO_Y1+8, INFO_X2-7, INFO_Y2-5, headingStr );

	int i, loyaltyLevel, y=INFO_Y1+114;

	for( i=0, loyaltyLevel=30 ; i<BUTTON_LOYALTY_COUNT ; loyaltyLevel+=10, i++, y+=20 )
		button_loyalty_array[i].paint_text( INFO_X1, y, INFO_X2, y+18, misc.format(loyaltyLevel), 0, loyaltyLevel==curAutoLoyalty );

	button_loyalty_disabled.paint_text( INFO_X1, y, INFO_X2, y+18, _("Disabled"), 0, curAutoLoyalty==0 );
	y+=20;

	button_cancel2.paint_text( INFO_X1, y, INFO_X2, y+18, _("Cancel") );
}
//----------- End of function Town::disp_auto_menu -----------//


//--------- Begin of function Town::detect_auto_menu ---------//
//
int Town::detect_auto_menu(int modeCollectTax)
{
	int i, rc=0, loyaltyLevel;

	for( i=0, loyaltyLevel=30 ; i<BUTTON_LOYALTY_COUNT ; loyaltyLevel+=10, i++ )
	{
		rc = button_loyalty_array[i].detect(0, 0, 1);

		if( rc )
			break;
	}

	if( !rc )
	{
		rc = button_loyalty_disabled.detect(0, 0, 1);
		loyaltyLevel = 0;
	}

	//------ set new settings now -------//

	if( rc==1 )
	{
		if( modeCollectTax )
		{
			if( !remote.is_enable() )
			{
				set_auto_collect_tax_loyalty(loyaltyLevel);
			}
			else
			{
				// packet structure <town recno> <loyalty level>
				short *shortPtr = (short *)remote.new_send_queue_msg(MSG_TOWN_AUTO_TAX, 2*sizeof(short) );
				*shortPtr = town_recno;
				shortPtr[1] = loyaltyLevel;
			}
		}
		else
		{
			if( !remote.is_enable() )
			{
				set_auto_grant_loyalty(loyaltyLevel);
			}
			else
			{
				// packet structure <town recno> <loyalty level>
				short *shortPtr = (short *)remote.new_send_queue_msg(MSG_TOWN_AUTO_GRANT, 2*sizeof(short) );
				*shortPtr = town_recno;
				shortPtr[1] = loyaltyLevel;
			}
		}
	}
	else if( rc==2 )
	{
		// ####### begin Gilbert 11/9 ########//
		//----- set the national policy -----//
		if( !remote.is_enable() )
		{
			Nation* nationPtr = nation_array[nation_recno];

			if( modeCollectTax )
				nationPtr->set_auto_collect_tax_loyalty(loyaltyLevel);
			else
				nationPtr->set_auto_grant_loyalty(loyaltyLevel);

			//----- update individual towns -----//

			Town* townPtr;

			for( i=town_array.size() ; i>0 ; i-- )
			{
				if( town_array.is_deleted(i) )
					continue;

				townPtr = town_array[i];
				if( townPtr->nation_recno == nation_recno )
				{
					if( modeCollectTax )
						townPtr->set_auto_collect_tax_loyalty(loyaltyLevel);
					else
						townPtr->set_auto_grant_loyalty(loyaltyLevel);
				}
			}
		}
		else
		{
			err_when(!nation_recno);
			if( modeCollectTax )
			{
				// packet structure <-nation recno> <loyalty level>
				short *shortPtr = (short *)remote.new_send_queue_msg(MSG_TOWN_AUTO_TAX, 2*sizeof(short) );
				*shortPtr = -nation_recno;
				shortPtr[1] = loyaltyLevel;
			}
			else
			{
				// packet structure <-nation recno> <loyalty level>
				short *shortPtr = (short *)remote.new_send_queue_msg(MSG_TOWN_AUTO_GRANT, 2*sizeof(short) );
				*shortPtr = -nation_recno;
				shortPtr[1] = loyaltyLevel;
			}
		}
		// ####### end Gilbert 11/9 ########//
	}

	//--------------------------------------//

	if( button_cancel2.detect() || rc )
	{
		// ##### begin Gilbert 26/9 ########//
		se_ctrl.immediate_sound("TURN_OFF");
		// ##### begin Gilbert 26/9 ########//
		town_menu_mode = TOWN_MENU_MAIN;
		info.disp();
		return 1;
	}

	return 0;
}
//----------- End of function Town::detect_auto_menu -----------//


//--------- Begin of function Town::disp_spy_menu ---------//
//
void Town::disp_spy_menu(int refreshFlag)
{
	disp_basic_info(refreshFlag);

	//---------- paint controls -----------//

	if( refreshFlag == INFO_REPAINT )
	{
		spy_count = spy_filter();

		//------ display browser field description -------//

		int x=RACE_BROWSE_X1+2;
		int y=RACE_BROWSE_Y1-23;

		vga_util.d3_panel_up( RACE_BROWSE_X1, y, RACE_BROWSE_X2, RACE_BROWSE_Y1-3 );

		font_san.put( x+4  , y+4, _("Spy Skill") );
		font_san.put( x+70 , y+4, _("Loyalty") );
		font_san.put( x+130, y+4, _("Action") );

		//------------ create browser ------------//

		browse_spy.init( RACE_BROWSE_X1, RACE_BROWSE_Y1, RACE_BROWSE_X2, RACE_BROWSE_Y2,
								0, 25, spy_count, put_spy_rec );

		browse_spy.open(1);
	}
	else
	{
		//---------- update controls -----------//

		if( spy_count != spy_filter() )
		{
			spy_count = spy_filter();

			if( spy_count>0 )
			{
				disable_refresh = 1;		// stay in the spy menu mode if disable_refresh is 1
				info.disp();
				disable_refresh = 0;
			}
			else
				info.disp();				// reset to the main menu mode if disable_refresh is 0

			return;
		}
		else
			browse_spy.update();
	}

	//----------- create the paint button ----------//

	if( refreshFlag == INFO_REPAINT )
	{
		int x=BUTTON_X1, y=RACE_BROWSE_Y2+5;

		button_spy_mobilize.paint( x, y, 'A', "MOBILSPY" );
		x+=BUTTON_ACTION_WIDTH;

		//--------- reward spy button --------//

		button_spy_reward.paint( x, y, 'A', "REWARD" );
		x+=BUTTON_ACTION_WIDTH;

		if( nation_recno != nation_array.player_recno )		// if the spy is in another nation's town
		{
			button_spy_action.paint( x, y, 'A', "SPYCHACT" );
			x+=BUTTON_ACTION_WIDTH;
		}

		//--- 7kmod-gameplay: the view secret button is replaced by the "Gather Intel" action mode ---//

		button_spy_view_secret.reset();

		if( 0 )
		{
			button_spy_view_secret.paint( x, y, 'A', "VSECRET" );
			x+=BUTTON_ACTION_WIDTH;

			if( x+BUTTON_ACTION_WIDTH-5 > INFO_X2 )
			{
				x  = BUTTON_X1;
				y += BUTTON_ACTION_HEIGHT;
			}
		}

		button_cancel.paint( x, y, 'A', "PREVMENU" );
	}
}
//----------- End of function Town::disp_spy_menu -----------//


//--------- Begin of function Town::detect_spy_menu ---------//
//
int Town::detect_spy_menu()
{
	browse_spy.detect();

	Spy* spyPtr = spy_array[ spy_filter( browse_spy.recno() ) ];

	//------- mobilize spy --------//

	if( button_spy_mobilize.detect() )
	{
		if( !remote.is_enable() )
		{
			if( spyPtr->mobilize_town_spy() )
			{
				spyPtr->notify_cloaked_nation_flag = 0;		// reset it so the player can control it 
				disp_spy_menu( INFO_UPDATE );
			}
		}
		else
		{
			// packet structure <spy recno>
			short *shortPtr = (short *)remote.new_send_queue_msg(MSG_SPY_LEAVE_TOWN, sizeof(short) );
			*shortPtr = spyPtr->spy_recno;
		}
		return 1;
	}

	//------ reward spy ---------//

	else if( button_spy_reward.detect() )
	{
		spyPtr->reward(COMMAND_PLAYER);
		return 1;
	}

	//----- change spy action --------//

	if( nation_recno != nation_array.player_recno )		// if the spy is in another nation's town
	{
		if( button_spy_action.detect() )		// set action mode
		{
			if( !remote.is_enable() )
			{
				spyPtr->set_next_action_mode();
				disp_spy_menu( INFO_UPDATE );
			}
			else
			{
				// packet structure <spy recno>
				short *shortPtr = (short *)remote.new_send_queue_msg(MSG_SPY_CYCLE_ACTION, sizeof(short) );
				*shortPtr = spyPtr->spy_recno;
			}
			return 1;
		}
	}

	//----- view secret report ------/

	if( 0 )		// 7kmod-gameplay: replaced by the "Gather Intel" action mode
	{
		if( button_spy_view_secret.detect() )
		{
			action_spy_recno = spyPtr->spy_recno;
			town_menu_mode   = TOWN_MENU_VIEW_SECRET;
			disable_refresh = 1;
			info.disp();
			disable_refresh = 0;
			return 1;
		}
	}

	//--------- cancel -----------//

	if( button_cancel.detect() || mouse.any_click(1) )		// right click to cancel
	{
		info.disp();
		return 1;
	}

	return 0;
}
//----------- End of function Town::detect_spy_menu -----------//


//--------- Begin of function Town::has_player_spy ---------//
//
// Whether this town has any player spies.
//
int Town::has_player_spy()
{
	int i;
	for( i=0 ; i<MAX_RACE ; i++ )
	{
		if( race_spy_count_array[i] > 0 )
			break;
	}

	if( i==MAX_RACE )		// no spies in this nation
		return 0;

	//----- look for player spy in the spy_array -----//

	Spy* spyPtr;

	for( i=spy_array.size() ; i>=1 ; i-- )
	{
		if( spy_array.is_deleted(i) )
			continue;

		spyPtr = spy_array[i];

		if( spyPtr->spy_place==SPY_TOWN &&
			 spyPtr->spy_place_para==town_recno &&
			 spyPtr->true_nation_recno==nation_array.player_recno )
		{
			return 1;
		}
	}

	return 0;
}
//----------- End of function Town::has_player_spy -----------//


//--------- Begin of function spy_filter ---------//
//
static int spy_filter(int recNo)
{
	Spy* spyPtr;
	int  recCount=0;

	for( int i=spy_array.size() ; i>=1 ; i-- )
	{
		if( spy_array.is_deleted(i) )
			continue;

		spyPtr = spy_array[i];

		if( spyPtr->spy_place==SPY_TOWN &&
			 spyPtr->spy_place_para==Town::if_town_recno &&
			 spyPtr->true_nation_recno==nation_array.player_recno )
		{
			recCount++;
		}

		if( recNo && recCount==recNo )
			return i;
	}

	err_when( recNo );

	return recCount;
}
//----------- End of function spy_filter -----------//


//-------- Begin of static function put_spy_rec --------//
//
static void put_spy_rec(int recNo, int x, int y, int refreshFlag)
{
	int x2 = x+browse_spy.rec_width-1;

	//-------- display icon of the spy unit -----//

	Spy* spyPtr = spy_array[ spy_filter(recNo) ];

	if( refreshFlag == INFO_REPAINT )
	{
		vga_util.d3_panel_down(x+1, y+1, x+RACE_ICON_WIDTH+4, y+RACE_ICON_HEIGHT+4 );
		vga_front.put_bitmap(x+3, y+3, race_res[spyPtr->race_id]->icon_bitmap_ptr);
	}

	//--------- set help parameters --------//

	if( mouse.in_area(x+1, y+1, x+RACE_ICON_WIDTH+4, y+RACE_ICON_HEIGHT+4) )
	{
		int unitId = race_res[spyPtr->race_id]->basic_unit_id;

		help.set_unit_help( unitId, 0, x+1, y+1, x+RACE_ICON_WIDTH+4, y+RACE_ICON_HEIGHT+4 );
	}

	//-------- display spy skill -------//

	font_san.put( x+40, y+6, spyPtr->spy_skill, 1, x+66 );

	//-------- display spy loyalty -------//

	font_san.put( x+67, y+6, spyPtr->spy_loyalty, 1, x+94 );

	//------ display the action mode of the spy ------//

	vga_util.blt_buf( x+95, y+6, x2, y+5+font_san.height(), 0 );

	font_san.center_put( x+95, y+6, x2, y+5+font_san.height(), spyPtr->action_str() );
}
//----------- End of static function put_spy_rec -----------//


//--------- Begin of function Town::recruit ---------//
//
// <int> trainSkillId =  -1 - non-trained unit
//                      >=1 - skill id. of the unit to be trained.
//
// [int] raceId       = the race id. of the unit to be recruited
//                      (default: the currently selected race)
//
// return: <int> recno of the recruited unit
//
int Town::recruit(int trainSkillId, int raceId, char remoteAction)
{
	//---- we can't train a new one when there is one currently under training ---//

	if( trainSkillId >= 1 && train_unit_recno )
		return 0;

	//--------------------------------------------//

	if( !raceId )
	{
		if( browse_race.recno() > race_filter() )
			return 0;

		raceId = race_filter(browse_race.recno());
	}

	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <town recno> <skill id> <race id>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_TOWN_RECRUIT, 4*sizeof(short));
		shortPtr[0] = town_recno;
		shortPtr[1] = trainSkillId;
		shortPtr[2] = raceId;
		shortPtr[3] = 1;
		return 0;
	}

	//---- check if there are units of the race ready for training ----//

	int recruitableCount = recruitable_race_pop(raceId, 1);

	if( recruitableCount == 0 )
		return 0;

	err_when( recruitableCount < 0 );		// 1-allow recruiting spies

	//-------- create an unit ------//

	int townRecno = town_recno;
	int nationRecno = nation_recno;        // save this town's info that is needed as promote_pop() will delete Town if all population of the town are promoted

	//--- if there are spies in this town, chances are that they will be mobilized ---//

	int shouldTrainSpy = race_spy_count_array[raceId-1] >= misc.random(recruitableCount)+1;		// 1-allow recruiting spies

	//---- if we are trying to train an enemy to our spy, then... -----//

	if( shouldTrainSpy && trainSkillId == SKILL_SPYING )
	{
		//-- if there are other non-spy units in the town, then train the other and don't train the spy --//

		if( recruitableCount > race_spy_count_array[raceId-1] )
		{
			shouldTrainSpy = 0;
		}
		//--- if all remaining units are spies, when you try to train one, all of them will become mobilized ---//

		else
		{
			int spyRecno = spy_array.find_town_spy(town_recno, raceId, 1);

			err_when( !spyRecno );

			Spy* spyPtr = spy_array[spyRecno];

			if( !spyPtr->mobilize_town_spy() )
				return 0;

			spyPtr->change_cloaked_nation( spyPtr->true_nation_recno );

			return 0;
		}
	}

	//------- if we should train a spy --------//

	int unitRecno=0;

	if( shouldTrainSpy )
	{
		int  spyCount = spy_array.size();
		int  spyRecno = misc.random(spyCount)+1;
		Spy* spyPtr;

		//-----------------------------------------------------//
		// Spies from other nations will first be mobilized,
		// when all peasants and spies are mobilized and
		// the only ones left in the town are spies from our
		// nation, then mobilize them finally.
		//-----------------------------------------------------//

		for( int mobileNationType=1 ; unitRecno==0 && mobileNationType<=2 ; mobileNationType++ )
		{
			if( mobileNationType==2 )	// only mobilize our own spies are there are the only ones in the town
			{
				if( recruitable_race_pop(raceId,1) > race_spy_count_array[raceId-1] )		// 1-allow recruiting spies
					break;
			}

			for( int i=0 ; i<spyCount ; i++ )
			{
				if( ++spyRecno > spyCount )
					spyRecno = 1;

				if( spy_array.is_deleted(spyRecno) )
					continue;

				spyPtr = spy_array[spyRecno];

				if( spyPtr->spy_place == SPY_TOWN
					 && spyPtr->spy_place_para == town_recno 
					 // ##### patch begin Gilbert 9/4 ######//
					 && spyPtr->race_id == raceId
					 // ##### patch end Gilbert 9/4 ######//
					 )
				{
					if( mobileNationType==1 )		// only mobilize spies from other nations, don't mobilize spies of our own nation
					{
						if( spyPtr->true_nation_recno == nation_recno )
								continue;
					}

					unitRecno = spyPtr->mobilize_town_spy(trainSkillId== -1);	// the parameter is whether decreasing the population immediately, if decrease immediately in recruit mode, not in training mode, 1-mobilize spies
					break;
				}
			}
		}
	}

	//-------- mobilize normal peasant units -------//

	if( !unitRecno )
		unitRecno = mobilize_town_people(raceId, trainSkillId== -1, 0 ); 	// the 2nd parameter is whether decreasing the population immediately, if decrease immediately in recruit mode, not in training mode, 2nd para 0-don't mobilize spies

	if( !unitRecno )
		return 0;

	err_when(unitRecno<=0 || unit_array.is_deleted(unitRecno));

	if( !unitRecno )
		return 0;

	Unit* unitPtr = unit_array[unitRecno];

	//-------- training skill -----------//

	if( trainSkillId > 0 )
	{
		if( trainSkillId == SKILL_SPYING )
		{
			unitPtr->spy_recno = spy_array.add_spy(unitRecno, TRAIN_SKILL_LEVEL);
		}
		else if( trainSkillId == TRAIN_CAPTAIN_ID )		// 7kmod-gameplay: a captain, made a general when trained
		{
			unitPtr->set_combat_level(CAPTAIN_COMBAT_LEVEL);
			unitPtr->skill.skill_id    = SKILL_LEADING;
			unitPtr->skill.skill_level = CAPTAIN_LEADERSHIP_LEVEL;
			unitPtr->skill.skill_potential = 50+misc.random(51);		// 50 to 100 potential
		}
		else
		{
			if( trainSkillId == SKILL_LEADING )		// also increase the combat level for leadership skill training
			{
				//--- 7kmod-gameplay: Elite Warriors technology ---//

				if( mod_tech_res.get_level(MOD_TECH_ELITE_WARRIORS, nationRecno) > 0 )
					unitPtr->set_combat_level(ELITE_WARRIORS_COMBAT_LEVEL);
				else
					unitPtr->set_combat_level(TRAIN_SKILL_LEVEL);
			}
		
			unitPtr->skill.skill_id    = trainSkillId;
			unitPtr->skill.skill_level = TRAIN_SKILL_LEVEL;

			//--- 7kmod-gameplay: Elite Workers technology ---//

			if( trainSkillId != SKILL_LEADING &&
				 mod_tech_res.get_level(MOD_TECH_ELITE_WORKERS, nationRecno) > 0 )
			{
				unitPtr->skill.skill_level = ELITE_WORKERS_SKILL_LEVEL;
			}
		}

		nation_array[nationRecno]->add_expense( EXPENSE_TRAIN_UNIT,
			(float) (trainSkillId == TRAIN_CAPTAIN_ID ? TRAIN_CAPTAIN_COST : TRAIN_SKILL_COST) );
	}
	else
	{
		//------ recruitment without training decreases loyalty --------//

		recruit_dec_loyalty(raceId);

		if( unitPtr->is_own() )
		{
			se_res.far_sound(unitPtr->cur_x_loc(), unitPtr->cur_y_loc(), 1,
				'S', unitPtr->sprite_id, "RDY" );
		}
	}

	//---- training solider or skilled unit takes time ----//

	if( trainSkillId >= 0 )
	{
		err_when(unitRecno<=0 || unit_array.is_deleted(unitRecno));

		err_when( train_unit_recno );		// if there is already a unit under training

		train_unit_recno = unitRecno;
		train_unit_firm_recno = 0;		// 7kmod: set by process_queue() if the unit goes into a Fort
		start_train_frame_no = sys.frame_count;	// as an offset for displaying the progress bar correctly

		unitPtr->deinit_sprite();
		unitPtr->unit_mode = UNIT_MODE_UNDER_TRAINING;
      unitPtr->unit_mode_para = town_recno;
	}

	//--- mobilize_pop() will delete the current Town if population goes down to 0 ---//

	if( town_recno == town_array.selected_recno )
	{
		if( town_array.is_deleted(townRecno) )
			info.disp();
	}

	return unitRecno;
}
//----------- End of function Town::recruit -----------//


//--------- Begin of function ceilf_int ---------//
//
// Similar to ceilf, but with int return. Does not use the standard library
// because different platforms may use incompatible internal fp implemenations.
// MS Visual C++ says they might use sse2. We depend on the compiler flags
// to locally emit the fp code we expect to use.
//
static int ceilf_int(float f)
{
	int x = f;
	float d = f - x;
	if( d>0 )
		x++;
	return x;
}
//----------- End of function ceilf_filter -----------//


//--------- Begin of function Town::recruit_dec_loyalty ---------//
//
// Decrease loyalty when an unit is recruited.
// This function is called by recruit() and Firm::pull_town_people()
//
// <int> raceId - the race to be recruited
// <int> decNow - decrease now, if it is 0, just return the
//						loyalty to be decreased without actual decreasing.
//						(default: 1)
//
// return: <int> - the loyalty decreased or to be decreased.
//
int Town::recruit_dec_loyalty(int raceId, int decNow)
{
	float loyaltyDec = MIN( 5, (float) MAX_TOWN_POPULATION / race_pop_array[raceId-1] );

	if( config_adv.fix_recruit_dec_loyalty )
	{
		loyaltyDec += accumulated_recruit_penalty/5;
		loyaltyDec = MIN(loyaltyDec, 10);
	}

	//------ recruitment without training decreases loyalty --------//

	if( decNow )
	{
		if( !config_adv.fix_recruit_dec_loyalty )
		{
			loyaltyDec += accumulated_recruit_penalty/5;

			loyaltyDec = MIN(loyaltyDec, 10);
		}

		accumulated_recruit_penalty += 5;

		//-------------------------------------//

		race_loyalty_array[raceId-1] -= loyaltyDec;

		if( race_loyalty_array[raceId-1] < 0 )
			race_loyalty_array[raceId-1] = (float) 0;
	}

	if( config_adv.fix_recruit_dec_loyalty )
	{
		// for ai estimating, round integer conversion up, do not truncate
		return ceilf_int(loyaltyDec);
	}

	return (int) loyaltyDec;
}
//----------- End of function Town::recruit_dec_loyalty -----------//


//--------- Begin of function Town::process_train ---------//
void Town::process_train()
{
	err_when( !train_unit_recno );

	Unit* unitPtr = unit_array[train_unit_recno];
	int   raceId  = unitPtr->race_id;

	//---- if the unit being trained was killed -----//

	int cancelFlag = 0;

	err_when( jobless_race_pop_array[raceId-1] < 0 );

	if( jobless_race_pop_array[raceId-1]==0 )		// the unit being trained was killed
	{
		cancelFlag = 1;
	}

	//-----------------------------------------------------------------//
	//
	// If after start training the unit (non-spy unit), a unit has been
	// mobilized, resulting that the spy count >= jobless_race,
	// we must cancel the training, otherwise when training finishes,
	// and dec_pop is called, spy count will > jobless count and cause error.
	//
	//-----------------------------------------------------------------//

	err_when( race_spy_count_array[raceId-1] > jobless_race_pop_array[raceId-1] );

	if( race_spy_count_array[raceId-1] == jobless_race_pop_array[raceId-1] )
		cancelFlag = 1;

	if( cancelFlag )
	{
		unit_array.disappear_in_town(train_unit_recno, town_recno);
		train_unit_recno = 0;
		train_unit_firm_recno = 0;		// 7kmod
		return;
	}

	//------------- process training ---------------//

	int totalDays = train_total_days();		// 7kmod-gameplay: captains take longer

	if( (int)(sys.frame_count-start_train_frame_no) / FRAMES_PER_DAY >= totalDays )
	{
		finish_train(unitPtr);
	}
}
//----------- End of function Town::process_train -----------//


//--------- Begin of function Town::finish_train ---------//

void Town::finish_train(Unit* unitPtr)
{
	err_when(train_unit_recno<=0 || unit_array.is_deleted(train_unit_recno));
	SpriteInfo*	spriteInfo = unitPtr->sprite_info;
	int 			xLoc=loc_x1; // xLoc & yLoc are used for returning results
	int 			yLoc=loc_y1;

	if( !world.locate_space(&xLoc, &yLoc, loc_x2, loc_y2, spriteInfo->loc_width, spriteInfo->loc_height) )
		return;

	int isCaptain = is_training_captain();		// 7kmod-gameplay

	unitPtr->init_sprite(xLoc, yLoc);

	if( isCaptain )
		unitPtr->set_rank(RANK_GENERAL);

	if( unitPtr->is_own() )
	{
		if( config_adv.news_notify_complete )
			news_array.unit_trained(unitPtr->sprite_recno, town_recno);
		se_res.far_sound( xLoc, yLoc, 1, 'S', unitPtr->sprite_id, "RDY");
	}

	unitPtr->unit_mode = 0;		// reset it to 0 from UNIT_MODE_UNDER_TRAINING
	train_unit_recno   = 0;

	short destFirmRecno = train_unit_firm_recno;		// 7kmod: the Fort the unit goes into
	train_unit_firm_recno = 0;
	int   unitRecno = unitPtr->sprite_recno;
	int   unitNationRecno = unitPtr->nation_recno;

	int townRecno = town_recno;		// save the recno as it can be deleted in dec_pop()

	dec_pop(unitPtr->race_id, 0);		// decrease the population now as the recruit() does do so

	//---- 7kmod: put the trained unit directly into its Fort if it still has room ----//

	if( destFirmRecno && !firm_array.is_deleted(destFirmRecno) && !unit_array.is_deleted(unitRecno) )
	{
		Firm* firmPtr = firm_array[destFirmRecno];

		if( firmPtr->is_recruit_firm() && firmPtr->nation_recno == unitNationRecno &&
			 !firmPtr->under_construction && firmPtr->worker_array &&
			 firmPtr->worker_count < MAX_WORKER &&
			 unit_array[unitRecno]->rank_id == RANK_SOLDIER )
		{
			firmPtr->assign_unit(unitRecno);
		}
	}

	//---- if this trained unit is tied to an AI action ----//

	if( train_unit_action_id )
	{
		nation_array[nation_recno]->process_action_id(train_unit_action_id);
		train_unit_action_id = 0;
	}

	//----- refresh if this town is currently selected ------//

	if(townRecno==town_array.selected_recno)
	{
		if(town_menu_mode==TOWN_MENU_MAIN)
		{
			info.disp();
		}
		else
		{
			disable_refresh = 1;
			info.disp();
			disable_refresh = 0;
		}
	}
}
//----------- End of function Town::finish_train -----------//


//--------- Begin of function Town::process_queue ---------//
void Town::process_queue()
{
	if(train_queue_count==0)
		return;

	if(jobless_population==0)
		return;

	err_when(train_queue_count > MAX_TRAIN_QUEUE);
	
	char queueCount = train_queue_count;
	char skillId, raceId;
	char i;
	for(i=0; i<queueCount; ++i)
	{
		if(can_train(train_queue_race_array[i]) &&
			(train_queue_skill_array[i] != TRAIN_CAPTAIN_ID ||
			 nation_array[nation_recno]->cash > TRAIN_CAPTAIN_COST))		// 7kmod-gameplay
		{
			skillId = train_queue_skill_array[i];
			raceId = train_queue_race_array[i];
			short firmRecno = train_queue_firm_array[i];		// 7kmod
			err_when(train_queue_count-i-1 < 0 || train_queue_count-i-1 > MAX_TRAIN_QUEUE);
			memmove(train_queue_skill_array, train_queue_skill_array+i+1,
						sizeof(train_queue_skill_array[0])*(train_queue_count-i-1));
			memmove(train_queue_race_array, train_queue_race_array+i+1,
						sizeof(train_queue_race_array[0])*(train_queue_count-i-1));
			memmove(train_queue_firm_array, train_queue_firm_array+i+1,
						sizeof(train_queue_firm_array[0])*(train_queue_count-i-1));		// 7kmod
			train_queue_count -= i+1;
			if( recruit(skillId, raceId, COMMAND_AUTO) && train_unit_recno )
				train_unit_firm_recno = firmRecno;		// 7kmod: the Fort the unit goes into when trained
			break;
		}
	}
	
	if(i==queueCount)
		train_queue_count = 0;

	if(town_menu_mode==TOWN_MENU_MAIN)
		info.disp();
}
//----------- End of function Town::process_queue -----------//


//--------- Begin of function Town::add_queue ---------//
void Town::add_queue(char skillId, char raceId, int amount, short firmRecno)
{
	err_when(amount < 0);
	if (amount < 0) return;

	int queueSpace = MAX_TRAIN_QUEUE - train_queue_count - (train_unit_recno>0);
	int enqueueAmount = MIN(queueSpace, amount);

	for (int i = 0; i < enqueueAmount; ++i)
	{
		train_queue_skill_array[train_queue_count] = skillId;
		train_queue_firm_array[train_queue_count] = firmRecno;		// 7kmod
		train_queue_race_array[train_queue_count++] = raceId;
	}	

	if( !train_unit_recno )
		process_queue();
}
//----------- End of function Town::add_queue -----------//


//--------- Begin of function Town::remove_queue ---------//
void Town::remove_queue(char skillId, int amount)
{
	err_when(amount < 1);
	if (amount < 1) return;

	for(int i=train_queue_count-1; i>=0; i--)
	{
		if(train_queue_skill_array[i] == skillId)
		{
			err_when(train_queue_count > MAX_TRAIN_QUEUE);

			misc.del_array_rec(train_queue_skill_array, train_queue_count, sizeof(train_queue_skill_array[0]), i+1);
			misc.del_array_rec(train_queue_race_array, train_queue_count, sizeof(train_queue_race_array[0]), i+1);
			misc.del_array_rec(train_queue_firm_array, train_queue_count, sizeof(train_queue_firm_array[0]), i+1);		// 7kmod
			train_queue_count--;			
			amount--;

			if (amount <= 0) return;
		}
	}

	// If there were less trained of skillId in the queue than were requested to be removed then
	// also cancel currently trained unit
	if(train_unit_recno)
	{
		Unit *unitPtr = unit_array[train_unit_recno];
		if( skillId == TRAIN_CAPTAIN_ID ? is_training_captain() :		// 7kmod-gameplay
			 ((unitPtr->skill).skill_id==skillId && !is_training_captain()) )
			cancel_train_unit();
	}
}
//----------- End of function Town::remove_queue -----------//


//--------- Begin of function Town::disp_debug_resistance ---------//
//
void Town::disp_debug_resistance(int refreshFlag)
{
	if( nation_recno == nation_array.player_recno )		// not for player's own town
		return;

	if( refreshFlag==INFO_REPAINT )
		vga_util.d3_panel_up( INFO_X1, INFO_Y2-50, INFO_X2, INFO_Y2 );

	//------ display resistance (only for independent town) -----//

	int x=INFO_X1+10, y=INFO_Y2-47;

	if( nation_recno ==0 )
	{
		int raceId = race_filter(browse_race.recno());

		for( int i=1 ; i<=nation_array.size() ; i++, x+=28 )
		{
			if( nation_array.is_deleted(i) )
				continue;

			if( refreshFlag==INFO_REPAINT )
				vga_front.bar( x, y, x+18, y+16, nation_array[i]->nation_color );

			font_san.put( x, y+18, (int) race_resistance_array[raceId-1][i-1], 1, x+19 );
			font_san.put( x, y+32, (int) race_target_resistance_array[raceId-1][i-1], 1, x+19 );
		}
	}
	else
	{
		//------ if this town is the nation's base town -----//

		String str;
		
		str = "Base: ";
		str += is_base_town;

		font_san.put( INFO_X1+10, y, str );

		str = "Town recno: ";
		str += town_recno;

		font_san.put( INFO_X1+70, y, str );

		str = "no_neighbor_space: ";
		str += no_neighbor_space;

		font_san.put( INFO_X1+10, y+16, str );

		str = "quality of life: ";
		str += quality_of_life;

		font_san.disp( INFO_X1+10, y+32, str, INFO_X2-10 );
	}
}
//----------- End of function Town::disp_debug_resistance -----------//


//--------- Begin of function Town::get_elected_race ---------//
int Town::get_selected_race()
{
	if(browse_race.recno() > race_filter())
		return 0;

	return race_filter(browse_race.recno());
}
//----------- End of function Town::get_elected_race -----------//
