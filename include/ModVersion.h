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

//Filename    : ModVersion.h
//Description : versions of 7kmod, displayed on the main menu
//
// Update the versions together with 7kmod/CHANGELOG.md and
// 7kmod/CHANGELOG-gameplay.md when a version is published.

#ifndef __MODVERSION_H
#define __MODVERSION_H

//------- version of the 7kmod branch --------//

#define MOD_7KMOD_VERSION		"0.1.0"


//------- version of the 7kmod-gameplay branch --------//
//
// The 7kmod-gameplay branch defines MOD_GAMEPLAY_VERSION in this section.
// It is kept apart from the 7kmod version above so that changing either
// of them doesn't conflict when 7kmod-gameplay is rebased on 7kmod.

#define MOD_GAMEPLAY_VERSION	"0.1.0"




//------- the text displayed on the main menu --------//

#ifdef MOD_GAMEPLAY_VERSION
	#define MOD_VERSION_TEXT		"7kmod-gameplay " MOD_GAMEPLAY_VERSION " (7kmod " MOD_7KMOD_VERSION ")"
#else
	#define MOD_VERSION_TEXT		"7kmod " MOD_7KMOD_VERSION
#endif

#endif
