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

//Filename    : ModTech.cpp
//Description : 7kmod-gameplay technologies researched in towers of science

#include <string.h>
#include <OFILE.h>
#include <ModTech.h>

//------- Define the technology information --------//

struct ModTechInfo
{
	const char* name;
	const char* des;
	float			complex_level;		// research difficulty, as TechInfo::complex_level
	char			max_level;
};

static ModTechInfo mod_tech_info_array[MAX_MOD_TECH] =
{
	// The weapons use 20 to 100. The level 1 catapult uses 20, these take 3/4 of its time.

	// MOD_TECH_ELITE_WARRIORS
	{ "Elite Warriors", "Trained soldiers start with 30 combat", (float) 15, 1 },

	// MOD_TECH_ELITE_WORKERS
	{ "Elite Workers", "Trained workers start with 30 skill", (float) 15, 1 },

	// MOD_TECH_BUILDING_REPAIR (research cost very low for testing)
	{ "Building Repair", "Buildings recover 1 HP every 5 days", (float) 1, 1 },

	// MOD_TECH_FRYHTAN_SLAYERS (research cost very low for testing)
	{ "Fryhtan Slayers", "Soldiers do 30% more damage to Fryhtans", (float) 1, 1 },
};

//------- Define the global object --------//

ModTechRes mod_tech_res;


//--------- Begin of function ModTechRes::reset --------//
//
void ModTechRes::reset()
{
	memset( nation_tech_level_array, 0, sizeof(nation_tech_level_array) );
	memset( nation_research_progress_array, 0, sizeof(nation_research_progress_array) );
}
//----------- End of function ModTechRes::reset ---------//


//--------- Begin of function ModTechRes::init_nation --------//
//
// Called when a nation is created.
//
void ModTechRes::init_nation(int nationRecno)
{
	for( int i=0 ; i<MAX_MOD_TECH ; i++ )
	{
		nation_tech_level_array[i][nationRecno-1] = 0;
		nation_research_progress_array[i][nationRecno-1] = (float) 0;
	}
}
//----------- End of function ModTechRes::init_nation ---------//


//--------- Begin of function ModTechRes::name --------//
//
const char* ModTechRes::name(int modTechId)
{
	return mod_tech_info_array[modTechId-1].name;
}
//----------- End of function ModTechRes::name ---------//


//--------- Begin of function ModTechRes::des --------//
//
const char* ModTechRes::des(int modTechId)
{
	return mod_tech_info_array[modTechId-1].des;
}
//----------- End of function ModTechRes::des ---------//


//--------- Begin of function ModTechRes::complex_level --------//
//
float ModTechRes::complex_level(int modTechId)
{
	return mod_tech_info_array[modTechId-1].complex_level;
}
//----------- End of function ModTechRes::complex_level ---------//


//--------- Begin of function ModTechRes::max_level --------//
//
int ModTechRes::max_level(int modTechId)
{
	return mod_tech_info_array[modTechId-1].max_level;
}
//----------- End of function ModTechRes::max_level ---------//


//--------- Begin of function ModTechRes::get_level --------//
//
int ModTechRes::get_level(int modTechId, int nationRecno)
{
	if( nationRecno < 1 || nationRecno > MAX_NATION )
		return 0;

	return nation_tech_level_array[modTechId-1][nationRecno-1];
}
//----------- End of function ModTechRes::get_level ---------//


//--------- Begin of function ModTechRes::can_research --------//
//
int ModTechRes::can_research(int modTechId, int nationRecno)
{
	return get_level(modTechId, nationRecno) < max_level(modTechId);
}
//----------- End of function ModTechRes::can_research ---------//


//--------- Begin of function ModTechRes::get_progress --------//
//
float ModTechRes::get_progress(int modTechId, int nationRecno)
{
	return nation_research_progress_array[modTechId-1][nationRecno-1];
}
//----------- End of function ModTechRes::get_progress ---------//


//--------- Begin of function ModTechRes::progress --------//
//
// Add research progress, as TechInfo::progress().
//
// return: <int> 1 - a new level has been researched
//               0 - not yet
//
int ModTechRes::progress(int modTechId, int nationRecno, float progressPoint)
{
	if( !can_research(modTechId, nationRecno) )
		return 0;

	float& curProgress = nation_research_progress_array[modTechId-1][nationRecno-1];

	curProgress += progressPoint;

	if( curProgress > 100 )
	{
		nation_tech_level_array[modTechId-1][nationRecno-1]++;
		curProgress = (float) 0;
		return 1;
	}

	return 0;
}
//----------- End of function ModTechRes::progress ---------//


//--------- Begin of function ModTechRes::write_file --------//
//
int ModTechRes::write_file(File* filePtr)
{
	if( !filePtr->file_write(nation_tech_level_array, sizeof(nation_tech_level_array)) )
		return 0;

	if( !filePtr->file_write(nation_research_progress_array, sizeof(nation_research_progress_array)) )
		return 0;

	return 1;
}
//----------- End of function ModTechRes::write_file ---------//


//--------- Begin of function ModTechRes::read_file --------//
//
// <int> techCount - the number of technologies in the saved game, older
//                    saved games have fewer. The others are not researched.
//
int ModTechRes::read_file(File* filePtr, int techCount)
{
	reset();

	if( !filePtr->file_read(nation_tech_level_array, sizeof(nation_tech_level_array[0]) * techCount) )
		return 0;

	if( !filePtr->file_read(nation_research_progress_array, sizeof(nation_research_progress_array[0]) * techCount) )
		return 0;

	return 1;
}
//----------- End of function ModTechRes::read_file ---------//
