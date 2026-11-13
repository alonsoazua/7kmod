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
//Description : names and versions of the mod (New Adversaries), displayed on
//              the main menu and used for the window title and the settings folder
//
// Update the versions together with 7kmod/CHANGELOG.md and
// 7kmod/CHANGELOG-gameplay.md when a version is published.

#ifndef __MODVERSION_H
#define __MODVERSION_H

//------- version of the 7kmod branch --------//

#define MOD_7KMOD_VERSION		"0.3.0"


//------- public version of New Adversaries --------//
//
// The players see a single version for New Adversaries (7kmod-gameplay) and
// New Adversaries Classic (7kmod). The branch versions above and below are
// kept in the changelogs.

#define MOD_PUBLIC_VERSION		"0.3.0"


//------- version of the 7kmod-gameplay branch --------//
//
// The 7kmod-gameplay branch defines MOD_GAMEPLAY_VERSION in this section.
// It is kept apart from the 7kmod version above so that changing either
// of them doesn't conflict when 7kmod-gameplay is rebased on 7kmod.




//------- name, settings folder and the text displayed on the main menu --------//
//
// MOD_GAME_NAME is also the window title. MOD_CONFIG_NAME is the folder of the
// saved games and settings, in %APPDATA%\7kfans.com\ on Windows.

#ifdef MOD_GAMEPLAY_VERSION
	#define MOD_GAME_NAME			"New Adversaries"
	#define MOD_CONFIG_NAME		"7k-newAdversaries"
#else
	#define MOD_GAME_NAME			"New Adversaries Classic"
	#define MOD_CONFIG_NAME		"7k-newAdversaries-Classic"
#endif

#define MOD_VERSION_TEXT		MOD_GAME_NAME " " MOD_PUBLIC_VERSION

#endif
