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

//Filename    : ModSpy.h
//Description : 7kmod-gameplay espionage: propaganda in independent towns,
//              villagers joining through propaganda, accidents in firms and
//              intelligence reports. See 7kmod/TODO-espionaje.md.

#ifndef __MODSPY_H
#define __MODSPY_H

#include <stdint.h>
#include <GAMEDEF.h>

class File;
class Town;
class Firm;
class Spy;

//------- Propaganda in independent towns --------//

#define PROPAGANDA_MAX_MONTH_DEC			10		// maximum resistance decrease of a race in a month, from all the spies of a nation
#define PROPAGANDA_MIN_RESISTANCE		30		// without a linked fort with a general, propaganda doesn't decrease resistance below this
#define PROPAGANDA_MAX_INFLUENCE			20		// maximum bonus to the influence of a linked fort
#define PROPAGANDA_MAX_JOIN_CHANCE		30		// maximum chance (%) in a month that a villager joins the nation
#define PROPAGANDA_MAX_JOIN_SKILL		70		// maximum skill level of a villager that joins

//------- Accidents in firms --------//

#define ACCIDENT_DAILY_CHANCE				730	// 1 in 730 a day for each firm with workers: 1 in 2 years
#define ACCIDENT_MAX_SPY_MULTIPLIER		4		// spies in sabotage mode multiply the chance: 1 spy x2, 2 spies x3, 3 or more x4
#define ACCIDENT_MAX_SPY_PER_YEAR		3		// accidents caused by spies, for each nation, in 365 days

//------- Intelligence reports --------//

#define INTEL_GATHER_DAYS					30		// days of gathering for a report, a nation gets at most one report of another nation in this time
#define MAX_INTEL_REPORT					60		// reports kept for all nations
#define MAX_INTEL_REPORT_PER_NATION		30		// reports kept for each nation
#define INTEL_NAME_LEN						50		// as NationBase::NATION_NAME_LEN
#define INTEL_MAX_TECH						40		// technologies stored in a report (weapons, then the mod technologies)

enum { INTEL_SECTION_TOWN,
		 INTEL_SECTION_ECONOMY,
		 INTEL_SECTION_TRADE,
		 INTEL_SECTION_NATION,
		 INTEL_SECTION_TECH,
		 INTEL_SECTION_MILITARY,
		 INTEL_SECTION_SPY,
		 INTEL_SECTION_COUNT };

enum { INTEL_TOWN_COUNT,
		 INTEL_POPULATION,
		 INTEL_LOYALTY,
		 INTEL_CASH,
		 INTEL_FOOD,
		 INTEL_INCOME,
		 INTEL_EXPENSE,
		 INTEL_MARKET_COUNT,
		 INTEL_CARAVAN_COUNT,
		 INTEL_REPUTATION,
		 INTEL_SOLDIER_COUNT,
		 INTEL_GENERAL_COUNT,
		 INTEL_WEAPON_COUNT,
		 INTEL_SHIP_COUNT,
		 INTEL_CAMP_COUNT,
		 INTEL_SPY_COUNT,
		 INTEL_SPY_ABROAD_COUNT,
		 INTEL_VALUE_COUNT };

#pragma pack(1)
struct IntelReport
{
	int32_t report_date;
	int8_t  spy_nation_recno;				// the nation that got the report
	int8_t  target_nation_recno;
	char	  target_nation_name[INTEL_NAME_LEN+1];		// the nation may be gone later
	uint8_t target_nation_color;

	int8_t  spy_race_id;
	uint16_t spy_name_id;
	int8_t  spy_skill;

	int8_t  section_flag[INTEL_SECTION_COUNT];		// whether the spy had enough skill for each section

	int32_t value_low[INTEL_VALUE_COUNT];			// the values are approximate: a range
	int32_t value_high[INTEL_VALUE_COUNT];

	int8_t  relation_status[MAX_NATION];			// -1 if the nation doesn't exist
	char	  relation_nation_name[MAX_NATION][INTEL_NAME_LEN+1];
	uint8_t relation_nation_color[MAX_NATION];

	int8_t  tech_level[INTEL_MAX_TECH];			// tech_res technologies, then the mod technologies
};
#pragma pack()

//------- Define class ModSpy --------//

class ModSpy
{
public:
	int32_t last_accident_date[MAX_NATION];								// the last accident not caused by spies of each nation
	int32_t spy_accident_date[MAX_NATION][ACCIDENT_MAX_SPY_PER_YEAR];	// the last accidents caused by spies in each nation
	int16_t intel_progress[MAX_NATION][MAX_NATION];						// days of gathering of the spies of a nation in another nation

	int16_t		intel_report_count;
	IntelReport intel_report_array[MAX_INTEL_REPORT];

public:
	void	reset();
	void	init_nation(int nationRecno);

	void	next_day();

	//--- propaganda ---//

	void	process_propaganda(Town* townPtr);
	int	propaganda_influence_bonus(Town* townPtr, int nationRecno, int raceId);
	static int	has_linked_general_camp(Town* townPtr, int nationRecno);

	//--- accidents ---//

	void	process_accident(Firm* firmPtr);

	//--- intelligence reports ---//

	int	player_report_count();
	IntelReport* get_player_report(int recNo);		// 1 - the latest

	int	write_file(File* filePtr);
	int	read_file(File* filePtr);

private:
	void	add_villager(Town* townPtr, int nationRecno, int raceId, int totalSkill);
	void	gather_intelligence();
	void	make_report(Spy* spyPtr, int targetNationRecno);
	void	add_report(IntelReport* reportPtr);
	void	del_report(int reportId);
};

extern ModSpy mod_spy;

#endif
