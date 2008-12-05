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


static gboolean held_key_timeout (struct held_key_payload *hk);
static gboolean list_has (GList *list, gchar *data);
static gboolean process_event ();


gboolean on_activity (GIOChannel *channel, GIOCondition *condition) {

	struct input_event *event;
	struct GQueue *event_q = g_queue_new();
	event = g_new (struct input_event, 1);
	int fd = g_io_channel_unix_get_fd (channel);	

	if (read (fd, event, sizeof(struct input_event)) < 0)
		perror ("read");

	if (event->type!=EV_SYN)  {/* Don't process sync events */
		g_queue_push_tail (input_obj->event_q, event);
		g_idle_add_full (100, (GSourceFunc)process_event, (gpointer)event_q, NULL);
		return TRUE;
	}
			
	g_free (event);
	return TRUE;
}

	
static gboolean process_event (gpointer event_q) {

	static struct held_key_payload hk;
	GHashTable *watches = input_get_watches (input_obj);
	GList *reportheld = input_get_reportheld (input_obj);
	gchar *event_source;

	struct input_event *event;
       	
       	while ( event = (struct input_event *)g_queue_pop_head ((GQueue *)event_q) )	{
		g_print ("Input: event, value:%d code:%u type:%u\n", event->value, event->code, event->type);
		event_source = g_hash_table_lookup (watches, event->code);
		if (!event_source) {
			g_print ("\tNo watch added for event code %u\n", event->code);
			continue;
		}
		
		if (event->value == 0x01) { /* Press */
			g_print ("\tInput: INFO: Got a keypress from %s\n", event_source);
			if (list_has(reportheld, (char *)event_source)) {
				hk.tv_sec = event->time.tv_sec;
				hk.code = event->code;
				g_print ("\tInput: %d\n", hk.tv_sec);
				input_obj->tag = g_timeout_add (1000, (GSourceFunc)held_key_timeout, &hk);
			}
			else {
				g_print ("\treportheld set to something other than true\n");
				g_source_remove(input_obj->tag);
			}
			g_signal_emit_by_name (input_obj, "event", event_source, "pressed", 0);
		}
		else if (event->value == 0x00) { /* Release */
			g_print ("\tInput: INFO: Released %s Key\n", event_source);
			if (input_obj->tag)  
				g_source_remove (input_obj->tag);
			
			g_signal_emit_by_name (input_obj, "event", event_source, "released", hk.held_secs);
		}
		g_free(event);
	}
	if (g_queue_is_empty(event_q))
		g_queue_free(event_q);
	return FALSE;
}


static gboolean list_has (GList *list, char *data) {
	int i = 0;
	gchar *_element;
	while (_element = g_list_nth_data (list, i++)) {
		if (g_strcmp0 (_element, data)==0) 
			return TRUE;
	}
	return FALSE;
}
		
		
static gboolean held_key_timeout (struct held_key_payload *hk) {

	GTimeVal currtime;
	int held_secs;
	GHashTable *watches = input_get_watches (input_obj);
	gchar *event_source = g_hash_table_lookup (watches, hk->code);

	g_get_current_time (&currtime);
	hk->held_secs = currtime.tv_sec - hk->tv_sec;
	g_print ("\t%ld %ld %ld\n", hk->held_secs, currtime.tv_sec, hk->tv_sec);
	g_signal_emit_by_name (input_obj, "event", event_source, "held", hk->held_secs);
       	return TRUE; /* Call me again */
}

