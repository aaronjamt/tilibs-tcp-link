/* Hey EMACS -*- linux-c -*- */
/* $Id: cmd84p.h 2074 2006-03-31 08:36:06Z roms $ */

/*  libticalcs - Ti Calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Li�vin
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __TICALCS_CMD84P__
#define __TICALCS_CMD84P__

#define PID84P_PRODUCT_NUMBER	0x0001
#define PID84P_PRODUCT_NAME		0x0002
#define PID84P_MAIN_PART_ID		0x0003
#define PID84P_HW_VERSION		0x0004
#define PID84P_UNKNOWN1			0x0005
#define PID84P_LANGUAGE_ID		0x0006
#define PID84P_SUBLANG_ID		0x0007
#define PID84P_DEVICE_TYPE		0x0008
#define PID84P_BOOT_VERSION		0x0009
#define PID84P_OS_MODE			0x000A
#define PID84P_OS_VERSION		0x000B
#define PID84P_PHYS_RAM			0x000C
#define PID84P_USER_RAM			0x000D
#define PID84P_FREE_RAM			0x000E
#define PID84P_PHYS_FLASH		0x000F
#define PID84P_USER_FLASH		0x0010
#define PID84P_FREE_FLASH		0x0011
#define PID84P_USER_PAGES		0x0012
#define PID84P_FREE_PAGES		0x0013
#define PID84P_UNKNOWN2			0x0019
#define PID84P_UNKNOWN3			0x001A
#define PID84P_UNKNOWN4			0x001B
#define PID84P_UNKNOWN5			0x001C
#define PID84P_UNKNOWN6			0x001D
#define PID84P_LCD_WIDTH		0x001E
#define PID84P_LCD_HEIGHT		0x001F
#define PID84P_SCREENSHOT		0x0022
#define PID84P_UNKNOWN7			0x0023
#define PID84P_CLK_ON			0x0024
#define PID84P_CLK_SEC			0x0025
#define PID84P_CLK_DATE_FMT		0x0027
#define PID84P_CLK_TIME_FMT		0x0028
#define PID84P_UNKNOWN8			0x0029
#define PID84P_BATTERY			0x002D
#define PID84P_UNKNOWN9			0x0030
#define PID84P_UNKNOWN10		0x0031
#define PID84P_UNKNOWN11		0x0032
#define PID84P_HOMESCREEN		0x0037
#define PID84P_SCREEN_SPLIT		0x0039 

typedef struct
{
	uint16_t	arg1;
	uint16_t	arg2;
	uint16_t	arg3;
	uint16_t	arg4;
	uint16_t	arg5;
} ModeSet;

typedef struct
{
	uint16_t	pid;
	uint8_t		ok;
	uint16_t	size;
	uint8_t*	data;
} CalcParm;

int ti84p_mode_set(CalcHandle *h);

int ti84p_os_begin(CalcHandle *h);
int ti84p_os_data(CalcHandle *h, uint16_t addr, uint8_t page, uint8_t flag, uint8_t* data);

int ti84p_params_request(CalcHandle *h, int nparams, uint16_t *pids, CalcParm **params);
void del_params_array(int nparams, CalcParm *params);
int ti84p_params_set(CalcHandle *h, const CalcParm *param);

#endif