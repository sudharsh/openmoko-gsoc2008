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

gboolean on_activity (int fd) {
	
	struct input_event event;
	read (fd, &event, sizeof(event));

	/* Ignore EV_SYN */
	if (event.type == EV_SYN) {
		g_print ("\tGot a SYN event, Ignoring\n");
		return FALSE;
	}
	g_message ("Got value %d", event.value);
	return FALSE;
}

		
