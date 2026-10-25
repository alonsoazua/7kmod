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

//Filename    : OR_SPY.CPP
//Description : Spy Report

#include <OVGA.h>
#include <OFONT.h>
#include <OFIRMRES.h>
#include <ONATION.h>
#include <ORACERES.h>
#include <OIMGRES.h>
#include <OVBROWIF.h>
#include <vga_util.h>
#include <OSPY.h>
#include <OINFO.h>
#include <OMOUSE.h>
#include <ODATE.h>
#include <OTECHRES.h>
#include <ModTech.h>
#include <ModSpy.h>
#include "gettext.h"

//------------- Define coordinations -----------//

enum { SPY_BROWSE_X1 = ZOOM_X1+6,
		 SPY_BROWSE_Y1 = ZOOM_Y1+6,
		 SPY_BROWSE_X2 = ZOOM_X2-6,
		 SPY_BROWSE_Y2 = ZOOM_Y2-6,
	  };

//------ 7kmod-gameplay: tabs and the intelligence reports ------//

enum { SPY_TAB_SPIES, SPY_TAB_INTEL, SPY_TAB_COUNT };

enum { SPY_TAB_Y1 = SPY_BROWSE_Y1,
		 SPY_TAB_Y2 = SPY_BROWSE_Y1+20,
		 SPY_TAB_WIDTH = 120,
		 SPY_LIST_Y1 = SPY_TAB_Y2+2 };		// the report starts below the tabs

enum { INTEL_BROWSE_Y1 = SPY_LIST_Y1+22,
		 INTEL_BROWSE_Y2 = INTEL_BROWSE_Y1+6*18+6,
		 INTEL_DETAIL_Y1 = INTEL_BROWSE_Y2+3 };

//----------- Define static variables ----------//

static VBrowseIF browse_spy;
static VBrowseIF browse_intel;				// 7kmod-gameplay
static char		  spy_report_tab = SPY_TAB_SPIES;
static short	  browse_intel_recno = 1;

//----------- Define static functions ----------//

static void put_spy_rec(int recNo, int x, int y, int refreshFlag);
static int  spy_filter(int recNo=0);
static void	disp_total();
static void disp_tabs();
static int  detect_tabs();
static void disp_intel(int refreshFlag);
static void put_intel_rec(int recNo, int x, int y, int refreshFlag);
static void disp_intel_detail(IntelReport* reportPtr);

//--------- Begin of function Info::disp_spy ---------//
//
void Info::disp_spy(int refreshFlag)
{
	//------- 7kmod-gameplay: tabs: spies and intelligence reports -------//

	static char lastTab = -1;

	if( lastTab != spy_report_tab )		// initialize the browser of the new tab
	{
		lastTab = spy_report_tab;
		refreshFlag = INFO_REPAINT;
	}

	disp_tabs();

	if( spy_report_tab == SPY_TAB_INTEL )
	{
		disp_intel(refreshFlag);
		return;
	}

	//------- Display the spy report -------//

	int x=SPY_BROWSE_X1+9;
	int y=SPY_LIST_Y1+4;

	vga_back.d3_panel_up(SPY_BROWSE_X1, SPY_LIST_Y1, SPY_BROWSE_X2, SPY_LIST_Y1+20 );

#if(defined(FRENCH))
	font_san.put( x	 , y, "Spy Name" );
	font_san.put( x+155, y, "Cloak" );
	font_san.put( x+205, y, "Location" );
	font_san.put( x+320, y, "Skill" );
	font_san.put( x+384, y, "Loyalty" );
	font_san.put( x+448, y, "Action" );
#else
	// German and US
	font_san.put( x	 , y, _("Spy Name") );
	font_san.put( x+155, y, _("Cloak") );
	font_san.put( x+205, y, _("Location") );
	font_san.put( x+330, y, _("Skill") );
	font_san.put( x+370, y, _("Loyalty") );
	font_san.put( x+435, y, _("Action") );
#endif

	if( refreshFlag == INFO_REPAINT )
	{
		browse_spy.init( SPY_BROWSE_X1, SPY_LIST_Y1+22, SPY_BROWSE_X2, SPY_BROWSE_Y2-20,
								 0, 21, spy_filter(), put_spy_rec, 1 );

		browse_spy.open(browse_spy_recno);
	}
	else
	{
		browse_spy.paint();
		browse_spy.open(browse_spy_recno, spy_filter());
	}

	//--------- Display total ------------//

	disp_total();
}
//----------- End of function Info::disp_spy -----------//


//--------- Begin of function Info::detect_spy ---------//
//
void Info::detect_spy()
{
	//------- 7kmod-gameplay: tabs and intelligence reports -------//

	if( detect_tabs() )
		return;

	if( spy_report_tab == SPY_TAB_INTEL )
	{
		if( browse_intel.detect() )
			browse_intel_recno = browse_intel.recno();

		return;
	}

	//------- detect the spy browser -------//

	if( browse_spy.detect() )
	{
		browse_spy_recno = browse_spy.recno();

		if( browse_spy.double_click )
		{
			Spy* spyPtr = spy_array[ spy_filter(browse_spy_recno) ];
			int  xLoc, yLoc;

			if( spyPtr->get_loc(xLoc, yLoc) )
				world.go_loc( xLoc, yLoc, 1 );
		}
	}
}
//----------- End of function Info::detect_spy -----------//


//-------- Begin of static function disp_total --------//

static void disp_total()
{
	int x = SPY_BROWSE_X1+9;
	int y = SPY_BROWSE_Y2-16;

	vga_back.d3_panel_up(SPY_BROWSE_X1, SPY_BROWSE_Y2-18, SPY_BROWSE_X2, SPY_BROWSE_Y2 );

	String str;

	snprintf( str, MAX_STR_LEN+1, _( "Total Spies: %s"), misc.format(browse_spy.total_rec()) );

	font_san.put( x, y, str );
}
//----------- End of static function disp_total -----------//


//-------- Begin of static function spy_filter --------//
//
// This function has dual purpose :
//
// 1. when <int> recNo is not given :
//    - return the total no. of firms of this nation
//
// 2. when <int> recNo is given :
//    - return the firm recno in firm_array of the given recno.
//
static int spy_filter(int recNo)
{
	int  totalSpy = spy_array.size();
	int  spyCount=0;
	Spy* spyPtr;

	for( int spyRecno=1 ; spyRecno<=totalSpy ; spyRecno++ )
	{
		if( spy_array.is_deleted(spyRecno) )
			continue;

		spyPtr = spy_array[spyRecno];

		if( spyPtr->true_nation_recno==info.viewing_nation_recno )
			spyCount++;

		if( recNo && spyCount==recNo )
			return spyRecno;
	}

	err_when( recNo );   // the recNo is not found, it is out of range

	return spyCount;
}
//----------- End of static function spy_filter -----------//


//-------- Begin of static function put_spy_rec --------//
//
static void put_spy_rec(int recNo, int x, int y, int refreshFlag)
{
	int  spyRecno = spy_filter(recNo);
	Spy* spyPtr   = spy_array[spyRecno];

	x+=3;
	y+=5;

	//------ display rank/skill icon -------//

	int 	 cloakedRankId  = spyPtr->cloaked_rank_id();
	int 	 cloakedSkillId = spyPtr->cloaked_skill_id();
	String str;

	switch( cloakedRankId )
	{
		case RANK_KING:
			str = "U_KING";
			break;

		case RANK_GENERAL:
			str = "U_GENE";
			break;

		case RANK_SOLDIER:
			if( cloakedSkillId )
			{
				str  = "U_";
				str += Skill::skill_code_array[cloakedSkillId-1];
			}
			else
			{
				str = "";
			}
			break;
	}

	if( str.len() > 0 )
		image_icon.put_back(x, y+1, str);

	//------ display race icon -------------//

	vga_back.put_bitmap( x+13, y-4, race_res[spyPtr->race_id]->icon_bitmap_ptr );

	//----------- display name -----------//

	font_san.put( x+39, y, race_res[spyPtr->race_id]->get_name(spyPtr->name_id), 0, 185 );

	//------- display cloaked nation color ------//

	int tx = x+170;

	if( spyPtr->cloaked_nation_recno==0 )		// independent nation
	{
		vga_back.bar( tx, y, tx+12, y+12, V_WHITE );
		vga_back.rect( tx, y, tx+12, y+12, 1, VGA_GRAY+8 );
	}
	else
	{
		nation_array[spyPtr->cloaked_nation_recno]->disp_nation_color(tx, y+2);
	}

	//---------- display other info ----------//

	switch( spyPtr->spy_place )
	{
		case SPY_FIRM:
			str = _(firm_res[firm_array[spyPtr->spy_place_para]->firm_id]->name);
			break;

		case SPY_TOWN:
			str = town_array[spyPtr->spy_place_para]->town_name();
			break;

		case SPY_MOBILE:
		{
			Unit* unitPtr = unit_array[spyPtr->spy_place_para];

			switch( unitPtr->unit_mode )
			{
				case UNIT_MODE_CONSTRUCT:
					str = _(firm_res[firm_array[unitPtr->unit_mode_para]->firm_id]->name);
					break;

				case UNIT_MODE_ON_SHIP:
					str = _("On Ship");
					break;

				default:
					str = _("Mobile");
			}
			break;
		}

		default:
			str = "";
	}

	font_san.put( x+205, y, str );

	font_san.put( x+335, y, spyPtr->spy_skill );
	font_san.put( x+385, y, spyPtr->spy_loyalty );
	font_san.put( x+435, y, spyPtr->action_str() );
}
//----------- End of static function put_spy_rec -----------//




//-------- Begin of static function disp_tabs --------//
//
// 7kmod-gameplay: the tabs of the spy report: Spies and Intelligence.
//
static void disp_tabs()
{
	static const char* tabStr[SPY_TAB_COUNT] = { N_("Spies"), N_("Intelligence") };

	int x=SPY_BROWSE_X1;

	for( int i=0 ; i<SPY_TAB_COUNT ; i++, x+=SPY_TAB_WIDTH )
	{
		if( spy_report_tab == i )
			vga_back.d3_panel_down( x, SPY_TAB_Y1, x+SPY_TAB_WIDTH-1, SPY_TAB_Y2 );
		else
			vga_back.d3_panel_up( x, SPY_TAB_Y1, x+SPY_TAB_WIDTH-1, SPY_TAB_Y2 );

		font_san.center_put( x, SPY_TAB_Y1, x+SPY_TAB_WIDTH-1, SPY_TAB_Y2, _(tabStr[i]) );
	}
}
//----------- End of static function disp_tabs -----------//


//-------- Begin of static function detect_tabs --------//
//
static int detect_tabs()
{
	int x=SPY_BROWSE_X1;

	for( int i=0 ; i<SPY_TAB_COUNT ; i++, x+=SPY_TAB_WIDTH )
	{
		if( mouse.single_click( x, SPY_TAB_Y1, x+SPY_TAB_WIDTH-1, SPY_TAB_Y2 ) )
		{
			spy_report_tab = i;
			return 1;
		}
	}

	return 0;
}
//----------- End of static function detect_tabs -----------//


//-------- Begin of static function disp_intel --------//
//
// 7kmod-gameplay: the list of intelligence reports and the selected report.
//
static void disp_intel(int refreshFlag)
{
	int x=SPY_BROWSE_X1+9;
	int y=SPY_LIST_Y1+4;

	vga_back.d3_panel_up(SPY_BROWSE_X1, SPY_LIST_Y1, SPY_BROWSE_X2, SPY_LIST_Y1+20 );

	font_san.put( x	 , y, _("Date") );
	font_san.put( x+115, y, _("Kingdom") );
	font_san.put( x+336, y, _("Spy") );
	font_san.put( x+490, y, _("Skill") );

	int reportCount = mod_spy.player_report_count();

	if( browse_intel_recno > reportCount )
		browse_intel_recno = reportCount;

	if( browse_intel_recno < 1 )
		browse_intel_recno = 1;

	if( refreshFlag == INFO_REPAINT )
	{
		browse_intel.init( SPY_BROWSE_X1, INTEL_BROWSE_Y1, SPY_BROWSE_X2, INTEL_BROWSE_Y2,
								 0, 18, reportCount, put_intel_rec, 1 );

		browse_intel.open(browse_intel_recno);
	}
	else
	{
		browse_intel.paint();
		browse_intel.open(browse_intel_recno, reportCount);
	}

	//------- the selected report -------//

	//--- the background of the report is light and translucent, as the lists of the other report windows ---//

	vga_util.d3_panel_down( SPY_BROWSE_X1, INTEL_DETAIL_Y1, SPY_BROWSE_X2, SPY_BROWSE_Y2 );

	IntelReport* reportPtr = reportCount ? mod_spy.get_player_report(browse_intel_recno) : NULL;

	if( reportPtr )
		disp_intel_detail(reportPtr);
	else
	{
		font_san.put_paragraph( SPY_BROWSE_X1+10, INTEL_DETAIL_Y1+10, SPY_BROWSE_X2-10, SPY_BROWSE_Y2-10,
			_("No intelligence reports yet. Set a spy in a town or building of another kingdom to \"Gather Intel\": it makes a report after 2 to 6 months, faster with more spying skill. Several spies in the same kingdom add up their skill.") );
	}
}
//----------- End of static function disp_intel -----------//


//-------- Begin of static function put_intel_rec --------//
//
static void put_intel_rec(int recNo, int x, int y, int refreshFlag)
{
	IntelReport* reportPtr = mod_spy.get_player_report(recNo);

	if( !reportPtr )
		return;

	x+=3;
	y+=3;

	font_san.put( x+6, y, date.date_str(reportPtr->report_date, 1) );

	vga_back.bar( x+118, y, x+130, y+12, reportPtr->target_nation_color );
	vga_back.rect( x+118, y, x+130, y+12, 1, reportPtr->target_nation_color+2 );

	font_san.put( x+136, y, reportPtr->target_nation_name, 0, x+334 );

	if( reportPtr->spy_race_id )
		font_san.put( x+339, y, race_res[reportPtr->spy_race_id]->get_name(reportPtr->spy_name_id), 0, x+490 );

	font_san.put( x+496, y, reportPtr->spy_skill );
}
//----------- End of static function put_intel_rec -----------//


//-------- Begin of static function value_str --------//
//
// The approximate value of a report: "80 - 120", or "100" if it is exact.
//
// <int> formatType - as Misc::format(): 1 - number, 2 - money
//
static const char* value_str(IntelReport* reportPtr, int valueId, int formatType=1)
{
	static String str;

	int low = reportPtr->value_low[valueId], high = reportPtr->value_high[valueId];

	str = misc.format(low, formatType);

	if( high != low )
	{
		str += " - ";
		str += misc.format(high, formatType);
	}

	return str;
}
//----------- End of static function value_str -----------//


//-------- Begin of static function put_line --------//
//
static void put_line(int x, int& y, const char* label, const char* valueStr)
{
	font_san.put( x, y, label );
	font_san.put( x+130, y, valueStr );
	y += 15;
}
//----------- End of static function put_line -----------//


//-------- Begin of static function put_section --------//
//
// Display the title of a section of the report. If the spy didn't have
// enough skill for it, display that instead and return 0.
//
static int put_section(IntelReport* reportPtr, int sectionId, int x, int& y, const char* title)
{
	static int sectionSkill[INTEL_SECTION_COUNT] = { 20, 30, 30, 40, 40, 50, 90 };

	font_san.put( x, y, title );
	y += 16;

	if( reportPtr->section_flag[sectionId] )
		return 1;

	String str;
	snprintf( str, MAX_STR_LEN+1, _("Requires spying skill %d."), sectionSkill[sectionId] );
	font_san.put( x+10, y, str );
	y += 18;

	return 0;
}
//----------- End of static function put_section -----------//


//-------- Begin of static function disp_intel_detail --------//
//
static void disp_intel_detail(IntelReport* reportPtr)
{
	String str;
	int x1 = SPY_BROWSE_X1+10, x2 = SPY_BROWSE_X1+285;
	int y  = INTEL_DETAIL_Y1+6;

	//------- title --------//

	// TRANSLATORS: Intelligence report on <Kingdom>, <Date>
	snprintf( str, MAX_STR_LEN+1, _("Intelligence report on %s, %s"), reportPtr->target_nation_name,
				 date.date_str(reportPtr->report_date) );
	font_san.put( x1, y, str, 0, SPY_BROWSE_X2-10 );
	y += 20;

	int topY = y;

	//------- left column --------//

	if( put_section( reportPtr, INTEL_SECTION_TOWN, x1, y, _("Towns") ) )
	{
		put_line( x1+10, y, _("Towns"), value_str(reportPtr, INTEL_TOWN_COUNT) );
		put_line( x1+10, y, _("Population"), value_str(reportPtr, INTEL_POPULATION) );
		put_line( x1+10, y, _("Average loyalty"), value_str(reportPtr, INTEL_LOYALTY) );
		y += 3;
	}

	if( put_section( reportPtr, INTEL_SECTION_ECONOMY, x1, y, _("Economy") ) )
	{
		put_line( x1+10, y, _("Treasure"), value_str(reportPtr, INTEL_CASH, 2) );
		put_line( x1+10, y, _("Food"), value_str(reportPtr, INTEL_FOOD) );
		put_line( x1+10, y, _("Yearly income"), value_str(reportPtr, INTEL_INCOME, 2) );
		put_line( x1+10, y, _("Yearly expenses"), value_str(reportPtr, INTEL_EXPENSE, 2) );
		y += 3;
	}

	if( put_section( reportPtr, INTEL_SECTION_TRADE, x1, y, _("Trade") ) )
	{
		put_line( x1+10, y, _("Markets"), value_str(reportPtr, INTEL_MARKET_COUNT) );
		put_line( x1+10, y, _("Caravans"), value_str(reportPtr, INTEL_CARAVAN_COUNT) );
		y += 3;
	}

	if( put_section( reportPtr, INTEL_SECTION_MILITARY, x1, y, _("Military") ) )
	{
		put_line( x1+10, y, _("Soldiers"), value_str(reportPtr, INTEL_SOLDIER_COUNT) );
		put_line( x1+10, y, _("Generals"), value_str(reportPtr, INTEL_GENERAL_COUNT) );
		put_line( x1+10, y, _("Weapons"), value_str(reportPtr, INTEL_WEAPON_COUNT) );
		put_line( x1+10, y, _("Ships"), value_str(reportPtr, INTEL_SHIP_COUNT) );
		put_line( x1+10, y, _("Forts"), value_str(reportPtr, INTEL_CAMP_COUNT) );
		y += 3;
	}

	if( put_section( reportPtr, INTEL_SECTION_SPY, x1, y, _("Spies") ) )
	{
		put_line( x1+10, y, _("Spies"), value_str(reportPtr, INTEL_SPY_COUNT) );
		put_line( x1+10, y, _("In other kingdoms"), value_str(reportPtr, INTEL_SPY_ABROAD_COUNT) );
	}

	//------- right column: kingdom --------//

	y = topY;

	if( put_section( reportPtr, INTEL_SECTION_NATION, x2, y, _("Kingdom") ) )
	{
		put_line( x2+10, y, _("Reputation"), value_str(reportPtr, INTEL_REPUTATION) );

		for( int i=0 ; i<MAX_NATION ; i++ )
		{
			int status = reportPtr->relation_status[i];

			if( status < 0 || status > 4 )
				continue;

			vga_back.bar( x2+10, y, x2+22, y+12, reportPtr->relation_nation_color[i] );
			vga_back.rect( x2+10, y, x2+22, y+12, 1, reportPtr->relation_nation_color[i]+2 );
			font_san.put( x2+28, y, reportPtr->relation_nation_name[i], 0, x2+195 );
			font_san.put( x2+200, y, _(NationRelation::relation_status_str_array[status]) );
			y += 15;
		}
		y += 3;
	}

	//------- right column: technologies --------//

	if( put_section( reportPtr, INTEL_SECTION_TECH, x2, y, _("Technology") ) )
	{
		int techX=x2+10, techCount=0;
		int startY = y;

		for( int i=1 ; i<=tech_res.tech_count+MAX_MOD_TECH && i<=INTEL_MAX_TECH ; i++ )
		{
			int level = reportPtr->tech_level[i-1];

			if( level <= 0 )
				continue;

			if( y > SPY_BROWSE_Y2-18 )		// continue in a second column
			{
				if( techX > x2+10 )
					break;

				techX += 135;
				y = startY;
			}

			if( i <= tech_res.tech_count )
			{
				TechInfo* techInfo = tech_res[i];

				if( techInfo->max_tech_level > 1 )
					snprintf( str, MAX_STR_LEN+1, "%s %s", techInfo->tech_des(), misc.roman_number(level) );
				else
					str = techInfo->tech_des();
			}
			else
				str = mod_tech_res.name(i-tech_res.tech_count);

			font_san.put( techX, y, str, 0, techX+130 );
			y += 15;
			techCount++;
		}

		if( techCount==0 )
			font_san.put( x2+10, y, _("None") );
	}
}
//----------- End of static function disp_intel_detail -----------//
