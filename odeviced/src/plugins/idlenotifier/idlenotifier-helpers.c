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
#include "idlenotifier.h"

gboolean on_activity (GIOChannel *source, GIOCondition condition, IdleNotifier *self) {
	gchar *curr_state = idle_notifier_GetState(self);
	uint tag = idle_notifier_get_tag(self);
	if (g_strcmp0(curr_state, "BUSY")!=0) {
		if (tag > 0)
			g_source_remove (tag);
		idle_notifier_SetState(self, "BUSY");
	}	
	return TRUE;
}


void start_timers (gchar* node, IdleNotifier *self) {
	GIOChannel *channel;
	channel = g_io_channel_new_file (node, "r", NULL);
	g_io_add_watch (channel, G_IO_IN, (GIOFunc)on_activity, self);
	g_message ("IdleNotifier: INFO: Started Timers");	
}

