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

//Filename    : OGODRES.H
//Description : Header file of object RaceRes

#ifndef __OGODRES_H
#define __OGODRES_H

#ifndef __ONATION_H
#include <ONATION.h>
#endif

#ifndef __OFILE_H
#include <OFILE.h>
#endif

//------------ Define god id. -------------//

enum { MAX_GOD=10 };

enum { GOD_NORMAN=1,
		 GOD_MAYA,
		 GOD_GREEK,
		 GOD_VIKING,
		 GOD_PERSIAN,
		 GOD_CHINESE,
		 GOD_JAPANESE,
		 GOD_EGYPTIAN,
		 GOD_INDIAN,
		 GOD_ZULU,
	  };

//------------ Define struct GodRec ---------------//

struct GodRec
{
	enum { RACE_CODE_LEN=8, UNIT_CODE_LEN=8, PRAY_POINTS_LEN=3,
			 CAST_POWER_RANGE_LEN=3, RACE_ID_LEN=3, UNIT_ID_LEN=3 };

	char race[RACE_CODE_LEN];
	char unit[UNIT_CODE_LEN];
	char exist_pray_points[PRAY_POINTS_LEN];
	char power_pray_points[PRAY_POINTS_LEN];
	char can_cast_power;			// whether this god creature can cast power or not
	char cast_power_range[CAST_POWER_RANGE_LEN];

	char race_id[RACE_ID_LEN];
	char unit_id[UNIT_ID_LEN];
};

//------------- Define struct GodInfo --------------//

struct GodInfoGF;

#pragma pack(1)
struct GodInfo
{
public:
	//----- constant vars ------//

	char  god_id;
	char  race_id;
	char  unit_id;
	short	exist_pray_points;		// pray points consumption for the god to exist for 100 frames
	short power_pray_points;		// pray points consumption for each casting of its power
	char	can_cast_power;			// whether this god creature can cast power or not
	char	cast_power_range;			// location range of casting power

	//------ game vars ------//

	char	nation_know_array[MAX_NATION];
	int	is_nation_know(int nationRecno)	{ return nation_know_array[nationRecno-1]; }

public:
	short	invoke(int firmRecno, int xLoc, int yLoc);

	void	enable_know(int nationRecno);
	void	disable_know(int nationRecno);

	void	write_record(GodInfoGF *r);
	void	read_record(GodInfoGF *r);
};
#pragma pack()

//------ 7kmod-gameplay: changes to the Greater Beings (GodRes::apply_mod_changes()) ------//

#define DRAGON_FIREBALL_PRAY_POINTS		100		// the Norman Dragon: a fireball with a button instead of attacking
#define DRAGON_FIREBALL_DAMAGE			150		// base damage (the attack of the Dragon has 15)
#define DRAGON_FIREBALL_RADIUS			109		// damage radius in pixels (45 + 2 locations)
#define DRAGON_FIREBALL_FIRE_RADIUS		5			// (3 + 2)
#define DRAGON_FIREBALL_CAST_RANGE		4			// the area shown by the cursor (cast_power_range)
#define CHINESE_DRAGON_ATTACK_PRAY_POINTS	5		// each attack of the Jing Nung (10 in the data)
#define PHOENIX_EXIST_PRAY_POINTS		30			// 1.5 per day (50 in the data: 2.5 per day)
#define PHOENIX_POWER_PRAY_POINTS		100		// exposing a spy in one of your villages

//----------- Define class GodRes ---------------//

class GodRes
{
public:
	char        init_flag;

	short       god_count;
	GodInfo*    god_info_array;

public:
	GodRes();

	void        init();
	void        deinit();

   int			is_god_unit(int unitId);		

	void			init_nation_know(int nationRecno);
	void			enable_know_all(int nationRecno);

	int         write_file(File*);
	int         read_file(File*);

	GodInfo*    operator[](int godId);      // pass godId  as recno

	void			apply_mod_changes();		// 7kmod-gameplay

private:
	void        load_god_info();
};

extern GodRes god_res;

//----------------------------------------------------//

#endif
