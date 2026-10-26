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
//Description : 7kmod-editor: the windows Kingdoms and Goals (phase 3).
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
