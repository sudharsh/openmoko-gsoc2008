
/* 
 * idle-helpers.c
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

#include <glib.h>
#include <linux/input.h>
#include "idlenotifier.h"


gboolean on_activity (GIOChannel *source, GIOCondition *condition) {
	const gchar *curr_state = idle_notifier_get_current_state(idlenotifier_obj);
	struct input_event *event;
	event = g_new (struct input_event, 1);
	int fd = g_io_channel_unix_get_fd (source);
	
	if (read (fd, event, sizeof(struct input_event)) < 0)
		perror ("read");

	if (g_strcmp0(curr_state, "BUSY")) 
		idle_notifier_SetState(idlenotifier_obj, "BUSY");
	
	g_free (event);
	return TRUE;
}

