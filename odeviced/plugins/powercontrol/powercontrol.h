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

#ifndef __POWERCONTROL_H__
#define __POWERCONTROL_H__

#include "plugin.h"
#include "helpers.h"

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS


#define TYPE_GENERIC_POWER_CONTROL (generic_power_control_get_type ())
#define GENERIC_POWER_CONTROL(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_GENERIC_POWER_CONTROL, GenericPowerControl))
#define GENERIC_POWER_CONTROL_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_GENERIC_POWER_CONTROL, GenericPowerControlClass))
#define IS_GENERIC_POWER_CONTROL(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_GENERIC_POWER_CONTROL))
#define IS_GENERIC_POWER_CONTROL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_GENERIC_POWER_CONTROL))
#define GENERIC_POWER_CONTROL_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_GENERIC_POWER_CONTROL, GenericPowerControlClass))

typedef struct _GenericPowerControl GenericPowerControl;
typedef struct _GenericPowerControlClass GenericPowerControlClass;
typedef struct _GenericPowerControlPrivate GenericPowerControlPrivate;

/* This vala source exists to generate the boilerplate code
   $ valac -C powercontrol.vala --pkg dbus-glib-1*/
struct _GenericPowerControl {
	GObject parent_instance;
	GenericPowerControlPrivate * priv;
};

struct _GenericPowerControlClass {
	GObjectClass parent_class;
};


gboolean generic_power_control_get_power (GenericPowerControl* self);
void generic_power_control_set_power (GenericPowerControl* self, gboolean power);
GenericPowerControl* generic_power_control_new (void);
GType generic_power_control_get_type (void);


G_END_DECLS

#endif
