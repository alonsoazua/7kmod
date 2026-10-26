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
		 EDITOR_BAR_TEST,
		 EDITOR_BAR_EXIT,
		 EDITOR_BAR_COUNT };

//------- the tabs of the side panel --------//

enum { EDITOR_TAB_UNIT,
		 EDITOR_TAB_TOWN,
		 EDITOR_TAB_FIRM,
		 EDITOR_TAB_NATURE,
		 EDITOR_TAB_COUNT };

#define EDITOR_TEST_FILE_NAME		"EDITOR.TMP"		// the copy of the scenario saved before testing it

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

public:
	ModEditor();

	void	reset();

	int	is_editing()		{ return editing_flag; }
	int	is_testing()		{ return testing_flag; }

	void	enter_edit_mode();
	void	start_test();
	void	return_to_editor();
	void	exit_editor();

	//--- interface ---//

	void	disp_top_bar();
	void	detect_top_bar();

	void	disp_panel(int refreshFlag);
	void	detect_panel();

	int	process_key(unsigned scanCode, unsigned skeyState);
	int	detect_view();

	//--- in-game menu during a test ---//

	void	disp_menu_option(int x, int y);
};

extern ModEditor mod_editor;
extern char loc_reveal_all_flag;		// 1 - Location::explored() and visibility() see the whole map (the editor mode)

#endif
