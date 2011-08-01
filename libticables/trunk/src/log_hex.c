/* Hey EMACS -*- linux-c -*- */
/* $Id: data_log.c 1720 2006-01-20 22:34:58Z roms $ */

/*  libticables2 - link cable library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Lievin
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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* 
	Hexadecimal logging.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <glib.h>

#include "logging.h"
#include "data_log.h"
#include "ticables.h"

#define HEX_FILE	"ticables-log.hex"

static char *fn = NULL;
static FILE *logfile = NULL;

int log_hex_start(void)
{
	fn = g_strconcat(g_get_home_dir(), G_DIR_SEPARATOR_S, LOG_DIR, G_DIR_SEPARATOR_S, HEX_FILE, NULL);

	logfile = fopen(fn, "wt");
	if (logfile == NULL) 
	{
		ticables_critical("Unable to open %s for logging.\n", fn);
		return -1;
	}

	fprintf(logfile, "TiCables-2 data logger\n");	// needed by log_dbus.c
	fprintf(logfile, "Version %s\n", ticables_version_get());
	fprintf(logfile, "\n");

  	return 0;
}

int log_hex_1(int dir, uint8_t data)
{
	static int array[20];
	static int i = 0;
	int j;
	int c;

	if (logfile == NULL)
		return -1;
	
	array[i++] = data;
	fprintf(logfile, "%02X ", data);

	if ((i > 1 ) && !(i % 16)) 
	{
		fprintf(logfile, "| ");
		for (j = 0; j < 16; j++) 
		{
			c = array[j];
			if ((c < 32) || (c > 127))
				fprintf(logfile, " ");
			else
				fprintf(logfile, "%c", c);
		}
		fprintf(logfile, "\n");
		i = 0;
	}

	return 0;
}

int log_hex_stop(void)
{
	if (logfile != NULL)
	{
		fclose(logfile);
		logfile = NULL;
	}

	g_free(fn);
	fn = NULL;

	return 0;
}
