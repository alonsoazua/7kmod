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

#ifndef __OGRPSEL_H
#define __OGRPSEL_H

#define MAX_SELECT_GROUP_NUM	9

//------------ class GroupSelect ----------//
class GroupSelect
{
	public:

	public:
		GroupSelect();
		~GroupSelect();

		void	init();
		void	deinit();

		void	group_units(int groupNum);
		int	select_grouped_units(int groupNum);		// 7kmod: returns the unit shown in the panel, 0 if the group has none
		void	key_select_group(int groupNum);		// 7kmod: Alt+number; the same number again soon also centers the map on the group

		//--- 7kmod: the buttons of the groups, at the right of the news, and the group of the selection ---//

		void	disp_buttons();
		int	detect_buttons();
		int	selected_group();		// the group that is exactly the selected units, 0 if none
};

extern GroupSelect group_select;

#endif