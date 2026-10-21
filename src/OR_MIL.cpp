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

//Filename    : OINFOMIL.CPP
//Description : Economy information screen

#include <OVGA.h>
#include <OFONT.h>
#include <OSPY.h>
#include <OIMGRES.h>
#include <OUNITRES.h>
#include <ORACERES.h>
#include <OVBROWIF.h>
#include <OBUTTON.h>
#include <OF_CAMP.h>
#include <ONATION.h>
#include <OUNIT.h>
#include <OINFO.h>
#include <OHONORS.h>
#include <OIMGRES.h>
#include <OMOUSE.h>
#include <OWORLD.h>
#include <OSE.h>
#include "gettext.h"

//------------- Define coordinations -----------//

enum { TROOP_BROWSE_X1 = ZOOM_X1+6,
		 TROOP_BROWSE_Y1 = ZOOM_Y1+6,
		 TROOP_BROWSE_X2 = ZOOM_X2-6,
		 TROOP_BROWSE_Y2 = TROOP_BROWSE_Y1+240
	  };

enum { UNIT_BROWSE_X1 = ZOOM_X1+6,
		 UNIT_BROWSE_Y1 = TROOP_BROWSE_Y2+6,
		 UNIT_BROWSE_X2 = ZOOM_X2-6,
		 UNIT_BROWSE_Y2 = ZOOM_Y2-6,
	  };

//------- 7kmod: tabs of the lower panel --------//

enum { MIL_TAB_UNITS=0, MIL_TAB_CONTRIBUTIONS };

enum { MIL_TAB_Y1 = UNIT_BROWSE_Y1,
		 MIL_TAB_Y2 = UNIT_BROWSE_Y1+20,
		 MIL_TAB_WIDTH = 120,
		 MIL_LIST_Y1 = MIL_TAB_Y2+2 };		// the lower panel starts below the tabs

enum { PAY_BUTTON_Y1 = UNIT_BROWSE_Y2-21,
		 PAY_BUTTON_Y2 = UNIT_BROWSE_Y2,
		 PAY_SELECTED_X1 = UNIT_BROWSE_X1,
		 PAY_SELECTED_X2 = UNIT_BROWSE_X1+200,
		 PAY_ALL_X1 = UNIT_BROWSE_X1+206,
		 PAY_ALL_X2 = UNIT_BROWSE_X1+406 };

//----------- Define static variables ----------//

static VBrowseIF browse_troop;
static VBrowseIF browse_unit;
static VBrowseIF browse_contrib;		// 7kmod

static char  mil_report_tab = MIL_TAB_UNITS;		// 7kmod
static short browse_contrib_recno = 1;

#define MAX_CONTRIB_GENERAL	500
static short contrib_unit_array[MAX_CONTRIB_GENERAL];		// 7kmod: the generals of the Contributions tab, sorted by risk
static int   contrib_unit_count;

//----------- Define static functions ----------//

static void put_troop_rec(int recNo, int x, int y, int refreshFlag);
static void put_unit_rec(int recNo, int x, int y, int refreshFlag);
static int  troop_filter(int recNo=0);
static int  unit_filter(int recNo=0);
static void	disp_troop_total();
static void disp_unit_total();
static void put_heading(char justify, int x1, int y1, int x2, int y2, const char *textPtr);
static void disp_tabs();
static int  detect_tabs();
static void build_contrib_list();
static int  contrib_filter(int recNo=0);
static void put_contrib_rec(int recNo, int x, int y, int refreshFlag);
static void disp_contrib_buttons();
static int  detect_contrib_buttons();

#define J_L Font::LEFT_JUSTIFY
#define J_C Font::CENTER_JUSTIFY
#define J_R Font::RIGHT_JUSTIFY

//--------- Begin of function Info::disp_military ---------//
//
void Info::disp_military(int refreshFlag)
{
	//------- Display the Troop report -------//

	int x=TROOP_BROWSE_X1+9;
	int y=TROOP_BROWSE_Y1+4;

	vga_back.d3_panel_up(TROOP_BROWSE_X1, TROOP_BROWSE_Y1, TROOP_BROWSE_X2, TROOP_BROWSE_Y1+33 );

	#if(defined(FRENCH))
		font_san.put( x	 , y+7 , "Commander" );
		font_san.put( x+170, y+7,  "Leadership" );
		font_san.put( x+275, y+7,  "Loyalty" );
		font_san.put( x+350, y,    "Points" );
		font_san.put( x+350, y+13, "de vie" );
		font_san.put( x+410, y+7,  "Garnison" );
		font_san.put( x+480, y+7, "Status" );
	#elif(defined(GERMAN))
		font_san.put( x	 , y+7 , "Commander" );
		font_san.put( x+210, y+7 , "Leadership" );
		font_san.put( x+275, y+7 , "Loyalty" );
		font_san.put( x+342, y+7 , "Hit Points" );
		font_san.put( x+406, y   , "Commanded" );
		font_san.put( x+415, y+13, "Soldiers" );
		font_san.put( x+485, y+7 , "Status" );
	#else
		put_heading( J_L, x    , y, x+200, y+29, _("Commander") );
		put_heading( J_C, x+200, y, x+285, y+29, _("Leadership") );
		put_heading( J_L, x+285, y, x+342, y+29, _("Loyalty") );
		put_heading( J_C, x+342, y, x+406, y+29, _("Hit Points") );
		put_heading( J_C, x+406, y, x+490, y+29, _("Commanded Soldiers") );
		put_heading( J_R, x+490, y, TROOP_BROWSE_X2-20, y+29, _("Status") );
	#endif

	if( refreshFlag == INFO_REPAINT )
	{
		browse_troop.init( TROOP_BROWSE_X1, TROOP_BROWSE_Y1+35, TROOP_BROWSE_X2, TROOP_BROWSE_Y2-20,
								 0, 22, troop_filter(), put_troop_rec, 1 );

		browse_troop.open(browse_troop_recno);
	}
	else
	{
		browse_troop.paint();
		browse_troop.open(browse_troop_recno, troop_filter());
	}

	disp_troop_total();

	//------- 7kmod: tabs of the lower panel -------//

	static char lastTab = -1;

	if( lastTab != mil_report_tab )		// initialize the browser of the new tab
	{
		lastTab = mil_report_tab;
		refreshFlag = INFO_REPAINT;
	}

	disp_tabs();

	//------- Display the unit report -------//

	if( mil_report_tab == MIL_TAB_UNITS )
	{
		x=UNIT_BROWSE_X1+9;
		y=MIL_LIST_Y1+4;

		vga_back.d3_panel_up(UNIT_BROWSE_X1, MIL_LIST_Y1, UNIT_BROWSE_X2, MIL_LIST_Y1+20 );

		font_san.put( x	 , y, _("Unit Type") );
		font_san.put( x+300, y, _("No. of Units") );

		if( refreshFlag == INFO_REPAINT )
		{
			browse_unit.init( UNIT_BROWSE_X1, MIL_LIST_Y1+22, UNIT_BROWSE_X2, UNIT_BROWSE_Y2-20,
									0, 16, unit_filter(), put_unit_rec, 1 );

			browse_unit.open(browse_unit_recno);
		}
		else
		{
			browse_unit.paint();
			browse_unit.open(browse_unit_recno, unit_filter());
		}

		disp_unit_total();
	}

	//------- 7kmod: Display the contributions of the generals -------//

	else
	{
		build_contrib_list();

		x=UNIT_BROWSE_X1+9;
		y=MIL_LIST_Y1+4;

		vga_back.d3_panel_up(UNIT_BROWSE_X1, MIL_LIST_Y1, UNIT_BROWSE_X2, MIL_LIST_Y1+20 );

		font_san.put( x	 , y, _("Commander") );
		font_san.put( x+170, y, _("Loyalty") );
		font_san.put( x+250, y, _("Contrib.") );
		font_san.put( x+315, y, _("Status") );
		font_san.put( x+410, y, _("Penalty") );
		font_san.put( x+475, y, _("Honors") );

		if( refreshFlag == INFO_REPAINT )
		{
			browse_contrib.init( UNIT_BROWSE_X1, MIL_LIST_Y1+22, UNIT_BROWSE_X2, PAY_BUTTON_Y1-3,
										0, 18, contrib_filter(), put_contrib_rec, 1 );

			browse_contrib.open(browse_contrib_recno);
		}
		else
		{
			browse_contrib.paint();
			browse_contrib.open(browse_contrib_recno, contrib_filter());
		}

		disp_contrib_buttons();
	}
}
//----------- End of function Info::disp_military -----------//


//--------- Begin of function Info::detect_military ---------//
//
void Info::detect_military()
{
	//------- detect the troop report -------//

	if( browse_troop.detect() )
	{
		browse_troop_recno = browse_troop.recno();

		if( browse_troop.double_click )
		{
			Unit* unitPtr = unit_array[ troop_filter(browse_troop_recno) ];
			short xLoc, yLoc;

			if( unitPtr->get_cur_loc(xLoc, yLoc) )
			{
				world.go_loc(xLoc, yLoc, 1);
			}
		}
	}

	//------- 7kmod: detect the tabs -------//

	if( detect_tabs() )
		return;

	//------- detect the unit report -------//

	if( mil_report_tab == MIL_TAB_UNITS )
	{
		if( browse_unit.detect() )
		{
			browse_unit_recno = browse_unit.recno();
		}
	}

	//------- 7kmod: detect the contributions -------//

	else
	{
		build_contrib_list();

		if( browse_contrib.detect() )
		{
			browse_contrib_recno = browse_contrib.recno();

			if( browse_contrib.double_click && contrib_filter() > 0 )
			{
				Unit* unitPtr = unit_array[ contrib_filter(browse_contrib_recno) ];
				short xLoc, yLoc;

				if( unitPtr->get_cur_loc(xLoc, yLoc) )
					world.go_loc(xLoc, yLoc, 1);
			}
		}

		detect_contrib_buttons();
	}
}
//----------- End of function Info::detect_military -----------//


//-------- Begin of static function disp_troop_total --------//

static void disp_troop_total()
{
	//---- count the total no. of soldiers ----//

	Unit* unitPtr;
	int   totalSoldier=0, totalCommandedSoldier=0;

	int i;
	for( i=unit_array.size() ; i>0 ; i-- )
	{
		if( unit_array.is_deleted(i) )
			continue;

		unitPtr = unit_array[i];

		if( unitPtr->nation_recno != info.viewing_nation_recno )
			continue;

		if( unitPtr->skill.skill_id != SKILL_LEADING )
			continue;

		if( unitPtr->rank_id > RANK_SOLDIER )
			totalCommandedSoldier += unitPtr->commanded_soldier_count();
		else if( !unitPtr->leader_unit_recno )
			totalSoldier++;
	}
	totalSoldier += totalCommandedSoldier;

	Nation* viewingNation = nation_array[info.viewing_nation_recno];

	for( i=viewingNation->ai_camp_count-1 ; i>=0 ; i-- )
	{
		totalSoldier += firm_array[ viewingNation->ai_camp_array[i] ]->worker_count;
	}

	//--------- paint the area ----------//

	int x=TROOP_BROWSE_X1+9;
	int y=TROOP_BROWSE_Y2-16;

	vga_back.d3_panel_up(TROOP_BROWSE_X1, TROOP_BROWSE_Y2-18, TROOP_BROWSE_X2, TROOP_BROWSE_Y2 );

	//------ display commander count ------//

	String str;

	snprintf( str, MAX_STR_LEN+1, _("Total Commanders: %s"), misc.format(browse_troop.total_rec()) );

	font_san.put( x, y, str );

	//------ display soldiers under command count ------//

	snprintf( str, MAX_STR_LEN+1, _("Total Soldiers Under Command: %s"), misc.format(totalCommandedSoldier) );

	font_san.put( x+160, y, str );

	//------ display soldiers count ------//

	snprintf( str, MAX_STR_LEN+1, _("Total Soldiers: %s"), misc.format(totalSoldier) );

#if(defined(FRENCH))
	font_san.put( x+396, y, str );
#else
	font_san.put( x+410, y, str );
#endif
}
//----------- End of static function disp_troop_total -----------//


//-------- Begin of static function disp_unit_total --------//

static void disp_unit_total()
{
	int totalUnitCount=0, totalUnitCost=0;

	UnitInfo* unitInfo;

	for( int unitId=1 ; unitId<=MAX_UNIT_TYPE ; unitId++ )
	{
		unitInfo = unit_res[unitId];

		int unitCount    = unitInfo->nation_unit_count_array[info.viewing_nation_recno-1];
		int generalCount = unitInfo->nation_general_count_array[info.viewing_nation_recno-1];

		totalUnitCount += unitCount;

		if( unitInfo->race_id )
		{
			totalUnitCost += generalCount * GENERAL_YEAR_SALARY;
			totalUnitCost += (unitCount-generalCount) * SOLDIER_YEAR_SALARY;
		}
		else
			totalUnitCost += unitCount * unitInfo->year_cost;
	}


	//--------- paint the area ----------//

	int x=UNIT_BROWSE_X1+9;
	int y=UNIT_BROWSE_Y2-16;

	vga_back.d3_panel_up(UNIT_BROWSE_X1, UNIT_BROWSE_Y2-18, UNIT_BROWSE_X2, UNIT_BROWSE_Y2 );

	String str;

	snprintf( str, MAX_STR_LEN+1, _("Total Units: %s"), misc.format(totalUnitCount) );

	font_san.put( x, y, str );
}
//----------- End of static function disp_unit_total -----------//


//-------- Begin of static function troop_filter --------//
//
// This function has dual purpose :
//
// 1. when <int> recNo is not given :
//    - return the total no. of firms of this nation
//
// 2. when <int> recNo is given :
//    - return the firm recno in firm_array of the given recno.
//
static int troop_filter(int recNo)
{
	int   i, unitCount=0, totalUnit=unit_array.size();
	Unit* unitPtr;

	for( i=1 ; i<=totalUnit ; i++ )
	{
		if( unit_array.is_deleted(i) )
			continue;

		unitPtr = unit_array[i];

		if( unitPtr->nation_recno == info.viewing_nation_recno &&
			 (unitPtr->rank_id==RANK_GENERAL || unitPtr->rank_id==RANK_KING) )
		{
			unitCount++;
		}

		if( recNo && unitCount==recNo )
			return i;
	}

	err_when( recNo );   // the recNo is not found, it is out of range

	return unitCount;
}
//----------- End of static function troop_filter -----------//


//-------- Begin of static function put_troop_rec --------//
//
static void put_troop_rec(int recNo, int x, int y, int refreshFlag)
{
	int   unitRecno = troop_filter(recNo);
	Unit* unitPtr   = unit_array[unitRecno];

	//---------- display bitmap ----------//

	x+=3;
	y+=3;

	vga_back.put_bitmap( x, y-2, unit_res[unitPtr->unit_id]->get_small_icon_ptr(unitPtr->rank_id) );

	//-------- display name & team size -------//

	y+=2;

	font_san.put( x+28 , y, unitPtr->unit_name(), 0, 238 );
	font_san.put( x+240, y, unitPtr->skill.get_skill(SKILL_LEADING) );

	if( unitPtr->rank_id != RANK_KING )
		font_san.put( x+295, y, unitPtr->loyalty );

	//--------- display hit points -----------//

	String str;

	str  = (int) unitPtr->hit_points;
	str += "/";
	str += unitPtr->max_hit_points;

	font_san.put( x+352, y, str );

	//---- display the no. of soldiers led by this general ---//

	font_san.put( x+445, y, unitPtr->commanded_soldier_count() );

	//---- display the status of the general ----//

	const char* statusStr;

	if( unitPtr->unit_mode == UNIT_MODE_OVERSEE )
	{
		int firmId = firm_array[ unitPtr->unit_mode_para ]->firm_id;

		if( firmId == FIRM_CAMP )
			statusStr = _("In Fort");

		else if( firmId == FIRM_BASE )
			statusStr = _("In Seat");

		else
			err_here();
	}
	else if( unitPtr->unit_mode == UNIT_MODE_ON_SHIP )
	{
		statusStr = _("On Ship");
	}
	else
	{
		statusStr = _("Mobile");
	}

#if(defined(FRENCH))
	font_san.put( x+470, y, statusStr );
#else
	// German and US
	font_san.put( x+486, y, statusStr );
#endif
}
//----------- End of static function put_troop_rec -----------//


//-------- Begin of static function unit_filter --------//
//
// This function has dual purpose :
//
// 1. when <int> recNo is not given :
//    - return the total no. of firms of this nation
//
// 2. when <int> recNo is given :
//    - return the firm recno in firm_array of the given recno.
//
static int unit_filter(int recNo)
{
	int 		 unitTypeCount=0;
	UnitInfo* unitInfo;

	//------- count human units -------//

	Nation* nationPtr = nation_array[info.viewing_nation_recno];

	unitTypeCount++;

	if( recNo && unitTypeCount==recNo )
		return -1;

	//------- count non-human unit types -------//

	for( int unitId=1 ; unitId<=MAX_UNIT_TYPE ; unitId++ )
	{
		unitInfo = unit_res[unitId];

		//---- if this is not a human unit -----//

		if( !unitInfo->race_id &&
			 unitInfo->nation_unit_count_array[info.viewing_nation_recno-1] > 0 )
		{
			unitTypeCount++;
		}

		if( recNo && unitTypeCount==recNo )
			return unitId;
	}

	err_when( recNo );   // the recNo is not found, it is out of range

	return unitTypeCount;
}
//----------- End of static function unit_filter -----------//


//-------- Begin of static function put_unit_rec --------//
//
static void put_unit_rec(int recNo, int x, int y, int refreshFlag)
{
	int   	 unitId = unit_filter(recNo);
	int   	 unitCount;
	const char* 	 str;
	UnitInfo* unitInfo;
	Nation* nationPtr = nation_array[info.viewing_nation_recno];
	int rc = unit_filter(recNo);

	switch( rc )
	{
		//------- count human units -------//

		case -1:
			str		  = _("Human General");
			unitCount  = nationPtr->total_general_count;
			break;

		default:
			unitInfo   = unit_res[rc];
			str 		  = _(unitInfo->name);
			unitCount  = unitInfo->nation_unit_count_array[ info.viewing_nation_recno-1 ];
	}

	//---------- display info --------//

	x+=3;
	y+=3;

	font_san.put( x   , y, str );
	font_san.put( x+320, y, misc.format(unitCount,1) );
}
//----------- End of static function put_unit_rec -----------//


//-------- Begin of static function put_heading --------//
//
static void put_heading(char justify, int x1, int y1, int x2, int y2, const char *textPtr)
{
	int dispLines=0;
	int totalLines=0;
	font_san.count_line(x1,y1,x2,y2,textPtr,0,dispLines,totalLines);
	if( dispLines > 1 )
		font_san.put_paragraph(x1,y1,x2,y2,textPtr,0,1,1,justify);
	else if( y1+7<y2 )
		font_san.put_paragraph(x1,y1+7,x2,y2,textPtr,0,1,1,justify);
}
//----------- End of static function put_heading -----------//


//-------- Begin of static function disp_tabs --------//
//
// 7kmod: the tabs of the lower panel: Units and Contributions.
//
static void disp_tabs()
{
	static const char* tabStr[] = { N_("Units"), N_("Contributions") };

	int x=UNIT_BROWSE_X1;

	for( int i=0 ; i<2 ; i++, x+=MIL_TAB_WIDTH )
	{
		if( mil_report_tab == i )
			vga_back.d3_panel_down( x, MIL_TAB_Y1, x+MIL_TAB_WIDTH-1, MIL_TAB_Y2 );
		else
			vga_back.d3_panel_up( x, MIL_TAB_Y1, x+MIL_TAB_WIDTH-1, MIL_TAB_Y2 );

		font_san.center_put( x, MIL_TAB_Y1, x+MIL_TAB_WIDTH-1, MIL_TAB_Y2, _(tabStr[i]) );
	}
}
//----------- End of static function disp_tabs -----------//


//-------- Begin of static function detect_tabs --------//
//
static int detect_tabs()
{
	int x=UNIT_BROWSE_X1;

	for( int i=0 ; i<2 ; i++, x+=MIL_TAB_WIDTH )
	{
		if( mouse.single_click( x, MIL_TAB_Y1, x+MIL_TAB_WIDTH-1, MIL_TAB_Y2 ) )
		{
			mil_report_tab = i;
			return 1;
		}
	}

	return 0;
}
//----------- End of static function detect_tabs -----------//


//-------- Begin of static function contrib_risk --------//
//
// 7kmod: how likely the general is to betray the nation, for sorting.
//
static int contrib_risk(Unit* unitPtr)
{
	return (100-unitPtr->target_loyalty)*100 + honors_penalty(unitPtr);
}
//----------- End of static function contrib_risk -----------//


//-------- Begin of static function build_contrib_list --------//
//
// 7kmod: build the list of the generals of the viewing nation, sorted by
// risk: the lowest target loyalty first.
//
static void build_contrib_list()
{
	contrib_unit_count = 0;

	for( int i=unit_array.size() ; i>0 && contrib_unit_count<MAX_CONTRIB_GENERAL ; i-- )
	{
		if( unit_array.is_deleted(i) )
			continue;

		Unit* unitPtr = unit_array[i];

		if( unitPtr->nation_recno == info.viewing_nation_recno &&
			 unitPtr->rank_id == RANK_GENERAL && unitPtr->race_id )
		{
			contrib_unit_array[contrib_unit_count++] = i;
		}
	}

	//------ sort by risk (insertion sort, the list is short) ------//

	for( int i=1 ; i<contrib_unit_count ; i++ )
	{
		short unitRecno = contrib_unit_array[i];
		int   risk = contrib_risk(unit_array[unitRecno]);
		int   j = i-1;

		while( j>=0 && contrib_risk(unit_array[contrib_unit_array[j]]) < risk )
		{
			contrib_unit_array[j+1] = contrib_unit_array[j];
			j--;
		}

		contrib_unit_array[j+1] = unitRecno;
	}

	if( browse_contrib_recno > contrib_unit_count )
		browse_contrib_recno = MAX(1, contrib_unit_count);
}
//----------- End of static function build_contrib_list -----------//


//-------- Begin of static function contrib_filter --------//
//
// 7kmod: without recNo, return the no. of generals in the list; with recNo,
// return the unit recno of that general.
//
static int contrib_filter(int recNo)
{
	if( !recNo )
		return contrib_unit_count;

	err_when( recNo < 1 || recNo > contrib_unit_count );

	return contrib_unit_array[recNo-1];
}
//----------- End of static function contrib_filter -----------//


//-------- Begin of static function put_contrib_rec --------//
//
// 7kmod: one general in the Contributions tab.
//
static void put_contrib_rec(int recNo, int x, int y, int refreshFlag)
{
	Unit* unitPtr = unit_array[ contrib_filter(recNo) ];

	x+=3;
	y+=3;

	//------- name -------//

	font_san.put( x, y, unitPtr->unit_name(), 0, x+160 );

	//------- loyalty and its target -------//

	int tx = font_san.put( x+167, y, unitPtr->loyalty );

	if( unitPtr->target_loyalty != unitPtr->loyalty )
	{
		image_icon.put_back( tx+3, y+1, unitPtr->target_loyalty > unitPtr->loyalty ? (char*)"ARROWUP" : (char*)"ARROWDWN" );
		font_san.put( tx+13, y, unitPtr->target_loyalty );
	}

	//------- contribution -------//

	font_san.put( x+247, y, unitPtr->nation_contribution );

	//------- status -------//

	int penalty = honors_penalty(unitPtr);
	const char* statusStr;

	if( unitPtr->target_loyalty < UNIT_BETRAY_LOYALTY )
		statusStr = _("At risk!");
	else if( penalty > 0 )
		statusStr = _("Unrewarded");
	else
		statusStr = _("Content");

	font_san.put( x+312, y, statusStr );

	//------- penalty and honors -------//

	String str;

	if( penalty > 0 )
	{
		str  = "-";
		str += penalty;
		font_san.put( x+410, y, str );

		str  = "$";
		str += honors_cost(unitPtr);
		font_san.put( x+472, y, str );
	}
	else
	{
		font_san.put( x+410, y, "0" );
		font_san.put( x+472, y, "-" );
	}
}
//----------- End of static function put_contrib_rec -----------//


//-------- Begin of static function contrib_selected_cost --------//
//
static int contrib_selected_cost()
{
	if( contrib_unit_count == 0 || browse_contrib_recno < 1 || browse_contrib_recno > contrib_unit_count )
		return 0;

	return honors_cost( unit_array[ contrib_filter(browse_contrib_recno) ] );
}
//----------- End of static function contrib_selected_cost -----------//


//-------- Begin of static function contrib_total_cost --------//
//
static int contrib_total_cost()
{
	int totalCost=0;

	for( int i=0 ; i<contrib_unit_count ; i++ )
		totalCost += honors_cost( unit_array[contrib_unit_array[i]] );

	return totalCost;
}
//----------- End of static function contrib_total_cost -----------//


//-------- Begin of static function disp_contrib_buttons --------//
//
// 7kmod: the buttons for paying the honors of the generals. Only for the
// player's nation.
//
static void disp_contrib_buttons()
{
	vga_back.d3_panel_up( UNIT_BROWSE_X1, PAY_BUTTON_Y1-1, UNIT_BROWSE_X2, UNIT_BROWSE_Y2 );

	if( info.viewing_nation_recno != nation_array.player_recno )
		return;

	String str;

	//------- Pay selected -------//

	int selectedCost = contrib_selected_cost();

	str  = _("Pay selected");
	str += ": $";
	str += selectedCost;

	vga_back.d3_panel_up( PAY_SELECTED_X1+2, PAY_BUTTON_Y1+1, PAY_SELECTED_X2, PAY_BUTTON_Y2-2 );
	font_san.center_put( PAY_SELECTED_X1+2, PAY_BUTTON_Y1+1, PAY_SELECTED_X2, PAY_BUTTON_Y2-2, str );

	if( selectedCost == 0 )
		vga_back.adjust_brightness( PAY_SELECTED_X1+2, PAY_BUTTON_Y1+1, PAY_SELECTED_X2, PAY_BUTTON_Y2-2, -6 );

	//------- Pay all -------//

	int totalCost = contrib_total_cost();

	str  = _("Pay all");
	str += ": $";
	str += totalCost;

	vga_back.d3_panel_up( PAY_ALL_X1, PAY_BUTTON_Y1+1, PAY_ALL_X2, PAY_BUTTON_Y2-2 );
	font_san.center_put( PAY_ALL_X1, PAY_BUTTON_Y1+1, PAY_ALL_X2, PAY_BUTTON_Y2-2, str );

	if( totalCost == 0 )
		vga_back.adjust_brightness( PAY_ALL_X1, PAY_BUTTON_Y1+1, PAY_ALL_X2, PAY_BUTTON_Y2-2, -6 );
}
//----------- End of static function disp_contrib_buttons -----------//


//-------- Begin of static function detect_contrib_buttons --------//
//
static int detect_contrib_buttons()
{
	if( info.viewing_nation_recno != nation_array.player_recno )
		return 0;

	//------- Pay selected -------//

	if( mouse.single_click( PAY_SELECTED_X1+2, PAY_BUTTON_Y1+1, PAY_SELECTED_X2, PAY_BUTTON_Y2-2 ) )
	{
		if( contrib_selected_cost() > 0 )
		{
			pay_honors( unit_array[ contrib_filter(browse_contrib_recno) ] );
			se_ctrl.immediate_sound("TURN_ON");
		}
		return 1;
	}

	//------- Pay all, in order of risk, until the money runs out -------//

	if( mouse.single_click( PAY_ALL_X1, PAY_BUTTON_Y1+1, PAY_ALL_X2, PAY_BUTTON_Y2-2 ) )
	{
		int paidCount=0;

		for( int i=0 ; i<contrib_unit_count ; i++ )
			paidCount += pay_honors( unit_array[contrib_unit_array[i]] );

		if( paidCount > 0 )
			se_ctrl.immediate_sound("TURN_ON");

		return 1;
	}

	return 0;
}
//----------- End of static function detect_contrib_buttons -----------//
