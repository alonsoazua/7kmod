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

//Filename    : ModEditorPanel.cpp
//Description : 7kmod-editor: the side panel of the editor (phase 2): the
//              kingdom of the objects placed, the palette of each tab,
//              placing objects on the map and the selected object.

#include <ModEditor.h>
#include <OSYS.h>
#include <OCONFIG.h>
#include <OVGA.h>
#include <vga_util.h>
#include <OFONT.h>
#include <OMOUSE.h>
#include <KEY.h>
#include <OBOX.h>
#include <OHELP.h>
#include <OSTR.h>
#include <OPOWER.h>
#include <OINFO.h>
#include <OWORLD.h>
#include <OWORLDMT.h>
#include <ONATION.h>
#include <OUNIT.h>
#include <OUNITRES.h>
#include <OSPRTRES.h>
#include <OTOWN.h>
#include <OFIRM.h>
#include <OFIRMRES.h>
#include <OF_MINE.h>
#include <OGODRES.h>
#include <OFIRMID.h>
#include <OSITE.h>
#include <OFIRMDIE.h>
#include <ORAWRES.h>
#include <ORACERES.h>
#include <OTOWNRES.h>
#include <OMONSRES.h>
#include <OSKILL.h>
#include <OTERRAIN.h>
#include <OTECHRES.h>
#include <OMISC.h>
#include <OSPY.h>
#include "gettext.h"

//------- the layout of the panel --------//

enum { OWNER_Y1 = INFO_Y1+4,
		 TAB_Y1 = INFO_Y1+28,
		 TAB_COLUMNS = 3,
		 TAB_WIDTH = 64,
		 TAB_HEIGHT = 20,
		 PALETTE_Y1 = INFO_Y1+74,
		 ENTRY_WIDTH = 97,
		 ENTRY_HEIGHT = 16,
		 OPTION_HEIGHT = 20,
		 SELECT_MIN_HEIGHT = 100 };		// the box of the selected object grows upwards with its rows; the palette ends above it

static const char* tab_str[EDITOR_TAB_COUNT] =
{
	N_("Units"), N_("Weapons"), N_("Towns"), N_("Buildings"), N_("Nature"), N_("Terrain")
};

#define TAB_X(i)	(INFO_X1+4 + ((i)%TAB_COLUMNS) * (TAB_WIDTH+2))
#define TAB_Y(i)	(TAB_Y1 + ((i)/TAB_COLUMNS) * (TAB_HEIGHT+2))

//------- the entries of the palettes --------//

enum { PLACE_SOLDIER=1,
		 PLACE_GENERAL,
		 PLACE_WORKER,
		 PLACE_UNIT,				// weapons and ships, id: unit id
		 PLACE_TOWN,
		 PLACE_FIRM,				// id: firm id
		 PLACE_RAW,				// id: raw id
		 PLACE_LAIR,
		 PLACE_TREE,				// the Terrain tab (ModEditorTerrain.cpp)
		 PLACE_ROCK,				// id: 0 - small, 1 - large
		 PLACE_CLEAR,
		 PLACE_GROUND,			// id: the terrain type
		 PLACE_SHAPE,				// id: EDITOR_TERRAIN_WATER, _LAND, _HILL or _RAMP
		 PLACE_FRYHTAN,			// 7kmod-gameplay: an invoked Fryhtan (ModEditorFryhtan.cpp)
		 PLACE_SPY };

struct PlaceEntry
{
	char  type;
	short id;
};

static PlaceEntry unit_entry_array[] =
{
	{ PLACE_SOLDIER, 0 }, { PLACE_GENERAL, 0 },
	{ PLACE_WORKER, 0 }, { PLACE_SPY, 0 },
	{ PLACE_FRYHTAN, 0 },
};

static PlaceEntry weapon_entry_array[] =
{
	{ PLACE_UNIT, UNIT_CATAPULT }, { PLACE_UNIT, UNIT_BALLISTA },
	{ PLACE_UNIT, UNIT_FLAMETHROWER }, { PLACE_UNIT, UNIT_CANNON },
	{ PLACE_UNIT, UNIT_EXPLOSIVE_CART }, { PLACE_UNIT, UNIT_F_BALLISTA },
	{ PLACE_UNIT, UNIT_VESSEL }, { PLACE_UNIT, UNIT_TRANSPORT },
	{ PLACE_UNIT, UNIT_CARAVEL }, { PLACE_UNIT, UNIT_GALLEON },
};

static PlaceEntry town_entry_array[] =
{
	{ PLACE_TOWN, 0 },
};

static PlaceEntry firm_entry_array[] =
{
	{ PLACE_FIRM, FIRM_CAMP }, { PLACE_FIRM, FIRM_MINE },
	{ PLACE_FIRM, FIRM_FACTORY }, { PLACE_FIRM, FIRM_INN },
	{ PLACE_FIRM, FIRM_MARKET }, { PLACE_FIRM, FIRM_RESEARCH },
	{ PLACE_FIRM, FIRM_WAR_FACTORY }, { PLACE_FIRM, FIRM_HARBOR },
	{ PLACE_FIRM, FIRM_BASE },
};

static PlaceEntry nature_entry_array[] =
{
	{ PLACE_RAW, 1 }, { PLACE_RAW, 2 }, { PLACE_RAW, 3 },
	{ PLACE_LAIR, 0 },
};

static PlaceEntry terrain_entry_array[] =
{
	{ PLACE_TREE, 0 }, { PLACE_CLEAR, 0 },
	{ PLACE_ROCK, 0 }, { PLACE_ROCK, 1 },
	{ PLACE_GROUND, TERRAIN_DARK_GRASS }, { PLACE_GROUND, TERRAIN_LIGHT_GRASS },
	{ PLACE_SHAPE, EDITOR_TERRAIN_WATER },
	{ PLACE_SHAPE, EDITOR_TERRAIN_LAND }, { PLACE_SHAPE, EDITOR_TERRAIN_HILL },
	{ PLACE_SHAPE, EDITOR_TERRAIN_RAMP },
};

static PlaceEntry* tab_entry_array[EDITOR_TAB_COUNT] =
	{ unit_entry_array, weapon_entry_array, town_entry_array, firm_entry_array, nature_entry_array, terrain_entry_array };

static int tab_entry_count[EDITOR_TAB_COUNT] =
{
	sizeof(unit_entry_array)/sizeof(PlaceEntry), sizeof(weapon_entry_array)/sizeof(PlaceEntry),
	sizeof(town_entry_array)/sizeof(PlaceEntry), sizeof(firm_entry_array)/sizeof(PlaceEntry),
	sizeof(nature_entry_array)/sizeof(PlaceEntry), sizeof(terrain_entry_array)/sizeof(PlaceEntry)
};

//------- the options of the palettes --------//

enum { OPTION_RACE=1, OPTION_COMBAT, OPTION_LEADERSHIP, OPTION_SKILL, OPTION_SKILL_LEVEL,
		 OPTION_VERSION, OPTION_POPULATION, OPTION_RESERVE, OPTION_MONSTER, OPTION_BRUSH,
		 OPTION_SPYING, OPTION_CLOAK,
		 OPTION_FRYHTAN, OPTION_FRYHTAN_COMBAT };		// 7kmod-gameplay

enum { MAX_OPTION = 3 };

static char worker_skill_array[] = { SKILL_MINING, SKILL_MFT, SKILL_RESEARCH, SKILL_CONSTRUCTION, SKILL_PRAYING, 0 };	// 0 - none: a peasant

//------- right click: moving the selected units --------//

enum { MAX_UNIT_PER_ORDER = 200 };

//------- the properties of the selected object --------//

enum { PROP_NONE, PROP_COMBAT, PROP_SKILL, PROP_VERSION, PROP_POPULATION, PROP_RESERVE,
		 PROP_RACE,				// towns and human units (the king changes the race of his kingdom)
		 PROP_MINERAL,			// mines: none, clay, copper, iron
		 PROP_MINE_RESERVE,
		 PROP_SPYING,			// spies: the spying skill
		 PROP_CLOAK };			// spies on the map: the kingdom they disguise as

enum { MAX_PROP = 4 };

//------- the layout of the box of the selected object --------//

struct SelectLayout
{
	int	y1;							// the top of the box
	int	info_y;						// the lines of a building (its general/overseer and workers)
	int	info_count;
	int	rows_y;						// the rows of the properties
	int	prop_count;
	char	prop_array[MAX_PROP];
	int	value_array[MAX_PROP];
	int	out_y;						// "All out" and "General out", -1 - none
	int	spy_y;						// "Spies out" of a town, -1 - none
	int	name_y;						// "Re-roll" and "Rename", -1 - none
	int	owner_y;						// "Set Owner" and "Delete"
};

//------- Declare static functions --------//

static const char* entry_name(PlaceEntry* entryPtr);
static const char* owner_name(int nationRecno);
static int	next_owner(int nationRecno, int dir);
static int	next_monster(int monsterId, int dir);
static int	get_options(char* optionArray);
static void	option_str(int optionId, String& labelStr, String& valueStr);
static void	change_option(int optionId, int dir);
static int	selected_properties(char* propArray, int* valueArray);
static void	change_property(int propId, int dir);
static int	weapon_max_version(int unitId);
static void	set_weapon_version(Unit* unitPtr, int version);
static int	unit_firm_at(int unitId, int xLoc, int yLoc);
static int	assign_to_firm(int unitRecno, int firmRecno);
static int	terrain_place_type(PlaceEntry* entryPtr);
static int	can_set_owner();
static int	can_place_unit(int unitId, int xLoc, int yLoc);
static int	place_object(PlaceEntry* entryPtr, int xLoc, int yLoc);
static void	disp_selected(SelectLayout& layout);
static void	select_layout(SelectLayout& layout);
static int	can_change_race(Unit* unitPtr);
static void	set_scroll(int nationRecno, int raceId, int knowFlag);
static int	can_reroll_unit(Unit* unitPtr);
static int	reroll_unit_name(Unit* unitPtr);
static void	reroll_town_name(Town* townPtr);
static int	selected_name_unit();
static void	update_scroll(int nationRecno, int raceId);
static int	next_race(int raceId, int dir);
static void	change_town_race(Town* townPtr, int newRaceId);
static int	change_unit_race(int unitRecno, int newRaceId);
static void	change_selected_units_race(int dir);
static void	set_mine_mineral(FirmMine* minePtr, int rawId);
static void	set_mine_reserve(FirmMine* minePtr, int reserveQty);
static int	place_spy(int xLoc, int yLoc);
static int	spy_place_at(int xLoc, int yLoc, int& placePara);
static int	place_spy_count(int spyPlace, int placePara);
static void	take_out_town_spies(Town* townPtr);
static void	put_arrow_row(int y, const char* labelStr, const char* valueStr, int enabled=1);
static int	detect_arrow_row(int y);


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

	//------- the kingdom of the objects placed -------//

	editor_put_button( INFO_X1+4, OWNER_Y1, INFO_X1+22, OWNER_Y1+19, "<", 1, 0 );
	editor_put_button( INFO_X2-22, OWNER_Y1, INFO_X2-4, OWNER_Y1+19, ">", 1, 0 );

	vga_util.d3_panel_down( INFO_X1+26, OWNER_Y1, INFO_X2-26, OWNER_Y1+19 );

	int x = INFO_X1+31;

	if( owner_nation_recno > 0 )
	{
		int color = nation_array[owner_nation_recno]->nation_color;
		vga_back.bar( x, OWNER_Y1+4, x+11, OWNER_Y1+15, color );
		x += 16;
	}

	font_san.put( x, OWNER_Y1+3, owner_name(owner_nation_recno), 0, INFO_X2-28 );

	//------- tabs -------//

	for( int i=0 ; i<EDITOR_TAB_COUNT ; i++ )
		editor_put_button( TAB_X(i), TAB_Y(i), TAB_X(i)+TAB_WIDTH-1, TAB_Y(i)+TAB_HEIGHT-1, _(tab_str[i]), 1, panel_tab==i );

	//------- the palette: above the box of the selected object -------//

	SelectLayout layout;
	select_layout(layout);

	int paletteY2 = layout.y1-4;

	vga_util.d3_panel_down( INFO_X1, PALETTE_Y1, INFO_X2, paletteY2 );

	PlaceEntry* entryArray = tab_entry_array[(int)panel_tab];
	int entryCount = tab_entry_count[(int)panel_tab];
	int y = PALETTE_Y1+4;

	for( int i=0 ; i<entryCount ; i++ )
	{
		int entryX = INFO_X1+4 + (i%2) * (ENTRY_WIDTH+2);
		int entryY = y + (i/2) * (ENTRY_HEIGHT+1);

		editor_put_button( entryX, entryY, entryX+ENTRY_WIDTH-1, entryY+ENTRY_HEIGHT-1,
								 entry_name(entryArray+i), 1, place_entry==i );
	}

	y += (entryCount+1)/2 * (ENTRY_HEIGHT+1) + 4;

	//------- the options -------//

	char optionArray[MAX_OPTION];
	int  optionCount = get_options(optionArray);

	for( int i=0 ; i<optionCount && y+17 <= paletteY2-2 ; i++ )		// the rows that fit above the selected object
	{
		String labelStr, valueStr;
		option_str( optionArray[i], labelStr, valueStr );

		put_arrow_row( y, labelStr, valueStr );
		y += OPTION_HEIGHT;
	}

	//------- the help of placing -------//

	if( place_entry >= 0 && paletteY2-16 >= y )
		font_san.put( INFO_X1+6, paletteY2-16, _("Click on the map to place it."), 0, INFO_X2-4 );

	//------- the selected object -------//

	disp_selected( layout );

	if( !useBackBuf )
		vga.use_front();

	vga_util.blt_buf( INFO_X1, INFO_Y1, INFO_X2, INFO_Y2, 0 );
}
//--------- End of function ModEditor::disp_panel ---------//


//-------- Begin of function ModEditor::detect_panel --------//
//
void ModEditor::detect_panel()
{
	//------- the kingdom of the objects placed -------//

	help.set_custom_help( INFO_X1+4, OWNER_Y1, INFO_X2-4, OWNER_Y1+19, _("Kingdom"),
		_("The kingdom of the units, towns and buildings that you place. Towns can also be independent.") );

	if( mouse.any_click( INFO_X1+4, OWNER_Y1, INFO_X1+22, OWNER_Y1+19 ) )
	{
		owner_nation_recno = next_owner(owner_nation_recno, -1);
		info.disp();
		return;
	}

	if( mouse.any_click( INFO_X2-22, OWNER_Y1, INFO_X2-4, OWNER_Y1+19 ) )
	{
		owner_nation_recno = next_owner(owner_nation_recno, 1);
		info.disp();
		return;
	}

	//------- tabs -------//

	for( int i=0 ; i<EDITOR_TAB_COUNT ; i++ )
	{
		if( mouse.any_click( TAB_X(i), TAB_Y(i), TAB_X(i)+TAB_WIDTH-1, TAB_Y(i)+TAB_HEIGHT-1 ) )
		{
			if( panel_tab != i )
			{
				panel_tab = i;
				place_entry = -1;
				info.disp();
			}
			return;
		}
	}

	//------- the palette -------//

	int entryCount = tab_entry_count[(int)panel_tab];
	int y = PALETTE_Y1+4;

	for( int i=0 ; i<entryCount ; i++ )
	{
		int entryX = INFO_X1+4 + (i%2) * (ENTRY_WIDTH+2);
		int entryY = y + (i/2) * (ENTRY_HEIGHT+1);

		if( mouse.any_click( entryX, entryY, entryX+ENTRY_WIDTH-1, entryY+ENTRY_HEIGHT-1 ) )
		{
			if( place_entry == i )
				place_entry = -1;		// click again: stop placing
			else
			{
				place_entry = i;
				power.reset_selection();
			}

			info.disp();
			return;
		}
	}

	y += (entryCount+1)/2 * (ENTRY_HEIGHT+1) + 4;

	//------- the options -------//

	char optionArray[MAX_OPTION];
	int  optionCount = get_options(optionArray);

	SelectLayout layout;
	select_layout(layout);

	for( int i=0 ; i<optionCount && y+17 <= layout.y1-6 ; i++ )
	{
		int dir = detect_arrow_row(y);

		if( dir )
		{
			change_option(optionArray[i], dir);
			info.disp();
			return;
		}

		y += OPTION_HEIGHT;
	}

	//------- the selected object: properties -------//

	for( int i=0 ; i<layout.prop_count ; i++ )
	{
		int dir = detect_arrow_row(layout.rows_y+i*20);

		if( dir )
		{
			change_property(layout.prop_array[i], dir);
			sys.zoom_need_redraw = 1;
			info.disp();
			return;
		}
	}

	//------- the selected building: take out its units -------//

	if( layout.out_y >= 0 )
	{
		int buttonY = layout.out_y;

		if( mouse.any_click( INFO_X1+6, buttonY, INFO_X1+100, buttonY+15 ) )
		{
			take_out_all();
			return;
		}

		if( mouse.any_click( INFO_X1+106, buttonY, INFO_X2-6, buttonY+15 ) )
		{
			take_out_general();
			return;
		}
	}

	//------- the selected town: take out its spies -------//

	if( layout.spy_y >= 0 && mouse.any_click( INFO_X1+6, layout.spy_y, INFO_X1+100, layout.spy_y+15 ) )
	{
		take_out_town_spies( town_array[town_array.selected_recno] );
		sys.zoom_need_redraw = 1;
		sys.map_need_redraw = 1;
		info.disp();
		return;
	}

	//------- the name: Re-roll and Rename -------//

	if( layout.name_y >= 0 )
	{
		int buttonY = layout.name_y;
		int unitRecno = selected_name_unit();
		Unit* unitPtr = unitRecno ? unit_array[unitRecno] : NULL;
		int isKing = unitPtr && unitPtr->rank_id == RANK_KING && unitPtr->nation_recno;

		if( mouse.any_click( INFO_X1+6, buttonY, INFO_X1+100, buttonY+15 ) )
		{
			if( town_array.selected_recno )
				reroll_town_name( town_array[town_array.selected_recno] );
			else if( unitPtr )
				reroll_unit_name(unitPtr);

			info.disp();
			return;
		}

		if( isKing && mouse.any_click( INFO_X1+106, buttonY, INFO_X2-6, buttonY+15 ) )
		{
			edit_king_name(unitPtr->nation_recno);
			info.disp();
			return;
		}
	}

	//------- Set Owner and Delete -------//

	int buttonY = layout.owner_y;

	if( mouse.any_click( INFO_X1+6, buttonY, INFO_X1+100, buttonY+16 ) )
	{
		set_selected_owner();
		return;
	}

	if( mouse.any_click( INFO_X1+106, buttonY, INFO_X2-6, buttonY+16 ) )
	{
		delete_selected();
		return;
	}
}
//--------- End of function ModEditor::detect_panel ---------//


//-------- Begin of function ModEditor::detect_view --------//
//
// Detect the clicks of the editor mode on the map, after selecting objects.
// Called by Sys::detect_view() instead of the commands of the normal game.
//
// return: <int> 1 - a click was used
//
int ModEditor::detect_view()
{
	//---- right click on the map: move the selected units, or put them in a building or town ----//

	if( mouse.any_click(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2, RIGHT_BUTTON) )
	{
		if( unit_array.selected_recno )
		{
			int xLoc = world.zoom_matrix->top_x_loc + (mouse.click_x(RIGHT_BUTTON)-ZOOM_X1)/ZOOM_LOC_WIDTH;
			int yLoc = world.zoom_matrix->top_y_loc + (mouse.click_y(RIGHT_BUTTON)-ZOOM_Y1)/ZOOM_LOC_HEIGHT;

			order_selected(xLoc, yLoc);
		}
		return 1;
	}

	return 0;
}
//--------- End of function ModEditor::detect_view ---------//


//-------- Begin of function ModEditor::detect_place --------//
//
// When placing an object, a click on the map places it instead of
// selecting. Called by Sys::detect_view() before selecting objects.
//
// return: <int> 1 - placing an object (the clicks are not used for selecting)
//
int ModEditor::detect_place()
{
	if( place_entry < 0 )
		return 0;

	//---- right click: stop placing ----//

	if( mouse.any_click(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2, RIGHT_BUTTON) )
	{
		place_entry = -1;
		info.disp();
		return 1;
	}

	PlaceEntry* entryPtr = tab_entry_array[(int)panel_tab] + place_entry;

	//---- painting the terrain by dragging: each new location under the mouse ----//

	static char dragPainting;
	static int  dragXLoc, dragYLoc;

	if( dragPainting )
	{
		if( !mouse.left_press )
			dragPainting = 0;

		else if( mouse.cur_x >= ZOOM_X1 && mouse.cur_x <= ZOOM_X2 && mouse.cur_y >= ZOOM_Y1 && mouse.cur_y <= ZOOM_Y2 )
		{
			int xLoc = world.zoom_matrix->top_x_loc + (mouse.cur_x-ZOOM_X1)/ZOOM_LOC_WIDTH;
			int yLoc = world.zoom_matrix->top_y_loc + (mouse.cur_y-ZOOM_Y1)/ZOOM_LOC_HEIGHT;

			if( xLoc != dragXLoc || yLoc != dragYLoc )
			{
				dragXLoc = xLoc;
				dragYLoc = yLoc;

				editor_terrain_quiet = 1;		// what can't be painted is skipped
				int rc = place_object(entryPtr, xLoc, yLoc);
				editor_terrain_quiet = 0;

				if( rc )
				{
					sys.zoom_need_redraw = 1;
					sys.map_need_redraw = 1;
				}
			}
		}
	}

	if( !mouse.any_click(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2) )
		return 1;

	int xLoc = world.zoom_matrix->top_x_loc + (mouse.click_x()-ZOOM_X1)/ZOOM_LOC_WIDTH;
	int yLoc = world.zoom_matrix->top_y_loc + (mouse.click_y()-ZOOM_Y1)/ZOOM_LOC_HEIGHT;

	if( place_object(entryPtr, xLoc, yLoc) )
	{
		sys.zoom_need_redraw = 1;
		sys.map_need_redraw = 1;
		info.disp();
	}

	//--- the brushes of the Terrain tab paint while the button is held (not the rocks and Ramp) ---//

	if( entryPtr->type == PLACE_TREE || entryPtr->type == PLACE_CLEAR || entryPtr->type == PLACE_GROUND ||
		 (entryPtr->type == PLACE_SHAPE && entryPtr->id != EDITOR_TERRAIN_RAMP) )
	{
		dragPainting = mouse.left_press != 0;		// the box of a message ends the dragging
		dragXLoc = xLoc;
		dragYLoc = yLoc;
	}

	return 1;
}
//--------- End of function ModEditor::detect_place ---------//


//-------- Begin of function ModEditor::get_place_area --------//
//
// The area of the object being placed at the given location, for the
// marker of ZoomMatrix::draw_build_marker().
//
// <int>  xLoc, yLoc           - the location under the mouse
// <int&> locWidth, locHeight  - for returning the size
//
// return: <int> -1 - not placing anything
//                0 - the object can't be placed here
//                1 - the object can be placed here
//
int ModEditor::get_place_area(int xLoc, int yLoc, int& locWidth, int& locHeight)
{
	if( !editing_flag || place_entry < 0 )
		return -1;

	PlaceEntry* entryPtr = tab_entry_array[(int)panel_tab] + place_entry;

	locWidth = locHeight = 1;

	switch( entryPtr->type )
	{
		case PLACE_SOLDIER:
		case PLACE_GENERAL:
		case PLACE_WORKER:
		case PLACE_UNIT:
		case PLACE_FRYHTAN:
		{
			int unitId = entryPtr->type==PLACE_UNIT ? entryPtr->id :
							 entryPtr->type==PLACE_FRYHTAN ? place_fryhtan_unit_id : race_res[place_race_id]->basic_unit_id;
			SpriteInfo* spriteInfo = sprite_res[unit_res[unitId]->sprite_id];

			locWidth  = spriteInfo->loc_width;
			locHeight = spriteInfo->loc_height;

			if( owner_nation_recno <= 0 )
				return 0;

			//--- on a building of the kingdom: the unit is put in it ---//

			if( entryPtr->type != PLACE_UNIT && unit_firm_at(unitId, xLoc, yLoc) )
				return 1;

			return can_place_unit(unitId, xLoc, yLoc);
		}

		case PLACE_SPY:
		{
			if( owner_nation_recno <= 0 )
				return 0;

			//--- on a town or a building: the spy goes in it, of any kingdom ---//

			int placePara;

			if( spy_place_at(xLoc, yLoc, placePara) )
				return 1;

			if( world.get_loc(xLoc, yLoc)->is_firm() || world.get_loc(xLoc, yLoc)->is_town() )
				return 0;

			return can_place_unit(race_res[place_race_id]->basic_unit_id, xLoc, yLoc);
		}

		case PLACE_TOWN:
			locWidth  = STD_TOWN_LOC_WIDTH;
			locHeight = STD_TOWN_LOC_HEIGHT;
			return world.can_build_town(xLoc, yLoc);

		case PLACE_FIRM:
			locWidth  = firm_res[entryPtr->id]->loc_width;
			locHeight = firm_res[entryPtr->id]->loc_height;
			return owner_nation_recno > 0 && world.can_build_firm(xLoc, yLoc, entryPtr->id) != 0;

		case PLACE_RAW:
		{
			Location* locPtr = world.get_loc(xLoc, yLoc);
			return locPtr->can_build_site(LOCATE_WALK_LAND) && !locPtr->is_power_off();
		}

		case PLACE_LAIR:
			locWidth  = firm_res[FIRM_MONSTER]->loc_width;
			locHeight = firm_res[FIRM_MONSTER]->loc_height;
			return world.can_build_firm(xLoc, yLoc, FIRM_MONSTER) != 0;

		case PLACE_TREE:
		case PLACE_CLEAR:
		case PLACE_ROCK:
		case PLACE_GROUND:
		case PLACE_SHAPE:
			return editor_terrain_area( terrain_place_type(entryPtr), entryPtr->id, xLoc, yLoc,
												 place_brush, locWidth, locHeight );
	}

	return -1;
}
//--------- End of function ModEditor::get_place_area ---------//


//-------- Begin of function ModEditor::delete_selected --------//
//
// Delete the selected units (except kings), town, building or natural
// resource.
//
void ModEditor::delete_selected()
{
	if( firm_array.selected_recno )
	{
		int firmRecno = firm_array.selected_recno;
		power.reset_selection();

		//--- a Seat of Power: the kingdom loses the scroll if it has no other Seat of its race ---//

		Firm* firmPtr = firm_array[firmRecno];
		int baseNationRecno = firmPtr->firm_id == FIRM_BASE ? firmPtr->nation_recno : 0;
		int baseRaceId = firmPtr->race_id;

		if( firm_array[firmRecno]->firm_id == FIRM_MONSTER )
		{
			//--- a lair is deleted with its Fryhtans (as when the game ends: they are not mobilized) ---//

			char oldExitFlag = sys.signal_exit_flag;
			sys.signal_exit_flag = 2;
			firm_array.del_firm(firmRecno);
			sys.signal_exit_flag = oldExitFlag;
		}
		else
		{
			//--- the units in the building come out; no ruins (FirmDie) in the editor ---//

			int dieCount = firm_die_array.size();

			firm_array.del_firm(firmRecno);

			if( firm_die_array.size() > dieCount && !firm_die_array.is_deleted(firm_die_array.size()) )
				firm_die_array.del(firm_die_array.size());
		}

		if( baseNationRecno )
			update_scroll(baseNationRecno, baseRaceId);
	}
	else if( town_array.selected_recno )
	{
		int townRecno = town_array.selected_recno;
		power.reset_selection();
		town_array.del_town(townRecno);
	}
	else if( site_array.selected_recno )
	{
		int siteRecno = site_array.selected_recno;

		if( site_array[siteRecno]->has_mine )
		{
			box.msg( _("Delete the mine first.") );
			return;
		}

		power.reset_selection();
		site_array.del_site(siteRecno);
	}
	else if( unit_array.selected_recno )
	{
		int kingCount=0;

		for( int i=unit_array.size() ; i>0 ; i-- )
		{
			if( unit_array.is_deleted(i) )
				continue;

			Unit* unitPtr = unit_array[i];

			if( !unitPtr->selected_flag || !unitPtr->is_visible() )
				continue;

			if( unitPtr->rank_id == RANK_KING )
			{
				kingCount++;
				continue;
			}

			unit_array.del(i);
		}

		power.reset_selection();

		if( kingCount )
			box.msg( _("Kings can't be deleted.") );
	}
	else
		return;

	sys.zoom_need_redraw = 1;
	sys.map_need_redraw = 1;
	info.disp();
}
//--------- End of function ModEditor::delete_selected ---------//


//-------- Begin of function ModEditor::set_selected_owner --------//
//
// Give the selected units, town or building to the kingdom of the
// objects placed (the one at the top of the panel).
//
void ModEditor::set_selected_owner()
{
	if( !can_set_owner() )
	{
		box.msg( _("The kingdom of this can't be changed to the selected one.") );
		return;
	}

	int nationRecno = owner_nation_recno;

	if( firm_array.selected_recno )
	{
		Firm* firmPtr = firm_array[firm_array.selected_recno];
		int oldNationRecno = firmPtr->nation_recno;

		firmPtr->change_nation(nationRecno);

		//--- a Seat of Power: the scroll goes with it ---//

		if( firmPtr->firm_id == FIRM_BASE )
		{
			if( oldNationRecno )
				update_scroll(oldNationRecno, firmPtr->race_id);

			if( nationRecno )
				update_scroll(nationRecno, firmPtr->race_id);
		}
	}
	else if( town_array.selected_recno )
	{
		Town* townPtr = town_array[town_array.selected_recno];

		townPtr->set_nation(nationRecno);

		//--- the loyalty or the resistance of an independent town ---//

		for( int raceId=1 ; raceId<=MAX_RACE ; raceId++ )
		{
			if( !townPtr->race_pop_array[raceId-1] )
				continue;

			if( nationRecno )
				townPtr->race_loyalty_array[raceId-1] = 80;
			else
			{
				int resistance = town_array.independent_town_resistance();

				for( int n=0 ; n<MAX_NATION ; n++ )
					townPtr->race_resistance_array[raceId-1][n] = (float) resistance;
			}
		}
	}
	else if( unit_array.selected_recno )
	{
		for( int i=unit_array.size() ; i>0 ; i-- )
		{
			if( unit_array.is_deleted(i) )
				continue;

			Unit* unitPtr = unit_array[i];

			if( !unitPtr->selected_flag || !unitPtr->is_visible() )
				continue;

			if( unitPtr->rank_id == RANK_KING || unit_res[unitPtr->unit_id]->unit_class == UNIT_CLASS_MONSTER )
				continue;

			//--- a spy: its true kingdom changes; it keeps its cloak, unless it was its own kingdom ---//

			if( unitPtr->spy_recno )
			{
				Spy* spyPtr = spy_array[unitPtr->spy_recno];
				int  oldNationRecno = spyPtr->true_nation_recno;

				if( oldNationRecno == nationRecno )
					continue;

				spyPtr->change_true_nation(nationRecno);

				if( unitPtr->nation_recno == oldNationRecno )
					unitPtr->spy_change_nation(nationRecno, COMMAND_AUTO);
				continue;
			}

			unitPtr->change_nation(nationRecno);
		}
	}

	sys.zoom_need_redraw = 1;
	sys.map_need_redraw = 1;
	info.disp();
}
//--------- End of function ModEditor::set_selected_owner ---------//


//-------- Begin of function ModEditor::order_selected --------//
//
// Right click with units selected: put them in the building or town of
// their kingdom at the location, or move them there, at once.
//
void ModEditor::order_selected(int xLoc, int yLoc)
{
	Location* locPtr = world.get_loc(xLoc, yLoc);

	//------ the selected units on the map ------//

	short unitRecnoArray[MAX_UNIT_PER_ORDER];
	int   unitCount=0;

	for( int i=unit_array.size() ; i>0 && unitCount<MAX_UNIT_PER_ORDER ; i-- )
	{
		if( unit_array.is_deleted(i) )
			continue;

		Unit* unitPtr = unit_array[i];

		if( unitPtr->selected_flag && unitPtr->is_visible() )
			unitRecnoArray[unitCount++] = i;
	}

	if( !unitCount )
		return;

	int failCount=0;

	//------ put them in a building ------//

	if( locPtr->is_firm() )
	{
		int firmRecno = locPtr->firm_recno();

		for( int i=0 ; i<unitCount ; i++ )
		{
			if( !assign_to_firm(unitRecnoArray[i], firmRecno) )
				failCount++;
		}
	}

	//------ put them in a town ------//

	else if( locPtr->is_town() )
	{
		Town* townPtr = town_array[locPtr->town_recno()];

		for( int i=0 ; i<unitCount ; i++ )
		{
			Unit* unitPtr = unit_array[unitRecnoArray[i]];

			if( unitPtr->nation_recno != townPtr->nation_recno || !unitPtr->race_id ||
				 unitPtr->rank_id == RANK_KING || townPtr->population >= MAX_TOWN_POPULATION )
			{
				failCount++;
				continue;
			}

			unitPtr->stop2();
			townPtr->assign_unit(unitRecnoArray[i]);
		}

		townPtr->auto_set_layout();
	}

	//------ move them ------//

	else
	{
		for( int i=0 ; i<unitCount ; i++ )
		{
			Unit* unitPtr = unit_array[unitRecnoArray[i]];
			int   mobileType = unitPtr->mobile_type;

			if( !locPtr->is_accessible(mobileType) )		// land units on land, ships on water
			{
				failCount++;
				continue;
			}

			int oldXLoc = unitPtr->next_x_loc();
			int oldYLoc = unitPtr->next_y_loc();
			int destX = xLoc, destY = yLoc;

			if( mobileType != UNIT_LAND )
			{
				destX &= ~1;
				destY &= ~1;
			}

			unitPtr->stop2();
			unitPtr->deinit_sprite(1);		// 1-keep it selected

			if( world.locate_space( &destX, &destY, destX, destY, unitPtr->sprite_info->loc_width,
											unitPtr->sprite_info->loc_height, mobileType ) )
			{
				unitPtr->init_sprite(destX, destY);
			}
			else
			{
				unitPtr->init_sprite(oldXLoc, oldYLoc);
				failCount++;
			}

			unitPtr->stop2();
		}
	}

	if( failCount )
	{
		String str;

		snprintf( str, MAX_STR_LEN+1, ngettext("%d unit couldn't be moved or put there.",
			"%d units couldn't be moved or put there.", failCount), failCount );

		box.msg( str );
	}

	//------ recount the selection ------//

	unit_array.selected_count = 0;
	int selectedRecno = 0;

	for( int i=unit_array.size() ; i>0 ; i-- )
	{
		if( !unit_array.is_deleted(i) && unit_array[i]->selected_flag && unit_array[i]->is_visible() )
		{
			unit_array.selected_count++;

			if( !selectedRecno || unit_array[i]->rank_id > unit_array[selectedRecno]->rank_id )
				selectedRecno = i;
		}
	}

	unit_array.selected_recno = selectedRecno;

	sys.zoom_need_redraw = 1;
	sys.map_need_redraw = 1;
	info.disp();
}
//--------- End of function ModEditor::order_selected ---------//


//-------- Begin of function ModEditor::take_out_all --------//
//
// The units in the selected building come out next to it.
//
void ModEditor::take_out_all()
{
	if( !firm_array.selected_recno )
		return;

	Firm* firmPtr = firm_array[firm_array.selected_recno];

	if( firmPtr->firm_id == FIRM_MONSTER )
		return;

	int firmRecno = firm_array.selected_recno;

	if( firmPtr->worker_array && firmPtr->worker_count > 0 )
		firmPtr->mobilize_all_workers(COMMAND_AUTO);

	if( !firm_array.is_deleted(firmRecno) && firm_array[firmRecno]->overseer_recno )
		firm_array[firmRecno]->mobilize_overseer();

	power.reset_selection();

	if( !firm_array.is_deleted(firmRecno) )
		firm_array.selected_recno = firmRecno;		// keep the building selected

	sys.zoom_need_redraw = 1;
	info.disp();
}
//--------- End of function ModEditor::take_out_all ---------//


//-------- Begin of function ModEditor::take_out_general --------//
//
void ModEditor::take_out_general()
{
	if( !firm_array.selected_recno )
		return;

	Firm* firmPtr = firm_array[firm_array.selected_recno];

	if( firmPtr->firm_id == FIRM_MONSTER || !firmPtr->overseer_recno )
		return;

	firmPtr->mobilize_overseer();

	sys.zoom_need_redraw = 1;
	info.disp();
}
//--------- End of function ModEditor::take_out_general ---------//


//-------- Begin of static function can_set_owner --------//
//
static int can_set_owner()
{
	int nationRecno = mod_editor.owner_nation_recno;

	if( firm_array.selected_recno )
	{
		Firm* firmPtr = firm_array[firm_array.selected_recno];

		return nationRecno > 0 && firmPtr->firm_id != FIRM_MONSTER &&
				 firmPtr->nation_recno != nationRecno;
	}

	if( town_array.selected_recno )
		return town_array[town_array.selected_recno]->nation_recno != nationRecno;

	if( unit_array.selected_recno )
	{
		Unit* unitPtr = unit_array[unit_array.selected_recno];

		return nationRecno > 0 && unitPtr->true_nation_recno() != nationRecno &&		// spies: their true kingdom
				 (unit_array.selected_count > 1 ||
				  (unitPtr->rank_id != RANK_KING && unit_res[unitPtr->unit_id]->unit_class != UNIT_CLASS_MONSTER));
	}

	return 0;
}
//----------- End of static function can_set_owner -----------//


//-------- Begin of static function place_object --------//
//
// return: <int> 1 - placed
//
static int place_object(PlaceEntry* entryPtr, int xLoc, int yLoc)
{
	int locWidth, locHeight;

	if( mod_editor.get_place_area(xLoc, yLoc, locWidth, locHeight) <= 0 )
	{
		if( mod_editor.owner_nation_recno == 0 &&
			 (entryPtr->type == PLACE_SOLDIER || entryPtr->type == PLACE_GENERAL ||
			  entryPtr->type == PLACE_UNIT || entryPtr->type == PLACE_FIRM || entryPtr->type == PLACE_FRYHTAN ||
			  entryPtr->type == PLACE_SPY) )
		{
			box.msg( _("Units and buildings need a kingdom. Choose one at the top of the panel.") );
		}
		return 0;
	}

	int nationRecno = mod_editor.owner_nation_recno;
	int raceId = mod_editor.place_race_id;

	switch( entryPtr->type )
	{
		case PLACE_SPY:
			return place_spy(xLoc, yLoc);

		case PLACE_SOLDIER:
		case PLACE_GENERAL:
		case PLACE_WORKER:
		case PLACE_UNIT:
		case PLACE_FRYHTAN:
		{
			int unitId, rankId = RANK_SOLDIER;

			if( entryPtr->type == PLACE_UNIT )
				unitId = entryPtr->id;
			else if( entryPtr->type == PLACE_FRYHTAN )
				unitId = mod_editor.place_fryhtan_unit_id;
			else
				unitId = race_res[raceId]->basic_unit_id;

			if( entryPtr->type == PLACE_GENERAL )
				rankId = RANK_GENERAL;

			//--- on a building of the kingdom: create the unit next to it and put it in ---//

			int firmRecno = entryPtr->type != PLACE_UNIT ? unit_firm_at(unitId, xLoc, yLoc) : 0;

			if( firmRecno )
			{
				Firm* firmPtr = firm_array[firmRecno];
				SpriteInfo* spriteInfo = sprite_res[unit_res[unitId]->sprite_id];

				xLoc = firmPtr->loc_x1;
				yLoc = firmPtr->loc_y1;

				if( !world.locate_space( &xLoc, &yLoc, firmPtr->loc_x2, firmPtr->loc_y2,
												 spriteInfo->loc_width, spriteInfo->loc_height ) )
				{
					return 0;
				}
			}
			else if( unit_res[unitId]->mobile_type != UNIT_LAND )
			{
				xLoc &= ~1;
				yLoc &= ~1;
			}

			int unitRecno = unit_array.add_unit( unitId, nationRecno, rankId, 80, xLoc, yLoc );

			if( !unitRecno )
				return 0;

			Unit* unitPtr = unit_array[unitRecno];

			switch( entryPtr->type )
			{
				case PLACE_SOLDIER:		// a soldier has the leadership skill, without skill it is a peasant
				case PLACE_GENERAL:
					unitPtr->set_combat_level( mod_editor.place_combat_level );
					unitPtr->skill.set_skill(SKILL_LEADING);
					unitPtr->skill.skill_level = mod_editor.place_leadership;
					break;

				case PLACE_WORKER:
					unitPtr->set_combat_level( CITIZEN_COMBAT_LEVEL );

					if( mod_editor.place_skill_id )
					{
						unitPtr->skill.set_skill( mod_editor.place_skill_id );
						unitPtr->skill.skill_level = mod_editor.place_skill_level;
					}
					else		// no skill: a peasant, as the ones that leave a town without a job
					{
						unitPtr->skill.skill_id = 0;
						unitPtr->skill.skill_level = 0;
					}
					break;

				case PLACE_UNIT:
					//--- weapons: the version must be set, or the attack of the weapon is wrong (it may crash) ---//

					if( unit_res[unitId]->unit_class == UNIT_CLASS_WEAPON )
						set_weapon_version( unitPtr, mod_editor.place_weapon_version );
					break;

				case PLACE_FRYHTAN:
					editor_init_fryhtan( unitPtr, mod_editor.place_fryhtan_combat );
					break;
			}

			if( firmRecno && !assign_to_firm(unitRecno, firmRecno) )
				box.msg( _("The unit can't be put in this building; it has been placed next to it.") );

			return 1;
		}

		case PLACE_TOWN:
		{
			int townRecno = town_array.add_town(nationRecno, raceId, xLoc, yLoc);

			if( !townRecno )
				return 0;

			Town* townPtr = town_array[townRecno];

			if( nationRecno )
				townPtr->init_pop( raceId, mod_editor.place_population, 80, 0, 1 );
			else
				townPtr->init_pop( raceId, mod_editor.place_population, town_array.independent_town_resistance(), 0, 1 );

			townPtr->auto_set_layout();
			return 1;
		}

		case PLACE_FIRM:
		{
			//--- no cost: the cash is restored after building ---//

			Nation* nationPtr = nation_array[nationRecno];
			float oldCash = nationPtr->cash;

			//--- nor its expense in the statistics of the year ---//

			float oldExpenseArray[EXPENSE_TYPE_COUNT];
			memcpy( oldExpenseArray, nationPtr->cur_year_expense_array, sizeof(oldExpenseArray) );
			float oldExpense = nationPtr->cur_year_expense, oldProfit = nationPtr->cur_year_profit;
			float oldFixedExpense = nationPtr->cur_year_fixed_expense;

			nationPtr->cash += firm_res[entryPtr->id]->setup_cost;

			//--- a Seat of Power: the kingdom gets the scroll of its race ---//

			int isBase = entryPtr->id == FIRM_BASE;
			int oldKnow = isBase ? nationPtr->know_base_array[raceId-1] : 0;

			if( isBase )
				set_scroll(nationRecno, raceId, 1);

			int firmRecno = firm_array.build_firm( xLoc, yLoc, nationRecno, entryPtr->id, race_res[raceId]->code );

			nationPtr->cash = oldCash;
			memcpy( nationPtr->cur_year_expense_array, oldExpenseArray, sizeof(oldExpenseArray) );
			nationPtr->cur_year_expense = oldExpense;
			nationPtr->cur_year_profit = oldProfit;
			nationPtr->cur_year_fixed_expense = oldFixedExpense;

			if( !firmRecno )
			{
				if( isBase && !oldKnow )
					set_scroll(nationRecno, raceId, 0);

				return 0;
			}

			firm_array[firmRecno]->complete_construction();
			return 1;
		}

		case PLACE_RAW:
			return site_array.add_site( xLoc, yLoc, SITE_RAW, entryPtr->id, mod_editor.place_reserve*1000 ) != 0;

		case PLACE_LAIR:
		{
			int monsterId = mod_editor.place_monster_id;

			if( !monsterId )
				monsterId = next_monster(0, 1);

			if( !monsterId )
				return 0;

			return monster_res[monsterId]->build_firm_monster(xLoc, yLoc) != 0;
		}

		case PLACE_TREE:
		case PLACE_CLEAR:
		case PLACE_ROCK:
		case PLACE_GROUND:
		case PLACE_SHAPE:
			return editor_terrain_place( terrain_place_type(entryPtr), entryPtr->id, xLoc, yLoc, mod_editor.place_brush );
	}

	return 0;
}
//----------- End of static function place_object -----------//


//-------- Begin of static function can_place_unit --------//
//
static int can_place_unit(int unitId, int xLoc, int yLoc)
{
	UnitInfo* unitInfo = unit_res[unitId];
	SpriteInfo* spriteInfo = sprite_res[unitInfo->sprite_id];
	int mobileType = unitInfo->mobile_type;

	if( mobileType != UNIT_LAND )		// ships are on even locations
	{
		xLoc &= ~1;
		yLoc &= ~1;
	}

	int xLoc2 = xLoc + spriteInfo->loc_width - 1;
	int yLoc2 = yLoc + spriteInfo->loc_height - 1;

	if( xLoc2 >= MAX_WORLD_X_LOC || yLoc2 >= MAX_WORLD_Y_LOC )
		return 0;

	for( int y=yLoc ; y<=yLoc2 ; y++ )
	{
		for( int x=xLoc ; x<=xLoc2 ; x++ )
		{
			if( !world.get_loc(x, y)->can_move(mobileType) )
				return 0;
		}
	}

	return 1;
}
//----------- End of static function can_place_unit -----------//


//-------- Begin of static function disp_selected --------//
//
// Display the selected object: its name, its kingdom, its properties (or
// the units in a building) and the buttons.
//
static void disp_selected(SelectLayout& layout)
{
	int x1=INFO_X1, x2=INFO_X2, y1=layout.y1, y2=INFO_Y2;

	vga_util.d3_panel_up( x1, y1, x2, y2 );

	int nationRecno = -1;		// -1: the object has no kingdom
	int isSpy = 0;
	String str;

	if( firm_array.selected_recno )
	{
		Firm* firmPtr = firm_array[firm_array.selected_recno];
		str = firmPtr->firm_name();
		nationRecno = firmPtr->firm_id==FIRM_MONSTER ? -1 : firmPtr->nation_recno;
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

		if( unit_array.selected_count > 1 )
		{
			snprintf( str, MAX_STR_LEN+1, _("%d units"), unit_array.selected_count );
		}
		else
			str = unitPtr->unit_name();

		nationRecno = unitPtr->nation_recno;

		if( unit_array.selected_count == 1 && unitPtr->spy_recno )		// a spy: its true kingdom
		{
			nationRecno = unitPtr->true_nation_recno();
			isSpy = 1;
		}
	}

	if( !str[0] )
	{
		font_san.put( x1+8, y1+5, _("Selected:") );
		font_san.put_paragraph( x1+8, y1+22, x2-8, y2-4,
			_("Nothing. Click to select, Esc to deselect. Right click moves the units or puts them in a building.") );
		return;
	}

	font_san.put( x1+8, y1+4, str, 0, x2-8 );

	//------- the kingdom -------//

	if( nationRecno >= 0 )
	{
		int x = x1+8;

		if( nationRecno > 0 && !nation_array.is_deleted(nationRecno) )
		{
			vga_back.bar( x, y1+20, x+11, y1+31, nation_array[nationRecno]->nation_color );
			x += 16;
		}

		str = owner_name(nationRecno);

		if( isSpy )
		{
			str += " - ";
			str += _("Spy");
		}

		font_san.put( x, y1+19, str, 0, x2-8 );
	}

	//------- a building: its units and the buttons to take them out -------//

	if( firm_array.selected_recno )
	{
		Firm* firmPtr = firm_array[firm_array.selected_recno];
		int y = layout.info_y;

		if( firm_res[firmPtr->firm_id]->need_overseer )
		{
			str = _("General");

			if( firmPtr->firm_id != FIRM_CAMP )
				str = _("Overseer");

			str += ": ";

			if( firmPtr->overseer_recno )
				str += unit_array[firmPtr->overseer_recno]->unit_name(0);
			else
				str += _("None");

			font_san.put( x1+8, y, str, 0, x2-8 );
			y += 14;
		}

		if( firmPtr->worker_array )
		{
			snprintf( str, MAX_STR_LEN+1, "%s: %d / %d",
				firmPtr->firm_id==FIRM_CAMP ? _("Soldiers") : _("Workers"), firmPtr->worker_count, MAX_WORKER );

			font_san.put( x1+8, y, str, 0, x2-8 );
			y += 14;
		}

		int spyCount = place_spy_count(SPY_FIRM, firm_array.selected_recno);

		if( spyCount )
		{
			snprintf( str, MAX_STR_LEN+1, "%s: %d", _("Spies"), spyCount );
			font_san.put( x1+8, y, str, 0, x2-8 );
		}

		if( layout.out_y >= 0 )
		{
			int buttonY = layout.out_y;

			editor_put_button( x1+6, buttonY, x1+100, buttonY+15, _("All out"),
				firmPtr->overseer_recno || firmPtr->worker_count, 0 );

			editor_put_button( x1+106, buttonY, x2-6, buttonY+15,
				firmPtr->firm_id==FIRM_CAMP ? _("General out") : _("Overseer out"),
				firmPtr->overseer_recno != 0, 0 );
		}
	}

	//------- a town: its spies and the button to take them out -------//

	if( town_array.selected_recno && layout.spy_y >= 0 )
	{
		snprintf( str, MAX_STR_LEN+1, "%s: %d", _("Spies"), place_spy_count(SPY_TOWN, town_array.selected_recno) );
		font_san.put( x1+8, layout.info_y, str, 0, x2-8 );

		editor_put_button( x1+6, layout.spy_y, x1+100, layout.spy_y+15, _("Spies out"), 1, 0 );
	}

	//------- the properties (also of buildings: the mineral of a mine) -------//

	{
		static const char* propStr[] = { "", N_("Combat"), "", N_("Version"), N_("Population"), N_("Reserve"),
			N_("Race"), N_("Mineral"), N_("Reserve"), N_("Spying"), N_("Cloak") };

		char* propArray = layout.prop_array;
		int*  valueArray = layout.value_array;

		for( int i=0 ; i<layout.prop_count ; i++ )
		{
			String labelStr, valueStr;

			if( propArray[i] == PROP_SKILL )
				labelStr = unit_array[unit_array.selected_recno]->skill.skill_des();
			else
				labelStr = _(propStr[(int)propArray[i]]);

			switch( propArray[i] )
			{
				case PROP_VERSION:
					valueStr = misc.roman_number(valueArray[i]);
					break;

				case PROP_RACE:
					valueStr = valueArray[i] ? _(race_res[valueArray[i]]->name) : "";
					break;

				case PROP_MINERAL:
					valueStr = valueArray[i] ? _(raw_res[valueArray[i]]->name) : _("None");
					break;

				case PROP_CLOAK:
					valueStr = owner_name(valueArray[i]);
					break;

				default:
					valueStr = misc.format(valueArray[i]);
			}

			int enabled = !(propArray[i]==PROP_RESERVE && site_array[site_array.selected_recno]->has_mine) &&
							  !(propArray[i]==PROP_MINE_RESERVE && valueArray[i]==0);

			put_arrow_row( layout.rows_y+i*20, labelStr, valueStr, enabled );
		}
	}

	//------- the name: Re-roll (people and towns), Rename (kings) -------//

	if( layout.name_y >= 0 )
	{
		int buttonY = layout.name_y;
		int unitRecno = selected_name_unit();
		Unit* unitPtr = unitRecno ? unit_array[unitRecno] : NULL;
		int isKing = unitPtr && unitPtr->rank_id == RANK_KING && unitPtr->nation_recno;

		editor_put_button( x1+6, buttonY, x1+100, buttonY+15, _("Re-roll"),
			!isKing || !mod_editor.lock_kingdom_name[unitPtr->nation_recno-1], 0 );

		if( isKing )
			editor_put_button( x1+106, buttonY, x2-6, buttonY+15, _("Rename"), 1, 0 );
	}

	//------- the buttons -------//

	int buttonY = layout.owner_y;

	editor_put_button( x1+6, buttonY, x1+100, buttonY+16, _("Set Owner"), can_set_owner(), 0 );
	editor_put_button( x1+106, buttonY, x2-6, buttonY+16, _("Delete"), 1, 0 );
}
//----------- End of static function disp_selected -----------//


//-------- Begin of static function selected_properties --------//
//
// The properties of the selected object that can be changed.
//
// <char*> propArray  - for returning the PROP_??? of each property
// <int*>  valueArray - for returning the values
//
// return: <int> the number of properties
//
static int selected_properties(char* propArray, int* valueArray)
{
	if( firm_array.selected_recno )
	{
		Firm* firmPtr = firm_array[firm_array.selected_recno];

		//--- buildings have no race (the one of a Seat of Power is chosen when it is placed) ---//

		if( firmPtr->firm_id != FIRM_MINE )
			return 0;

		FirmMine* minePtr = (FirmMine*) firmPtr;

		propArray[0]  = PROP_MINERAL;
		valueArray[0] = minePtr->raw_id;
		propArray[1]  = PROP_MINE_RESERVE;
		valueArray[1] = minePtr->raw_id ? (int) minePtr->reserve_qty : 0;
		return 2;
	}

	if( town_array.selected_recno )
	{
		Town* townPtr = town_array[town_array.selected_recno];

		propArray[0]  = PROP_RACE;
		valueArray[0] = townPtr->majority_race();
		propArray[1]  = PROP_POPULATION;
		valueArray[1] = townPtr->population;
		return 2;
	}

	if( site_array.selected_recno )
	{
		Site* sitePtr = site_array[site_array.selected_recno];

		if( sitePtr->site_type != SITE_RAW )
			return 0;

		propArray[0]  = PROP_RESERVE;
		valueArray[0] = sitePtr->reserve_qty;
		return 1;
	}

	if( unit_array.selected_recno && unit_array.selected_count==1 )
	{
		Unit* unitPtr = unit_array[unit_array.selected_recno];
		int count=0;

		switch( unit_res[unitPtr->unit_id]->unit_class )
		{
			case UNIT_CLASS_HUMAN:
				if( can_change_race(unitPtr) )
				{
					propArray[count]    = PROP_RACE;
					valueArray[count++] = unitPtr->race_id;
				}

				propArray[count]    = PROP_COMBAT;
				valueArray[count++] = unitPtr->skill.combat_level;

				if( unitPtr->skill.skill_id && unitPtr->skill.skill_id != SKILL_SPYING )
				{
					propArray[count]    = PROP_SKILL;		// leadership or the skill of a worker
					valueArray[count++] = unitPtr->skill.skill_level;
				}

				if( unitPtr->spy_recno )
				{
					propArray[count]    = PROP_SPYING;
					valueArray[count++] = spy_array[unitPtr->spy_recno]->spy_skill;

					if( unitPtr->is_visible() && unitPtr->rank_id != RANK_KING )
					{
						propArray[count]    = PROP_CLOAK;
						valueArray[count++] = unitPtr->nation_recno;
					}
				}
				break;

			case UNIT_CLASS_WEAPON:
				propArray[count]    = PROP_VERSION;
				valueArray[count++] = unitPtr->get_weapon_version();
				break;

			case UNIT_CLASS_MONSTER:		// 7kmod-gameplay: an invoked Fryhtan
				if( editor_is_invoked_fryhtan(unitPtr) )
				{
					propArray[count]    = PROP_COMBAT;
					valueArray[count++] = unitPtr->skill.combat_level;
				}
				break;
		}

		return count;
	}

	//------ several units: the race of the people among them ------//

	if( unit_array.selected_recno )
	{
		for( int i=unit_array.size() ; i>0 ; i-- )
		{
			if( unit_array.is_deleted(i) )
				continue;

			Unit* unitPtr = unit_array[i];

			if( unitPtr->selected_flag && can_change_race(unitPtr) )
			{
				propArray[0]  = PROP_RACE;
				valueArray[0] = unitPtr->race_id;
				return 1;
			}
		}
	}

	return 0;
}
//----------- End of static function selected_properties -----------//


//-------- Begin of static function change_property --------//
//
static void change_property(int propId, int dir)
{
	switch( propId )
	{
		case PROP_COMBAT:
		{
			Unit* unitPtr = unit_array[unit_array.selected_recno];
			int value = unitPtr->skill.combat_level;

			unitPtr->set_combat_level( MAX( 10, MIN(100, value/10*10 + dir*10) ) );
			break;
		}

		case PROP_SKILL:
		{
			Unit* unitPtr = unit_array[unit_array.selected_recno];
			int value = unitPtr->skill.skill_level;

			unitPtr->skill.skill_level = MAX( 10, MIN(100, value/10*10 + dir*10) );
			break;
		}

		case PROP_SPYING:
		{
			Spy* spyPtr = spy_array[unit_array[unit_array.selected_recno]->spy_recno];
			spyPtr->spy_skill = MAX( 10, MIN(100, spyPtr->spy_skill/10*10 + dir*10) );
			break;
		}

		case PROP_CLOAK:		// independent, then the kingdoms
		{
			Unit* unitPtr = unit_array[unit_array.selected_recno];
			int newNationRecno = next_owner(unitPtr->nation_recno, dir);

			if( newNationRecno != unitPtr->nation_recno )
				unitPtr->spy_change_nation(newNationRecno, COMMAND_AUTO);

			sys.map_need_redraw = 1;
			break;
		}

		case PROP_VERSION:
		{
			Unit* unitPtr = unit_array[unit_array.selected_recno];
			int version = unitPtr->get_weapon_version() + dir;

			if( version >= 1 && version <= weapon_max_version(unitPtr->unit_id) )
				set_weapon_version( unitPtr, version );
			break;
		}

		case PROP_POPULATION:
		{
			Town* townPtr = town_array[town_array.selected_recno];
			int raceId = townPtr->majority_race();

			if( dir > 0 )
			{
				int addPop = MIN( 5, MAX_TOWN_POPULATION - townPtr->population );

				if( addPop <= 0 )
					return;

				if( townPtr->nation_recno )
					townPtr->init_pop( raceId, addPop, 80, 0, 1 );
				else
					townPtr->init_pop( raceId, addPop, town_array.independent_town_resistance(), 0, 1 );
			}
			else
			{
				//--- without the death cry of the villagers (Town::kill_town_people() plays it) ---//

				char oldSoundFlag = config.sound_effect_flag;
				config.sound_effect_flag = 0;

				for( int i=0 ; i<5 && townPtr->population > 1 && townPtr->race_pop_array[raceId-1] > 0 ; i++ )
					townPtr->kill_town_people(raceId);

				config.sound_effect_flag = oldSoundFlag;
			}

			townPtr->auto_set_layout();
			sys.zoom_need_redraw = 1;
			break;
		}

		case PROP_RESERVE:
		{
			Site* sitePtr = site_array[site_array.selected_recno];

			if( sitePtr->has_mine )
				return;

			sitePtr->reserve_qty = MAX( 1000, MIN(MAX_RAW_RESERVE_QTY, (sitePtr->reserve_qty/1000 + dir)*1000) );
			break;
		}

		case PROP_RACE:
			if( town_array.selected_recno )
			{
				Town* townPtr = town_array[town_array.selected_recno];
				int oldRaceId = townPtr->majority_race();

				change_town_race( townPtr, next_race(oldRaceId, dir) );

				if( townPtr->majority_race() != oldRaceId )
					reroll_town_name(townPtr);		// a name of the new race
			}
			else
				change_selected_units_race(dir);
			break;

		case PROP_MINERAL:
		{
			FirmMine* minePtr = (FirmMine*) firm_array[firm_array.selected_recno];
			set_mine_mineral( minePtr, (minePtr->raw_id + dir + MAX_RAW+1) % (MAX_RAW+1) );		// none, clay, copper, iron
			break;
		}

		case PROP_MINE_RESERVE:
		{
			FirmMine* minePtr = (FirmMine*) firm_array[firm_array.selected_recno];

			if( minePtr->raw_id )
				set_mine_reserve( minePtr, ((int)minePtr->reserve_qty/1000 + dir) * 1000 );
			break;
		}
	}
}
//----------- End of static function change_property -----------//


//-------- Begin of static function get_options --------//
//
// The options of the palette of the current tab (and entry).
//
// return: <int> the number of options
//
static int get_options(char* optionArray)
{
	int count=0;

	switch( mod_editor.panel_tab )
	{
		case EDITOR_TAB_UNIT:
		{
			if( mod_editor.place_entry >= 0 && unit_entry_array[(int)mod_editor.place_entry].type == PLACE_FRYHTAN )
			{
				optionArray[count++] = OPTION_FRYHTAN;		// 7kmod-gameplay: an invoked Fryhtan has no race
				optionArray[count++] = OPTION_FRYHTAN_COMBAT;
				break;
			}

			optionArray[count++] = OPTION_RACE;

			int entryType = mod_editor.place_entry >= 0 ? unit_entry_array[(int)mod_editor.place_entry].type : 0;

			if( entryType == PLACE_SPY )
			{
				optionArray[count++] = OPTION_SPYING;
				optionArray[count++] = OPTION_CLOAK;
			}
			else if( entryType == PLACE_WORKER )
			{
				optionArray[count++] = OPTION_SKILL;

				if( mod_editor.place_skill_id )
					optionArray[count++] = OPTION_SKILL_LEVEL;
			}
			else
			{
				optionArray[count++] = OPTION_COMBAT;
				optionArray[count++] = OPTION_LEADERSHIP;
			}
			break;
		}

		case EDITOR_TAB_WEAPON:
			optionArray[count++] = OPTION_VERSION;
			break;

		case EDITOR_TAB_TOWN:
			optionArray[count++] = OPTION_RACE;
			optionArray[count++] = OPTION_POPULATION;
			break;

		case EDITOR_TAB_FIRM:		// buildings have no race, except the Seat of Power
			if( mod_editor.place_entry >= 0 && firm_entry_array[(int)mod_editor.place_entry].id == FIRM_BASE )
				optionArray[count++] = OPTION_RACE;
			break;

		case EDITOR_TAB_NATURE:
			optionArray[count++] = OPTION_RESERVE;
			optionArray[count++] = OPTION_MONSTER;
			break;

		case EDITOR_TAB_TERRAIN:
			optionArray[count++] = OPTION_BRUSH;
			break;
	}

	return count;
}
//----------- End of static function get_options -----------//


//-------- Begin of static function option_str --------//
//
static void option_str(int optionId, String& labelStr, String& valueStr)
{
	switch( optionId )
	{
		case OPTION_RACE:
			labelStr = _("Race");
			valueStr = _(race_res[mod_editor.place_race_id]->name);
			break;

		case OPTION_COMBAT:
			labelStr = _("Combat");
			valueStr = mod_editor.place_combat_level;
			break;

		case OPTION_LEADERSHIP:
			labelStr = _("Leadership");
			valueStr = mod_editor.place_leadership;
			break;

		case OPTION_SKILL:
			labelStr = _("Skill");
			valueStr = mod_editor.place_skill_id ? _(Skill::skill_str_array[mod_editor.place_skill_id-1]) : _("None");
			break;

		case OPTION_SKILL_LEVEL:
			labelStr = _("Level");
			valueStr = mod_editor.place_skill_level;
			break;

		case OPTION_VERSION:
			labelStr = _("Version");
			valueStr = misc.roman_number(mod_editor.place_weapon_version);
			break;

		case OPTION_POPULATION:
			labelStr = _("Population");
			valueStr = mod_editor.place_population;
			break;

		case OPTION_RESERVE:
			labelStr = _("Reserve");
			valueStr = misc.format(mod_editor.place_reserve*1000);
			break;

		case OPTION_SPYING:
			labelStr = _("Spying");
			valueStr = mod_editor.place_spy_skill;
			break;

		case OPTION_CLOAK:
			labelStr = _("Cloak");

			if( mod_editor.place_cloak_nation < 0 || mod_editor.place_cloak_nation == mod_editor.owner_nation_recno ||
				 (mod_editor.place_cloak_nation > 0 && nation_array.is_deleted(mod_editor.place_cloak_nation)) )
			{
				valueStr = _("Own");
			}
			else
				valueStr = owner_name(mod_editor.place_cloak_nation);
			break;

		case OPTION_BRUSH:
			labelStr = _("Brush");
			snprintf( valueStr, MAX_STR_LEN+1, "%d x %d", mod_editor.place_brush, mod_editor.place_brush );
			break;

		case OPTION_FRYHTAN:
			labelStr = _("Type");
			valueStr = editor_fryhtan_name( mod_editor.place_fryhtan_unit_id );
			break;

		case OPTION_FRYHTAN_COMBAT:
			labelStr = _("Combat");
			valueStr = mod_editor.place_fryhtan_combat;
			break;

		case OPTION_MONSTER:
		{
			labelStr = _("Fryhtan");

			int monsterId = mod_editor.place_monster_id;

			if( !monsterId )
				monsterId = mod_editor.place_monster_id = next_monster(0, 1);

			valueStr = monsterId ? _(monster_res[monsterId]->name) : "";
			break;
		}
	}
}
//----------- End of static function option_str -----------//


//-------- Begin of static function change_option --------//
//
static void change_option(int optionId, int dir)
{
	switch( optionId )
	{
		case OPTION_RACE:
			mod_editor.place_race_id += dir;

			if( mod_editor.place_race_id < 1 )
				mod_editor.place_race_id = race_res.race_count;

			if( mod_editor.place_race_id > race_res.race_count )
				mod_editor.place_race_id = 1;
			break;

		case OPTION_COMBAT:
			mod_editor.place_combat_level = MAX( 10, MIN(100, mod_editor.place_combat_level + dir*10) );
			break;

		case OPTION_LEADERSHIP:
			mod_editor.place_leadership = MAX( 10, MIN(100, mod_editor.place_leadership + dir*10) );
			break;

		case OPTION_SKILL:
		{
			int skillCount = sizeof(worker_skill_array);
			int i;

			for( i=0 ; i<skillCount && worker_skill_array[i] != mod_editor.place_skill_id ; i++ );

			i = (i + dir + skillCount) % skillCount;
			mod_editor.place_skill_id = worker_skill_array[i];
			break;
		}

		case OPTION_SKILL_LEVEL:
			mod_editor.place_skill_level = MAX( 10, MIN(100, mod_editor.place_skill_level + dir*10) );
			break;

		case OPTION_VERSION:
			mod_editor.place_weapon_version = MAX( 1, MIN(3, mod_editor.place_weapon_version + dir) );
			break;

		case OPTION_POPULATION:
			mod_editor.place_population = MAX( 5, MIN(MAX_TOWN_POPULATION, mod_editor.place_population + dir*5) );
			break;

		case OPTION_RESERVE:
			mod_editor.place_reserve = MAX( 1, MIN(MAX_RAW_RESERVE_QTY/1000, mod_editor.place_reserve + dir) );
			break;

		case OPTION_MONSTER:
			mod_editor.place_monster_id = next_monster(mod_editor.place_monster_id, dir);
			break;

		case OPTION_BRUSH:
			mod_editor.place_brush = MAX( 1, MIN(5, mod_editor.place_brush + dir*2) );
			break;

		case OPTION_FRYHTAN:
			mod_editor.place_fryhtan_unit_id = editor_next_fryhtan( mod_editor.place_fryhtan_unit_id, dir );
			break;

		case OPTION_FRYHTAN_COMBAT:
			mod_editor.place_fryhtan_combat = MAX( 10, MIN(100, mod_editor.place_fryhtan_combat + dir*10) );
			break;

		case OPTION_SPYING:
			mod_editor.place_spy_skill = MAX( 10, MIN(100, mod_editor.place_spy_skill + dir*10) );
			break;

		case OPTION_CLOAK:
		{
			//--- the list: own kingdom (-1), independent (0), the other kingdoms ---//

			int cloakNation = mod_editor.place_cloak_nation;

			if( cloakNation > 0 && (nation_array.is_deleted(cloakNation) || cloakNation == mod_editor.owner_nation_recno) )
				cloakNation = -1;

			for( int i=0 ; i<=MAX_NATION+1 ; i++ )
			{
				cloakNation += dir;

				if( cloakNation > MAX_NATION )
					cloakNation = -1;

				if( cloakNation < -1 )
					cloakNation = MAX_NATION;

				if( cloakNation <= 0 ||
					 (cloakNation <= nation_array.size() && !nation_array.is_deleted(cloakNation) &&
					  cloakNation != mod_editor.owner_nation_recno) )
				{
					break;
				}
			}

			mod_editor.place_cloak_nation = cloakNation;
			break;
		}
	}
}
//----------- End of static function change_option -----------//


//-------- Begin of static function can_reroll_unit --------//
//
// People have a name of the list of their race (weapons and ships are numbered).
//
static int can_reroll_unit(Unit* unitPtr)
{
	return unit_res[unitPtr->unit_id]->unit_class == UNIT_CLASS_HUMAN && unitPtr->race_id && unitPtr->name_id &&
			 !unitPtr->spy_recno;		// spies keep a copy of their name (Spy::name_id)
}
//----------- End of static function can_reroll_unit -----------//


//-------- Begin of static function reroll_unit_name --------//
//
// Another name of the list of the race. A king: also the name of the kingdom,
// unless it is locked (Kingdoms window).
//
// return: <int> 1 - the name has changed
//
static int reroll_unit_name(Unit* unitPtr)
{
	if( !can_reroll_unit(unitPtr) )
		return 0;

	int isKing = unitPtr->rank_id == RANK_KING && unitPtr->nation_recno;

	if( isKing && mod_editor.lock_kingdom_name[unitPtr->nation_recno-1] )
		return 0;

	RaceInfo* raceInfo = race_res[unitPtr->race_id];

	uint16_t newNameId = raceInfo->get_new_name_id();		// before freeing the old one: not the same name again
	raceInfo->free_name_id(unitPtr->name_id);
	unitPtr->name_id = newNameId;

	if( isKing )
		nation_array[unitPtr->nation_recno]->nation_name_id = newNameId;

	return 1;
}
//----------- End of static function reroll_unit_name -----------//


//-------- Begin of static function reroll_town_name --------//
//
// Another name of the list of the race of the town; the buildings named after
// it (Firm::closest_town_name_id) take the new name.
//
static void reroll_town_name(Town* townPtr)
{
	int raceId = townPtr->majority_race();

	if( !raceId )
		return;

	int oldNameId = townPtr->town_name_id;
	int newNameId = town_res.get_new_name_id(raceId);		// before freeing the old one: not the same name again

	if( oldNameId )
		town_res.free_name_id(oldNameId);

	townPtr->town_name_id = newNameId;

	for( int i=firm_array.size() ; i>0 ; i-- )
	{
		if( !firm_array.is_deleted(i) && firm_array[i]->closest_town_name_id == oldNameId )
			firm_array[i]->closest_town_name_id = newNameId;
	}
}
//----------- End of static function reroll_town_name -----------//


//-------- Begin of static function selected_name_unit --------//
//
// return: <int> the unit selected alone whose name can change, 0 - none
//
static int selected_name_unit()
{
	if( firm_array.selected_recno || town_array.selected_recno || site_array.selected_recno ||
		 !unit_array.selected_recno || unit_array.selected_count != 1 )
	{
		return 0;
	}

	return can_reroll_unit(unit_array[unit_array.selected_recno]) ? unit_array.selected_recno : 0;
}
//----------- End of static function selected_name_unit -----------//


//-------- Begin of static function set_scroll --------//
//
// Give or take the scroll of power of a race to a kingdom (it can build a
// Seat of Power of the race), as when it is found in the game.
//
static void set_scroll(int nationRecno, int raceId, int knowFlag)
{
	if( !nationRecno || nation_array.is_deleted(nationRecno) || raceId < 1 )
		return;

	for( int godId=1 ; godId<=god_res.god_count ; godId++ )
	{
		GodInfo* godInfo = god_res[godId];

		if( godInfo->race_id != raceId )
			continue;

		if( knowFlag )
			godInfo->enable_know(nationRecno);
		else
			godInfo->disable_know(nationRecno);

		return;
	}

	nation_array[nationRecno]->know_base_array[raceId-1] = knowFlag;
}
//----------- End of static function set_scroll -----------//


//-------- Begin of static function update_scroll --------//
//
// After a Seat of Power is removed or changes its kingdom: the kingdom has
// the scroll of the race if it has a Seat of the race.
//
static void update_scroll(int nationRecno, int raceId)
{
	if( !nationRecno || nation_array.is_deleted(nationRecno) || raceId < 1 )
		return;

	set_scroll( nationRecno, raceId, nation_array[nationRecno]->base_count_array[raceId-1] > 0 );
}
//----------- End of static function update_scroll -----------//


//-------- Begin of static function entry_name --------//
//
static const char* entry_name(PlaceEntry* entryPtr)
{
	switch( entryPtr->type )
	{
		case PLACE_SOLDIER:	return _("Soldier");
		case PLACE_GENERAL:	return _("General");
		case PLACE_WORKER:	return _("Worker");
		case PLACE_SPY:		return _("Spy");
		case PLACE_UNIT:		return _(unit_res[entryPtr->id]->name);
		case PLACE_FRYHTAN:	return _("Fryhtan");
		case PLACE_TOWN:		return _("Town");
		case PLACE_FIRM:
		{
			const char* name = _(firm_res[entryPtr->id]->name);

			if( font_san.text_width(name) > ENTRY_WIDTH-4 )		// the names that don't fit
			{
				if( entryPtr->id == FIRM_RESEARCH )
					return _("Twr. Science");		// "Twr of Science" doesn't fit either

				if( entryPtr->id == FIRM_BASE )
					return _("Seat of Pwr");
			}

			return name;
		}
		case PLACE_RAW:		return _(raw_res[entryPtr->id]->name);
		case PLACE_LAIR:		return _("Fryhtan Lair");
		case PLACE_TREE:		return _("Trees");
		case PLACE_CLEAR:		return _("Clear");
		case PLACE_ROCK:		return entryPtr->id ? _("Large rock") : _("Small rock");
		case PLACE_GROUND:
			switch( entryPtr->id )
			{
				case TERRAIN_DARK_GRASS:	return _("Dark grass");
				case TERRAIN_LIGHT_GRASS:	return _("Light grass");
				default:							return _("Dirt");
			}

		case PLACE_SHAPE:
			switch( entryPtr->id )
			{
				case EDITOR_TERRAIN_WATER:		return _("Water");
				case EDITOR_TERRAIN_LAND:		return _("Land");
				case EDITOR_TERRAIN_RAMP:		return _("Ramp");
				default:							return _("Hill");
			}
	}

	return "";
}
//----------- End of static function entry_name -----------//


//-------- Begin of static function weapon_max_version --------//
//
// The highest version of a weapon: the maximum level of its technology.
//
static int weapon_max_version(int unitId)
{
	for( int i=1 ; i<=tech_res.tech_count ; i++ )
	{
		if( tech_res[i]->unit_id == unitId )
			return MAX(1, tech_res[i]->max_tech_level);
	}

	return 1;
}
//----------- End of static function weapon_max_version -----------//


//-------- Begin of static function set_weapon_version --------//
//
// Set the version of a weapon (kept in nation_contribution) and its attack.
// A weapon with version 0 (created for a kingdom without the technology)
// takes the attacks of all versions, and the game may crash when choosing the
// sprite of the attack.
//
static void set_weapon_version(Unit* unitPtr, int version)
{
	version = MAX( 1, MIN(weapon_max_version(unitPtr->unit_id), version) );

	unitPtr->set_weapon_version(version);
	unitPtr->fix_attack_info();
}
//----------- End of static function set_weapon_version -----------//


//-------- Begin of static function unit_firm_at --------//
//
// return: <int> the recno of the building of the kingdom of the objects
//               placed at the location, 0 - none
//
static int unit_firm_at(int unitId, int xLoc, int yLoc)
{
	Location* locPtr = world.get_loc(xLoc, yLoc);

	if( !locPtr->is_firm() )
		return 0;

	int firmRecno = locPtr->firm_recno();
	Firm* firmPtr = firm_array[firmRecno];

	if( firmPtr->nation_recno != mod_editor.owner_nation_recno || firmPtr->nation_recno == 0 ||
		 firmPtr->under_construction || !firm_res[firmPtr->firm_id]->need_unit() )
	{
		return 0;
	}

	return firmRecno;
}
//----------- End of static function unit_firm_at -----------//


//-------- Begin of static function assign_to_firm --------//
//
// Put a unit on the map in a building of its kingdom, with the rules of the
// game (Unit::firm_can_assign()). It doesn't replace a worker when the
// building is full.
//
// return: <int> 1 - the unit is in the building
//
static int assign_to_firm(int unitRecno, int firmRecno)
{
	Unit* unitPtr = unit_array[unitRecno];
	Firm* firmPtr = firm_array[firmRecno];

	if( unitPtr->nation_recno != firmPtr->nation_recno || !unitPtr->is_visible() )
		return 0;

	switch( unitPtr->firm_can_assign(firmRecno) )
	{
		case 1:		// a worker (soldiers in forts)
			if( !firmPtr->worker_array || firmPtr->is_worker_full() )
				return 0;
			break;

		case 2:		// the overseer (the general of a fort)
			if( unitPtr->rank_id == RANK_SOLDIER )
				return 0;
			break;

		default:		// repair (construction workers), spies...
			return 0;
	}

	unitPtr->stop2();
	firmPtr->assign_unit(unitRecno);

	return unit_array.is_deleted(unitRecno) || !unit_array[unitRecno]->is_visible();
}
//----------- End of static function assign_to_firm -----------//


//-------- Begin of static function owner_name --------//
//
static const char* owner_name(int nationRecno)
{
	if( nationRecno <= 0 || nation_array.is_deleted(nationRecno) )
		return _("Independent");

	return nation_array[nationRecno]->nation_name();
}
//----------- End of static function owner_name -----------//


//-------- Begin of static function next_owner --------//
//
// The next kingdom of the list: independent, then the kingdoms.
//
static int next_owner(int nationRecno, int dir)
{
	for( int i=0 ; i<=MAX_NATION ; i++ )
	{
		nationRecno += dir;

		if( nationRecno > MAX_NATION )
			nationRecno = 0;

		if( nationRecno < 0 )
			nationRecno = MAX_NATION;

		if( nationRecno==0 || (nationRecno <= nation_array.size() && !nation_array.is_deleted(nationRecno)) )
			return nationRecno;
	}

	return 0;
}
//----------- End of static function next_owner -----------//


//-------- Begin of static function next_monster --------//
//
// The next Fryhtan that has a lair.
//
static int next_monster(int monsterId, int dir)
{
	for( int i=0 ; i<monster_res.monster_count ; i++ )
	{
		monsterId += dir;

		if( monsterId > monster_res.monster_count )
			monsterId = 1;

		if( monsterId < 1 )
			monsterId = monster_res.monster_count;

		if( monster_res[monsterId]->firm_build_code[0] )
			return monsterId;
	}

	return 0;
}
//----------- End of static function next_monster -----------//


//-------- Begin of static function put_arrow_row --------//
//
// A row with a label, "<", the value and ">".
//
static void put_arrow_row(int y, const char* labelStr, const char* valueStr, int enabled)
{
	font_san.put( INFO_X1+8, y+3, labelStr, 0, INFO_X1+84 );

	editor_put_button( INFO_X1+86, y, INFO_X1+104, y+17, "<", enabled, 0 );
	vga_util.d3_panel_down( INFO_X1+107, y, INFO_X2-25, y+17 );
	font_san.center_put( INFO_X1+107, y, INFO_X2-25, y+17, valueStr );
	editor_put_button( INFO_X2-22, y, INFO_X2-4, y+17, ">", enabled, 0 );
}
//----------- End of static function put_arrow_row -----------//


//-------- Begin of static function detect_arrow_row --------//
//
// return: <int> -1 - "<" clicked, 1 - ">" clicked, 0 - none
//
static int detect_arrow_row(int y)
{
	if( mouse.any_click( INFO_X1+86, y, INFO_X1+104, y+17 ) )
		return -1;

	if( mouse.any_click( INFO_X2-22, y, INFO_X2-4, y+17 ) )
		return 1;

	return 0;
}
//----------- End of static function detect_arrow_row -----------//


//-------- Begin of static function spy_place_at --------//
//
// The town or building at the location where a spy being placed goes in: a
// town of any kingdom (or independent) with room, or a building of a kingdom
// that has room for one more worker (or soldier).
//
// <int&> placePara - for returning the town or firm recno
//
// return: <int> SPY_TOWN, SPY_FIRM or 0 - none
//
static int spy_place_at(int xLoc, int yLoc, int& placePara)
{
	Location* locPtr = world.get_loc(xLoc, yLoc);

	if( locPtr->is_town() )
	{
		placePara = locPtr->town_recno();
		return town_array[placePara]->population < MAX_TOWN_POPULATION ? SPY_TOWN : 0;
	}

	if( locPtr->is_firm() )
	{
		placePara = locPtr->firm_recno();
		Firm* firmPtr = firm_array[placePara];

		if( !firmPtr->nation_recno || firmPtr->firm_id == FIRM_MONSTER || firmPtr->under_construction ||
			 !firm_res[firmPtr->firm_id]->need_unit() || !firmPtr->worker_array || firmPtr->is_worker_full() )
		{
			return 0;
		}

		if( firmPtr->firm_id == FIRM_BASE && firmPtr->race_id != mod_editor.place_race_id )		// only people of its race pray
			return 0;

		return SPY_FIRM;
	}

	return 0;
}
//----------- End of static function spy_place_at -----------//


//-------- Begin of static function place_spy --------//
//
// Place a spy of the kingdom of the objects placed, with the race and spying
// skill of the options:
//
// - on a town (of any kingdom or independent): he lives in it as a villager.
// - on a building of a kingdom: he works in it (a soldier in a Fort).
// - on the map: a unit disguised as the kingdom of the Cloak option.
//
// It uses the functions of the game for spies (SpyArray::add_spy(),
// Unit::spy_change_nation(), Town::assign_unit(), Firm::assign_unit()).
//
// return: <int> 1 - placed
//
static int place_spy(int xLoc, int yLoc)
{
	int nationRecno = mod_editor.owner_nation_recno;
	int unitId = race_res[mod_editor.place_race_id]->basic_unit_id;
	int placePara = 0;
	int spyPlace = spy_place_at(xLoc, yLoc, placePara);

	//--- in a town or a building: create the unit next to it, then put it in ---//

	if( spyPlace )
	{
		int x1, y1, x2, y2;

		if( spyPlace == SPY_TOWN )
		{
			Town* townPtr = town_array[placePara];
			x1 = townPtr->loc_x1; y1 = townPtr->loc_y1; x2 = townPtr->loc_x2; y2 = townPtr->loc_y2;
		}
		else
		{
			Firm* firmPtr = firm_array[placePara];
			x1 = firmPtr->loc_x1; y1 = firmPtr->loc_y1; x2 = firmPtr->loc_x2; y2 = firmPtr->loc_y2;
		}

		xLoc = x1;
		yLoc = y1;

		if( !world.locate_space( &xLoc, &yLoc, x2, y2, 1, 1 ) )
			return 0;
	}

	int unitRecno = unit_array.add_unit( unitId, nationRecno, RANK_SOLDIER, 80, xLoc, yLoc );

	if( !unitRecno )
		return 0;

	Unit* unitPtr = unit_array[unitRecno];

	unitPtr->set_combat_level( CITIZEN_COMBAT_LEVEL );		// a villager without a job, as the spies trained in towns
	unitPtr->skill.skill_id = 0;
	unitPtr->skill.skill_level = 0;

	unitPtr->spy_recno = spy_array.add_spy( unitRecno, mod_editor.place_spy_skill );

	//--- the kingdom he disguises as: the one of the town or building, or the Cloak option ---//

	int cloakNation;

	if( spyPlace == SPY_TOWN )
		cloakNation = town_array[placePara]->nation_recno;
	else if( spyPlace == SPY_FIRM )
		cloakNation = firm_array[placePara]->nation_recno;
	else
	{
		cloakNation = mod_editor.place_cloak_nation;

		if( cloakNation < 0 || (cloakNation > 0 && nation_array.is_deleted(cloakNation)) )
			cloakNation = nationRecno;
	}

	if( cloakNation != nationRecno )
	{
		unitPtr->spy_change_nation( cloakNation, COMMAND_AUTO );

		if( cloakNation )
			unitPtr->loyalty = 80;		// as the other people placed (Unit::betray() lowers it)
	}

	//------- put him in the town or the building -------//

	if( spyPlace == SPY_TOWN )
	{
		Town* townPtr = town_array[placePara];

		unitPtr->stop2();
		townPtr->assign_unit(unitRecno);
		townPtr->auto_set_layout();
	}
	else if( spyPlace == SPY_FIRM )
	{
		Firm* firmPtr = firm_array[placePara];

		//--- his job in the building: a soldier in a Fort (combat and leadership of the Soldier
		//    options), praying in a Seat of Power, the skill of the building in the others (level of
		//    the Worker option) ---//

		if( firmPtr->firm_id == FIRM_CAMP )
		{
			unitPtr->set_combat_level( mod_editor.place_combat_level );
			unitPtr->skill.set_skill( SKILL_LEADING );
			unitPtr->skill.skill_level = mod_editor.place_leadership;
		}
		else if( firmPtr->firm_id == FIRM_BASE )
		{
			unitPtr->skill.set_skill( SKILL_PRAYING );
			unitPtr->skill.skill_level = mod_editor.place_skill_level;
		}
		else if( firm_res[firmPtr->firm_id]->firm_skill_id )
		{
			unitPtr->skill.set_skill( firm_res[firmPtr->firm_id]->firm_skill_id );
			unitPtr->skill.skill_level = mod_editor.place_skill_level;
		}

		if( !assign_to_firm(unitRecno, placePara) )
			box.msg( _("The unit can't be put in this building; it has been placed next to it.") );
	}

	return 1;
}
//----------- End of static function place_spy -----------//


//-------- Begin of static function place_spy_count --------//
//
// The number of spies (of all kingdoms) in a town or a building.
//
static int place_spy_count(int spyPlace, int placePara)
{
	int count=0;

	for( int i=spy_array.size() ; i>0 ; i-- )
	{
		if( spy_array.is_deleted(i) )
			continue;

		Spy* spyPtr = spy_array[i];

		if( spyPtr->spy_place == spyPlace && spyPtr->spy_place_para == placePara )
			count++;
	}

	return count;
}
//----------- End of static function place_spy_count -----------//


//-------- Begin of static function take_out_town_spies --------//
//
// The spies (of all kingdoms) of a town come out as units next to it.
//
static void take_out_town_spies(Town* townPtr)
{
	for( int i=spy_array.size() ; i>0 ; i-- )
	{
		if( spy_array.is_deleted(i) )
			continue;

		Spy* spyPtr = spy_array[i];

		if( spyPtr->spy_place == SPY_TOWN && spyPtr->spy_place_para == townPtr->town_recno &&
			 townPtr->population > 1 )		// a town can't be left empty
		{
			spyPtr->mobilize_town_spy();
		}
	}

	townPtr->auto_set_layout();
}
//----------- End of static function take_out_town_spies -----------//


//-------- Begin of static function select_layout --------//
//
// The layout of the box of the selected object: it grows upwards with the
// lines and rows it needs, the buttons stay at the bottom.
//
static void select_layout(SelectLayout& layout)
{
	layout.prop_count = selected_properties(layout.prop_array, layout.value_array);
	layout.info_count = 0;

	int hasOutButtons = 0;

	if( firm_array.selected_recno )
	{
		Firm* firmPtr = firm_array[firm_array.selected_recno];

		if( firm_res[firmPtr->firm_id]->need_overseer )
			layout.info_count++;

		if( firmPtr->worker_array )
			layout.info_count++;

		hasOutButtons = firmPtr->firm_id != FIRM_MONSTER;

		if( place_spy_count(SPY_FIRM, firm_array.selected_recno) )
			layout.info_count++;
	}

	int hasSpyButton = 0;

	if( town_array.selected_recno && place_spy_count(SPY_TOWN, town_array.selected_recno) )
	{
		layout.info_count++;
		hasSpyButton = 1;
	}

	int infoHeight = layout.info_count ? layout.info_count*14+2 : 0;

	int hasNameButtons = town_array.selected_recno || selected_name_unit();

	int height = 36 + infoHeight + layout.prop_count*20 + (hasOutButtons ? 20 : 0) + (hasNameButtons ? 20 : 0) +
					 (hasSpyButton ? 20 : 0) + 20 + 3;

	layout.y1      = INFO_Y2 - MAX(height, (int)SELECT_MIN_HEIGHT);
	layout.info_y  = layout.y1 + 36;
	layout.rows_y  = layout.info_y + infoHeight;
	layout.owner_y = INFO_Y2 - 19;
	layout.out_y   = hasOutButtons ? layout.owner_y - 17 : -1;
	layout.name_y  = hasNameButtons ? layout.owner_y - 17 : -1;
	layout.spy_y   = hasSpyButton ? layout.owner_y - (hasNameButtons ? 34 : 17) : -1;
}
//----------- End of static function select_layout -----------//


//-------- Begin of static function can_change_race --------//
//
// Units whose race can be changed: people on the map (not spies).
//
static int can_change_race(Unit* unitPtr)
{
	return unit_res[unitPtr->unit_id]->unit_class == UNIT_CLASS_HUMAN &&
			 unitPtr->race_id && !unitPtr->spy_recno && !unitPtr->unit_mode &&
			 unitPtr->is_visible() && !unitPtr->is_unit_dead();
}
//----------- End of static function can_change_race -----------//


//-------- Begin of static function next_race --------//
//
static int next_race(int raceId, int dir)
{
	raceId += dir;

	if( raceId < 1 )
		raceId = race_res.race_count;

	if( raceId > race_res.race_count )
		raceId = 1;

	return raceId;
}
//----------- End of static function next_race -----------//


//-------- Begin of static function change_town_race --------//
//
// The villagers without a job change to the new race; the workers of the
// buildings keep their race (they are counted in the buildings), and so do
// the spies.
//
static void change_town_race(Town* townPtr, int newRaceId)
{
	for( int raceId=1 ; raceId<=MAX_RACE ; raceId++ )
	{
		if( raceId == newRaceId )
			continue;

		int count = townPtr->recruitable_race_pop(raceId, 0);		// 0-without the spies

		if( count <= 0 )
			continue;

		//--- a new race in the town: it starts with the loyalty and the resistance of the old one ---//

		if( townPtr->race_pop_array[newRaceId-1] == 0 )
		{
			townPtr->race_loyalty_array[newRaceId-1] = townPtr->race_loyalty_array[raceId-1];
			townPtr->race_target_loyalty_array[newRaceId-1] = townPtr->race_target_loyalty_array[raceId-1];

			for( int j=0 ; j<MAX_NATION ; j++ )
			{
				townPtr->race_resistance_array[newRaceId-1][j] = townPtr->race_resistance_array[raceId-1][j];
				townPtr->race_target_resistance_array[newRaceId-1][j] = townPtr->race_target_resistance_array[raceId-1][j];
			}
		}

		int loyalty = (int) townPtr->race_loyalty_array[raceId-1];

		//--- one by one; the population never reaches 0 (the town would be deleted) nor goes over the maximum ---//

		for( int i=0 ; i<count ; i++ )
		{
			if( townPtr->population > 1 )
			{
				townPtr->dec_pop(raceId, 0);
				townPtr->inc_pop(newRaceId, 0, loyalty);
			}
			else
			{
				townPtr->inc_pop(newRaceId, 0, loyalty);
				townPtr->dec_pop(raceId, 0);
			}
		}
	}

	townPtr->auto_set_layout();
	town_array.distribute_demand();
}
//----------- End of static function change_town_race -----------//


//-------- Begin of static function change_unit_race --------//
//
// The look of a person depends on its race (each race has its own unit),
// so a new unit of the new race replaces it at the same place, with the
// same kingdom, rank, skills, loyalty and team, and a name of the new race.
// A king changes the race of his kingdom; he takes a name of the new race
// too (so does the kingdom), unless the name of the kingdom is locked.
//
// return: <int> the recno of the new unit, 0 - not changed
//
static int change_unit_race(int unitRecno, int newRaceId)
{
	Unit* oldUnit = unit_array[unitRecno];

	if( !can_change_race(oldUnit) || oldUnit->race_id == newRaceId )
		return 0;

	int newUnitId = race_res[newRaceId]->basic_unit_id;
	int oldRaceId = oldUnit->race_id;
	int nationRecno = oldUnit->nation_recno;
	int rankId = oldUnit->rank_id;
	int xLoc = oldUnit->next_x_loc();
	int yLoc = oldUnit->next_y_loc();

	//------- the new unit, next to the old one -------//

	int newXLoc = xLoc, newYLoc = yLoc;

	if( !world.locate_space(&newXLoc, &newYLoc, xLoc, yLoc, 1, 1) )
		return 0;

	int newRecno = unit_array.add_unit( newUnitId, nationRecno, rankId==RANK_KING ? RANK_GENERAL : rankId,
													oldUnit->loyalty, newXLoc, newYLoc );
	if( !newRecno )
		return 0;

	oldUnit = unit_array[unitRecno];		// the array may have been reallocated
	Unit* newUnit = unit_array[newRecno];

	Skill oldSkill = oldUnit->skill;
	newUnit->skill = oldSkill;
	newUnit->set_combat_level( oldSkill.combat_level );
	newUnit->hit_points = (float) newUnit->max_hit_points;

	//------- the team: its general, or its soldiers -------//

	if( oldUnit->leader_unit_recno && !unit_array.is_deleted(oldUnit->leader_unit_recno) )
	{
		Unit* leaderUnit = unit_array[oldUnit->leader_unit_recno];
		newUnit->leader_unit_recno = oldUnit->leader_unit_recno;

		if( leaderUnit->team_info )
		{
			for( int i=0 ; i<leaderUnit->team_info->member_count ; i++ )
			{
				if( leaderUnit->team_info->member_unit_array[i] == unitRecno )
					leaderUnit->team_info->member_unit_array[i] = newRecno;
			}
		}
	}

	if( oldUnit->team_info && newUnit->team_info )
		memcpy( newUnit->team_info, oldUnit->team_info, sizeof(TeamInfo) );

	if( rankId == RANK_GENERAL || rankId == RANK_KING )
	{
		for( int i=unit_array.size() ; i>0 ; i-- )
		{
			if( !unit_array.is_deleted(i) && unit_array[i]->leader_unit_recno == unitRecno )
				unit_array[i]->leader_unit_recno = newRecno;
		}
	}

	//------- the king: the race of the kingdom changes; the name too, unless it is locked -------//

	if( rankId == RANK_KING )
	{
		Nation* nationPtr = nation_array[nationRecno];

		char kingName[HUMAN_NAME_LEN+1];
		strncpy( kingName, nationPtr->king_name(), HUMAN_NAME_LEN );
		kingName[HUMAN_NAME_LEN] = '\0';

		int oldNameId = nationPtr->nation_name_id;

		nationPtr->set_king(newRecno, 1);		// the race of the kingdom is the race of its king

		if( !mod_editor.lock_kingdom_name[nationRecno-1] )
		{
			nationPtr->nation_name_id = newUnit->name_id;		// the new unit has a name of the new race (re-roll)
		}
		else if( oldNameId < 0 )
			nationPtr->nation_name_id = oldNameId;		// a custom name
		else
		{
			nation_array.set_human_name(nationRecno, kingName);		// keep the name, which is looked up by race otherwise
			nationPtr->nation_name_id = -nationRecno;
		}

		//--- the Seat of Power known at the start (config.seat_of_power_unlocked) follows the race ---//

		if( config.seat_of_power_unlocked && !nationPtr->is_ai() )
		{
			for( int godId=1 ; godId<=god_res.god_count ; godId++ )
			{
				GodInfo* godInfo = god_res[godId];

				if( godInfo->race_id == oldRaceId && godInfo->is_nation_know(nationRecno) )
					godInfo->disable_know(nationRecno);

				if( godInfo->race_id == newRaceId )
					godInfo->enable_know(nationRecno);
			}
		}
	}

	//------- delete the old unit (without the death of a king or a general), move the new one in its place -------//

	unit_array[unitRecno]->deinit_unit_id();		// the counts of units of the kingdom (skipped by signal_exit_flag)

	char oldExitFlag = sys.signal_exit_flag;
	sys.signal_exit_flag = 2;
	unit_array.del(unitRecno);
	sys.signal_exit_flag = oldExitFlag;

	newUnit = unit_array[newRecno];
	newUnit->stop2();
	newUnit->deinit_sprite(1);

	int destX = xLoc, destY = yLoc;

	if( world.get_loc(xLoc, yLoc)->can_move(newUnit->mobile_type) )		// the place of the old unit (locate_space() only looks around it)
		newUnit->init_sprite(xLoc, yLoc);

	else if( world.locate_space( &destX, &destY, xLoc, yLoc, newUnit->sprite_info->loc_width, newUnit->sprite_info->loc_height ) )
		newUnit->init_sprite(destX, destY);
	else
		newUnit->init_sprite(newXLoc, newYLoc);

	newUnit->stop2();

	return newRecno;
}
//----------- End of static function change_unit_race -----------//


//-------- Begin of static function change_selected_units_race --------//
//
// All the selected people change to the next race of the first of them.
//
static void change_selected_units_race(int dir)
{
	short unitArray[MAX_UNIT_PER_ORDER];
	int	unitCount=0, raceId=0;

	for( int i=unit_array.size() ; i>0 && unitCount<MAX_UNIT_PER_ORDER ; i-- )
	{
		if( unit_array.is_deleted(i) )
			continue;

		Unit* unitPtr = unit_array[i];

		if( unitPtr->selected_flag && can_change_race(unitPtr) )
		{
			if( !raceId )
				raceId = unitPtr->race_id;

			unitArray[unitCount++] = i;
		}
	}

	if( !raceId )
		return;

	int newRaceId = next_race(raceId, dir);

	for( int i=0 ; i<unitCount ; i++ )
	{
		int newRecno = change_unit_race(unitArray[i], newRaceId);

		if( newRecno )
			unit_array[newRecno]->selected_flag = 1;
	}

	//------ recount the selection ------//

	unit_array.selected_count = 0;
	int selectedRecno = 0;

	for( int i=unit_array.size() ; i>0 ; i-- )
	{
		if( !unit_array.is_deleted(i) && unit_array[i]->selected_flag && unit_array[i]->is_visible() )
		{
			unit_array.selected_count++;

			if( !selectedRecno || unit_array[i]->rank_id > unit_array[selectedRecno]->rank_id )
				selectedRecno = i;
		}
	}

	unit_array.selected_recno = selectedRecno;
	sys.map_need_redraw = 1;
}
//----------- End of static function change_selected_units_race -----------//


//-------- Begin of static function set_mine_mineral --------//
//
// A mine takes its mineral and reserve from the raw site under it; the
// editor changes both. Without a site, choosing a mineral creates one with
// the reserve of the Nature tab; changing the mineral keeps the reserve;
// none deletes the site. What the mine has already mined is emptied.
//
static void set_mine_mineral(FirmMine* minePtr, int rawId)
{
	int oldRawId = minePtr->raw_id;

	if( rawId == oldRawId )
		return;

	Nation* nationPtr = minePtr->nation_recno ? nation_array[minePtr->nation_recno] : NULL;

	if( oldRawId && nationPtr )
		nationPtr->raw_count_array[oldRawId-1]--;

	if( !rawId )
	{
		if( minePtr->site_recno )
		{
			site_array.untapped_raw_count++;		// SiteArray::del_site() counts it as an untapped site
			site_array.del_site(minePtr->site_recno);
			minePtr->site_recno = 0;
		}

		minePtr->reserve_qty = (float) 0;
	}
	else
	{
		int reserveQty = (int) minePtr->reserve_qty;

		if( reserveQty <= 0 )
			reserveQty = mod_editor.place_reserve * 1000;

		if( minePtr->site_recno )
		{
			site_array[minePtr->site_recno]->object_id = rawId;
		}
		else
		{
			int xLoc = minePtr->center_x, yLoc = minePtr->center_y;

			site_array.add_site( xLoc, yLoc, SITE_RAW, rawId, reserveQty );

			minePtr->site_recno = world.get_loc(xLoc, yLoc)->site_recno();

			site_array[minePtr->site_recno]->has_mine = 1;
			site_array.untapped_raw_count--;
		}

		site_array[minePtr->site_recno]->reserve_qty = reserveQty;
		minePtr->reserve_qty = (float) reserveQty;
	}

	minePtr->raw_id    = rawId;
	minePtr->stock_qty = (float) 0;

	if( rawId && nationPtr )
		nationPtr->raw_count_array[rawId-1]++;
}
//----------- End of static function set_mine_mineral -----------//


//-------- Begin of static function set_mine_reserve --------//
//
static void set_mine_reserve(FirmMine* minePtr, int reserveQty)
{
	reserveQty = MAX( 1000, MIN(MAX_RAW_RESERVE_QTY, reserveQty) );

	minePtr->reserve_qty = (float) reserveQty;

	if( minePtr->site_recno )
		site_array[minePtr->site_recno]->reserve_qty = reserveQty;
}
//----------- End of static function set_mine_reserve -----------//


//-------- Begin of static function terrain_place_type --------//
//
// return: <int> EDITOR_TERRAIN_??? of an entry of the Terrain tab
//
static int terrain_place_type(PlaceEntry* entryPtr)
{
	switch( entryPtr->type )
	{
		case PLACE_TREE:	return EDITOR_TERRAIN_TREE;
		case PLACE_ROCK:	return EDITOR_TERRAIN_ROCK;
		case PLACE_CLEAR:	return EDITOR_TERRAIN_CLEAR;
		case PLACE_SHAPE:	return entryPtr->id;
		default:				return EDITOR_TERRAIN_GROUND;
	}
}
//----------- End of static function terrain_place_type -----------//
