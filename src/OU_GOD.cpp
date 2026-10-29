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

//Filename   : OU_GOD.CPP
//Description: God Unit

#include <OSYS.h>
#include <OGAME.h>
#include <OBUTT3D.h>
#include <OGODRES.h>
#include <OF_BASE.h>
#include <ORACERES.h>
#include <OPOWER.h>
#include <OU_MARI.h>
#include <OU_GOD.h>
#include <OHELP.h>
#include "gettext.h"
#include <OWEATHER.h>
#include <OTORNADO.h>
#include <OBULLET.h>
#include <OEFFECT.h>
#include <OSFRMRES.h>
#include <OSPRTRES.h>
#include <OSPY.h>
#include <OTOWN.h>
#include <OWORLD.h>
#include <OWORLDMT.h>
#include <OUNITRES.h>
#include <COLCODE.h>
#include <OVGA.h>
#include <OMISC.h>
#include <string.h>

//--------- Define static vars ----------//

static Button3D button_cast, button_cast2;

//------- 7kmod-gameplay: the changes to the Greater Beings --------//

static short djini_convert_recno;		// the soldier converted by the power of the Djini (UnitGod::cast_power())

static char* fireball_icon();

//------- Define class PhoenixEffect --------//
//
// 7kmod-gameplay: the effect of the Phoenix exposing a spy: the ring of the
// Mind Turner (GJ_STAR) drawn in the colors of fire. Effects are not saved.

class PhoenixEffect : public Effect
{
public:
	void draw();
};

static char phoenix_color_table[256];		// purple -> fire colors (indices of PAL_STD)

//--------- Begin of function UnitGod::init_derived --------//
void UnitGod::init_derived()
{
	// ##### patch begin Gilbert 22/1 ######//
	cast_power_type = 0;
	// ##### end begin Gilbert 22/1 ######//
	if(unit_id==UNIT_PERSIAN_HEALER || unit_id==UNIT_VIKING_GOD ||
		unit_id==UNIT_KUKULCAN || unit_id==UNIT_JAPANESE_GOD)
		can_attack_flag = 0; // unable to attack

	if(unit_id==UNIT_EGYPTIAN_GOD || unit_id==UNIT_INDIAN_GOD ||
		unit_id==UNIT_ZULU_GOD)
		can_attack_flag = 0; // unable to attack

	if( unit_id==UNIT_DRAGON )		// 7kmod-gameplay: the Dragon throws its fireball with a button instead
		can_attack_flag = 0;
}
//-------- End of function UnitGod::init_derived -------//


//------- Begin of function UnitGod::pre_process -------//
//
void UnitGod::pre_process()
{
	Unit::pre_process();

	if( game.game_mode == GAME_TEST )
		return;

	//---- set force_move_flag to 1 if the god does not have the ability to attack ----//

	if( god_id != GOD_CHINESE )		// only the Chinese dragon can attack (7kmod-gameplay: the Norman one uses a button)
		force_move_flag = 1;

	if( god_id == GOD_NORMAN )			// 7kmod-gameplay: also the Dragons of games saved before
		can_attack_flag = 0;

	//-*********** simulate aat ************-//
	#ifdef DEBUG
		if(debug_sim_game_type)
			return;
	#endif
	//-*********** simulate aat ************-//

	//--- if the seat of power supporting this unit is destroyed, this unit dies ---//

	if( firm_array.is_deleted(base_firm_recno) )
	{
		hit_points=(float)0;
		set_die();
		return;
	}

	//---- this unit consume pray points as it exists ----//

	FirmBase* firmBase = (FirmBase*) firm_array[base_firm_recno];

	err_when( firmBase->firm_id != FIRM_BASE );

	firmBase->pray_points -= (float) god_res[god_id]->exist_pray_points / 200;

	if( firmBase->pray_points < 0 )
		firmBase->pray_points = (float) 0;

	//--------- update hit points --------//

	hit_points = (short) firmBase->pray_points;

	if( hit_points == 0 )
		set_die();
}
//-------- End of function UnitGod::pre_process -------//


//--------- Begin of function UnitGod::process_attack --------//

int UnitGod::process_attack()
{
	if( !Sprite::process_attack() )		// return 1 if the unit just finished its current attack
		return 0;

	if( game.game_mode == GAME_TEST )
		return 1;

	//-*********** simulate aat ************-//
	#ifdef DEBUG
		if(debug_sim_game_type)
			return 1;
	#endif
	//-*********** simulate aat ************-//

	//------- consumer pray points --------//
	/* The other gods have their prayer points consumed
	 * in UnitGod::cast_power(). See comments there for an
	 * explanation of the exploit this avoids.
	 */
	if (god_id == GOD_CHINESE)		// 7kmod-gameplay: the fireball of the Norman Dragon is consumed in cast_power()
		consume_power_pray_points();

	return 1;
}
//---------- End of function UnitGod::process_attack ----------//


//--------- Begin of function UnitGod::disp_info ---------//
//
void UnitGod::disp_info(int refreshFlag)
{
	disp_basic_info(INFO_Y1, refreshFlag);
	disp_unit_profile( INFO_Y1+54, refreshFlag );

	if( !is_own() )
		return;

	if( game.game_mode == GAME_TEST )
		return;

	if( god_res[god_id]->can_cast_power )
	{
		//-------- get the button name --------//

		const char* buttonName;

		switch(god_id)
		{
			case GOD_PERSIAN:
				buttonName = "GODHEAL";
				break;

			case GOD_JAPANESE:
				buttonName = "GODMIND";
				break;

			case GOD_MAYA:
				buttonName = "GODINCCL";
				break;

			case GOD_VIKING:
				buttonName = "GODRAIN";
				break;

			case GOD_EGYPTIAN:
				buttonName = "GODEGYPT";
				break;

			case GOD_INDIAN:
				buttonName = "GODMUGL";
				break;

			case GOD_ZULU:
				buttonName = "GODZULU";
				break;

			case GOD_NORMAN:		// 7kmod-gameplay: the fireball (its icon is a frame of the fireball, see below)
				buttonName = "GODHEAL";
				break;

			case GOD_GREEK:		// 7kmod-gameplay: exposing a spy
				buttonName = "SPYMENU";
				break;

			default:
				err_here();
		}

		//----------- create the cast button -----------//

		button_cast.create(INFO_X1, INFO_Y1+101, 'A', buttonName );

		if( god_id == GOD_NORMAN && fireball_icon() )
			button_cast.icon_ptr = fireball_icon();

		button_cast.paint();

		if( hit_points >= god_res[god_id]->power_pray_points )
			button_cast.enable();
		else
			button_cast.disable();

		//--- 7kmod: the powers that have no help text in HELP.RES get their own ---//

		const char* helpTitle;
		const char* helpDetail;

		if( power_help(helpTitle, helpDetail) )
		{
			button_cast.set_help_code("");
			help.set_custom_help( button_cast.x1, button_cast.y1, button_cast.x2, button_cast.y2, helpTitle, helpDetail );
		}

		//------ The viking gods have two types of power ------//

		if( god_id == GOD_VIKING )
		{
			button_cast2.paint(INFO_X1+BUTTON_ACTION_WIDTH, INFO_Y1+101, 'A', "GODTORNA" );

			if( hit_points >= god_res[god_id]->power_pray_points )
				button_cast2.enable();
			else
				button_cast2.disable();
		}
	}
}
//---------- End of function UnitGod::disp_info ----------//


//--------- Begin of function UnitGod::power_help ---------//
//
// 7kmod: the help of the power button of the Greater Beings whose button
// has no entry in HELP.RES (GODMUGL, GODEGYPT and GODZULU), in the style of
// the others.
//
// return: <int> 1 - the god has its own help text
//
int UnitGod::power_help(const char*& helpTitle, const char*& helpDetail)
{
	switch( god_id )
	{
		case GOD_INDIAN:		// 7kmod-gameplay: it also converts a soldier
			helpTitle  = _("Lower Enemy Loyalty");
			helpDetail = _("When you Click on this tile, your cursor will change into a large white circle. Place this circle over the units of kingdoms you are at war with, or of no kingdom, and Click again to lower their loyalty. The soldier with the lowest combat level in the circle joins your kingdom. Repeat until the Djini's power has been depleted.");
			return 1;

		case GOD_NORMAN:		// 7kmod-gameplay
			helpTitle  = _("Fireball");
			helpDetail = _("When you Click on this tile, your cursor will change into a large white circle. Place this circle over your target and Click again: the Dragon throws a fireball that burns everything in the circle. Each fireball costs 100 points.");
			return 1;

		case GOD_GREEK:		// 7kmod-gameplay
			helpTitle  = _("Expose Spy");
			helpDetail = _("When you Click on this tile, your cursor will change into a small square. Place this square over one of your villages and Click again: the Phoenix exposes and executes the enemy spy with the lowest spying skill in it. Costs 100 points, even if there is no spy.");
			return 1;

		case GOD_EGYPTIAN:
			helpTitle  = _("Increase Population");
			helpDetail = _("When you Click on this tile, your cursor will change into a small square. Place this square over one of your villages and Click again to add villagers to it. Repeat until Isis's power has been depleted.");
			return 1;

		case GOD_ZULU:
			helpTitle  = _("Increase Leadership");
			helpDetail = _("When you Click on this tile, your cursor will change into a small square. Place this square over one of your Zulu generals or your Zulu king and Click again to increase his leadership. Repeat until uNkulunkulu's power has been depleted.");
			return 1;
	}

	return 0;
}
//---------- End of function UnitGod::power_help ----------//


//--------- Begin of function UnitGod::dragon_fireball ---------//
//
// 7kmod-gameplay: the power of the Norman Dragon: a fireball thrown at the
// location, with much more damage and area than its old attack.
//
void UnitGod::dragon_fireball(int xLoc, int yLoc)
{
	if( attack_count <= 0 )
		return;

	AttackInfo* attackInfo = attack_info_array + cur_attack;
	Bullet* bulletPtr = NULL;

	if( !bullet_array.create_bullet(attackInfo->bullet_sprite_id, &bulletPtr) || !bulletPtr )
		return;

	bulletPtr->init(BULLET_BY_UNIT, sprite_recno, xLoc, yLoc, UNIT_LAND);

	bulletPtr->attack_damage = (float) (DRAGON_FIREBALL_DAMAGE + misc.random(3)) / ATTACK_SLOW_DOWN;
	bulletPtr->damage_radius = DRAGON_FIREBALL_RADIUS;
	bulletPtr->fire_radius	 = DRAGON_FIREBALL_FIRE_RADIUS;
}
//---------- End of function UnitGod::dragon_fireball ----------//


//--------- Begin of function UnitGod::phoenix_target_town ---------//
//
// 7kmod-gameplay: the Phoenix casts its power on a village of its kingdom.
//
// return: <int> the recno of the village, 0 - not a village of the kingdom
//
int UnitGod::phoenix_target_town(int xLoc, int yLoc)
{
	if( xLoc < 0 || yLoc < 0 || xLoc >= MAX_WORLD_X_LOC || yLoc >= MAX_WORLD_Y_LOC )
		return 0;

	Location* locPtr = world.get_loc(xLoc, yLoc);

	if( !locPtr->is_town() )
		return 0;

	int townRecno = locPtr->town_recno();

	if( town_array.is_deleted(townRecno) || town_array[townRecno]->nation_recno != nation_recno )
		return 0;

	return townRecno;
}
//---------- End of function UnitGod::phoenix_target_town ----------//


//--------- Begin of function UnitGod::phoenix_expose_spy ---------//
//
// 7kmod-gameplay: the power of the Phoenix: the spy of another kingdom with
// the lowest spying skill in the village is exposed and executed, as when
// caught by counter-espionage. Without spies, the points are spent anyway.
//
void UnitGod::phoenix_expose_spy(int townRecno)
{
	if( !townRecno || town_array.is_deleted(townRecno) )
		return;

	Town* townPtr = town_array[townRecno];

	//------- the effect: a ring of fire over the village -------//

	AttackInfo* attackInfo = unit_res.get_attack_info( unit_res[ god_res[GOD_JAPANESE]->unit_id ]->first_attack );
	int effectSpriteId = attackInfo->effect_id ? attackInfo->effect_id : attackInfo->bullet_sprite_id;

	if( effectSpriteId )
	{
		if( !phoenix_color_table[1] )		// not initialized yet
		{
			for( int i=0 ; i<256 ; i++ )
				phoenix_color_table[i] = (char) i;

			static unsigned char purpleArray[] = { 138, 137, 172, 205, 173, 174, 170 };
			static unsigned char fireArray[]   = { 176, 177, 177, 177, 160, 161, 160 };

			for( int i=0 ; i<(int)sizeof(purpleArray) ; i++ )
				phoenix_color_table[purpleArray[i]] = (char) fireArray[i];
		}

		PhoenixEffect* effectPtr = new PhoenixEffect;
		effectPtr->init( effectSpriteId, townPtr->center_x*ZOOM_LOC_WIDTH, townPtr->center_y*ZOOM_LOC_HEIGHT,
							  SPRITE_IDLE, 0, 8, 0 );
		effect_array.add(effectPtr);
	}

	//------- the spy with the lowest skill -------//

	int bestSpyRecno=0, bestSkill=1000;

	for( int i=spy_array.size() ; i>0 ; i-- )
	{
		if( spy_array.is_deleted(i) )
			continue;

		Spy* spyPtr = spy_array[i];

		if( spyPtr->spy_place == SPY_TOWN && spyPtr->spy_place_para == townRecno &&
			 spyPtr->true_nation_recno != nation_recno && spyPtr->spy_skill < bestSkill )
		{
			bestSkill = spyPtr->spy_skill;
			bestSpyRecno = i;
		}
	}

	if( bestSpyRecno )
		spy_array[bestSpyRecno]->get_killed();		// with the news of a spy caught
}
//---------- End of function UnitGod::phoenix_expose_spy ----------//


//--------- Begin of function PhoenixEffect::draw ---------//
//
// As Sprite::draw(), with the colors of the bitmap changed.
//
void PhoenixEffect::draw()
{
	int needMirror;
	SpriteFrame* spriteFrame = cur_sprite_frame(&needMirror);
	update_abs_pos(spriteFrame);

	char* bitmapPtr = sprite_info->res_bitmap.read_imported(spriteFrame->bitmap_offset);

	int x1 = abs_x1-World::view_top_x;
	int y1 = abs_y1-World::view_top_y;

	if( x1 <= -spriteFrame->width || x1 >= ZOOM_WIDTH || y1 <= -spriteFrame->height || y1 >= ZOOM_HEIGHT )
		return;

	int x2 = abs_x2-World::view_top_x;
	int y2 = abs_y2-World::view_top_y;

	int srcX1 = MAX(0,x1)-x1, srcY1 = MAX(0,y1)-y1;
	int srcX2 = MIN(ZOOM_WIDTH-1,x2)-x1, srcY2 = MIN(ZOOM_HEIGHT-1,y2)-y1;

	if( needMirror )
		vga_back.put_bitmap_area_trans_remap_decompress_hmirror( x1+ZOOM_X1, y1+ZOOM_Y1, bitmapPtr, srcX1, srcY1, srcX2, srcY2, phoenix_color_table );
	else
		vga_back.put_bitmap_area_trans_remap_decompress( x1+ZOOM_X1, y1+ZOOM_Y1, bitmapPtr, srcX1, srcY1, srcX2, srcY2, phoenix_color_table );
}
//---------- End of function PhoenixEffect::draw ----------//


//--------- Begin of static function fireball_icon ---------//
//
// 7kmod-gameplay: the icon of the fireball button of the Dragon: a frame
// of its fireball (DR_ARROW, flying north-east), a compressed transparent
// bitmap like the icons of the buttons.
//
// return: <char*> the bitmap, NULL if not available
//
static char* fireball_icon()
{
	static char* iconPtr = NULL;
	static char  triedFlag = 0;

	if( triedFlag )
		return iconPtr;

	triedFlag = 1;

	int unitId = god_res[GOD_NORMAN]->unit_id;
	UnitInfo* unitInfo = unit_res[unitId];

	if( !unitInfo->first_attack )
		return NULL;

	int spriteId = unit_res.get_attack_info(unitInfo->first_attack)->bullet_sprite_id;

	if( !spriteId )
		return NULL;

	SpriteInfo* spriteInfo = sprite_res[spriteId];
	SpriteMove* spriteMove = spriteInfo->move_array + 1;		// north-east

	if( spriteMove->frame_count < 3 )
		return NULL;

	spriteInfo->load_bitmap_res();

	SpriteFrame* spriteFrame = sprite_frame_res[spriteMove->first_frame_recno + 2];
	unsigned char* bitmapPtr = (unsigned char*) spriteInfo->res_bitmap.read_imported(spriteFrame->bitmap_offset);

	//--- the size of the compressed bitmap (see IMGbltTransDecompress()) ---//

	int width  = bitmapPtr[0] + (bitmapPtr[1]<<8);
	int height = bitmapPtr[2] + (bitmapPtr[3]<<8);
	int esi = 4, pixelsToSkip = 0;

	for( int j=0 ; j<height ; ++j )
	{
		for( int i=0 ; i<width ; ++i )
		{
			if( pixelsToSkip != 0 )
			{
				if( pixelsToSkip >= width-i )
				{
					pixelsToSkip -= width-i;
					break;
				}
				i += pixelsToSkip;
				pixelsToSkip = 0;
			}

			int al = bitmapPtr[esi++];

			if( al == MANY_TRANSPARENT_CODE )
				pixelsToSkip = bitmapPtr[esi++] - 1;
			else if( al > MANY_TRANSPARENT_CODE )
				pixelsToSkip = 256 - al - 1;
		}
	}

	iconPtr = mem_add(esi);
	memcpy( iconPtr, bitmapPtr, esi );

	return iconPtr;
}
//---------- End of static function fireball_icon ----------//


//--------- Begin of function UnitGod::detect_info ---------//
//
void UnitGod::detect_info()
{
	if( detect_basic_info() )
		return;

	if( detect_unit_profile() )
		return;

	if( detect_select_hotkey() )
		return;

	if( !is_own() )
		return;

	if( game.game_mode == GAME_TEST )
		return;

	if( god_res[god_id]->can_cast_power )
	{
		// ###### begin Gilbert 14/10 ######//
		int rc=0;
		char castPowerType = 0;

		if( button_cast.detect() )
		{
			// cast_power_type = 1;
			castPowerType = 1;
			rc = 1;
		}

		if( button_cast2.detect() )
		{
			castPowerType = 2;
			//cast_power_type = 2;
			//cast_origin_x = cur_x_loc();
			//cast_origin_y = cur_y_loc();
			rc = 1;
		}

		//----------------------------------------//

		if( rc && castPowerType)
		{
			if( god_id == GOD_VIKING && castPowerType == 1 )	// summon rain, summon immediately, no need to select target
				go_cast_power(next_x_loc(), next_y_loc(), castPowerType, COMMAND_PLAYER);
			else
				power.issue_command(COMMAND_GOD_CAST_POWER, sprite_recno, castPowerType);
		}
		// ###### end Gilbert 14/10 ######//
	}
}
//---------- End of function UnitGod::detect_info ----------//


//--------- Begin of function UnitGod::is_in_build_menu ---------//
// Returns true if a unit is currently in build mode.
// Only reliable if this unit is the selected unit.
// Used by Info to detect if the build mode is opened.
//
bool UnitGod::is_in_build_menu()
{
	return false;
}
//----------- End of function UnitGod::is_in_build_menu -----------//


//--------- Begin of function UnitGod::cast_power ---------//
//
// <int> castXLoc, castYLoc - the location which the power should
//										be casted on.
//
void UnitGod::cast_power(int xLoc, int yLoc)
{
	err_when( !god_res[god_id]->can_cast_power );

	if( firm_array.is_deleted(base_firm_recno) )
		return;

	//--- 7kmod-gameplay: the Phoenix only casts on a village of its kingdom (no points spent elsewhere) ---//

	if( god_id == GOD_GREEK && !phoenix_target_town(xLoc, yLoc) )
		return;

	//------- consumer pray points --------//
	/* This must be done here to avoid an exploit where a human player 
	 * can cast an ability and then order the god to move within a 
	 * few frames and not consumer prayer points. This function is not
	 * used for the Chinese and Norman dragons so the function is also
	 * called in UnitGod::process_attack() for the dragons.
	 */
	if( !consume_power_pray_points() )
		return;

	//--- 7kmod-gameplay: the fireball of the Dragon and the Phoenix exposing a spy ---//

	if( god_id == GOD_NORMAN )
	{
		dragon_fireball(xLoc, yLoc);
		return;
	}

	if( god_id == GOD_GREEK )
	{
		phoenix_expose_spy( phoenix_target_town(xLoc, yLoc) );
		return;
	}

	//---- viking god does not need a range for casting power ----//

	if( god_id == GOD_VIKING )
	{
		if( cast_power_type == 1 )
			viking_summon_rain();
		else
			viking_summon_tornado();
		return;
	}

	//------ cast power on the selected area ------//

	GodInfo* godInfo = god_res[god_id];

	int xLoc1 = xLoc - godInfo->cast_power_range + 1;
	int yLoc1 = yLoc - godInfo->cast_power_range + 1;
	int xLoc2 = xLoc + godInfo->cast_power_range - 1;
	int yLoc2 = yLoc + godInfo->cast_power_range - 1;

	int t;
	int centerY = (yLoc1+yLoc2) / 2;
	Location* locPtr;

	djini_convert_recno = 0;		// 7kmod-gameplay: set by indian_cast_power()

	for( yLoc=yLoc1 ; yLoc<=yLoc2 ; yLoc++ )
	{
		t=abs(yLoc-centerY)/2;
		locPtr = world.get_loc(xLoc1+t, yLoc);

		for( xLoc=xLoc1+t ; xLoc<=xLoc2-t ; xLoc++, locPtr++ )
		{
			if( xLoc>=0 && xLoc<MAX_WORLD_X_LOC &&
				 yLoc>=0 && yLoc<MAX_WORLD_Y_LOC )
			{
				cast_on_loc(xLoc, yLoc);
			}
		}
	}

	//--- 7kmod-gameplay: the Djini converts the soldier with the lowest combat level in the area ---//

	if( god_id == GOD_INDIAN && djini_convert_recno && !unit_array.is_deleted(djini_convert_recno) )
		unit_array[djini_convert_recno]->betray(nation_recno);		// with the loyalty of a unit that changes sides
}
//---------- End of function UnitGod::cast_power ----------//


//--------- Begin of function UnitGod::viking_summon_rain ---------//
//
void UnitGod::viking_summon_rain()
{
	magic_weather.cast_rain(10, 8);         // 10 days, rain scale 8
	magic_weather.cast_lightning(7 );               // 7 days
}
//---------- End of function UnitGod::viking_summon_rain ----------//


//--------- Begin of function UnitGod::viking_summon_tornado ---------//
//
void UnitGod::viking_summon_tornado()
{
	// ######## begin Gilbert 14/10 ########//
	short xLoc = next_x_loc();
	short yLoc = next_y_loc();
	char dir = final_dir % 8;
	// ######## end Gilbert 14/10 ########//

	// put a tornado one location ahead
	if( dir == 0 || dir == 1 || dir == 7 )
		if( yLoc > 0 )
			yLoc--;
	if( dir >= 1 && dir <= 3 )
		if( xLoc < MAX_WORLD_X_LOC-1 )
			xLoc++;
	if( dir >= 3 && dir <= 5 )
		if( yLoc < MAX_WORLD_Y_LOC-1)
			yLoc++;
	if( dir >= 5 && dir <= 7 )
		if( xLoc > 0 )
			xLoc--;

	tornado_array.add_tornado( xLoc, yLoc, 600 );
	magic_weather.cast_wind(10, 1, dir * 45 );              // 10 days
}
//---------- End of function UnitGod::viking_summon_tornado ----------//


//--------- Begin of function UnitGod::cast_on_loc ---------//
//
void UnitGod::cast_on_loc(int castXLoc, int castYLoc)
{
	Location* locPtr = world.get_loc( castXLoc, castYLoc );

	//--- if there is any unit on the location ---//

	if( locPtr->has_unit(UNIT_LAND) )
	{
		cast_on_unit( locPtr->unit_recno(UNIT_LAND), 1 );
	}
	else if( locPtr->has_unit(UNIT_SEA) )
	{
		Unit* unitPtr = unit_array[ locPtr->unit_recno(UNIT_SEA) ];

		//-- only heal human units belonging to our nation in ships --//

		if( unitPtr->nation_recno == nation_recno &&
			 unit_res[unitPtr->unit_id]->unit_class == UNIT_CLASS_SHIP )
		{
			UnitMarine* unitMarine = (UnitMarine*) unitPtr;

			for( int i=0 ; i<unitMarine->unit_count ; i++ )
			{
				int divider = 4;		// the size of a ship is 4 locations (2x2)

				cast_on_unit( unitMarine->unit_recno_array[i], divider );		// the effects are weaken on ship units, only 50% of the original effects
			}
		}
	}

	//--------- on firms ---------//

	else if( locPtr->is_firm() )
	{
		Firm* firmPtr = firm_array[ locPtr->firm_recno() ];
		int	divider = (firmPtr->loc_x2-firmPtr->loc_x1+1) * (firmPtr->loc_y2-firmPtr->loc_y1+1);
		if( god_id == GOD_ZULU )
			divider = 1;		// range of zulu god is 1, no need to divide

		if( firmPtr->overseer_recno )
		{
			cast_on_unit( firmPtr->overseer_recno, divider );
		}

		if( firmPtr->worker_array && firm_res[firmPtr->firm_id]->live_in_town==0 )
		{
			Worker* workerPtr = firmPtr->worker_array;

			for( int i=0 ; i<firmPtr->worker_count ; i++, workerPtr++ )
			{
				cast_on_worker(workerPtr, firmPtr->nation_recno, divider);
			}
		}
	}

	//--------- on towns ----------//

	else if( locPtr->is_town() )
	{
		Town* townPtr = town_array[ locPtr->town_recno() ];

		if( god_id == GOD_JAPANESE && townPtr->nation_recno != nation_recno)
		{
			int divider = STD_TOWN_LOC_WIDTH * STD_TOWN_LOC_HEIGHT;

			for( int i=0 ; i<MAX_RACE ; i++ )
			{
				if( townPtr->race_pop_array[i]==0 )
					continue;

				float changePoints = (float)7 + misc.random(8);		// decrease 7 to 15 loyalty points instantly

				if( townPtr->nation_recno )
					townPtr->change_loyalty(i+1, -changePoints/divider);
				else
					townPtr->change_resistance(i+1, nation_recno, -changePoints/divider);
			}
		}
		else if( god_id == GOD_EGYPTIAN && townPtr->nation_recno == nation_recno)
		{
			int headCount;
			int raceId;

			for( headCount = 5; headCount > 0 && townPtr->population < MAX_TOWN_GROWTH_POPULATION
				&& (raceId = townPtr->pick_random_race(1,1)); --headCount )
			{
				townPtr->inc_pop(raceId, 0, (int)townPtr->race_loyalty_array[raceId-1]);
			}
		}
	}
}
//---------- End of function UnitGod::cast_on_loc ----------//


//--------- Begin of function UnitGod::cast_on_unit ---------//
//
// <int> unitRecno - recno of the unit to cast on
// <int> divider   - divide the amount of effects by this number
//
void UnitGod::cast_on_unit(int unitRecno, int divider)
{
	switch(god_id)
	{
		case GOD_PERSIAN:
			persian_cast_power( unitRecno, divider );
			break;

		case GOD_JAPANESE:
			japanese_cast_power( unitRecno, divider );
			break;

		case GOD_MAYA:
			maya_cast_power( unitRecno, divider );
			break;

		case GOD_EGYPTIAN:
			egyptian_cast_power( unitRecno, divider);
			break;

		case GOD_INDIAN:
			indian_cast_power( unitRecno, divider);
			break;

		case GOD_ZULU:
			zulu_cast_power( unitRecno, divider);
			break;

		default:
			err_here();
	}
}
//---------- End of function UnitGod::cast_on_unit ----------//


//--------- Begin of function UnitGod::cast_on_worker ---------//
//
// <Worker*> workerPtr - pointer to the worker to be affected by the effect.
//
void UnitGod::cast_on_worker(Worker* workerPtr, int nationRecno, int divider)
{
	switch(god_id)
	{
		case GOD_PERSIAN:
			persian_cast_power( workerPtr, nationRecno, divider );
			break;

		case GOD_JAPANESE:
			japanese_cast_power( workerPtr, nationRecno, divider );
			break;

		case GOD_MAYA:
			maya_cast_power( workerPtr, nationRecno, divider );
			break;

		case GOD_EGYPTIAN:
			egyptian_cast_power(workerPtr, nationRecno, divider);
			break;

		case GOD_INDIAN:
			indian_cast_power(workerPtr, nationRecno, divider);
			break;
	
		case GOD_ZULU:
			zulu_cast_power(workerPtr, nationRecno, divider);
			break;

		default:
			err_here();
	}
}
//---------- End of function UnitGod::cast_on_worker ----------//


//--------- Begin of function UnitGod::persian_cast_power ---------//
//
void UnitGod::persian_cast_power(int unitRecno, int divider)
{
	Unit* unitPtr = unit_array[unitRecno];

	//-- only heal human units belonging to our nation --//
	//-- 7kmod-gameplay: and the Fryhtans of our nation --//

	if( unitPtr->nation_recno == nation_recno &&
		 (unitPtr->race_id > 0 || unit_res[unitPtr->unit_id]->unit_class == UNIT_CLASS_MONSTER) )
	{
		float changePoints;

		//--- 7kmod-gameplay: the general of a fort or Seat of Power heals as much as his soldiers ---//

		if( unitPtr->unit_mode == UNIT_MODE_OVERSEE &&
			 (firm_array[unitPtr->unit_mode_para]->firm_id == FIRM_CAMP ||
			  firm_array[unitPtr->unit_mode_para]->firm_id == FIRM_BASE) )
		{
			changePoints = (float) unitPtr->max_hit_points / (3+misc.random(3));	 // divided by (3 to 5)
		}
		else
			changePoints = (float) unitPtr->max_hit_points / (6+misc.random(4));	 // divided by (6 to 9)

		changePoints = MAX( changePoints, 10 );

		unitPtr->change_hit_points( changePoints/divider );
	}
}
//---------- End of function UnitGod::persian_cast_power ----------//


//--------- Begin of function UnitGod::japanese_cast_power ---------//
//
void UnitGod::japanese_cast_power(int unitRecno, int divider)
{
	Unit* unitPtr = unit_array[unitRecno];

	//-- only cast on enemy units -----//

	if( unitPtr->nation_recno != nation_recno && unitPtr->race_id > 0 )
	{
		int changePoints = 7 + misc.random(8);		// decrease 7 to 15 loyalty points instantly

		unitPtr->change_loyalty( -MAX(1, changePoints/divider) );
	}
}
//---------- End of function UnitGod::japanese_cast_power ----------//


//--------- Begin of function UnitGod::maya_cast_power ---------//
//
void UnitGod::maya_cast_power(int unitRecno, int divider)
{
	Unit* unitPtr = unit_array[unitRecno];

	//-- only cast on mayan units belonging to our nation --//

	if( unitPtr->nation_recno == nation_recno && unitPtr->race_id == RACE_MAYA )
	{
		int changePoints = 15 + misc.random(10);		// add 15 to 25 points to its combat level instantly

		int newCombatLevel = unitPtr->skill.combat_level + changePoints/divider;

		if( newCombatLevel > 100 )
			newCombatLevel = 100;

		float oldHitPoints = unitPtr->hit_points;

		unitPtr->set_combat_level(newCombatLevel);

		unitPtr->hit_points = oldHitPoints;		// keep the hit points unchanged.
	}
}
//---------- End of function UnitGod::maya_cast_power ----------//


//--------- Begin of function UnitGod::persian_cast_power ---------//
//
void UnitGod::persian_cast_power(Worker* workerPtr, int nationRecno, int divider)
{
	//-- only heal human units belonging to our nation --//
	//-- 7kmod-gameplay: and the Fryhtans of our nation in forts --//

	if( nationRecno == nation_recno &&
		 (workerPtr->race_id > 0 || unit_res[workerPtr->unit_id]->unit_class == UNIT_CLASS_MONSTER) )
	{
		int changePoints = workerPtr->max_hit_points() / (3+misc.random(3));	 // 7kmod-gameplay: divided by (3 to 5), it was (4 to 7)

		changePoints = MAX( changePoints, 10 );

		workerPtr->change_hit_points( MAX(1, changePoints/divider) );
	}
}
//---------- End of function UnitGod::persian_cast_power ----------//


//--------- Begin of function UnitGod::japanese_cast_power ---------//
//
void UnitGod::japanese_cast_power(Worker* workerPtr, int nationRecno, int divider)
{
	//-- only cast on enemy units -----//

	if( nationRecno != nation_recno && workerPtr->race_id > 0 )
	{
		int changePoints = 7 + misc.random(8);		// decrease 7 to 15 loyalty points instantly

		workerPtr->change_loyalty( -MAX(1, changePoints/divider) );
	}
}
//---------- End of function UnitGod::japanese_cast_power ----------//


//--------- Begin of function UnitGod::maya_cast_power ---------//
//
void UnitGod::maya_cast_power(Worker* workerPtr, int nationRecno, int divider)
{
	//-- only cast on mayan units belonging to our nation --//

	if( nationRecno == nation_recno && workerPtr->race_id == RACE_MAYA )
	{
		int changePoints = 15 + misc.random(10);		// add 15 to 25 points to its combat level instantly

		int newCombatLevel = workerPtr->combat_level + MAX(1, changePoints/divider);

		if( newCombatLevel > 100 )
			newCombatLevel = 100;

		workerPtr->combat_level = newCombatLevel;
	}
}
//---------- End of function UnitGod::maya_cast_power ----------//


//--------- Begin of function UnitGod::consume_power_pray_points ---------//
//
// return one if points consumed, otherwise zero.
//
int UnitGod::consume_power_pray_points()
{
	if( firm_array.is_deleted(base_firm_recno) )
		return 0;

	FirmBase* firmBase = (FirmBase*) firm_array[base_firm_recno];

	err_when( firmBase->firm_id != FIRM_BASE );

	firmBase->pray_points -= god_res[god_id]->power_pray_points;

	if( firmBase->pray_points < 0 )
		firmBase->pray_points = (float) 0;

	hit_points = (short) firmBase->pray_points;

	return 1;
}
//---------- End of function UnitGod::consume_power_pray_points ----------//



//--------- Begin of function UnitGod::egyptian_cast_power ---------//
//
void UnitGod::egyptian_cast_power(int unitRecno, int divider)
{
	// no effect
}
//---------- End of function UnitGod::egyptian_cast_power ----------//

//--------- Begin of function UnitGod::indian_cast_power ---------//
//
void UnitGod::indian_cast_power(int unitRecno, int divider)
{
	Unit* unitPtr = unit_array[unitRecno];

	if( unitPtr->is_visible() && nation_array.should_attack(nation_recno, unitPtr->nation_recno) )
	{
		unitPtr->change_loyalty(-30 + misc.random(11));

		//--- 7kmod-gameplay: the soldier with the lowest combat level (then the lowest loyalty) is converted ---//
		//
		// Only people of the rank of soldier, not spies, on the map.

		if( unit_res[unitPtr->unit_id]->unit_class == UNIT_CLASS_HUMAN && unitPtr->race_id &&
			 unitPtr->rank_id == RANK_SOLDIER && !unitPtr->spy_recno && !unitPtr->unit_mode )
		{
			int better = 1;

			if( djini_convert_recno && !unit_array.is_deleted(djini_convert_recno) )
			{
				Unit* bestUnit = unit_array[djini_convert_recno];

				better = unitPtr->skill.combat_level < bestUnit->skill.combat_level ||
							(unitPtr->skill.combat_level == bestUnit->skill.combat_level && unitPtr->loyalty < bestUnit->loyalty);
			}

			if( better )
				djini_convert_recno = unitRecno;
		}
	}
}
//---------- End of function UnitGod::indian_cast_power ----------//


//--------- Begin of function UnitGod::zulu_cast_power ---------//
//
void UnitGod::zulu_cast_power(int unitRecno, int divider)
{
	// no effect
	Unit* unitPtr = unit_array[unitRecno];

	if( nation_recno == unitPtr->nation_recno && 
		unitPtr->race_id == RACE_ZULU && unitPtr->rank_id != RANK_SOLDIER)
	{
		int changePoints = 30;	// add 15 twice to avoid 130 becomes -126
		if( divider > 2 )
		{
			unitPtr->skill.skill_level += changePoints/divider;
			if( unitPtr->skill.skill_level > 100 )
				unitPtr->skill.skill_level = 100;
		}
		else
		{
			for(int t = 2; t > 0; --t )
			{
				unitPtr->skill.skill_level += changePoints/2/divider;
				if( unitPtr->skill.skill_level > 100 )
					unitPtr->skill.skill_level = 100;
			}
		}
	}
}
//---------- End of function UnitGod::zulu_cast_power ----------//


//--------- Begin of function UnitGod::egyptian_cast_power ---------//
//
void UnitGod::egyptian_cast_power(Worker *workerPtr, int nationRecno, int divider)
{
	// no effect
}
//---------- End of function UnitGod::egyptian_cast_power ----------//


//--------- Begin of function UnitGod::indian_cast_power ---------//
//
void UnitGod::indian_cast_power(Worker *workerPtr, int nationRecno, int divider)
{
	// no effect
}
//---------- End of function UnitGod::indian_cast_power ----------//


//--------- Begin of function UnitGod::zulu_cast_power ---------//
//
void UnitGod::zulu_cast_power(Worker *workerPtr, int nationRecno, int divider)
{
	// no effect
}
//---------- End of function UnitGod::zulu_cast_power ----------//
