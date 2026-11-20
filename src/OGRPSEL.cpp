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

#include <memory.h>
#include <OGRPSEL.h>
#include <OPOWER.h>
#include <OUNIT.h>
#include <ONATION.h>
#include <OINFO.h>
#include <OSYS.h>
#include <OVGA.h>
#include <OFONT.h>
#include <OMOUSE.h>
#include <OWORLD.h>
#include <OMISC.h>
#include <OSE.h>
#include <OHELP.h>
#include <ModEditor.h>
#include <gettext.h>

//--- 7kmod: the buttons of the groups, in a column above the clear news button (ONEWS.cpp) ---//

enum { GROUP_BUTTON_X1 = ZOOM_X2-13,		// as the news buttons: NEWS_X2+3
		 GROUP_BUTTON_SIZE = 10,
		 GROUP_BUTTON_SPACE = 12,
		 GROUP_BUTTON_Y9 = ZOOM_Y2-5-38 };		// the button of group 9, above the clear news button (NEWS_Y2-26)

static int group_button_y1(int groupNum) { return GROUP_BUTTON_Y9 - (MAX_SELECT_GROUP_NUM-groupNum) * GROUP_BUTTON_SPACE; }

static char group_has_units[MAX_SELECT_GROUP_NUM+1];		// set by disp_buttons(), for detect_buttons()

enum { GROUP_KEY_DOUBLE_TIME = 500 };		// Alt+number twice within this (ms) also centers the map on the group
static int           last_key_group;
static unsigned long last_key_time;

// the colors of the news buttons (NEWS_LOG, NEWS_CLR): light and dark border, face
enum { GROUP_BUTTON_LIGHT = 0x4F, GROUP_BUTTON_DARK = 0x99, GROUP_BUTTON_FACE = 0x5E,
		 GROUP_BUTTON_FACE_DOWN = 0x5C, GROUP_BUTTON_DIGIT = 0x93 };

// the digits 1-9 in 3x5 pixels, as the lines of the news buttons; a row per char, bit 2 = left
static const unsigned char group_digit_font[10][5] =
{
	{ 7,5,5,5,7 }, { 2,6,2,2,7 }, { 7,1,7,4,7 }, { 7,1,3,1,7 }, { 5,5,7,1,1 },
	{ 7,4,7,1,7 }, { 7,4,7,5,7 }, { 7,1,1,2,2 }, { 7,5,7,5,7 }, { 7,5,7,1,7 },
};

//--------------- begin of function GroupSelect::GroupSelect ----------------//
GroupSelect::GroupSelect()
{
	//memset(this, 0, sizeof(GroupSelect));
}
//--------------- end of function GroupSelect::GroupSelect ----------------//


//--------------- begin of function GroupSelect::~GroupSelect ----------------//
GroupSelect::~GroupSelect()
{
	deinit();
}
//--------------- end of function GroupSelect::~GroupSelect ----------------//


//--------------- begin of function GroupSelect::init ----------------//
void GroupSelect::init()
{
}
//--------------- end of function GroupSelect::init ----------------//


//--------------- begin of function GroupSelect::deinit ----------------//
void GroupSelect::deinit()
{
}
//--------------- end of function GroupSelect::deinit ----------------//


//--------------- begin of function GroupSelect::group_units ----------------//
// <int> groupNum = 1 - MAX_SELECT_GROUP_NUM
//
void GroupSelect::group_units(int groupNum)
{
	err_when(groupNum<1 || groupNum>MAX_SELECT_GROUP_NUM);

	Unit	*unitPtr;
	int playerNation;

	if(!unit_array.selected_recno)
		return; // invalid call
	else
	{
		playerNation = nation_array.player_recno;
		unitPtr = unit_array[unit_array.selected_recno];
		if(unitPtr->nation_recno!=playerNation)
			return;
	}

	//---------- group selected units ----------//
	int arraySize = unit_array.size();
	for(int i=arraySize; i>0; --i)
	{
		if(unit_array.is_deleted(i))
			continue;

		unitPtr = unit_array[i];
		if(unitPtr->nation_recno!=playerNation)
			continue;


		if(unitPtr->selected_flag)
				unitPtr->group_select_id = groupNum; // new selected unit
		else if(unitPtr->group_select_id==groupNum) // old member in this group
			unitPtr->group_select_id = 0;
	}

	info.disp();		// 7kmod: the group in the unit panel and in the list of units
}
//--------------- end of function GroupSelect::group_units ----------------//


//--------------- begin of function GroupSelect::select_grouped_units ----------------//
// <int> groupNum = 1 - MAX_SELECT_GROUP_NUM
//
int GroupSelect::select_grouped_units(int groupNum)
{
	err_when(groupNum<1 || groupNum>MAX_SELECT_GROUP_NUM);

	int arraySize = unit_array.size();
	Unit	*unitPtr;
	int	playerNation = nation_array.player_recno;

	//---------- count selected units ----------//
	int groupUnitCount = 0;
	int i;
	for(i=arraySize; i>0; --i)
	{
		if(unit_array.is_deleted(i))
			continue;

		unitPtr = unit_array[i];

		if(unitPtr->nation_recno!=playerNation)
			continue;

		if(!unitPtr->is_visible())
			continue;

		if(unitPtr->group_select_id==groupNum)
		{
			groupUnitCount++;
			break;
		}
	}

	if(!groupUnitCount)
		return 0;
	
	power.reset_selection();

	//---------- select units in this group ----------//
	Unit	*showSelectedUnitPtr = NULL;
	unit_array.selected_count = 0;
	for(i=arraySize; i>0; --i)
	{
		if(unit_array.is_deleted(i))
			continue;

		unitPtr = unit_array[i];

		if(unitPtr->nation_recno!=playerNation)
			continue;

		if(!unitPtr->is_visible())
			continue;

		if(unitPtr->group_select_id==groupNum)
		{
			unitPtr->selected_flag = 1;
			unit_array.selected_count++;

			if(showSelectedUnitPtr==NULL || unitPtr->hit_points>showSelectedUnitPtr->hit_points)
				showSelectedUnitPtr = unitPtr;
		}
	}

	if(showSelectedUnitPtr)
	{
		unit_array.selected_recno = showSelectedUnitPtr->sprite_recno;
		info.disp();
		return showSelectedUnitPtr->sprite_recno;
	}

	return 0;
}
//--------------- end of function GroupSelect::select_grouped_units ----------------//


//--------------- begin of function GroupSelect::key_select_group ----------------//
//
// 7kmod: Alt+number selects the group; the same number again within
// GROUP_KEY_DOUBLE_TIME also centers the map on it (on the unit shown in the
// panel), as a double click on its button.
//
void GroupSelect::key_select_group(int groupNum)
{
	unsigned long curTime = misc.get_time();
	int doublePress = groupNum == last_key_group && curTime - last_key_time < GROUP_KEY_DOUBLE_TIME;

	int unitRecno = select_grouped_units(groupNum);

	if( doublePress && unitRecno )
	{
		Unit* unitPtr = unit_array[unitRecno];
		world.go_loc( unitPtr->next_x_loc(), unitPtr->next_y_loc() );

		last_key_group = 0;		// a third press is a new first one
	}
	else
	{
		last_key_group = groupNum;
		last_key_time  = curTime;
	}
}
//--------------- end of function GroupSelect::key_select_group ----------------//


//--------------- begin of function GroupSelect::disp_buttons ----------------//
//
// 7kmod: a column of small buttons 1-9 above the clear news button, each in
// its own place, only for the groups with units on the map. A click selects
// the group (as Alt+number) and a double click also centers the map on it.
//
void GroupSelect::disp_buttons()
{
	memset( group_has_units, 0, sizeof(group_has_units) );

	int playerNation = nation_array.player_recno;

	if( !playerNation || sys.view_mode != MODE_NORMAL || mod_editor.is_editing() )
		return;

	for( int i=unit_array.size() ; i>0 ; i-- )
	{
		if( unit_array.is_deleted(i) )
			continue;

		Unit* unitPtr = unit_array[i];

		if( unitPtr->group_select_id > 0 && unitPtr->group_select_id <= MAX_SELECT_GROUP_NUM &&
			 unitPtr->nation_recno == playerNation && unitPtr->is_visible() )
		{
			group_has_units[(int)unitPtr->group_select_id] = 1;
		}
	}

	int selectedGroup = selected_group();

	vga.use_back();

	for( int g=1 ; g<=MAX_SELECT_GROUP_NUM ; g++ )
	{
		if( !group_has_units[g] )
			continue;

		int x1 = GROUP_BUTTON_X1, y1 = group_button_y1(g);
		int x2 = x1+GROUP_BUTTON_SIZE-1, y2 = y1+GROUP_BUTTON_SIZE-1;

		int pressed = g == selectedGroup;		// the selected group, pressed

		vga_back.bar( x1, y1, x2, y2, pressed ? GROUP_BUTTON_LIGHT : GROUP_BUTTON_DARK );
		vga_back.bar( x1, y1, x2-1, y2-1, pressed ? GROUP_BUTTON_DARK : GROUP_BUTTON_LIGHT );
		vga_back.bar( x1+1, y1+1, x2-1, y2-1, pressed ? GROUP_BUTTON_FACE_DOWN : GROUP_BUTTON_FACE );

		for( int row=0 ; row<5 ; row++ )		// the digit, centered
		{
			for( int col=0 ; col<3 ; col++ )
			{
				if( group_digit_font[g][row] & (4>>col) )
					vga_back.bar( x1+4+col, y1+3+row, x1+4+col, y1+3+row, GROUP_BUTTON_DIGIT );
			}
		}
	}

	vga.use_front();
}
//--------------- end of function GroupSelect::disp_buttons ----------------//


//--------------- begin of function GroupSelect::detect_buttons ----------------//
//
int GroupSelect::detect_buttons()
{
	if( !nation_array.player_recno || sys.view_mode != MODE_NORMAL || mod_editor.is_editing() )
		return 0;

	for( int g=1 ; g<=MAX_SELECT_GROUP_NUM ; g++ )
	{
		if( !group_has_units[g] )
			continue;

		int x1 = GROUP_BUTTON_X1, y1 = group_button_y1(g);
		int x2 = x1+GROUP_BUTTON_SIZE-1, y2 = y1+GROUP_BUTTON_SIZE-1;

		int doubleClick = mouse.double_click(x1, y1, x2, y2);

		if( doubleClick || mouse.single_click(x1, y1, x2, y2) )
		{
			int unitRecno = select_grouped_units(g);

			if( doubleClick && unitRecno )		// a double click also centers the map on the group
			{
				Unit* unitPtr = unit_array[unitRecno];
				world.go_loc( unitPtr->next_x_loc(), unitPtr->next_y_loc() );
			}

			se_ctrl.immediate_sound("TURN_ON");
			return 1;
		}
	}

	return 0;
}
//--------------- end of function GroupSelect::detect_buttons ----------------//


//--------------- begin of function GroupSelect::selected_group ----------------//
//
// 7kmod: the group whose units on the map are exactly the selected units, 0
// if the selection isn't a group.
//
int GroupSelect::selected_group()
{
	if( !unit_array.selected_recno || unit_array.is_deleted(unit_array.selected_recno) )
		return 0;

	int groupNum = unit_array[unit_array.selected_recno]->group_select_id;

	if( groupNum < 1 || groupNum > MAX_SELECT_GROUP_NUM )
		return 0;

	int playerNation = nation_array.player_recno;

	for( int i=unit_array.size() ; i>0 ; i-- )
	{
		if( unit_array.is_deleted(i) )
			continue;

		Unit* unitPtr = unit_array[i];

		if( !unitPtr->is_visible() || unitPtr->nation_recno != playerNation )
			continue;

		if( (unitPtr->group_select_id == groupNum) != (unitPtr->selected_flag != 0) )		// in the group but not selected, or selected but not in the group
			return 0;
	}

	return groupNum;
}
//--------------- end of function GroupSelect::selected_group ----------------//

