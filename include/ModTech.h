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

//Filename    : ModTech.h
//Description : 7kmod-gameplay technologies researched in towers of science
//
// These technologies are separate from the weapon technologies of
// TechRes, which are read from the game data and all unlock a weapon.
// They are only shown in the "Technologies" research menu of the towers
// of science.

#ifndef __MODTECH_H
#define __MODTECH_H

#include <GAMEDEF.h>

class File;

//------- Define the technologies --------//

enum { MOD_TECH_ELITE_WARRIORS=1,		// units trained in Leadership start with ELITE_WARRIORS_COMBAT_LEVEL combat
		 MOD_TECH_ELITE_WORKERS,			// units trained in other skills start with ELITE_WORKERS_SKILL_LEVEL skill
		 MOD_TECH_BUILDING_REPAIR,			// buildings recover 1 hit point every BUILDING_REPAIR_DAYS days
		 MAX_MOD_TECH=MOD_TECH_BUILDING_REPAIR
	  };

#define MAX_MOD_TECH_BEFORE_ELITE_WORKERS		1		// the number of technologies in saved games before GAME_VERSION_7KMOD_ELITE_WORKERS
#define MAX_MOD_TECH_BEFORE_BUILDING_REPAIR	2		// the number of technologies in saved games before GAME_VERSION_7KMOD_BUILDING_REPAIR

#define ELITE_WARRIORS_COMBAT_LEVEL		30
#define ELITE_WORKERS_SKILL_LEVEL		30
#define BUILDING_REPAIR_DAYS				5

//------- Define class ModTechRes --------//

class ModTechRes
{
public:
	char  nation_tech_level_array[MAX_MOD_TECH][MAX_NATION];				// the researched level of each technology of each nation
	float nation_research_progress_array[MAX_MOD_TECH][MAX_NATION];	// research progress, the level goes up when it reaches 100

public:
	void			reset();
	void			init_nation(int nationRecno);

	const char*	name(int modTechId);
	const char*	des(int modTechId);
	float			complex_level(int modTechId);
	int			max_level(int modTechId);

	int			get_level(int modTechId, int nationRecno);
	int			can_research(int modTechId, int nationRecno);
	float			get_progress(int modTechId, int nationRecno);
	int			progress(int modTechId, int nationRecno, float progressPoint);

	int			write_file(File* filePtr);
	int			read_file(File* filePtr, int techCount=MAX_MOD_TECH);
};

extern ModTechRes mod_tech_res;

#endif
