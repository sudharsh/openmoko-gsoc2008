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

#ifndef __BACKLIGHT_H__
#define __BACKLIGHT_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS


#define TYPE_BACKLIGHT_PLUGIN (backlight_plugin_get_type ())
#define BACKLIGHT_PLUGIN(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_BACKLIGHT_PLUGIN, BacklightPlugin))
#define BACKLIGHT_PLUGIN_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_BACKLIGHT_PLUGIN, BacklightPluginClass))
#define IS_BACKLIGHT_PLUGIN(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_BACKLIGHT_PLUGIN))
#define IS_BACKLIGHT_PLUGIN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_BACKLIGHT_PLUGIN))
#define BACKLIGHT_PLUGIN_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_BACKLIGHT_PLUGIN, BacklightPluginClass))

typedef struct _BacklightPlugin BacklightPlugin;
typedef struct _BacklightPluginClass BacklightPluginClass;
typedef struct _BacklightPluginPrivate BacklightPluginPrivate;

/*
 * This file exists to generate the sources.
 * $ valac -C backlight.vala --pkg dbus-glib-1
 */
struct _BacklightPlugin {
	GObject parent_instance;
	BacklightPluginPrivate * priv;
};

struct _BacklightPluginClass {
	GObjectClass parent_class;
};


gint backlight_plugin_get_max_brightness (BacklightPlugin* self);
gboolean backlight_plugin_set_brightness (BacklightPlugin* self);
BacklightPlugin* backlight_plugin_new (void);
GType backlight_plugin_get_type (void);


G_END_DECLS

#endif
