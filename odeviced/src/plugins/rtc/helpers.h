/* 
 *  helpers.vala
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

#ifndef __HELPERS_H__
#define __HELPERS_H__

#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>
#include <string.h>
#include "plugin.h"

G_BEGIN_DECLS



void odeviced_register_dbus_object (ODevicedPlugin* plugin, GObject* interface_obj);
char* odeviced_get_device (void);
GList* odeviced_compute_objects (ODevicedPlugin* plugin, GType klass);
char* odeviced_compute_name (const char* path);
gint odeviced_read_integer (const char* node);
char* odeviced_read_string (const char* node);
gboolean odeviced_write_integer (const char* node, gint val);


G_END_DECLS

#endif
