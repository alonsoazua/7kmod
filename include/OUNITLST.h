/*
 * Seven Kingdoms: Ancient Adversaries
 *
 * Copyright 2026 7kmod
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

//Filename    : OUNITLST.H
//Description : 7kmod - list of the selected units in the unit panel
//
// With several units selected, the ">" button at the right of the name bar
// of the unit panel replaces the rest of the panel with a list of the
// selected units, like the list of soldiers of a fort. "<" goes back.
//
// When a selection box has no units of the player but has units of other
// kingdoms (or wild Fryhtans, rebels...), the most important of them is
// selected and the others are kept in an "inspect" list, which the unit
// panel shows in the same way (without their combat levels). The game only
// allows selecting several units of the player, so this list is kept apart
// from the selection.

#ifndef __OUNITLST_H
#define __OUNITLST_H

class Unit;

//------- the maximum number of units in the list -------//

#define MAX_UNIT_LIST		100

//------- functions -------//

int  unit_list_units(Unit* mainUnit, short* recnoArray);		// the units of the list of this unit panel, sorted
int  unit_list_is_open();
void unit_list_close();

void unit_list_disp_toggle(int x1, int y1, int refreshFlag);	// the ">" / "<" button
void unit_list_reset_toggle();									// when the button is not displayed
int  unit_list_detect_toggle();

void unit_list_disp(Unit* mainUnit, int dispY1, int refreshFlag);
int  unit_list_detect(Unit* mainUnit, int dispY1);

void unit_list_set_inspect(short* recnoArray, int unitCount);	// units of other kingdoms in a selection box
void unit_list_clear_inspect();

#endif
