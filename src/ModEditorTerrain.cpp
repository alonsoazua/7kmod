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

//Filename    : ModEditorTerrain.cpp
//Description : 7kmod-editor: the Terrain tab (phase 5): trees, rocks,
//              clearing, painting the type of the ground (5a, 5b), and water,
//              land and hills (5c).
//
// The ground: each corner of the map has a type (sea, dark grass, light
// grass, dark dirt) and a subtype (bottom, middle, top): 12 "levels". Each
// location is a tile (terrain_id) that joins the levels of its 4 corners
// (TerrainRes::scan()). The height map of the generator is not kept, so the
// levels of the corners are rebuilt from the tiles.
//
// There is a tile for any 4 corners within 3 levels of each other (checked
// with the 3 terrain sets), so painting keeps the corners next to each
// other within 3 levels: the corners around the brush follow it (the rings
// of transition between types are made automatically).
//
// The ground types only paint land: the corners of tiles with sea (coasts),
// hills, buildings, towns, natural resources or special tiles are not
// changed, so what can be walked or sailed doesn't change. Water, Land and
// Hill (5c) do change it: then the flags of the locations and the regions
// are calculated again (paint_shape()).

#include <ModEditor.h>
#include <OSYS.h>
#include <OWORLD.h>
#include <OWORLDMT.h>
#include <OMATRIX.h>
#include <OTERRAIN.h>
#include <OPLANT.h>
#include <OROCK.h>
#include <OROCKRES.h>
#include <OMISC.h>
#include <OHILLRES.h>
#include <OREGION.h>
#include <OTOWN.h>
#include <OFIRM.h>
#include <OFIRMRES.h>
#include <OFIRMID.h>
#include <OF_HARB.h>
#include <OSITE.h>
#include <ONATION.h>
#include <OBOX.h>
#include "gettext.h"
#include <string.h>
#include <stdlib.h>

//------- the levels of the corners --------//

enum { LEVEL_PER_TYPE = 3,
		 MAX_LEVEL_STEP = 3,					// the maximum difference of levels of the corners of a tile
		 LEVEL_PROTECTED = -1 };

enum { MAX_BRUSH = 5,
		 BOX_MARGIN = 4,						// the corners around the brush that may follow it
		 BOX_SIZE = MAX_BRUSH + 1 + BOX_MARGIN*2 };

static char corner_level[BOX_SIZE][BOX_SIZE];
static char corner_orig_level[BOX_SIZE][BOX_SIZE];
static char corner_fixed[BOX_SIZE][BOX_SIZE];		// 2 - protected, 1 - in the brush, 0 - free

//------- what paint_ground() may change --------//

enum { PAINT_GROUND,		// the ground types: only land
		 PAINT_SHAPE,		// water and land: also the sea and the coasts, not where there are units
		 PAINT_PLATEAU };	// rebuilding hills (rebuild_plateau()): the hills may change

static char paint_mode = PAINT_GROUND;

static int  changed_x1, changed_y1, changed_x2, changed_y2;		// the tiles changed by the last paint_ground()

//------- Declare static functions --------//

static int  tile_is_protected(int xLoc, int yLoc);
static int  get_corner_level(int cornerX, int cornerY, int& isProtected);
static int  level_type(int level)		{ return level / LEVEL_PER_TYPE + 1; }
static int  level_subtype(int level)	{ return 1 << (level % LEVEL_PER_TYPE); }
static int  paint_ground(int terrainType, int x1, int y1, int x2, int y2);
static int  add_trees(int x1, int y1, int x2, int y2);
static int  clear_area(int x1, int y1, int x2, int y2);
static int  add_rock(int largeFlag, int xLoc, int yLoc, int checkOnly);
static void remove_rock(int rockArrayRecno, RockArray& rockArray, int isDirt);
static void reset_power_off(int x1, int y1, int x2, int y2);
static int  paint_shape(int shapeType, int x1, int y1, int x2, int y2);
static int  rebuild_plateau(int bx1, int by1, int bx2, int by2, int addFlag, int rampX, int rampY);
static int  brush_has_plateau(int x1, int y1, int x2, int y2);
static void set_fire_src(Location* locPtr);


//-------- Begin of function editor_terrain_place --------//
//
// Place something of the Terrain tab.
//
// <int> placeType - EDITOR_TERRAIN_???
// <int> placeId   - the terrain type for EDITOR_TERRAIN_GROUND, 1 for large rocks
// <int> xLoc, yLoc - the location clicked
// <int> brushSize  - 1, 3 or 5
//
// return: <int> 1 - something has changed
//
int editor_terrain_place(int placeType, int placeId, int xLoc, int yLoc, int brushSize)
{
	int x1 = xLoc - brushSize/2;
	int y1 = yLoc - brushSize/2;
	int x2 = x1 + brushSize - 1;
	int y2 = y1 + brushSize - 1;

	x1 = MAX(x1, 0);
	y1 = MAX(y1, 0);
	x2 = MIN(x2, MAX_WORLD_X_LOC-1);
	y2 = MIN(y2, MAX_WORLD_Y_LOC-1);

	switch( placeType )
	{
		case EDITOR_TERRAIN_TREE:
			return add_trees(x1, y1, x2, y2);

		case EDITOR_TERRAIN_ROCK:
			return add_rock(placeId, xLoc, yLoc, 0);

		case EDITOR_TERRAIN_CLEAR:
		{
			int rc = clear_area(x1, y1, x2, y2);

			if( brush_has_plateau(x1, y1, x2, y2) )		// Clear also removes the plateau (its hills are rebuilt around)
				rc = rebuild_plateau(x1, y1, x2, y2, 0, -1, -1) || rc;

			return rc;
		}

		case EDITOR_TERRAIN_RAMP:
			return rebuild_plateau(xLoc, yLoc, xLoc, yLoc, -1, xLoc, yLoc);

		case EDITOR_TERRAIN_GROUND:
			return paint_ground(placeId, x1, y1, x2, y2);

		case EDITOR_TERRAIN_WATER:
		case EDITOR_TERRAIN_LAND:
		case EDITOR_TERRAIN_HILL:
			return paint_shape(placeType, x1, y1, x2, y2);
	}

	return 0;
}
//--------- End of function editor_terrain_place ---------//


//-------- Begin of function editor_terrain_area --------//
//
// The area of the marker of the Terrain tab.
//
// return: <int> 1 - it can be placed, 0 - it can't
//
int editor_terrain_area(int placeType, int placeId, int xLoc, int yLoc, int brushSize, int& locWidth, int& locHeight)
{
	if( placeType == EDITOR_TERRAIN_ROCK )
	{
		locWidth = locHeight = placeId ? 4 : 2;
		return add_rock(placeId, xLoc, yLoc, 1);
	}

	if( placeType == EDITOR_TERRAIN_RAMP )
	{
		locWidth = locHeight = 3;
		return 1;
	}

	locWidth = locHeight = brushSize;
	return 1;
}
//--------- End of function editor_terrain_area ---------//


//-------- Begin of static function add_trees --------//
//
// Put a tree of the type of the ground on each empty location of the area.
//
static int add_trees(int x1, int y1, int x2, int y2)
{
	int count=0;

	for( int y=y1 ; y<=y2 ; y++ )
	{
		for( int x=x1 ; x<=x2 ; x++ )
		{
			Location* locPtr = world.get_loc(x, y);

			if( !locPtr->can_add_plant() || locPtr->is_power_off() )
				continue;

			int teraType = terrain_res[locPtr->terrain_id]->average_type;
			int plantBitmap = plant_res.scan(0, teraType, 0);

			if( !plantBitmap )
				continue;

			int plantId = plant_res.plant_recno(plantBitmap);
			PlantInfo* plantInfo = plant_res[plantId];

			if( plantInfo->tera_type[0] != teraType && plantInfo->tera_type[1] != teraType &&
				 plantInfo->tera_type[2] != teraType )
			{
				continue;
			}

			//--- a grown tree: one of the larger bitmaps ---//

			int bitmapCount = plantInfo->bitmap_count;
			int plantSize = bitmapCount/2 + misc.random(bitmapCount - bitmapCount/2);

			locPtr->set_plant( plantInfo->first_bitmap + plantSize,
				ZOOM_LOC_WIDTH/4 + misc.random(ZOOM_LOC_WIDTH/2), ZOOM_LOC_HEIGHT*3/8 + misc.random(ZOOM_LOC_HEIGHT/4) );

			locPtr->set_fire_src(100);
			world.plant_count++;
			count++;
		}
	}

	return count > 0;
}
//----------- End of static function add_trees -----------//


//-------- Begin of static function clear_area --------//
//
// Remove the trees, the rocks and the dirt patches in the area.
//
static int clear_area(int x1, int y1, int x2, int y2)
{
	int count=0;

	for( int y=y1 ; y<=y2 ; y++ )
	{
		for( int x=x1 ; x<=x2 ; x++ )
		{
			Location* locPtr = world.get_loc(x, y);

			if( locPtr->is_plant() )
			{
				locPtr->remove_plant();
				world.plant_count--;
				count++;
			}
			else if( locPtr->is_rock() )
			{
				remove_rock( locPtr->rock_array_recno(), rock_array, 0 );
				count++;
			}

			if( locPtr->has_dirt() )
			{
				remove_rock( locPtr->dirt_recno(), dirt_array, 1 );
				count++;
			}
		}
	}

	return count > 0;
}
//----------- End of static function clear_area -----------//


//-------- Begin of static function remove_rock --------//
//
// Remove a rock (or a dirt patch) from all its locations.
//
static void remove_rock(int rockArrayRecno, RockArray& rockArray, int isDirt)
{
	if( !rockArrayRecno || !rockArray.get(rockArrayRecno) )
		return;

	Rock* rockPtr = rockArray[rockArrayRecno];
	RockInfo* rockInfo = rock_res.get_rock_info(rockPtr->rock_recno);

	int x1 = rockPtr->loc_x, y1 = rockPtr->loc_y;
	int x2 = MIN( x1 + rockInfo->loc_width - 1, MAX_WORLD_X_LOC-1 );
	int y2 = MIN( y1 + rockInfo->loc_height - 1, MAX_WORLD_Y_LOC-1 );

	for( int y=y1 ; y<=y2 ; y++ )
	{
		for( int x=x1 ; x<=x2 ; x++ )
		{
			Location* locPtr = world.get_loc(x, y);

			if( isDirt )
			{
				if( locPtr->has_dirt() && locPtr->dirt_recno() == rockArrayRecno )
					locPtr->remove_dirt();
			}
			else
			{
				if( locPtr->is_rock() && locPtr->rock_array_recno() == rockArrayRecno )
					locPtr->remove_rock();
			}
		}
	}

	rockArray.del(rockArrayRecno);

	if( !isDirt )
		reset_power_off(x1-1, y1-1, x2+1, y2+1);
}
//----------- End of static function remove_rock -----------//


//-------- Begin of static function reset_power_off --------//
//
// After removing a rock, the locations where buildings can be built again:
// a rock sets "power off" on its locations and around them. Here it is
// cleared, except on the edges of the map, coasts, hills, rocks and next to
// rocks and hills (as World::set_loc_flags(), World::add_rock() and
// World::gen_hills()).
//
static void reset_power_off(int x1, int y1, int x2, int y2)
{
	x1 = MAX(x1, 1);
	y1 = MAX(y1, 1);
	x2 = MIN(x2, MAX_WORLD_X_LOC-2);
	y2 = MIN(y2, MAX_WORLD_Y_LOC-2);

	for( int y=y1 ; y<=y2 ; y++ )
	{
		for( int x=x1 ; x<=x2 ; x++ )
		{
			Location* locPtr = world.get_loc(x, y);

			if( !(locPtr->loc_flag & LOCATE_POWER_OFF) )
				continue;

			if( locPtr->loc_flag & LOCATE_COAST )
				continue;

			int keepFlag = 0;

			for( int dy=-1 ; dy<=1 && !keepFlag ; dy++ )
			{
				for( int dx=-1 ; dx<=1 ; dx++ )
				{
					Location* loc2 = world.get_loc(x+dx, y+dy);

					if( loc2->is_rock() || loc2->has_hill() ||
						 ((loc2->loc_flag & LOCATE_COAST) && terrain_res[loc2->terrain_id]->average_type==TERRAIN_OCEAN) )
					{
						keepFlag = 1;
						break;
					}
				}
			}

			if( !keepFlag )
				locPtr->loc_flag &= ~LOCATE_POWER_OFF;
		}
	}
}
//----------- End of static function reset_power_off -----------//


//-------- Begin of static function add_rock --------//
//
// Put a small (2x2 or less) or large (up to 6x6) rock at the location, of
// the type of the ground, as World::gen_rocks().
//
// <int> checkOnly - 1: only check whether a rock of that size fits
//
static int add_rock(int largeFlag, int xLoc, int yLoc, int checkOnly)
{
	int size = largeFlag ? 4 : 2;
	int x2 = xLoc + size - 1, y2 = yLoc + size - 1;

	if( x2 >= MAX_WORLD_X_LOC || y2 >= MAX_WORLD_Y_LOC )
		return 0;

	if( !world.can_add_rock(xLoc, yLoc, x2, y2) )
		return 0;

	if( checkOnly )
		return 1;

	int teraType = terrain_res[world.get_loc(xLoc, yLoc)->terrain_id]->average_type;
	short rockRecno;

	if( largeFlag )
		rockRecno = rock_res.search("R", 3, 4, 3, 4, -1, 0, teraType);
	else
		rockRecno = rock_res.search("R", 1, 2, 1, 2, -1, 0, teraType);

	if( !rockRecno )
		return 0;

	RockInfo* rockInfo = rock_res.get_rock_info(rockRecno);

	x2 = xLoc + rockInfo->loc_width - 1;
	y2 = yLoc + rockInfo->loc_height - 1;

	if( x2 >= MAX_WORLD_X_LOC || y2 >= MAX_WORLD_Y_LOC ||
		 !world.can_add_rock(xLoc, yLoc, x2, y2) ||
		 !rockInfo->valid_terrain(terrain_res[world.get_loc(x2, yLoc)->terrain_id]->average_type) ||
		 !rockInfo->valid_terrain(terrain_res[world.get_loc(xLoc, y2)->terrain_id]->average_type) ||
		 !rockInfo->valid_terrain(terrain_res[world.get_loc(x2, y2)->terrain_id]->average_type) )
	{
		return 0;
	}

	world.add_rock(rockRecno, xLoc, yLoc);
	return 1;
}
//----------- End of static function add_rock -----------//


//======== 5c: water, land and hills ========//
//
// They change what can be walked or sailed, so after them the flags of the
// locations (coast, walkable, power off) and the regions are calculated
// again, as World::set_loc_flags(), set_region_id() and set_harbor_bit() do
// when the map is generated.

enum { MAX_CHANGED = BOX_SIZE*BOX_SIZE };

static short changed_x[MAX_CHANGED], changed_y[MAX_CHANGED], changed_old_id[MAX_CHANGED];
static int   changed_count;


static uint8_t* region_buf;		// the regions being calculated, one per location
static int*     fill_stack;		// the stack of the flood fill (x, y pairs)


//-------- Begin of static function base_region_type --------//
//
// The type of a location for the regions, as when the map is generated:
// before the buildings, trees and rocks (only hills block).
//
static char region_override;		// 1 - count_regions() takes the rebuilt hills (post_cell) instead of the map

static RegionType base_region_type(Location* locPtr)
{
	if( locPtr->has_hill() )
		return REGION_INPASSABLE;

	if( terrain_res[locPtr->terrain_id]->average_type == TERRAIN_OCEAN )
		return REGION_SEA;

	return REGION_LAND;
}
//----------- End of static function base_region_type -----------//


static RegionType region_type_at(int x, int y);

//-------- Begin of static function count_regions --------//
//
// Fill region_buf with the regions of the map (flood fill).
//
// return: <int> the number of regions, -1 - more than the game supports
//
static int count_regions()
{
	int locCount = MAX_WORLD_X_LOC*MAX_WORLD_Y_LOC;

	region_buf = (uint8_t*) mem_resize( region_buf, locCount );
	fill_stack = (int*) mem_resize( fill_stack, locCount*2*sizeof(int) );

	memset( region_buf, 0, locCount );

	int regionCount=0;

	for( int y=0 ; y<MAX_WORLD_Y_LOC ; y++ )
	{
		for( int x=0 ; x<MAX_WORLD_X_LOC ; x++ )
		{
			if( region_buf[y*MAX_WORLD_X_LOC+x] )
				continue;

			RegionType regionType = region_type_at(x, y);

			if( regionType == REGION_INPASSABLE )
				continue;

			if( ++regionCount >= 255 )
				return -1;

			int top=0;
			fill_stack[top*2] = x;  fill_stack[top*2+1] = y;  top++;
			region_buf[y*MAX_WORLD_X_LOC+x] = regionCount;

			while( top > 0 )
			{
				top--;
				int cx = fill_stack[top*2], cy = fill_stack[top*2+1];

				static int dxArray[4] = { -1, 1, 0, 0 }, dyArray[4] = { 0, 0, -1, 1 };

				for( int i=0 ; i<4 ; i++ )
				{
					int nx = cx+dxArray[i], ny = cy+dyArray[i];

					if( nx < 0 || ny < 0 || nx >= MAX_WORLD_X_LOC || ny >= MAX_WORLD_Y_LOC )
						continue;

					if( region_buf[ny*MAX_WORLD_X_LOC+nx] || region_type_at(nx, ny) != regionType )
						continue;

					region_buf[ny*MAX_WORLD_X_LOC+nx] = regionCount;
					fill_stack[top*2] = nx;  fill_stack[top*2+1] = ny;  top++;
				}
			}
		}
	}

	return regionCount;
}
//----------- End of static function count_regions -----------//


//-------- Begin of static function apply_regions --------//
//
// Set the regions of region_buf: the locations, RegionArray (as
// World::set_region_id()), the towns, buildings and sites, and the regions
// of the AI.
//
static void apply_regions(int regionCount)
{
	int x, y;

	for( y=0 ; y<MAX_WORLD_Y_LOC ; y++ )
		for( x=0 ; x<MAX_WORLD_X_LOC ; x++ )
			world.get_loc(x,y)->region_id = region_buf[y*MAX_WORLD_X_LOC+x];

	//------- RegionArray: sizes, types and adjacency -------//

	region_array.init(regionCount);

	int regionId=0;

	for( y=0 ; y<MAX_WORLD_Y_LOC ; y++ )
	{
		for( x=0 ; x<MAX_WORLD_X_LOC ; x++ )
		{
			Location* locPtr = world.get_loc(x,y);
			int thisRegionId = locPtr->region_id;

			if( !thisRegionId )
				continue;

			region_array.inc_size( thisRegionId );

			if( thisRegionId > regionId )
			{
				if( thisRegionId == regionId+1 )
					regionId++;

				region_array.set_region( thisRegionId, base_region_type(locPtr) );
			}

			for( int dy=-1 ; dy<=1 ; dy++ )
			{
				for( int dx=-1 ; dx<=1 ; dx++ )
				{
					int nx=x+dx, ny=y+dy;

					if( (!dx && !dy) || nx < 0 || ny < 0 || nx >= MAX_WORLD_X_LOC || ny >= MAX_WORLD_Y_LOC )
						continue;

					int adjRegionId = world.get_loc(nx,ny)->region_id;

					if( adjRegionId && adjRegionId < thisRegionId )
						region_array.set_adjacent( thisRegionId, adjRegionId );
				}
			}
		}
	}

	region_array.sort_region();

	//------- the regions of the towns, buildings and sites -------//

	for( int i=town_array.size() ; i>0 ; i-- )
	{
		if( !town_array.is_deleted(i) )
		{
			Town* townPtr = town_array[i];
			townPtr->region_id = world.get_region_id( townPtr->center_x, townPtr->center_y );
		}
	}

	for( int i=firm_array.size() ; i>0 ; i-- )
	{
		if( firm_array.is_deleted(i) )
			continue;

		Firm* firmPtr = firm_array[i];
		firmPtr->region_id = world.get_region_id( firmPtr->center_x, firmPtr->center_y );

		if( firmPtr->firm_id == FIRM_HARBOR )		// the land and the sea of a harbor, as FirmHarbor::init()
		{
			FirmHarbor* harborPtr = (FirmHarbor*) firmPtr;
			int x1 = firmPtr->loc_x1, y1 = firmPtr->loc_y1;

			switch( firm_res.get_build(firmPtr->firm_build_id)->build_code[0] )
			{
				case 'N':
					harborPtr->land_region_id = world.get_loc(x1+1, y1+2)->region_id;
					harborPtr->sea_region_id  = world.get_loc(x1+1, y1)->region_id;
					break;
				case 'S':
					harborPtr->land_region_id = world.get_loc(x1+1, y1)->region_id;
					harborPtr->sea_region_id  = world.get_loc(x1+1, y1+2)->region_id;
					break;
				case 'W':
					harborPtr->land_region_id = world.get_loc(x1+2, y1+1)->region_id;
					harborPtr->sea_region_id  = world.get_loc(x1, y1+1)->region_id;
					break;
				case 'E':
					harborPtr->land_region_id = world.get_loc(x1, y1+1)->region_id;
					harborPtr->sea_region_id  = world.get_loc(x1+2, y1+1)->region_id;
					break;
			}
		}
	}

	for( int i=site_array.size() ; i>0 ; i-- )
	{
		if( !site_array.is_deleted(i) )
		{
			Site* sitePtr = site_array[i];
			sitePtr->region_id = world.get_region_id( sitePtr->map_x_loc, sitePtr->map_y_loc );
		}
	}

	//------- the statistics of the regions and the regions of the AI -------//

	region_array.init_region_stat();

	for( int i=nation_array.size() ; i>0 ; i-- )
	{
		if( !nation_array.is_deleted(i) && nation_array[i]->is_ai() )
			nation_array[i]->update_ai_region();
	}
}
//----------- End of static function apply_regions -----------//


//-------- Begin of static function update_location_flags --------//
//
// After the ground changes between land and sea: the coast flag, walkable
// or sailable, and "power off" (no building on coasts and next to the sea),
// as World::set_loc_flags().
//
static void update_location_flags(int x1, int y1, int x2, int y2)
{
	x1 = MAX(x1, 0);
	y1 = MAX(y1, 0);
	x2 = MIN(x2, MAX_WORLD_X_LOC-1);
	y2 = MIN(y2, MAX_WORLD_Y_LOC-1);

	for( int y=y1 ; y<=y2 ; y++ )
	{
		for( int x=x1 ; x<=x2 ; x++ )
		{
			Location* locPtr = world.get_loc(x, y);
			TerrainInfo* terrainInfo = terrain_res[locPtr->terrain_id];

			//--- no dirt patches on the sea ---//

			if( locPtr->has_dirt() && terrainInfo->average_type == TERRAIN_OCEAN )
				remove_rock( locPtr->dirt_recno(), dirt_array, 1 );

			if( !(locPtr->loc_flag & LOCATE_BLOCK_MASK) )
			{
				locPtr->walkable_off();
				locPtr->walkable_reset();
			}

			locPtr->loc_flag &= ~LOCATE_COAST;
		}
	}

	for( int y=y1 ; y<=y2 ; y++ )
	{
		for( int x=x1 ; x<=x2 ; x++ )
		{
			Location* locPtr = world.get_loc(x, y);
			TerrainInfo* terrainInfo = terrain_res[locPtr->terrain_id];

			if( terrainInfo->is_coast() )
			{
				locPtr->loc_flag |= LOCATE_COAST;

				if( terrainInfo->average_type != TERRAIN_OCEAN )
					locPtr->set_power_off();
				else
					world.set_surr_power_off(x, y);
			}

			set_fire_src(locPtr);
		}
	}

	reset_power_off(x1, y1, x2, y2);		// the land that is no longer next to the sea
}
//----------- End of static function update_location_flags -----------//


//-------- Begin of static function undo_shape --------//
//
// Restore the tiles changed, when the map would have too many regions.
//
static void undo_shape()
{
	for( int i=0 ; i<changed_count ; i++ )
		world.get_loc(changed_x[i], changed_y[i])->terrain_id = changed_old_id[i];
}
//----------- End of static function undo_shape -----------//




//-------- Begin of static function paint_shape --------//
//
// Water, land or a hill on the locations x1..x2, y1..y2.
//
// return: <int> 1 - the map has changed
//
static int paint_shape(int shapeType, int x1, int y1, int x2, int y2)
{
	int rc;

	switch( shapeType )
	{
		case EDITOR_TERRAIN_WATER:
			paint_mode = PAINT_SHAPE;
			clear_area(x1, y1, x2, y2);			// no trees, rocks or dirt patches in the water
			rc = paint_ground(TERRAIN_OCEAN, x1, y1, x2, y2);
			break;

		case EDITOR_TERRAIN_LAND:
			paint_mode = PAINT_SHAPE;
			rc = paint_ground(TERRAIN_DARK_GRASS, x1, y1, x2, y2);
			break;

		default:		// EDITOR_TERRAIN_HILL: the plateau and its hills are rebuilt
			return rebuild_plateau(x1, y1, x2, y2, 1, -1, -1);
	}

	paint_mode = PAINT_GROUND;

	if( !rc )
		return 0;

	update_location_flags(changed_x1-1, changed_y1-1, changed_x2+1, changed_y2+1);

	//------- the regions; too many: undo -------//

	int regionCount = count_regions();

	if( regionCount < 0 )
	{
		undo_shape();
		update_location_flags(changed_x1-1, changed_y1-1, changed_x2+1, changed_y2+1);

		box.msg( _("The map would have too many separate areas of land and water.") );
		return 0;
	}

	apply_regions(regionCount);

	//------- where harbors can be built, around the tiles changed (as World::set_harbor_bit()) -------//

	{
		for( int y=MAX(changed_y1-3, 0) ; y<=MIN(changed_y2+1, MAX_WORLD_Y_LOC-3) ; y++ )
		{
			for( int x=MAX(changed_x1-3, 0) ; x<=MIN(changed_x2+1, MAX_WORLD_X_LOC-3) ; x++ )
			{
				Location* locPtr = world.get_loc(x,y);
				locPtr->clear_harbor_bit();

				if( world.can_build_firm(x, y, FIRM_HARBOR) )
					locPtr->set_harbor_bit();
			}
		}
	}

	return 1;
}
//----------- End of static function paint_shape -----------//






//======== 5c: hills (plateaus) ========//
//
// The map generator makes the hills from the height map (World::gen_hills()):
// the high ground (dirt) is a plateau; each tile on its edge (dirt and lower
// ground) gets the low block of the hill of its pattern and the tile above
// it the high block; the tiles inside are moved one row up (the top of the
// plateau looks higher); then straight parts of the edges get exits (ramps).
//
// The editor works on the plateau, not on the hill blocks: around the brush
// it rebuilds the plateau as it was before gen_hills() (the "pre" tiles,
// reconstructed from the map: a tile is on the plateau if the tile above it
// has a high hill block or is the top of a plateau), adds or removes the
// brush, and runs the steps of gen_hills() again on that area. So strokes
// next to each other join, and paths can be drawn.

enum { HILL_MARGIN = 6,						// the tiles rebuilt around the brush
		 HILL_GRID = MAX_BRUSH + 2*HILL_MARGIN + 8 };

enum { EXIT_SOUTH, EXIT_NORTH, EXIT_WEST, EXIT_EAST, EXIT_DIR_COUNT };

enum { MIN_EXIT_SEPARATION = 7 };		// as gen_hills()

struct HillCell
{
	short terrain_id;
	short hill1;			// the top block (Location::cargo_recno of a hill)
	short hill2;			// the block below (Location::extra_para of a hill)
};

static uint8_t* top_map;						// 1 - the tile is the top of a plateau (or its high edge), for the whole map

static int		grid_x0, grid_y0;				// the world location of grid[0][0]
static char		pre_plateau[HILL_GRID][HILL_GRID];		// 1 - the tile was high ground before gen_hills()
static short	pre_terrain[HILL_GRID][HILL_GRID];
static HillCell post_cell[HILL_GRID][HILL_GRID];
static char		was_plateau[HILL_GRID][HILL_GRID];		// 1 - the tile was part of a plateau before this change

static int		rebuild_x1, rebuild_y1, rebuild_x2, rebuild_y2;		// the area rebuilt (world locations)
static int		auto_exit_x1, auto_exit_y1, auto_exit_x2, auto_exit_y2;	// new exits only here
static int		no_exit_x1, no_exit_y1, no_exit_x2, no_exit_y2;			// no exits here (removing a ramp)

#define GRID(x,y)		[(y)-grid_y0][(x)-grid_x0]

static int		mark_top_map();
static int		cell_has_hill(HillCell& c)		{ return c.hill1 != 0; }
static void		cell_set_hill(HillCell& c, int hillId);
static void		cell_remove_hill(HillCell& c)	{ c.hill1 = c.hill2 = 0; }
static int		cell_walkable(HillCell& c);
static int		try_exit(int dir, int x, int y, int checkOnly);
static int		exit_near(int dir, int x, int y);
static int		rebuild_plateau(int bx1, int by1, int bx2, int by2, int addFlag, int rampX, int rampY);
static void		set_fire_src(Location* locPtr);


//-------- Begin of static function cell_set_hill --------//
//
// As Location::set_hill(): the block of higher priority is on top.
//
static void cell_set_hill(HillCell& c, int hillId)
{
	if( !hillId )
		return;

	if( c.hill1 )
	{
		if( hill_res[c.hill1]->priority <= hill_res[hillId]->priority )
		{
			c.hill2 = c.hill1;
			c.hill1 = hillId;
		}
		else
			c.hill2 = hillId;
	}
	else
	{
		c.hill1 = hillId;
		c.hill2 = 0;
	}
}
//----------- End of static function cell_set_hill -----------//


//-------- Begin of static function cell_walkable --------//
//
// As the generator sees it: no hill and not sea.
//
static int cell_walkable(HillCell& c)
{
	return !c.hill1 && terrain_res[c.terrain_id]->average_type != TERRAIN_OCEAN;
}
//----------- End of static function cell_walkable -----------//


//-------- Begin of static function is_dirt_top --------//
//
// A tile whose ground is all dirt (the top of a plateau, or flat dirt).
//
static int is_dirt_top(Location* locPtr)
{
	TerrainInfo* terrainInfo = terrain_res[locPtr->terrain_id];

	return terrainInfo->average_type >= TERRAIN_DARK_DIRT && terrainInfo->secondary_type >= TERRAIN_DARK_DIRT;
}
//----------- End of static function is_dirt_top -----------//


//-------- Begin of static function is_exit_ground --------//
//
// The walkable middle of an exit: a special tile between dirt and lower ground.
//
static int is_exit_ground(Location* locPtr)
{
	TerrainInfo* terrainInfo = terrain_res[locPtr->terrain_id];

	return terrainInfo->special_flag >= 'A' && terrainInfo->special_flag <= 'L' &&
			 MAX(terrainInfo->average_type, terrainInfo->secondary_type) >= TERRAIN_DARK_DIRT;
}
//----------- End of static function is_exit_ground -----------//


//-------- Begin of static function mark_top_map --------//
//
// For the whole map: the tiles that are the top of a plateau: high hill
// blocks, the middle of exits, and the dirt connected to them (flat dirt
// painted with the Dirt entry, not next to hills, isn't a plateau).
//
static int mark_top_map()
{
	int locCount = MAX_WORLD_X_LOC*MAX_WORLD_Y_LOC;

	top_map    = (uint8_t*) mem_resize( top_map, locCount );
	fill_stack = (int*) mem_resize( fill_stack, locCount*2*sizeof(int) );

	memset( top_map, 0, locCount );

	int top=0;

	for( int y=0 ; y<MAX_WORLD_Y_LOC ; y++ )
	{
		for( int x=0 ; x<MAX_WORLD_X_LOC ; x++ )
		{
			Location* locPtr = world.get_loc(x, y);

			if( locPtr->has_hill() || is_exit_ground(locPtr) )
			{
				top_map[y*MAX_WORLD_X_LOC+x] = 2;		// 2 - a seed (low blocks are seeds but not tops)
				fill_stack[top*2] = x;  fill_stack[top*2+1] = y;  top++;
			}
		}
	}

	//--- the dirt reached from the hills ---//

	while( top > 0 )
	{
		top--;
		int cx = fill_stack[top*2], cy = fill_stack[top*2+1];

		static int dxArray[4] = { -1, 1, 0, 0 }, dyArray[4] = { 0, 0, -1, 1 };

		for( int i=0 ; i<4 ; i++ )
		{
			int nx = cx+dxArray[i], ny = cy+dyArray[i];

			if( nx < 0 || ny < 0 || nx >= MAX_WORLD_X_LOC || ny >= MAX_WORLD_Y_LOC )
				continue;

			if( top_map[ny*MAX_WORLD_X_LOC+nx] )
				continue;

			Location* locPtr = world.get_loc(nx, ny);

			if( locPtr->has_hill() || !is_dirt_top(locPtr) )
				continue;

			top_map[ny*MAX_WORLD_X_LOC+nx] = 1;
			fill_stack[top*2] = nx;  fill_stack[top*2+1] = ny;  top++;
		}
	}

	//--- the seeds: tops if they have a high block or are the middle of an exit ---//

	for( int y=0 ; y<MAX_WORLD_Y_LOC ; y++ )
	{
		for( int x=0 ; x<MAX_WORLD_X_LOC ; x++ )
		{
			uint8_t& t = top_map[y*MAX_WORLD_X_LOC+x];

			if( t != 2 )
				continue;

			Location* locPtr = world.get_loc(x, y);

			t = 0;

			if( is_exit_ground(locPtr) )
				t = 1;
			else if( hill_res[locPtr->hill_id1()]->priority == HIGH_HILL_PRIORITY ||
						(locPtr->hill_id2() && hill_res[locPtr->hill_id2()]->priority == HIGH_HILL_PRIORITY) )
			{
				t = 1;
			}
		}
	}

	return 1;
}
//----------- End of static function mark_top_map -----------//


//-------- Begin of static function set_fire_src --------//
//
// The flammability of a location, as World::init_fire().
//
static void set_fire_src(Location* locPtr)
{
	if( locPtr->has_hill() )
		locPtr->set_fire_src(-100);
	else if( locPtr->is_wall() )
		locPtr->set_fire_src(-50);
	else if( locPtr->is_firm() || locPtr->is_plant() || locPtr->is_town() )
		locPtr->set_fire_src(100);
	else
	{
		switch( terrain_res[locPtr->terrain_id]->average_type )
		{
			case TERRAIN_OCEAN:			locPtr->set_fire_src(-100);  break;
			case TERRAIN_DARK_GRASS:	locPtr->set_fire_src(100);   break;
			case TERRAIN_LIGHT_GRASS:	locPtr->set_fire_src(50);    break;
			default:							locPtr->set_fire_src(-50);   break;
		}
	}
}
//----------- End of static function set_fire_src -----------//


//-------- Begin of static function exit_near --------//
//
// Whether there is an exit of this direction along the same edge within
// MIN_EXIT_SEPARATION of the location, in the new tiles (the grid) or on
// the map.
//
static int exit_near(int dir, int x, int y)
{
	static char leftSpecial[EXIT_DIR_COUNT] = { 'B', 'D', 'G', 'J' };

	for( int d=-MIN_EXIT_SEPARATION ; d<=MIN_EXIT_SEPARATION ; d++ )
	{
		int ex = x, ey = y;

		if( dir == EXIT_SOUTH || dir == EXIT_NORTH )
			ex += d;
		else
			ey += d;

		int hillId;

		if( ex >= rebuild_x1 && ex <= rebuild_x2 && ey >= rebuild_y1 && ey <= rebuild_y2 )
			hillId = post_cell GRID(ex,ey).hill1;
		else if( ex >= 0 && ey >= 0 && ex < MAX_WORLD_X_LOC && ey < MAX_WORLD_Y_LOC && world.get_loc(ex,ey)->has_hill() )
			hillId = world.get_loc(ex,ey)->hill_id1();
		else
			continue;

		if( hillId && hill_res[hillId]->special_flag == leftSpecial[dir] )
			return 1;
	}

	return 0;
}
//----------- End of static function exit_near -----------//


//-------- Begin of static function try_exit --------//
//
// An exit (ramp) as gen_hills() makes them, on the new tiles (the grid).
//
// <int> dir    - EXIT_SOUTH, EXIT_NORTH, EXIT_WEST or EXIT_EAST
// <int> x, y   - the first block of the exit (the left one, or the top one)
// <int> checkOnly - 1: only check whether an exit fits there
//
// return: <int> 1 - the exit fits (and was made)
//
static int try_exit(int dir, int x, int y, int checkOnly)
{
	enum { SOUTH_PATTERN1=11, SOUTH_PATTERN2=15, SOUTH_PATTERN3=19, SOUTH_PATTERN4=23,
			 NORTH_PATTERN1=12, NORTH_PATTERN2=16, NORTH_PATTERN3=20, NORTH_PATTERN4=24,
			 WEST_PATTERN1=9,   WEST_PATTERN2=13,  WEST_PATTERN3=17,  WEST_PATTERN4=21,
			 EAST_PATTERN1=10,  EAST_PATTERN2=14,  EAST_PATTERN3=18,  EAST_PATTERN4=22 };

	const int terrainType = TERRAIN_DARK_DIRT;

	//--- all the cells used must be in the rebuilt area ---//

	int cx2 = x, cy2 = y;

	if( dir == EXIT_SOUTH )			{ cx2 = x+2; cy2 = y+1; }
	else if( dir == EXIT_NORTH )	{ cx2 = x+2; cy2 = y+1; }
	else								{ cx2 = x+1; cy2 = y+4; }

	int cx1 = dir == EXIT_EAST ? x-1 : x;
	int cy1 = y-1;

	if( cx1 < rebuild_x1 || cy1 < rebuild_y1 || cx2 > rebuild_x2 || cy2 > rebuild_y2 )
		return 0;

	#define CELL(xx,yy)	post_cell GRID(xx,yy)

	HillBlockInfo *h1, *h2, *h3;
	char h1p, h2p, h3p;

	switch( dir )
	{
		case EXIT_SOUTH:
		case EXIT_NORTH:
		{
			int p1, p2, p3, p4;

			if( dir == EXIT_SOUTH )	{ p1=SOUTH_PATTERN1; p2=SOUTH_PATTERN2; p3=SOUTH_PATTERN3; p4=SOUTH_PATTERN4; }
			else							{ p1=NORTH_PATTERN1; p2=NORTH_PATTERN2; p3=NORTH_PATTERN3; p4=NORTH_PATTERN4; }

			#define IS_PATTERN(h) (h==p1 || h==p2 || h==p3 || h==p4)

			if( !cell_has_hill(CELL(x,y)) || (h1=hill_res[CELL(x,y).hill1])->priority != HIGH_HILL_PRIORITY ||
				 h1->special_flag || !(h1p=h1->pattern_id) || !IS_PATTERN(h1p) )
				return 0;

			if( !cell_has_hill(CELL(x+1,y)) || (h2=hill_res[CELL(x+1,y).hill1])->priority != HIGH_HILL_PRIORITY ||
				 !(h2p=h2->pattern_id) || !IS_PATTERN(h2p) )
				return 0;

			if( !cell_has_hill(CELL(x+2,y)) || (h3=hill_res[CELL(x+2,y).hill1])->priority != HIGH_HILL_PRIORITY ||
				 !(h3p=h3->pattern_id) || !IS_PATTERN(h3p) )
				return 0;

			#undef IS_PATTERN

			if( dir == EXIT_SOUTH && !cell_walkable(CELL(x+1, y-1)) )
				return 0;

			if( dir == EXIT_NORTH && !cell_walkable(CELL(x+1, y+1)) )
				return 0;

			if( checkOnly )
				return 1;

			char leftSpecial  = dir==EXIT_SOUTH ? 'B' : 'D';
			char rightSpecial = dir==EXIT_SOUTH ? 'C' : 'E';
			char centreSpecial = dir==EXIT_SOUTH ? 'A' : 'F';
			int  terrainId, hillId;

			//--- the first square ---//

			if( dir == EXIT_SOUTH )
			{
				if( h1p == p3 )  h1p = p1;
				else if( h1p == p4 )  h1p = p2;
			}
			else
			{
				if( h1p == p4 )  h1p = p1;
				else if( h1p == p3 )  h1p = p2;
			}

			hillId = hill_res.scan(h1p, HIGH_HILL_PRIORITY, leftSpecial, 0);
			cell_remove_hill(CELL(x,y));
			cell_set_hill(CELL(x,y), hillId);

			if( (terrainId = terrain_res.scan(terrainType-1, terrainType-1, 0, 0, 1, 0)) != 0 )
				CELL(x,y).terrain_id = terrainId;

			if( dir == EXIT_SOUTH )		// the next row
			{
				HillCell& c = CELL(x, y+1);
				int lowId = hill_res.locate(h1p, hill_res[hillId]->sub_pattern_id, LOW_HILL_PRIORITY, leftSpecial);

				if( !c.hill2 )
					cell_remove_hill(c);

				cell_set_hill(c, lowId);

				if( (terrainId = terrain_res.scan(terrainType-1, terrainType-1, 0, 0, 1, 0)) != 0 )
					c.terrain_id = terrainId;
			}

			//--- the second square: walkable ---//

			cell_remove_hill(CELL(x+1,y));

			if( dir == EXIT_SOUTH )
			{
				if( (terrainId = terrain_res.scan(terrainType, BOTTOM_MASK, terrainType, BOTTOM_MASK,
						terrainType, BOTTOM_MASK, terrainType, BOTTOM_MASK)) != 0 )
					CELL(x+1,y).terrain_id = terrainId;

				cell_remove_hill(CELL(x+1,y+1));

				if( (terrainId = terrain_res.scan(terrainType, terrainType-1, SOUTH_PATTERN2, 0, 1, centreSpecial)) != 0 )
					CELL(x+1,y+1).terrain_id = terrainId;
			}
			else
			{
				if( (terrainId = terrain_res.scan(terrainType, terrainType-1, NORTH_PATTERN2, 0, 1, centreSpecial)) != 0 )
					CELL(x+1,y).terrain_id = terrainId;
			}

			//--- the third square ---//

			if( dir == EXIT_SOUTH )
			{
				if( h3p == p4 )  h3p = p1;
				if( h3p == p3 )  h3p = p2;
			}
			else
			{
				if( h3p == p3 )  h3p = p1;
				if( h3p == p4 )  h3p = p2;
			}

			hillId = hill_res.scan(h3p, HIGH_HILL_PRIORITY, rightSpecial, 0);
			cell_remove_hill(CELL(x+2,y));
			cell_set_hill(CELL(x+2,y), hillId);

			if( (terrainId = terrain_res.scan(terrainType-1, terrainType-1, 0, 0, 1, 0)) != 0 )
				CELL(x+2,y).terrain_id = terrainId;

			if( dir == EXIT_SOUTH )
			{
				HillCell& c = CELL(x+2, y+1);
				int lowId = hill_res.locate(h3p, hill_res[hillId]->sub_pattern_id, LOW_HILL_PRIORITY, rightSpecial);

				if( !c.hill2 )
					cell_remove_hill(c);

				cell_set_hill(c, lowId);

				if( (terrainId = terrain_res.scan(terrainType-1, terrainType-1, 0, 0, 1, 0)) != 0 )
					c.terrain_id = terrainId;
			}

			return 1;
		}

		default:		// EXIT_WEST, EXIT_EAST: four blocks in a column
		{
			int p1, p2, p3, p4;

			if( dir == EXIT_WEST )	{ p1=WEST_PATTERN1; p2=WEST_PATTERN2; p3=WEST_PATTERN3; p4=WEST_PATTERN4; }
			else							{ p1=EAST_PATTERN1; p2=EAST_PATTERN2; p3=EAST_PATTERN3; p4=EAST_PATTERN4; }

			#define IS_PATTERN(h) (h==p1 || h==p2 || h==p3 || h==p4)

			if( !cell_has_hill(CELL(x,y)) || (h1=hill_res[CELL(x,y).hill1])->priority != HIGH_HILL_PRIORITY ||
				 h1->special_flag || !(h1p=h1->pattern_id) || !IS_PATTERN(h1p) )
				return 0;

			if( !cell_has_hill(CELL(x,y+1)) )
				return 0;

			if( !cell_has_hill(CELL(x,y+2)) || (h2=hill_res[CELL(x,y+2).hill1])->priority != HIGH_HILL_PRIORITY ||
				 !(h2p=h2->pattern_id) || !IS_PATTERN(h2p) )
				return 0;

			if( !cell_has_hill(CELL(x,y+3)) || (h3=hill_res[CELL(x,y+3).hill1])->priority != HIGH_HILL_PRIORITY ||
				 !(h3p=h3->pattern_id) || !(h3p == p1 || h3p == p4) )
				return 0;

			#undef IS_PATTERN

			int sideX = dir==EXIT_WEST ? x+1 : x-1;

			if( !cell_walkable(CELL(sideX, y+2)) )
				return 0;

			if( checkOnly )
				return 1;

			char topSpecial    = dir==EXIT_WEST ? 'G' : 'J';
			char bottomSpecial = dir==EXIT_WEST ? 'I' : 'L';
			char centreSpecial = dir==EXIT_WEST ? 'H' : 'K';
			int  terrainId, hillId, hill2;

			//--- the first row ---//

			if( h1p == p3 )  h1p = p1;
			else if( h1p == p4 )  h1p = p2;

			hillId = hill_res.scan(h1p, HIGH_HILL_PRIORITY, topSpecial, 0);
			hill2 = CELL(x,y).hill2;
			cell_remove_hill(CELL(x,y));
			cell_set_hill(CELL(x,y), hillId);
			cell_set_hill(CELL(x,y), hill2);

			//--- the next row ---//

			{
				HillCell& c = CELL(x, y+1);
				cell_remove_hill(c);
				cell_set_hill(c, hill_res.locate(h1p, hill_res[hillId]->sub_pattern_id, LOW_HILL_PRIORITY, topSpecial));

				if( (terrainId = terrain_res.scan(terrainType-1, terrainType-1, 0, 0, 1, 0)) != 0 )
					c.terrain_id = terrainId;
			}

			//--- the third row: walkable ---//

			cell_remove_hill(CELL(x,y+2));

			if( (terrainId = terrain_res.scan(terrainType, terrainType-1, p2, 0, 1, centreSpecial)) != 0 )
				CELL(x,y+2).terrain_id = terrainId;

			//--- the fourth row ---//

			if( h3p == p4 )  h3p = p1;
			if( h3p == p3 )  h3p = p2;

			hillId = hill_res.scan(h3p, HIGH_HILL_PRIORITY, bottomSpecial, 0);
			cell_remove_hill(CELL(x,y+3));
			cell_set_hill(CELL(x,y+3), hillId);

			if( (terrainId = terrain_res.scan(terrainType-1, terrainType-1, 0, 0, 1, 0)) != 0 )
				CELL(x,y+3).terrain_id = terrainId;

			//--- the next row ---//

			{
				HillCell& c = CELL(x, y+4);
				cell_set_hill(c, hill_res.locate(h3p, hill_res[hillId]->sub_pattern_id, LOW_HILL_PRIORITY, bottomSpecial));

				if( (terrainId = terrain_res.scan(terrainType-1, terrainType-1, 0, 0, 1, 0)) != 0 )
					c.terrain_id = terrainId;
			}

			return 1;
		}
	}

	#undef CELL
}
//----------- End of static function try_exit -----------//


//-------- Begin of static function rebuild_plateau --------//
//
// Add (Hill) or remove (Clear) the plateau on the brush, or change a ramp,
// and rebuild the hills around.
//
// <int> bx1..by2 - the brush (the locations where the top of the plateau will be or won't be)
// <int> addFlag  - 1: add the plateau, 0: remove it, -1: only rebuild (ramps)
// <int> rampX, rampY - a ramp: >= 0: make a ramp at the location, or remove the one there
//
// return: <int> 1 - the map has changed
//
static int rebuild_plateau(int bx1, int by1, int bx2, int by2, int addFlag, int rampX, int rampY)
{
	//------- the area rebuilt, and the grid around it -------//

	rebuild_x1 = bx1 - HILL_MARGIN;
	rebuild_y1 = by1 - HILL_MARGIN;
	rebuild_x2 = bx2 + HILL_MARGIN;
	rebuild_y2 = by2 + HILL_MARGIN;

	if( rebuild_x1 < 5 || rebuild_y1 < 5 || rebuild_x2 > MAX_WORLD_X_LOC-6 || rebuild_y2 > MAX_WORLD_Y_LOC-6 )
	{
		box.msg( _("Hills can't be made so close to the edge of the map.") );
		return 0;
	}

	grid_x0 = rebuild_x1 - 3;
	grid_y0 = rebuild_y1 - 3;

	int gridX2 = rebuild_x2 + 3;		// the last tile of the grid used
	int gridY2 = rebuild_y2 + 3;

	err_when( gridX2 - grid_x0 >= HILL_GRID || gridY2 - grid_y0 >= HILL_GRID );

	mark_top_map();

	//------- the plateau before gen_hills(): the tile above a top -------//

	int x, y;

	for( y=grid_y0 ; y<=gridY2 ; y++ )
	{
		for( x=grid_x0 ; x<=gridX2 ; x++ )
		{
			int isPre = top_map[(y-1)*MAX_WORLD_X_LOC+x];

			pre_plateau GRID(x,y) = isPre;
			was_plateau GRID(x,y) = isPre || top_map[y*MAX_WORLD_X_LOC+x] || world.get_loc(x,y)->has_hill();
		}
	}

	//------- the brush: the top of the plateau is one row above the high ground -------//

	if( addFlag >= 0 )
	{
		for( y=by1 ; y<=by2 ; y++ )
			for( x=bx1 ; x<=bx2 ; x++ )
				pre_plateau GRID(x,y+1) = addFlag;

		//--- strips of high ground less than 3 wide in the area become low ground ---//
		//
		// They would be lumps of hill with no top (what Clear leaves at the side
		// of a plateau). A tile stays high if it is part of a 3x3 square of high tiles.

		static char keepHigh[HILL_GRID][HILL_GRID];

		for( y=rebuild_y1 ; y<=rebuild_y2+1 ; y++ )
		{
			for( x=rebuild_x1 ; x<=rebuild_x2 ; x++ )
			{
				keepHigh GRID(x,y) = 0;

				if( !pre_plateau GRID(x,y) )
					continue;

				for( int sy=y-2 ; sy<=y && !keepHigh GRID(x,y) ; sy++ )
				{
					for( int sx=x-2 ; sx<=x && !keepHigh GRID(x,y) ; sx++ )
					{
						int all = 1;
						for( int yy=sy ; yy<sy+3 && all ; yy++ )
							for( int xx=sx ; xx<sx+3 ; xx++ )
								if( !pre_plateau GRID(xx,yy) ) { all = 0; break; }
						keepHigh GRID(x,y) = all;
					}
				}
			}
		}

		for( y=rebuild_y1 ; y<=rebuild_y2+1 ; y++ )
			for( x=rebuild_x1 ; x<=rebuild_x2 ; x++ )
				pre_plateau GRID(x,y) = keepHigh GRID(x,y);
	}

	//------- the levels of the corners of the pre tiles -------//
	//
	// Corners between 4 high tiles: dirt; corners touching high tiles: light
	// grass (the ring of the hill); other corners of tiles that were part of a
	// plateau: light grass; the rest keep the level they have. All of them
	// except the protected ones (buildings, towns, the sea...) may follow
	// their neighbours, so that no two corners next to each other are more
	// than MAX_LEVEL_STEP apart.

	int cwx = gridX2 - grid_x0;		// corners 1..cwx-1 between the grid tiles
	int cwy = gridY2 - grid_y0;

	static char cornerLevel[HILL_GRID][HILL_GRID], cornerOrigLevel[HILL_GRID][HILL_GRID], cornerFixed[HILL_GRID][HILL_GRID];

	const int dirtLevel  = (TERRAIN_DARK_DIRT-1) * LEVEL_PER_TYPE + 1;
	const int lightLevel = (TERRAIN_LIGHT_GRASS-1) * LEVEL_PER_TYPE + 1;

	int i, j;

	for( j=1 ; j<=cwy ; j++ )		// corner (i,j) is the north-west corner of the grid tile (i,j)
	{
		for( i=1 ; i<=cwx ; i++ )
		{
			int highCount = pre_plateau[j-1][i-1] + pre_plateau[j-1][i] + pre_plateau[j][i-1] + pre_plateau[j][i];
			int nearPlateau = was_plateau[j-1][i-1] || was_plateau[j-1][i] || was_plateau[j][i-1] || was_plateau[j][i];

			int isProtected;

			paint_mode = PAINT_PLATEAU;
			int curLevel = get_corner_level(grid_x0+i, grid_y0+j, isProtected);
			paint_mode = PAINT_GROUND;

			cornerOrigLevel[j][i] = curLevel;

			if( highCount == 4 )
			{
				cornerLevel[j][i] = dirtLevel;
				cornerFixed[j][i] = 1;
			}
			else if( highCount > 0 )
			{
				cornerLevel[j][i] = lightLevel;
				cornerFixed[j][i] = 1;
			}
			else if( isProtected )
			{
				cornerLevel[j][i] = curLevel;
				cornerFixed[j][i] = 2;
			}
			else
			{
				cornerLevel[j][i] = nearPlateau ? lightLevel : curLevel;
				cornerFixed[j][i] = 0;
			}

			//--- the edge of the grid doesn't change ---//

			if( i==1 || j==1 || i==cwx || j==cwy )
			{
				if( highCount == 0 )
					cornerLevel[j][i] = curLevel;

				cornerFixed[j][i] = 2;
			}
		}
	}

	//------- the free corners follow their neighbours (within MAX_LEVEL_STEP) -------//

	for( int pass=0 ; pass<50 ; pass++ )
	{
		int changed=0;

		for( j=1 ; j<=cwy ; j++ )
		{
			for( i=1 ; i<=cwx ; i++ )
			{
				for( int dj=-1 ; dj<=1 ; dj++ )
				{
					for( int di=-1 ; di<=1 ; di++ )
					{
						int ni=i+di, nj=j+dj;

						if( (!di && !dj) || ni<1 || nj<1 || ni>cwx || nj>cwy )
							continue;

						int a = cornerLevel[j][i], b = cornerLevel[nj][ni];

						if( abs(a-b) <= MAX_LEVEL_STEP )
							continue;

						int fixA = cornerFixed[j][i], fixB = cornerFixed[nj][ni];

						if( fixA == fixB && fixA == 0 )		// between two free corners, the unchanged one follows
						{
							fixA = cornerLevel[j][i] != cornerOrigLevel[j][i];
							fixB = cornerLevel[nj][ni] != cornerOrigLevel[nj][ni];
						}

						if( fixA == fixB )
							continue;

						if( fixA > fixB )
							cornerLevel[nj][ni] = a > b ? a-MAX_LEVEL_STEP : a+MAX_LEVEL_STEP;
						else
							cornerLevel[j][i] = b > a ? b-MAX_LEVEL_STEP : b+MAX_LEVEL_STEP;

						changed=1;
					}
				}
			}
		}

		if( !changed )
			break;
	}

	//------- the pre tiles (all the tiles of the rebuilt rows, plus one below) -------//

	for( y=rebuild_y1 ; y<=rebuild_y2+1 ; y++ )
	{
		for( x=rebuild_x1 ; x<=rebuild_x2 ; x++ )
		{
			int gi = x-grid_x0, gj = y-grid_y0;

			int nw = cornerLevel[gj][gi],   ne = cornerLevel[gj][gi+1];
			int sw = cornerLevel[gj+1][gi], se = cornerLevel[gj+1][gi+1];

			Location* locPtr = world.get_loc(x, y);

			int changed = nw != cornerOrigLevel[gj][gi] || ne != cornerOrigLevel[gj][gi+1] ||
							  sw != cornerOrigLevel[gj+1][gi] || se != cornerOrigLevel[gj+1][gi+1] ||
							  was_plateau GRID(x,y) || pre_plateau GRID(x,y);

			if( !changed )
			{
				pre_terrain GRID(x,y) = locPtr->terrain_id;
				continue;
			}

			int terrainId = terrain_res.scan( level_type(nw), level_subtype(nw), level_type(ne), level_subtype(ne),
														 level_type(sw), level_subtype(sw), level_type(se), level_subtype(se), 0, 1, 0 );
			if( !terrainId )
			{
				box.msg( _("The hill can't be made here (too close to water or to something that doesn't change).") );
				return 0;
			}

			pre_terrain GRID(x,y) = terrainId;
		}
	}

	//------- gen_hills(): the edges become hills, the top moves one row up -------//

	for( y=rebuild_y1-1 ; y<=rebuild_y2+1 ; y++ )
	{
		for( x=rebuild_x1 ; x<=rebuild_x2 ; x++ )
		{
			HillCell& c = post_cell GRID(x,y);

			c.terrain_id = (y >= rebuild_y1) ? pre_terrain GRID(x,y) : world.get_loc(x,y)->terrain_id;
			c.hill1 = c.hill2 = 0;
		}
	}

	for( y=rebuild_y1 ; y<=rebuild_y2+1 ; y++ )
	{
		for( x=rebuild_x1 ; x<=rebuild_x2 ; x++ )
		{
			HillCell& c = post_cell GRID(x,y);
			HillCell& above = post_cell GRID(x,y-1);
			TerrainInfo* terrainInfo = terrain_res[c.terrain_id];

			int priTerrain = terrainInfo->average_type;
			int secTerrain = terrainInfo->secondary_type;
			int highTerrain = MAX(priTerrain, secTerrain);
			int lowTerrain = MIN(priTerrain, secTerrain);

			if( highTerrain < TERRAIN_DARK_DIRT )
				continue;

			if( lowTerrain >= TERRAIN_DARK_DIRT )
			{
				above = c;		// the top: one row up
			}
			else
			{
				int patternId = terrainInfo->pattern_id;
				int hillId = hill_res.scan(patternId, LOW_HILL_PRIORITY, 0, 0);

				if( !hillId )
					continue;

				cell_set_hill(c, hillId);
				cell_set_hill(above, hill_res.locate(patternId, hill_res[hillId]->sub_pattern_id, HIGH_HILL_PRIORITY, 0));

				int pureId = terrain_res.scan(lowTerrain, lowTerrain, 0);

				if( pureId )
					c.terrain_id = pureId;
			}
		}
	}

	//------- exits: the ones there were, a ramp asked for, and new ones on new edges -------//

	static char leftSpecial[EXIT_DIR_COUNT] = { 'B', 'D', 'G', 'J' };

	no_exit_x1 = no_exit_y1 = no_exit_x2 = no_exit_y2 = -100;

	int rampMade = 0, rampRemoved = 0;

	if( rampX >= 0 )
	{
		//--- an exit at the location: it is removed (it isn't kept and no new one is made there) ---//

		for( y=rampY-2 ; y<=rampY+2 && !rampRemoved ; y++ )
		{
			for( x=rampX-2 ; x<=rampX+2 ; x++ )
			{
				Location* locPtr = world.get_loc(x, y);

				if( is_exit_ground(locPtr) ||
					 (locPtr->has_hill() && hill_res[locPtr->hill_id1()]->special_flag >= 'A' && hill_res[locPtr->hill_id1()]->special_flag <= 'L') )
				{
					rampRemoved = 1;
					break;
				}
			}
		}

		if( rampRemoved )
		{
			no_exit_x1 = rampX-4;  no_exit_y1 = rampY-4;
			no_exit_x2 = rampX+4;  no_exit_y2 = rampY+4;
		}
		else
		{
			//--- a new exit at the location or next to it ---//

			for( int dist=0 ; dist<=2 && !rampMade ; dist++ )
			{
				for( y=rampY-dist ; y<=rampY+dist && !rampMade ; y++ )
				{
					for( x=rampX-dist ; x<=rampX+dist && !rampMade ; x++ )
					{
						for( int dir=0 ; dir<EXIT_DIR_COUNT ; dir++ )
						{
							int ex = x, ey = y;

							if( dir == EXIT_SOUTH || dir == EXIT_NORTH )
								ex--;			// the click is on the middle of the exit
							else
								ey -= 2;

							if( try_exit(dir, ex, ey, 0) )
							{
								rampMade = 1;
								break;
							}
						}
					}
				}
			}

			if( !rampMade )
			{
				box.msg( _("A ramp needs a straight part of the edge of a hill, 3 blocks long.") );
				return 0;
			}
		}
	}

	//--- the exits there were in the area (if they still fit) ---//

	for( y=rebuild_y1 ; y<=rebuild_y2 ; y++ )
	{
		for( x=rebuild_x1 ; x<=rebuild_x2 ; x++ )
		{
			Location* locPtr = world.get_loc(x, y);

			if( !locPtr->has_hill() || (x >= no_exit_x1 && x <= no_exit_x2 && y >= no_exit_y1 && y <= no_exit_y2) )
				continue;

			int special = hill_res[locPtr->hill_id1()]->special_flag;

			for( int dir=0 ; dir<EXIT_DIR_COUNT ; dir++ )
			{
				if( special == leftSpecial[dir] && !exit_near(dir, x, y) )
					try_exit(dir, x, y, 0);
			}
		}
	}

	//--- new exits on the new edges, as gen_hills(), away from other exits ---//

	if( addFlag >= 0 )
	{
		auto_exit_x1 = bx1-3;  auto_exit_y1 = by1-3;
		auto_exit_x2 = bx2+3;  auto_exit_y2 = by2+4;

		for( y=auto_exit_y1 ; y<=auto_exit_y2 ; y++ )
		{
			for( x=auto_exit_x1 ; x<=auto_exit_x2 ; x++ )
			{
				if( x >= no_exit_x1 && x <= no_exit_x2 && y >= no_exit_y1 && y <= no_exit_y2 )
					continue;

				for( int dir=0 ; dir<EXIT_DIR_COUNT ; dir++ )
				{
					if( !exit_near(dir, x, y) && try_exit(dir, x, y, 1) )
						try_exit(dir, x, y, 0);
				}
			}
		}
	}

	//------- check: hills only where there is nothing (units, buildings, towns, resources, walls, rocks) -------//

	for( y=rebuild_y1 ; y<=rebuild_y2 ; y++ )
	{
		for( x=rebuild_x1 ; x<=rebuild_x2 ; x++ )
		{
			HillCell& c = post_cell GRID(x,y);
			Location* locPtr = world.get_loc(x, y);

			if( !c.hill1 || locPtr->has_hill() )
				continue;

			if( locPtr->is_firm() || locPtr->is_town() || locPtr->has_site() || locPtr->is_wall() || locPtr->is_rock() ||
				 (locPtr->cargo_recno && !(locPtr->loc_flag & LOCATE_BLOCK_MASK)) ||
				 terrain_res[locPtr->terrain_id]->average_type == TERRAIN_OCEAN )
			{
				box.msg( _("The hill can't be made here: there are units, buildings, natural resources or water in its way.") );
				return 0;
			}
		}
	}

	//------- the regions with the new hills (before changing the map: too many, nothing changes) -------//

	region_override = 1;
	int regionCount = count_regions();
	region_override = 0;

	if( regionCount < 0 )
	{
		box.msg( _("The map would have too many separate areas of land and water.") );
		return 0;
	}

	//------- set the new tiles and hills -------//

	int changeCount=0;

	for( y=rebuild_y1 ; y<=rebuild_y2 ; y++ )
	{
		for( x=rebuild_x1 ; x<=rebuild_x2 ; x++ )
		{
			HillCell& c = post_cell GRID(x,y);
			Location* locPtr = world.get_loc(x, y);

			int sameHill = locPtr->has_hill() ? (locPtr->hill_id1()==c.hill1 && locPtr->hill_id2()==c.hill2) : !c.hill1;

			if( locPtr->terrain_id == c.terrain_id && sameHill )
				continue;

			changeCount++;

			if( c.hill1 )
			{
				if( locPtr->is_plant() )
				{
					locPtr->remove_plant();
					world.plant_count--;
				}

				if( locPtr->has_dirt() )
					remove_rock( locPtr->dirt_recno(), dirt_array, 1 );
			}

			if( locPtr->has_hill() )
				locPtr->remove_hill();

			locPtr->terrain_id = c.terrain_id;

			if( c.hill1 )
			{
				locPtr->set_hill(c.hill1);

				if( c.hill2 )
					locPtr->set_hill(c.hill2);
			}
			else if( !(locPtr->loc_flag & LOCATE_BLOCK_MASK) )
			{
				locPtr->walkable_off();
				locPtr->walkable_reset();
			}

			//--- a tree that doesn't grow on the new ground is removed ---//

			if( locPtr->is_plant() )
			{
				int teraType = terrain_res[locPtr->terrain_id]->average_type;
				int plantId = plant_res.plant_recno(locPtr->plant_id());
				PlantInfo* plantInfo = plantId ? plant_res[plantId] : NULL;

				if( !plantInfo || (plantInfo->tera_type[0] != teraType &&
					 plantInfo->tera_type[1] != teraType && plantInfo->tera_type[2] != teraType) )
				{
					locPtr->remove_plant();
					world.plant_count--;
					locPtr->walkable_off();
					locPtr->walkable_reset();
				}
			}

			set_fire_src(locPtr);
		}
	}

	if( !changeCount && !rampMade && !rampRemoved )
		return 0;

	//------- no building on and next to hills (as gen_hills()) -------//

	for( y=rebuild_y1 ; y<=rebuild_y2 ; y++ )
	{
		for( x=rebuild_x1 ; x<=rebuild_x2 ; x++ )
		{
			Location* locPtr = world.get_loc(x, y);

			if( locPtr->has_hill() )
			{
				locPtr->set_power_off();
				world.set_surr_power_off(x, y);
			}
		}
	}

	reset_power_off(rebuild_x1-1, rebuild_y1-1, rebuild_x2+1, rebuild_y2+1);

	//------- the regions (calculated above) -------//

	apply_regions(regionCount);

	return 1;
}
//----------- End of static function rebuild_plateau -----------//


//-------- Begin of static function region_type_at --------//
//
static RegionType region_type_at(int x, int y)
{
	if( region_override && x >= rebuild_x1 && x <= rebuild_x2 && y >= rebuild_y1 && y <= rebuild_y2 )
	{
		HillCell& c = post_cell GRID(x,y);

		if( c.hill1 )
			return REGION_INPASSABLE;

		return terrain_res[c.terrain_id]->average_type == TERRAIN_OCEAN ? REGION_SEA : REGION_LAND;
	}

	return base_region_type( world.get_loc(x,y) );
}
//----------- End of static function region_type_at -----------//


//-------- Begin of static function brush_has_plateau --------//
//
// Whether there are hills or the top of a plateau on the brush (or just
// below it: the plateau of a top is one row below).
//
static int brush_has_plateau(int x1, int y1, int x2, int y2)
{
	mark_top_map();

	for( int y=y1 ; y<=MIN(y2+1, MAX_WORLD_Y_LOC-1) ; y++ )
	{
		for( int x=x1 ; x<=x2 ; x++ )
		{
			if( world.get_loc(x, y)->has_hill() || top_map[y*MAX_WORLD_X_LOC+x] )
				return 1;
		}
	}

	return 0;
}
//----------- End of static function brush_has_plateau -----------//


//-------- Begin of static function tile_is_protected --------//
//
// Whether the tile of a location must not change: tiles with sea (coasts),
// hills, buildings, towns, natural resources or special tiles.
//
static int tile_is_protected(int xLoc, int yLoc)
{
	if( xLoc < 0 || yLoc < 0 || xLoc >= MAX_WORLD_X_LOC || yLoc >= MAX_WORLD_Y_LOC )
		return 0;

	Location* locPtr = world.get_loc(xLoc, yLoc);
	TerrainInfo* terrainInfo = terrain_res[locPtr->terrain_id];

	if( terrainInfo->special_flag )
		return 1;

	if( paint_mode != PAINT_SHAPE &&		// water and land change the sea and the coasts
		 (terrainInfo->nw_type == TERRAIN_OCEAN || terrainInfo->ne_type == TERRAIN_OCEAN ||
		  terrainInfo->sw_type == TERRAIN_OCEAN || terrainInfo->se_type == TERRAIN_OCEAN) )
	{
		return 1;
	}

	if( (locPtr->has_hill() && paint_mode != PAINT_PLATEAU) || locPtr->is_firm() || locPtr->is_town() || locPtr->has_site() )
		return 1;

	if( paint_mode == PAINT_SHAPE )
	{
		//--- not where units are (land units would be in the sea, ships on land), walls and rocks ---//

		if( locPtr->is_wall() || locPtr->is_rock() )
			return 1;

		if( locPtr->cargo_recno && !(locPtr->loc_flag & LOCATE_BLOCK_MASK) )
			return 1;
	}

	return 0;
}
//----------- End of static function tile_is_protected -----------//


//-------- Begin of static function get_corner_level --------//
//
// Rebuild the level of a corner from the tiles around it: the type they
// agree on and a subtype allowed by all of them (the middle one if possible).
//
// <int>  cornerX, cornerY - the corner, 0 to MAX_WORLD_X_LOC (the corner x is the north-west corner of the location x)
// <int&> isProtected      - for returning whether the corner must not change
//
// return: <int> the level
//
static int get_corner_level(int cornerX, int cornerY, int& isProtected)
{
	static int dxArray[4] = { -1, 0, -1, 0 };		// the tiles around the corner: NW, NE, SW, SE of the corner
	static int dyArray[4] = { -1, -1, 0, 0 };

	int type=0, mask=7;
	isProtected = 0;

	int cornerTypeArray[4], cornerMaskArray[4];

	for( int i=0 ; i<4 ; i++ )
	{
		cornerTypeArray[i] = 0;

		int x = cornerX + dxArray[i];
		int y = cornerY + dyArray[i];

		if( x < 0 || y < 0 || x >= MAX_WORLD_X_LOC || y >= MAX_WORLD_Y_LOC )
			continue;

		TerrainInfo* terrainInfo = terrain_res[world.get_loc(x, y)->terrain_id];

		switch( i )		// the corner of this tile that is our corner
		{
			case 0:  cornerTypeArray[i] = terrainInfo->se_type;  cornerMaskArray[i] = terrainInfo->se_subtype;  break;
			case 1:  cornerTypeArray[i] = terrainInfo->sw_type;  cornerMaskArray[i] = terrainInfo->sw_subtype;  break;
			case 2:  cornerTypeArray[i] = terrainInfo->ne_type;  cornerMaskArray[i] = terrainInfo->ne_subtype;  break;
			default: cornerTypeArray[i] = terrainInfo->nw_type;  cornerMaskArray[i] = terrainInfo->nw_subtype;  break;
		}

		if( tile_is_protected(x, y) )
			isProtected = 1;
	}

	//--- the tiles don't agree (pattern substitutions): don't change it ---//

	for( int i=0 ; i<4 ; i++ )
	{
		if( !cornerTypeArray[i] )
			continue;

		if( !type )
			type = cornerTypeArray[i];
		else if( type != cornerTypeArray[i] )
		{
			isProtected = 1;
		}
	}

	for( int i=0 ; i<4 ; i++ )
	{
		if( cornerTypeArray[i] == type )
			mask &= cornerMaskArray[i];
	}

	if( !type )
		type = TERRAIN_DARK_GRASS;

	if( !mask )
	{
		isProtected = 1;
		mask = MIDDLE_MASK;
	}

	int sub;

	if( mask & MIDDLE_MASK )
		sub = 1;
	else if( mask & BOTTOM_MASK )
		sub = 0;
	else
		sub = 2;

	return (type-1) * LEVEL_PER_TYPE + sub;
}
//----------- End of static function get_corner_level -----------//


//-------- Begin of static function paint_ground --------//
//
// Paint the type of the ground on the locations x1..x2, y1..y2.
//
// return: <int> 1 - the ground has changed
//
static int paint_ground(int terrainType, int x1, int y1, int x2, int y2)
{
	int targetLevel = (terrainType-1) * LEVEL_PER_TYPE + 1;		// the middle subtype

	//------- the corners of the box: the brush and the margin -------//

	int boxX1 = x1 - BOX_MARGIN;					// corner coordinates
	int boxY1 = y1 - BOX_MARGIN;
	int boxW  = (x2-x1+2) + BOX_MARGIN*2;		// the brush has (x2-x1+2) corners across
	int boxH  = (y2-y1+2) + BOX_MARGIN*2;

	int i, j;

	for( j=0 ; j<boxH ; j++ )
	{
		for( i=0 ; i<boxW ; i++ )
		{
			int cx = boxX1+i, cy = boxY1+j;

			if( cx < 0 || cy < 0 || cx > MAX_WORLD_X_LOC || cy > MAX_WORLD_Y_LOC )
			{
				corner_level[j][i] = corner_orig_level[j][i] = LEVEL_PROTECTED;
				corner_fixed[j][i] = 2;
				continue;
			}

			int isProtected;
			corner_level[j][i] = corner_orig_level[j][i] = get_corner_level(cx, cy, isProtected);

			int inBrush = cx >= x1 && cx <= x2+1 && cy >= y1 && cy <= y2+1;

			if( isProtected || i==0 || j==0 || i==boxW-1 || j==boxH-1 )		// the edge of the box doesn't change
				corner_fixed[j][i] = 2;
			else if( inBrush )
			{
				corner_fixed[j][i] = 1;
				corner_level[j][i] = targetLevel;
			}
			else
				corner_fixed[j][i] = 0;
		}
	}

	//------- the corners around follow: neighbours within MAX_LEVEL_STEP -------//

	for( int pass=0 ; pass<50 ; pass++ )
	{
		int changed=0;

		for( j=0 ; j<boxH ; j++ )
		{
			for( i=0 ; i<boxW ; i++ )
			{
				if( corner_level[j][i] == LEVEL_PROTECTED )
					continue;

				for( int dj=-1 ; dj<=1 ; dj++ )
				{
					for( int di=-1 ; di<=1 ; di++ )
					{
						int ni=i+di, nj=j+dj;

						if( (!di && !dj) || ni<0 || nj<0 || ni>=boxW || nj>=boxH )
							continue;

						if( corner_level[nj][ni] == LEVEL_PROTECTED )
							continue;

						int a = corner_level[j][i], b = corner_level[nj][ni];

						if( abs(a-b) <= MAX_LEVEL_STEP )
							continue;

						//--- the less fixed one follows; between two free corners, the unchanged one follows ---//

						int fixA = corner_fixed[j][i], fixB = corner_fixed[nj][ni];

						if( fixA == fixB && fixA == 0 )
						{
							fixA = corner_level[j][i] != corner_orig_level[j][i];
							fixB = corner_level[nj][ni] != corner_orig_level[nj][ni];
						}

						if( fixA == fixB )		// both protected (as it was), or both unchanged
							continue;

						if( fixA > fixB )
							corner_level[nj][ni] = a > b ? a-MAX_LEVEL_STEP : a+MAX_LEVEL_STEP;
						else
							corner_level[j][i] = b > a ? b-MAX_LEVEL_STEP : b+MAX_LEVEL_STEP;

						changed=1;
					}
				}
			}
		}

		if( !changed )
			break;
	}

	//------- the new tiles: all of them must be found, or nothing changes -------//

	short newTerrainId[BOX_SIZE][BOX_SIZE];
	int   changeCount=0;

	memset( newTerrainId, 0, sizeof(newTerrainId) );

	for( j=0 ; j<boxH-1 ; j++ )
	{
		for( i=0 ; i<boxW-1 ; i++ )
		{
			int xLoc = boxX1+i, yLoc = boxY1+j;		// the tile with this corner as its north-west corner

			if( xLoc < 0 || yLoc < 0 || xLoc >= MAX_WORLD_X_LOC || yLoc >= MAX_WORLD_Y_LOC )
				continue;

			if( corner_level[j][i]     == corner_orig_level[j][i] &&
				 corner_level[j][i+1]   == corner_orig_level[j][i+1] &&
				 corner_level[j+1][i]   == corner_orig_level[j+1][i] &&
				 corner_level[j+1][i+1] == corner_orig_level[j+1][i+1] )
			{
				continue;		// unchanged
			}

			int nw = corner_level[j][i],   ne = corner_level[j][i+1];
			int sw = corner_level[j+1][i], se = corner_level[j+1][i+1];

			int terrainId = terrain_res.scan( level_type(nw), level_subtype(nw), level_type(ne), level_subtype(ne),
														 level_type(sw), level_subtype(sw), level_type(se), level_subtype(se), 0, 1, 0 );

			if( !terrainId )
			{
				return 0;		// no tile for these corners: don't paint
			}

			newTerrainId[j][i] = terrainId;
			changeCount++;
		}
	}

	if( !changeCount )
		return 0;

	//------- set the new tiles (and remember them, for 5c) -------//

	changed_count = 0;
	changed_x1 = MAX_WORLD_X_LOC;  changed_y1 = MAX_WORLD_Y_LOC;
	changed_x2 = -1;               changed_y2 = -1;

	for( j=0 ; j<boxH-1 ; j++ )
	{
		for( i=0 ; i<boxW-1 ; i++ )
		{
			if( !newTerrainId[j][i] )
				continue;

			Location* locPtr = world.get_loc(boxX1+i, boxY1+j);

			if( changed_count < MAX_CHANGED )
			{
				changed_x[changed_count] = boxX1+i;
				changed_y[changed_count] = boxY1+j;
				changed_old_id[changed_count] = locPtr->terrain_id;
				changed_count++;
			}

			changed_x1 = MIN(changed_x1, boxX1+i);  changed_x2 = MAX(changed_x2, boxX1+i);
			changed_y1 = MIN(changed_y1, boxY1+j);  changed_y2 = MAX(changed_y2, boxY1+j);

			locPtr->terrain_id = newTerrainId[j][i];

			//--- a tree that doesn't grow on the new ground is removed ---//

			if( locPtr->is_plant() )
			{
				int teraType = terrain_res[locPtr->terrain_id]->average_type;
				int plantId = plant_res.plant_recno(locPtr->plant_id());
				PlantInfo* plantInfo = plantId ? plant_res[plantId] : NULL;

				if( !plantInfo || (plantInfo->tera_type[0] != teraType &&
					 plantInfo->tera_type[1] != teraType && plantInfo->tera_type[2] != teraType) )
				{
					locPtr->remove_plant();
					world.plant_count--;
				}
			}
		}
	}

	return 1;
}
//----------- End of static function paint_ground -----------//
