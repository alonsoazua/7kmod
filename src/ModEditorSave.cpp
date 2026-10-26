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

//Filename    : ModEditorSave.cpp
//Description : 7kmod-editor: saving the scenario (phase 1).
//
// The "Save" button of the editor asks the name, the difficulty, the score
// bonus and the description of the scenario and saves the three files of a
// scenario in the SCENMOD directory of the config directory:
//
//   X.SCN - the saved game
//   X.SCT - the text: name, "Difficulty:N", "Bonus:N", a separator line and
//           the description
//   X.SCP - the picture of the scenario screen: <short> width, <short> height
//           and the flat bitmap (VgaBuf::put_large_bitmap()), taken from the
//           middle of the map view
//
// The scenarios of SCENMOD appear in the scenario screen with the others.

#include <ModEditor.h>
#include <OSYS.h>
#include <OGAME.h>
#include <OCONFIG.h>
#include <OGFILE.h>
#include <OVGA.h>
#include <vga_util.h>
#include <OFONT.h>
#include <OMOUSE.h>
#include <KEY.h>
#include <OBOX.h>
#include <OBUTTON.h>
#include <OGETA.h>
#include <OPOWER.h>
#include <OINFO.h>
#include <OMISC.h>
#include <OSTR.h>
#include <OWORLD.h>
#include <OWORLDMT.h>
#include <FilePath.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "gettext.h"

//------- the save dialog --------//

enum { DLG_X1 = 30,
		 DLG_Y1 = 86,
		 DLG_X2 = 545,
		 DLG_Y2 = 548 };

enum { LABEL_X = DLG_X1+16,
		 FIELD_X = DLG_X1+120,
		 FIELD_X2 = DLG_X2-16,
		 LINE_HEIGHT = 22 };

enum { DIFFICULTY_MIN=1, DIFFICULTY_MAX=10,		// as the original scenarios
		 BONUS_MIN=0, BONUS_MAX=500, BONUS_STEP=50 };

//------- Declare static functions --------//

static void	make_file_name(char* fileName, const char* scenName);
static int	write_text_file(const char* path, ModEditor* editor);
static int	write_picture_file(const char* path);
static void	capture_zoom(int topXLoc, int topYLoc);


//-------- Begin of function ModEditor::init_scenario_dir --------//
//
// Set (and create) the directory of the scenarios of the editor:
// <config dir>/SCENMOD/. Called before reading the list of scenarios and
// before saving one.
//
void ModEditor::init_scenario_dir()
{
	char* dirPath = sys.dir_scenario_path[EDITOR_SCENARIO_DIR_ID];

	dirPath[0] = 0;

	if( !sys.dir_config[0] )
		return;

	FilePath path(sys.dir_config);
	path += EDITOR_SCENARIO_DIR_NAME;
	path += PATH_DELIM;

	if( path.error_flag || !misc.mkpath(path) )
		return;

	strcpy( dirPath, path );
}
//--------- End of function ModEditor::init_scenario_dir ---------//


//-------- Begin of function ModEditor::edit_scenario_info --------//
//
// The window of the data of the scenario: name, difficulty, score bonus and
// description. The "Scenario" button of the editor (saveFlag 0) and the
// "Save" button (saveFlag 1).
//
// return: <int> 1 - OK or Save, 0 - Cancel (the data are not changed)
//
int ModEditor::edit_scenario_info(int saveFlag)
{
	const char* dirPath = sys.dir_scenario_path[EDITOR_SCENARIO_DIR_ID];

	if( scen_difficulty < DIFFICULTY_MIN )
		scen_difficulty = DIFFICULTY_MIN;

	//------- keep the data for Cancel -------//

	char  oldName[EDITOR_NAME_LEN+1];
	char  oldDesc[EDITOR_DESC_LINES][EDITOR_DESC_LEN+1];
	short oldDifficulty = scen_difficulty, oldBonus = scen_bonus;

	memcpy( oldName, scen_name, sizeof(oldName) );
	memcpy( oldDesc, scen_desc, sizeof(oldDesc) );

	//------- the dialog -------//

	power.win_opened = 1;

	int useBackBuf = vga.use_back_buf;
	vga.use_front();

	Button buttonDiffDown, buttonDiffUp, buttonBonusDown, buttonBonusUp, buttonSave, buttonCancel;
	GetAGroup getGroup(1+EDITOR_DESC_LINES);

	int y = DLG_Y1+10;

	//--- the background: on the back buffer (the background of the text fields), then on the front ---//

	vga.use_back();

	vga_util.d3_panel_up( DLG_X1, DLG_Y1, DLG_X2, DLG_Y2 );
	vga_util.d3_panel_up( DLG_X1, DLG_Y1, DLG_X2, DLG_Y2 );		// twice: less translucent

	font_san.center_put( DLG_X1, y, DLG_X2, y+16, saveFlag ? _("Save Scenario") : _("Scenario") );
	y += 30;

	int nameY = y;
	font_san.put( LABEL_X, nameY, _("Name:") );
	vga_util.d3_panel_down( FIELD_X-4, nameY-3, FIELD_X2, nameY+font_san.max_font_height+2 );
	y += 30;

	int valueY = y;
	font_san.put( LABEL_X, valueY, _("Difficulty:") );
	font_san.put( DLG_X1+270, valueY, _("Score Bonus:") );
	y += 30;

	font_san.put( LABEL_X, y, _("Description:") );
	y += 18;

	int descY = y;

	for( int i=0 ; i<EDITOR_DESC_LINES ; i++ )
	{
		vga_util.d3_panel_down( LABEL_X-4, descY+i*LINE_HEIGHT-3, FIELD_X2, descY+i*LINE_HEIGHT+font_san.max_font_height+2 );
	}

	y = descY + EDITOR_DESC_LINES*LINE_HEIGHT + 10;

	int fileY = y;
	y += 22;

	if( saveFlag )
	{
		font_san.put_paragraph( LABEL_X, y, FIELD_X2, y+60,
			_("The picture of the scenario is the middle of the map view: move the view before saving to choose it. The scenario will appear in Single Player > Scenario.") );
	}

	vga.use_front();
	vga_util.blt_buf( DLG_X1, DLG_Y1, DLG_X2, DLG_Y2, 0 );

	//------- the controls -------//

	getGroup[0].init( FIELD_X, nameY, FIELD_X2-4, scen_name, EDITOR_NAME_LEN, &font_san, 0, 0 );

	for( int i=0 ; i<EDITOR_DESC_LINES ; i++ )
		getGroup[1+i].init( LABEL_X, descY+i*LINE_HEIGHT, FIELD_X2-4, scen_desc[i], EDITOR_DESC_LEN, &font_san, 0, 0 );

	getGroup.set_focus(0, 0);

	buttonDiffDown.create_text( FIELD_X+46, valueY-3, FIELD_X+66, valueY+15, "<" );
	buttonDiffUp.create_text( FIELD_X+70, valueY-3, FIELD_X+90, valueY+15, ">" );
	buttonBonusDown.create_text( DLG_X1+410, valueY-3, DLG_X1+430, valueY+15, "<" );
	buttonBonusUp.create_text( DLG_X1+434, valueY-3, DLG_X1+454, valueY+15, ">" );

	buttonSave.create_text( (DLG_X1+DLG_X2)/2-110, DLG_Y2-34, (DLG_X1+DLG_X2)/2-10, DLG_Y2-10, saveFlag ? _("Save") : _("OK") );
	buttonCancel.create_text( (DLG_X1+DLG_X2)/2+10, DLG_Y2-34, (DLG_X1+DLG_X2)/2+110, DLG_Y2-10, _("Cancel") );

	buttonDiffDown.paint();
	buttonDiffUp.paint();
	buttonBonusDown.paint();
	buttonBonusUp.paint();
	buttonSave.paint();
	buttonCancel.paint();

	//------- the loop -------//

	int rc = 0, refreshValues = 1;
	char fileName[EDITOR_FILE_NAME_LEN+1] = "";
	char lastFileName[EDITOR_FILE_NAME_LEN+1] = "\xff";		// force displaying the file name

	while( 1 )
	{
		sys.yield();
		vga.flip();
		mouse.get_event();

		if( sys.signal_exit_flag == 1 )
			break;

		//---- the values ----//

		if( refreshValues )
		{
			vga_front.d3_panel_down( FIELD_X-4, valueY-3, FIELD_X+40, valueY+15 );
			font_san.put( FIELD_X+2, valueY, scen_difficulty );

			vga_front.d3_panel_down( DLG_X1+364, valueY-3, DLG_X1+406, valueY+15 );
			font_san.put( DLG_X1+370, valueY, scen_bonus );

			refreshValues = 0;
		}

		//---- the file name ----//

		make_file_name( fileName, scen_name );

		if( strcmp(fileName, lastFileName) != 0 )
		{
			strcpy( lastFileName, fileName );

			String str;
			str  = _("File:");
			str += " ";
			str += EDITOR_SCENARIO_DIR_NAME;
			str += PATH_DELIM;
			str += fileName;
			str += ".SCN";

			vga_util.blt_buf( LABEL_X, fileY, FIELD_X2, fileY+font_san.max_font_height, 0 );		// restore the background
			font_san.put( LABEL_X, fileY, str, 0, FIELD_X2 );
		}

		getGroup.paint();

		//---- detect ----//

		getGroup.detect();

		if( buttonDiffDown.detect() && scen_difficulty > DIFFICULTY_MIN )
		{
			scen_difficulty--;
			refreshValues = 1;
		}
		else if( buttonDiffUp.detect() && scen_difficulty < DIFFICULTY_MAX )
		{
			scen_difficulty++;
			refreshValues = 1;
		}
		else if( buttonBonusDown.detect() && scen_bonus > BONUS_MIN )
		{
			scen_bonus -= BONUS_STEP;
			refreshValues = 1;
		}
		else if( buttonBonusUp.detect() && scen_bonus < BONUS_MAX )
		{
			scen_bonus += BONUS_STEP;
			refreshValues = 1;
		}
		else if( buttonSave.detect() )
		{
			if( !scen_name[0] )
			{
				box.msg( _("Please enter the name of the scenario.") );
				continue;
			}

			//--- an existing file, saved with another name: ask before replacing it ---//

			if( saveFlag && strcmp(fileName, scen_file_name) != 0 )
			{
				FilePath path(dirPath);
				path += fileName;
				path += ".SCN";

				if( misc.is_file_exist(path) )
				{
					String str;
					snprintf( str, MAX_STR_LEN+1, _("A scenario with the file name %s already exists. Replace it?"), fileName );

					if( !box.ask(str, _("Yes"), _("No")) )
						continue;
				}
			}

			rc = 1;
			break;
		}
		else if( buttonCancel.detect(KEY_ESC) )
		{
			break;
		}
	}

	power.win_opened = 0;

	if( !rc )		// Cancel: restore the data
	{
		memcpy( scen_name, oldName, sizeof(oldName) );
		memcpy( scen_desc, oldDesc, sizeof(oldDesc) );
		scen_difficulty = oldDifficulty;
		scen_bonus = oldBonus;
	}

	if( useBackBuf )
		vga.use_back();

	//------- redraw the screen -------//

	if( !rc || !saveFlag )
		redraw_screen();

	return rc;
}
//--------- End of function ModEditor::edit_scenario_info ---------//


//-------- Begin of function ModEditor::save_scenario --------//
//
void ModEditor::save_scenario()
{
	if( !editing_flag )
		return;

	init_scenario_dir();

	const char* dirPath = sys.dir_scenario_path[EDITOR_SCENARIO_DIR_ID];

	if( !dirPath[0] )
	{
		box.msg( _("The folder of the scenarios couldn't be created.") );
		return;
	}

	power.reset_selection();		// no selection marks in the picture

	if( !edit_scenario_info(1) )
		return;

	char fileName[EDITOR_FILE_NAME_LEN+1];
	make_file_name( fileName, scen_name );

	//------- save -------//

	{
		FilePath path(dirPath);
		path += fileName;

		String scnPath, sctPath, scpPath;
		scnPath  = (char*) path;	scnPath += ".SCN";
		sctPath  = (char*) path;	sctPath += ".SCT";
		scpPath  = (char*) path;	scpPath += ".SCP";

		//--- the picture first: it draws the map view on the back buffer ---//

		int pictOk = write_picture_file(scpPath);

		//--- the saved game: as it will be played ---//

		String gameFileName;
		gameFileName  = EDITOR_SCENARIO_DIR_NAME;
		gameFileName += PATH_DELIM;
		gameFileName += fileName;
		gameFileName += ".SCN";

		loc_reveal_all_flag = 0;
		config.frame_speed = test_frame_speed;

		int gameOk = game_file.save_game(gameFileName);

		config.frame_speed = 0;
		loc_reveal_all_flag = 1;

		int textOk = gameOk && write_text_file(sctPath, this);

		if( gameOk && textOk )
		{
			strcpy( scen_file_name, fileName );

			String str;
			snprintf( str, MAX_STR_LEN+1, _("The scenario has been saved as %s. It appears in Single Player > Scenario."), (char*) gameFileName );

			if( !pictOk )
			{
				str += " ";
				str += _("The picture couldn't be saved.");
			}

			box.msg( str );
		}
		else
		{
			box.msg( _("The scenario couldn't be saved.") );
		}
	}

	redraw_screen();
}
//--------- End of function ModEditor::save_scenario ---------//


//-------- Begin of function ModEditor::redraw_screen --------//
//
// Redraw the whole screen after a window of the editor.
//
void ModEditor::redraw_screen()
{
	sys.zoom_need_redraw = 1;
	sys.map_need_redraw = 1;
	sys.need_redraw_flag = 1;
	sys.disp_frame();
	sys.disp_view_mode();
	info.disp();
}
//--------- End of function ModEditor::redraw_screen ---------//


//-------- Begin of static function make_file_name --------//
//
// The file name of the scenario from its name: letters and numbers in
// uppercase, "-" for spaces, at most EDITOR_FILE_NAME_LEN characters.
// ("_" is not used: Font::put_paragraph() takes it as underline.)
//
static void make_file_name(char* fileName, const char* scenName)
{
	int len=0;

	for( const char* p=scenName ; *p && len<EDITOR_FILE_NAME_LEN ; p++ )
	{
		unsigned char c = (unsigned char) *p;

		if( c < 128 && isalnum(c) )
			fileName[len++] = toupper(c);

		else if( (c==' ' || c=='-' || c=='_') && len>0 && fileName[len-1] != '-' )
			fileName[len++] = '-';
	}

	while( len>0 && fileName[len-1]=='-' )
		len--;

	fileName[len] = 0;

	if( !len )
		strcpy( fileName, "SCENARIO" );
}
//----------- End of static function make_file_name -----------//


//-------- Begin of static function write_text_file --------//
//
static int write_text_file(const char* path, ModEditor* editor)
{
	FILE* filePtr = fopen(path, "wb");

	if( !filePtr )
		return 0;

	fprintf( filePtr, "%s\r\n", editor->scen_name );
	fprintf( filePtr, "Difficulty:%d\r\n", editor->scen_difficulty );
	fprintf( filePtr, "Bonus:%d\r\n", editor->scen_bonus );
	fprintf( filePtr, "------------------------------\r\n" );

	//--- the description, without the empty lines at the end ---//

	int lineCount = EDITOR_DESC_LINES;

	while( lineCount > 0 && !editor->scen_desc[lineCount-1][0] )
		lineCount--;

	for( int i=0 ; i<lineCount ; i++ )
		fprintf( filePtr, "%s\r\n", editor->scen_desc[i] );

	int ok = !ferror(filePtr);

	return fclose(filePtr)==0 && ok;
}
//----------- End of static function write_text_file -----------//


//-------- Begin of static function write_picture_file --------//
//
// The picture is the middle of the map view, 758 x 142. The view is 576
// pixels wide, so it is drawn twice: at the current view and 6 locations to
// the right (or to the left at the right edge of the map).
//
static int write_picture_file(const char* path)
{
	ZoomMatrix* zoomMatrix = world.zoom_matrix;

	int oldTopX = zoomMatrix->top_x_loc;
	int oldTopY = zoomMatrix->top_y_loc;

	enum { SHIFT_LOC = 6 };		// 6*32 = 192 pixels > 758-576

	int maxTopX = MAX_WORLD_X_LOC - zoomMatrix->disp_x_loc;
	int leftX = MIN( oldTopX, maxTopX-SHIFT_LOC );

	if( leftX < 0 )
		leftX = 0;

	int pictY1 = ZOOM_Y1 + (ZOOM_HEIGHT-EDITOR_PICT_HEIGHT)/2;
	int rightWidth = EDITOR_PICT_WIDTH - ZOOM_WIDTH;

	char* pictBuf = (char*) mem_add( EDITOR_PICT_WIDTH * EDITOR_PICT_HEIGHT );

	//------- the left part: the view at leftX -------//

	capture_zoom( leftX, oldTopY );

	for( int y=0 ; y<EDITOR_PICT_HEIGHT ; y++ )
		memcpy( pictBuf + y*EDITOR_PICT_WIDTH, vga_back.buf_ptr(ZOOM_X1, pictY1+y), ZOOM_WIDTH );

	//------- the right part: the view at leftX+SHIFT_LOC -------//

	capture_zoom( leftX+SHIFT_LOC, oldTopY );

	int srcX = ZOOM_X1 + ZOOM_WIDTH - SHIFT_LOC*ZOOM_LOC_WIDTH;		// the column of the first pixel after the left part

	for( int y=0 ; y<EDITOR_PICT_HEIGHT ; y++ )
		memcpy( pictBuf + y*EDITOR_PICT_WIDTH + ZOOM_WIDTH, vga_back.buf_ptr(srcX, pictY1+y), rightWidth );

	//------- restore the view -------//

	zoomMatrix->top_x_loc = oldTopX;
	zoomMatrix->top_y_loc = oldTopY;
	sys.zoom_need_redraw = 1;

	//------- write the file -------//

	int ok = 0;
	FILE* filePtr = fopen(path, "wb");

	if( filePtr )
	{
		int16_t size[2] = { EDITOR_PICT_WIDTH, EDITOR_PICT_HEIGHT };

		ok = fwrite( size, sizeof(size), 1, filePtr ) == 1 &&
			  fwrite( pictBuf, EDITOR_PICT_WIDTH * EDITOR_PICT_HEIGHT, 1, filePtr ) == 1;

		ok = (fclose(filePtr)==0) && ok;
	}

	mem_del(pictBuf);

	return ok;
}
//----------- End of static function write_picture_file -----------//


//-------- Begin of static function capture_zoom --------//
//
// Draw the map view (terrain and objects) at the given location on the
// back buffer, as Sys::disp_zoom().
//
static void capture_zoom(int topXLoc, int topYLoc)
{
	ZoomMatrix* zoomMatrix = world.zoom_matrix;

	zoomMatrix->top_x_loc = topXLoc;
	zoomMatrix->top_y_loc = topYLoc;

	World::view_top_x = topXLoc * ZOOM_LOC_WIDTH;
	World::view_top_y = topYLoc * ZOOM_LOC_HEIGHT;

	int useBackBuf = vga.use_back_buf;
	vga.use_back();

	zoomMatrix->draw();
	zoomMatrix->disp();
	zoomMatrix->draw_frame();

	if( !useBackBuf )
		vga.use_front();
}
//----------- End of static function capture_zoom -----------//


//-------- Begin of function ModEditor::load_scenario_info --------//
//
// Read the data of a scenario opened with "Edit": the name, the difficulty,
// the score bonus and the description of its .SCT file. The description
// is wrapped in the lines of the window.
//
// <char*> sctPath          - the path of the .SCT file
// <int>   isEditorScenario - whether it is a scenario of SCENMOD (saving it
//                            replaces it), otherwise saving it saves a copy
// <char*> fileName         - the file name of the .SCN
//
void ModEditor::load_scenario_info(const char* sctPath, int isEditorScenario, const char* fileName)
{
	memset( scen_name, 0, sizeof(scen_name) );
	memset( scen_desc, 0, sizeof(scen_desc) );
	scen_file_name[0] = 0;

	//------ the file name of a scenario of SCENMOD ------//

	if( isEditorScenario )
	{
		int len = strlen(fileName);
		const char* dot = strrchr(fileName, '.');

		if( dot )
			len = dot - fileName;

		if( len <= EDITOR_FILE_NAME_LEN )
		{
			memcpy( scen_file_name, fileName, len );
			scen_file_name[len] = 0;
		}
	}

	//------ read the text file ------//

	FILE* filePtr = fopen(sctPath, "rb");

	if( !filePtr )
		return;

	enum { LINE_LEN = 1000, TEXT_LEN = 4000 };

	char line[LINE_LEN+1];
	char text[TEXT_LEN+1] = "";
	int  lineNo = 0, inDesc = 0;

	while( fgets(line, LINE_LEN, filePtr) )
	{
		//--- remove the end of the line and the end of file character ---//

		int len = strlen(line);

		while( len > 0 && (line[len-1]=='\r' || line[len-1]=='\n' || line[len-1]==0x1A) )
			line[--len] = 0;

		lineNo++;

		if( lineNo == 1 )
		{
			strncpy( scen_name, line, EDITOR_NAME_LEN );
			scen_name[EDITOR_NAME_LEN] = 0;
		}
		else if( !inDesc )
		{
			if( strncmp(line, "Difficulty:", 11)==0 )
				scen_difficulty = (short) atoi(line+11);

			else if( strncmp(line, "Bonus:", 6)==0 )
				scen_bonus = (short) atoi(line+6);

			else if( line[0]=='-' )		// the separator line
				inDesc = 1;
		}
		else
		{
			//--- the description: the lines are joined with a line break mark ---//

			if( strlen(text) + len + 2 < TEXT_LEN )
			{
				strcat( text, line );
				strcat( text, "\n" );
			}
		}
	}

	fclose(filePtr);

	scen_difficulty = MAX( 1, MIN(10, scen_difficulty) );
	scen_bonus = MAX( 0, MIN(500, scen_bonus) );

	//------ wrap the description in the lines of the window ------//

	int descLine = 0, pos = 0;
	char* p = text;

	while( *p && descLine < EDITOR_DESC_LINES )
	{
		if( *p == '\n' )		// a line break of the file: next line (skip empty lines at the start)
		{
			if( pos > 0 )
			{
				descLine++;
				pos = 0;
			}
			p++;
			continue;
		}

		//--- the next word ---//

		char* wordEnd = p;

		while( *wordEnd && *wordEnd != ' ' && *wordEnd != '\n' )
			wordEnd++;

		int wordLen = wordEnd - p;

		if( pos > 0 && pos + 1 + wordLen > EDITOR_DESC_LEN )		// doesn't fit: next line
		{
			descLine++;
			pos = 0;

			if( descLine >= EDITOR_DESC_LINES )
				break;
		}

		if( pos > 0 )
			scen_desc[descLine][pos++] = ' ';

		int copyLen = MIN( wordLen, EDITOR_DESC_LEN - pos );
		memcpy( scen_desc[descLine]+pos, p, copyLen );
		pos += copyLen;
		scen_desc[descLine][pos] = 0;

		p = wordEnd;

		while( *p == ' ' )
			p++;
	}
}
//--------- End of function ModEditor::load_scenario_info ---------//
