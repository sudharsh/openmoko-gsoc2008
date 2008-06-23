/* 
 *  plugins.vala
 *  Author: Sudharshan "Sup3rkiddo" sudharsh@gmail.com>
 *  All Rights Reserved
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */

#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>
#include <string.h>
#include <gmodule.h>

G_BEGIN_DECLS


#define ODEVICED_TYPE_PLUGIN (odeviced_plugin_get_type ())
#define ODEVICED_PLUGIN(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), ODEVICED_TYPE_PLUGIN, ODevicedPlugin))
#define ODEVICED_PLUGIN_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), ODEVICED_TYPE_PLUGIN, ODevicedPluginClass))
#define ODEVICED_IS_PLUGIN(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ODEVICED_TYPE_PLUGIN))
#define ODEVICED_IS_PLUGIN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), ODEVICED_TYPE_PLUGIN))
#define ODEVICED_PLUGIN_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), ODEVICED_TYPE_PLUGIN, ODevicedPluginClass))

typedef struct _ODevicedPlugin ODevicedPlugin;
typedef struct _ODevicedPluginClass ODevicedPluginClass;
typedef struct _ODevicedPluginPrivate ODevicedPluginPrivate;

typedef enum  {
	ODEVICED_PLUGIN_ERROR_LOAD_ERROR
} ODevicedPluginError;
#define ODEVICED_PLUGIN_ERROR odeviced_plugin_error_quark ()
struct _ODevicedPlugin {
	GObject parent_instance;
	ODevicedPluginPrivate * priv;
	GKeyFile* conf;
};

struct _ODevicedPluginClass {
	GObjectClass parent_class;
};


GQuark odeviced_plugin_error_quark (void);
ODevicedPlugin* odeviced_plugin_new (const char* name, const char* path);
gboolean odeviced_plugin_register (ODevicedPlugin* self, GError** error);
GModule* odeviced_plugin_get_library (ODevicedPlugin* self);
gint odeviced_plugin_get_handle (ODevicedPlugin* self);
const char* odeviced_plugin_get_path (ODevicedPlugin* self);
const char* odeviced_plugin_get_name (ODevicedPlugin* self);
GType odeviced_plugin_get_type (void);


G_END_DECLS

#endif
