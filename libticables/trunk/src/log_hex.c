/* Hey EMACS -*- linux-c -*- */
/* $Id: data_log.c 1720 2006-01-20 22:34:58Z roms $ */

/*  libCables - Ti Link Cable library, a part of the TiLP project
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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* This unit allows to trace bytes which are transferred between PC
   and TI calculator.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <glib.h>

#include "logging.h"
#include "data_log.h"

#define LOG_FILE  "ticables-hex.log"

static char *fn = NULL;
static FILE *log = NULL;

int log_hex_start(void)
{
  // build filenames
#ifdef __WIN32__
	fn = g_strconcat("C:\\", LOG_FILE, NULL);
#else
	fn = g_strconcat(g_get_home_dir(), "/", LOG_FILE, NULL);
#endif

  	log = fopen(fn, "wt");
  	if (log == NULL) 
	{
    		ticables_error("Unable to open <%s> for logging.\n", fn);
    		return -1;
  	}

  	return 0;
}

int log_hex_1(int dir, uint8_t data)
{
  	static int array[20];
  	static int i = 0;
  	int j;
  	int c;

  	if (log == NULL)
    		return -1;
  	
	array[i++] = data;
  	fprintf(log, "%02X ", data);

  	if ((i > 1 ) && !(i % 16)) 
	{
    	fprintf(log, "| ");
    	for (j = 0; j < 16; j++) 
		{
      		c = array[j];
      		if ((c < 32) || (c > 127))
				fprintf(log, " ");
      		else
				fprintf(log, "%c", c);
    	}
    	fprintf(log, "\n");
    	i = 0;
  	}

  	return 0;
}

int log_hex_N(int dir, uint8_t *data, int len)
{
	int i;

	if (log == NULL)
    		return -1;

	for(i = 0; i < len; i++)
		log_hex_1(dir, data[i]);
  	
  	return 0;
}

int log_hex_stop(void)
{
  	if (log != NULL)
	{
    		fclose(log);
			log = NULL;
	}
  	g_free(fn);

  	return 0;
}