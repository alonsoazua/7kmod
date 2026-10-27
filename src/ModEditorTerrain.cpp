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
		 PAINT_HILL,		// dirt whose edges become hills: land without units
		 PAINT_UNHILL };	// light grass where hills were removed (the corners that don't agree take the lowest type)

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
static int  clear_hills(int x1, int y1, int x2, int y2);


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
			return clear_hills(x1, y1, x2, y2) || rc;
		}

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

static short hill_x[MAX_CHANGED*2], hill_y[MAX_CHANGED*2];
static int   hill_count;

static uint8_t* region_buf;		// the regions being calculated, one per location
static int*     fill_stack;		// the stack of the flood fill (x, y pairs)


//-------- Begin of static function base_region_type --------//
//
// The type of a location for the regions, as when the map is generated:
// before the buildings, trees and rocks (only hills block).
//
static RegionType base_region_type(Location* locPtr)
{
	if( locPtr->has_hill() )
		return REGION_INPASSABLE;

	if( terrain_res[locPtr->terrain_id]->average_type == TERRAIN_OCEAN )
		return REGION_SEA;

	return REGION_LAND;
}
//----------- End of static function base_region_type -----------//


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

			RegionType regionType = base_region_type( world.get_loc(x,y) );

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

					if( region_buf[ny*MAX_WORLD_X_LOC+nx] || base_region_type(world.get_loc(nx,ny)) != regionType )
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
		}
	}

	reset_power_off(x1, y1, x2, y2);		// the land that is no longer next to the sea
}
//----------- End of static function update_location_flags -----------//


//-------- Begin of static function undo_shape --------//
//
// Restore the tiles changed and remove the hills added, when the map would
// have too many regions.
//
static void undo_shape()
{
	for( int i=0 ; i<hill_count ; i++ )
	{
		Location* locPtr = world.get_loc(hill_x[i], hill_y[i]);

		if( locPtr->has_hill() )
		{
			locPtr->remove_hill();
			locPtr->walkable_reset();
		}
	}

	for( int i=0 ; i<changed_count ; i++ )
		world.get_loc(changed_x[i], changed_y[i])->terrain_id = changed_old_id[i];
}
//----------- End of static function undo_shape -----------//


//-------- Begin of static function add_hills --------//
//
// Turn the edges of the dirt just painted into hills, as World::gen_hills():
// a tile between dirt and lower ground gets the low block of the hill of its
// pattern, the tile above it the high block, and its ground becomes the
// lower one. Without exits: the plateau inside can't be reached by land.
//
static void add_hills()
{
	hill_count = 0;

	//--- from north to south, as gen_hills() (the high block goes on the tile above) ---//

	for( int i=0 ; i<changed_count ; i++ )
	{
		for( int j=i+1 ; j<changed_count ; j++ )
		{
			if( changed_y[j] < changed_y[i] || (changed_y[j] == changed_y[i] && changed_x[j] < changed_x[i]) )
			{
				short t;
				t = changed_x[i];  changed_x[i] = changed_x[j];  changed_x[j] = t;
				t = changed_y[i];  changed_y[i] = changed_y[j];  changed_y[j] = t;
				t = changed_old_id[i];  changed_old_id[i] = changed_old_id[j];  changed_old_id[j] = t;
			}
		}
	}

	for( int i=0 ; i<changed_count ; i++ )
	{
		int x = changed_x[i], y = changed_y[i];
		Location* locPtr = world.get_loc(x, y);
		TerrainInfo* terrainInfo = terrain_res[locPtr->terrain_id];

		int priTerrain = terrainInfo->average_type;
		int secTerrain = terrainInfo->secondary_type;
		int highTerrain = MAX(priTerrain, secTerrain);
		int lowTerrain = MIN(priTerrain, secTerrain);

		if( highTerrain < TERRAIN_DARK_DIRT || lowTerrain >= TERRAIN_DARK_DIRT )
			continue;		// not an edge of the dirt

		int patternId = terrainInfo->pattern_id;
		int hillId = hill_res.scan(patternId, LOW_HILL_PRIORITY, 0, 0);

		if( !hillId )
			continue;

		if( locPtr->is_plant() )
		{
			locPtr->remove_plant();
			world.plant_count--;
		}

		if( locPtr->has_dirt() )		// a dirt patch would leave a hole in the hill
			remove_rock( locPtr->dirt_recno(), dirt_array, 1 );

		if( !locPtr->can_add_hill() )
			continue;

		int newTerrainId = terrain_res.scan(lowTerrain, lowTerrain, 0);

		if( !newTerrainId )
			continue;

		locPtr->set_hill(hillId);
		locPtr->set_fire_src(-100);
		locPtr->set_power_off();
		world.set_surr_power_off(x, y);
		locPtr->terrain_id = newTerrainId;

		hill_x[hill_count] = x;  hill_y[hill_count] = y;  hill_count++;

		//--- the high block on the tile above ---//

		if( y > 0 )
		{
			Location* aboveLoc = world.get_loc(x, y-1);

			if( aboveLoc->is_plant() )
			{
				aboveLoc->remove_plant();
				world.plant_count--;
			}

			if( aboveLoc->has_dirt() )
				remove_rock( aboveLoc->dirt_recno(), dirt_array, 1 );

			int highHillId = hill_res.locate(patternId, hill_res[hillId]->sub_pattern_id, HIGH_HILL_PRIORITY, 0);

			if( highHillId && aboveLoc->can_add_hill() && !tile_is_protected(x, y-1) )
			{
				aboveLoc->set_hill(highHillId);
				aboveLoc->set_fire_src(-100);
				aboveLoc->set_power_off();
				world.set_surr_power_off(x, y-1);

				hill_x[hill_count] = x;  hill_y[hill_count] = y-1;  hill_count++;
			}
		}
	}
}
//----------- End of static function add_hills -----------//


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

		default:		// EDITOR_TERRAIN_HILL: dirt on land, its edges become hills
			paint_mode = PAINT_HILL;
			rc = paint_ground(TERRAIN_DARK_DIRT, x1, y1, x2, y2);
			break;
	}

	paint_mode = PAINT_GROUND;

	if( !rc )
		return 0;

	hill_count = 0;

	if( shapeType == EDITOR_TERRAIN_HILL )
		add_hills();
	else
		update_location_flags(changed_x1-1, changed_y1-1, changed_x2+1, changed_y2+1);

	//------- the regions; too many: undo -------//

	int regionCount = count_regions();

	if( regionCount < 0 )
	{
		undo_shape();

		if( shapeType != EDITOR_TERRAIN_HILL )
			update_location_flags(changed_x1-1, changed_y1-1, changed_x2+1, changed_y2+1);

		box.msg( _("The map would have too many separate areas of land and water.") );
		return 0;
	}

	apply_regions(regionCount);

	//------- where harbors can be built, around the tiles changed (as World::set_harbor_bit()) -------//

	if( shapeType != EDITOR_TERRAIN_HILL )
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


//-------- Begin of static function clear_hills --------//
//
// Remove the hill blocks in the area (the Clear entry).
//
// return: <int> 1 - a hill was removed
//
static int clear_hills(int x1, int y1, int x2, int y2)
{
	int count=0;

	for( int y=y1 ; y<=y2 ; y++ )
	{
		for( int x=x1 ; x<=x2 ; x++ )
		{
			Location* locPtr = world.get_loc(x, y);

			if( !locPtr->has_hill() )
				continue;

			locPtr->remove_hill();
			locPtr->set_fire_src(0);
			locPtr->walkable_reset();
			count++;
		}
	}

	if( !count )
		return 0;

	//--- the ground under the hills doesn't match the plateau: light grass joins them ---//

	paint_mode = PAINT_UNHILL;
	paint_ground(TERRAIN_LIGHT_GRASS, x1, y1, x2, y2);
	paint_mode = PAINT_GROUND;

	reset_power_off(x1-1, y1-1, x2+1, y2+1);

	int regionCount = count_regions();

	if( regionCount > 0 )
		apply_regions(regionCount);

	return 1;
}
//----------- End of static function clear_hills -----------//


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

	if( locPtr->has_hill() || locPtr->is_firm() || locPtr->is_town() || locPtr->has_site() )
		return 1;

	if( paint_mode != PAINT_GROUND )
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

	//--- the tiles don't agree (pattern substitutions): don't change it; after removing ---//
	//--- hills (the ground under them is the lower one), the lowest type is taken ---//

	for( int i=0 ; i<4 ; i++ )
	{
		if( !cornerTypeArray[i] )
			continue;

		if( !type )
			type = cornerTypeArray[i];
		else if( type != cornerTypeArray[i] )
		{
			if( paint_mode == PAINT_UNHILL )
				type = MIN(type, cornerTypeArray[i]);
			else
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
