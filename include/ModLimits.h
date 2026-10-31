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

//Filename    : ModLimits.h
//Description : 7k-mod-IA: population limits of the kingdoms

#ifndef __MODLIMITS_H
#define __MODLIMITS_H

//------------------------------------------------------------------//
//
// 7k-mod-IA: limits per kingdom, set in config.txt
// (nation_max_town_population, nation_max_military_units; 0 - no limit)
// and saved in the game.
//
// - town population: the people who live in the towns of the kingdom. At
//   the limit there are no new births in its towns.
// - military units: soldiers and generals (leadership, not the king or
//   spies), siege weapons, warships (Caravel, Galleon) and the Fryhtans of
//   the kingdom, also the ones being trained or built. At the limit the
//   kingdom can't train Leadership units (nor captains), build weapons or
//   warships, invoke Fryhtans or hire mercenaries with leadership.
//
// Units won by capture, surrender or conversion may go over the limit.
//
//------------------------------------------------------------------//

int mod_is_military_unit_id(int unitId);			// weapons, warships, Fryhtans (not people: their skill decides)
int mod_military_unit_count(int nationRecno);
int mod_military_limit_reached(int nationRecno);
int mod_town_population_limit_reached(int nationRecno);
int mod_check_military_limit(int nationRecno);		// 1 - at the limit, with a message for the player
class Button3D;
void mod_military_limit_help(int nationRecno, Button3D& button, const char* helpCode, const char* helpTitle);	// the help of a button disabled by the limit

#endif
