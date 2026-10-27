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

//Filename    : ModEditorWin.cpp
//Description : 7kmod-editor: the windows Kingdoms and Goals (phase 3) and New Map.
//
// The windows are drawn over the map. Each change repaints the whole window
// over a copy of the map view, so that the translucent panels don't get
// lighter each time.

#include <ModEditor.h>
#include <OSYS.h>
#include <OCONFIG.h>
#include <OVGA.h>
#include <vga_util.h>
#include <OFONT.h>
#include <OMOUSE.h>
#include <KEY.h>
#include <OPOWER.h>
#include <OINFO.h>
#include <ODATE.h>
#include <OSTR.h>
#include <OWORLDMT.h>
#include <ONATION.h>
#include <OTECHRES.h>
#include <ORACERES.h>
#include <OGAME.h>
#include <OGETA.h>
#include <OBOX.h>
#include <OMISC.h>
#include <GAMEDEF.h>
#include <stdlib.h>
#include <time.h>
#include "gettext.h"

//------- the window --------//

enum { WIN_X1 = ZOOM_X1+6,
		 WIN_Y1 = ZOOM_Y1+6,
		 WIN_X2 = ZOOM_X2-6,
		 WIN_Y2 = ZOOM_Y2-6 };

//------- the Kingdoms window --------//

enum { ROW_Y1 = WIN_Y1+50,
		 ROW_HEIGHT = 24,
		 NAME_X1 = WIN_X1+8,
		 NAME_X2 = WIN_X1+186,
		 CASH_X = WIN_X1+196,
		 FOOD_X = WIN_X1+312,
		 REPU_X = WIN_X1+428,
		 DETAIL_Y1 = ROW_Y1 + MAX_NATION*ROW_HEIGHT + 8,
		 TECH_X = WIN_X1+8,
		 RELATION_X = WIN_X1+290 };

enum { CASH_STEP = 1000, CASH_MAX = 1000000,
		 FOOD_STEP = 1000, FOOD_MAX = 1000000,
		 REPU_STEP = 10 };

enum { NO_CONTACT = -1 };		// the relation status "No contact" in the window

//------- Declare static functions --------//

static char* save_view();
static void	 rest_view(char* viewBitmap);
static void	 put_value(int x, int y, int width, const char* valueStr);
static int	 detect_value(int x, int y, int width);
static void	 put_window(const char* title);
static void	 put_close_button();
static int	 detect_close_button();
static void	 disp_kingdoms(int selNation);
static int	 detect_kingdoms(int& selNation);
static int	 relation_value(int nationRecno, int withNationRecno);
static void	 set_relation_value(int nationRecno, int withNationRecno, int value);


//-------- Begin of function ModEditor::edit_kingdoms --------//
//
// The window of the kingdoms: treasure, food, reputation, technologies and
// relations.
//
void ModEditor::edit_kingdoms()
{
	if( !nation_array.nation_count )
		return;

	int selNation = nation_array.player_recno;

	if( !selNation || nation_array.is_deleted(selNation) )
	{
		for( selNation=1 ; selNation<=nation_array.size() && nation_array.is_deleted(selNation) ; selNation++ );
	}

	power.win_opened = 1;

	int useBackBuf = vga.use_back_buf;
	char* viewBitmap = save_view();

	int refresh = 1;

	while( 1 )
	{
		sys.yield();
		vga.flip();
		mouse.get_event();

		if( sys.signal_exit_flag == 1 )
			break;

		if( mod_editor.detect_bar_switch() )		// another button of the editor bar (or the same one: close)
			break;

		if( refresh )
		{
			rest_view(viewBitmap);
			disp_kingdoms(selNation);
			refresh = 0;
		}

		if( detect_close_button() || mouse.key_code==KEY_ESC )		// not the right button: it changes the relations
			break;

		if( detect_kingdoms(selNation) )
			refresh = 1;
	}

	mem_del(viewBitmap);

	power.win_opened = 0;

	if( useBackBuf )
		vga.use_back();
	else
		vga.use_front();

	redraw_screen();
}
//--------- End of function ModEditor::edit_kingdoms ---------//


//-------- Begin of static function disp_kingdoms --------//
//
static void disp_kingdoms(int selNation)
{
	vga.use_back();

	put_window( _("Kingdoms") );

	font_san.put( NAME_X1, WIN_Y1+32, _("Kingdom") );
	font_san.put( CASH_X+20, WIN_Y1+32, _("Treasure") );
	font_san.put( FOOD_X+20, WIN_Y1+32, _("Food") );
	font_san.put( REPU_X+20, WIN_Y1+32, _("Reputation") );

	//------- one row for each kingdom -------//

	String str;

	for( int n=1 ; n<=nation_array.size() ; n++ )
	{
		if( nation_array.is_deleted(n) )
			continue;

		Nation* nationPtr = nation_array[n];
		int y = ROW_Y1 + (n-1)*ROW_HEIGHT;

		if( n == selNation )
			vga_util.d3_panel_down( NAME_X1-4, y-3, NAME_X2, y+18 );

		vga_back.bar( NAME_X1, y+1, NAME_X1+11, y+12, nationPtr->nation_color );

		str = nationPtr->nation_name();

		if( n == nation_array.player_recno )
		{
			str += " ";
			str += _("(you)");
		}

		font_san.put( NAME_X1+16, y, str, 0, NAME_X2-2 );

		str = misc.format((int)nationPtr->cash, 2);
		put_value( CASH_X, y, 110, str );

		str = misc.format((int)nationPtr->food);
		put_value( FOOD_X, y, 110, str );

		str = misc.format((int)nationPtr->reputation);
		put_value( REPU_X, y, 110, str );
	}

	//------- the selected kingdom: technologies -------//

	Nation* nationPtr = nation_array[selNation];
	int y = DETAIL_Y1;

	vga_back.bar( TECH_X, y+1, TECH_X+11, y+12, nationPtr->nation_color );
	font_san.put( TECH_X+16, y, nationPtr->nation_name(), 0, RELATION_X-10 );
	y += 22;

	font_san.put( TECH_X, y, _("Technology") );
	y += 18;

	for( int i=1 ; i<=tech_res.tech_count ; i++, y+=20 )
	{
		TechInfo* techInfo = tech_res[i];

		font_san.put( TECH_X+6, y+2, techInfo->tech_des(), 0, TECH_X+150 );

		snprintf( str, MAX_STR_LEN+1, "%d / %d", techInfo->get_nation_tech_level(selNation), techInfo->max_tech_level );
		put_value( TECH_X+154, y, 100, str );
	}

	editor_put_button( TECH_X+6, y+4, TECH_X+126, y+23, _("All technologies"), 1, 0 );
	editor_put_button( TECH_X+134, y+4, TECH_X+254, y+23, _("No technologies"), 1, 0 );

	//------- the selected kingdom: relations -------//

	y = DETAIL_Y1 + 22;

	font_san.put( RELATION_X, y, _("Relations") );
	y += 18;

	for( int n=1 ; n<=nation_array.size() ; n++, y+=0 )
	{
		if( nation_array.is_deleted(n) || n==selNation )
			continue;

		Nation* withNation = nation_array[n];

		vga_back.bar( RELATION_X+6, y+3, RELATION_X+17, y+14, withNation->nation_color );
		font_san.put( RELATION_X+22, y+2, withNation->nation_name(), 0, RELATION_X+150 );

		int value = relation_value(selNation, n);

		editor_put_button( RELATION_X+156, y, RELATION_X+266, y+18,
			value==NO_CONTACT ? _("No contact") : _(NationRelation::relation_status_str_array[value]), 1, 0 );

		y += 22;
	}

	put_close_button();

	vga.use_front();
	vga_util.blt_buf( WIN_X1, WIN_Y1, WIN_X2, WIN_Y2, 0 );
}
//----------- End of static function disp_kingdoms -----------//


//-------- Begin of static function detect_kingdoms --------//
//
// return: <int> 1 - something has changed
//
static int detect_kingdoms(int& selNation)
{
	for( int n=1 ; n<=nation_array.size() ; n++ )
	{
		if( nation_array.is_deleted(n) )
			continue;

		Nation* nationPtr = nation_array[n];
		int y = ROW_Y1 + (n-1)*ROW_HEIGHT;
		int dir;

		if( mouse.any_click( NAME_X1-4, y-3, NAME_X2, y+18 ) )
		{
			selNation = n;
			return 1;
		}

		if( (dir = detect_value(CASH_X, y, 110)) != 0 )
		{
			nationPtr->cash = (float) MAX( 0, MIN(CASH_MAX, ((int)nationPtr->cash/CASH_STEP + dir) * CASH_STEP) );
			return 1;
		}

		if( (dir = detect_value(FOOD_X, y, 110)) != 0 )
		{
			nationPtr->food = (float) MAX( 0, MIN(FOOD_MAX, ((int)nationPtr->food/FOOD_STEP + dir) * FOOD_STEP) );
			return 1;
		}

		if( (dir = detect_value(REPU_X, y, 110)) != 0 )
		{
			int repu = (int)nationPtr->reputation;
			repu = (repu >= 0 ? repu/REPU_STEP : -((-repu+REPU_STEP-1)/REPU_STEP)) + dir;
			nationPtr->reputation = (float) MAX( -100, MIN(100, repu*REPU_STEP) );
			return 1;
		}
	}

	//------- technologies -------//

	int y = DETAIL_Y1 + 40;

	for( int i=1 ; i<=tech_res.tech_count ; i++, y+=20 )
	{
		int dir = detect_value(TECH_X+154, y, 100);

		if( dir )
		{
			TechInfo* techInfo = tech_res[i];
			int level = techInfo->get_nation_tech_level(selNation) + dir;

			if( level >= 0 && level <= techInfo->max_tech_level )
				techInfo->set_nation_tech_level(selNation, level);

			return 1;
		}
	}

	if( mouse.any_click( TECH_X+6, y+4, TECH_X+126, y+23 ) )
	{
		for( int i=1 ; i<=tech_res.tech_count ; i++ )
			tech_res[i]->set_nation_tech_level(selNation, tech_res[i]->max_tech_level);
		return 1;
	}

	if( mouse.any_click( TECH_X+134, y+4, TECH_X+254, y+23 ) )
	{
		for( int i=1 ; i<=tech_res.tech_count ; i++ )
			tech_res[i]->set_nation_tech_level(selNation, 0);
		return 1;
	}

	//------- relations -------//

	y = DETAIL_Y1 + 40;

	for( int n=1 ; n<=nation_array.size() ; n++ )
	{
		if( nation_array.is_deleted(n) || n==selNation )
			continue;

		int clicked = 0;

		if( mouse.any_click( RELATION_X+156, y, RELATION_X+266, y+18, 0 ) )
			clicked = 1;
		else if( mouse.any_click( RELATION_X+156, y, RELATION_X+266, y+18, 1 ) )
			clicked = 2;

		if( clicked )
		{
			int value = relation_value(selNation, n);

			if( clicked==1 )		// left click: next status
				value = value==NATION_ALLIANCE ? NO_CONTACT : value+1;
			else						// right click: previous status
				value = value==NO_CONTACT ? NATION_ALLIANCE : value-1;

			set_relation_value(selNation, n, value);
			return 1;
		}

		y += 22;
	}

	return 0;
}
//----------- End of static function detect_kingdoms -----------//


//-------- Begin of static function relation_value --------//
//
// return: <int> NO_CONTACT or NATION_HOSTILE..NATION_ALLIANCE
//
static int relation_value(int nationRecno, int withNationRecno)
{
	NationRelation* relation = nation_array[nationRecno]->get_relation(withNationRecno);

	if( !relation->has_contact )
		return NO_CONTACT;

	return relation->status;
}
//----------- End of static function relation_value -----------//


//-------- Begin of static function set_relation_value --------//
//
static void set_relation_value(int nationRecno, int withNationRecno, int value)
{
	Nation* nationPtr = nation_array[nationRecno];
	Nation* withNation = nation_array[withNationRecno];

	if( value == NO_CONTACT )
	{
		nationPtr->get_relation(withNationRecno)->has_contact = 0;
		withNation->get_relation(nationRecno)->has_contact = 0;
		value = NATION_NEUTRAL;
	}
	else
		nationPtr->establish_contact(withNationRecno);

	//--- the status of both kingdoms (1 - recursive call: one side at a time) ---//

	nationPtr->set_relation_status(withNationRecno, value, 1);
	withNation->set_relation_status(nationRecno, value, 1);

	if( value == NATION_HOSTILE )
	{
		nationPtr->set_trade_treaty(withNationRecno, 0);
	}
}
//----------- End of static function set_relation_value -----------//


//-------- Begin of function ModEditor::edit_goals --------//
//
// The window of the goals of the scenario: the options of the Goals tab of
// the new game screen.
//
void ModEditor::edit_goals()
{
	enum { GOAL_MONSTER, GOAL_POPULATION, GOAL_ECONOMY, GOAL_SCORE, GOAL_YEAR, GOAL_COUNT };

	static const char* goalStr[GOAL_COUNT] =
	{
		N_("Destroy all Fryhtan lairs"),
		N_("Achieve a population of"),
		N_("Achieve an economic score of"),
		N_("Achieve a total score of"),
		N_("Achieve the goals within (years)"),
	};

	char* flagPtr[GOAL_COUNT] = { &config.goal_destroy_monster, &config.goal_population_flag,
		&config.goal_economic_score_flag, &config.goal_total_score_flag, &config.goal_year_limit_flag };

	int* valuePtr[GOAL_COUNT] = { NULL, &config.goal_population, &config.goal_economic_score,
		&config.goal_total_score, &config.goal_year_limit };

	power.win_opened = 1;

	int useBackBuf = vga.use_back_buf;
	char* viewBitmap = save_view();

	int refresh = 1;

	enum { GOAL_Y1 = WIN_Y1+80, GOAL_HEIGHT = 30, CHECK_X = WIN_X1+20, VALUE_X = WIN_X1+330 };

	while( 1 )
	{
		sys.yield();
		vga.flip();
		mouse.get_event();

		if( sys.signal_exit_flag == 1 )
			break;

		if( mod_editor.detect_bar_switch() )		// another button of the editor bar (or the same one: close)
			break;

		//------- display -------//

		if( refresh )
		{
			rest_view(viewBitmap);
			vga.use_back();

			put_window( _("Goals") );

			font_san.put_paragraph( CHECK_X, WIN_Y1+34, WIN_X2-20, WIN_Y1+76,
				_("The player always wins by defeating all the other kingdoms. With other goals, the player also wins by achieving all the goals chosen.") );

			for( int i=0 ; i<GOAL_COUNT ; i++ )
			{
				int y = GOAL_Y1 + i*GOAL_HEIGHT;

				editor_put_button( CHECK_X, y, CHECK_X+17, y+17, *flagPtr[i] ? "X" : "", 1, *flagPtr[i] );
				font_san.put( CHECK_X+26, y+3, _(goalStr[i]) );

				if( valuePtr[i] )
					put_value( VALUE_X, y, 110, misc.format(*valuePtr[i]) );
			}

			put_close_button();

			vga.use_front();
			vga_util.blt_buf( WIN_X1, WIN_Y1, WIN_X2, WIN_Y2, 0 );

			refresh = 0;
		}

		//------- detect -------//

		if( detect_close_button() || mouse.key_code==KEY_ESC || mouse.any_click(RIGHT_BUTTON) )
			break;

		for( int i=0 ; i<GOAL_COUNT ; i++ )
		{
			int y = GOAL_Y1 + i*GOAL_HEIGHT;
			int dir;

			if( mouse.any_click( CHECK_X, y, CHECK_X+17, y+17 ) ||
				 mouse.any_click( CHECK_X+26, y, VALUE_X-10, y+17 ) )
			{
				*flagPtr[i] = !*flagPtr[i];
				refresh = 1;
				break;
			}

			if( valuePtr[i] && (dir = detect_value(VALUE_X, y, 110)) != 0 )
			{
				int& value = *valuePtr[i];

				switch( i )		// the same steps and limits as the new game screen
				{
					case GOAL_POPULATION:
					case GOAL_ECONOMY:
						value = MAX( 100, MIN(5000, value + dir*100) );
						break;

					case GOAL_SCORE:
						if( dir > 0 )
							value += value >= 2000 ? 500 : 100;
						else
							value -= value > 2000 ? 500 : 100;

						value = MAX( 100, MIN(10000, value) );
						break;

					case GOAL_YEAR:
						if( dir > 0 )
							value += value >= 20 ? 5 : 1;
						else
							value -= value > 20 ? 5 : 1;

						value = MAX( 1, MIN(100, value) );
						break;
				}

				refresh = 1;
				break;
			}
		}
	}

	//------- the deadline, as when a scenario starts -------//

	info.goal_deadline = date.julian( date.year(info.game_date)+config.goal_year_limit,
								date.month(info.game_date), date.day(info.game_date) );

	mem_del(viewBitmap);

	power.win_opened = 0;

	if( useBackBuf )
		vga.use_back();
	else
		vga.use_front();

	redraw_screen();
}
//--------- End of function ModEditor::edit_goals ---------//


//-------- Begin of static function save_view --------//
//
// Keep a copy of the map view (on the back buffer) under the window.
//
static char* save_view()
{
	char* viewBitmap = (char*) mem_add( 2*sizeof(short) + (WIN_X2-WIN_X1+1)*(WIN_Y2-WIN_Y1+1) );

	vga_back.read_bitmap( WIN_X1, WIN_Y1, WIN_X2, WIN_Y2, viewBitmap );

	return viewBitmap;
}
//----------- End of static function save_view -----------//


//-------- Begin of static function rest_view --------//
//
static void rest_view(char* viewBitmap)
{
	vga_back.put_bitmap( WIN_X1, WIN_Y1, viewBitmap );
}
//----------- End of static function rest_view -----------//


//-------- Begin of static function put_window --------//
//
static void put_window(const char* title)
{
	vga_util.d3_panel_up( WIN_X1, WIN_Y1, WIN_X2, WIN_Y2 );
	vga_util.d3_panel_up( WIN_X1, WIN_Y1, WIN_X2, WIN_Y2 );		// twice: less translucent

	font_san.center_put( WIN_X1, WIN_Y1+6, WIN_X2, WIN_Y1+22, title );
}
//----------- End of static function put_window -----------//


//-------- Begin of static function put_close_button --------//
//
static void put_close_button()
{
	int x = (WIN_X1+WIN_X2)/2;

	editor_put_button( x-50, WIN_Y2-30, x+50, WIN_Y2-8, _("Close"), 1, 0 );
}
//----------- End of static function put_close_button -----------//


//-------- Begin of static function detect_close_button --------//
//
static int detect_close_button()
{
	int x = (WIN_X1+WIN_X2)/2;

	return mouse.any_click( x-50, WIN_Y2-30, x+50, WIN_Y2-8 );
}
//----------- End of static function detect_close_button -----------//


//-------- Begin of static function put_value --------//
//
// "<", the value, ">" in the given width.
//
static void put_value(int x, int y, int width, const char* valueStr)
{
	editor_put_button( x, y, x+17, y+17, "<", 1, 0 );
	vga_util.d3_panel_down( x+20, y, x+width-21, y+17 );
	font_san.center_put( x+20, y, x+width-21, y+17, valueStr );
	editor_put_button( x+width-18, y, x+width-1, y+17, ">", 1, 0 );
}
//----------- End of static function put_value -----------//


//-------- Begin of static function detect_value --------//
//
// return: <int> -1 - "<", 1 - ">", 0 - none
//
static int detect_value(int x, int y, int width)
{
	if( mouse.any_click( x, y, x+17, y+17 ) )
		return -1;

	if( mouse.any_click( x+width-18, y, x+width-1, y+17 ) )
		return 1;

	return 0;
}
//----------- End of static function detect_value -----------//


//------- the New Map window --------//

enum { NM_OPT_RACE, NM_OPT_COLOR, NM_OPT_AI_COUNT, NM_OPT_DIFFICULTY, NM_OPT_TERRAIN, NM_OPT_LAND,
		 NM_LEFT_COUNT,
		 NM_OPT_EXPLORE = NM_LEFT_COUNT, NM_OPT_FOG, NM_OPT_CASH, NM_OPT_AI_CASH, NM_OPT_AGGRESSIVE,
		 NM_OPT_MONSTER, NM_OPT_RANDOM_START, NM_OPT_RAW, NM_OPT_NEAR_RAW, NM_OPT_TOWNS,
		 NM_OPT_TOWN_RESIST, NM_OPT_TOWN_EMERGE, NM_OPT_NATION_EMERGE, NM_OPT_EVENTS, NM_OPT_SEAT,
		 NM_OPT_COUNT };

enum { NM_ROW_Y1 = WIN_Y1+36,
		 NM_ROW_HEIGHT = 24,
		 NM_LEFT_X = WIN_X1+10,
		 NM_LEFT_VALUE_X = WIN_X1+144,
		 NM_LEFT_VALUE_W = 122,
		 NM_RIGHT_X = WIN_X1+274,
		 NM_RIGHT_VALUE_X = WIN_X2-120,
		 NM_RIGHT_VALUE_W = 114,
		 NM_NAME_Y = NM_ROW_Y1 + (NM_LEFT_COUNT+1)*NM_ROW_HEIGHT,
		 NM_MAP_ID_Y = NM_NAME_Y + NM_ROW_HEIGHT,
		 NM_RANDOM_Y = NM_MAP_ID_Y + NM_ROW_HEIGHT,
		 NM_RATING_Y = NM_RANDOM_Y + NM_ROW_HEIGHT + 12,
		 NM_MAP_ID_LEN = 11 };			// enough for a dword in decimal

struct NewMapOption
{
	const char* label;
	char			value_count;		// the number of values
	char			is_number;			// 1 - the value is displayed as a number: min_value + index
	char			min_value;
	char			set_custom;			// 1 - changing it changes the difficulty to Custom, as in the new game screen
	const char* value_str[6];
};

static NewMapOption new_map_option[NM_OPT_COUNT] =
{
	{ N_("Nationality"), MAX_RACE, 0, 0, 0, { NULL } },
	{ N_("Your Color"), MAX_COLOR_SCHEME, 0, 0, 0, { NULL } },
	{ N_("Computer Kingdoms"), MAX_NATION-1, 1, 1, 1, { NULL } },
	{ N_("Difficulty"), 6, 0, 0, 0, { N_("Level I"), N_("Level II"), N_("Level III"), N_("Level IV"), N_("Level V"), N_("Custom") } },
	{ N_("Terrain"), 3, 0, 0, 0, { N_("Standard"), N_("Snow"), N_("Badlands") } },
	{ N_("Land Mass"), 3, 0, 0, 0, { N_("Small"), N_("Medium"), N_("Large") } },

	{ N_("World Map"), 2, 0, 0, 1, { N_("Explored"), N_("Unexplored") } },
	{ N_("Fog of War"), 2, 0, 0, 1, { N_("On"), N_("Off") } },
	{ N_("Human's Treasure"), 4, 0, 0, 1, { N_("Small"), N_("Medium"), N_("Large"), N_("Very Large") } },
	{ N_("Computer's Treasure"), 4, 0, 0, 1, { N_("Small"), N_("Medium"), N_("Large"), N_("Very Large") } },
	{ N_("AI Aggressiveness"), 4, 0, 0, 1, { N_("Low"), N_("Medium"), N_("High"), N_("Very High") } },
	{ N_("Fryhtans"), 3, 0, 0, 1, { N_("None"), N_("Defensive"), N_("Offensive") } },
	{ N_("Random Kingdoms"), 2, 0, 0, 1, { N_("Yes"), N_("No") } },
	{ N_("Resources at Start"), MAX_RAW_RES_SITE-MIN_RAW_RES_SITE+1, 1, MIN_RAW_RES_SITE, 1, { NULL } },
	{ N_("Resources Nearby?"), 2, 0, 0, 1, { N_("Yes"), N_("No") } },
	{ N_("Independent Villages"), 3, 0, 0, 1, { N_("Few"), N_("More"), N_("Many") } },
	{ N_("Village Resistance"), 3, 0, 0, 1, { N_("Low"), N_("Medium"), N_("High") } },
	{ N_("New Villages Emerge?"), 2, 0, 0, 1, { N_("Yes"), N_("No") } },
	{ N_("New Kingdoms Emerge?"), 2, 0, 0, 1, { N_("Yes"), N_("No") } },
	{ N_("Random Events"), 4, 0, 0, 1, { N_("Never"), N_("Seldom"), N_("Occasional"), N_("Frequent") } },
	{ N_("Seats of Power"), 2, 0, 0, 0, { N_("Unlocked"), N_("Locked") } },
};

static char new_map_race_table[MAX_RACE] =		// the order of the races in the new game screen
#if(MAX_RACE == 10)
{
	RACE_CHINESE, RACE_EGYPTIAN, RACE_GREEK, RACE_JAPANESE, RACE_MAYA,
	RACE_INDIAN, RACE_NORMAN, RACE_PERSIAN, RACE_VIKING, RACE_ZULU
};
#else
{
	RACE_CHINESE, RACE_GREEK, RACE_JAPANESE, RACE_MAYA,
	RACE_PERSIAN, RACE_NORMAN, RACE_VIKING
};
#endif

static short new_map_town_count[3] = { 7, 15, 30 };		// Few, More, Many: as in the new game screen
static short new_map_latitude[3] = { 45, 70, 20 };

static int	 new_map_get(Config& cfg, int optionId);
static void	 new_map_set(Config& cfg, int optionId, int index);
static void	 new_map_option_pos(int optionId, int& labelX, int& valueX, int& valueWidth, int& y);
static int	 new_map_random_seed();


//-------- Begin of function ModEditor::edit_new_map --------//
//
// The window of a new map: the options of the new game screen and the map
// I.D. "Create" ends the main loop and Game::editor_new_map() generates the
// new map (new_map_flag). The goals are kept: they are in the Goals window.
//
void ModEditor::edit_new_map()
{
	Config tempConfig = config;

	if( tempConfig.ai_nation_count < 1 )		// as in the new game screen
		tempConfig.ai_nation_count = MAX_NATION-1;

	char mapIdStr[NM_MAP_ID_LEN+1];
	snprintf( mapIdStr, sizeof(mapIdStr), "%d", new_map_random_seed() );

	power.win_opened = 1;

	int useBackBuf = vga.use_back_buf;
	char* viewBitmap = save_view();

	GetAGroup getGroup(2);
	int fieldsInited = 0;
	int refresh = 1, rc = 0;

	int centerX = (WIN_X1+WIN_X2)/2;

	while( 1 )
	{
		sys.yield();
		vga.flip();
		mouse.get_event();

		if( sys.signal_exit_flag == 1 )
			break;

		if( mod_editor.detect_bar_switch() )		// another button of the editor bar (or the same one: close)
			break;

		//------- display -------//

		if( refresh )
		{
			rest_view(viewBitmap);
			vga.use_back();

			put_window( _("New Map") );

			for( int i=0 ; i<NM_OPT_COUNT ; i++ )
			{
				NewMapOption* opt = new_map_option+i;
				int labelX, valueX, valueWidth, y;

				new_map_option_pos(i, labelX, valueX, valueWidth, y);

				font_san.put( labelX, y+2, _(opt->label), 0, valueX-6 );

				int index = new_map_get(tempConfig, i);
				String str;

				if( i == NM_OPT_RACE )
					str = race_res[ new_map_race_table[index] ]->name;
				else if( opt->is_number )
					str = misc.format( opt->min_value + index );
				else if( opt->value_str[0] )
					str = _(opt->value_str[index]);

				put_value( valueX, y, valueWidth, i==NM_OPT_COLOR ? "" : (char*)str );

				if( i == NM_OPT_COLOR )
				{
					vga_back.bar( valueX+26, y+3, valueX+valueWidth-27, y+14,
						game.color_remap_array[index+1].main_color );
				}
			}

			//------- name and map I.D. -------//

			font_san.put( NM_LEFT_X, NM_NAME_Y+2, _("Your Name"), 0, NM_LEFT_VALUE_X-6 );
			vga_util.d3_panel_down( NM_LEFT_VALUE_X, NM_NAME_Y, NM_LEFT_VALUE_X+NM_LEFT_VALUE_W-1, NM_NAME_Y+17 );

			font_san.put( NM_LEFT_X, NM_MAP_ID_Y+2, _("Map I.D."), 0, NM_LEFT_VALUE_X-6 );
			vga_util.d3_panel_down( NM_LEFT_VALUE_X, NM_MAP_ID_Y, NM_LEFT_VALUE_X+NM_LEFT_VALUE_W-1, NM_MAP_ID_Y+17 );

			editor_put_button( NM_LEFT_VALUE_X, NM_RANDOM_Y, NM_LEFT_VALUE_X+NM_LEFT_VALUE_W-1, NM_RANDOM_Y+17,
				_("Random Map I.D."), 1, 0 );

			//------- the difficulty rating -------//

			String str;
			str  = _("Difficulty Rating");
			str += ": ";
			str += misc.format( tempConfig.single_player_difficulty() );
			font_san.put( NM_LEFT_X, NM_RATING_Y, str );

			font_san.put_paragraph( NM_LEFT_X, NM_RATING_Y+24, NM_RIGHT_X-14, WIN_Y2-40,
				_("The goals are in the Goals window. Create: a new map with these options. The scenario being edited will be lost.") );

			//------- Create and Cancel -------//

			editor_put_button( centerX-110, WIN_Y2-30, centerX-10, WIN_Y2-8, _("Create"), 1, 0 );
			editor_put_button( centerX+10, WIN_Y2-30, centerX+110, WIN_Y2-8, _("Cancel"), 1, 0 );

			vga.use_front();
			vga_util.blt_buf( WIN_X1, WIN_Y1, WIN_X2, WIN_Y2, 0 );

			//--- the text fields ---//

			if( !fieldsInited )
			{
				getGroup[0].init( NM_LEFT_VALUE_X+4, NM_NAME_Y+1, NM_LEFT_VALUE_X+NM_LEFT_VALUE_W-5,
					tempConfig.player_name, HUMAN_NAME_LEN, &font_san, 0, 0 );
				getGroup[1].init( NM_LEFT_VALUE_X+4, NM_MAP_ID_Y+1, NM_LEFT_VALUE_X+NM_LEFT_VALUE_W-5,
					mapIdStr, NM_MAP_ID_LEN, &font_san, 0, 0 );
				getGroup.set_focus(1, 0);
				fieldsInited = 1;		// the background of the fields is on the back buffer
			}

			refresh = 0;
		}

		vga.use_front();
		getGroup.paint();

		//------- detect -------//

		getGroup.detect();

		if( mouse.any_click( centerX+10, WIN_Y2-30, centerX+110, WIN_Y2-8 ) || mouse.key_code==KEY_ESC )
			break;

		if( mouse.any_click( centerX-110, WIN_Y2-30, centerX-10, WIN_Y2-8 ) )
		{
			if( !box.ask( _("Create a new map? Unsaved changes will be lost."), _("Yes"), _("No") ) )
				continue;

			rc = 1;
			break;
		}

		if( mouse.any_click( NM_LEFT_VALUE_X, NM_RANDOM_Y, NM_LEFT_VALUE_X+NM_LEFT_VALUE_W-1, NM_RANDOM_Y+17 ) )
		{
			snprintf( mapIdStr, sizeof(mapIdStr), "%d", new_map_random_seed() );
			getGroup[1].init( NM_LEFT_VALUE_X+4, NM_MAP_ID_Y+1, NM_LEFT_VALUE_X+NM_LEFT_VALUE_W-5,
				mapIdStr, NM_MAP_ID_LEN, &font_san, 0, 0 );
			getGroup.set_focus(1, 0);
			continue;
		}

		for( int i=0 ; i<NM_OPT_COUNT ; i++ )
		{
			int labelX, valueX, valueWidth, y;

			new_map_option_pos(i, labelX, valueX, valueWidth, y);

			int dir = detect_value(valueX, y, valueWidth);

			if( !dir )
				continue;

			NewMapOption* opt = new_map_option+i;
			int index = new_map_get(tempConfig, i) + dir;

			if( opt->is_number )		// numbers: limited, the others: cycle
				index = MAX( 0, MIN(opt->value_count-1, index) );
			else
				index = (index + opt->value_count) % opt->value_count;

			new_map_set(tempConfig, i, index);

			if( opt->set_custom )
				tempConfig.difficulty_level = OPTION_CUSTOM;

			refresh = 1;
			break;
		}
	}

	mem_del(viewBitmap);

	power.win_opened = 0;

	if( useBackBuf )
		vga.use_back();
	else
		vga.use_front();

	if( !rc )
	{
		redraw_screen();
		return;
	}

	//------- create: the main loop ends, Game::editor_new_map() generates the map -------//

	tempConfig.difficulty_rating = tempConfig.single_player_difficulty();
	config = tempConfig;
	config.save("CONFIG.DAT");

	new_map_seed = atol(mapIdStr);		// 0 (not a number) - random
	new_map_flag = 1;

	sys.signal_exit_flag = 2;
}
//--------- End of function ModEditor::edit_new_map ---------//


//-------- Begin of static function new_map_option_pos --------//
//
static void new_map_option_pos(int optionId, int& labelX, int& valueX, int& valueWidth, int& y)
{
	if( optionId < NM_LEFT_COUNT )
	{
		labelX = NM_LEFT_X;
		valueX = NM_LEFT_VALUE_X;
		valueWidth = NM_LEFT_VALUE_W;
		y = NM_ROW_Y1 + optionId*NM_ROW_HEIGHT;
	}
	else
	{
		labelX = NM_RIGHT_X;
		valueX = NM_RIGHT_VALUE_X;
		valueWidth = NM_RIGHT_VALUE_W;
		y = NM_ROW_Y1 + (optionId-NM_LEFT_COUNT)*NM_ROW_HEIGHT;
	}
}
//----------- End of static function new_map_option_pos -----------//


//-------- Begin of static function new_map_get --------//
//
// return: <int> the index of the value of the option (0 to value_count-1)
//
static int new_map_get(Config& cfg, int optionId)
{
	switch( optionId )
	{
		case NM_OPT_RACE:
			for( int i=0 ; i<MAX_RACE ; i++ )
			{
				if( new_map_race_table[i] == cfg.race_id )
					return i;
			}
			return 0;

		case NM_OPT_COLOR:			return MAX( 0, cfg.player_nation_color-1 );
		case NM_OPT_AI_COUNT:		return cfg.ai_nation_count-1;
		case NM_OPT_DIFFICULTY:		return cfg.difficulty_level;
		case NM_OPT_TERRAIN:			return cfg.terrain_set-1;
		case NM_OPT_LAND:				return cfg.land_mass-1;
		case NM_OPT_EXPLORE:			return cfg.explore_whole_map ? 0 : 1;
		case NM_OPT_FOG:				return cfg.fog_of_war ? 0 : 1;
		case NM_OPT_CASH:				return cfg.start_up_cash-1;
		case NM_OPT_AI_CASH:			return cfg.ai_start_up_cash-1;
		case NM_OPT_AGGRESSIVE:		return cfg.ai_aggressiveness-1;
		case NM_OPT_MONSTER:			return cfg.monster_type;
		case NM_OPT_RANDOM_START:	return cfg.random_start_up ? 0 : 1;
		case NM_OPT_RAW:				return cfg.start_up_raw_site - MIN_RAW_RES_SITE;
		case NM_OPT_NEAR_RAW:		return cfg.start_up_has_mine_nearby ? 0 : 1;
		case NM_OPT_TOWNS:			return cfg.start_up_independent_town >= 30 ? 2 : cfg.start_up_independent_town <= 7 ? 0 : 1;
		case NM_OPT_TOWN_RESIST:	return cfg.independent_town_resistance-1;
		case NM_OPT_TOWN_EMERGE:	return cfg.new_independent_town_emerge ? 0 : 1;
		case NM_OPT_NATION_EMERGE:	return cfg.new_nation_emerge ? 0 : 1;
		case NM_OPT_EVENTS:			return cfg.random_event_frequency;
		case NM_OPT_SEAT:				return cfg.seat_of_power_unlocked ? 0 : 1;
	}

	return 0;
}
//----------- End of static function new_map_get -----------//


//-------- Begin of static function new_map_set --------//
//
static void new_map_set(Config& cfg, int optionId, int index)
{
	switch( optionId )
	{
		case NM_OPT_RACE:				cfg.race_id = new_map_race_table[index]; break;
		case NM_OPT_COLOR:			cfg.player_nation_color = index+1; break;
		case NM_OPT_AI_COUNT:		cfg.ai_nation_count = index+1; break;

		case NM_OPT_DIFFICULTY:
			if( index == OPTION_CUSTOM )
				cfg.difficulty_level = OPTION_CUSTOM;
			else
				cfg.change_difficulty(index);
			break;

		case NM_OPT_TERRAIN:
			cfg.terrain_set = index+1;
			cfg.latitude = new_map_latitude[index];
			break;

		case NM_OPT_LAND:				cfg.land_mass = index+1; break;
		case NM_OPT_EXPLORE:			cfg.explore_whole_map = !index; break;
		case NM_OPT_FOG:				cfg.fog_of_war = !index; break;
		case NM_OPT_CASH:				cfg.start_up_cash = index+1; break;
		case NM_OPT_AI_CASH:			cfg.ai_start_up_cash = index+1; break;
		case NM_OPT_AGGRESSIVE:		cfg.ai_aggressiveness = index+1; break;
		case NM_OPT_MONSTER:			cfg.monster_type = index; break;
		case NM_OPT_RANDOM_START:	cfg.random_start_up = !index; break;
		case NM_OPT_RAW:				cfg.start_up_raw_site = MIN_RAW_RES_SITE + index; break;
		case NM_OPT_NEAR_RAW:		cfg.start_up_has_mine_nearby = !index; break;
		case NM_OPT_TOWNS:			cfg.start_up_independent_town = new_map_town_count[index]; break;
		case NM_OPT_TOWN_RESIST:	cfg.independent_town_resistance = index+1; break;
		case NM_OPT_TOWN_EMERGE:	cfg.new_independent_town_emerge = !index; break;
		case NM_OPT_NATION_EMERGE:	cfg.new_nation_emerge = !index; break;
		case NM_OPT_EVENTS:			cfg.random_event_frequency = index; break;
		case NM_OPT_SEAT:				cfg.seat_of_power_unlocked = !index; break;
	}
}
//----------- End of static function new_map_set -----------//


//-------- Begin of static function new_map_random_seed --------//
//
// A random map I.D., different each time (Info::init_random_seed(0) gives
// the same one within a second).
//
static int new_map_random_seed()
{
	static unsigned count = 0;

	unsigned seed = (unsigned) time(NULL);

	seed = seed << 4 | seed >> (32-4);		// as Info::init_random_seed()
	seed ^= (unsigned) misc.get_time() * 2654435761u + (++count) * 40503u;

	int randomSeed = (int) (seed & 0x7fffffff);

	return randomSeed ? randomSeed : 1;
}
//----------- End of static function new_map_random_seed -----------//
