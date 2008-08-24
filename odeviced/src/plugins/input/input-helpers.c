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

#include <stdio.h>
#include <glib.h>
#include <unistd.h>
#include <linux/input.h>

#include "input.h"
#include "input-helpers.h"

struct held_key_payload hk;

static gboolean on_activity (GIOChannel *channel, GIOCondition *condition, GQueue *event_q) {

	struct input_event *event;
	event = g_new (struct input_event, 1);
	int fd = g_io_channel_unix_get_fd (channel);	
	if (read (fd, event, sizeof(struct input_event)) < 0)
		perror ("read");

	if (event->type!=EV_SYN) { /* Don't process sync events */
		g_queue_push_tail (event_q, event);
		return TRUE;
	}
	g_free (event);
	return TRUE;
}
	
 
static gboolean process_event (GQueue *event_q) {	

	gchar *event_source;
	GHashTable *watches = input_get_watches (input_obj);
	GList *reportheld = input_get_reportheld (input_obj);
       	
	struct input_event *event;
	event = g_queue_pop_head (event_q);
       
	while (event) {
  		g_print ("Input: event, value:%d code:%u type:%u\n", event->value, event->code, event->type);
		event_source = g_hash_table_lookup (watches, event->code);
		if (!event_source) {
			g_print ("\tNo watch added for event code %u\n", event->code);
			goto end;
		}
		
		if (event->value == 0x01) { /* Press */
			g_print ("\tInput: INFO: Got a keypress from %s\n", event_source);
			if (list_has(reportheld, (char *)event_source)) {
				hk.tv_sec = event->time.tv_sec;
				hk.event_source = g_strdup (event_source);
				g_print ("\tInput: %d\n", hk.tv_sec);
				input_obj->tag = g_timeout_add_seconds (1, (GSourceFunc)held_key_timeout, &hk);
			}
			else {
				g_print ("\treportheld set to something other than true\n");
				g_signal_emit_by_name (input_obj, "event", event_source, "pressed", 0);
				input_obj->tag = 0;
			}
			
		}
		else if (event->value == 0x00) { /* Release */
			g_print ("\tInput: INFO: Released %s Key\n", event_source);
			if (input_obj->tag) 
				g_source_remove (input_obj->tag);

			if (hk.event_source)
				g_free (hk.event_source);		       
			g_signal_emit_by_name (input_obj, "event", event_source, "released", 0);
		}
	end: event = g_queue_pop_head (event_q);
	}
	g_queue_foreach (event_q, (GFunc)g_free, NULL);
	return TRUE;
}


static gboolean list_has (GList *list, char *data) {
	int i = 0;
	char *_element = g_list_nth_data (list, 0);
	while (_element) {
		if (g_strcmp0 (_element, data)==0) 
			return TRUE;
		_element = g_list_nth_data (list, i++);
	}
	return FALSE;
}
		
		


static gboolean held_key_timeout (struct held_key_payload *hk) {
	GTimeVal currtime;
	int held_secs;
	g_get_current_time (&currtime);
	held_secs = currtime.tv_sec - hk->tv_sec;
	g_print ("\t%ld %ld %ld\n", held_secs, currtime.tv_sec, hk->tv_sec);
	g_signal_emit_by_name (input_obj, "event", hk->event_source, "held", held_secs);
       	return TRUE; /* Call me again */
}


void process_watch (GIOChannel *channel) {
	GQueue *event_q = g_queue_new();
	g_io_add_watch (channel, G_IO_IN, (GIOFunc)on_activity, event_q);
	g_idle_add_full (G_PRIORITY_HIGH_IDLE, (GSourceFunc)process_event, event_q, NULL);
}
