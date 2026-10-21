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

//Filename    : OF_BASE.H
//Description : Header of FirmBase

#ifndef __OF_BASE_H
#define __OF_BASE_H

#ifndef __OUNIT_H
#include <OUNIT.h>
#endif

#ifndef __OSKILL_H
#include <OSKILL.h>
#endif

#ifndef __OFIRM_H
#include <OFIRM.h>
#endif

//------- Define constant -----------//

#define  	MAX_BASE_PRAYER		30
#define		MAX_PRAY_POINTS		400

//--- 7kmod-gameplay: Fryhtans invoked from the Seat of Power ---//

#define		INVOKE_FRYHTAN_COMBAT_LEVEL	50
#define		INVOKED_FRYHTAN_DAMAGE_BONUS	25		// % of extra damage of the Fryhtans of a nation
#define		INVOKED_ROKKEN_ARMOR				5		// armor of the invoked Rokken (4 for the other units)
#define		INVOKED_DOINK_EXTRA_DAMAGE		1		// extra base damage per hit of the invoked Doink

//------- Define class FirmBase --------//

struct FirmBaseCrc;
struct FirmBaseGF;

#pragma pack(1)
class FirmBase : public Firm
{
public:
	short		god_id;
	short  	god_unit_recno;		// unit recno of the summoned god

	float		pray_points;

public:
	FirmBase();
	~FirmBase();

	void		init_derived();

	void 		assign_unit(int unitRecno);
	void 		assign_overseer(int overseerRecno);

	void 		change_nation(int newNationRecno);

	void 		put_info(int refreshFlag);
	int		detect_info();

	void		next_day();
	void		process_ai();

	int		can_invoke();
	void		invoke_god();

	int		can_invoke_fryhtan();						// 7kmod-gameplay
	int		invoke_fryhtan_unit_id();
	int		invoke_fryhtan_pray_points();
	int		invoke_fryhtan(char remoteAction);

	virtual	FirmBase* cast_to_FirmBase() { return this; };

	//-------------- multiplayer checking codes ---------------//
	virtual	uint8_t crc8();
	virtual	void	clear_ptr();
	virtual	void	init_crc(FirmBaseCrc *c);

	int		write_derived_file(File *filePtr);
	int		read_derived_file(File *filePtr);
	void		write_derived_record(FirmBaseGF *r);
	void		read_derived_record(FirmBaseGF *r);

private:
	void 		disp_base_info(int dispY1, int refreshFlag);
	void 		disp_god_info(int dispY1, int refreshFlag);
	void 		disp_invoke_fryhtan_cost();					// 7kmod-gameplay

	void		train_unit();
	void 		recover_hit_point();

	//------------- AI actions --------------//

	void 		think_assign_unit();
	void 		think_invoke_god();
};
#pragma pack()

//--------------------------------------//

#endif
