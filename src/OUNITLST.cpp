/*
 * Seven Kingdoms: Ancient Adversaries
 *
 * Copyright 2026 7kmod
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

//Filename    : OUNITLST.CPP
//Description : 7kmod - list of the selected units in the unit panel

#include <stdlib.h>
#include <string.h>
#include <OVGA.h>
#include <vga_util.h>
#include <OFONT.h>
#include <OMOUSE.h>
#include <OHELP.h>
#include <OINFO.h>
#include <OBUTTON.h>
#include <OPOWER.h>
#include <OSKILL.h>
#include <OUNIT.h>
#include <OUNITRES.h>
#include <ONATION.h>
#include <OSE.h>
#include <OMISC.h>
#include <OSTR.h>
#include <OUNITLST.h>
#include "gettext.h"

//------- layout of the list, like the list of soldiers of a fort -------//

#define LIST_COLUMN_COUNT		4
#define LIST_ROW_COUNT			8
#define LIST_PAGE_SIZE			(LIST_COLUMN_COUNT*LIST_ROW_COUNT)
#define LIST_CELL_WIDTH			50
#define LIST_CELL_HEIGHT		29

//------- static vars -------//

static char   list_open_flag;							// whether the list replaces the unit panel
static short  list_page;									// the page displayed
static Button button_toggle;							// ">" / "<"
static Button button_list_prev, button_list_next;

static short  inspect_unit_array[MAX_UNIT_LIST];	// units of other kingdoms in the last selection box
static short  inspect_unit_count;

// The recno of a unit that dies or enters a building is given at once to
// the next unit created (DynArrayB::linkin()), anywhere on the map. These
// identify the units of the list, to remove the recnos that are reused.

static short  inspect_unit_id_array[MAX_UNIT_LIST];
static uint16_t inspect_name_id_array[MAX_UNIT_LIST];		// as Unit::name_id (a short would make the names over 32767 negative and never equal)
static char   inspect_nation_array[MAX_UNIT_LIST];

static short  last_list_recno_array[MAX_UNIT_LIST];	// to know when the list changes
static short  last_list_count;
static short  last_list_page;

static int	  sort_main_nation_recno;					// for sorting the inspect list

//------- static functions -------//

static int  is_inspect_unit(int unitRecno);
static int  is_valid_inspect_unit(int listId);
static void purge_inspect_list();
static int  sort_own_function(const void* a, const void* b);
static int  sort_inspect_function(const void* a, const void* b);
static int  is_civilian_skill(int skillId);
static void disp_hit_points(int x1, int y1, int x2, int hitPoints, int maxHitPoints);
static void select_one_unit(int unitRecno);


//--------- Begin of function unit_list_units ---------//
//
// Return the units of the list shown in the panel of the given unit, sorted.
//
// - If it is a unit of the player and several units are selected, the
//   list is the selected units.
// - If it is a unit of another kingdom (or without kingdom) which is in the
//   inspect list of the last selection box, the list is the inspect list.
//
// <Unit*>  mainUnit   - the unit displayed in the unit panel
// <short*> recnoArray - the array to fill (MAX_UNIT_LIST)
//
// return: <int> the number of units, 0 if there is no list
//
int unit_list_units(Unit* mainUnit, short* recnoArray)
{
	int unitCount = 0;

	if( mainUnit->is_own() )
	{
		if( unit_array.selected_count < 2 || !mainUnit->selected_flag )
			return 0;

		for( int i=1 ; i<=unit_array.size() && unitCount<MAX_UNIT_LIST ; i++ )
		{
			if( unit_array.is_deleted(i) )
				continue;

			Unit* unitPtr = unit_array[i];

			if( unitPtr->selected_flag && unitPtr->is_own() && unitPtr->is_visible() &&
				 unitPtr->hit_points > 0 )
			{
				recnoArray[unitCount++] = i;
			}
		}

		qsort( recnoArray, unitCount, sizeof(short), sort_own_function );
	}
	else
	{
		purge_inspect_list();

		if( !is_inspect_unit(mainUnit->sprite_recno) )
			return 0;

		for( int i=0 ; i<inspect_unit_count ; i++ )
		{
			int unitRecno = inspect_unit_array[i];

			if( unit_array.is_deleted(unitRecno) )
				continue;

			Unit* unitPtr = unit_array[unitRecno];

			if( unitPtr->is_visible() && unitPtr->hit_points > 0 && !unitPtr->is_own() )
				recnoArray[unitCount++] = unitRecno;
		}

		sort_main_nation_recno = mainUnit->nation_recno;

		qsort( recnoArray, unitCount, sizeof(short), sort_inspect_function );
	}

	return unitCount;
}
//----------- End of function unit_list_units -----------//


//--------- Begin of function unit_list_is_open ---------//
//
int unit_list_is_open()
{
	return list_open_flag;
}
//----------- End of function unit_list_is_open -----------//


//--------- Begin of function unit_list_close ---------//
//
void unit_list_close()
{
	list_open_flag = 0;
	list_page = 0;
}
//----------- End of function unit_list_close -----------//


//--------- Begin of function unit_list_disp_toggle ---------//
//
// Display the ">" (open the list) or "<" (back to the unit panel) button.
//
void unit_list_disp_toggle(int x1, int y1, int refreshFlag)
{
	if( refreshFlag == INFO_REPAINT )
		button_toggle.paint_text( x1, y1, x1+17, y1+17, list_open_flag ? "<" : ">" );

	help.set_custom_help( x1, y1, x1+17, y1+17,
		list_open_flag ? _("Back to the unit") : _("List of the units") );
}
//----------- End of function unit_list_disp_toggle -----------//


//--------- Begin of function unit_list_reset_toggle ---------//
//
void unit_list_reset_toggle()
{
	button_toggle.reset();
}
//----------- End of function unit_list_reset_toggle -----------//


//--------- Begin of function unit_list_detect_toggle ---------//
//
int unit_list_detect_toggle()
{
	if( !button_toggle.init_flag || !button_toggle.detect() )
		return 0;

	list_open_flag = !list_open_flag;
	list_page = 0;

	se_ctrl.immediate_sound("TURN_ON");
	info.disp();
	return 1;
}
//----------- End of function unit_list_detect_toggle -----------//


//--------- Begin of function unit_list_disp ---------//
//
// Display the list in place of the rest of the unit panel.
//
// <Unit*> mainUnit - the unit displayed in the unit panel
// <int>   dispY1   - the top of the list panel
//
void unit_list_disp(Unit* mainUnit, int dispY1, int refreshFlag)
{
	short recnoArray[MAX_UNIT_LIST];

	int unitCount = unit_list_units(mainUnit, recnoArray);
	int pageCount = (unitCount+LIST_PAGE_SIZE-1) / LIST_PAGE_SIZE;

	if( list_page >= pageCount )
		list_page = MAX(0, pageCount-1);

	//--- if the units of the list have changed, paint the whole interface again ---//
	//
	// The panel must not be painted again over itself: it is not opaque, it
	// lightens what is below it, so it would get lighter each time.
	//
	//----------------------------------------------------------------------//

	int listChanged = unitCount != last_list_count || list_page != last_list_page ||
							memcmp(recnoArray, last_list_recno_array, sizeof(short)*unitCount) != 0;

	last_list_count = unitCount;
	last_list_page  = list_page;
	memcpy( last_list_recno_array, recnoArray, sizeof(short)*unitCount );

	if( refreshFlag == INFO_UPDATE && listChanged )
	{
		info.disp();
		return;
	}

	//---------- paint the panel ---------//

	int stripY1 = INFO_Y2 - 24;		// the page strip at the bottom

	if( refreshFlag == INFO_REPAINT )
	{
		vga_util.d3_panel_up( INFO_X1, dispY1, INFO_X2, INFO_Y2 );

		if( pageCount > 1 )
		{
			String str;

			str  = _("Page");
			str += " ";
			str += list_page+1;
			str += " / ";
			str += pageCount;

			font_san.put( INFO_X1+6, stripY1+5, str );

			button_list_prev.paint_text( INFO_X2-40, stripY1+2, INFO_X2-23, stripY1+20, "<" );
			button_list_next.paint_text( INFO_X2-20, stripY1+2, INFO_X2-3, stripY1+20, ">" );
		}
		else
		{
			button_list_prev.reset();
			button_list_next.reset();
		}
	}

	//---------- display the units ----------//

	int ownList = mainUnit->is_own();
	int firstId = list_page * LIST_PAGE_SIZE;

	for( int i=firstId ; i<unitCount && i<firstId+LIST_PAGE_SIZE ; i++ )
	{
		Unit*     unitPtr  = unit_array[recnoArray[i]];
		UnitInfo* unitInfo = unit_res[unitPtr->unit_id];

		int x = INFO_X1+4 + (i-firstId)%LIST_COLUMN_COUNT*LIST_CELL_WIDTH;
		int y = dispY1+4  + (i-firstId)/LIST_COLUMN_COUNT*LIST_CELL_HEIGHT;

		if( refreshFlag == INFO_REPAINT )
		{
			vga_front.put_bitmap( x+2, y+2, unitInfo->get_small_icon_ptr(unitPtr->rank_id) );

			//--- the color of the kingdom, for units of other kingdoms ---//

			if( !ownList && unitPtr->nation_recno )
			{
				vga_front.bar( x+2, y+2, x+7, y+7, nation_array[unitPtr->nation_recno]->nation_color );
				vga_front.rect( x+2, y+2, x+7, y+7, 1, V_BLACK );
			}
		}

		//--- highlight the unit displayed in the unit panel ---//

		vga_front.rect( x, y, x+27, y+23, 2,
			recnoArray[i] == mainUnit->sprite_recno ? V_YELLOW : vga_front.color_up );

		//------ hit points bar -------//

		disp_hit_points( x+2, y+24, x+25, (int) unitPtr->hit_points, unitPtr->max_hit_points );

		//--- combat level, or skill level for civilians, only for the player's units ---//

		if( ownList && unitInfo->unit_class != UNIT_CLASS_WEAPON &&
			 unitInfo->unit_class != UNIT_CLASS_SHIP && unitInfo->unit_class != UNIT_CLASS_CARAVAN )
		{
			int level;

			if( unitPtr->race_id && is_civilian_skill(unitPtr->skill.skill_id) )
				level = unitPtr->skill.skill_level;
			else
				level = unitPtr->skill.combat_level;

			font_san.disp( x+30, y+6, level, 1, x+LIST_CELL_WIDTH-1 );
		}

		//------- help: the name of the unit -------//

		if( mouse.in_area(x, y, x+27, y+23) )
			help.set_custom_help( x, y, x+27, y+23, unitPtr->unit_name() );
	}
}
//----------- End of function unit_list_disp -----------//


//--------- Begin of function unit_list_detect ---------//
//
// Left click on a unit: select only that unit and show its panel.
// Right click on a unit: remove it from the selection (or the inspect list).
//
int unit_list_detect(Unit* mainUnit, int dispY1)
{
	short recnoArray[MAX_UNIT_LIST];

	int unitCount = unit_list_units(mainUnit, recnoArray);
	int pageCount = (unitCount+LIST_PAGE_SIZE-1) / LIST_PAGE_SIZE;

	//-------- page buttons --------//

	if( pageCount > 1 )
	{
		if( button_list_prev.init_flag && button_list_prev.detect() )
		{
			if( list_page > 0 )
				list_page--;
			info.disp();
			return 1;
		}

		if( button_list_next.init_flag && button_list_next.detect() )
		{
			if( list_page < pageCount-1 )
				list_page++;
			info.disp();
			return 1;
		}
	}

	//-------- units --------//

	int ownList = mainUnit->is_own();
	int firstId = list_page * LIST_PAGE_SIZE;

	for( int i=firstId ; i<unitCount && i<firstId+LIST_PAGE_SIZE ; i++ )
	{
		int x = INFO_X1+4 + (i-firstId)%LIST_COLUMN_COUNT*LIST_CELL_WIDTH;
		int y = dispY1+4  + (i-firstId)/LIST_COLUMN_COUNT*LIST_CELL_HEIGHT;

		int unitRecno = recnoArray[i];

		//--- left click: select only this unit and show its panel ---//

		if( mouse.any_click(x, y, x+27, y+23, LEFT_BUTTON) )
		{
			unit_list_close();

			// for the player's units, the others are no longer selected;
			// for units of other kingdoms, the inspect list is kept and ">" shows it again

			select_one_unit(unitRecno);
			return 1;
		}

		//--- right click: remove it from the selection or the inspect list ---//

		if( mouse.any_click(x, y, x+27, y+23, RIGHT_BUTTON) )
		{
			if( ownList )
			{
				Unit* unitPtr = unit_array[unitRecno];

				unitPtr->selected_flag = 0;
				unit_array.selected_count--;

				if( unit_array.selected_recno == unitRecno )		// the unit of the panel: the next of the list is shown
				{
					unit_array.selected_recno = 0;

					for( int j=0 ; j<unitCount ; j++ )
					{
						if( recnoArray[j] != unitRecno )
						{
							unit_array.selected_recno = recnoArray[j];
							break;
						}
					}
				}
			}
			else
			{
				for( int j=0 ; j<inspect_unit_count ; j++ )
				{
					if( inspect_unit_array[j] == unitRecno )
					{
						inspect_unit_array[j] = 0;		// removed by purge_inspect_list()
						break;
					}
				}

				purge_inspect_list();

				if( mainUnit->sprite_recno == unitRecno )		// the unit of the panel: the next of the list is shown
				{
					for( int j=0 ; j<unitCount ; j++ )
					{
						if( recnoArray[j] != unitRecno )
						{
							select_one_unit(recnoArray[j]);
							return 1;
						}
					}
				}
			}

			info.disp();
			return 1;
		}
	}

	return 0;
}
//----------- End of function unit_list_detect -----------//


//--------- Begin of function unit_list_set_inspect ---------//
//
void unit_list_set_inspect(short* recnoArray, int unitCount)
{
	inspect_unit_count = MIN(unitCount, MAX_UNIT_LIST);

	memcpy( inspect_unit_array, recnoArray, sizeof(short)*inspect_unit_count );

	for( int i=0 ; i<inspect_unit_count ; i++ )
	{
		Unit* unitPtr = unit_array[inspect_unit_array[i]];

		inspect_unit_id_array[i] = unitPtr->unit_id;
		inspect_name_id_array[i] = unitPtr->name_id;
		inspect_nation_array[i]  = unitPtr->nation_recno;
	}
}
//----------- End of function unit_list_set_inspect -----------//


//--------- Begin of function unit_list_clear_inspect ---------//
//
void unit_list_clear_inspect()
{
	inspect_unit_count = 0;
}
//----------- End of function unit_list_clear_inspect -----------//


//--------- Begin of function unit_list_is_inspected ---------//
//
// Whether the given unit is in the inspect list (units of other kingdoms
// in the last selection box) and that list is active, that is, the unit
// displayed in the unit panel is one of them. These units display their
// hit points bar on the map, like the selected units.
//
int unit_list_is_inspected(int unitRecno)
{
	if( !inspect_unit_count || !unit_array.selected_recno || unit_array.selected_count != 1 )
		return 0;

	return is_inspect_unit(unit_array.selected_recno) && is_inspect_unit(unitRecno);
}
//----------- End of function unit_list_is_inspected -----------//


//--------- Begin of static function is_inspect_unit ---------//
//
static int is_inspect_unit(int unitRecno)
{
	for( int i=0 ; i<inspect_unit_count ; i++ )
	{
		if( inspect_unit_array[i] == unitRecno )
			return is_valid_inspect_unit(i);
	}

	return 0;
}
//----------- End of static function is_inspect_unit -----------//


//--------- Begin of static function is_valid_inspect_unit ---------//
//
// Whether the unit of the inspect list is still the same unit: its recno
// may have been given to a new unit.
//
static int is_valid_inspect_unit(int listId)
{
	int unitRecno = inspect_unit_array[listId];

	if( !unitRecno || unit_array.is_deleted(unitRecno) )
		return 0;

	Unit* unitPtr = unit_array[unitRecno];

	return unitPtr->unit_id      == inspect_unit_id_array[listId] &&
			 unitPtr->name_id      == inspect_name_id_array[listId] &&
			 unitPtr->nation_recno == inspect_nation_array[listId];
}
//----------- End of static function is_valid_inspect_unit -----------//


//--------- Begin of static function purge_inspect_list ---------//
//
// Remove the units that are no longer valid from the inspect list.
//
static void purge_inspect_list()
{
	int newCount = 0;

	for( int i=0 ; i<inspect_unit_count ; i++ )
	{
		if( !is_valid_inspect_unit(i) )
			continue;

		inspect_unit_array[newCount]    = inspect_unit_array[i];
		inspect_unit_id_array[newCount] = inspect_unit_id_array[i];
		inspect_name_id_array[newCount] = inspect_name_id_array[i];
		inspect_nation_array[newCount]  = inspect_nation_array[i];
		newCount++;
	}

	inspect_unit_count = newCount;
}
//----------- End of static function purge_inspect_list -----------//


//--------- Begin of static function select_one_unit ---------//
//
static void select_one_unit(int unitRecno)
{
	power.reset_selection();

	Unit* unitPtr = unit_array[unitRecno];

	unitPtr->selected_flag = 1;
	unit_array.selected_recno = unitRecno;
	unit_array.selected_count = 1;

	info.disp();
}
//----------- End of static function select_one_unit -----------//


//--------- Begin of static function is_civilian_skill ---------//
//
static int is_civilian_skill(int skillId)
{
	switch( skillId )
	{
		case SKILL_CONSTRUCTION:
		case SKILL_MINING:
		case SKILL_MFT:
		case SKILL_RESEARCH:
		case SKILL_SPYING:
		case SKILL_PRAYING:
			return 1;
	}

	return 0;
}
//----------- End of static function is_civilian_skill -----------//


//--------- Begin of static function sort_own_function ---------//
//
// The player's units: kings and generals first, then by type of unit and
// by combat level (highest first).
//
static int sort_own_function(const void* a, const void* b)
{
	Unit* unitPtr1 = unit_array[*((short*)a)];
	Unit* unitPtr2 = unit_array[*((short*)b)];

	if( unitPtr1->rank_id != unitPtr2->rank_id )
		return unitPtr2->rank_id - unitPtr1->rank_id;

	if( unitPtr1->unit_id != unitPtr2->unit_id )
		return unitPtr1->unit_id - unitPtr2->unit_id;

	if( unitPtr1->skill.combat_level != unitPtr2->skill.combat_level )
		return unitPtr2->skill.combat_level - unitPtr1->skill.combat_level;

	return unitPtr1->sprite_recno - unitPtr2->sprite_recno;
}
//----------- End of static function sort_own_function -----------//


//--------- Begin of static function sort_inspect_function ---------//
//
// Units of other kingdoms: grouped by kingdom, the kingdom of the unit of
// the panel first and the units without kingdom (wild Fryhtans...) last.
// In each group, kings and generals first, then by type of unit and by hit
// points (the combat level of these units is not revealed).
//
static int sort_inspect_function(const void* a, const void* b)
{
	Unit* unitPtr1 = unit_array[*((short*)a)];
	Unit* unitPtr2 = unit_array[*((short*)b)];

	if( unitPtr1->nation_recno != unitPtr2->nation_recno )
	{
		if( unitPtr1->nation_recno == sort_main_nation_recno )
			return -1;

		if( unitPtr2->nation_recno == sort_main_nation_recno )
			return 1;

		if( !unitPtr1->nation_recno )		// no kingdom: last
			return 1;

		if( !unitPtr2->nation_recno )
			return -1;

		return unitPtr1->nation_recno - unitPtr2->nation_recno;
	}

	if( unitPtr1->rank_id != unitPtr2->rank_id )
		return unitPtr2->rank_id - unitPtr1->rank_id;

	if( unitPtr1->unit_id != unitPtr2->unit_id )
		return unitPtr1->unit_id - unitPtr2->unit_id;

	if( (int) unitPtr1->hit_points != (int) unitPtr2->hit_points )
		return (int) unitPtr2->hit_points - (int) unitPtr1->hit_points;

	return unitPtr1->sprite_recno - unitPtr2->sprite_recno;
}
//----------- End of static function sort_inspect_function -----------//


//--------- Begin of static function disp_hit_points ---------//
//
// The hit points bar, as in the list of workers of a firm.
//
static void disp_hit_points(int x1, int y1, int x2, int hitPoints, int maxHitPoints)
{
	#define HIT_BAR_TYPE_COUNT  3

	int  hit_bar_color_array[HIT_BAR_TYPE_COUNT] = { 0xA8, 0xB4, 0xAC };
	int  hit_bar_max_array[HIT_BAR_TYPE_COUNT] 	= { 50, 100, 200 };
	char hitBarColor = (char) hit_bar_color_array[HIT_BAR_TYPE_COUNT-1];

	for( int i=0 ; i<HIT_BAR_TYPE_COUNT ; i++ )
	{
		if( maxHitPoints <= hit_bar_max_array[i] )
		{
			hitBarColor = (char) hit_bar_color_array[i];
			break;
		}
	}

	enum { HIT_BAR_DARK_BORDER = 3,
			 HIT_BAR_BODY 		   = 1 };

	int barWidth = (x2-x1+1) * hitPoints / MAX(1, MAX(hitPoints, maxHitPoints));

	vga_front.bar( x1, y1, x1+barWidth-1, y1+1, hitBarColor + HIT_BAR_BODY );

	if( x1+barWidth <= x2 )
		vga_util.blt_buf( x1+barWidth, y1, x2, y1+1, 0 );

	y1+=2;

	vga_front.bar( x1, y1, x1+barWidth-1, y1, hitBarColor + HIT_BAR_DARK_BORDER );
	vga_front.bar( x1+barWidth, y1, x1+barWidth, y1, V_BLACK );

	if( x1+barWidth+1 <= x2 )
		vga_util.blt_buf( x1+barWidth+1, y1, x2, y1, 0 );

	y1++;

	vga_front.bar( x1+1, y1, x1+barWidth, y1, V_BLACK );

	if( x1+barWidth+1 <= x2 )
		vga_util.blt_buf( x1+barWidth+1, y1, x2, y1, 0 );
}
//----------- End of static function disp_hit_points -----------//
