
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

static gboolean on_activity (GIOChannel *source, GIOCondition condition, IdleNotifier *self) {
	gchar *curr_state = idle_notifier_GetState(self);
	uint tag = idle_notifier_get_tag(self);
	if (g_strcmp0(curr_state, "BUSY")!=0) {
		if (tag > 0)
			g_source_remove (tag);
		idle_notifier_SetState(self, "BUSY");
	}
	g_free (curr_state);
	return TRUE;
}


void start_timers (GIOChannel *channel, IdleNotifier *self) {
	g_io_add_watch_full (channel, G_PRIORITY_LOW, G_IO_IN, (GIOFunc)on_activity, self, NULL);
	g_message ("IdleNotifier: INFO: Started Timers");	
}

