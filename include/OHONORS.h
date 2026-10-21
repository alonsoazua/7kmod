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

//Filename    : OHONORS.H
//Description : 7kmod - honors of the generals (rewards for their
//              contribution) and loyalty breakdowns

#ifndef __OHONORS_H
#define __OHONORS_H

class Unit;
class Worker;
class String;

//---- the penalty of an unrewarded general is at most this amount ----//

#define MAX_HONORS_PENALTY		50

//------- functions for the honors of the generals ------//

int  honors_unrewarded(Unit* unitPtr);		// contribution not covered by rewards
int  honors_penalty(Unit* unitPtr);			// loyalty penalty for the unrewarded contribution
int  honors_reward_count(Unit* unitPtr);	// rewards needed to remove the penalty
int  honors_cost(Unit* unitPtr);				// cost of these rewards
int  pay_honors(Unit* unitPtr);				// pay the honors, return the no. of rewards given
int  is_honors_general(Unit* unitPtr);		// whether the unit is a general of the player
void check_honors_request(Unit* unitPtr, int justPromoted=0);	// add the news when a general reaches the maximum penalty
void remove_honors_request(Unit* unitPtr);	// remove the news (the general died, changed nation...)

//------- functions for the loyalty breakdowns ------//

int  unit_loyalty_breakdown(Unit* unitPtr, String& title, String& detail);
int  worker_loyalty_breakdown(Worker* workerPtr, int firmRecno, String& title, String& detail);
void set_unit_loyalty_help(Unit* unitPtr, int x1, int y1, int x2, int y2);
void set_worker_loyalty_help(Worker* workerPtr, int firmRecno, int x1, int y1, int x2, int y2);

//------- Define class HonorsRequest --------//
//
// The request for honors of a general being answered. It is displayed in
// the kingdoms report (F1), like the replies to diplomatic messages, in the
// hidden report mode NATION_REPORT_HONORS. The game continues meanwhile.
//

class HonorsRequest
{
public:
	char	active_flag;
	short	unit_recno;
	short	unit_name_id;
	char	save_view_mode;		// the view to go back to after answering
	char	cur_choice_id;			// the choice under the mouse cursor

public:
	HonorsRequest()		{ active_flag=0; unit_recno=0; unit_name_id=0; save_view_mode=0; cur_choice_id=0; }

	int	is_active()		{ return active_flag; }
	void	open(int unitRecno, int unitNameId);
	void	close();
	int	is_valid();

	void	disp_report(int x1, int y1, int x2, int lineHeight);
	void	detect_report(int x1, int y1, int x2, int lineHeight);
};

extern HonorsRequest honors_request;

//------------------------------------------//

#endif
