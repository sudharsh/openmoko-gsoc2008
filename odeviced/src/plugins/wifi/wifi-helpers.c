/* 
 * wifi_helpers.c
 * Written by Sudharshan "Sup3rkiddo" S <sudharsh@gmail.com>
 * All Rights Reserved
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */ 

/* Some (..well most) of these are from neod */

#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>

#include <linux/if.h>
#include <linux/wireless.h>

#include <glib.h>

static gboolean wifi_get_status (const char *iface) {
	
	struct iwreq wrq;
	int sock = socket (AF_INET, SOCK_DGRAM, 0);
	if (!sock)
	{
		perror( "Unable to open socket" );
		return 0;
	}

	memset (&wrq, 0, sizeof (struct iwreq));
	strncpy ((char *)&wrq.ifr_name, iface, IFNAMSIZ);

	if (ioctl (sock, SIOCGIWTXPOW, &wrq) != 0)
	{
		perror( "Error performing ioctl" );
		close (sock);
		return 0;
	}
	
	close (sock);
	
	return !wrq.u.txpower.disabled;
}



static gboolean wifi_set_control (const char *iface, gboolean enable) {
	struct iwreq wrq;
	int sock = socket (AF_INET, SOCK_DGRAM, 0);
	if (!sock)
	{
		perror( "Unable to open socket" );
		return 0;
	}

	memset (&wrq, 0, sizeof (struct iwreq));
	strncpy ((char *)&wrq.ifr_name, iface, IFNAMSIZ);

	if (ioctl (sock, SIOCGIWTXPOW, &wrq) != 0)
	{
		perror( "Error performing ioctl" );
		close (sock);
		return 0;
	}

	if ( wrq.u.txpower.disabled != !enable )
	{
		wrq.u.txpower.disabled = !enable;

		if (ioctl (sock, SIOCSIWTXPOW, &wrq) != 0)
		{
			perror( "Error performing ioctl" );
			close (sock);
			return 0;
		}
	}

	close (sock);
	return 1;

}
