/* 
 * backlight.vala
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

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>
#include <string.h>
#include <src/daemon/plugin.h>

G_BEGIN_DECLS


#define TYPE_DISPLAY (display_get_type ())
#define DISPLAY(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_DISPLAY, Display))
#define DISPLAY_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_DISPLAY, DisplayClass))
#define IS_DISPLAY(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_DISPLAY))
#define IS_DISPLAY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_DISPLAY))
#define DISPLAY_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_DISPLAY, DisplayClass))

typedef struct _Display Display;
typedef struct _DisplayClass DisplayClass;
typedef struct _DisplayPrivate DisplayPrivate;

struct _Display {
	GObject parent_instance;
	DisplayPrivate * priv;
};

struct _DisplayClass {
	GObjectClass parent_class;
};


gint display_GetMaximumBrightness (Display* self);
gboolean display_SetBrightness (Display* self, gint brightness);
gint display_GetBrightness (Display* self);
gboolean display_GetBacklightPower (Display* self);
void display_SetBacklightPower (Display* self, gboolean power);
char* display_GetName (Display* self);
const char* display_get_node (Display* self);
const char* display_get_dbus_path (Display* self);
GType display_get_type (void);
extern GList* display_list;
gboolean display_init (ODevicedPlugin* plugin);


G_END_DECLS

#endif
