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

//Filename    : ModEditor.cpp
//Description : 7kmod-editor: the scenario editor.
//
// Phase 0 (MVP): a new map is generated as in "New Game" and opened in the
// editor mode: the game is paused, the whole map is shown and the top bar and
// the side panel show the editor interface. "Test" plays the scenario from a
// saved copy; F12 or "Return to Editor" in the game menu load that copy
// again in the editor mode. See 7kmod/TODO-editor.md.

#include <ModEditor.h>
#include <OSYS.h>
#include <OGAME.h>
#include <OBATTLE.h>
#include <OCONFIG.h>
#include <ConfigAdv.h>
#include <OGFILE.h>
#include <OVGA.h>
#include <vga_util.h>
#include <OFONT.h>
#include <OMOUSE.h>
#include <OMOUSE2.h>
#include <KEY.h>
#include <OBOX.h>
#include <OHELP.h>
#include <OSTR.h>
#include <OPOWER.h>
#include <OINFO.h>
#include <OIMGRES.h>
#include <ONATION.h>
#include <OUNIT.h>
#include <OTOWN.h>
#include <OFIRM.h>
#include <OSITE.h>
#include <ORAWRES.h>
#include <OREMOTE.h>
#include <OWORLDMT.h>
#include <string.h>
#include "gettext.h"

//------- Define static vars --------//

ModEditor mod_editor;

char loc_reveal_all_flag=0;

//------- the top bar: 2 rows of 4 buttons over the report buttons --------//

enum { BAR_X1=2, BAR_Y1=2, BAR_X2=260, BAR_Y2=51 };
enum { BAR_BUTTON_HEIGHT=20 };

static short bar_button_x[EDITOR_BAR_COUNT] = { 5,  68, 131, 194,   5,  88 };
static short bar_button_y[EDITOR_BAR_COUNT] = { 5,   5,   5,   5,  28,  28 };
static short bar_button_w[EDITOR_BAR_COUNT] = { 62, 62,  62,  62,  80,  50 };

static const char* bar_button_str[EDITOR_BAR_COUNT] =
{
	N_("Scenario"), N_("Kingdoms"), N_("Goals"), N_("Save"), N_("Test (F12)"), N_("Exit")
};

static char bar_button_enabled[EDITOR_BAR_COUNT] = { 0, 0, 0, 1, 1, 1 };		// Phase 1: Save, Test and Exit

//------- the side panel --------//

enum { TAB_WIDTH=98, TAB_HEIGHT=20 };

static const char* tab_str[EDITOR_TAB_COUNT] =
{
	N_("Units"), N_("Towns"), N_("Buildings"), N_("Nature")
};

//------- Declare static functions --------//

static void	put_button(int x1, int y1, int x2, int y2, const char* str, int enabled, int pushed);
static void	disp_selected(int x1, int y1, int x2, int y2);


//-------- Begin of function ModEditor::ModEditor --------//
//
ModEditor::ModEditor()
{
	reset();
}
//--------- End of function ModEditor::ModEditor ---------//


//-------- Begin of function ModEditor::reset --------//
//
void ModEditor::reset()
{
	editing_flag = 0;
	testing_flag = 0;
	panel_tab = EDITOR_TAB_UNIT;
	test_frame_speed = 0;

	memset( scen_name, 0, sizeof(scen_name) );
	memset( scen_desc, 0, sizeof(scen_desc) );
	scen_difficulty = 1;
	scen_bonus = 50;
	scen_file_name[0] = 0;

	loc_reveal_all_flag = 0;
}
//--------- End of function ModEditor::reset ---------//


//-------- Begin of function ModEditor::enter_edit_mode --------//
//
// Pause the game, show the whole map and the editor interface.
//
void ModEditor::enter_edit_mode()
{
	editing_flag = 1;
	testing_flag = 0;

	//------ pause the game ------//

	if( config.frame_speed > 0 )
		test_frame_speed = config.frame_speed;
	else if( test_frame_speed <= 0 )
		test_frame_speed = config_adv.game_new_default_frame_speed;

	config.frame_speed = 0;
	sys.user_pause_flag = 1;			// Sys::unpause() doesn't unpause it when the window gets the focus

	//------ show the whole map without changing the explored areas ------//

	loc_reveal_all_flag = 1;

	//------ the normal view, no commands ------//

	if( sys.view_mode != MODE_NORMAL )
		sys.set_view_mode(MODE_NORMAL);

	power.command_id = 0;

	sys.map_need_redraw = 1;
	sys.zoom_need_redraw = 1;
	sys.need_redraw_flag = 1;
}
//--------- End of function ModEditor::enter_edit_mode ---------//


//-------- Begin of function ModEditor::start_test --------//
//
// Save a copy of the scenario and play it.
//
void ModEditor::start_test()
{
	if( !editing_flag )
		return;

	//----- the saved copy is the scenario as it would be played -----//

	loc_reveal_all_flag = 0;
	config.frame_speed = test_frame_speed;

	if( !game_file.save_game(EDITOR_TEST_FILE_NAME) )
	{
		config.frame_speed = 0;
		loc_reveal_all_flag = 1;
		box.msg( _("The scenario couldn't be saved for the test.") );
		return;
	}

	//------- play it -------//

	editing_flag = 0;
	testing_flag = 1;

	config.frame_speed = 0;
	sys.set_speed(test_frame_speed, COMMAND_AUTO);
	sys.user_pause_flag = 0;

	power.reset_selection();

	sys.map_need_redraw = 1;
	sys.zoom_need_redraw = 1;
	sys.need_redraw_flag = 1;
	sys.disp_frame();
	sys.disp_view_mode();
	info.disp();

	box.msg( _("Testing the scenario. Press F12 or choose \"Return to Editor\" in the game menu to return to the editor.") );
}
//--------- End of function ModEditor::start_test ---------//


//-------- Begin of function ModEditor::return_to_editor --------//
//
// Load the copy saved before the test and return to the editor mode.
//
void ModEditor::return_to_editor()
{
	if( !testing_flag )
		return;

	if( sys.view_mode != MODE_NORMAL )
		sys.set_view_mode(MODE_NORMAL);

	sys.signal_exit_flag = 2;		// for deinit functions to recognize that this is an end game deinitialization (as Sys::load_game())

	int rc = game_file.load_game(sys.dir_config, EDITOR_TEST_FILE_NAME);

	if( rc == 1 )
	{
		sys.signal_exit_flag = 0;

		enter_edit_mode();

		sys.disp_frame();
		sys.disp_view_mode();
		info.disp();
	}
	else if( rc == 0 )		// not loaded, continue the test
	{
		sys.signal_exit_flag = 0;
		box.msg( _("The copy of the scenario couldn't be loaded.") );
	}
	else						// partially loaded: return to the main menu
	{
		sys.signal_exit_flag = 2;
		box.msg( _("The copy of the scenario couldn't be loaded.") );
	}
}
//--------- End of function ModEditor::return_to_editor ---------//


//-------- Begin of function ModEditor::exit_editor --------//
//
void ModEditor::exit_editor()
{
	if( box.ask( _("Exit the editor? Unsaved changes will be lost."), _("Yes"), _("No") ) )
		sys.signal_exit_flag = 2;		// return to the main menu
}
//--------- End of function ModEditor::exit_editor ---------//


//-------- Begin of function ModEditor::disp_top_bar --------//
//
// Display the editor buttons over the report buttons. Called by
// Sys::disp_view_mode().
//
void ModEditor::disp_top_bar()
{
	int useBackBuf = vga.use_back_buf;

	vga.use_back();

	vga_util.d3_panel_up( BAR_X1, BAR_Y1, BAR_X2, BAR_Y2 );

	for( int i=0 ; i<EDITOR_BAR_COUNT ; i++ )
	{
		put_button( bar_button_x[i], bar_button_y[i], bar_button_x[i]+bar_button_w[i]-1,
						bar_button_y[i]+BAR_BUTTON_HEIGHT-1, _(bar_button_str[i]), bar_button_enabled[i], 0 );
	}

	font_san.center_put( 140, 28, 256, 28+BAR_BUTTON_HEIGHT-1, _("Scenario Editor") );

	if( !useBackBuf )
		vga.use_front();

	vga_util.blt_buf( BAR_X1, BAR_Y1, BAR_X2, BAR_Y2, 0 );
}
//--------- End of function ModEditor::disp_top_bar ---------//


//-------- Begin of function ModEditor::detect_top_bar --------//
//
void ModEditor::detect_top_bar()
{
	//------ the help of the buttons (instead of the help of the report buttons) ------//

	static const char* helpStr[EDITOR_BAR_COUNT] =
	{
		N_("The name, description, difficulty and score bonus of the scenario."),
		N_("The kingdoms of the scenario: race, color, king, player or computer, treasure, food, technologies and relations."),
		N_("The goals of the scenario."),
		N_("Save the scenario."),
		N_("Play the scenario from a copy. Press F12 or choose \"Return to Editor\" in the game menu to return to the editor as it was before the test."),
		N_("Exit the editor and return to the main menu."),
	};

	help.set_custom_help( BAR_X1, BAR_Y1, BAR_X2, BAR_Y2, _("Scenario Editor") );

	for( int i=0 ; i<EDITOR_BAR_COUNT ; i++ )
	{
		String str;

		str = _(helpStr[i]);

		if( !bar_button_enabled[i] )
		{
			str += " ";
			str += _("It will be available in a next phase of the editor.");
		}

		help.set_custom_help( bar_button_x[i], bar_button_y[i], bar_button_x[i]+bar_button_w[i]-1,
									 bar_button_y[i]+BAR_BUTTON_HEIGHT-1, _(bar_button_str[i]), str );
	}

	//------ detect the buttons ------//

	for( int i=0 ; i<EDITOR_BAR_COUNT ; i++ )
	{
		if( !bar_button_enabled[i] )
			continue;

		if( !mouse.single_click( bar_button_x[i], bar_button_y[i], bar_button_x[i]+bar_button_w[i]-1,
										 bar_button_y[i]+BAR_BUTTON_HEIGHT-1 ) )
		{
			continue;
		}

		switch( i )
		{
			case EDITOR_BAR_SAVE:
				save_scenario();
				break;

			case EDITOR_BAR_TEST:
				start_test();
				break;

			case EDITOR_BAR_EXIT:
				exit_editor();
				break;
		}

		return;
	}
}
//--------- End of function ModEditor::detect_top_bar ---------//


//-------- Begin of function ModEditor::disp_panel --------//
//
// Display the side panel of the editor. Called by Info::disp() and
// Info::update().
//
void ModEditor::disp_panel(int refreshFlag)
{
	if( refreshFlag != INFO_REPAINT )
		return;

	int useBackBuf = vga.use_back_buf;

	vga.use_back();

	vga_back.put_bitmap( INFO_X1, INFO_Y1, info.info_background_bitmap );

	int x=INFO_X1+4, y=INFO_Y1+4;

	//------- title -------//

	vga_util.d3_panel_up( INFO_X1, y, INFO_X2, y+20 );
	font_san.center_put( INFO_X1, y, INFO_X2, y+20, _("Scenario Editor") );
	y += 26;

	//------- tabs -------//

	for( int i=0 ; i<EDITOR_TAB_COUNT ; i++ )
	{
		int tabX = x + (i%2) * (TAB_WIDTH+2);
		int tabY = y + (i/2) * (TAB_HEIGHT+2);

		put_button( tabX, tabY, tabX+TAB_WIDTH-1, tabY+TAB_HEIGHT-1, _(tab_str[i]), 1, panel_tab==i );
	}

	y += 2*(TAB_HEIGHT+2) + 4;

	//------- the palette (next phase) -------//

	int paletteY2 = INFO_Y2-76;

	vga_util.d3_panel_down( INFO_X1, y, INFO_X2, paletteY2 );

	static const char* paletteStr[EDITOR_TAB_COUNT] =
	{
		N_("Placing units (soldiers, weapons, ships, kings, generals and spies) will be available in the next phase of the editor."),
		N_("Placing towns (race and population) will be available in the next phase of the editor."),
		N_("Placing buildings (type, kingdom, finished or not) will be available in the next phase of the editor."),
		N_("Placing natural resources and Fryhtan lairs will be available in the next phase of the editor."),
	};

	font_san.put_paragraph( INFO_X1+8, y+8, INFO_X2-8, paletteY2-6, _(paletteStr[(int)panel_tab]) );

	//------- the selected object -------//

	disp_selected( INFO_X1, paletteY2+4, INFO_X2, INFO_Y2 );

	if( !useBackBuf )
		vga.use_front();

	vga_util.blt_buf( INFO_X1, INFO_Y1, INFO_X2, INFO_Y2, 0 );
}
//--------- End of function ModEditor::disp_panel ---------//


//-------- Begin of function ModEditor::detect_panel --------//
//
void ModEditor::detect_panel()
{
	int x=INFO_X1+4, y=INFO_Y1+30;

	for( int i=0 ; i<EDITOR_TAB_COUNT ; i++ )
	{
		int tabX = x + (i%2) * (TAB_WIDTH+2);
		int tabY = y + (i/2) * (TAB_HEIGHT+2);

		if( mouse.single_click( tabX, tabY, tabX+TAB_WIDTH-1, tabY+TAB_HEIGHT-1 ) )
		{
			if( panel_tab != i )
			{
				panel_tab = i;
				info.disp();
			}
			return;
		}
	}
}
//--------- End of function ModEditor::detect_panel ---------//


//-------- Begin of function ModEditor::detect_view --------//
//
// Detect the clicks of the editor mode on the map. Called by
// Sys::detect_view() instead of the commands of the normal game.
//
// return: <int> 1 - a click was used
//
int ModEditor::detect_view()
{
	//---- right click on the map: deselect ----//

	if( mouse.any_click(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2, RIGHT_BUTTON) )
	{
		power.reset_selection();
		info.disp();
		return 1;
	}

	return 0;
}
//--------- End of function ModEditor::detect_view ---------//


//-------- Begin of function ModEditor::process_key --------//
//
// In the editor mode only the keys of the editor work. During a test, F12
// returns to the editor.
//
// return: <int> 1 - the key was used by the editor (or ignored in the editor mode)
//
int ModEditor::process_key(unsigned scanCode, unsigned skeyState)
{
	if( !editing_flag && !testing_flag )
		return 0;

	int keyCode = mouse.is_key(scanCode, skeyState, (unsigned short) 0, K_UNIQUE_KEY);

	if( testing_flag )
	{
		if( keyCode == KEY_F12 )
		{
			return_to_editor();
			return 1;
		}
		return 0;
	}

	//------- editor mode -------//

	switch( keyCode )
	{
		case KEY_F12:
			start_test();
			break;

		case KEY_F11:
			sys.capture_screen();
			break;

		case KEY_ESC:
			power.reset_selection();
			info.disp();
			break;
	}

	return 1;
}
//--------- End of function ModEditor::process_key ---------//


//-------- Begin of function ModEditor::disp_menu_option --------//
//
// During a test, the "Training" option of the game menu is "Return to
// Editor": cover the text of the option with the texture of the button and
// write the new text.
//
// <int> x, y - the position of the option in the game menu
//
void ModEditor::disp_menu_option(int x, int y)
{
	VgaBuf* vgaBuf = Vga::active_buf;

	enum { CLEAR_X1=52, CLEAR_X2=117, CLEAR_Y1=4, CLEAR_Y2=29, SRC_X1=8, SRC_WIDTH=40 };

	for( int j=CLEAR_Y1 ; j<=CLEAR_Y2 ; j++ )
	{
		char* rowPtr = vgaBuf->buf_ptr(x, y+j);

		for( int i=CLEAR_X1 ; i<=CLEAR_X2 ; i++ )
			rowPtr[i] = rowPtr[SRC_X1 + (i-CLEAR_X1)%SRC_WIDTH];
	}

	const char* str = _("Return to Editor");

	font_bible.put( x + (172-font_bible.text_width(str))/2, y+8, str );
}
//--------- End of function ModEditor::disp_menu_option ---------//


//-------- Begin of static function put_button --------//
//
// Display a button of the editor on the back buffer.
//
static void put_button(int x1, int y1, int x2, int y2, const char* str, int enabled, int pushed)
{
	if( pushed )
		vga_util.d3_panel_down( x1, y1, x2, y2 );
	else
		vga_util.d3_panel_up( x1, y1, x2, y2 );

	font_san.center_put( x1, y1, x2, y2, str );

	if( !enabled )
		vga_back.adjust_brightness( x1+1, y1+1, x2-1, y2-1, -8 );
}
//----------- End of static function put_button -----------//


//-------- Begin of static function disp_selected --------//
//
// Display the name and the kingdom of the selected object.
//
static void disp_selected(int x1, int y1, int x2, int y2)
{
	vga_util.d3_panel_up( x1, y1, x2, y2 );

	int nationRecno = -1;		// -1: the object has no kingdom
	String str;

	if( firm_array.selected_recno )
	{
		Firm* firmPtr = firm_array[firm_array.selected_recno];
		str = firmPtr->firm_name();
		nationRecno = firmPtr->nation_recno;
	}
	else if( town_array.selected_recno )
	{
		Town* townPtr = town_array[town_array.selected_recno];
		str = townPtr->town_name();
		nationRecno = townPtr->nation_recno;
	}
	else if( site_array.selected_recno )
	{
		Site* sitePtr = site_array[site_array.selected_recno];

		switch( sitePtr->site_type )
		{
			case SITE_RAW:
				str = _(raw_res[sitePtr->object_id]->name);
				break;
			case SITE_SCROLL:
				str = _("Scroll of Power");
				break;
			case SITE_GOLD_COIN:
				str = _("Treasure");
				break;
		}
	}
	else if( unit_array.selected_recno )
	{
		Unit* unitPtr = unit_array[unit_array.selected_recno];
		str = unitPtr->unit_name();
		nationRecno = unitPtr->nation_recno;
	}

	font_san.put( x1+8, y1+6, _("Selected:") );

	if( !str[0] )
	{
		font_san.put_paragraph( x1+8, y1+22, x2-8, y2-4, _("Nothing. Click on the map to select; right click or Esc to deselect.") );
		return;
	}

	font_san.put( x1+8, y1+22, str, 0, x2-8 );

	if( nationRecno > 0 && !nation_array.is_deleted(nationRecno) )
	{
		Nation* nationPtr = nation_array[nationRecno];

		vga_back.bar( x1+8, y1+38, x1+20, y1+50, nationPtr->nation_color );
		font_san.put( x1+26, y1+38, nationPtr->nation_name(), 0, x2-8 );
	}
	else if( nationRecno == 0 )
	{
		font_san.put( x1+8, y1+38, _("Independent") );
	}
}
//----------- End of static function disp_selected -----------//
