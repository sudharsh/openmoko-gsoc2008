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

#include "powercontrol.h"
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus-glib.h>




enum  {
	GENERIC_POWER_CONTROL_DUMMY_PROPERTY
};
static gpointer generic_power_control_parent_class = NULL;
static gboolean _dbus_generic_power_control_get_power (GenericPowerControl* self, gboolean* result, GError** error);
static gboolean _dbus_generic_power_control_set_power (GenericPowerControl* self, gboolean power, GError** error);


static void g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER (GClosure * closure, GValue * return_value, guint n_param_values, const GValue * param_values, gpointer invocation_hint, gpointer marshal_data);
static void g_cclosure_user_marshal_BOOLEAN__BOOLEAN_POINTER (GClosure * closure, GValue * return_value, guint n_param_values, const GValue * param_values, gpointer invocation_hint, gpointer marshal_data);

gboolean generic_power_control_get_power (GenericPowerControl* self) {
	g_return_val_if_fail (IS_GENERIC_POWER_CONTROL (self), FALSE);
	return TRUE;
}


void generic_power_control_set_power (GenericPowerControl* self, gboolean power) {
	g_return_if_fail (IS_GENERIC_POWER_CONTROL (self));
	g_print ("hi");
}


/* This vala source exists to generate the boilerplate code
   $ valac -C powercontrol.vala --pkg dbus-glib-1*/
GenericPowerControl* generic_power_control_new (void) {
	GenericPowerControl * self;
	self = g_object_newv (TYPE_GENERIC_POWER_CONTROL, 0, NULL);
	return self;
}


static gboolean _dbus_generic_power_control_get_power (GenericPowerControl* self, gboolean* result, GError** error) {
	*result = generic_power_control_get_power (self);
	return !error || !*error;
}


static gboolean _dbus_generic_power_control_set_power (GenericPowerControl* self, gboolean power, GError** error) {
	generic_power_control_set_power (self, power);
	return !error || !*error;
}


static void generic_power_control_class_init (GenericPowerControlClass * klass) {
	generic_power_control_parent_class = g_type_class_peek_parent (klass);
	static const DBusGMethodInfo generic_power_control_dbus_methods[] = {
{ (GCallback) _dbus_generic_power_control_get_power, g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER, 0 },
{ (GCallback) _dbus_generic_power_control_set_power, g_cclosure_user_marshal_BOOLEAN__BOOLEAN_POINTER, 75 },
}
;
	static const DBusGObjectInfo generic_power_control_dbus_object_info = { 0, generic_power_control_dbus_methods, 2, "org.freesmartphone.Device.Plugins.Powercontrol\0get_power\0S\0result\0O\0F\0N\0b\0\0org.freesmartphone.Device.Plugins.Powercontrol\0set_power\0S\0power\0I\0b\0\0", "", "" };
	dbus_g_object_type_install_info (TYPE_GENERIC_POWER_CONTROL, &generic_power_control_dbus_object_info);
}


static void generic_power_control_instance_init (GenericPowerControl * self) {
}


GType generic_power_control_get_type (void) {
	static GType generic_power_control_type_id = 0;
	if (G_UNLIKELY (generic_power_control_type_id == 0)) {
		static const GTypeInfo g_define_type_info = { sizeof (GenericPowerControlClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) generic_power_control_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (GenericPowerControl), 0, (GInstanceInitFunc) generic_power_control_instance_init };
		generic_power_control_type_id = g_type_register_static (G_TYPE_OBJECT, "GenericPowerControl", &g_define_type_info, 0);
	}
	return generic_power_control_type_id;
}

G_MODULE_EXPORT gboolean powercontrol_init (ODevicedPlugin *plugin) {
	GenericPowerControl *powerobj;
	powerobj = generic_power_control_new();
	plugin->dbus_object_path = g_strdup ("/org/freesmartphone/Device/Plugins/PowerControl");
	odeviced_helpers_register_dbus_object (plugin, G_OBJECT(powerobj));
	return TRUE;
}


G_MODULE_EXPORT gboolean powercontrol_close (ODevicedPlugin *plugin) {
	return TRUE;
}


static void g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER (GClosure * closure, GValue * return_value, guint n_param_values, const GValue * param_values, gpointer invocation_hint, gpointer marshal_data) {
	typedef gboolean (*GMarshalFunc_BOOLEAN__POINTER_POINTER) (gpointer data1, gpointer arg_1, gpointer arg_2, gpointer data2);
	register GMarshalFunc_BOOLEAN__POINTER_POINTER callback;
	register GCClosure * cc;
	register gpointer data1, data2;
	gboolean v_return;
	cc = ((GCClosure *) (closure));
	g_return_if_fail (return_value != NULL);
	g_return_if_fail (n_param_values == 3);
	if (G_CCLOSURE_SWAP_DATA (closure)) {
		data1 = closure->data;
		data2 = param_values->data[0].v_pointer;
	} else {
		data1 = param_values->data[0].v_pointer;
		data2 = closure->data;
	}
	callback = ((GMarshalFunc_BOOLEAN__POINTER_POINTER) ((marshal_data ? marshal_data : cc->callback)));
	v_return = callback (data1, g_value_get_pointer (param_values + 1), g_value_get_pointer (param_values + 2), data2);
	g_value_set_boolean (return_value, v_return);
}


static void g_cclosure_user_marshal_BOOLEAN__BOOLEAN_POINTER (GClosure * closure, GValue * return_value, guint n_param_values, const GValue * param_values, gpointer invocation_hint, gpointer marshal_data) {
	typedef gboolean (*GMarshalFunc_BOOLEAN__BOOLEAN_POINTER) (gpointer data1, gboolean arg_1, gpointer arg_2, gpointer data2);
	register GMarshalFunc_BOOLEAN__BOOLEAN_POINTER callback;
	register GCClosure * cc;
	register gpointer data1, data2;
	gboolean v_return;
	cc = ((GCClosure *) (closure));
	g_return_if_fail (return_value != NULL);
	g_return_if_fail (n_param_values == 3);
	if (G_CCLOSURE_SWAP_DATA (closure)) {
		data1 = closure->data;
		data2 = param_values->data[0].v_pointer;
	} else {
		data1 = param_values->data[0].v_pointer;
		data2 = closure->data;
	}
	callback = ((GMarshalFunc_BOOLEAN__BOOLEAN_POINTER) ((marshal_data ? marshal_data : cc->callback)));
	v_return = callback (data1, g_value_get_boolean (param_values + 1), g_value_get_pointer (param_values + 2), data2);
	g_value_set_boolean (return_value, v_return);
}



