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

//Filename    : OF_RESE.CPP
//Description : Firm Magic Tower

#include <OINFO.h>
#include <OVGA.h>
#include <vga_util.h>
#include <ODATE.h>
#include <OSTR.h>
#include <ONEWS.h>
#include <OFONT.h>
#include <OCONFIG.h>
#include <OBUTT3D.h>
#include <OMOUSE.h>
#include <OSYS.h>
#include <OPOWER.h>
#include <OUNIT.h>
#include <OGAME.h>
#include <ONATION.h>
#include <OTECHRES.h>
#include <OF_RESE.h>
#include <OREMOTE.h>
#include <OSE.h>
#include <OSERES.h>
#include <OBUTTCUS.h>
#include "gettext.h"
#include <ModTech.h>
#include <OHELP.h>
#include <ONEWS.h>
#include <OINFO.h>

//------------- Define constant ------------//

#define MAX_RESEARCH_OPTION		6
#define RESEARCH_OPTION_HEIGHT 	48

//---------- Define constant ------------//

enum { RESEARCH_MENU_MAIN,
		 RESEARCH_MENU_RESEARCH,
		 RESEARCH_MENU_CATEGORY,		// 7kmod-gameplay: choose between technologies and war machines
		 RESEARCH_MENU_MOD_TECH,		// 7kmod-gameplay: the technologies of the mod
	  };

//----------- Define static vars -------------//

static Button3D button_select_research;
static char     research_menu_mode;
static char     disable_refresh=0;
static ButtonCustom	button_research_array[MAX_RESEARCH_OPTION];
// ######## begin Gilbert 16/8 ######//
static ButtonCustom	button_cancel;
// ######## end Gilbert 16/8 ######//
static int added_count;			// no. of buttons in button_research_array
static Button3D	button_vacate_firm;

//------ 7kmod-gameplay: research categories and mod technologies ------//

static ButtonCustom	button_category_tech, button_category_weapon;
static ButtonCustom	button_mod_tech_array[MAX_MOD_TECH];
static int				mod_tech_added_count;

static void i_disp_mod_tech_button(ButtonCustom *, int);

//---------- Declare static functions ---------//

static void i_disp_research_button(ButtonCustom *, int);
static void set_all_research(int techId);

//--------- Begin of function FirmResearch::FirmResearch ---------//
//
FirmResearch::FirmResearch()
{
	firm_skill_id = SKILL_RESEARCH;
}
//----------- End of function FirmResearch::FirmResearch -----------//


//--------- Begin of function FirmResearch::~FirmResearch ---------//
//
FirmResearch::~FirmResearch()
{
	terminate_research();
}
//----------- End of function FirmResearch::~FirmResearch -----------//


//--------- Begin of function FirmResearch::init_derived ---------//
//
void FirmResearch::init_derived()
{
	tech_id = 0;				   // the id. of the tech this firm is currently researching
	complete_percent = (float) 0;		// percent completed on researching the current technology
	mod_tech_id = 0;				// 7kmod-gameplay
}
//----------- End of function FirmResearch::init_derived -----------//


//------ 7kmod: the button row is moved up 6 pixels (from INFO_Y1+235) ------//
//------ so that a second row fits for the spy button of the player's tower ------//

#define RESEARCH_BUTTON_Y	(INFO_Y1+229)


//--------- Begin of function FirmResearch::put_info ---------//
//
void FirmResearch::put_info(int refreshFlag)
{
	if( disp_recruit_town_menu_if_active(refreshFlag) )		// 7kmod
		return;

	if( refreshFlag==INFO_REPAINT && !disable_refresh )
		research_menu_mode = RESEARCH_MENU_MAIN;

	switch( research_menu_mode )
	{
		case RESEARCH_MENU_MAIN:
			disp_main_menu(refreshFlag);
			break;

		case RESEARCH_MENU_RESEARCH:
			disp_research_menu(refreshFlag);
			break;

		case RESEARCH_MENU_CATEGORY:
			disp_research_category_menu(refreshFlag);
			break;

		case RESEARCH_MENU_MOD_TECH:
			disp_mod_tech_menu(refreshFlag);
			break;
	}
}
//----------- End of function FirmResearch::put_info -----------//


//--------- Begin of function FirmResearch::detect_info ---------//
//
int FirmResearch::detect_info()
{
	if( is_recruit_town_menu_active() )		// 7kmod
		return detect_recruit_town_menu();

	switch( research_menu_mode )
	{
		case RESEARCH_MENU_MAIN:
			return detect_main_menu();

		case RESEARCH_MENU_RESEARCH:
			return detect_research_menu();

		case RESEARCH_MENU_CATEGORY:
			return detect_research_category_menu();

		case RESEARCH_MENU_MOD_TECH:
			return detect_mod_tech_menu();
	}

	return 0;
}
//----------- End of function FirmResearch::detect_info -----------//


//--------- Begin of function FirmResearch::disp_main_menu ---------//
//
void FirmResearch::disp_main_menu(int refreshFlag)
{
	disp_basic_info(INFO_Y1, refreshFlag);

	if( !should_show_info() )
		return;

	disp_research_info(INFO_Y1+54, refreshFlag);
	disp_worker_list(INFO_Y1+107, refreshFlag);
	disp_worker_info(INFO_Y1+171, refreshFlag);

	//------ display mobilize button -------//

	int x = INFO_X1;

	if( own_firm() )
	{
		if (refreshFlag == INFO_REPAINT)
		{
			button_select_research.paint(INFO_X1, RESEARCH_BUTTON_Y, 'A', "RESEARCH");
			button_vacate_firm.paint(INFO_X1 + BUTTON_ACTION_WIDTH, RESEARCH_BUTTON_Y, 'A', "RECRUIT");
			button_vacate_firm.set_help_code("MOBILIZE");
		}

		if( have_own_workers() )
			button_vacate_firm.enable();
		else
			button_vacate_firm.disable();

		x += (BUTTON_ACTION_WIDTH * 2);

		disp_recruit_buttons(x, RESEARCH_BUTTON_Y, refreshFlag);		// 7kmod
		x += BUTTON_ACTION_WIDTH*2;
	}

	//--- 7kmod: the button row of the player's own tower is full, so its spy button is displayed in a second row ---//

	if( own_firm() )
		disp_spy_button( INFO_X1, RESEARCH_BUTTON_Y+BUTTON_ACTION_HEIGHT, refreshFlag );
	else
		disp_spy_button( x, RESEARCH_BUTTON_Y, refreshFlag );
}
//----------- End of function FirmResearch::disp_main_menu -----------//


//--------- Begin of function FirmResearch::detect_main_menu ---------//
//
int FirmResearch::detect_main_menu()
{
	//-------- detect basic info -----------//

	if( detect_basic_info() )
		return 1;

	//----------- detect worker -----------//

	if( detect_worker_list() )
	{
		disp_research_info(INFO_Y1+54, INFO_UPDATE);
		disp_worker_info(INFO_Y1+171, INFO_UPDATE);
		return 1;
	}

	//-------- detect spy button ----------//

	if( detect_spy_button() )
		return 1;

	if( !own_firm() )
		return 0;

	//------ 7kmod: detect recruit buttons -------//

	if( detect_recruit_buttons() )
		return 1;

	//------ detect the select research button -------//

	if( button_select_research.detect(GETKEY(KEYEVENT_FIRM_BUILD)) )
	{
		research_menu_mode = RESEARCH_MENU_CATEGORY;		// 7kmod-gameplay: first choose between technologies and war machines
		disable_refresh = 1;    // static var for disp_info() only
		info.disp();
		disable_refresh = 0;
		return 1;
	}

	//-------- detect mobilize button ----------//

	if (button_vacate_firm.detect())
	{
		mobilize_all_workers(COMMAND_PLAYER);
		return 1;
	}

	return 0;
}
//----------- End of function FirmResearch::detect_main_menu -----------//


/** \brief Make all Towers of Science research the same tech.
 *
 *  \param[in] techId Technology to research.
 *
 *  \note This function is only used by human players (via the Tower of Science
 *  research interface).
 */
void set_all_research(int techId)
{
	for( int i=firm_array.size() ; i>0 ; i-- )
	{
		if( firm_array.is_deleted(i) )
			continue;

		Firm* firmPtr = firm_array[i];

		if( firmPtr->nation_recno == nation_array.player_recno &&
			firmPtr->firm_id == FIRM_RESEARCH &&
			!firm_array.is_deleted(i) )
		{
			(dynamic_cast<FirmResearch*>(firmPtr))->start_research(techId, COMMAND_PLAYER);
		}
	}
}


//--------- Begin of function FirmResearch::disp_research_menu ---------//
//
void FirmResearch::disp_research_menu(int refreshFlag)
{
	if( refreshFlag != INFO_REPAINT )
		return;

	int techId, y=INFO_Y1;
	added_count=0;

	for( techId=1 ; techId<=tech_res.tech_count ; techId++ )
	{
		if( !tech_res[techId]->can_research(nation_recno) )
			continue;

		if( added_count < MAX_RESEARCH_OPTION )
		{
			button_research_array[added_count].paint(INFO_X1, y, INFO_X2, y+RESEARCH_OPTION_HEIGHT-2,
				i_disp_research_button, ButtonCustomPara(this, techId) );
			added_count++;
			y += RESEARCH_OPTION_HEIGHT;
		}
	}

	// ###### begin Gilbert 16/8 ######//
	// button_cancel.paint(INFO_X1, y, "CANCEL1", "CANCEL1D" );
	button_cancel.paint(INFO_X1, y, INFO_X2, y+RESEARCH_OPTION_HEIGHT*3/4,
		ButtonCustom::disp_text_button_func, ButtonCustomPara((void*)_("Cancel"),0) );
	// ###### end Gilbert 16/8 ######//
}
//----------- End of function FirmResearch::disp_research_menu -----------//


//--------- Begin of function FirmResearch::detect_research_menu ---------//
//
int FirmResearch::detect_research_menu()
{
	int i;
	for( i = 0; i < added_count; ++i )
	{
		int rc = button_research_array[i].detect(0, 0, 1, 0);
		if( rc )
		{
			int techId = button_research_array[i].custom_para.value;
			if( tech_res[techId]->can_research(nation_recno) )
			{
				if ( rc == 1 )  // Left click.
				{
					start_research(techId, COMMAND_PLAYER);
				}
				else if ( rc == 2 )  // Right click.
				{
					set_all_research(techId);
				}
				// ##### begin Gilbert 25/9 ######//
				se_ctrl.immediate_sound("TURN_ON");
				// ##### end Gilbert 25/9 ######//

				research_menu_mode = RESEARCH_MENU_MAIN;
				info.disp();
			}
			else
			{
				// ##### begin Gilbert 25/9 ######//
				se_ctrl.immediate_sound("TURN_OFF");
				// ##### end Gilbert 25/9 ######//
			}
			return 1;
		}
	}

	//------ detect the cancel button --------//

	if( i >= added_count )		// no research button has been pressed
	{
		if( button_cancel.detect() )
		{
			// ##### begin Gilbert 25/9 ######//
			se_ctrl.immediate_sound("TURN_OFF");
			// ##### end Gilbert 25/9 ######//
			research_menu_mode = RESEARCH_MENU_MAIN;
			info.disp();
			return 1;
		}
	}

	return 0;
}
//----------- End of function FirmResearch::detect_research_menu -----------//


//-------- Begin of static function i_disp_research_button --------//
//
void i_disp_research_button(ButtonCustom *button, int repaintBody)
{
	int x1 = button->x1;
	int y1 = button->y1;
	int x2 = button->x2;
	int y2 = button->y2;

	if( button->pushed_flag )
	{
		vga_util.d3_panel2_down(x1, y1, x2, y2);
		x1++;
		y1++;
	}
	else
	{
		vga_util.d3_panel2_up(x1, y1, x2, y2);
		x2--;
		y2--;
	}

	//--------------------------------------------//

	TechInfo* techInfo = tech_res[button->custom_para.value];

	// Vga::active_buf->d3_panel_down(x1+2, y1+2, x1+TECH_LARGE_ICON_WIDTH+7, y1+TECH_LARGE_ICON_HEIGHT+7, 2, 0 );
	Vga::active_buf->put_bitmap(x1+4, y1+4, techInfo->tech_large_icon() );

	//------ display research description -------//

	String str;

	str = techInfo->tech_des();

	Firm *firmPtr = (Firm *) button->custom_para.ptr;
	int researchVersion = techInfo->get_nation_tech_level(firmPtr->nation_recno)+1;		// research the next non-researched version

	if( researchVersion > 1 )
	{
		str += " ";
		str += misc.roman_number(researchVersion);
	}

	font_bible.put( x1+TECH_LARGE_ICON_WIDTH+12, y1+14, str );
}
//--------- End of static function i_disp_research_button ---------//


//--------- Begin of function FirmResearch::disp_research_info ---------//
//
void FirmResearch::disp_research_info(int dispY1, int refreshFlag)
{
	static short lastTechId=0;
	static short lastModTechId=0;

	if( refreshFlag==INFO_UPDATE && (lastTechId != tech_id || lastModTechId != mod_tech_id) )
	{
		lastTechId = tech_id;
		lastModTechId = mod_tech_id;
		info.disp();
	}

	lastTechId = tech_id;
	lastModTechId = mod_tech_id;

	//---------------- paint the panel --------------//

	if( refreshFlag == INFO_REPAINT )
		vga_util.d3_panel_up( INFO_X1, dispY1, INFO_X2, dispY1+50 );

	//------ 7kmod-gameplay: researching a mod technology, which has no icon ------//

	if( mod_tech_id )
	{
		int x=INFO_X1+4, y=dispY1+4;

		//--- the same layout as the weapons ---//

		if( refreshFlag == INFO_REPAINT )
		{
			vga_util.d3_panel_down( x, y, x+TECH_LARGE_ICON_WIDTH+3, y+TECH_LARGE_ICON_HEIGHT+3, 2 );
			vga_front.put_bitmap( x+2, y+2, (char*) mod_tech_res.icon(mod_tech_id) );

			font_san.put( x+TECH_LARGE_ICON_WIDTH+10, y+4, mod_tech_res.name(mod_tech_id) );
		}

		x += TECH_LARGE_ICON_WIDTH+10;

		vga_front.indicator( 0, x-2, y+21, mod_tech_res.get_progress(mod_tech_id, nation_recno), (float)100, VGA_GRAY );
		return;
	}

	if( !tech_id )
		return;

	int x=INFO_X1+4, y=dispY1+4;

	//-------- display the icon of the researching item ---------//

	TechInfo* techInfo = tech_res[tech_id];

	if( refreshFlag == INFO_REPAINT )
	{
		vga_util.d3_panel_down( x, y, x+TECH_LARGE_ICON_WIDTH+3, y+TECH_LARGE_ICON_HEIGHT+3, 2 );
		vga_front.put_bitmap( x+2, y+2, techInfo->tech_large_icon() );

		//----------- display text ------------//

		x += TECH_LARGE_ICON_WIDTH+10;

		String str;

		str  = techInfo->tech_des();

		int researchVersion = techInfo->get_nation_tech_level(nation_recno)+1;		// research the next non-researched version

		if( researchVersion > 1 )
		{
			str += " ";
			str += misc.roman_number(researchVersion);
		}

		font_san.put( x, y+4, str);
	}
	else
	{
		x += TECH_LARGE_ICON_WIDTH+10;
	}

	vga_front.indicator( 0, x-2, y+21, techInfo->get_progress(nation_recno), (float)100, VGA_GRAY );
}
//----------- End of function FirmResearch::disp_research_info -----------//


//--------- Begin of function FirmResearch::next_day ---------//
//
void FirmResearch::next_day()
{
	//----- call next_day() of the base class -----//

	Firm::next_day();

	//----------- update population -------------//

	recruit_worker();

	//-------- train up the skill ------------//

	update_worker();

	//--------- calculate productivity ----------//

	calc_productivity();

	//--------- process research ----------//

	process_research();
}
//----------- End of function FirmResearch::next_day -----------//


//------- Begin of function FirmResearch::change_nation ---------//
//
void FirmResearch::change_nation(int newNationRecno)
{
	terminate_research();

	//-------- change the nation of this firm now ----------//

	Firm::change_nation(newNationRecno);
}
//-------- End of function FirmResearch::change_nation ---------//


//--------- Begin of function FirmResearch::start_research --------//
//
// Start researching on the specific technology.
//
// <int> techId - id. of the technology to research.
//
void FirmResearch::start_research(int techId, char remoteAction)
{
	TechInfo* techInfo = tech_res[techId];

	err_when( !techInfo->can_research(nation_recno) );

	if( !remoteAction && remote.is_enable())
	{
		// packet structure : <firm recno> <tech Id>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_F_RESEARCH_START, 2*sizeof(short) );
		shortPtr[0] = firm_recno;
		shortPtr[1] = (short) techId;
		return;
	}

	//---- if the firm currently is already researching something ---//

	if( tech_id || mod_tech_id )
		terminate_research();

	//-------- set self parameters ---------//

	tech_id = techId;

	//------- set TechRes parameters -------//

	techInfo->inc_nation_is_researching(nation_recno);
}
//----------- End of function FirmResearch::start_research ---------//


//--------- Begin of function FirmResearch::process_research --------//
//
// Process the current research.
//
void FirmResearch::process_research()
{
	if( mod_tech_id )		// 7kmod-gameplay
	{
		process_mod_research();
		return;
	}

	if( !tech_id )
		return;

	//------- make a progress with the research ------//

	TechInfo* techInfo = tech_res[tech_id];
	float		 progressPoint;

	if( config.fast_build && nation_recno==nation_array.player_recno )
		progressPoint = (float) productivity / 100 + (float) 0.5;
	else
		progressPoint = (float) productivity / 300;

	int 	newLevel 	 = techInfo->get_nation_tech_level(nation_recno)+1;
	float levelDivider = ((float)(newLevel+1)/2);		// from 1.0 to 2.0

	progressPoint = progressPoint * (float) 30
						 / techInfo->complex_level
						 / levelDivider;					// more complex and higher level technology will take longer to research

	int techId = tech_id;		// techInfo->progress() will reset tech_id if the current research level is the MAX tech level, so we have to save it now

	if( techInfo->progress(nation_recno, progressPoint) )
	{
		if( tech_id )			// TechRes::progress() may have called terminate_research() if the tech level reaches the maximum
		{
			int techId = tech_id;

			research_complete();

			//----- research next level technology automatically -----//

			if( !firm_ai )		// for player's firm only
			{
				if( techInfo->get_nation_tech_level(nation_recno) < techInfo->max_tech_level )
				{
					start_research( techId, COMMAND_AUTO );

					if( firm_recno == firm_array.selected_recno )
						info.disp();
				}
			}
		}

		//--------- add news ---------//

		if( own_firm() )
		{
			news_array.tech_researched( techId, tech_res[techId]->get_nation_tech_level(nation_recno) );

			se_res.far_sound(center_x, center_y, 1, 'F', firm_id, "FINS", 
				'S', unit_res[tech_res[techId]->unit_id]->sprite_id);
			if( firm_recno == firm_array.selected_recno )
				info.disp();
		}
	}
}
//----------- End of function FirmResearch::process_research ---------//


//--------- Begin of function FirmResearch::research_complete --------//
//
void FirmResearch::research_complete()
{
	short techId = tech_id;         // backup tech_id

	tech_res[tech_id]->dec_nation_is_researching(nation_recno);

	tech_id = 0;         		// reset parameters
	complete_percent = (float) 0;
}
//----------- End of function FirmResearch::research_complete ---------//


//--------- Begin of function FirmResearch::terminate_research --------//
//
void FirmResearch::terminate_research()
{
	if( mod_tech_id )		// 7kmod-gameplay
	{
		mod_tech_id = 0;
		complete_percent = (float) 0;
	}

	if( !tech_id )
		return;

	tech_res[tech_id]->dec_nation_is_researching(nation_recno);

	tech_id = 0;         		// reset parameters
	complete_percent = (float) 0;
}
//----------- End of function FirmResearch::terminate_research ---------//


//--------- Begin of function FirmResearch::disp_research_category_menu ---------//
//
// 7kmod-gameplay: choose between researching technologies and war machines.
//
void FirmResearch::disp_research_category_menu(int refreshFlag)
{
	if( refreshFlag != INFO_REPAINT )
		return;

	int y=INFO_Y1;

	button_category_tech.paint( INFO_X1, y, INFO_X2, y+RESEARCH_OPTION_HEIGHT-2,
		ButtonCustom::disp_text_button_func, ButtonCustomPara((void*)"Technologies",0) );
	y += RESEARCH_OPTION_HEIGHT;

	button_category_weapon.paint( INFO_X1, y, INFO_X2, y+RESEARCH_OPTION_HEIGHT-2,
		ButtonCustom::disp_text_button_func, ButtonCustomPara((void*)"War Machines",0) );
	y += RESEARCH_OPTION_HEIGHT;

	button_cancel.paint(INFO_X1, y, INFO_X2, y+RESEARCH_OPTION_HEIGHT*3/4,
		ButtonCustom::disp_text_button_func, ButtonCustomPara((void*)_("Cancel"),0) );
}
//----------- End of function FirmResearch::disp_research_category_menu -----------//


//--------- Begin of function FirmResearch::detect_research_category_menu ---------//
//
int FirmResearch::detect_research_category_menu()
{
	int newMode = -1;

	if( button_category_tech.detect() )
		newMode = RESEARCH_MENU_MOD_TECH;

	else if( button_category_weapon.detect() )
		newMode = RESEARCH_MENU_RESEARCH;

	else if( button_cancel.detect() )
		newMode = RESEARCH_MENU_MAIN;

	if( newMode < 0 )
		return 0;

	se_ctrl.immediate_sound( newMode==RESEARCH_MENU_MAIN ? "TURN_OFF" : "TURN_ON" );

	research_menu_mode = newMode;

	if( newMode != RESEARCH_MENU_MAIN )
		disable_refresh = 1;    // static var for put_info() only

	info.disp();
	disable_refresh = 0;
	return 1;
}
//----------- End of function FirmResearch::detect_research_category_menu -----------//


//--------- Begin of function FirmResearch::disp_mod_tech_menu ---------//
//
// 7kmod-gameplay: display the mod technologies that can be researched.
//
void FirmResearch::disp_mod_tech_menu(int refreshFlag)
{
	if( refreshFlag != INFO_REPAINT )
		return;

	int y=INFO_Y1;
	mod_tech_added_count=0;

	for( int modTechId=1 ; modTechId<=MAX_MOD_TECH ; modTechId++ )
	{
		if( !mod_tech_res.can_research(modTechId, nation_recno) )
			continue;

		button_mod_tech_array[mod_tech_added_count].paint(INFO_X1, y, INFO_X2, y+RESEARCH_OPTION_HEIGHT-2,
			i_disp_mod_tech_button, ButtonCustomPara(this, modTechId) );

		mod_tech_added_count++;
		y += RESEARCH_OPTION_HEIGHT;
	}

	if( mod_tech_added_count==0 )
	{
		font_san.d3_put( INFO_X1, y, INFO_X2, y+RESEARCH_OPTION_HEIGHT-2, "All technologies have been researched" );
		y += RESEARCH_OPTION_HEIGHT;
	}

	button_cancel.paint(INFO_X1, y, INFO_X2, y+RESEARCH_OPTION_HEIGHT*3/4,
		ButtonCustom::disp_text_button_func, ButtonCustomPara((void*)_("Cancel"),0) );
}
//----------- End of function FirmResearch::disp_mod_tech_menu -----------//


//--------- Begin of function FirmResearch::detect_mod_tech_menu ---------//
//
int FirmResearch::detect_mod_tech_menu()
{
	for( int i=0 ; i<mod_tech_added_count ; i++ )
	{
		//--- the help: the name and the description of the technology ---//

		ButtonCustom* buttonPtr = button_mod_tech_array+i;
		int helpTechId = buttonPtr->custom_para.value;

		help.set_custom_help( buttonPtr->x1, buttonPtr->y1, buttonPtr->x2, buttonPtr->y2,
			mod_tech_res.name(helpTechId), mod_tech_res.des(helpTechId) );

		if( button_mod_tech_array[i].detect() )
		{
			int modTechId = button_mod_tech_array[i].custom_para.value;

			if( mod_tech_res.can_research(modTechId, nation_recno) )
			{
				start_mod_research(modTechId, COMMAND_PLAYER);
				se_ctrl.immediate_sound("TURN_ON");
			}
			else
			{
				se_ctrl.immediate_sound("TURN_OFF");
			}

			research_menu_mode = RESEARCH_MENU_MAIN;
			info.disp();
			return 1;
		}
	}

	if( button_cancel.detect() )
	{
		se_ctrl.immediate_sound("TURN_OFF");
		research_menu_mode = RESEARCH_MENU_MAIN;
		info.disp();
		return 1;
	}

	return 0;
}
//----------- End of function FirmResearch::detect_mod_tech_menu -----------//


//--------- Begin of function FirmResearch::start_mod_research ---------//
//
// 7kmod-gameplay: start researching a mod technology.
//
void FirmResearch::start_mod_research(int modTechId, char remoteAction)
{
	if( !remoteAction && remote.is_enable() )
	{
		// packet structure : <firm recno> <mod tech id>
		short *shortPtr = (short *)remote.new_send_queue_msg(MSG_F_RESEARCH_START_MOD_TECH, 2*sizeof(short) );
		shortPtr[0] = firm_recno;
		shortPtr[1] = (short) modTechId;
		return;
	}

	if( !mod_tech_res.can_research(modTechId, nation_recno) )
		return;

	//---- if the firm currently is already researching something ---//

	if( tech_id || mod_tech_id )
		terminate_research();

	mod_tech_id = modTechId;
}
//----------- End of function FirmResearch::start_mod_research -----------//


//--------- Begin of function FirmResearch::process_mod_research ---------//
//
// 7kmod-gameplay: make progress with the research of a mod technology, in
// the same way as FirmResearch::process_research().
//
void FirmResearch::process_mod_research()
{
	float progressPoint;

	if( config.fast_build && nation_recno==nation_array.player_recno )
		progressPoint = (float) productivity / 100 + (float) 0.5;
	else
		progressPoint = (float) productivity / 300;

	progressPoint = progressPoint * (float) 30 / mod_tech_res.complex_level(mod_tech_id);

	int modTechId = mod_tech_id;

	if( !mod_tech_res.progress(modTechId, nation_recno, progressPoint) )
		return;

	//------ the technology has been researched ------//

	if( own_firm() )
	{
		news_array.tech_researched( -modTechId, mod_tech_res.get_level(modTechId, nation_recno) );		// negative id. for mod technologies
		se_res.far_sound(center_x, center_y, 1, 'F', firm_id, "FINS");
	}

	//--- stop all the towers of this nation researching the technology, including this one, if it has reached its max level ---//

	if( !mod_tech_res.can_research(modTechId, nation_recno) )
	{
		for( int i=firm_array.size() ; i>0 ; i-- )
		{
			if( firm_array.is_deleted(i) )
				continue;

			Firm* firmPtr = firm_array[i];

			if( firmPtr->firm_id == FIRM_RESEARCH &&
				 firmPtr->nation_recno == nation_recno &&
				 ((FirmResearch*)firmPtr)->mod_tech_id == modTechId )
			{
				((FirmResearch*)firmPtr)->terminate_research();
			}
		}
	}

	if( firm_recno == firm_array.selected_recno )
		info.disp();
}
//----------- End of function FirmResearch::process_mod_research -----------//


//-------- Begin of static function i_disp_mod_tech_button --------//
//
// 7kmod-gameplay: button of a mod technology in the research menu.
// button->custom_para.value is the mod technology id.
//
static void i_disp_mod_tech_button(ButtonCustom *button, int repaintBody)
{
	int x1 = button->x1;
	int y1 = button->y1;
	int x2 = button->x2;
	int y2 = button->y2;

	if( button->pushed_flag )
	{
		vga_util.d3_panel2_down(x1, y1, x2, y2);
		x1++;
		y1++;
	}
	else
	{
		vga_util.d3_panel2_up(x1, y1, x2, y2);
		x2--;
		y2--;
	}

	int modTechId = button->custom_para.value;

	//--- the same layout as the weapons: the icon and the name; the description is in the help ---//

	Vga::active_buf->put_bitmap( x1+4, y1+4, (char*) mod_tech_res.icon(modTechId) );

	font_bible.put( x1+TECH_LARGE_ICON_WIDTH+12, y1+14, mod_tech_res.name(modTechId), 0, x2-4 );
}
//--------- End of static function i_disp_mod_tech_button ---------//
