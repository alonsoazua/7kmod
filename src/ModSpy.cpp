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

//Filename    : ModSpy.cpp
//Description : 7kmod-gameplay espionage: propaganda in independent towns,
//              villagers joining through propaganda, accidents in firms and
//              intelligence reports. See 7kmod/TODO-espionaje.md.

#include <string.h>
#include <stdlib.h>
#include <ALL.h>
#include <OFILE.h>
#include <OMISC.h>
#include <OINFO.h>
#include <OWORLD.h>
#include <OTOWN.h>
#include <OFIRM.h>
#include <OFIRMID.h>
#include <OSPY.h>
#include <ONATION.h>
#include <OUNIT.h>
#include <OUNITRES.h>
#include <OSPRTRES.h>
#include <ORACERES.h>
#include <OTECHRES.h>
#include <ONEWS.h>
#include <ModTech.h>
#include <ModSpy.h>

//------- Define the global object --------//

ModSpy mod_spy;

//------- the spying skill needed for each section of an intelligence report -------//

static char intel_section_skill_array[INTEL_SECTION_COUNT] = { 20, 30, 30, 40, 40, 50, 90 };


//--------- Begin of function ModSpy::reset --------//
//
void ModSpy::reset()
{
	memset( last_accident_date, 0, sizeof(last_accident_date) );
	memset( spy_accident_date, 0, sizeof(spy_accident_date) );
	memset( intel_progress, 0, sizeof(intel_progress) );

	intel_report_count = 0;
	memset( intel_report_array, 0, sizeof(intel_report_array) );
}
//----------- End of function ModSpy::reset ---------//


//--------- Begin of function ModSpy::init_nation --------//
//
// Called when a nation is created, as the nation recno may have been used
// by another nation before.
//
void ModSpy::init_nation(int nationRecno)
{
	int n = nationRecno-1;

	last_accident_date[n] = 0;

	for( int i=0 ; i<ACCIDENT_MAX_SPY_PER_YEAR ; i++ )
		spy_accident_date[n][i] = 0;

	for( int i=0 ; i<MAX_NATION ; i++ )
	{
		intel_progress[n][i] = 0;
		intel_progress[i][n] = 0;
	}

	for( int i=intel_report_count-1 ; i>=0 ; i-- )
	{
		if( intel_report_array[i].spy_nation_recno == nationRecno )
			del_report(i);
	}
}
//----------- End of function ModSpy::init_nation ---------//


//--------- Begin of function ModSpy::next_day --------//
//
void ModSpy::next_day()
{
	gather_intelligence();
}
//----------- End of function ModSpy::next_day ---------//


//--------- Begin of function ModSpy::has_linked_general_camp --------//
//
// Whether the nation has a fort with a general linked to the town, with
// the link enabled (as in Town::update_target_resistance()).
//
int ModSpy::has_linked_general_camp(Town* townPtr, int nationRecno)
{
	for( int i=0 ; i<townPtr->linked_firm_count ; i++ )
	{
		if( townPtr->linked_firm_enable_array[i] != LINK_EE )
			continue;

		Firm* firmPtr = firm_array[ townPtr->linked_firm_array[i] ];

		if( firmPtr->firm_id == FIRM_CAMP && firmPtr->nation_recno == nationRecno &&
			 firmPtr->overseer_recno )
		{
			return 1;
		}
	}

	return 0;
}
//----------- End of function ModSpy::has_linked_general_camp ---------//


//--------- Begin of function ModSpy::process_propaganda --------//
//
// Called every 30 days for each independent town.
//
// - The resistance of each race towards the nation of the spies decreases by
//   the skill of the spies / PROPAGANDA_SKILL_DIVIDER, at most PROPAGANDA_MAX_MONTH_DEC. Spies of
//   another race count a quarter. Without a linked fort with a general, it
//   doesn't go below PROPAGANDA_MIN_RESISTANCE.
// - There is a chance that a villager leaves the town and joins the nation:
//   for each race, the skill of the spies / PROPAGANDA_SKILL_DIVIDER * the population of the race
//   / 30 %, at most PROPAGANDA_MAX_JOIN_CHANCE % in total.
//
void ModSpy::process_propaganda(Town* townPtr)
{
	if( townPtr->nation_recno )
		return;

	//------ the skill of the spies of each nation, for each race -------//

	int  effSkill[MAX_NATION][MAX_RACE];
	char hasSpy[MAX_NATION];

	memset( effSkill, 0, sizeof(effSkill) );
	memset( hasSpy, 0, sizeof(hasSpy) );

	int spyCount = spy_array.size();

	for( int i=1 ; i<=spyCount ; i++ )
	{
		if( spy_array.is_deleted(i) )
			continue;

		Spy* spyPtr = spy_array[i];

		if( spyPtr->spy_place != SPY_TOWN || spyPtr->spy_place_para != townPtr->town_recno ||
			 spyPtr->action_mode != SPY_SOW_DISSENT || !spyPtr->true_nation_recno )
		{
			continue;
		}

		int n = spyPtr->true_nation_recno-1;

		hasSpy[n] = 1;

		for( int r=0 ; r<MAX_RACE ; r++ )
			effSkill[n][r] += spyPtr->race_id == r+1 ? spyPtr->spy_skill : spyPtr->spy_skill/4;
	}

	//------ process the propaganda of each nation -------//

	for( int n=0 ; n<MAX_NATION ; n++ )
	{
		if( !hasSpy[n] || nation_array.is_deleted(n+1) )
			continue;

		//------ decrease the resistance -------//

		float minResistance = has_linked_general_camp(townPtr, n+1) ? (float) 0 : (float) PROPAGANDA_MIN_RESISTANCE;
		float joinChance[MAX_RACE];
		float totalJoinChance = (float) 0;

		for( int r=0 ; r<MAX_RACE ; r++ )
		{
			joinChance[r] = (float) 0;

			int nonSpyPop = townPtr->race_pop_array[r] - townPtr->race_spy_count_array[r];

			if( nonSpyPop <= 0 || effSkill[n][r] == 0 )
				continue;

			float decValue = MIN( (float) PROPAGANDA_MAX_MONTH_DEC, (float) effSkill[n][r] / PROPAGANDA_SKILL_DIVIDER );
			float* resistancePtr = &townPtr->race_resistance_array[r][n];

			if( *resistancePtr > minResistance )
			{
				*resistancePtr -= decValue;

				if( *resistancePtr < minResistance )
					*resistancePtr = minResistance;
			}

			joinChance[r] = (float) effSkill[n][r] / PROPAGANDA_SKILL_DIVIDER * nonSpyPop / 30;
			totalJoinChance += joinChance[r];
		}

		//------ a villager joins the nation -------//

		int totalNonSpyPop = townPtr->population;

		for( int r=0 ; r<MAX_RACE ; r++ )
			totalNonSpyPop -= townPtr->race_spy_count_array[r];

		if( totalJoinChance <= 0 || totalNonSpyPop < 2 )		// the town never becomes empty
			continue;

		float chance = MIN( totalJoinChance, (float) PROPAGANDA_MAX_JOIN_CHANCE );

		if( misc.random(1000) >= (int) (chance * 10) )
			continue;

		//--- the race of the villager, by the chance of each race ---//

		float pick = (float) misc.random(1000) * totalJoinChance / 1000;
		int   raceId = 0;

		for( int r=0 ; r<MAX_RACE ; r++ )
		{
			if( joinChance[r] <= 0 )
				continue;

			raceId = r+1;

			if( pick < joinChance[r] )
				break;

			pick -= joinChance[r];
		}

		if( raceId )
			add_villager( townPtr, n+1, raceId, effSkill[n][raceId-1] );
	}
}
//----------- End of function ModSpy::process_propaganda ---------//


//--------- Begin of function ModSpy::add_villager --------//
//
// A villager of an independent town joins a nation through propaganda.
//
// <int> totalSkill - the skill of the spies of the nation for this race
//
void ModSpy::add_villager(Town* townPtr, int nationRecno, int raceId, int totalSkill)
{
	if( townPtr->recruitable_race_pop(raceId, 0) == 0 )		// 0-not spies
		return;

	//---- look for an empty location for the unit to stand ----//

	int         unitId = race_res[raceId]->basic_unit_id;
	SpriteInfo* spriteInfo = sprite_res[unit_res[unitId]->sprite_id];
	int         xLoc=townPtr->loc_x1, yLoc=townPtr->loc_y1;

	if( !world.locate_space( &xLoc, &yLoc, townPtr->loc_x2, townPtr->loc_y2, spriteInfo->loc_width, spriteInfo->loc_height ) )
		return;

	//---------- add the unit now -----------//

	int unitRecno = unit_array.add_unit( unitId, nationRecno, RANK_SOLDIER, 60, xLoc, yLoc );

	if( !unitRecno )
		return;

	Unit* unitPtr = unit_array[unitRecno];

	int combatLevel = townPtr->town_combat_level + misc.random(21) - 10;		// the combat level of the defenders of the town, -10 to +10

	combatLevel = MAX( CITIZEN_COMBAT_LEVEL, MIN(100, combatLevel) );

	unitPtr->set_combat_level( combatLevel );

	static char skillIdArray[] = { SKILL_CONSTRUCTION, SKILL_MINING, SKILL_MFT, SKILL_RESEARCH, SKILL_LEADING };

	unitPtr->skill.skill_id    = skillIdArray[ misc.random(sizeof(skillIdArray)) ];
	unitPtr->skill.skill_level = MIN( PROPAGANDA_MAX_JOIN_SKILL, 10 + misc.random( MIN(PROPAGANDA_MAX_JOIN_SKILL-10, totalSkill/2) + 1 ) );

	//-------- decrease the town's population ------//

	int townNameId = townPtr->town_name_id;

	townPtr->dec_pop(raceId, 0);		// 0-the unit doesn't have a job

	//-------- news ---------//

	if( nationRecno == nation_array.player_recno )
		news_array.villager_joined(unitRecno, townNameId);
}
//----------- End of function ModSpy::add_villager ---------//


//--------- Begin of function ModSpy::propaganda_influence_bonus --------//
//
// The bonus of the propaganda of a nation to the influence of its linked fort
// on an independent town: skill / 5 for each spy of the race of the general
// (skill / 20 for other races), at most PROPAGANDA_MAX_INFLUENCE.
//
// <int> raceId - the race of the general of the fort
//
int ModSpy::propaganda_influence_bonus(Town* townPtr, int nationRecno, int raceId)
{
	if( townPtr->nation_recno )
		return 0;

	int bonus=0, spyCount=spy_array.size();

	for( int i=1 ; i<=spyCount ; i++ )
	{
		if( spy_array.is_deleted(i) )
			continue;

		Spy* spyPtr = spy_array[i];

		if( spyPtr->spy_place == SPY_TOWN && spyPtr->spy_place_para == townPtr->town_recno &&
			 spyPtr->action_mode == SPY_SOW_DISSENT && spyPtr->true_nation_recno == nationRecno )
		{
			bonus += spyPtr->race_id == raceId ? spyPtr->spy_skill/5 : spyPtr->spy_skill/20;
		}
	}

	return MIN( bonus, PROPAGANDA_MAX_INFLUENCE );
}
//----------- End of function ModSpy::propaganda_influence_bonus ---------//


//--------- Begin of function ModSpy::process_accident --------//
//
// Called every day for each firm. In a firm with workers (not forts), an
// accident kills 1 or 2 workers, 1 in ACCIDENT_DAILY_CHANCE a day. Enemy
// spies in sabotage mode multiply the chance (1 spy x2, 2 spies x3, 3 or
// more x4).
//
// Each nation has at most 1 accident not caused by spies and
// ACCIDENT_MAX_SPY_PER_YEAR accidents caused by spies in 365 days.
//
void ModSpy::process_accident(Firm* firmPtr)
{
	if( !firmPtr->nation_recno || !firmPtr->worker_array || firmPtr->worker_count==0 ||
		 firmPtr->firm_id == FIRM_CAMP || firmPtr->firm_id == FIRM_BASE )		// not forts and seats of power
	{
		return;
	}

	//----- count the saboteurs and the workers that are not spies -----//

	int saboteurCount=0, spyNationRecno=0, normalWorkerCount=0;

	for( int i=0 ; i<firmPtr->worker_count ; i++ )
	{
		int spyRecno = firmPtr->worker_array[i].spy_recno;

		if( !spyRecno )
		{
			normalWorkerCount++;
			continue;
		}

		Spy* spyPtr = spy_array[spyRecno];

		if( spyPtr->action_mode == SPY_SABOTAGE && spyPtr->true_nation_recno != firmPtr->nation_recno )
		{
			saboteurCount++;

			if( !spyNationRecno )
				spyNationRecno = spyPtr->true_nation_recno;
		}
	}

	if( normalWorkerCount==0 )
		return;

	int multiplier = 1 + MIN( saboteurCount, ACCIDENT_MAX_SPY_MULTIPLIER-1 );

	if( misc.random(ACCIDENT_DAILY_CHANCE) >= multiplier )
		return;

	//---- was it caused by the spies? (their part of the chance) ----//

	int bySpy = saboteurCount > 0 && misc.random(multiplier) > 0;
	int n = firmPtr->nation_recno-1;
	int gameDate = info.game_date;

	if( !bySpy )
	{
		if( gameDate - last_accident_date[n] < 365 )
			return;

		last_accident_date[n] = gameDate;
	}
	else
	{
		int recentCount=0, oldestId=0;

		for( int i=0 ; i<ACCIDENT_MAX_SPY_PER_YEAR ; i++ )
		{
			if( gameDate - spy_accident_date[n][i] < 365 )
				recentCount++;

			if( spy_accident_date[n][i] < spy_accident_date[n][oldestId] )
				oldestId = i;
		}

		if( recentCount >= ACCIDENT_MAX_SPY_PER_YEAR )
			return;

		spy_accident_date[n][oldestId] = gameDate;
	}

	//------ 1 or 2 workers die -------//

	int deathCount = MIN( normalWorkerCount, 1+misc.random(2) );

	for( int k=0 ; k<deathCount ; k++ )
	{
		int pick = misc.random(normalWorkerCount-k);

		for( int i=0 ; i<firmPtr->worker_count ; i++ )
		{
			if( firmPtr->worker_array[i].spy_recno )
				continue;

			if( pick-- == 0 )
			{
				firmPtr->kill_worker(i+1);
				break;
			}
		}
	}

	//-------- news ---------//

	if( firmPtr->nation_recno == nation_array.player_recno )
		news_array.firm_accident(firmPtr->firm_recno, deathCount);

	if( bySpy && spyNationRecno == nation_array.player_recno )
		news_array.spy_accident(firmPtr->firm_recno, deathCount);
}
//----------- End of function ModSpy::process_accident ---------//


//--------- Begin of function ModSpy::gather_intelligence --------//
//
// Called every day. For each nation and target nation, if the nation has at
// least a spy gathering intelligence in a town or firm of the target nation,
// the days of gathering increase. The days needed for a report depend on the
// total skill of the spies gathering there: INTEL_MAX_GATHER_DAYS with
// INTEL_MIN_GATHER_SKILL or less, INTEL_MIN_GATHER_DAYS with
// INTEL_FULL_GATHER_SKILL or more. The spy with the highest skill makes the
// report.
//
void ModSpy::gather_intelligence()
{
	short bestSpyArray[MAX_NATION][MAX_NATION];
	int   totalSkillArray[MAX_NATION][MAX_NATION];

	memset( bestSpyArray, 0, sizeof(bestSpyArray) );
	memset( totalSkillArray, 0, sizeof(totalSkillArray) );

	int spyCount = spy_array.size();

	for( int i=1 ; i<=spyCount ; i++ )
	{
		if( spy_array.is_deleted(i) )
			continue;

		Spy* spyPtr = spy_array[i];

		if( spyPtr->action_mode != SPY_GATHER_INTEL || !spyPtr->true_nation_recno )
			continue;

		int targetNationRecno = spyPtr->spy_place_nation_recno();

		if( !targetNationRecno || targetNationRecno == spyPtr->true_nation_recno ||
			 nation_array.is_deleted(targetNationRecno) )
		{
			continue;
		}

		short* bestPtr = &bestSpyArray[spyPtr->true_nation_recno-1][targetNationRecno-1];

		totalSkillArray[spyPtr->true_nation_recno-1][targetNationRecno-1] += spyPtr->spy_skill;

		if( !*bestPtr || spy_array[*bestPtr]->spy_skill < spyPtr->spy_skill )
			*bestPtr = i;
	}

	for( int n=0 ; n<MAX_NATION ; n++ )
	{
		for( int t=0 ; t<MAX_NATION ; t++ )
		{
			if( !bestSpyArray[n][t] )
				continue;

			int totalSkill = MAX( INTEL_MIN_GATHER_SKILL, MIN(INTEL_FULL_GATHER_SKILL, totalSkillArray[n][t]) );
			int neededDays = INTEL_MAX_GATHER_DAYS - (totalSkill - INTEL_MIN_GATHER_SKILL) *
								  (INTEL_MAX_GATHER_DAYS - INTEL_MIN_GATHER_DAYS) / (INTEL_FULL_GATHER_SKILL - INTEL_MIN_GATHER_SKILL);

			if( ++intel_progress[n][t] >= neededDays )
			{
				intel_progress[n][t] = 0;
				make_report( spy_array[bestSpyArray[n][t]], t+1 );
			}
		}
	}
}
//----------- End of function ModSpy::gather_intelligence ---------//


//--------- Begin of static function set_range --------//
//
// The value is shown as a range, with an error of +/- (100-skill)/2 %. The
// real value is at a random point in the range.
//
static void set_range(IntelReport* reportPtr, int valueId, int value, int minValue, int maxValue)
{
	int errorPercent = (100 - reportPtr->spy_skill) / 2;
	int spread = (abs(value) * errorPercent + 50) / 100;

	if( spread == 0 )
	{
		reportPtr->value_low[valueId] = reportPtr->value_high[valueId] = value;
		return;
	}

	int low = value - misc.random(2*spread+1);
	int high = low + 2*spread;

	reportPtr->value_low[valueId]  = MAX( minValue, low );
	reportPtr->value_high[valueId] = MIN( maxValue, high );
}
//----------- End of static function set_range ---------//


//--------- Begin of function ModSpy::make_report --------//
//
void ModSpy::make_report(Spy* spyPtr, int targetNationRecno)
{
	Nation* nationPtr = nation_array[targetNationRecno];
	IntelReport report;

	memset( &report, 0, sizeof(report) );

	report.report_date         = info.game_date;
	report.spy_nation_recno    = spyPtr->true_nation_recno;
	report.target_nation_recno = targetNationRecno;
	strncpy( report.target_nation_name, nationPtr->nation_name(), INTEL_NAME_LEN );
	report.target_nation_color = nationPtr->nation_color;

	report.spy_race_id = spyPtr->race_id;
	report.spy_name_id = spyPtr->name_id;
	report.spy_skill   = spyPtr->spy_skill;

	for( int i=0 ; i<INTEL_SECTION_COUNT ; i++ )
		report.section_flag[i] = spyPtr->spy_skill >= intel_section_skill_array[i];

	//------- towns -------//

	int townCount=0, population=0;
	float loyaltySum = (float) 0;

	for( int i=town_array.size() ; i>0 ; i-- )
	{
		if( town_array.is_deleted(i) )
			continue;

		Town* townPtr = town_array[i];

		if( townPtr->nation_recno != targetNationRecno )
			continue;

		townCount++;
		population += townPtr->population;
		loyaltySum += (float) townPtr->average_loyalty() * townPtr->population;
	}

	set_range( &report, INTEL_TOWN_COUNT, townCount, 0, 0x7FFFFFFF );
	set_range( &report, INTEL_POPULATION, population, 0, 0x7FFFFFFF );
	set_range( &report, INTEL_LOYALTY, population ? (int) (loyaltySum / population) : 0, 0, 100 );

	//------- economy -------//

	set_range( &report, INTEL_CASH, (int) nationPtr->cash, -0x7FFFFFFF, 0x7FFFFFFF );
	set_range( &report, INTEL_FOOD, (int) nationPtr->food, -0x7FFFFFFF, 0x7FFFFFFF );
	set_range( &report, INTEL_INCOME, (int) nationPtr->income_365days(), 0, 0x7FFFFFFF );
	set_range( &report, INTEL_EXPENSE, (int) nationPtr->expense_365days(), 0, 0x7FFFFFFF );

	//------- firms: markets, forts and the soldiers in them -------//

	int marketCount=0, campCount=0, soldierCount=0;

	for( int i=firm_array.size() ; i>0 ; i-- )
	{
		if( firm_array.is_deleted(i) )
			continue;

		Firm* firmPtr = firm_array[i];

		if( firmPtr->nation_recno != targetNationRecno )
			continue;

		if( firmPtr->firm_id == FIRM_MARKET )
			marketCount++;

		else if( firmPtr->firm_id == FIRM_CAMP )
		{
			campCount++;

			for( int j=0 ; j<firmPtr->worker_count ; j++ )
			{
				if( firmPtr->worker_array[j].race_id )
					soldierCount++;
			}
		}
	}

	//------- units: caravans, mobile soldiers and generals -------//

	int caravanCount=0, generalCount=0;

	for( int i=unit_array.size() ; i>0 ; i-- )
	{
		if( unit_array.is_deleted(i) )
			continue;

		Unit* unitPtr = unit_array[i];

		if( unitPtr->nation_recno != targetNationRecno || unitPtr->hit_points <= 0 )
			continue;

		if( unitPtr->unit_id == UNIT_CARAVAN )
			caravanCount++;

		else if( unitPtr->race_id )
		{
			if( unitPtr->rank_id == RANK_GENERAL )
				generalCount++;

			else if( unitPtr->rank_id == RANK_SOLDIER && unitPtr->is_visible() &&
						(unitPtr->skill.skill_id == 0 || unitPtr->skill.skill_id == SKILL_LEADING) )
			{
				soldierCount++;
			}
		}
	}

	set_range( &report, INTEL_MARKET_COUNT, marketCount, 0, 0x7FFFFFFF );
	set_range( &report, INTEL_CARAVAN_COUNT, caravanCount, 0, 0x7FFFFFFF );

	//------- nation -------//

	set_range( &report, INTEL_REPUTATION, (int) nationPtr->reputation, -100, 100 );

	for( int i=1 ; i<=MAX_NATION ; i++ )
	{
		if( i==targetNationRecno || i > nation_array.size() || nation_array.is_deleted(i) )
		{
			report.relation_status[i-1] = -1;
			continue;
		}

		Nation* otherNation = nation_array[i];

		report.relation_status[i-1] = nationPtr->get_relation_status(i);
		strncpy( report.relation_nation_name[i-1], otherNation->nation_name(), INTEL_NAME_LEN );
		report.relation_nation_color[i-1] = otherNation->nation_color;
	}

	//------- military -------//

	set_range( &report, INTEL_SOLDIER_COUNT, soldierCount, 0, 0x7FFFFFFF );
	set_range( &report, INTEL_GENERAL_COUNT, generalCount, 0, 0x7FFFFFFF );
	set_range( &report, INTEL_WEAPON_COUNT, nationPtr->total_weapon_count, 0, 0x7FFFFFFF );
	set_range( &report, INTEL_SHIP_COUNT, nationPtr->total_ship_count, 0, 0x7FFFFFFF );
	set_range( &report, INTEL_CAMP_COUNT, campCount, 0, 0x7FFFFFFF );

	//------- technologies -------//

	int techId=0;

	for( int i=1 ; i<=tech_res.tech_count && techId<INTEL_MAX_TECH ; i++ )
		report.tech_level[techId++] = tech_res[i]->get_nation_tech_level(targetNationRecno);

	for( int i=1 ; i<=MAX_MOD_TECH && techId<INTEL_MAX_TECH ; i++ )
		report.tech_level[techId++] = mod_tech_res.get_level(i, targetNationRecno);

	//------- spies -------//

	int spyTotal=0, spyAbroad=0;

	for( int i=spy_array.size() ; i>0 ; i-- )
	{
		if( spy_array.is_deleted(i) )
			continue;

		Spy* otherSpy = spy_array[i];

		if( otherSpy->true_nation_recno != targetNationRecno )
			continue;

		spyTotal++;

		if( otherSpy->spy_place != SPY_MOBILE && otherSpy->spy_place_nation_recno() != targetNationRecno )
			spyAbroad++;
	}

	set_range( &report, INTEL_SPY_COUNT, spyTotal, 0, 0x7FFFFFFF );
	set_range( &report, INTEL_SPY_ABROAD_COUNT, spyAbroad, 0, 0x7FFFFFFF );

	//------- add the report -------//

	add_report( &report );

	if( report.spy_nation_recno == nation_array.player_recno )
		news_array.intel_report(targetNationRecno);
}
//----------- End of function ModSpy::make_report ---------//


//--------- Begin of function ModSpy::add_report --------//
//
void ModSpy::add_report(IntelReport* reportPtr)
{
	//--- delete the oldest report of the nation if it has too many, or the oldest report ---//

	int nationReportCount=0, oldestNationReportId=-1;

	for( int i=0 ; i<intel_report_count ; i++ )
	{
		if( intel_report_array[i].spy_nation_recno == reportPtr->spy_nation_recno )
		{
			if( oldestNationReportId < 0 )
				oldestNationReportId = i;

			nationReportCount++;
		}
	}

	if( nationReportCount >= MAX_INTEL_REPORT_PER_NATION )
		del_report( oldestNationReportId );

	else if( intel_report_count >= MAX_INTEL_REPORT )
		del_report( 0 );

	intel_report_array[intel_report_count++] = *reportPtr;		// in date order
}
//----------- End of function ModSpy::add_report ---------//


//--------- Begin of function ModSpy::del_report --------//
//
void ModSpy::del_report(int reportId)
{
	memmove( intel_report_array+reportId, intel_report_array+reportId+1,
				sizeof(IntelReport) * (intel_report_count-reportId-1) );

	intel_report_count--;
}
//----------- End of function ModSpy::del_report ---------//


//--------- Begin of function ModSpy::player_report_count --------//
//
int ModSpy::player_report_count()
{
	int count=0;

	for( int i=0 ; i<intel_report_count ; i++ )
	{
		if( intel_report_array[i].spy_nation_recno == info.viewing_nation_recno )
			count++;
	}

	return count;
}
//----------- End of function ModSpy::player_report_count ---------//


//--------- Begin of function ModSpy::get_player_report --------//
//
// <int> recNo - 1 is the latest report
//
IntelReport* ModSpy::get_player_report(int recNo)
{
	for( int i=intel_report_count-1 ; i>=0 ; i-- )
	{
		if( intel_report_array[i].spy_nation_recno == info.viewing_nation_recno )
		{
			if( --recNo == 0 )
				return intel_report_array+i;
		}
	}

	return NULL;
}
//----------- End of function ModSpy::get_player_report ---------//


//--------- Begin of function ModSpy::write_file --------//
//
int ModSpy::write_file(File* filePtr)
{
	if( !filePtr->file_write(last_accident_date, sizeof(last_accident_date)) )
		return 0;

	if( !filePtr->file_write(spy_accident_date, sizeof(spy_accident_date)) )
		return 0;

	if( !filePtr->file_write(intel_progress, sizeof(intel_progress)) )
		return 0;

	filePtr->file_put_short(intel_report_count);

	if( intel_report_count > 0 &&
		 !filePtr->file_write(intel_report_array, sizeof(IntelReport) * intel_report_count) )
	{
		return 0;
	}

	return 1;
}
//----------- End of function ModSpy::write_file ---------//


//--------- Begin of function ModSpy::read_file --------//
//
int ModSpy::read_file(File* filePtr)
{
	reset();

	if( !filePtr->file_read(last_accident_date, sizeof(last_accident_date)) )
		return 0;

	if( !filePtr->file_read(spy_accident_date, sizeof(spy_accident_date)) )
		return 0;

	if( !filePtr->file_read(intel_progress, sizeof(intel_progress)) )
		return 0;

	intel_report_count = filePtr->file_get_short();

	if( intel_report_count < 0 || intel_report_count > MAX_INTEL_REPORT )
	{
		intel_report_count = 0;
		return 0;
	}

	if( intel_report_count > 0 &&
		 !filePtr->file_read(intel_report_array, sizeof(IntelReport) * intel_report_count) )
	{
		return 0;
	}

	return 1;
}
//----------- End of function ModSpy::read_file ---------//
