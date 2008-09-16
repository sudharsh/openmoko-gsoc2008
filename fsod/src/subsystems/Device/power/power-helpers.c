/* 
 * power-helpers.c
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

#include <netlink/netlink.h>
#include <linux/netlink.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <glib.h>

#include "power.h"
#include "power-helpers.h"

/* Return FD for kernel netlink socket */
int power_helpers_get_netlink_fd() {	
	int sockfd;		
	struct sockaddr_nl nl;

	nl.nl_family = AF_NETLINK;
	nl.nl_pid = getpid();
	nl.nl_groups = 0xffffffff;

	sockfd = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
	if (sockfd == -1) 
		perror ("Socket");

	bind (sockfd, (struct sockaddr *)&nl, sizeof(nl));
	return sockfd;
}


static gboolean process_uevent (GIOChannel *source, GIOCondition *condition, Power *self) {
	gchar *_status = power_GetPowerStatus(self);
	const gchar *curr_status = power_get_curr_status(self);
	int sockfd = g_io_channel_unix_get_fd (source);
	char buf[1024];

	recv(sockfd, &buf, sizeof(buf), 0);
	/* Sneaky little way of checking if 'power_supply' is present in `buf' */
	if (g_strsplit(buf, "power_supply", 0) && g_strcmp0(_status, curr_status)) { 
		g_message ("Battery status changed to %s", _status);
		g_signal_emit_by_name(self, "battery_status_changed", _status);
		power_set_curr_status(self, _status);
	}
	return TRUE;	
}


void power_helpers_start_watch (GIOChannel *channel, Power *self) {
	g_io_add_watch (channel, G_IO_IN, (GIOFunc)process_uevent, self);
}
