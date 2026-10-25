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

//Filename    : OUNITAM.CPP
//Description : Object UnitArray - part 2
//
// For the detail, see ounitam.txt

#include <math.h>

#include <ALL.h>
#include <OVGA.h>
#include <OSYS.h>
#include <OSTR.h>
#include <OREMOTE.h>
#include <OWORLD.h>
#include <OTERRAIN.h>
#include <OUNIT.h>
#include <OUNITRES.h>
#include <dbglog.h>

DBGLOG_DEFAULT_CHANNEL(Unit);

#ifdef NO_DEBUG_UNIT
#undef err_when
#undef err_here
#undef err_if
#undef err_else
#undef err_now
#define err_when(cond)
#define err_here()
#define err_if(cond)
#define err_else
#define err_now(msg)
#undef DEBUG
#endif

//----------- Define static variables -----------//

//------ static variables for all actions -------//
short	UnitArray::selected_land_unit_count;
short	UnitArray::selected_sea_unit_count;
short	UnitArray::selected_air_unit_count;
short	*UnitArray::selected_land_unit_array=NULL;
short	*UnitArray::selected_sea_unit_array=NULL;
short	*UnitArray::selected_air_unit_array=NULL;

//------ static variables for movement -------//
static short		square_size, not_tested_loc, rec_height, rec_width;
static int			move_scale; // use integer for intrgral division
static int			x, y;
static short		lower_right_case, upper_left_case;	// indicate which case should be used
static int			*distance, *sorted_member, *sorted_distance;
static char			*done_flag;

//static int			rec_x1, rec_y1, rec_x2, rec_y2; // for bondary, corner improvement

static short		*filtering_unit_array;
static int			filtering_unit_count;
static short		*filtered_unit_array;
static int			filtered_unit_count;

//-------------- define static functions -------------//
static int cal_rectangle_lower_right_x(int refXLoc)
{
	// the rule:	refXLoc + (rec_width/(move_scale*2))*move_scale
	if(move_scale==1)
		return refXLoc + rec_width/2;
	else // move_scale == 2
		return refXLoc + (rec_width/4)*2;
}

static int cal_rectangle_lower_right_y(int refYLoc)
{
	// the rule:	refYLoc + ((rec_height-move_scale)/(move_scale*2))*move_scale
	if(move_scale==1)
		return refYLoc + (rec_height-1)/2;
	else // move_scale == 2
		return refYLoc + ((rec_height-2)/4)*2;
}

static int cal_rectangle_upper_left_x(int refXLoc)
{
	// the rule:	refXLoc - ((rec_width-move_scale)/(move_scale*2))*move_scale
	if(move_scale==1)
		return refXLoc - (rec_width-1)/2;
	else // move_scale == 2
		return refXLoc - ((rec_width-2)/4)*2;
}

static int cal_rectangle_upper_left_y(int refYLoc)
{
	// the rule:	refYLoc - (rec_height/(move_scale*2))*move_scale
	if(move_scale==1)
		return refYLoc - rec_height/2;
	else // move_scale == 2
		return refYLoc - (rec_height/4)*2;
}

//--------- Begin of function UnitArray::move_to ---------//
//
// Order the unit to move to a specific location following the
// shortest path.
//
// <int> 	destXLoc, destYLoc - the location of the destination.
// <int>		divided				 - whether the units are divided by their mobile type
// <short*> selectedUnitArray  - an array of recno of selected units.
// <int>    selectedCount 		 - no. of selected units.
// [char]   remoteAction 	    - whether this is an action carried out by a remote machine or not.
//											(default: 0)
//
// Note: the caller function must make sure all units in selectedUnitArray are validate (non-deleted)
//
void UnitArray::move_to(int destXLoc, int destYLoc, int divided, short* selectedUnitArray, int selectedCount, char remoteAction)
{
	err_when(destXLoc<0 || destYLoc<0 || destXLoc>MAX_WORLD_X_LOC-1 || destYLoc>MAX_WORLD_Y_LOC-1);

	//-------- if it's a multiplayer game --------//
	if( !remoteAction && remote.is_enable() )
	{
		short* shortPtr = (short*) remote.new_send_queue_msg(MSG_UNIT_MOVE,
								sizeof(short) * (4+selectedCount) );

		shortPtr[0] = destXLoc;
		shortPtr[1] = destYLoc;
		shortPtr[2] = selectedCount;
		shortPtr[3] = divided;

		memcpy( shortPtr+4, selectedUnitArray, sizeof(short) * selectedCount );
	}
	else
	{
		err_when( selectedCount > 10000 );		// error

		if(!divided)
		{
			//----------- divide units ------------//
			divide_array(destXLoc, destYLoc, selectedUnitArray, selectedCount);

			//---------- process group move --------------//
			// ##### patch begin Gilbert 18/8 ######//
			if(selected_land_unit_count)
				move_to(destXLoc, destYLoc, 1, selected_land_unit_array, selected_land_unit_count, COMMAND_AUTO);
				
			if(selected_sea_unit_count)
			{
				Location *locPtr = world.get_loc(destXLoc, destYLoc);
				if(terrain_res[locPtr->terrain_id]->average_type == TERRAIN_OCEAN)
					move_to(destXLoc, destYLoc, 1, selected_sea_unit_array, selected_sea_unit_count, COMMAND_AUTO);
				else
					ship_to_beach(destXLoc, destYLoc, 1, selected_sea_unit_array, selected_sea_unit_count, COMMAND_AUTO);
			}

			if(selected_air_unit_count)
				move_to(destXLoc, destYLoc, 1, selected_air_unit_array, selected_air_unit_count, COMMAND_AUTO);
			// ##### patch end Gilbert 18/8 ######//

			//---------------- deinit static parameters ------------------//
			selected_land_unit_count = selected_sea_unit_count = selected_air_unit_count = 0;
			mem_del(selected_land_unit_array);
			mem_del(selected_sea_unit_array);
			mem_del(selected_air_unit_array);
			return;
		}
		else
		{
			//---------------------------------------------------------//
			// set the unit_group_id
			//---------------------------------------------------------//
			Unit* unitPtr;
			uint32_t curGroupId = unit_array.cur_group_id++; 

			for(int k=0; k<selectedCount; k++)
			{
				unitPtr = (Unit*) get_ptr(selectedUnitArray[k]);
				err_when(!unitPtr);
				err_when(!unitPtr->is_visible() || unitPtr->hit_points<=0);

				unitPtr->unit_group_id = curGroupId;

				unitPtr->action_mode = ACTION_MOVE;
				unitPtr->action_para = 0;

				if(unitPtr->action_mode2!=ACTION_MOVE)
				{
					unitPtr->action_mode2 = ACTION_MOVE;
					unitPtr->action_para2 = 0;
					unitPtr->action_x_loc2 = unitPtr->action_y_loc2 = -1;
				}// else keep the data to check whether same action mode is ordered
			}

			//--------------------------------------------------------------//
			// if only the leader unit is moving, no need to use formation
			// movement although the button is pushed
			//--------------------------------------------------------------//
			if(selectedCount==1)
			{
				unitPtr = (Unit*) get_ptr(selectedUnitArray[0]);
				unitPtr->move_to(destXLoc, destYLoc, 1);
			}
			else
			{
				Unit *firstUnitPtr = operator[](selectedUnitArray[0]);

				if(firstUnitPtr->mobile_type==UNIT_LAND)
				{
					move_to_now_with_filter(destXLoc, destYLoc, selectedUnitArray, selectedCount);
					seek_path.set_sub_mode(); // reset sub_mode searching
					seek_path_reuse.set_sub_mode(); //------ reset sub mode of path reuse searching
				}
				else
					move_to_now_with_filter(destXLoc, destYLoc, selectedUnitArray, selectedCount);
			}
		}
	}
}
//----------- End of function UnitArray::move_to -----------//


//----------Begin of function UnitArray::move_to_now_with_filter------------//
// the selected units may be in different territory. Group them by their region id
// and process searching for each group
//
// <int>		destX					- the x location to move to
// <int>		destY					- the y location to move to
// <short*>	selectedUnitArray	- recno. of selected units
// <int>		selectedCount		- num of selected units
//
void UnitArray::move_to_now_with_filter(int destX, int destY, short* selectedUnitArray, int selectedCount)
{
	int destRegionId = world.get_loc(destX, destY)->region_id;
	Unit *unitPtr = operator[](selectedUnitArray[0]);
	
	//-------------- no filtering for unit air --------------------------//
	if(unitPtr->mobile_type==UNIT_AIR)
	{
		move_to_now(destX, destY, selectedUnitArray, selectedCount);
		return;
	}

	//----------------- init data structure --------------//
	int arraySize = sizeof(short)*selectedCount;
	filtered_unit_array = (short*) mem_add(arraySize);
	filtering_unit_array = (short*) mem_add(arraySize);
	memcpy(filtering_unit_array, selectedUnitArray, arraySize);
	filtering_unit_count = selectedCount;

	int unprocessCount = selectedCount;
	int filterRegionId = destRegionId;
	int filterDestX = destX, filterDestY = destY;
	int loopCount, filteringCount, i;

	//-------------------------------------------------------------------------------//
	// group the unit by their region id and process group searching for each group
	//-------------------------------------------------------------------------------//
	for(loopCount=0; loopCount<=unprocessCount; loopCount++) // checking for unprocessCount+1, plus one for the case that unit not on the same territory of destination
	{
		memset(filtered_unit_array, 0, arraySize);
		filtered_unit_count = 0;
		filteringCount = filtering_unit_count;
		filtering_unit_count = 0;
		
		//-------------- filter for filterRegionId --------------//
		for(i=0; i<filteringCount; i++)
		{
			unitPtr = operator[](filtering_unit_array[i]);
			if(world.get_loc(unitPtr->next_x_loc(), unitPtr->next_y_loc())->region_id==filterRegionId)
				filtered_unit_array[filtered_unit_count++] = filtering_unit_array[i];
			else
				filtering_unit_array[filtering_unit_count++] = filtering_unit_array[i];
		}

		//---- process for filtered_unit_array and prepare for looping ----//
		if(filtered_unit_count)
			move_to_now(filterDestX, filterDestY, filtered_unit_array, filtered_unit_count);

		if(!filtering_unit_count)
			break;

		//---------------- update parameters for next checking ------------------//
		unitPtr = operator[](filtering_unit_array[0]);
		filterRegionId = world.get_loc(unitPtr->next_x_loc(), unitPtr->next_y_loc())->region_id;
		filterDestX = destX;
		filterDestY = destY;
		unitPtr->different_territory_destination(filterDestX, filterDestY); // reference parameters to get the location the units should move to
	}

	mem_del(filtered_unit_array);
	mem_del(filtering_unit_array);
}
//----------- End of function UnitArray::move_to_now_with_filter -----------//


//--------- Begin of static function formation_role ---------//
//
// 7kmod-pathing: the row of a unit in a new formation, from the front:
// 0 - melee units, 1 - ranged units, 2 - siege weapons and caravans,
// 3 - generals and kings.
//
static int formation_role(Unit* unitPtr)
{
	if( unitPtr->rank_id == RANK_GENERAL || unitPtr->rank_id == RANK_KING )
		return 3;

	char unitClass = unit_res[unitPtr->unit_id]->unit_class;

	if( unitClass == UNIT_CLASS_WEAPON || unitClass == UNIT_CLASS_CARAVAN )
		return 2;

	if( unitPtr->max_attack_range() > 1 )
		return 1;

	return 0;
}
//----------- End of static function formation_role -----------//


//------- 7kmod-pathing: scattered groups that gather before marching -------//

#define FORM_UP_MIN_DIST		15		// a scattered group farther than this from the destination gathers first
#define MAX_FORM_UP_GROUP		20
#define FORM_UP_MAX_FRAMES		65		// the group marches after this even if the units aren't ready (about 3 seconds)
#define FORM_UP_NEAR_DIST		2		// a unit is ready when it is this near its place
#define FORM_UP_READY_PERCENT	80		// the group marches when this % of the units are ready

struct FormUpGroup
{
	uint32_t group_id;
	short		dest_x_loc, dest_y_loc;
	short		unit_count;
	short		unit_recno_array[100];
	uint16_t	unit_name_id_array[100];
	short		place_x_array[100];		// the place of each unit in the formation where the group gathers
	short		place_y_array[100];
	int		frame_count;
};

static FormUpGroup form_up_array[MAX_FORM_UP_GROUP];		// not saved: after loading a game, the units stay where they gathered
static int			 form_up_count = 0;
static int			 form_up_disabled = 0;

//--------- Begin of function UnitArray::add_form_up ---------//
//
// 7kmod-pathing: remember a group that is gathering, to order it to march
// to the destination when it has gathered (see process_form_up()).
//
void UnitArray::add_form_up(int destX, int destY, short* selectedUnitArray, int selectedCount, short* placeXArray, short* placeYArray,
									 int centerX, int centerY)
{
	if( form_up_count >= MAX_FORM_UP_GROUP )		// the oldest one marches now
	{
		form_up_array[0].frame_count = FORM_UP_MAX_FRAMES+1;
		process_form_up();

		if( form_up_count >= MAX_FORM_UP_GROUP )
			return;
	}

	FormUpGroup* groupPtr = form_up_array + form_up_count++;

	groupPtr->group_id	  = operator[](selectedUnitArray[0])->unit_group_id;
	groupPtr->dest_x_loc	  = destX;
	groupPtr->dest_y_loc	  = destY;
	groupPtr->unit_count	  = MIN(selectedCount, 100);
	groupPtr->frame_count  = 0;

	for( int i=0 ; i<groupPtr->unit_count ; i++ )
	{
		groupPtr->unit_recno_array[i]	 = selectedUnitArray[i];
		groupPtr->unit_name_id_array[i] = operator[](selectedUnitArray[i])->name_id;
		groupPtr->place_x_array[i]		 = placeXArray[i];
		groupPtr->place_y_array[i]		 = placeYArray[i];

		//--- for hurrying to the place and for the path line to the destination ---//

		Unit* unitPtr = operator[](selectedUnitArray[i]);

		unitPtr->form_up_group_id	  = groupPtr->group_id;
		unitPtr->form_up_place_x_loc = placeXArray[i];
		unitPtr->form_up_place_y_loc = placeYArray[i];
		unitPtr->form_up_dest_x_loc  = MAX( 0, MIN(MAX_WORLD_X_LOC-1, destX + placeXArray[i] - centerX) );
		unitPtr->form_up_dest_y_loc  = MAX( 0, MIN(MAX_WORLD_Y_LOC-1, destY + placeYArray[i] - centerY) );
	}
}
//----------- End of function UnitArray::add_form_up -----------//


//--------- Begin of function UnitArray::is_form_up_group ---------//
//
// 7kmod-pathing: whether the group is still gathering.
//
int UnitArray::is_form_up_group(uint32_t groupId)
{
	for( int i=0 ; i<form_up_count ; i++ )
	{
		if( form_up_array[i].group_id == groupId )
			return 1;
	}

	return 0;
}
//----------- End of function UnitArray::is_form_up_group -----------//


//--------- Begin of function UnitArray::process_form_up ---------//
//
// 7kmod-pathing: called every frame. When FORM_UP_READY_PERCENT % of the
// units of a gathering group are ready (within FORM_UP_NEAR_DIST of their
// place), or after FORM_UP_MAX_FRAMES, the whole group marches to the
// destination. The units that got another order, died or left are not counted.
//
void UnitArray::process_form_up()
{
	for( int g=form_up_count-1 ; g>=0 ; g-- )
	{
		FormUpGroup* groupPtr = form_up_array+g;
		short validArray[100];
		int   validCount=0, readyCount=0;

		for( int i=0 ; i<groupPtr->unit_count ; i++ )
		{
			int unitRecno = groupPtr->unit_recno_array[i];

			if( is_deleted(unitRecno) )
				continue;

			Unit* unitPtr = operator[](unitRecno);

			if( unitPtr->name_id != groupPtr->unit_name_id_array[i] || unitPtr->unit_group_id != groupPtr->group_id ||
				 !unitPtr->is_visible() || unitPtr->hit_points <= 0 || unitPtr->action_mode2 != ACTION_MOVE &&
				 unitPtr->action_mode2 != ACTION_STOP )
			{
				continue;		// it got another order
			}

			validArray[validCount++] = unitRecno;

			if( MAX( abs(unitPtr->next_x_loc()-groupPtr->place_x_array[i]),
						abs(unitPtr->next_y_loc()-groupPtr->place_y_array[i]) ) <= FORM_UP_NEAR_DIST )
			{
				readyCount++;
			}
		}

		if( validCount==0 || readyCount*100 >= validCount*FORM_UP_READY_PERCENT ||
			 ++groupPtr->frame_count > FORM_UP_MAX_FRAMES )
		{
			int destX = groupPtr->dest_x_loc, destY = groupPtr->dest_y_loc;

			//------ remove the group ------//

			memmove( form_up_array+g, form_up_array+g+1, sizeof(FormUpGroup)*(form_up_count-g-1) );
			form_up_count--;

			//------ march to the destination, keeping the formation ------//

			if( validCount > 0 )
			{
				form_up_disabled = 1;
				move_to(destX, destY, 0, validArray, validCount, COMMAND_AUTO);
				form_up_disabled = 0;
			}
		}
	}
}
//----------- End of function UnitArray::process_form_up -----------//


//--------- Begin of function UnitArray::move_in_formation ---------//
//
// 7kmod-pathing: move a group of land units in formation, like the "magic box"
// of modern RTS games:
//
// - If the group is together (the box it occupies is at most FORMATION_MAX_SPAN
//   locations wide and high, and the units fill at least half of it) and the
//   destination is outside of the box, each unit moves to the destination plus
//   its offset from the center of the box, so the group keeps its shape.
// - If the group is scattered (e.g. soldiers that just left a fort, spread
//   around it), a new formation is made at the destination: rows perpendicular
//   to the direction of the movement (diagonal for a diagonal movement), with
//   1 row up to 4 units, 2 rows up to 12, and then at least 3 rows of at most
//   10 units each. Melee units go
//   in the front rows, then ranged units, then siege weapons, and generals and
//   kings at the back.
// - If the destination is inside the box of a group that is together, or very
//   close to the center of a scattered group, the units gather around the
//   destination as in the original game (move_to_now_gather()).
//
// Each unit searches its own path to its place (no path reuse), so they don't
// cross each other. If the place of a unit is not walkable, is occupied by a
// unit of another group or is in another region, the closest free location
// around it is used.
//
// return: <int> 1 - the units have been ordered to move in formation
//               0 - not possible, move_to_now_gather() moves them as usual
//
int UnitArray::move_in_formation(int destX, int destY, short* selectedUnitArray, int selectedCount)
{
	#define FORMATION_MAX_SPAN		10
	#define FORMATION_MAX_UNIT		100
	#define FORMATION_SEARCH_SIZE	9		// the size of the square to look for a free location around a place
	#define FORMATION_MIN_DIST		3		// the minimum distance to the center of a scattered group to make a new formation

	if( selectedCount < 2 || selectedCount > FORMATION_MAX_UNIT )
		return 0;

	//------- only land units of size 1x1 --------//

	int minX=MAX_WORLD_X_LOC, minY=MAX_WORLD_Y_LOC, maxX=-1, maxY=-1;
	int sumX=0, sumY=0;
	int i, j;

	for( i=0 ; i<selectedCount ; i++ )
	{
		Unit* unitPtr = operator[](selectedUnitArray[i]);

		if( unitPtr->mobile_type != UNIT_LAND ||
			 unitPtr->sprite_info->loc_width != 1 || unitPtr->sprite_info->loc_height != 1 )
		{
			return 0;
		}

		int x = unitPtr->next_x_loc(), y = unitPtr->next_y_loc();

		minX = MIN(minX, x);
		minY = MIN(minY, y);
		maxX = MAX(maxX, x);
		maxY = MAX(maxY, y);
		sumX += x;
		sumY += y;
	}

	short placeXArray[FORMATION_MAX_UNIT], placeYArray[FORMATION_MAX_UNIT];		// the wanted place of each unit

	int slotX = destX, slotY = destY;		// the center of the new formation
	int formUp = 0;							// whether the scattered group gathers before marching

	int boxWidth = maxX-minX+1, boxHeight = maxY-minY+1;

	if( boxWidth <= FORMATION_MAX_SPAN && boxHeight <= FORMATION_MAX_SPAN &&
		 selectedCount*2 >= boxWidth*boxHeight )
	{
		//------- the group is together: keep its shape -------//

		if( destX >= minX && destX <= maxX && destY >= minY && destY <= maxY )
			return 0;		// destination inside the box: gather as usual

		int centerX = (minX+maxX)/2, centerY = (minY+maxY)/2;

		for( i=0 ; i<selectedCount ; i++ )
		{
			Unit* unitPtr = operator[](selectedUnitArray[i]);

			placeXArray[i] = destX + unitPtr->next_x_loc() - centerX;
			placeYArray[i] = destY + unitPtr->next_y_loc() - centerY;
		}
	}
	else
	{
		//------- the group is scattered: make a new formation -------//

		int centerX = sumX/selectedCount, centerY = sumY/selectedCount;
		int vecX = destX-centerX, vecY = destY-centerY;

		if( MAX(abs(vecX), abs(vecY)) < FORMATION_MIN_DIST )
			return 0;

		//--- a far destination: first gather in formation where the group is, then march (as in AoE II) ---//

		if( !form_up_disabled && MAX(abs(vecX), abs(vecY)) > FORM_UP_MIN_DIST )
		{
			formUp = 1;
			slotX  = centerX;
			slotY  = centerY;
		}

		//--- the direction of the movement, in one of the 8 directions ---//

		int frontX = (vecX>0) - (vecX<0);
		int frontY = (vecY>0) - (vecY<0);

		if( abs(vecX) > 2*abs(vecY) )
			frontY = 0;
		else if( abs(vecY) > 2*abs(vecX) )
			frontX = 0;

		int sideX = -frontY, sideY = frontX;		// along the rows

		//--- the number of rows and the units per row ---//

		#define FORMATION_MAX_WIDTH	10		// with more units, more rows are added

		int rowCount = selectedCount <= 4 ? 1 : (selectedCount <= 12 ? 2 :
							MAX(3, (selectedCount+FORMATION_MAX_WIDTH-1) / FORMATION_MAX_WIDTH));
		int colCount = (selectedCount+rowCount-1) / rowCount;

		//--- sort the units: by role, then the ones more to the front first ---//

		short orderArray[FORMATION_MAX_UNIT];
		int   keyArray[FORMATION_MAX_UNIT];

		for( i=0 ; i<selectedCount ; i++ )
		{
			Unit* unitPtr = operator[](selectedUnitArray[i]);
			int front = (unitPtr->next_x_loc()-centerX)*frontX + (unitPtr->next_y_loc()-centerY)*frontY;

			orderArray[i] = i;
			keyArray[i] = formation_role(unitPtr)*10000 - front;
		}

		for( i=1 ; i<selectedCount ; i++ )		// insertion sort, stable
		{
			short id = orderArray[i];

			for( j=i ; j>0 && keyArray[orderArray[j-1]] > keyArray[id] ; j-- )
				orderArray[j] = orderArray[j-1];

			orderArray[j] = id;
		}

		//--- each row: the units sorted by their side position, so that they don't cross ---//

		for( int row=0 ; row<rowCount ; row++ )
		{
			int firstId = row*colCount;
			int rowUnits = MIN(colCount, selectedCount-firstId);

			if( rowUnits <= 0 )
				break;

			for( i=firstId+1 ; i<firstId+rowUnits ; i++ )
			{
				short id = orderArray[i];
				Unit* unitPtr = operator[](selectedUnitArray[id]);
				int side = unitPtr->next_x_loc()*sideX + unitPtr->next_y_loc()*sideY;

				for( j=i ; j>firstId ; j-- )
				{
					Unit* prevPtr = operator[](selectedUnitArray[orderArray[j-1]]);

					if( prevPtr->next_x_loc()*sideX + prevPtr->next_y_loc()*sideY <= side )
						break;

					orderArray[j] = orderArray[j-1];
				}

				orderArray[j] = id;
			}

			int rowOffset = (rowCount-1)/2 - row;		// the formation is centered on the destination, the first row in front

			for( i=0 ; i<rowUnits ; i++ )
			{
				int sideOffset = i - (rowUnits-1)/2;
				short id = orderArray[firstId+i];

				placeXArray[id] = slotX + sideOffset*sideX + rowOffset*frontX;
				placeYArray[id] = slotY + sideOffset*sideY + rowOffset*frontY;
			}
		}
	}

	//------- the closest free location around each place (the place itself first) -------//

	Unit* firstUnit = operator[](selectedUnitArray[0]);
	int destRegionId = formUp ? world.get_loc(firstUnit->next_x_loc(), firstUnit->next_y_loc())->region_id :
										 world.get_loc(destX, destY)->region_id;
	uint32_t groupId = firstUnit->unit_group_id;

	for( i=0 ; i<selectedCount ; i++ )
	{
		int wantX = placeXArray[i], wantY = placeYArray[i];
		int found = 0;

		for( int n=1 ; n<=FORMATION_SEARCH_SIZE*FORMATION_SEARCH_SIZE && !found ; n++ )
		{
			int xShift, yShift;

			misc.cal_move_around_a_point(n, FORMATION_SEARCH_SIZE, FORMATION_SEARCH_SIZE, xShift, yShift);

			int x = wantX+xShift, y = wantY+yShift;

			if( x<0 || x>=MAX_WORLD_X_LOC || y<0 || y>=MAX_WORLD_Y_LOC )
				continue;

			Location* locPtr = world.get_loc(x, y);

			if( locPtr->region_id != destRegionId || !locPtr->is_unit_group_accessible(UNIT_LAND, groupId) )
				continue;

			//--- not taken by another unit of the group ---//

			for( j=0 ; j<i && (placeXArray[j]!=x || placeYArray[j]!=y) ; j++ );

			if( j<i )
				continue;

			placeXArray[i] = x;
			placeYArray[i] = y;
			found = 1;
		}

		if( !found )		// no room for the formation, gather as usual
			return 0;
	}

	//------- order each unit to its place, with its own path -------//

	for( i=0 ; i<selectedCount ; i++ )
	{
		Unit* unitPtr = operator[](selectedUnitArray[i]);

		if( unitPtr->cur_action==SPRITE_ATTACK )
			unitPtr->stop();

		if( unitPtr->cur_action==SPRITE_IDLE )
			unitPtr->set_ready();

		unitPtr->move_to( placeXArray[i], placeYArray[i], 1 );
	}

	//------- the group marches to the destination when it has gathered -------//

	if( formUp )
		add_form_up(destX, destY, selectedUnitArray, selectedCount, placeXArray, placeYArray, slotX, slotY);

	return 1;
}
//----------- End of function UnitArray::move_in_formation -----------//


//--------- Begin of function UnitArray::set_group_speed ---------//
//
// 7kmod-pathing: the units of a group move at their own speed, but a faster
// unit waits when it has advanced too much more than a slower unit of the
// group that started near it (see Unit::process_move()). Each unit waits for
// the slowest unit that was within GROUP_SLOW_NEAR_DIST locations.
//
void UnitArray::set_group_speed(short* selectedUnitArray, int selectedCount)
{
	#define GROUP_SLOW_NEAR_DIST	10
	#define GROUP_WAIT_ENABLED		0		// disabled for now: it didn't work well in the tests

	int i, j;

	for( i=0 ; i<selectedCount ; i++ )
	{
		if( operator[](selectedUnitArray[i])->mobile_type != UNIT_LAND )
			return;
	}

	for( i=0 ; i<selectedCount ; i++ )
	{
		Unit* unitPtr = operator[](selectedUnitArray[i]);

		unitPtr->group_move_group_id = unitPtr->unit_group_id;
		unitPtr->group_start_x_loc   = unitPtr->next_x_loc();
		unitPtr->group_start_y_loc   = unitPtr->next_y_loc();
		unitPtr->group_waiting       = 0;
		unitPtr->group_wait_frames   = 0;
		unitPtr->group_slow_recno    = 0;

		//--- the slowest unit near it that is slower than it ---//

		if( !GROUP_WAIT_ENABLED )
			continue;

		int mySpeed = unitPtr->move_speed(), slowSpeed = mySpeed;

		for( j=0 ; j<selectedCount ; j++ )
		{
			Unit* otherUnit = operator[](selectedUnitArray[j]);

			if( otherUnit == unitPtr )
				continue;

			int dist = MAX( abs(otherUnit->next_x_loc()-unitPtr->next_x_loc()), abs(otherUnit->next_y_loc()-unitPtr->next_y_loc()) );
			int speed = otherUnit->move_speed();

			if( dist <= GROUP_SLOW_NEAR_DIST && speed < slowSpeed )
			{
				slowSpeed = speed;
				unitPtr->group_slow_recno = otherUnit->sprite_recno;
			}
		}
	}
}
//----------- End of function UnitArray::set_group_speed -----------//


//--------- Begin of function UnitArray::move_to_now ---------//
//
// 7kmod-pathing: move in formation if possible, otherwise gather around the
// destination as in the original game. Then match the speed of the group.
//
void UnitArray::move_to_now(int destXLoc, int destYLoc, short* selectedUnitArray, int selectedCount)
{
	if( !move_in_formation(destXLoc, destYLoc, selectedUnitArray, selectedCount) )
		move_to_now_gather(destXLoc, destYLoc, selectedUnitArray, selectedCount);

	set_group_speed(selectedUnitArray, selectedCount);
}
//----------- End of function UnitArray::move_to_now -----------//


//--------- Begin of function UnitArray::move_to_now_gather ---------//
//
// Order the unit to move to a specific location following the
// shortest path.
//
// <int> 	destXLoc, destYLoc - the location of the destination.
// <short*> selectedUnitArray  - an array of recno of selected units.
// <int>    selectedCount 		 - no. of selected units.
//
void UnitArray::move_to_now_gather(int destXLoc, int destYLoc, short* selectedUnitArray, int selectedCount)
{
	err_when(destXLoc<0 || destYLoc<0 || destXLoc>=MAX_WORLD_X_LOC || destYLoc>=MAX_WORLD_Y_LOC);
	err_when( selectedCount > 10000 );

	//------------ define vars -----------------------//
	int		unprocessCount;// = selectedCount;		// num. of unprocessed sprite
	int		k;													// for counting
	short		vecX, vecY;										// used to reset x, y
	short		oddCount, evenCount;
	int		j;
	Unit*		unitPtr = unit_array[selectedUnitArray[0]];
	uint32_t		curGroupId = unitPtr->unit_group_id;
	int		mobileType = unitPtr->mobile_type;
	Location	*worldLocMatrix=world.loc_matrix, *locPtr;
	//int		sizeOneSelectedCount=0, sizeTwoSelectedCount=0;
	int		sizeOneSelectedCount = selectedCount;

	//---------- set Unit::unit_group_id and count the unit by size ----------//
	int i;
	for( i=0 ; i<selectedCount ; i++ )
	{
		unitPtr = operator[](selectedUnitArray[i]);
		err_when(unitPtr->cur_action==SPRITE_IDLE && (unitPtr->cur_x!=unitPtr->next_x || unitPtr->cur_y!=unitPtr->next_y));
		err_when(unitPtr->action_para); // action_para should be set to zero in move_to()

		if(unitPtr->cur_action==SPRITE_ATTACK)
			unitPtr->stop();

		err_when(unitPtr->cur_action==SPRITE_ATTACK && unitPtr->action_para==0);
		if(unitPtr->cur_action==SPRITE_IDLE)
			unitPtr->set_ready();
		
		/*switch(unitPtr->sprite_info->loc_width)
		{
			case 1:	sizeOneSelectedCount++;
						break;

			case 2:	sizeTwoSelectedCount++;
						break;

			default:	err_here();
						break;
		}*/
	}
	unprocessCount = sizeOneSelectedCount;

	//---- construct array to store size one selected unit ----//
	short* selectedSizeOneUnitArray;
	if(sizeOneSelectedCount)
	{
		selectedSizeOneUnitArray = (short*)mem_add(sizeof(short)*sizeOneSelectedCount);
		memset(selectedSizeOneUnitArray, 0, sizeof(short)*sizeOneSelectedCount);
		for(i=0, k=0; i<selectedCount && unprocessCount; i++)
		{
			unitPtr = operator[](selectedUnitArray[i]);
			if(unitPtr->sprite_info->loc_width==1)
			{
				selectedSizeOneUnitArray[k++] = selectedUnitArray[i];
				unprocessCount--;
			}
		}
	}
	unprocessCount = sizeOneSelectedCount;

	//----------- variables initialization ---------------//
	int destX, destY;
	if(mobileType==UNIT_LAND)
	{
		x = destX = destXLoc;
		y = destY = destYLoc;
		move_scale = 1;
	}
	else // UNIT_AIR, UNIT_SEA
	{
		x = destX = (destXLoc/2)*2;
		y = destY = (destYLoc/2)*2;
		move_scale = 2;
	}
	
	//if(sizeOneSelectedCount)
	//{
		//----- initialize parameters and construct data structure -----//
		oddCount =1;
		evenCount = 3;
		square_size = not_tested_loc = lower_right_case = upper_left_case = 0;

		distance = (int*)mem_add(sizeof(int)*sizeOneSelectedCount);		// used in the function construct_sorted_array and this function,
		memset(distance, 0, sizeof(int)*sizeOneSelectedCount);				// and allocate/free the memory in this function

		sorted_distance = (int*)mem_add(sizeof(int)*sizeOneSelectedCount);
		memset(sorted_distance, 0, sizeof(int)*sizeOneSelectedCount);

		sorted_member = (int*)mem_add(sizeof(int)*sizeOneSelectedCount);
		memset(sorted_member, 0, sizeof(int)*sizeOneSelectedCount);

		done_flag = (char*)mem_add(sizeof(char)*sizeOneSelectedCount);
		memset(done_flag, 0, sizeof(char)*sizeOneSelectedCount);

		//--- calculate the rectangle size used to allocate space for the sprites----//
		unprocessCount = sizeOneSelectedCount;
		while(unprocessCount)
		{
			//=============================
			// process odd size square
			//=============================
			vecX = short(oddCount/4)*move_scale;
			vecY = vecX;
			k = 0;

			for(j=0; j<oddCount && unprocessCount; j++)
			{
				x = destX+vecX;
				y = destY+vecY;

				if(x>=0 && y>=0 && x<MAX_WORLD_X_LOC && y<MAX_WORLD_Y_LOC)
				{	
					if( worldLocMatrix[y*MAX_WORLD_X_LOC+x].is_unit_group_accessible(mobileType, curGroupId) )
						unprocessCount--;
				}
				
				if(k++ < int(oddCount/2))	// reset vecX, vecY
					vecX -= move_scale;
				else
					vecY -= move_scale;
			}
			square_size+=move_scale;
			if(j<oddCount)
				not_tested_loc = oddCount-j;
			oddCount+=4;

			if(unprocessCount)
			{
				//=============================
				// process even size square
				//=============================
				vecY = (-short(evenCount/4)-1)*move_scale;
				vecX = vecY+move_scale;
				k = 0;

				for(j=0; j<evenCount && unprocessCount; j++)
				{
					x = destX+vecX;
					y = destY+vecY;

					if(x>=0 && y>=0 && x<MAX_WORLD_X_LOC && y<MAX_WORLD_Y_LOC)
					{
						if(worldLocMatrix[y*MAX_WORLD_X_LOC+x].is_unit_group_accessible(mobileType, curGroupId) )
							unprocessCount--;
					}

					if(k++ < int(evenCount/2))	// reset vecX, vecY
						vecX += move_scale;
					else
						vecY += move_scale;
				}
				square_size+=move_scale;
				if(j<evenCount)
					not_tested_loc = evenCount-j;
				evenCount+=4;
			}
		}
		
		rec_height = rec_width = square_size;	// get the height and width of the rectangle
		if(not_tested_loc >= (square_size/move_scale))
			rec_width -= move_scale;
		
		//--- decide to use upper_left_case or lower_right_case, however, it maybe changed for boundary improvement----//
		x = cal_rectangle_lower_right_x(destX);
		y = cal_rectangle_lower_right_y(destY);

		for(i=0; i<sizeOneSelectedCount; i++)
		{
			unitPtr = (Unit*) get_ptr(selectedSizeOneUnitArray[i]);

			if(unitPtr->next_y_loc()<y)		// lower_right_case or upper_left_case
				lower_right_case++;
			else if(unitPtr->next_y_loc()>y)
				upper_left_case++;
		}
		
		if(lower_right_case==upper_left_case) // in case both values are equal, check by upper_left_case
		{
			x = cal_rectangle_upper_left_x(destX);
			y = cal_rectangle_upper_left_y(destY);

			lower_right_case = upper_left_case = 0;
			for(i=0; i<sizeOneSelectedCount; i++)
			{
				unitPtr = (Unit*) get_ptr(selectedSizeOneUnitArray[i]);

				if(unitPtr->next_y_loc()<y)		// lower_right_case or upper_left_case
					lower_right_case++;
				else if(unitPtr->next_y_loc()>y)
					upper_left_case++;
			}
		}

		//------------ determine x, y and lower_right_case/upper_left_case-----------//
		determine_position_to_construct_table(selectedCount, destX, destY, mobileType);
		err_when(x<0 || y<0 || x>=MAX_WORLD_X_LOC || y>=MAX_WORLD_Y_LOC);

		//------------ construct a table to store distance -------//
		construct_sorted_array(selectedSizeOneUnitArray, sizeOneSelectedCount);	// distance and sorted_member should be initialized first
		err_when(x<0 || y<0 || x>=MAX_WORLD_X_LOC || y>=MAX_WORLD_Y_LOC);

		//------------ process the movement -----------//
		unprocessCount = sizeOneSelectedCount;//selectedCount;
		k=0;

		//-******************* auto correct ***********************-//
		int autoCorrectStartX = x;
		int autoCorrectStartY = y;
		//-******************* auto correct ***********************-//
		
		if(lower_right_case >= upper_left_case)
		{
			while(unprocessCount)
			{
				locPtr = worldLocMatrix+y*MAX_WORLD_X_LOC+x;
				for(i=x; i>x-rec_width && unprocessCount; i-=move_scale, locPtr-=move_scale)
				{
					if(locPtr->is_unit_group_accessible(mobileType, curGroupId))
					{
						do
						{
							unitPtr = (Unit*) get_ptr(selectedSizeOneUnitArray[sorted_member[k++]]);
						}while(unitPtr->sprite_info->loc_width>1);
						
						err_when(k>sizeOneSelectedCount);
						if(sizeOneSelectedCount>1)
						{
							if(unprocessCount==sizeOneSelectedCount) // the first unit to move
							{	
								unitPtr->move_to(i, y, 1, 4, 0, sizeOneSelectedCount, GENERAL_GROUP_MOVEMENT, REUSE_PATH_INITIAL);
								if(unitPtr->mobile_type==UNIT_LAND && unitPtr->nation_recno)
									unitPtr->select_search_sub_mode(unitPtr->next_x_loc(), unitPtr->next_y_loc(), i, y, unitPtr->nation_recno, SEARCH_MODE_REUSE);
								unitPtr->move_to(i, y, 1, 4, 0, sizeOneSelectedCount, GENERAL_GROUP_MOVEMENT, REUSE_PATH_FIRST_SEEK);
							}
							else
							{
								err_when(unprocessCount==sizeOneSelectedCount);
								if(unitPtr->mobile_type==UNIT_LAND && unitPtr->nation_recno)
									unitPtr->select_search_sub_mode(unitPtr->next_x_loc(), unitPtr->next_y_loc(), i, y, unitPtr->nation_recno, SEARCH_MODE_REUSE);
								unitPtr->move_to(i, y, 1, 4, 0, sizeOneSelectedCount, GENERAL_GROUP_MOVEMENT, REUSE_PATH_SEARCH);
							}
						}
						else
							unitPtr->move_to(i, y, 1);
						unprocessCount--;
					}
				}
				y-=move_scale;
				//-******************* auto correct ***********************-//
				#ifdef DEBUG
				err_when(unprocessCount && y<0);
				#else
				if(unprocessCount && y<0)
					y = autoCorrectStartY;
				#endif
				//-******************* auto correct ***********************-//
			}
		}
		else // upper_left_case
		{
			while(unprocessCount)
			{
				locPtr = worldLocMatrix+y*MAX_WORLD_X_LOC+x;
				for(i=x; i<x+rec_width && unprocessCount; i+=move_scale, locPtr+=move_scale)
				{
					if(locPtr->is_unit_group_accessible(mobileType, curGroupId))
					{	
						do
						{
							unitPtr = (Unit*) get_ptr(selectedSizeOneUnitArray[sorted_member[k++]]);
						}while(unitPtr->sprite_info->loc_width>1);
						err_when(k>sizeOneSelectedCount);

						if(sizeOneSelectedCount>1)
						{
							if(unprocessCount==sizeOneSelectedCount) // the first unit to move
							{
								unitPtr->move_to(i, y, 1, 4, 0, sizeOneSelectedCount, GENERAL_GROUP_MOVEMENT, REUSE_PATH_INITIAL);
								if(unitPtr->mobile_type==UNIT_LAND && unitPtr->nation_recno)
									unitPtr->select_search_sub_mode(unitPtr->next_x_loc(), unitPtr->next_y_loc(), i, y, unitPtr->nation_recno, SEARCH_MODE_REUSE);
								unitPtr->move_to(i, y, 1, 4, 0, sizeOneSelectedCount, GENERAL_GROUP_MOVEMENT, REUSE_PATH_FIRST_SEEK);
							}
							else
							{
								err_when(unprocessCount==sizeOneSelectedCount);
								if(unitPtr->mobile_type==UNIT_LAND && unitPtr->nation_recno)
									unitPtr->select_search_sub_mode(unitPtr->next_x_loc(), unitPtr->next_y_loc(), i, y, unitPtr->nation_recno, SEARCH_MODE_REUSE);
								unitPtr->move_to(i, y, 1, 4, 0, sizeOneSelectedCount, GENERAL_GROUP_MOVEMENT, REUSE_PATH_SEARCH);
							}
						}
						else
							unitPtr->move_to(i, y, 1);
						unprocessCount--;
					}
				}
				y+=move_scale;
				//-******************* auto correct ***********************-//
				if(unprocessCount && y>=MAX_WORLD_Y_LOC)
				{
					MSG("Unit attempted to walk off map...correcting.\n");
					y = autoCorrectStartY;
				}
				//-******************* auto correct ***********************-//
			}
		}
		//---------------- destruct data structure --------------//
		mem_del(done_flag);
		mem_del(sorted_distance);
		mem_del(sorted_member);
		mem_del(distance);
		mem_del(selectedSizeOneUnitArray);
	//}// end if (sizeOneSelectedCount)
	/*
	//=============================================================================//
	//----- order sprite with size two to move to a specified position ------------//
	//=============================================================================//

	int sizeTwoUnprocessCount = sizeTwoSelectedCount;
	int surX, surY, suaCount=0;
	char w, h, blocked=0;

	if(sizeOneSelectedCount)	// mix, size one units have processed
	{	
		if(rec_width>square_size)
			square_size = rec_width;
		if(rec_height>square_size)
			square_size = rec_height;
		square_size = ((square_size+1)/2)<<1;	// change to multiply of two
		rec_width = rec_height = square_size;

		x = destX-rec_width/2+1;
		y = destY-rec_height/2;
	}
	else	// all are size 2 units
	{
		//-***************** testing ********************-//
		err_here();
		//-***************** testing ********************-//

		square_size = rec_width = rec_height = 2;
		x = destX;
		y = destY;

		if(x<0)
			x = 0;
		else if(x>=MAX_WORLD_X_LOC-1)
			x = MAX_WORLD_X_LOC-2;

		if(y<0)
			y = 0;
		else if(y>=MAX_WORLD_Y_LOC-1)
			y = MAX_WORLD_Y_LOC-2;
		
		blocked = 0;
		for(h=0, surY=y; h<2 && !blocked; h++, surY++)
		{
			for(w=0, surX=x; w<2 && !blocked; w++, surX++)
			{	
				locPtr = worldLocMatrix+surY*MAX_WORLD_X_LOC+surX;
				blocked = !locPtr->is_unit_group_accessible(mobileType, curGroupId);
			}
		}

		if(!blocked)
		{
			do
			{
				unitPtr = (Unit*) get_ptr(selectedUnitArray[suaCount++]);
			}while(unitPtr->sprite_info->loc_width<2);

			if(sizeTwoSelectedCount>1)
			{
				unitPtr->move_to(x, y, 1, 4, 0, sizeTwoSelectedCount, GENERAL_GROUP_MOVEMENT, REUSE_PATH_INITIAL);
				unitPtr->move_to(x, y, 1, 4, 0, sizeTwoSelectedCount, GENERAL_GROUP_MOVEMENT, REUSE_PATH_FIRST_SEEK);
			}
			else
				unitPtr->move_to(x, y, 1);
			sizeTwoUnprocessCount--;
		}
	}

	while(sizeTwoUnprocessCount)
	{
		//-***************** testing ********************-//
		err_here();
		//-***************** testing ********************-//

		int moveToX, moveToY;
		int boundedX, boundedY;
		
		//------------- upper edge --------------//
		moveToY = y-2;
		moveToX = x;
		if(moveToY>=0)
		{
			if(x+rec_width+2 > MAX_WORLD_X_LOC-2)
				boundedX = MAX_WORLD_X_LOC-1;
			else
				boundedX = x+rec_width+2;
			
			while(moveToX<boundedX && sizeTwoUnprocessCount)
			{
				//--------------- is the position blocked? ----------//
				if(moveToX>=MAX_WORLD_X_LOC-1 || moveToY>=MAX_WORLD_Y_LOC-1)
					blocked = 1;
				else
				{
					blocked = 0;
					for(h=0, surY=moveToY; h<2 && !blocked; h++, surY++)
					{
						for(w=0, surX=moveToX; w<2 && !blocked; w++, surX++)
						{	
							locPtr = worldLocMatrix+surY*MAX_WORLD_X_LOC+surX;
							blocked = !locPtr->is_unit_group_accessible(mobileType, curGroupId);
						}
					}
				}

				if(!blocked)
				{
					do
					{
						unitPtr = (Unit*) get_ptr(selectedUnitArray[suaCount++]);
					}while(unitPtr->sprite_info->loc_width<2);

					if(sizeTwoSelectedCount>1)
					{
						if(sizeTwoUnprocessCount==sizeTwoSelectedCount) // the first unit to move
						{	
							unitPtr->move_to(moveToX, moveToY, 1, 4, 0, sizeTwoSelectedCount, GENERAL_GROUP_MOVEMENT, REUSE_PATH_INITIAL);
							unitPtr->move_to(moveToX, moveToY, 1, 4, 0, sizeTwoSelectedCount, GENERAL_GROUP_MOVEMENT, REUSE_PATH_FIRST_SEEK);
						}
						else
						{
							err_if(sizeTwoUnprocessCount==sizeTwoSelectedCount)
								err_here();
							
							unitPtr->move_to(moveToX, moveToY, 1, 4, 0, sizeTwoSelectedCount, GENERAL_GROUP_MOVEMENT, REUSE_PATH_SEARCH);
						}
					}
					else
						unitPtr->move_to(moveToX, moveToY, 1);
					sizeTwoUnprocessCount--;
				}
				moveToX+=2;
			}	
		}

		//------------- right edge --------------//
		moveToX = x+rec_width;
		moveToY = y;
		if(moveToX<MAX_WORLD_X_LOC-1)
		{
			if(y+rec_height+2 > MAX_WORLD_Y_LOC-2)
				boundedY = MAX_WORLD_Y_LOC-1;
			else
				boundedY = y+rec_height+2;
			
			while(moveToY<boundedY && sizeTwoUnprocessCount)
			{
				//--------------- is the position blocked? ----------//
				if(moveToX>=MAX_WORLD_X_LOC-1 || moveToY>=MAX_WORLD_Y_LOC-1)
					blocked = 1;
				else
				{
					blocked = 0;
					for(h=0, surY=moveToY; h<2 && !blocked; h++, surY++)
					{
						for(w=0, surX=moveToX; w<2 && !blocked; w++, surX++)
						{
							locPtr = worldLocMatrix+surY*MAX_WORLD_X_LOC+surX;
							blocked = !locPtr->is_unit_group_accessible(mobileType, curGroupId);
						}
					}
				}

				if(!blocked)
				{
					do
					{
						unitPtr = (Unit*) get_ptr(selectedUnitArray[suaCount++]);
					}while(unitPtr->sprite_info->loc_width<2);

					if(sizeTwoSelectedCount>1)
					{
						if(sizeTwoUnprocessCount==sizeTwoSelectedCount) // the first unit to move
						{	
							unitPtr->move_to(moveToX, moveToY, 1, 4, 0, sizeTwoSelectedCount, GENERAL_GROUP_MOVEMENT, REUSE_PATH_INITIAL);
							unitPtr->move_to(moveToX, moveToY, 1, 4, 0, sizeTwoSelectedCount, GENERAL_GROUP_MOVEMENT, REUSE_PATH_FIRST_SEEK);
						}
						else
						{
							err_if(sizeTwoUnprocessCount==sizeTwoSelectedCount)
								err_here();
							
							unitPtr->move_to(moveToX, moveToY, 1, 4, 0, sizeTwoSelectedCount, GENERAL_GROUP_MOVEMENT, REUSE_PATH_SEARCH);
						}
					}
					else
						unitPtr->move_to(moveToX, moveToY, 1);
					sizeTwoUnprocessCount--;
				}
				moveToY+=2;
			}
		}

		//------------- lower edge ----------------//
		moveToX = x+rec_width-2;
		moveToY = y+rec_height;
		if(moveToY<MAX_WORLD_Y_LOC-1)
		{
			if(x-3 < 0)
				boundedX = -1;
			else
				boundedX = x-3;
			
			while(moveToX>boundedX && sizeTwoUnprocessCount)
			{
				//--------------- is the position blocked? ----------//
				if(moveToX>=MAX_WORLD_X_LOC-1 || moveToY>=MAX_WORLD_Y_LOC-1)
					blocked = 1;
				else
				{
					blocked = 0;
					for(h=0, surY=moveToY; h<2 && !blocked; h++, surY++)
					{
						for(w=0, surX=moveToX; w<2 && !blocked; w++, surX++)
						{
							locPtr = worldLocMatrix+surY*MAX_WORLD_X_LOC+surX;
							blocked = !locPtr->is_unit_group_accessible(mobileType, curGroupId);
						}
					}
				}

				if(!blocked)
				{
					do
					{
						unitPtr = (Unit*) get_ptr(selectedUnitArray[suaCount++]);
					}while(unitPtr->sprite_info->loc_width<2);
					if(sizeTwoSelectedCount>1)
					{
						if(sizeTwoUnprocessCount==sizeTwoSelectedCount) // the first unit to move
						{	
							unitPtr->move_to(moveToX, moveToY, 1, 4, 0, sizeTwoSelectedCount, GENERAL_GROUP_MOVEMENT, REUSE_PATH_INITIAL);
							unitPtr->move_to(moveToX, moveToY, 1, 4, 0, sizeTwoSelectedCount, GENERAL_GROUP_MOVEMENT, REUSE_PATH_FIRST_SEEK);
						}
						else
						{
							err_if(sizeTwoUnprocessCount==sizeTwoSelectedCount)
								err_here();
							
							unitPtr->move_to(moveToX, moveToY, 1, 4, 0, sizeTwoSelectedCount, GENERAL_GROUP_MOVEMENT, REUSE_PATH_SEARCH);
						}
					}
					else
						unitPtr->move_to(moveToX, moveToY, 1);
					sizeTwoUnprocessCount--;
				}
				moveToX-=2;
			}
		}

		//------------- left edge ---------------//
		moveToX = x-2;
		moveToY = y+rec_height-2;
		if(moveToX>=0)
		{
			if(y-3 < 0)
				boundedY = -1;
			else
				boundedY = y-3;
			
			while(moveToY>boundedY && sizeTwoUnprocessCount)
			{
				//--------------- is the position blocked? ----------//
				if(moveToX>=MAX_WORLD_X_LOC-1 || moveToY>=MAX_WORLD_Y_LOC-1)
					blocked = 1;
				else
				{
					blocked = 0;
					for(h=0, surY=moveToY; h<2 && !blocked; h++, surY++)
					{
						for(w=0, surX=moveToX; w<2 && !blocked; w++, surX++)
						{
							locPtr = worldLocMatrix+surY*MAX_WORLD_X_LOC+surX;
							blocked = !locPtr->is_unit_group_accessible(mobileType, curGroupId);
						}
					}
				}

				if(!blocked)
				{
					do
					{
						unitPtr = (Unit*) get_ptr(selectedUnitArray[suaCount++]);
					}while(unitPtr->sprite_info->loc_width<2);

					if(sizeTwoSelectedCount>1)
					{
						if(sizeTwoUnprocessCount==sizeTwoSelectedCount) // the first unit to move
						{	
							unitPtr->move_to(moveToX, moveToY, 1, 4, 0, sizeTwoSelectedCount, GENERAL_GROUP_MOVEMENT, REUSE_PATH_INITIAL);
							unitPtr->move_to(moveToX, moveToY, 1, 4, 0, sizeTwoSelectedCount, GENERAL_GROUP_MOVEMENT, REUSE_PATH_FIRST_SEEK);
						}
						else
						{
							err_if(sizeTwoUnprocessCount==sizeTwoSelectedCount)
								err_here();
							
							unitPtr->move_to(moveToX, moveToY, 1, 4, 0, sizeTwoSelectedCount, GENERAL_GROUP_MOVEMENT, REUSE_PATH_SEARCH);
						}
					}
					else
						unitPtr->move_to(moveToX, moveToY, 1);
					sizeTwoUnprocessCount--;
				}
				moveToY-=2;
			}
		}

		//------- reset square_size, rec_width, rec_height -----------//
		rec_width+=4;
		rec_height+=4;
		x-=2;
		y-=2;
	}*/
}
//----------- End of function UnitArray::move_to_now_gather -----------//


//----------Begin of function UnitArray::construct_sorted_array------------//
//
// construct a table to store the d(x,y) value from the reference(rectangule
// starting) point in group movement control
//
// For the detail, see ounitam.txt
//
//
// <short*> selectedUnitArray  - an array of recno of selected units.
// <int>    selectedCount 		 - no. of selected units.
//
void UnitArray::construct_sorted_array(short* selectedUnitArray, int selectedCount)
{
	Unit*			unitPtr;
	int			MIN, dist;		// for comparison
	int			i, j, k;
	const int	c = 1000;		// c value for the d(x,y) function

	if(lower_right_case >= upper_left_case)
	{
		for(i=0; i<selectedCount; i++)
		{
			unitPtr = (Unit*) get_ptr(selectedUnitArray[i]);
			// d(x,y)=x+c*|y|
			distance[i] = MAX_WORLD_X_LOC;	// to aviod -ve no. in the following line
			distance[i] += (x-unitPtr->cur_x_loc()+c*abs(unitPtr->cur_y_loc()-y)); // plus/minus x coord difference
		}
	}
	else	// upper_left_case
	{
		for(i=0; i<selectedCount; i++)
		{
			unitPtr = (Unit*) get_ptr(selectedUnitArray[i]);
			// d(x,y)=x+c*|y|
			distance[i] = MAX_WORLD_X_LOC;	// to aviod -ve no. in the following line
			distance[i] += (unitPtr->cur_x_loc()-x+c*abs(unitPtr->cur_y_loc()-y)); // plus/minus x coord difference
		}
	}
	
	//---------------------------------------------------------------//
	// this part of code using a technique to adjust the distance value
	// such that the selected group can change from lower right form
	// to upper left form or upper left form to lower right form in a
	// better way.
	//---------------------------------------------------------------//
	//------ sorting the distance and store in sortedDistance Array -------//
	for(j=0; j<selectedCount; j++)
	{
		MIN = 0xFFFFFF;
		for(i=0; i<selectedCount; i++)
		{	
			if(done_flag[i]==0 && (dist = distance[i])<MIN)
			{
				MIN = dist;
				k = i;
			}
		}
		sorted_distance[j] = k;
		done_flag[k] = 1;
	}

	//----------------- find the minimum value --------------//
	#ifdef DEBUG
		MIN = 0xFFFFFF;
		for(j=0; j<selectedCount; j++)
		{
			if((dist = distance[j])<MIN)
				MIN = dist;
		}

		err_when(MIN!=distance[sorted_distance[0]]);
	#else
		MIN = distance[sorted_distance[0]];
	#endif
	
	int defArraySize		= 5;	//****** BUGHERE, 5 is chosen arbitrary
	int *leftQuotZ  = (int*) mem_add(defArraySize*sizeof(int));
	int *rightQuotZ = (int*) mem_add(defArraySize*sizeof(int));
	int remainder = MIN%c;
	int index;

	//-- adjust the value to allow changing form between upper left and lower right shape --//
	
	for(j=0; j<defArraySize; j++)
		leftQuotZ[j] = rightQuotZ[j] = MIN-remainder;
	
	for(j=0; j<selectedCount; j++)
	{
		if((dist = distance[sorted_distance[j]]%c) < remainder)
		{
			if((index = remainder-dist) <= defArraySize)	// the case can be handled by this array size
			{
				distance[sorted_distance[j]] = leftQuotZ[index-1] + dist;
				leftQuotZ[index-1] += c;
			}
		}
		else
		{
			if(dist>=remainder)
			{
				if((index = dist-remainder) < defArraySize)	// the case can be handled by this array size
				{
					distance[sorted_distance[j]] = rightQuotZ[index] + dist;
					rightQuotZ[index] += c;
				}
			}
		}
	}

	mem_del(leftQuotZ);
	mem_del(rightQuotZ);

	//---------- sorting -------------//
	for(j=0; j<selectedCount; j++)
	{
		MIN = 0xFFFFFF;
		for(i=0; i<selectedCount; i++)
		{	
			if((dist = distance[i])<MIN)
			{
				MIN = dist;
				k = i;
			}
		}
		sorted_member[j] = k;
		distance[k] = 0xFFFFFF;
	}
}
//--------End of function UnitArray::construct_sorted_array----------------//


//--- Begin of function UnitArray::determine_position_to_construct_table ---//
//
// determine the position of the rectangule starting point in the two forms
//	(i.e. upper left case or lower right case). This point is used to indicate
// the direction to fill the rectangle
//
// <int> 	destXLoc, destYLoc - the location of the destination.
// <int>    selectedCount 		 - no. of selected units.
//
void UnitArray::determine_position_to_construct_table(int selectedCount, int destXLoc, int destYLoc, char mobileType)
{
	//======================================================================//
	// boundary, corner improvement
	//======================================================================//
	int sqrtValue;

	//======================================================================//
	// lower right case
	//======================================================================//
	if(lower_right_case>=upper_left_case)
	{
		//--------- calculate x, y location for lower right case ---------//
		x = cal_rectangle_lower_right_x(destXLoc);
		y = cal_rectangle_lower_right_y(destYLoc);

		if(x<rec_width)
		{
			//================== left edge =================//
			sqrtValue = (int)sqrt((float)selectedCount);
			if(sqrtValue*sqrtValue != selectedCount)
				sqrtValue++;
			if(mobileType!=UNIT_LAND)
				sqrtValue = sqrtValue<<1; // change to scale 2
			rec_width = rec_height = sqrtValue;
			
			//------------- top left corner --------------//
			if(y<rec_height)
			{
				upper_left_case = lower_right_case+1;
				x = y = 0;
			}
			//------------ bottom left corner ---------------//
			else if(y>=MAX_WORLD_Y_LOC-move_scale)
			{
				if(not_tested_loc>=square_size/move_scale)
					rec_width -= move_scale;

				x = rec_width-move_scale;
				y = MAX_WORLD_Y_LOC-move_scale;
			}
			//------------- just left edge -------------//
			else
				x = rec_width-move_scale;
		}
		else if(x>=MAX_WORLD_X_LOC-move_scale)
		{
			//============== right edge ==============//

			//----------- top right corner -----------//
			if(y<rec_height)
			{
				sqrtValue = (int)sqrt((float)selectedCount);
				if(sqrtValue*sqrtValue != selectedCount)
					sqrtValue++;
				if(mobileType!=UNIT_LAND)
					sqrtValue = sqrtValue<<1; // change to scale 2
				rec_width = rec_height = sqrtValue;

				upper_left_case = lower_right_case+1;
				x = MAX_WORLD_X_LOC-rec_width;
				y = 0;
			}
			//---------- bottom right corner ------------//
			else if(y>=MAX_WORLD_Y_LOC-move_scale)
			{
				y = MAX_WORLD_Y_LOC-move_scale;
				x = MAX_WORLD_X_LOC-move_scale;
			}
			//---------- just right edge ---------------//
			else
			{
				int squareSize = square_size/move_scale;
				if(squareSize*(squareSize-1)>=selectedCount)
					rec_width -= move_scale;
				x = MAX_WORLD_X_LOC-move_scale;
			}
		}
		else if(y<rec_height)
		{
			//================= top edge ===============//
			sqrtValue = (int)sqrt((float)selectedCount);
			if(sqrtValue*sqrtValue != selectedCount)
				sqrtValue++;
			if(mobileType!=UNIT_LAND)
				sqrtValue = sqrtValue<<1; // change to scale 2
			rec_width = rec_height = sqrtValue;

			upper_left_case = lower_right_case+1;
			//if(mobileType==UNIT_LAND)
			//	x = destXLoc-((rec_width-1)/2);
			//else
			//	x = destXLoc-(rec_width/4)*2;
			x = cal_rectangle_upper_left_x(destXLoc);
			y = 0;
		}
		else if(y>=MAX_WORLD_Y_LOC-move_scale)
		{
			//================== bottom edge ====================//
			if(not_tested_loc>=square_size/move_scale)
				rec_width += move_scale;
			
			//if(mobileType==UNIT_LAND)
			//	x = destXLoc+(rec_width/2);
			//else
			//	x = destXLoc+(rec_width/4)*2;
			x = cal_rectangle_lower_right_x(destXLoc);
			y = MAX_WORLD_Y_LOC-move_scale;
		}
	}
	//======================================================================//
	// upper left case
	//======================================================================//
	else
	{
		//--------- calculate x, y location for upper left case ---------//
		x = cal_rectangle_upper_left_x(destXLoc);
		y = cal_rectangle_upper_left_y(destYLoc);

		if(x<0)
		{
			//================= left edge ==================//
			
			//------------- top left corner --------------//
			if(y<0)
			{
				sqrtValue = (int)sqrt((float)selectedCount);
				if(sqrtValue*sqrtValue != selectedCount)
					sqrtValue++;
				if(mobileType!=UNIT_LAND)
					sqrtValue = sqrtValue<<1; // change to scale 2
				rec_width = rec_height = sqrtValue;
				x = y = 0;
			}
			//------------- bottom left corner --------------//
			else if(y+rec_height>=MAX_WORLD_Y_LOC-move_scale)
			{
				lower_right_case = upper_left_case+1;
				x = rec_width-move_scale;
				y = MAX_WORLD_Y_LOC-move_scale;
			}
			//------------- just left edge ------------------//
			else
			{
				sqrtValue = (int)sqrt((float)selectedCount);
				if(sqrtValue*sqrtValue != selectedCount)
					sqrtValue++;
				if(mobileType!=UNIT_LAND)
					sqrtValue = sqrtValue<<1; // change to scale 2
				rec_width = rec_height = sqrtValue;
				x = 0;
			}
		}
		//================ right edge ================//
		else if(x+rec_width>=MAX_WORLD_X_LOC-move_scale)
		{
			//------------- top right corner ------------------//
			if(y<0)
			{
				sqrtValue = (int)sqrt((float)selectedCount);
				if(sqrtValue*sqrtValue != selectedCount)
					sqrtValue++;
				if(mobileType!=UNIT_LAND)
					sqrtValue = sqrtValue<<1; // change to scale 2
				rec_width = rec_height = sqrtValue;
				x = MAX_WORLD_X_LOC-rec_width;
				y = 0;
			}
			//------------- bottom right corner ------------------//
			else if(y+rec_height>=MAX_WORLD_Y_LOC-move_scale)
			{
				lower_right_case = upper_left_case+1;
				x = MAX_WORLD_X_LOC-move_scale;
				y = MAX_WORLD_Y_LOC-move_scale;
			}
			//------------- just right edge ------------------//
			else
			{
				sqrtValue = (int)sqrt((float)selectedCount);
				if(sqrtValue*sqrtValue != selectedCount)
					sqrtValue++;
				if(mobileType!=UNIT_LAND)
					sqrtValue = sqrtValue<<1; // change to scale 2
				rec_width = rec_height = sqrtValue;

				int squareSize = square_size/move_scale;
				if(squareSize*(squareSize-1)>=selectedCount)
					rec_width -= move_scale;
				lower_right_case = upper_left_case+1;
				x = MAX_WORLD_X_LOC-move_scale;
				//if(mobileType==UNIT_LAND)
				//	y = destYLoc+((rec_height-1)/2);
				//else
				//	y = destYLoc+((rec_height-2)/4)*2;
				y = cal_rectangle_lower_right_y(destYLoc);
			}
		}
		//================= top edge ================//
		else if(y<0)
		{
			sqrtValue = (int)sqrt((float)selectedCount);
			if(sqrtValue*sqrtValue != selectedCount)
				sqrtValue++;

			rec_width = rec_height = sqrtValue;
			y = 0;
		}
		//================= bottom edge ================//
		else if(y+rec_height>=MAX_WORLD_Y_LOC-move_scale)
		{
			if(not_tested_loc>=square_size)
				rec_width += move_scale;
			y = MAX_WORLD_Y_LOC-move_scale;
		}
	}

	/*if(lower_right_case>=upper_left_case)
	{
		x = cal_rectangle_lower_right_x(destXLoc);
		y = cal_rectangle_lower_right_y(destYLoc);

		rec_x1 = x - rec_width + move_scale;
		rec_y1 = y - rec_height + move_scale;
		rec_x2 = x;
		rec_y2 = y;
	}
	else
	{
		x = cal_rectangle_upper_left_x(destXLoc);
		y = cal_rectangle_upper_left_y(destYLoc);
	}*/
}
//--- End of function UnitArray::determine_position_to_construct_table -----//
