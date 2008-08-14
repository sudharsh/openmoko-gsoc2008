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


struct held_key_payload {
	Input *obj;
	gchar *event_source;
	struct input_event *event;
};
	

static gboolean held_key_timeout (Input *self, struct held_key_payload *hk);

/* FIXME: make this asynchronous, use idle processing */
static gboolean on_activity (GIOChannel *channel, GIOCondition *condition, Input *self) {

	struct input_event event;
	struct held_key_payload *hk;
	gchar *event_source;
	GHashTable *watches = input_get_watches (self);
	GList *reportheld = input_get_reportheld (self);
	
	int fd = g_io_channel_unix_get_fd (channel);
	
	read (fd, &event, sizeof(event));
	g_print ("Input: event, value:%d code:%u type:%u\n", event.value, event.code, event.type);
	/* Ignore EV_SYN */
	if (event.type == EV_SYN) {
		g_print ("\tInput: INFO: Got a SYN event, Ignoring\n");
		return TRUE;
	}

	event_source = g_hash_table_lookup (watches, (void *)&event.code);
	if (!event_source) {
		g_print ("\tNo watch added for event code %u\n", event.code);
		return TRUE;
	}

	if (event.value == 0x01) { /* Press */
		g_print ("\tInput: INFO: Got a keypress from %s\n", event_source);
		if (!g_list_find (reportheld, event_source)) {
		    g_signal_emit_by_name (self, "event", event_source, "pressed", 0);
		    return TRUE;
		}
		hk->obj = self;
		hk->event_source = event_source;
		hk->event = &event;
		g_timeout_add_seconds (1, (GSourceFunc)held_key_timeout, hk);
		
	}
	if (event.value == 0x00) { /* Release */
		g_print ("\tInput: INFO: Released %s Key\n", event_source);
		g_signal_emit_by_name (self, "event", event_source, "released", 0);
	}	

	return TRUE;
}


static gboolean held_key_timeout (Input *self, struct held_key_payload *hk) {
	GTimeVal currtime;
	g_get_current_time (&currtime);
	g_signal_emit_by_name (self, "event", hk->event_source, "pressed", (int)(currtime.tv_sec - hk->event->time.tv_sec));
	return TRUE; /* Call me again */
}


void process_watch (GIOChannel *channel, Input *self) {
	g_io_add_watch (channel, G_IO_IN, (GIOFunc)on_activity, self);
	g_message ("Input: INFO: Added watch");	
}
