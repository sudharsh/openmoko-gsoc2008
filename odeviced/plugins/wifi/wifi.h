/* 
 * wifi.vala
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

#ifndef __WIFI_H__
#define __WIFI_H__

#include "helpers.h"

#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>

#include <linux/if.h>
#include <linux/wireless.h>

G_BEGIN_DECLS


#define TYPE_WIFI_PLUGIN (wifi_plugin_get_type ())
#define WIFI_PLUGIN(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_WIFI_PLUGIN, WifiPlugin))
#define WIFI_PLUGIN_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_WIFI_PLUGIN, WifiPluginClass))
#define IS_WIFI_PLUGIN(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_WIFI_PLUGIN))
#define IS_WIFI_PLUGIN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_WIFI_PLUGIN))
#define WIFI_PLUGIN_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_WIFI_PLUGIN, WifiPluginClass))

typedef struct _WifiPlugin WifiPlugin;
typedef struct _WifiPluginClass WifiPluginClass;
typedef struct _WifiPluginPrivate WifiPluginPrivate;

/* This file exists to generate the wifi.[c/h] for modifications */
struct _WifiPlugin {
	GObject parent_instance;
	WifiPluginPrivate * priv;
};

struct _WifiPluginClass {
	GObjectClass parent_class;
};


gboolean wifi_plugin_is_on (WifiPlugin* self, const char* iface);
gboolean wifi_plugin_control (WifiPlugin* self, const char* iface, gboolean enable);
WifiPlugin* wifi_plugin_new (void);
GType wifi_plugin_get_type (void);
gboolean wifi_init (ODevicedPlugin *plugin);
gboolean wifi_close (ODevicedPlugin *plugin);


G_END_DECLS

#endif
