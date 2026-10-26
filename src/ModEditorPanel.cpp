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
#include <OFIRMID.h>
#include <OSITE.h>
#include <OFIRMDIE.h>
#include <ORAWRES.h>
#include <ORACERES.h>
#include <OMONSRES.h>
#include <OSKILL.h>
#include <OTECHRES.h>
#include <OMISC.h>
#include "gettext.h"

//------- the layout of the panel --------//

enum { OWNER_Y1 = INFO_Y1+4,
		 TAB_Y1 = INFO_Y1+28,
		 TAB_COLUMNS = 3,
		 TAB_WIDTH = 64,
		 TAB_HEIGHT = 20,
		 PALETTE_Y1 = INFO_Y1+74,
		 PALETTE_Y2 = INFO_Y2-104,
		 ENTRY_WIDTH = 97,
		 ENTRY_HEIGHT = 16,
		 OPTION_HEIGHT = 20,
		 SELECT_Y1 = INFO_Y2-100 };

static const char* tab_str[EDITOR_TAB_COUNT] =
{
	N_("Units"), N_("Weapons"), N_("Towns"), N_("Buildings"), N_("Nature")
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
		 PLACE_LAIR };

struct PlaceEntry
{
	char  type;
	short id;
};

static PlaceEntry unit_entry_array[] =
{
	{ PLACE_SOLDIER, 0 }, { PLACE_GENERAL, 0 },
	{ PLACE_WORKER, 0 },
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
};

static PlaceEntry nature_entry_array[] =
{
	{ PLACE_RAW, 1 }, { PLACE_RAW, 2 }, { PLACE_RAW, 3 },
	{ PLACE_LAIR, 0 },
};

static PlaceEntry* tab_entry_array[EDITOR_TAB_COUNT] =
	{ unit_entry_array, weapon_entry_array, town_entry_array, firm_entry_array, nature_entry_array };

static int tab_entry_count[EDITOR_TAB_COUNT] =
{
	sizeof(unit_entry_array)/sizeof(PlaceEntry), sizeof(weapon_entry_array)/sizeof(PlaceEntry),
	sizeof(town_entry_array)/sizeof(PlaceEntry), sizeof(firm_entry_array)/sizeof(PlaceEntry),
	sizeof(nature_entry_array)/sizeof(PlaceEntry)
};

//------- the options of the palettes --------//

enum { OPTION_RACE=1, OPTION_COMBAT, OPTION_LEADERSHIP, OPTION_SKILL, OPTION_SKILL_LEVEL,
		 OPTION_VERSION, OPTION_POPULATION, OPTION_RESERVE, OPTION_MONSTER };

enum { MAX_OPTION = 3 };

static char worker_skill_array[] = { SKILL_MINING, SKILL_MFT, SKILL_RESEARCH, SKILL_CONSTRUCTION };

//------- right click: moving the selected units --------//

enum { MAX_UNIT_PER_ORDER = 200 };

//------- the properties of the selected object --------//

enum { PROP_NONE, PROP_COMBAT, PROP_SKILL, PROP_VERSION, PROP_POPULATION, PROP_RESERVE };

enum { MAX_PROP = 2 };

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
static int	can_set_owner();
static int	can_place_unit(int unitId, int xLoc, int yLoc);
static int	place_object(PlaceEntry* entryPtr, int xLoc, int yLoc);
static void	disp_selected(int y1);
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

	//------- the palette -------//

	vga_util.d3_panel_down( INFO_X1, PALETTE_Y1, INFO_X2, PALETTE_Y2 );

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

	for( int i=0 ; i<optionCount ; i++ )
	{
		String labelStr, valueStr;
		option_str( optionArray[i], labelStr, valueStr );

		put_arrow_row( y, labelStr, valueStr );
		y += OPTION_HEIGHT;
	}

	//------- the help of placing -------//

	if( place_entry >= 0 )
		font_san.put( INFO_X1+6, PALETTE_Y2-16, _("Click on the map to place it."), 0, INFO_X2-4 );

	//------- the selected object -------//

	disp_selected( SELECT_Y1 );

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

	for( int i=0 ; i<optionCount ; i++ )
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

	char propArray[MAX_PROP];
	int  valueArray[MAX_PROP];
	int  propCount = selected_properties(propArray, valueArray);

	for( int i=0 ; i<propCount ; i++ )
	{
		int dir = detect_arrow_row(SELECT_Y1+36+i*20);

		if( dir )
		{
			change_property(propArray[i], dir);
			info.disp();
			return;
		}
	}

	//------- the selected building: take out its units -------//

	if( firm_array.selected_recno )
	{
		int buttonY = SELECT_Y1+64;

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

	//------- Set Owner and Delete -------//

	int buttonY = SELECT_Y1+81;

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

	if( !mouse.any_click(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2) )
		return 1;

	int xLoc = world.zoom_matrix->top_x_loc + (mouse.click_x()-ZOOM_X1)/ZOOM_LOC_WIDTH;
	int yLoc = world.zoom_matrix->top_y_loc + (mouse.click_y()-ZOOM_Y1)/ZOOM_LOC_HEIGHT;

	PlaceEntry* entryPtr = tab_entry_array[(int)panel_tab] + place_entry;

	if( place_object(entryPtr, xLoc, yLoc) )
	{
		sys.zoom_need_redraw = 1;
		sys.map_need_redraw = 1;
		info.disp();
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
		{
			int unitId = entryPtr->type==PLACE_UNIT ? entryPtr->id : race_res[place_race_id]->basic_unit_id;
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
		firm_array[firm_array.selected_recno]->change_nation(nationRecno);
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

		return nationRecno > 0 && unitPtr->nation_recno != nationRecno &&
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
			  entryPtr->type == PLACE_UNIT || entryPtr->type == PLACE_FIRM) )
		{
			box.msg( _("Units and buildings need a kingdom. Choose one at the top of the panel.") );
		}
		return 0;
	}

	int nationRecno = mod_editor.owner_nation_recno;
	int raceId = mod_editor.place_race_id;

	switch( entryPtr->type )
	{
		case PLACE_SOLDIER:
		case PLACE_GENERAL:
		case PLACE_WORKER:
		case PLACE_UNIT:
		{
			int unitId, rankId = RANK_SOLDIER;

			if( entryPtr->type == PLACE_UNIT )
				unitId = entryPtr->id;
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
					unitPtr->skill.set_skill( mod_editor.place_skill_id );
					unitPtr->skill.skill_level = mod_editor.place_skill_level;
					break;

				case PLACE_UNIT:
					//--- weapons: the version must be set, or the attack of the weapon is wrong (it may crash) ---//

					if( unit_res[unitId]->unit_class == UNIT_CLASS_WEAPON )
						set_weapon_version( unitPtr, mod_editor.place_weapon_version );
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

			nationPtr->cash += firm_res[entryPtr->id]->setup_cost;

			int firmRecno = firm_array.build_firm( xLoc, yLoc, nationRecno, entryPtr->id, race_res[raceId]->code );

			nationPtr->cash = oldCash;

			if( !firmRecno )
				return 0;

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
static void disp_selected(int y1)
{
	int x1=INFO_X1, x2=INFO_X2, y2=INFO_Y2;

	vga_util.d3_panel_up( x1, y1, x2, y2 );

	int nationRecno = -1;		// -1: the object has no kingdom
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

		font_san.put( x, y1+19, owner_name(nationRecno), 0, x2-8 );
	}

	//------- a building: its units and the buttons to take them out -------//

	if( firm_array.selected_recno )
	{
		Firm* firmPtr = firm_array[firm_array.selected_recno];
		int y = y1+36;

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
		}

		if( firmPtr->firm_id != FIRM_MONSTER )
		{
			int buttonY = y1+64;

			editor_put_button( x1+6, buttonY, x1+100, buttonY+15, _("All out"),
				firmPtr->overseer_recno || firmPtr->worker_count, 0 );

			editor_put_button( x1+106, buttonY, x2-6, buttonY+15,
				firmPtr->firm_id==FIRM_CAMP ? _("General out") : _("Overseer out"),
				firmPtr->overseer_recno != 0, 0 );
		}
	}

	//------- the properties -------//

	else
	{
		static const char* propStr[] = { "", N_("Combat"), "", N_("Version"), N_("Population"), N_("Reserve") };

		char propArray[MAX_PROP];
		int  valueArray[MAX_PROP];
		int  propCount = selected_properties(propArray, valueArray);

		for( int i=0 ; i<propCount ; i++ )
		{
			String labelStr, valueStr;

			if( propArray[i] == PROP_SKILL )
				labelStr = unit_array[unit_array.selected_recno]->skill.skill_des();
			else
				labelStr = _(propStr[(int)propArray[i]]);

			if( propArray[i] == PROP_VERSION )
				valueStr = misc.roman_number(valueArray[i]);
			else
				valueStr = misc.format(valueArray[i]);

			int enabled = !(propArray[i]==PROP_RESERVE && site_array[site_array.selected_recno]->has_mine);

			put_arrow_row( y1+36+i*20, labelStr, valueStr, enabled );
		}
	}

	//------- the buttons -------//

	int buttonY = y1+81;

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
		return 0;

	if( town_array.selected_recno )
	{
		propArray[0]  = PROP_POPULATION;
		valueArray[0] = town_array[town_array.selected_recno]->population;
		return 1;
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
				propArray[count]    = PROP_COMBAT;
				valueArray[count++] = unitPtr->skill.combat_level;

				if( unitPtr->skill.skill_id && unitPtr->skill.skill_id != SKILL_SPYING )
				{
					propArray[count]    = PROP_SKILL;		// leadership or the skill of a worker
					valueArray[count++] = unitPtr->skill.skill_level;
				}
				break;

			case UNIT_CLASS_WEAPON:
				propArray[count]    = PROP_VERSION;
				valueArray[count++] = unitPtr->get_weapon_version();
				break;
		}

		return count;
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
				for( int i=0 ; i<5 && townPtr->population > 1 && townPtr->race_pop_array[raceId-1] > 0 ; i++ )
					townPtr->kill_town_people(raceId);
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
			optionArray[count++] = OPTION_RACE;

			int isWorker = mod_editor.place_entry >= 0 &&
								unit_entry_array[(int)mod_editor.place_entry].type == PLACE_WORKER;

			if( isWorker )
			{
				optionArray[count++] = OPTION_SKILL;
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

		case EDITOR_TAB_FIRM:
			optionArray[count++] = OPTION_RACE;
			break;

		case EDITOR_TAB_NATURE:
			optionArray[count++] = OPTION_RESERVE;
			optionArray[count++] = OPTION_MONSTER;
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
			valueStr = _(Skill::skill_str_array[mod_editor.place_skill_id-1]);
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
	}
}
//----------- End of static function change_option -----------//


//-------- Begin of static function entry_name --------//
//
static const char* entry_name(PlaceEntry* entryPtr)
{
	switch( entryPtr->type )
	{
		case PLACE_SOLDIER:	return _("Soldier");
		case PLACE_GENERAL:	return _("General");
		case PLACE_WORKER:	return _("Worker");
		case PLACE_UNIT:		return _(unit_res[entryPtr->id]->name);
		case PLACE_TOWN:		return _("Town");
		case PLACE_FIRM:		return _(firm_res[entryPtr->id]->name);
		case PLACE_RAW:		return _(raw_res[entryPtr->id]->name);
		case PLACE_LAIR:		return _("Fryhtan Lair");
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
