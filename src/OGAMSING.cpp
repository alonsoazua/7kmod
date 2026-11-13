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

//Filename    : OGAMSING.CPP
//Description : Single player game interface

#include <OVGA.h>
#include <vga_util.h>
#include <OSYS.h>
#include <OMOUSE.h>
#include <OFONT.h>
#include <string.h>
#include "gettext.h"
#include <OIMGRES.h>
#include <ORACERES.h>
#include <OINFO.h>
#include <OBUTT3D.h>
#include <OGET.h>
#include <OBATTLE.h>
#include <OGAME.h>
#include <ModEditor.h>
#include <OCONFIG.h>
#include <OMUSIC.h>
#include <OBUTTCUS.h>
#include <OCOLTBL.h>
#include <OGETA.h>
#include <ModFryhtan.h>
#include <ModSmoothText.h>
#include <stdlib.h>

//---------- Define option modes --------//

enum { OPTION_BASIC,
		 OPTION_ADVANCED,
		 OPTION_ADVANCE2,
		 OPTION_GOAL,
	  };

static char option_mode;

//--------- Define constant ----------//

enum { BASIC_OPTION_X_SPACE = 78,
		 BASIC_OPTION_HEIGHT = 32 };

enum { COLOR_OPTION_X_SPACE = 35,
		 COLOR_OPTION_HEIGHT = 32 };

enum { ADVANCED_OPTION_X_SPACE = 89,
		 ADVANCED_OPTION_Y_SPACE = 34,
		 ADVANCED_OPTION_HEIGHT  = 32  };

static char race_table[MAX_RACE] =		// race translation table
#if(MAX_RACE == 10)
{
	RACE_CHINESE, RACE_EGYPTIAN, RACE_GREEK, RACE_JAPANESE, RACE_MAYA,
	RACE_INDIAN, RACE_NORMAN, RACE_PERSIAN, RACE_VIKING, RACE_ZULU
};
#else
{
	RACE_CHINESE, RACE_GREEK, RACE_JAPANESE, RACE_MAYA,
	RACE_PERSIAN, RACE_NORMAN, RACE_VIKING
};
#endif

static char reverse_race_table[MAX_RACE] =		// race translation table
#if(MAX_RACE == 10)
{
	6, 4, 2, 8, 7, 0, 3, 1, 5, 9
};
#else
{
	5, 3, 1, 6, 4, 0, 2
};
#endif

//------- Declare static functions -------//

static int	select_option();

//-------- Begin of function Game::single_player_game --------//
//
// <int> noAI - if there should be no AI in the game.
//
void Game::single_player_game(int noAI)
{
	sys.is_mp_game = 0;

	if( !select_option() )
		return;

	// ------ attempt to save the config -------//
	config.save("CONFIG.DAT");

	//------ start single player game ----------//

	init();
	battle.run(0);			// 0-not multiplayer game
	deinit();
}
//--------- End of function Game::single_player_game ---------//


//-------- Begin of function Game::editor_new_scenario --------//
//
// 7kmod-editor: the "New" button of the scenario screen: open a new map in
// the editor, with the options of the last game (CONFIG.DAT) and a random
// map I.D. The options can be changed with the New button of the editor.
//
void Game::editor_new_scenario()
{
	// the same corrections as the new game screen (select_option())

	if( config.ai_nation_count < 1 )		// the last multiplayer game may set it to zero
		config.ai_nation_count = MAX_NATION-1;

	if( config.difficulty_level != OPTION_CUSTOM )
		config.change_difficulty(config.difficulty_level);

	config.difficulty_rating = config.single_player_difficulty();

	editor_new_map(0);
}
//--------- End of function Game::editor_new_scenario ---------//


//-------- Begin of function Game::editor_new_map --------//
//
// 7kmod-editor: generate a new map with the current config and open it in
// the editor. When the New button of the editor creates another map
// (ModEditor::new_map_flag), the main loop ends and the new map is
// generated here.
//
// <int> randomSeed - the map I.D., 0 - random
//
void Game::editor_new_map(int randomSeed)
{
	do
	{
		sys.is_mp_game = 0;

		mod_editor.reset();						// the data of the last scenario is erased
		mod_editor.editing_flag = 1;			// Sys::run() enters the editor mode

		info.init_random_seed(randomSeed);

		init();
		battle.run(0);
		deinit();

		randomSeed = mod_editor.new_map_seed;

		if( mod_editor.new_map_flag && sys.signal_exit_flag == 2 )
			sys.signal_exit_flag = 0;
	}
	while( mod_editor.new_map_flag );

	mod_editor.reset();
}
//--------- End of function Game::editor_new_map ---------//



// define bit flag for refreshFlag
static void disp_virtual_button(ButtonCustom *, int);
static void disp_virtual_tick(ButtonCustom *, int);
static void disp_fryhtan_kingdom(Config& tempConfig, int lastMonsterId);	// 7kmod-fryhtan
static void disp_species_button(ButtonCustom *, int);		// 7kmod-fryhtan

//------- 7kmod-fryhtan: the Humans / Fryhtans tabs and the grid of species -------//

enum { RACE_GRID_X1 = 222,
		 RACE_GRID_WIDTH = 6*BASIC_OPTION_X_SPACE,		// the 10 race buttons (5 x 2) and the empty column at their right
		 RACE_GRID_HEIGHT = 2*BASIC_OPTION_HEIGHT,
		 SPECIES_COLUMNS = 5,								// 5 x 2 species per page, and the arrows in the 6th column
		 SPECIES_CELL_WIDTH = BASIC_OPTION_X_SPACE,
		 SPECIES_PAGE_SIZE = SPECIES_COLUMNS*2,
		 SPECIES_PAGE_COUNT = (FRYHTAN_SPECIES_COUNT+SPECIES_PAGE_SIZE-1) / SPECIES_PAGE_SIZE,
		 SPECIES_ARROW_X1 = RACE_GRID_X1 + SPECIES_COLUMNS*SPECIES_CELL_WIDTH,
		 KINGDOM_TAB_HEIGHT = 20 };		// the text tabs above the grid

static char race_grid_buf[RACE_GRID_WIDTH*RACE_GRID_HEIGHT];		// the race buttons of the background
static char species_grid_buf[SPECIES_PAGE_COUNT][RACE_GRID_WIDTH*RACE_GRID_HEIGHT];	// the pages of species buttons made from them

static void save_grid(char* gridBuf, int y1);
static void restore_grid(const char* gridBuf, int y1);
static void make_species_grid(int y1, const int* speciesArray, int page);
static void disp_kingdom_tabs(int x1, int y1, int fryhtanFlag, int* humanX2, int* fryhtanX1, int* fryhtanX2);
static const struct SmoothText* find_smooth_text(const char* text, int regularFlag=0);
static void put_smooth_text(VgaBuf* vgaBuf, int x, int y, const struct SmoothText* smoothText);

#define SGOPTION_PAGE           0x40000000
#define SGOPTION_RACE           0x00000001
#define SGOPTION_COLOR          0x00000002
#define SGOPTION_AI_NATION      0x00000004
#define SGOPTION_DIFFICULTY     0x00000008
#define SGOPTION_TERRAIN        0x00000010
#define SGOPTION_LAND_MASS      0x00000020
#define SGOPTION_NAME_FIELD     0x00000040
#define SGOPTION_MAP_ID         0x00000080
#define SGOPTION_EXPLORED       0x00000100
#define SGOPTION_FOG            0x00000200
#define SGOPTION_TREASURE       0x00000400
#define SGOPTION_AI_TREASURE    0x00000800
#define SGOPTION_AI_AGGRESSIVE  0x00001000
#define SGOPTION_FRYHTANS       0x00002000
#define SGOPTION_RANDOM_STARTUP 0x00004000
#define SGOPTION_FRYHTAN_KINGDOM 0x00008000		// 7kmod-fryhtan
#define SGOPTION_RAW            0x00010000
#define SGOPTION_NEAR_RAW       0x00020000
#define SGOPTION_START_TOWN     0x00040000
#define SGOPTION_TOWN_STRENGTH  0x00080000
#define SGOPTION_TOWN_EMERGE    0x00100000
#define SGOPTION_KINGDOM_EMERGE 0x00200000
#define SGOPTION_RANDOM_EVENT   0x00400000
#define SGOPTION_SEAT_OF_POWER  0x00800000		// 7kmod
#define SGOPTION_CLEAR_ENEMY    0x01000000
#define SGOPTION_CLEAR_MONSTER  0x02000000
#define SGOPTION_ENOUGH_PEOPLE  0x04000000
#define SGOPTION_ENOUGH_INCOME  0x08000000
#define SGOPTION_ENOUGH_SCORE   0x10000000
#define SGOPTION_TIME_LIMIT     0x20000000
#define SGOPTION_ALL            0x7fffffff

static int select_option()
{
	const int offsetY = 124;
	char optionMode = OPTION_BASIC;
	char menuTitleBitmap[] = "TOP-NSPG";
	
	Config tempConfig = config;

	// last multi-player game may set ai_nation_count to zero
	if( tempConfig.ai_nation_count < 1 )
		tempConfig.ai_nation_count = MAX_NATION-1;

	// some setting may be modified in the last game
	if( tempConfig.difficulty_level != OPTION_CUSTOM )
		tempConfig.change_difficulty(tempConfig.difficulty_level);

	int i;
	long refreshFlag = SGOPTION_ALL;
	int retFlag = 0;

	// --------- initialize race button group ---------- //

	ButtonCustomGroup raceGroup(MAX_RACE);
	for( i = 0; i < MAX_RACE; ++i )
	{
#if(MAX_RACE == 10)
		raceGroup[i].create(222+(i%5)*BASIC_OPTION_X_SPACE, offsetY+81+(i/5)*BASIC_OPTION_HEIGHT,
			222+(i%5+1)*BASIC_OPTION_X_SPACE-1, offsetY+81+(i/5+1)*BASIC_OPTION_HEIGHT-1,
			disp_virtual_button, ButtonCustomPara(&raceGroup, race_table[i]), 0, 0);

#define Y_SHIFT_FLAG 1
#else
		raceGroup[i].create(118+i*BASIC_OPTION_X_SPACE, offsetY+103,
			118+(i+1)*BASIC_OPTION_X_SPACE-1, offsetY+103+BASIC_OPTION_HEIGHT-1,
			disp_virtual_button, ButtonCustomPara(&raceGroup, race_table[i]), 0, 0);
#define Y_SHIFT_FLAG 0
#endif
	}

	// --------- 7kmod-fryhtan: the Humans / Fryhtans tabs ---------- //
	//
	// Two text tabs above the race buttons, the chosen one underlined: Humans
	// (the 10 races) and Fryhtans (the 13 species in the same place, 7 x 2, in
	// the whole width). The tab follows the kingdom chosen
	// (Config::fryhtan_monster_id), which is kept in 7KMOD.DAT.

	const int fryhtanY1 = offsetY+81;		// the first row of races
	const int tabX1 = RACE_GRID_X1+2, tabY1 = fryhtanY1-KINGDOM_TAB_HEIGHT-1;
	int humanTabX2=0, fryhtanTabX1=0, fryhtanTabX2=0;		// set by disp_kingdom_tabs()

	//------ the species, in alphabetical order ------//

	int speciesArray[FRYHTAN_SPECIES_COUNT];

	for( i=0 ; i<FRYHTAN_SPECIES_COUNT ; i++ )
		speciesArray[i] = i+1;

	for( i=1 ; i<FRYHTAN_SPECIES_COUNT ; i++ )		// insertion sort by name
	{
		for( int j=i ; j>0 && strcmp(ModFryhtan::menu_species_name(speciesArray[j-1]), ModFryhtan::menu_species_name(speciesArray[j])) > 0 ; j-- )
		{
			int t = speciesArray[j]; speciesArray[j] = speciesArray[j-1]; speciesArray[j-1] = t;
		}
	}

	ButtonCustomGroup speciesGroup(SPECIES_PAGE_SIZE);		// the buttons of the page shown; set_species_page() sets their species

	for( i=0 ; i<SPECIES_PAGE_SIZE ; i++ )
	{
		int cellX1 = RACE_GRID_X1 + (i%SPECIES_COLUMNS)*SPECIES_CELL_WIDTH;
		int cellY1 = fryhtanY1 + (i/SPECIES_COLUMNS)*BASIC_OPTION_HEIGHT;

		speciesGroup[i].create(cellX1, cellY1, cellX1+SPECIES_CELL_WIDTH-1, cellY1+BASIC_OPTION_HEIGHT-1,
			disp_species_button, ButtonCustomPara(&speciesGroup, 0), 0, 0);
	}

	int speciesPage = 0;

	#define SPECIES_INDEX(monsterId)	( [&]() { for( int k=0 ; k<FRYHTAN_SPECIES_COUNT ; k++ ) if( speciesArray[k]==(monsterId) ) return k; return 0; }() )

	auto setSpeciesPage = [&](int page)		// the species of the buttons of a page (0 for the empty ones)
	{
		speciesPage = page;

		for( int k=0 ; k<SPECIES_PAGE_SIZE ; k++ )
		{
			int index = page*SPECIES_PAGE_SIZE + k;
			speciesGroup[k].custom_para.value = index < FRYHTAN_SPECIES_COUNT ? speciesArray[index] : 0;
		}
	};

	int lastFryhtanMonsterId = ModFryhtan::is_valid_species(tempConfig.fryhtan_monster_id) ?
										tempConfig.fryhtan_monster_id : FRYHTAN_DEFAULT_MONSTER_ID;

	if( !ModFryhtan::is_valid_species(tempConfig.fryhtan_monster_id) )
		tempConfig.fryhtan_monster_id = 0;

	setSpeciesPage( SPECIES_INDEX(lastFryhtanMonsterId) / SPECIES_PAGE_SIZE );		// the page of the chosen species

	// --------- initialize color button group ---------- //

	ButtonCustomGroup colorGroup(MAX_COLOR_SCHEME);
	for( i = 0; i < MAX_COLOR_SCHEME; ++i )
	{
		#if(Y_SHIFT_FLAG)
			#define Y_SHIFT 14
		#else
			#define Y_SHIFT 0
		#endif
		colorGroup[i].create(195+i*COLOR_OPTION_X_SPACE, offsetY+149+Y_SHIFT,
			195+(i+1)*COLOR_OPTION_X_SPACE-1, offsetY+149+Y_SHIFT+COLOR_OPTION_HEIGHT-1,
			disp_virtual_button, ButtonCustomPara(&colorGroup, i+1), 0, 0);
		#undef Y_SHIFT
	}

	// ---------- initialize ai_nation_count buttons --------//

	ButtonCustom aiNationInc, aiNationDec;
	{
		#if(Y_SHIFT_FLAG)
			#define Y_SHIFT 14
		#else
			#define Y_SHIFT 0
		#endif
		aiNationInc.create(595, offsetY+149+Y_SHIFT, 
			595+COLOR_OPTION_X_SPACE-1, offsetY+149+Y_SHIFT+COLOR_OPTION_HEIGHT-1,
			disp_virtual_button, ButtonCustomPara(NULL, +1) );
		aiNationDec.create(630, offsetY+149+Y_SHIFT, 
			630+COLOR_OPTION_X_SPACE-1, offsetY+149+Y_SHIFT+COLOR_OPTION_HEIGHT-1,
			disp_virtual_button, ButtonCustomPara(NULL, -1) );
		#undef Y_SHIFT
	}

	// ---------- initialize difficulty_level button group -------//

	ButtonCustomGroup diffGroup(6);
	for( i = 0; i < 6; ++i )
	{
		#if(Y_SHIFT_FLAG)
			#define Y_SHIFT 17
		#else
			#define Y_SHIFT 0
		#endif
		diffGroup[i].create( 205+i*BASIC_OPTION_X_SPACE, offsetY+194+Y_SHIFT,
			205+(i+1)*BASIC_OPTION_X_SPACE-1, offsetY+194+Y_SHIFT+BASIC_OPTION_HEIGHT-1,
			disp_virtual_button, ButtonCustomPara(&diffGroup, i), 0, 0);
		#undef Y_SHIFT
	}

	// --------- initialize terrain_set button group -------//

	ButtonCustomGroup terrainGroup(3);
	for( i = 0; i < 3; ++i )
	{
		#if(Y_SHIFT_FLAG)
			#define Y_SHIFT 17
		#else
			#define Y_SHIFT 0
		#endif
		terrainGroup[i].create(168+i*BASIC_OPTION_X_SPACE, offsetY+258+Y_SHIFT, 
			168+(i+1)*BASIC_OPTION_X_SPACE-1, offsetY+258+Y_SHIFT+BASIC_OPTION_HEIGHT-1,
			disp_virtual_button, ButtonCustomPara(&terrainGroup, i+1), 0, 0);
		#undef Y_SHIFT
	}

	// --------- initialize land_mass button group -------//

	ButtonCustomGroup landGroup(3);
	for( i = 0; i < 3; ++i )
	{
		#if(Y_SHIFT_FLAG)
			#define Y_SHIFT 17
		#else
			#define Y_SHIFT 0
		#endif
		landGroup[i].create(439+i*BASIC_OPTION_X_SPACE, offsetY+258+Y_SHIFT, 
			439+(i+1)*BASIC_OPTION_X_SPACE-1, offsetY+258+Y_SHIFT+BASIC_OPTION_HEIGHT-1,
			disp_virtual_button, ButtonCustomPara(&landGroup, i+1), 0, 0);
		#undef Y_SHIFT
	}

	GetA playerNameField;
	// ####### begin Gilbert 3/11 #######//
//	playerNameField.init( 318, offsetY+327, 690, tempConfig.player_name,
//		HUMAN_NAME_LEN, &font_san, 0, 1);
	playerNameField.init( 318, offsetY+322, 690, tempConfig.player_name,
		HUMAN_NAME_LEN, &font_bard, 0, 1);
	// ####### end Gilbert 3/11 #######//

	// --------- initialize info.random_seed field ----------//

	const int mapIdSize = 11;		// enough to hold a dword in decimal
	char mapIdStr[mapIdSize+1];
	info.init_random_seed(0);
	sprintf( mapIdStr,"%d",info.random_seed);
	GetA mapIdField;
#if(defined(SPANISH))
	#define MAPID_X1 586
#elif(defined(FRENCH))
	#define MAPID_X1 572
#else
	#define MAPID_X1 564
#endif
	mapIdField.init( MAPID_X1, offsetY+112, 700, mapIdStr, mapIdSize, &font_san, 0, 1);
#undef MAPID_X1
	// --------- initialize explore_whole_map button group -------//

	ButtonCustomGroup exploreGroup(2);
	for( i = 0; i < 2; ++i )
	{
		exploreGroup[i].create(335+i*BASIC_OPTION_X_SPACE, offsetY+103, 
			335+(i+1)*BASIC_OPTION_X_SPACE-1, offsetY+103+BASIC_OPTION_HEIGHT-1,
			disp_virtual_button, ButtonCustomPara(&exploreGroup, 1-i), 0, 0);
	}

	// --------- initialize fog_of_war button group -------//

	ButtonCustomGroup fogGroup(2);
	for( i = 0; i < 2; ++i )
	{
		fogGroup[i].create(335+i*BASIC_OPTION_X_SPACE, offsetY+135, 
			335+(i+1)*BASIC_OPTION_X_SPACE-1, offsetY+135+BASIC_OPTION_HEIGHT-1,
			disp_virtual_button, ButtonCustomPara(&fogGroup, 1-i), 0, 0);
	}

	// --------- initialize start_up_cash/start_up_food button group -------//

	ButtonCustomGroup treasureGroup(4);
	for( i = 0; i < 4; ++i )
	{
		treasureGroup[i].create(335+i*BASIC_OPTION_X_SPACE, offsetY+167,
			335+(i+1)*BASIC_OPTION_X_SPACE-1, offsetY+167+BASIC_OPTION_HEIGHT-1,
			disp_virtual_button, ButtonCustomPara(&treasureGroup, i+1), 0, 0);
	}

	// --------- initialize ai_start_up_cash/food button group -------//

	ButtonCustomGroup aiTreasureGroup(4);
	for( i = 0; i < 4; ++i )
	{
		aiTreasureGroup[i].create(335+i*BASIC_OPTION_X_SPACE, offsetY+199,
			335+(i+1)*BASIC_OPTION_X_SPACE-1, offsetY+199+BASIC_OPTION_HEIGHT-1,
			disp_virtual_button, ButtonCustomPara(&aiTreasureGroup, i+1), 0, 0);
	}

	// --------- initialize ai_aggressiveness -------//

	ButtonCustomGroup aiAggressiveGroup(4);
	for( i = 0; i < 4; ++i )
	{
		aiAggressiveGroup[i].create(335+i*BASIC_OPTION_X_SPACE, offsetY+231, 
			335+(i+1)*BASIC_OPTION_X_SPACE-1, offsetY+231+BASIC_OPTION_HEIGHT-1,
			disp_virtual_button, ButtonCustomPara(&aiAggressiveGroup, i+1), 0, 0);
	}

	// --------- initialize monster_type -------//

	ButtonCustomGroup monsterGroup(3);
	for( i = 0; i < 3; ++i )
	{
		monsterGroup[i].create(335+i*BASIC_OPTION_X_SPACE, offsetY+263,
			335+(i+1)*BASIC_OPTION_X_SPACE-1, offsetY+263+BASIC_OPTION_HEIGHT-1,
			disp_virtual_button, ButtonCustomPara(&monsterGroup, i), 0, 0);
	}

	// --------- initialize random startup button group -------//

	ButtonCustomGroup randomStartUpGroup(2);
	for( i = 0; i < 2; ++i )
	{
		randomStartUpGroup[i].create(335+i*BASIC_OPTION_X_SPACE, offsetY+295, 
			335+(i+1)*BASIC_OPTION_X_SPACE-1, offsetY+295+BASIC_OPTION_HEIGHT-1,
			disp_virtual_button, ButtonCustomPara(&randomStartUpGroup, 1-i), 0, 0);
	}

	//  -------- initialize start_up_raw_site buttons --------- //

	ButtonCustom rawSiteInc, rawSiteDec;
	rawSiteInc.create( 368, offsetY+100, 
		368+COLOR_OPTION_X_SPACE-1, offsetY+100+COLOR_OPTION_HEIGHT-1,
		disp_virtual_button, ButtonCustomPara(NULL,0));
	rawSiteDec.create( 403, offsetY+100, 
		403+COLOR_OPTION_X_SPACE-1, offsetY+100+COLOR_OPTION_HEIGHT-1,
		disp_virtual_button, ButtonCustomPara(NULL,0));

	// --------- initialize start_up_has_mine_nearby button group --------//

	ButtonCustomGroup nearRawGroup(2);
	for( i = 0; i < 2; ++i )
	{
		nearRawGroup[i].create(332+i*BASIC_OPTION_X_SPACE, offsetY+132,
			332+(i+1)*BASIC_OPTION_X_SPACE-1, offsetY+132+BASIC_OPTION_HEIGHT-1,
			disp_virtual_button, ButtonCustomPara(&nearRawGroup, 1-i), 0, 0);
	}

	// --------- initialize start_up_independent_town button group --------//

	static short startTownArray[3] = { 7, 15, 30 };

	ButtonCustomGroup townStartGroup(3);
	for( i = 0; i < 3; ++i )
	{
		townStartGroup[i].create(332+i*BASIC_OPTION_X_SPACE, offsetY+164, 
			332+(i+1)*BASIC_OPTION_X_SPACE-1, offsetY+164+BASIC_OPTION_HEIGHT-1,
			disp_virtual_button, ButtonCustomPara(&townStartGroup, startTownArray[i]), 0, 0);
	}

	// --------- initialize independent_town_resistance button group --------//

	ButtonCustomGroup townResistGroup(3);
	for( i = 0; i < 3; ++i )
	{
		townResistGroup[i].create(332+i*BASIC_OPTION_X_SPACE, offsetY+196, 
			332+(i+1)*BASIC_OPTION_X_SPACE-1, offsetY+196+BASIC_OPTION_HEIGHT-1,
			disp_virtual_button, ButtonCustomPara(&townResistGroup, i+1), 0, 0);
	}

	// --------- initialize new_independent_town_emerge button group --------//

	ButtonCustomGroup townEmergeGroup(2);
	for( i = 0; i < 2; ++i )
	{
		townEmergeGroup[i].create(332+i*BASIC_OPTION_X_SPACE, offsetY+228,
			332+(i+1)*BASIC_OPTION_X_SPACE-1, offsetY+228+BASIC_OPTION_HEIGHT-1,
			disp_virtual_button, ButtonCustomPara(&townEmergeGroup, 1-i), 0, 0);
	}

	// --------- 7kmod: initialize seat_of_power_unlocked button group --------//

	ButtonCustomGroup seatOfPowerGroup(2);
	for( i = 0; i < 2; ++i )
	{
		seatOfPowerGroup[i].create(332+i*BASIC_OPTION_X_SPACE, offsetY+324,
			332+(i+1)*BASIC_OPTION_X_SPACE-1, offsetY+324+BASIC_OPTION_HEIGHT-1,
			disp_virtual_button, ButtonCustomPara(&seatOfPowerGroup, 1-i), 0, 0);
	}

	// --------- initialize new_nation_emerge button group --------//

	ButtonCustomGroup nationEmergeGroup(2);
	for( i = 0; i < 2; ++i )
	{
		nationEmergeGroup[i].create(332+i*BASIC_OPTION_X_SPACE, offsetY+260, 
			332+(i+1)*BASIC_OPTION_X_SPACE-1, offsetY+260+BASIC_OPTION_HEIGHT-1,
			disp_virtual_button, ButtonCustomPara(&nationEmergeGroup, 1-i), 0, 0);
	}

	// --------- initialize random_event_frequency button group --------//

	ButtonCustomGroup randomEventGroup(4);
	for( i = 0; i < 4; ++i )
	{
		randomEventGroup[i].create(332+i*BASIC_OPTION_X_SPACE, offsetY+292, 
			332+(i+1)*BASIC_OPTION_X_SPACE-1, offsetY+292+BASIC_OPTION_HEIGHT-1,
			disp_virtual_button, ButtonCustomPara(&randomEventGroup, i), 0, 0);
	}

	// ---------- initialize goal buttons ----------//

	ButtonCustom clearEnemyButton, clearMonsterButton, enoughPeopleButton, enoughIncomeButton, enoughScoreButton, timeLimitButton;
	ButtonCustom peopleInc, peopleDec, incomeInc, incomeDec, scoreInc, scoreDec, yearInc, yearDec;

	clearEnemyButton.create( 214, offsetY+145, 214+19, offsetY+145+19,
		disp_virtual_tick, ButtonCustomPara(NULL, 0), 0, 1);
	clearEnemyButton.enable_flag = 0;;
	clearMonsterButton.create( 214, offsetY+178, 214+19, offsetY+178+19,
		disp_virtual_tick, ButtonCustomPara(NULL, 0), 0, 
		tempConfig.goal_destroy_monster);
	enoughPeopleButton.create( 214, offsetY+211, 214+19, offsetY+211+19,
		disp_virtual_tick, ButtonCustomPara(NULL, 0), 0, 
		tempConfig.goal_population_flag);
	enoughIncomeButton.create( 214, offsetY+244, 214+19, offsetY+244+19,	// -9
		disp_virtual_tick, ButtonCustomPara(NULL, 0), 0,
		tempConfig.goal_economic_score_flag);
	enoughScoreButton.create( 214, offsetY+277, 214+19, offsetY+277+19,
		disp_virtual_tick, ButtonCustomPara(NULL, 0), 0,
		tempConfig.goal_total_score_flag);
	timeLimitButton.create( 214, offsetY+309, 214+19, offsetY+309+19,		// +23
		disp_virtual_tick, ButtonCustomPara(NULL, 0), 0,
		tempConfig.goal_year_limit_flag);

	peopleInc.create( 529, offsetY+206, 
		529+COLOR_OPTION_X_SPACE-1, offsetY+206+COLOR_OPTION_HEIGHT-1, 
		disp_virtual_button, ButtonCustomPara(NULL, 0) );
	peopleDec.create( 564, offsetY+206, 
		564+COLOR_OPTION_X_SPACE-1, offsetY+206+COLOR_OPTION_HEIGHT-1, 
		disp_virtual_button, ButtonCustomPara(NULL, 0) );
	incomeInc.create( 529, offsetY+238,
		529+COLOR_OPTION_X_SPACE-1, offsetY+238+COLOR_OPTION_HEIGHT-1, 
		disp_virtual_button, ButtonCustomPara(NULL, 0) );
	incomeDec.create( 564, offsetY+238,
		564+COLOR_OPTION_X_SPACE-1, offsetY+238+COLOR_OPTION_HEIGHT-1, 
		disp_virtual_button, ButtonCustomPara(NULL, 0) );
	scoreInc.create( 529, offsetY+270,
		529+COLOR_OPTION_X_SPACE-1, offsetY+270+COLOR_OPTION_HEIGHT-1, 
		disp_virtual_button, ButtonCustomPara(NULL, 0) );
	scoreDec.create( 564, offsetY+270,
		564+COLOR_OPTION_X_SPACE-1, offsetY+270+COLOR_OPTION_HEIGHT-1, 
		disp_virtual_button, ButtonCustomPara(NULL, 0) );
	yearInc.create( 529, offsetY+302,
		529+COLOR_OPTION_X_SPACE-1, offsetY+302+COLOR_OPTION_HEIGHT-1, 
		disp_virtual_button, ButtonCustomPara(NULL, 0) );
	yearDec.create( 564, offsetY+302,
		564+COLOR_OPTION_X_SPACE-1, offsetY+302+COLOR_OPTION_HEIGHT-1, 
		disp_virtual_button, ButtonCustomPara(NULL, 0) );

	Button3D startButton, returnButton;
	// startButton.create(250, 553, "BB-START", "BB-START", 1, 0);
	// returnButton.create(440, 553, "BB-RETURN", "BB-RETURN", 1, 0);
	startButton.create(194, 538, "START-U", "START-D", 1, 0);
	returnButton.create(440, 538, "CANCEL-U", "CANCEL-D", 1, 0);

	vga_front.unlock_buf();

	while(1)
	{
		if( sys.need_redraw_flag )
		{
			refreshFlag = SGOPTION_ALL;
			sys.need_redraw_flag = 0;
		}

		vga_front.lock_buf();

		sys.yield();
		mouse.get_event();

		if( sys.signal_exit_flag == 1 )
		{
				retFlag = 0;
				break;
		}

		// -------- display ----------//
		if( refreshFlag )
		{
			// ------- display basic option ---------//
			if( optionMode == OPTION_BASIC )
			{
				if( refreshFlag & SGOPTION_PAGE )
				{
					image_menu.put_to_buf( &vga_back, "SPG-BSC");
#if(MAX_RACE == 10)
					// protection : image_menu.put_to_buf( &vga_back, "SPG-BSC");
					// ensure the user has the release version (I_MENU.RES)
					// image_menu2.put_to_buf( &vga_back, "SPG-BSC") get the real one
					image_menu2.put_to_buf( &vga_back, "SPG-BSC");
#endif
					image_menu.put_back( 234, 15, menuTitleBitmap);

					//--- 7kmod-fryhtan: the species buttons, made from the race buttons ---//

					save_grid( race_grid_buf, fryhtanY1 );
					for( int page=0 ; page<SPECIES_PAGE_COUNT ; page++ )
					{
						restore_grid( race_grid_buf, fryhtanY1 );
						make_species_grid( fryhtanY1, speciesArray, page );
						save_grid( species_grid_buf[page], fryhtanY1 );
					}

					restore_grid( tempConfig.fryhtan_monster_id ? species_grid_buf[speciesPage] : race_grid_buf, fryhtanY1 );

					vga_util.blt_buf(0,0,VGA_WIDTH-1,VGA_HEIGHT-1,0);
				}
				if( refreshFlag & (SGOPTION_RACE|SGOPTION_FRYHTAN_KINGDOM) )
				{
					//--- 7kmod-fryhtan: the grid of the active tab: races or species ---//

					restore_grid( tempConfig.fryhtan_monster_id ? species_grid_buf[speciesPage] : race_grid_buf, fryhtanY1 );

					if( tempConfig.fryhtan_monster_id )
					{
						int speciesIndex = SPECIES_INDEX(tempConfig.fryhtan_monster_id);

						vga_util.blt_buf( RACE_GRID_X1, fryhtanY1, RACE_GRID_X1+RACE_GRID_WIDTH-1, fryhtanY1+RACE_GRID_HEIGHT-1, 0 );		// the empty cells and the arrows

						if( speciesIndex / SPECIES_PAGE_SIZE == speciesPage )		// the chosen species is on this page
							speciesGroup.paint( speciesIndex % SPECIES_PAGE_SIZE );
						else
						{
							for( int k=0 ; k<SPECIES_PAGE_SIZE ; k++ )
								speciesGroup[k].paint(0);
						}
					}
					else
					{
						vga_util.blt_buf( RACE_GRID_X1, fryhtanY1, RACE_GRID_X1+RACE_GRID_WIDTH-1, fryhtanY1+RACE_GRID_HEIGHT-1, 0 );		// the empty column at the right
						raceGroup.paint( reverse_race_table[tempConfig.race_id-1] );
					}

					disp_kingdom_tabs( tabX1, tabY1, tempConfig.fryhtan_monster_id != 0, &humanTabX2, &fryhtanTabX1, &fryhtanTabX2 );
					disp_fryhtan_kingdom( tempConfig, lastFryhtanMonsterId );
				}
				if( refreshFlag & SGOPTION_COLOR )
					colorGroup.paint( tempConfig.player_nation_color-1 );
				if( refreshFlag & SGOPTION_AI_NATION )
				{
					#if(Y_SHIFT_FLAG)
						#define Y_SHIFT 14
					#else
						#define Y_SHIFT 0
					#endif
					font_san.center_put(564, offsetY+154+Y_SHIFT, 564+25, offsetY+154+Y_SHIFT+21,
						misc.format(tempConfig.ai_nation_count), 1);
					aiNationInc.paint();
					aiNationDec.paint();
					#undef Y_SHIFT
				}
				if( refreshFlag & SGOPTION_DIFFICULTY )
					diffGroup.paint(tempConfig.difficulty_level);
				if( refreshFlag & SGOPTION_TERRAIN )
					terrainGroup.paint(tempConfig.terrain_set-1);
				if( refreshFlag & SGOPTION_LAND_MASS )
					landGroup.paint(tempConfig.land_mass-1);
				if( refreshFlag & SGOPTION_NAME_FIELD )
				{
					playerNameField.paint();

					if( !(refreshFlag & (SGOPTION_RACE|SGOPTION_FRYHTAN_KINGDOM)) )		// 7kmod-fryhtan: the kingdom name follows the player name
						disp_fryhtan_kingdom( tempConfig, lastFryhtanMonsterId );
				}
			}

			// ------- display advanced option ---------//
			if( optionMode == OPTION_ADVANCED )
			{
				if( refreshFlag & SGOPTION_PAGE )
				{
					image_menu.put_to_buf( &vga_back, "SPG-O1");
					image_menu.put_back( 234, 15, menuTitleBitmap);
					vga_util.blt_buf(0,0,VGA_WIDTH-1,VGA_HEIGHT-1,0);
				}
				if( refreshFlag & SGOPTION_MAP_ID )
					mapIdField.paint();
				if( refreshFlag & SGOPTION_EXPLORED )
					exploreGroup.paint(1-tempConfig.explore_whole_map);
				if( refreshFlag & SGOPTION_FOG )
					fogGroup.paint(1-tempConfig.fog_of_war);
				if( refreshFlag & SGOPTION_TREASURE )
					treasureGroup.paint( tempConfig.start_up_cash-1 );
				if( refreshFlag & SGOPTION_AI_TREASURE )
					aiTreasureGroup.paint( tempConfig.ai_start_up_cash-1 );
				if( refreshFlag & SGOPTION_AI_AGGRESSIVE )
					aiAggressiveGroup.paint(tempConfig.ai_aggressiveness-1);
				if( refreshFlag & SGOPTION_FRYHTANS )
					monsterGroup.paint(tempConfig.monster_type);
				if( refreshFlag & SGOPTION_RANDOM_STARTUP )
					randomStartUpGroup.paint(1-tempConfig.random_start_up);
			}

			// ------- display advanced option ---------//
			if( optionMode == OPTION_ADVANCE2 )
			{
				if( refreshFlag & SGOPTION_PAGE )
				{
					image_menu.put_to_buf( &vga_back, "SPG-O2");
					image_menu.put_back( 234, 15, menuTitleBitmap);

					//--- 7kmod: add the Seats of Power option below the last row of the background ---//
					//
					// Copy the Yes/No buttons of "New Kingdoms Emerge?" and draw the label in
					// italic style like the labels of the background.

					for( int copyY=0 ; copyY<BASIC_OPTION_HEIGHT ; copyY++ )
					{
						memcpy( vga_back.buf_ptr(332, offsetY+324+copyY),
								  vga_back.buf_ptr(332, offsetY+260+copyY), BASIC_OPTION_X_SPACE*2 );
					}

					const SmoothText* seatText = find_smooth_text( _("Seats of Power Unlocked?"), 1 );		// 7kmod-fryhtan: smooth text, regular as the labels

					if( seatText )
						put_smooth_text( &vga_back, 139, offsetY+328, seatText );
					else
					{
						font_std.put_slanted( vga_back.buf_ptr(), vga_back.buf_pitch(), 141, offsetY+331,
							_("Seats of Power Unlocked?") );
					}

					vga_util.blt_buf(0,0,VGA_WIDTH-1,VGA_HEIGHT-1,0);
				}
				if( refreshFlag & SGOPTION_RAW )
				{
					font_san.center_put(337, offsetY+105, 337+25, offsetY+105+21,
						misc.format(tempConfig.start_up_raw_site), 1);
					rawSiteInc.paint();
					rawSiteDec.paint();
				}
				if( refreshFlag & SGOPTION_NEAR_RAW )
					nearRawGroup.paint(1-tempConfig.start_up_has_mine_nearby);
				if( refreshFlag & SGOPTION_START_TOWN )
					townStartGroup.paint(
					tempConfig.start_up_independent_town >= 30 ? 2 :
					tempConfig.start_up_independent_town <= 7 ? 0 :
					1
					);
				if( refreshFlag & SGOPTION_TOWN_STRENGTH )
					townResistGroup.paint(tempConfig.independent_town_resistance-1);
				if( refreshFlag & SGOPTION_TOWN_EMERGE )
					townEmergeGroup.paint(1-tempConfig.new_independent_town_emerge);
				if( refreshFlag & SGOPTION_KINGDOM_EMERGE )
					nationEmergeGroup.paint(1-tempConfig.new_nation_emerge);
				if( refreshFlag & SGOPTION_RANDOM_EVENT )
					randomEventGroup.paint(tempConfig.random_event_frequency);
				if( refreshFlag & SGOPTION_SEAT_OF_POWER )		// 7kmod
					seatOfPowerGroup.paint(1-tempConfig.seat_of_power_unlocked);
			}

			// ------- display goal option ---------//
			if( optionMode == OPTION_GOAL )
			{
				if( refreshFlag & SGOPTION_PAGE )
				{
					image_menu.put_to_buf( &vga_back, "SPG-GOAL");
					image_menu.put_back( 234, 15, menuTitleBitmap);
					vga_util.blt_buf(0,0,VGA_WIDTH-1,VGA_HEIGHT-1,0);
				}
				if( refreshFlag & SGOPTION_CLEAR_ENEMY )
					clearEnemyButton.paint();
				if( refreshFlag & SGOPTION_CLEAR_MONSTER )
					clearMonsterButton.paint(tempConfig.goal_destroy_monster);
				if( refreshFlag & SGOPTION_ENOUGH_PEOPLE )
				{
					enoughPeopleButton.paint(tempConfig.goal_population_flag);
					font_san.center_put( 456, offsetY+211, 456+67, offsetY+211+21,
						misc.format(tempConfig.goal_population) ,1);
					peopleInc.paint();
					peopleDec.paint();
				}
				if( refreshFlag & SGOPTION_ENOUGH_INCOME )
				{
					enoughIncomeButton.paint(tempConfig.goal_economic_score_flag);
					font_san.center_put( 456, offsetY+243, 456+67, offsetY+243+21,
						misc.format(tempConfig.goal_economic_score), 1);
					incomeInc.paint();
					incomeDec.paint();
				}
				if( refreshFlag & SGOPTION_ENOUGH_SCORE )
				{
					enoughScoreButton.paint(tempConfig.goal_total_score_flag);
					font_san.center_put( 456, offsetY+275, 456+67, offsetY+275+21,
						misc.format(tempConfig.goal_total_score), 1);
					scoreInc.paint();
					scoreDec.paint();
				}
				if( refreshFlag & SGOPTION_TIME_LIMIT )
				{
					timeLimitButton.paint(tempConfig.goal_year_limit_flag);
					font_san.center_put( 456, offsetY+307, 456+33, offsetY+307+21,
						misc.format(tempConfig.goal_year_limit), 1);
					yearInc.paint();
					yearDec.paint();
				}
			}

			// ------- display difficulty -------//
			if( refreshFlag & SGOPTION_DIFFICULTY )
			{
				font_san.center_put( 718, offsetY+74, 780, offsetY+108,
					misc.format(tempConfig.single_player_difficulty()), 1 );
			}

			// -------- repaint button -------//
			if( refreshFlag & SGOPTION_PAGE )
			{
				startButton.paint();
				returnButton.paint();
			}

			refreshFlag = 0;
		}
		sys.blt_virtual_buf();

		if( config.music_flag )
		{
			if( !music.is_playing(1) )
				music.play(1, sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0 );
		}
		else
			music.stop();

		// --------- detect basic option -------- //
		if( optionMode == OPTION_BASIC )
		{
			if( !tempConfig.fryhtan_monster_id && raceGroup.detect() >= 0)
			{
				tempConfig.race_id = raceGroup[raceGroup()].custom_para.value;
				//refreshFlag |= SGOPTION_RACE;
			}
			//--- 7kmod-fryhtan: the Humans / Fryhtans tabs and the species buttons ---//
			else if( tempConfig.fryhtan_monster_id && speciesGroup.detect() >= 0 )
			{
				if( speciesGroup[speciesGroup()].custom_para.value )		// not an empty button
				{
					lastFryhtanMonsterId = speciesGroup[speciesGroup()].custom_para.value;
					tempConfig.fryhtan_monster_id = lastFryhtanMonsterId;
				}
				refreshFlag |= SGOPTION_FRYHTAN_KINGDOM;		// the kingdom name below, and the button pressed
			}
			//--- the arrows: the previous and the next page of species ---//
			else if( tempConfig.fryhtan_monster_id && speciesPage > 0 &&
						mouse.single_click(SPECIES_ARROW_X1, fryhtanY1, SPECIES_ARROW_X1+BASIC_OPTION_X_SPACE-1, fryhtanY1+BASIC_OPTION_HEIGHT-1) )
			{
				setSpeciesPage( speciesPage-1 );
				refreshFlag |= SGOPTION_FRYHTAN_KINGDOM;
			}
			else if( tempConfig.fryhtan_monster_id && speciesPage < SPECIES_PAGE_COUNT-1 &&
						mouse.single_click(SPECIES_ARROW_X1, fryhtanY1+BASIC_OPTION_HEIGHT, SPECIES_ARROW_X1+BASIC_OPTION_X_SPACE-1, fryhtanY1+2*BASIC_OPTION_HEIGHT-1) )
			{
				setSpeciesPage( speciesPage+1 );
				refreshFlag |= SGOPTION_FRYHTAN_KINGDOM;
			}
			else if( mouse.single_click(tabX1, tabY1, humanTabX2, tabY1+KINGDOM_TAB_HEIGHT-1) )
			{
				if( tempConfig.fryhtan_monster_id )		// back to a human kingdom
				{
					tempConfig.fryhtan_monster_id = 0;
					refreshFlag |= SGOPTION_FRYHTAN_KINGDOM;
				}
			}
			else if( mouse.single_click(fryhtanTabX1, tabY1, fryhtanTabX2, tabY1+KINGDOM_TAB_HEIGHT-1) )
			{
				if( !tempConfig.fryhtan_monster_id )
				{
					tempConfig.fryhtan_monster_id = lastFryhtanMonsterId;
					refreshFlag |= SGOPTION_FRYHTAN_KINGDOM;
				}
			}
			else if( colorGroup.detect() >= 0)
			{
				tempConfig.player_nation_color = colorGroup[colorGroup()].custom_para.value;
				//refreshFlag |= SGOPTION_COLOR;
			}
			else if( aiNationInc.detect() )
			{
				tempConfig.ai_nation_count++;
				if( tempConfig.ai_nation_count >= MAX_NATION )
					tempConfig.ai_nation_count = MAX_NATION-1;
				tempConfig.difficulty_level = OPTION_CUSTOM;
				refreshFlag |= SGOPTION_AI_NATION | SGOPTION_DIFFICULTY;
			}
			else if( aiNationDec.detect() )
			{
				tempConfig.ai_nation_count--;
				if( tempConfig.ai_nation_count <= 0 )
					tempConfig.ai_nation_count = 1;
				tempConfig.difficulty_level = OPTION_CUSTOM;
				refreshFlag |= SGOPTION_AI_NATION | SGOPTION_DIFFICULTY;
			}
			else if( diffGroup.detect() >= 0)
			{
				if( diffGroup[diffGroup()].custom_para.value != OPTION_CUSTOM )
				{
					tempConfig.change_difficulty(diffGroup[diffGroup()].custom_para.value);
					// all but SGOPTION_PAGE;
					refreshFlag |= SGOPTION_ALL & ~SGOPTION_PAGE;
				}
			}
			else if( terrainGroup.detect() >= 0)
			{
				tempConfig.terrain_set = terrainGroup[terrainGroup()].custom_para.value;
				static short latitudeArray[3] = { 45, 70, 20 };
				err_when( tempConfig.terrain_set <= 0 || tempConfig.terrain_set > 3 );
				tempConfig.latitude = latitudeArray[tempConfig.terrain_set-1];
				//refreshFlag |= SGOPTION_TERRAIN;
			}
			else if( landGroup.detect() >= 0)
			{
				tempConfig.land_mass = landGroup[landGroup()].custom_para.value;
				//refreshFlag |= SGOPTION_LAND_MASS;
			}
			else if( playerNameField.detect() )
			{
				refreshFlag |= SGOPTION_NAME_FIELD;
			}
		}

		// -------- detect advanced option ---------//

		if( optionMode == OPTION_ADVANCED )
		{
			if( mapIdField.detect() )
			{
				refreshFlag |= SGOPTION_MAP_ID;
			}
			else
			if( exploreGroup.detect() >= 0 )
			{
				tempConfig.explore_whole_map = exploreGroup[exploreGroup()].custom_para.value;
				tempConfig.difficulty_level = OPTION_CUSTOM;
				// refreshFlag |= SGOPTION_EXPLORED;
				refreshFlag |= SGOPTION_DIFFICULTY;
			}
			else if( fogGroup.detect() >= 0 )
			{
				tempConfig.fog_of_war = fogGroup[fogGroup()].custom_para.value;
				tempConfig.difficulty_level = OPTION_CUSTOM;
				// refreshFlag |= SGOPTION_FOG
				refreshFlag |= SGOPTION_DIFFICULTY;
			}
			else if( treasureGroup.detect() >= 0 )
			{
				tempConfig.start_up_cash = treasureGroup[treasureGroup()].custom_para.value;
				tempConfig.difficulty_level = OPTION_CUSTOM;
				// refreshFlag |= SGOPTION_TREASURE;
				refreshFlag |= SGOPTION_DIFFICULTY;
			}
			else if( aiTreasureGroup.detect() >= 0 )
			{
				tempConfig.ai_start_up_cash = aiTreasureGroup[aiTreasureGroup()].custom_para.value;
				tempConfig.difficulty_level = OPTION_CUSTOM;
				// refreshFlag |= SGOPTION_AI_TREASURE;
				refreshFlag |= SGOPTION_DIFFICULTY;
			}
			else if( aiAggressiveGroup.detect() >= 0 )
			{
				tempConfig.ai_aggressiveness = 
					aiAggressiveGroup[aiAggressiveGroup()].custom_para.value;
				tempConfig.difficulty_level = OPTION_CUSTOM;
				// refreshFlag |= SGOPTION_AI_AGGRESSIVE;
				refreshFlag |= SGOPTION_DIFFICULTY;
			}
			else if( monsterGroup.detect() >= 0 )
			{
				tempConfig.monster_type = monsterGroup[monsterGroup()].custom_para.value;
				tempConfig.difficulty_level = OPTION_CUSTOM;
				// refreshFlag |= SGOPTION_FRYHTANS;
				refreshFlag |= SGOPTION_DIFFICULTY;
			}
			else if( randomStartUpGroup.detect() >= 0)
			{
				tempConfig.random_start_up = randomStartUpGroup[randomStartUpGroup()].custom_para.value;
				tempConfig.difficulty_level = OPTION_CUSTOM;
				// refreshFlag |= SGOPTION_RANDOM_STARTUP;
				refreshFlag |= SGOPTION_DIFFICULTY;
			}
		}

		// -------- detect advanced option ---------//

		if( optionMode == OPTION_ADVANCE2 )
		{
			if( rawSiteInc.detect() )
			{
				if( ++tempConfig.start_up_raw_site > MAX_RAW_RES_SITE )
					tempConfig.start_up_raw_site = MAX_RAW_RES_SITE;
				tempConfig.difficulty_level = OPTION_CUSTOM;
				refreshFlag |= SGOPTION_RAW | SGOPTION_DIFFICULTY;
			}
			else if( rawSiteDec.detect() )
			{
				if( --tempConfig.start_up_raw_site < MIN_RAW_RES_SITE )
					tempConfig.start_up_raw_site = MIN_RAW_RES_SITE;
				tempConfig.difficulty_level = OPTION_CUSTOM;
				refreshFlag |= SGOPTION_RAW | SGOPTION_DIFFICULTY;

			}
			else if( nearRawGroup.detect() >= 0)
			{
				tempConfig.start_up_has_mine_nearby = nearRawGroup[nearRawGroup()].custom_para.value;
				tempConfig.difficulty_level = OPTION_CUSTOM;
				// refreshFlag |= SGOPTION_NEAR_RAW;
				refreshFlag |= SGOPTION_DIFFICULTY;

			}
			else if( townStartGroup.detect() >= 0)
			{
				tempConfig.start_up_independent_town = townStartGroup[townStartGroup()].custom_para.value;
				tempConfig.difficulty_level = OPTION_CUSTOM;
				// resfreshFlag |= SGOPTION_START_TOWN;
				refreshFlag |= SGOPTION_DIFFICULTY;
			}
			else if( townResistGroup.detect() >= 0)
			{
				tempConfig.independent_town_resistance = townResistGroup[townResistGroup()].custom_para.value;
				tempConfig.difficulty_level = OPTION_CUSTOM;
				// resfreshFlag |= SGOPTION_TOWN_RESIST;
				refreshFlag |= SGOPTION_DIFFICULTY;
			}
			else if( townEmergeGroup.detect() >= 0)
			{
				tempConfig.new_independent_town_emerge = townEmergeGroup[townEmergeGroup()].custom_para.value;
				tempConfig.difficulty_level = OPTION_CUSTOM;
				// refreshFlag |= SGOPTION_TOWN_EMERGE;
				refreshFlag |= SGOPTION_DIFFICULTY;
			}
			else if( seatOfPowerGroup.detect() >= 0)		// 7kmod, it doesn't change the difficulty
			{
				tempConfig.seat_of_power_unlocked = seatOfPowerGroup[seatOfPowerGroup()].custom_para.value;
			}
			else if( nationEmergeGroup.detect() >= 0)
			{
				tempConfig.new_nation_emerge = nationEmergeGroup[nationEmergeGroup()].custom_para.value;
				tempConfig.difficulty_level = OPTION_CUSTOM;
				// refreshFlag |= SGOPTION_NATION_EMERGE;
				refreshFlag |= SGOPTION_DIFFICULTY;
			}
			else if( randomEventGroup.detect() >= 0)
			{
				tempConfig.random_event_frequency = randomEventGroup[randomEventGroup()].custom_para.value;
				tempConfig.difficulty_level = OPTION_CUSTOM;
				// refreshFlag |= SGOPTION_RANDOM_EVENT;
				refreshFlag |= SGOPTION_DIFFICULTY;
			}
		}

		// -------- detect goal option ----------//

		if( optionMode == OPTION_GOAL )
		{
			if( clearEnemyButton.detect() )
			{
			}
			else if( clearMonsterButton.detect() )
			{
				tempConfig.goal_destroy_monster = clearMonsterButton.pushed_flag;
			}
			else if( enoughPeopleButton.detect() )
			{
				tempConfig.goal_population_flag = enoughPeopleButton.pushed_flag;
			}
			else if( enoughIncomeButton.detect() )
			{
				tempConfig.goal_economic_score_flag = enoughIncomeButton.pushed_flag;
			}
			else if( enoughScoreButton.detect() )
			{
				tempConfig.goal_total_score_flag = enoughScoreButton.pushed_flag;
			}
			else if( timeLimitButton.detect() )
			{
				tempConfig.goal_year_limit_flag = timeLimitButton.pushed_flag;
			}
			else if( peopleInc.detect() )
			{
				tempConfig.goal_population += 100;
				if( tempConfig.goal_population > 5000 )
					tempConfig.goal_population = 5000;
				refreshFlag |= SGOPTION_ENOUGH_PEOPLE;
			}
			else if( peopleDec.detect() )
			{
				tempConfig.goal_population -= 100;
				if( tempConfig.goal_population < 100 )
					tempConfig.goal_population = 100;
				refreshFlag |= SGOPTION_ENOUGH_PEOPLE;
			}
			else if( incomeInc.detect() )
			{
				tempConfig.goal_economic_score += 100;
				if( tempConfig.goal_economic_score > 5000 )
				{
					tempConfig.goal_economic_score = 5000;
				}
				refreshFlag |= SGOPTION_ENOUGH_INCOME;
			}
			else if( incomeDec.detect() )
			{
				tempConfig.goal_economic_score -= 100;
				if( tempConfig.goal_economic_score < 100 )
				{
					tempConfig.goal_economic_score = 100;
				}
				refreshFlag |= SGOPTION_ENOUGH_INCOME;
			}
			else if( scoreInc.detect() )
			{
				if( tempConfig.goal_total_score >= 2000 )
					tempConfig.goal_total_score += 500;
				else
					tempConfig.goal_total_score += 100;
				if( tempConfig.goal_total_score > 10000 )
					tempConfig.goal_total_score = 10000;
				refreshFlag |= SGOPTION_ENOUGH_SCORE;
			}
			else if( scoreDec.detect() )
			{
				if( tempConfig.goal_total_score > 2000 )
					tempConfig.goal_total_score -= 500;
				else
					tempConfig.goal_total_score -= 100;
				if( tempConfig.goal_total_score < 100 )
					tempConfig.goal_total_score = 100;
				refreshFlag |= SGOPTION_ENOUGH_SCORE;
			}
			else if( yearInc.detect() )
			{
				if( tempConfig.goal_year_limit >= 20 )
					tempConfig.goal_year_limit += 5;
				else
					tempConfig.goal_year_limit++;
				if( tempConfig.goal_year_limit > 100 )
				{
					tempConfig.goal_year_limit = 100;
				}
				refreshFlag |= SGOPTION_TIME_LIMIT;
			}
			else if( yearDec.detect() )
			{
				if( tempConfig.goal_year_limit > 20 )
					tempConfig.goal_year_limit -= 5;
				else
					tempConfig.goal_year_limit--;
				if( tempConfig.goal_year_limit < 1 )
				{
					tempConfig.goal_year_limit = 1;
				}
				refreshFlag |= SGOPTION_TIME_LIMIT;
			}
		}

		// --------- detect switch option button ------//

		if( mouse.single_click(96, offsetY+12, 218, offsetY+54) )
		{
			if( optionMode != OPTION_BASIC )
			{
				optionMode = OPTION_BASIC;
				refreshFlag = SGOPTION_ALL;
			}
		}
		else if( mouse.single_click(236, offsetY+12, 363, offsetY+54) )
		{
			if( optionMode != OPTION_ADVANCED )
			{
				optionMode = OPTION_ADVANCED;
				refreshFlag = SGOPTION_ALL;
			}
		}
		else if( mouse.single_click(380, offsetY+12, 506, offsetY+54) )
		{
			if( optionMode != OPTION_ADVANCE2 )
			{
				optionMode = OPTION_ADVANCE2;
				refreshFlag = SGOPTION_ALL;
			}
		}
		else if( mouse.single_click(523, offsetY+12, 649, offsetY+54) )
		{
			if( optionMode != OPTION_GOAL )
			{
				optionMode = OPTION_GOAL;
				refreshFlag = SGOPTION_ALL;
			}
		}

		// --------- detect input name --------//

		// --------- detect start button --------//
		if( startButton.detect() )
		{
			retFlag = 1;
			break;			// break while(1)
		}
		else if( returnButton.detect() )
		{
			retFlag = 0;
			break;			// break while(1)
		}

		vga_front.unlock_buf();

	}

	if( !vga_front.buf_locked )
		vga_front.lock_buf();

	if( retFlag )
	{
		info.init_random_seed( atol(mapIdStr) );
		tempConfig.difficulty_rating = tempConfig.single_player_difficulty();

		config = tempConfig;
	}

	return retFlag;
}
#ifdef Y_SHIFT
	#error
#endif
#undef Y_SHIFT_FLAG



//------ Begin of static function disp_fryhtan_kingdom ------//
//
// 7kmod-fryhtan: display, below the player name, the name and the size of
// the Fryhtan kingdom.
//
static void disp_fryhtan_kingdom(Config& tempConfig, int lastMonsterId)
{
	const int offsetY = 124;
	const char* speciesName = ModFryhtan::menu_species_name(lastMonsterId);

	//------- the name and the size of the kingdom below the player name -------//

	int lineX1 = 240, lineY1 = offsetY+349, lineX2 = 690, lineY2 = lineY1+font_san.height()+1;

	mouse.hide_area(lineX1, lineY1, lineX2, lineY2);

	IMGcopy(vga_front.buf_ptr(), vga_front.buf_pitch(), vga_back.buf_ptr(), vga_back.buf_pitch(),
		lineX1, lineY1, lineX2, lineY2);

	if( tempConfig.fryhtan_monster_id )
	{
		char firstName[HUMAN_NAME_LEN+1];

		strncpy( firstName, tempConfig.player_name, HUMAN_NAME_LEN );
		firstName[HUMAN_NAME_LEN] = '\0';

		char* spacePtr = strchr(firstName, ' ');		// the first word, as NationBase::king_name(1)

		if( spacePtr )
			*spacePtr = '\0';

		char lineStr[200];

		// TRANSLATORS: Fryhtan kingdom: <Name> <Fryhtan species> Horde (Large: 2 population)
		snprintf( lineStr, sizeof(lineStr), _("Fryhtan kingdom: %s %s Horde (%s)"), firstName,
					 speciesName, ModFryhtan::menu_species_size_text(lastMonsterId) );

		font_san.put( lineX1, lineY1, lineStr );
	}

	mouse.show_area();
}
//------ End of static function disp_fryhtan_kingdom ------//


static void disp_virtual_button(ButtonCustom *button, int)
{
	mouse.hide_area(button->x1, button->y1, button->x2, button->y2);
	if( !button->pushed_flag )
	{
		// copy from back buffer to front buffer
		IMGcopy(vga_front.buf_ptr(), vga_front.buf_pitch(),
			vga_back.buf_ptr(), vga_back.buf_pitch(),
			button->x1, button->y1, button->x2, button->y2 );
	}
	else
	{
		// copy from back buffer to front buffer, but the area is
		// darkened by 2 scale
		IMGcopyRemap(vga_front.buf_ptr(), vga_front.buf_pitch(),
			vga_back.buf_ptr(), vga_back.buf_pitch(),
			button->x1, button->y1, button->x2, button->y2,
			vga.vga_color_table->get_table(-2) );

		// draw black frame
		if( button->x2-button->x1+1 == BASIC_OPTION_X_SPACE &&
			button->y2-button->y1+1 == BASIC_OPTION_HEIGHT )
		{
			image_interface.put_front(button->x1, button->y1, "BAS_DOWN");
		}
		else if( button->x2-button->x1+1 == COLOR_OPTION_X_SPACE &&
			button->y2-button->y1+1 == COLOR_OPTION_HEIGHT )
		{
			image_interface.put_front(button->x1, button->y1, "COL_DOWN");
		}
	}
	mouse.show_area();
}


static void disp_virtual_tick(ButtonCustom *button, int )
{
	mouse.hide_area(button->x1, button->y1, button->x2, button->y2);

	// copy from back buffer to front buffer
	IMGcopy(vga_front.buf_ptr(), vga_front.buf_pitch(),
		vga_back.buf_ptr(), vga_back.buf_pitch(),
		button->x1, button->y1, button->x2, button->y2 );

	if( button->pushed_flag )
		image_menu.put_front( button->x1+3, button->y1+3, "NMPG-RCH" );

	mouse.show_area();
}


//------ Begin of static function save_grid / restore_grid ------//
//
// 7kmod-fryhtan: save or put back the area of the race buttons in the
// background (vga_back).
//
static void save_grid(char* gridBuf, int y1)
{
	for( int y=0 ; y<RACE_GRID_HEIGHT ; y++ )
		memcpy( gridBuf+y*RACE_GRID_WIDTH, vga_back.buf_ptr(RACE_GRID_X1, y1+y), RACE_GRID_WIDTH );
}

static void restore_grid(const char* gridBuf, int y1)
{
	for( int y=0 ; y<RACE_GRID_HEIGHT ; y++ )
		memcpy( vga_back.buf_ptr(RACE_GRID_X1, y1+y), gridBuf+y*RACE_GRID_WIDTH, RACE_GRID_WIDTH );
}
//------ End of static function save_grid / restore_grid ------//


//------ Begin of static function make_species_grid ------//
//
// 7kmod-fryhtan: make a page of species buttons (5 x 2) and the arrows (the
// 6th column) in the area of the race buttons of the background (vga_back),
// from a race button (race_grid_buf must have been saved): the button with
// the text erased and the name of the species written. The arrow that has no
// page to go to is darkened.
//
static void make_species_grid(int y1, const int* speciesArray, int page)
{
	unsigned int seed = 23456;		// a local generator, not misc.random(), which the game uses

	//------ the font for all the names: the race font if all fit, else the smaller ones ------//

	static Font* fontPtr = NULL;

	if( !fontPtr )
	{
		Font* fontArray[] = { &font_std, &font_san, &font_mid, &font_small };
		int f;

		for( f=0 ; f<4 ; f++ )
		{
			int k;
			for( k=0 ; k<FRYHTAN_SPECIES_COUNT ; k++ )
			{
				if( fontArray[f]->text_width(ModFryhtan::menu_species_name(speciesArray[k])) > SPECIES_CELL_WIDTH-12 )
					break;
			}

			if( k==FRYHTAN_SPECIES_COUNT )
				break;
		}

		fontPtr = fontArray[ MIN(f,3) ];
	}

	for( int i=0 ; i<(SPECIES_COLUMNS+1)*2 ; i++ )
	{
		int col = i%(SPECIES_COLUMNS+1), row = i/(SPECIES_COLUMNS+1);
		int destX = RACE_GRID_X1 + col*SPECIES_CELL_WIDTH;
		int destY = y1 + row*BASIC_OPTION_HEIGHT;
		const char* srcCell = race_grid_buf + row*BASIC_OPTION_HEIGHT*RACE_GRID_WIDTH;		// the first race button of the row

		//------- copy the button -------//

		int x, y;

		for( y=0 ; y<BASIC_OPTION_HEIGHT ; y++ )
			memcpy( vga_back.buf_ptr(destX, destY+y), srcCell + y*RACE_GRID_WIDTH, SPECIES_CELL_WIDTH );

		//--- erase the text with pixels picked from the face of the button above the text ---//

		const int faceX1 = 6, faceX2 = SPECIES_CELL_WIDTH-8;
		const int bandY1 = 4, bandY2 = 6;
		const int textY1 = 7, textY2 = BASIC_OPTION_HEIGHT-6;		// also the tails of letters like the g of Mughul

		for( y=textY1 ; y<=textY2 ; y++ )
		{
			for( x=faceX1 ; x<=faceX2 ; x++ )
			{
				seed = seed * 1103515245 + 12345;
				int pickX = faceX1 + (seed>>16) % (faceX2-faceX1+1);
				int pickY = bandY1 + (seed>>8) % (bandY2-bandY1+1);

				*vga_back.buf_ptr(destX+x, destY+y) = *vga_back.buf_ptr(destX+pickX, destY+pickY);
			}
		}

		//------------ the arrows ------------//

		if( col == SPECIES_COLUMNS )
		{
			int upFlag = (row==0);
			int centerX = destX + SPECIES_CELL_WIDTH/2 - 2;
			int tipY = destY + (upFlag ? 9 : BASIC_OPTION_HEIGHT-11);

			for( int h=0 ; h<9 ; h++ )		// a triangle, 9 lines
			{
				int lineY = upFlag ? tipY+h : tipY-h;
				vga_back.bar( centerX-h, lineY, centerX+h, lineY, V_BLACK );
			}

			int enabled = upFlag ? page > 0 : page < SPECIES_PAGE_COUNT-1;

			if( !enabled )		// darkened
			{
				BYTE* remapTable = vga.vga_color_table->get_table(-4);

				for( y=0 ; y<BASIC_OPTION_HEIGHT ; y++ )
				{
					char* ptr = vga_back.buf_ptr(destX, destY+y);

					for( x=0 ; x<SPECIES_CELL_WIDTH ; x++ )
						ptr[x] = (char) remapTable[(unsigned char) ptr[x]];
				}
			}
			continue;
		}

		//------------ the name of the species ------------//

		int index = page*SPECIES_PAGE_SIZE + row*SPECIES_COLUMNS + col;

		if( index >= FRYHTAN_SPECIES_COUNT )		// the empty buttons of the last page
			continue;

		const char* text = ModFryhtan::menu_species_name(speciesArray[index]);

		const SmoothText* smoothText = find_smooth_text(text);		// the smooth name (not if it's translated)

		if( smoothText )
		{
			put_smooth_text( &vga_back, destX + (SPECIES_CELL_WIDTH-smoothText->width)/2 - 1,
								  destY + (BASIC_OPTION_HEIGHT-smoothText->height)/2 - 1, smoothText );
			continue;
		}

		int textWidth = fontPtr->text_width(text);
		int textX = destX + (SPECIES_CELL_WIDTH-textWidth)/2 - 1;
		int textY = destY + (BASIC_OPTION_HEIGHT-fontPtr->height())/2 - 1;

		for( int boldX=0 ; boldX<2 ; boldX++ )		// drawn twice, 1 pixel apart, to look as bold as the race names
			fontPtr->put_slanted( vga_back.buf_ptr(), vga_back.buf_pitch(), textX+boldX, textY, text );
	}
}
//------ End of static function make_species_grid ------//


//------ Begin of static function disp_species_button ------//
//
// 7kmod-fryhtan: a species button, as disp_virtual_button() for a button
// narrower than the race buttons (a black frame instead of BAS_DOWN).
//
static void disp_species_button(ButtonCustom *button, int)
{
	mouse.hide_area(button->x1, button->y1, button->x2, button->y2);

	if( !button->pushed_flag )
	{
		IMGcopy(vga_front.buf_ptr(), vga_front.buf_pitch(),
			vga_back.buf_ptr(), vga_back.buf_pitch(),
			button->x1, button->y1, button->x2, button->y2 );
	}
	else
	{
		IMGcopyRemap(vga_front.buf_ptr(), vga_front.buf_pitch(),
			vga_back.buf_ptr(), vga_back.buf_pitch(),
			button->x1, button->y1, button->x2, button->y2,
			vga.vga_color_table->get_table(-2) );

		vga_front.rect( button->x1, button->y1, button->x2, button->y2, 2, V_BLACK );
	}

	mouse.show_area();
}
//------ End of static function disp_species_button ------//


//------ Begin of static function disp_kingdom_tabs ------//
//
// 7kmod-fryhtan: the text tabs "Humans" and "Fryhtans" above the race
// buttons, the chosen one underlined. It returns the click areas.
//
static void disp_kingdom_tabs(int x1, int y1, int fryhtanFlag, int* humanX2, int* fryhtanX1, int* fryhtanX2)
{
	const int gap = 24;
	const char* strArray[2] = { _("Humans"), _("Fryhtans") };

	//--- the smooth texts: bold for the chosen tab, regular for the other (NULL if translated) ---//

	const SmoothText* boldText[2]    = { find_smooth_text(strArray[0]), find_smooth_text(strArray[1]) };
	const SmoothText* regularText[2] = { find_smooth_text(strArray[0], 1), find_smooth_text(strArray[1], 1) };

	int widthArray[2];

	for( int tab=0 ; tab<2 ; tab++ )
	{
		if( boldText[tab] && regularText[tab] )
			widthArray[tab] = MAX( boldText[tab]->width, regularText[tab]->width );
		else
			widthArray[tab] = font_std.text_width(strArray[tab]) + 3;
	}

	*humanX2   = x1+widthArray[0];
	*fryhtanX1 = *humanX2+gap;
	*fryhtanX2 = *fryhtanX1+widthArray[1];

	int y2 = y1+KINGDOM_TAB_HEIGHT-1;

	mouse.hide_area(x1, y1, *fryhtanX2+4, y2+2);

	vga_util.blt_buf( x1, y1, *fryhtanX2+4, y2+2, 0 );		// the background

	for( int tab=0 ; tab<2 ; tab++ )
	{
		int tabX1 = tab==0 ? x1 : *fryhtanX1;
		int tabX2 = tab==0 ? *humanX2 : *fryhtanX2;
		int chosen = (tab==1) == (fryhtanFlag!=0);

		const SmoothText* smoothText = chosen ? boldText[tab] : regularText[tab];

		if( smoothText && boldText[tab] && regularText[tab] )
			put_smooth_text( &vga_front, tabX1, y1, smoothText );
		else
		{
			int textY = y1 + (KINGDOM_TAB_HEIGHT-font_std.height())/2 - 2;

			font_std.put_slanted( vga_front.buf_ptr(), vga_front.buf_pitch(), tabX1+1, textY, strArray[tab] );		// slanted, as the labels

			if( chosen )
				font_std.put_slanted( vga_front.buf_ptr(), vga_front.buf_pitch(), tabX1+2, textY, strArray[tab] );		// bold
		}

		if( chosen )
			vga_front.bar( tabX1+2, y2, tabX2-2, y2+1, V_BLACK );		// the underline
	}

	mouse.show_area();
}
//------ End of static function disp_kingdom_tabs ------//


//------ Begin of static function find_smooth_text ------//
//
// 7kmod-fryhtan: the smooth (anti-aliased) version of a text, made by
// 7kmod/tools/make_smooth_text.py (include/ModSmoothText.h).
//
// <const char*> text        - the text to display (translated): a translated
//                             text isn't found, so the caller uses the game font
// [int]         regularFlag - 1-the regular version instead of the bold one
//
// return: <SmoothText*> the text, NULL if there is no smooth version of it
//
static const SmoothText* find_smooth_text(const char* text, int regularFlag)
{
	for( unsigned i=0 ; i<sizeof(smooth_text_array)/sizeof(smooth_text_array[0]) ; i++ )
	{
		const char* key = smooth_text_array[i].text;
		int keyLen = strlen(key);
		int regularKey = keyLen > 0 && key[keyLen-1] == '#';

		if( regularKey != (regularFlag!=0) )
			continue;

		if( (int) strlen(text) == keyLen-regularKey && !strncmp(text, key, keyLen-regularKey) )
			return smooth_text_array+i;
	}

	return NULL;
}
//------ End of static function find_smooth_text ------//


//------ Begin of static function put_smooth_text ------//
//
// 7kmod-fryhtan: draw a smooth text by darkening the pixels of the buffer
// with the color tables of the game (level 10 = black), so the anti-aliasing
// keeps to the 256-color palette.
//
static void put_smooth_text(VgaBuf* vgaBuf, int x, int y, const SmoothText* smoothText)
{
	const unsigned char* dataPtr = smoothText->data;

	for( int j=0 ; j<smoothText->height ; j++ )
	{
		if( y+j < 0 || y+j >= VGA_HEIGHT )
		{
			dataPtr += smoothText->width;
			continue;
		}

		char* bufPtr = vgaBuf->buf_ptr(x, y+j);

		for( int i=0 ; i<smoothText->width ; i++, dataPtr++ )
		{
			if( *dataPtr && x+i >= 0 && x+i < VGA_WIDTH )
				bufPtr[i] = (char) vga.vga_color_table->get_table(-(int)*dataPtr)[ (unsigned char) bufPtr[i] ];
		}
	}
}
//------ End of static function put_smooth_text ------//
