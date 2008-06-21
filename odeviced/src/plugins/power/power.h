/* 
 * powercontrol.vala
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

#ifndef __POWER_H__
#define __POWER_H__

#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>
#include <string.h>

G_BEGIN_DECLS


#define TYPE_POWER (power_get_type ())
#define POWER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_POWER, Power))
#define POWER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_POWER, PowerClass))
#define IS_POWER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_POWER))
#define IS_POWER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_POWER))
#define POWER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_POWER, PowerClass))

typedef struct _Power Power;
typedef struct _PowerClass PowerClass;
typedef struct _PowerPrivate PowerPrivate;

/* This vala source exists to generate the boilerplate code
   $ valac -C powercontrol.vala --pkg dbus-glib-1*/
struct _Power {
	GObject parent_instance;
	PowerPrivate * priv;
};

struct _PowerClass {
	GObjectClass parent_class;
};


gint power_GetCurrentEnergy (Power* self);
gint power_GetMaxEnergy (Power* self);
gint power_GetEnergyFullDesign (Power* self);
char* power_GetBatteryStatus (Power* self);
char* power_GetName (Power* self);
char* power_GetType (Power* self);
char* power_GetModel (Power* self);
char* power_GetManufacturer (Power* self);
char* power_GetTechnology (Power* self);
gint power_GetEnergyPercentage (Power* self);
const char* power_get_node (Power* self);
const char* power_get_dbus_path (Power* self);
GType power_get_type (void);


G_END_DECLS

#endif
