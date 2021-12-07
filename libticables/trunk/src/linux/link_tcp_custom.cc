/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

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

/* TCP/IPv4 Serial comm (custom ti/esp8266 thing) */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "../ticables.h"
#include "../logging.h"
#include "../error.h"
#include "../gettext.h"
#include "../internal.h"
#include "detect.h"

#define dev_fd		(GPOINTER_TO_INT(h->priv))
#define termset		((struct termios *)(h->priv2))

static int gry_prepare(CableHandle *h)
{
	return 0;
}

static int gry_open(CableHandle *h)
{
	ticables_info("Opening socket...");
	struct sockaddr_in serv_addr;
    struct hostent *server;

    h->priv = GINT_TO_POINTER(socket(AF_INET, SOCK_STREAM, 0));

	if (dev_fd == -1)
	{
		if (errno == EACCES)
		{
			ticables_critical(_("unable to open this serial port: %s (wrong permissions).\n"), h->device);
		}
		else
		{
			ticables_critical(_("unable to open this serial port: %s\n"), h->device);
		}
		return ERR_GRY_OPEN;
	}
    char *addr = getenv("TI_ADDR");
    if (!addr) {
        ticables_critical("Address enviroment var not set! Set TI_ADDR!\n");
	return ERR_GRY_OPEN;
    }
    ticables_info("Connecting to: '%s'...", addr);
    server = gethostbyname(addr);
    if (server == NULL) {
        ticables_critical("ERROR, no such host: '%s'\n", addr);
        exit(1);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(86);
    if (connect(dev_fd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) {
        ticables_critical("ERROR connecting");
        exit(1);
    }

	ticables_info("Socket opened.");
	return 0;
}

static int gry_close(CableHandle *h)
{
	ticables_info("Closing socket...");
	close(dev_fd);
	free(h->priv2);
	h->priv2 = NULL;

	ticables_info("Socket closed.");
	return 0;
}

static int gry_reset(CableHandle *h)
{
	return 0;
}


static int gry_put(CableHandle* h, uint8_t *data, uint32_t len)
{
	ticables_info("Sending data (%u bytes)...", len);

	if (len == 0) {
		ticables_warning("`%u bytes` is not a valid length. Not sending data but returning success.", len);
		return 0;
	} else if (len < 0) {
		ticables_critical("`%u bytes` is not a valid length.", len);
		return ERR_WRITE_ERROR;
	}
	ssize_t ret;

	ret = write(dev_fd, (void *)data, len);
	switch (ret)
	{
	case -1:		//error
		return ERR_WRITE_ERROR;
	case 0:		// timeout
		return ERR_WRITE_TIMEOUT;
	}

	usleep(1000 * len); // Sleep 1 millisecond per byte to help microcontroller catch up. Actual transfer speed should be much slower than this, so we always keep the microcontroller's buffer filled.

	return 0;
}

static int gry_get(CableHandle* h, uint8_t *data, uint32_t len)
{
	ticables_info("Receiving data (%u bytes)...", len);
	ssize_t ret;
	ssize_t i;

	for(i = 0; i < len; )
	{
		ret = read(dev_fd, (void *)(data+i), len - i);
		switch (ret)
		{
		case -1:		//error
			return ERR_READ_ERROR;
		case 0:		// timeout
			return ERR_READ_TIMEOUT;
		}

		i += ret;
	}

	return 0;
}

// Migrate these functions into ioports.c
static int dcb_read_io(CableHandle *h)
{
}

static int dcb_write_io(CableHandle *h, int data)
{
}

static int gry_probe(CableHandle *h)
{
	return 0;
}

static int gry_check(CableHandle *h, int *status)
{
	ticables_info("gry_check");
	fd_set rdfs;
	struct timeval tv;
	int retval;

	if (dev_fd < 0)
	{
		return ERR_READ_ERROR;
	}

	*status = STATUS_NONE;

	FD_ZERO(&rdfs);
	FD_SET(dev_fd, &rdfs);
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	retval = select(dev_fd + 1, &rdfs, NULL, NULL, &tv);
	switch (retval) {
	case -1:			//error
		return ERR_READ_ERROR;
	case 0:			//no data
		return 0;
	default:			// data available
		*status = STATUS_RX;
		break;
	}

	return 0;
}

static int gry_timeout(CableHandle *h)
{
	return 0;
}

static int gry_set_device(CableHandle *h, const char * device)
{
	ticables_info("Setting device.");
	if (device != NULL)
	{
		char * device2 = strdup(device);
		if (device2 != NULL)
		{
			free(h->device);
			h->device = device2;
		}
		else
		{
			ticables_warning(_("unable to set device %s.\n"), device);
		}
		return 0;
	}
	return ERR_ILLEGAL_ARG;
}

extern const CableFncts cable_gry =
{
	CABLE_GRY,
	"GRY",
	N_("GrayLink"),
	N_("GrayLink serial cable"),
	!0,
	&gry_prepare,
	&gry_open, &gry_close, &gry_reset, &gry_probe, &gry_timeout,
	&gry_put, &gry_get, &gry_check,
	&noop_set_red_wire, &noop_set_white_wire,
	&noop_get_red_wire, &noop_get_white_wire,
	NULL, NULL,
	&gry_set_device,
	NULL
};
