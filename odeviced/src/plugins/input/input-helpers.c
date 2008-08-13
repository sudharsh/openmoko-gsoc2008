/* 
 * input-helpers.c
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

/* Internal helpers for the input plugin */

#include <glib.h>
#include <unistd.h>
#include <linux/input.h>

#include "input.h"

gboolean on_activity (GIOChannel *source, GIOCondition *condition, Input *self) {
	struct input_event event;
	int fd = g_io_channel_unix_get_fd (source);
	
	read (fd, &event, sizeof(event));

	/* Ignore EV_SYN */
	if (event.type == EV_SYN) {
		g_print ("\tInput: INFO: Got a SYN event, Ignoring\n");
		return TRUE;
	}

	if (event.value == 0x01) 
		g_print ("\tInput: INFO: Got a keypress\n");

	g_message ("Got regular input event, value:%d code:%u type:%u", event.value, event.code, event.type);
	return TRUE;
}

void process_watch (GIOChannel *channel, Input *self) {
	g_io_add_watch (channel, G_IO_IN, (GIOFunc)on_activity, self);
	g_message ("Input: INFO: Added watch");	
}
