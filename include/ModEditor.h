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

//Filename    : ModEditor.h
//Description : 7kmod-editor: the scenario editor. See 7kmod/TODO-editor.md.

#ifndef __MODEDITOR_H
#define __MODEDITOR_H

#include <OBUTTON.h>

//------- the buttons of the top bar --------//

enum { EDITOR_BAR_SCENARIO,
		 EDITOR_BAR_KINGDOMS,
		 EDITOR_BAR_GOALS,
		 EDITOR_BAR_SAVE,
		 EDITOR_BAR_NEW,
		 EDITOR_BAR_TEST,
		 EDITOR_BAR_EXIT,
		 EDITOR_BAR_COUNT };

//------- the tabs of the side panel --------//

enum { EDITOR_TAB_UNIT,
		 EDITOR_TAB_WEAPON,			// weapons and ships
		 EDITOR_TAB_TOWN,
		 EDITOR_TAB_FIRM,
		 EDITOR_TAB_NATURE,
		 EDITOR_TAB_TERRAIN,			// trees, rocks and the ground (phase 5)
		 EDITOR_TAB_COUNT };

//------- the entries of the Terrain tab (ModEditorTerrain.cpp) --------//

enum { EDITOR_TERRAIN_TREE=1,
		 EDITOR_TERRAIN_ROCK,		// id: 0 - small, 1 - large
		 EDITOR_TERRAIN_CLEAR,
		 EDITOR_TERRAIN_GROUND };	// id: the terrain type

int editor_terrain_place(int placeType, int placeId, int xLoc, int yLoc, int brushSize);
int editor_terrain_area(int placeType, int placeId, int xLoc, int yLoc, int brushSize, int& locWidth, int& locHeight);

#define EDITOR_TEST_FILE_NAME		"EDITOR.TMP"		// the copy of the scenario saved before testing it

//------- the scenarios saved by the editor --------//

#define EDITOR_SCENARIO_DIR_NAME	"SCENMOD"			// in the config directory of the game
#define EDITOR_SCENARIO_DIR_ID		2						// the id of the directory in Sys::dir_scenario_path[]

enum { EDITOR_NAME_LEN=40,				// the name of the scenario
		 EDITOR_DESC_LINES=9,			// the lines of the description
		 EDITOR_DESC_LEN=75,				// the characters of a line of the description
		 EDITOR_FILE_NAME_LEN=20,		// the file name of the scenario, without the extension
		 EDITOR_PICT_WIDTH=758,			// the picture of the scenario screen (.SCP)
		 EDITOR_PICT_HEIGHT=142 };

//------- Define class ModEditor --------//

class ModEditor
{
public:
	char	editing_flag;			// 1 - the game is in the editor mode: paused, the whole map shown, the editor interface
	char	testing_flag;			// 1 - the scenario is being tested: the normal game, F12 returns to the editor
	char	panel_tab;				// EDITOR_TAB_???
	short	test_frame_speed;		// the speed of the game when testing

	Button	button_bar[EDITOR_BAR_COUNT];
	Button	button_tab[EDITOR_TAB_COUNT];

	//--- the data of the scenario, for saving it (not saved in the game) ---//

	char	scen_name[EDITOR_NAME_LEN+1];
	char	scen_desc[EDITOR_DESC_LINES][EDITOR_DESC_LEN+1];
	short	scen_difficulty;
	short	scen_bonus;
	char	scen_file_name[EDITOR_FILE_NAME_LEN+1];		// the file name it was last saved with, without the extension

	//--- placing objects (phase 2) ---//

	char	owner_nation_recno;		// the kingdom of the objects placed, 0 - independent
	char	place_entry;				// the entry of the palette of the current tab being placed, -1 - none
	char	place_race_id;				// the race of soldiers, generals, towns and buildings
	short	place_combat_level;		// the combat level of soldiers and generals
	short	place_leadership;			// the leadership of soldiers and generals
	char	place_skill_id;			// the skill of workers: SKILL_CONSTRUCTION, SKILL_MINING, SKILL_MFT or SKILL_RESEARCH
	short	place_skill_level;		// the skill level of workers
	char	place_weapon_version;	// the version of weapons (1 to 3, at most the maximum of each weapon)
	short	place_population;			// the population of towns
	short	place_reserve;				// the reserve of natural resources, in 1000s
	short	place_monster_id;			// the Fryhtan of lairs
	char	place_brush;				// the size of the brush of the Terrain tab: 1, 3 or 5

	short	edit_scen_recno;
	char	pending_bar;				// EDITOR_BAR_??? pressed while a window was open, -1 - none			// the scenario selected for "Edit" in the scenario screen (Game::select_scenario())

	//--- a new map (the New button: Game::editor_new_map() generates it after the main loop ends) ---//

	char	new_map_flag;				// 1 - generate a new map with the new config after leaving the main loop
	int	new_map_seed;				// its map I.D.

public:
	ModEditor();

	void	reset();

	int	is_editing()		{ return editing_flag; }
	int	is_testing()		{ return testing_flag; }

	void	enter_edit_mode();
	void	start_test();
	void	return_to_editor();
	void	exit_editor();

	//--- saving the scenario ---//

	static void init_scenario_dir();
	void	save_scenario();

	//--- interface ---//

	void	disp_top_bar();
	void	detect_top_bar();
	void	run_bar_button(int buttonId);
	int	detect_bar_switch();

	void	disp_panel(int refreshFlag);
	void	detect_panel();

	int	process_key(unsigned scanCode, unsigned skeyState);
	int	detect_view();
	int	detect_place();
	int	get_place_area(int xLoc, int yLoc, int& locWidth, int& locHeight);		// for ZoomMatrix::draw_build_marker()

	//--- the selected object ---//

	void	delete_selected();
	void	set_selected_owner();
	void	order_selected(int xLoc, int yLoc);		// right click: move or put in a building or town
	void	take_out_all();
	void	take_out_general();
	void	load_scenario_info(const char* sctPath, int isEditorScenario, const char* fileName);

	//--- the windows (phase 3) ---//

	int	edit_scenario_info(int saveFlag);
	void	redraw_screen();
	void	edit_kingdoms();
	void	edit_goals();
	void	edit_new_map();

	//--- in-game menu during a test ---//

	void	disp_menu_option(int x, int y);
};

extern ModEditor mod_editor;

void editor_put_button(int x1, int y1, int x2, int y2, const char* str, int enabled, int pushed);		// on the back buffer
extern char loc_reveal_all_flag;		// 1 - Location::explored() and visibility() see the whole map (the editor mode)

#endif
