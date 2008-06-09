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

#include "power.h"
#include "helpers.h"
#include <dbus/dbus-glib.h>




struct _PowerPrivate {
	char* power_supply_node;
	char* status;
	GKeyFile* conf;
	gint max_energy;
	gint low_energy_threshold;
};

#define POWER_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TYPE_POWER, PowerPrivate))
enum  {
	POWER_DUMMY_PROPERTY
};
static gboolean power_poll_energy (Power* self);
static gboolean _power_poll_energy_gsource_func (gpointer self);
static GObject * power_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_properties);
static gpointer power_parent_class = NULL;
static gboolean _dbus_power_current_energy (Power* self, gint* result, GError** error);
static gboolean _dbus_power_get_max_energy (Power* self, gint* result, GError** error);
static gboolean _dbus_power_energy_full_design (Power* self, gint* result, GError** error);
static gboolean _dbus_power_battery_status (Power* self, char** result, GError** error);
static gboolean _dbus_power_type (Power* self, char** result, GError** error);
static gboolean _dbus_power_model_name (Power* self, char** result, GError** error);
static gboolean _dbus_power_manufacturer (Power* self, char** result, GError** error);
static gboolean _dbus_power_technology (Power* self, char** result, GError** error);
static void power_dispose (GObject * obj);


static void g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER (GClosure * closure, GValue * return_value, guint n_param_values, const GValue * param_values, gpointer invocation_hint, gpointer marshal_data);

gint power_current_energy (Power* self) {
	char* _tmp0;
	gint _tmp1;
	g_return_val_if_fail (IS_POWER (self), 0);
	_tmp0 = NULL;
	return (_tmp1 = odeviced_read_integer ((_tmp0 = g_strconcat (self->priv->power_supply_node, "/energy_now", NULL))), (_tmp0 = (g_free (_tmp0), NULL)), _tmp1);
}


gint power_get_max_energy (Power* self) {
	g_return_val_if_fail (IS_POWER (self), 0);
	return self->priv->max_energy;
}


gint power_energy_full_design (Power* self) {
	char* _tmp0;
	gint _tmp1;
	g_return_val_if_fail (IS_POWER (self), 0);
	_tmp0 = NULL;
	return (_tmp1 = odeviced_read_integer ((_tmp0 = g_strconcat (self->priv->power_supply_node, "/energy_full_design", NULL))), (_tmp0 = (g_free (_tmp0), NULL)), _tmp1);
}


char* power_battery_status (Power* self) {
	char* _tmp0;
	char* _tmp1;
	g_return_val_if_fail (IS_POWER (self), NULL);
	_tmp0 = NULL;
	_tmp1 = NULL;
	return (_tmp1 = odeviced_read_string ((_tmp0 = g_strconcat (self->priv->power_supply_node, "/status", NULL))), (_tmp0 = (g_free (_tmp0), NULL)), _tmp1);
}


char* power_type (Power* self) {
	char* _tmp0;
	char* _tmp1;
	g_return_val_if_fail (IS_POWER (self), NULL);
	_tmp0 = NULL;
	_tmp1 = NULL;
	return (_tmp1 = odeviced_read_string ((_tmp0 = g_strconcat (self->priv->power_supply_node, "/type", NULL))), (_tmp0 = (g_free (_tmp0), NULL)), _tmp1);
}


char* power_model_name (Power* self) {
	char* _tmp0;
	char* _tmp1;
	g_return_val_if_fail (IS_POWER (self), NULL);
	_tmp0 = NULL;
	_tmp1 = NULL;
	return (_tmp1 = odeviced_read_string ((_tmp0 = g_strconcat (self->priv->power_supply_node, "/model_name", NULL))), (_tmp0 = (g_free (_tmp0), NULL)), _tmp1);
}


char* power_manufacturer (Power* self) {
	char* _tmp0;
	char* _tmp1;
	g_return_val_if_fail (IS_POWER (self), NULL);
	_tmp0 = NULL;
	_tmp1 = NULL;
	return (_tmp1 = odeviced_read_string ((_tmp0 = g_strconcat (self->priv->power_supply_node, "/manufacturer", NULL))), (_tmp0 = (g_free (_tmp0), NULL)), _tmp1);
}


char* power_technology (Power* self) {
	char* _tmp0;
	char* _tmp1;
	g_return_val_if_fail (IS_POWER (self), NULL);
	_tmp0 = NULL;
	_tmp1 = NULL;
	return (_tmp1 = odeviced_read_string ((_tmp0 = g_strconcat (self->priv->power_supply_node, "/technology", NULL))), (_tmp0 = (g_free (_tmp0), NULL)), _tmp1);
}


static gboolean power_poll_energy (Power* self) {
	gint _curr;
	g_return_val_if_fail (IS_POWER (self), FALSE);
	_curr = power_current_energy (self);
	g_message ("power.vala:90: Current energy, %d", _curr);
	return TRUE;
}


/* This vala source exists to generate the boilerplate code
   $ valac -C powercontrol.vala --pkg dbus-glib-1*/
Power* power_new (void) {
	Power * self;
	self = g_object_newv (TYPE_POWER, 0, NULL);
	return self;
}


static gboolean _power_poll_energy_gsource_func (gpointer self) {
	return power_poll_energy (self);
}


static GObject * power_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_properties) {
	GObject * obj;
	PowerClass * klass;
	GObjectClass * parent_class;
	Power * self;
	GError * inner_error;
	klass = POWER_CLASS (g_type_class_peek (TYPE_POWER));
	parent_class = G_OBJECT_CLASS (g_type_class_peek_parent (klass));
	obj = parent_class->constructor (type, n_construct_properties, construct_properties);
	self = POWER (obj);
	inner_error = NULL;
	{
		g_timeout_add_seconds (((guint) (300)), _power_poll_energy_gsource_func, self);
		{
			char* dev;
			char* _tmp0;
			gint _min;
			char* _tmp1;
			dev = odeviced_get_device ();
			g_key_file_load_from_file (self->priv->conf, "/usr/share/odeviced/plugins/power.plugin", G_KEY_FILE_NONE, &inner_error);
			if (inner_error != NULL) {
				goto __catch6_g_error;
			}
			_tmp0 = NULL;
			self->priv->power_supply_node = (_tmp0 = g_key_file_get_string (self->priv->conf, dev, "power_supply_node", &inner_error), (self->priv->power_supply_node = (g_free (self->priv->power_supply_node), NULL)), _tmp0);
			if (inner_error != NULL) {
				goto __catch6_g_error;
			}
			_min = g_key_file_get_integer (self->priv->conf, dev, "low_energy_threshold", &inner_error);
			if (inner_error != NULL) {
				goto __catch6_g_error;
			}
			_tmp1 = NULL;
			self->priv->max_energy = odeviced_read_integer ((_tmp1 = g_strconcat (self->priv->power_supply_node, "/energy_full", NULL)));
			_tmp1 = (g_free (_tmp1), NULL);
			/* Prolly use this for warning during low battery */
			self->priv->low_energy_threshold = self->priv->max_energy * (_min / 100);
			dev = (g_free (dev), NULL);
		}
		goto __finally6;
		__catch6_g_error:
		{
			GError * error;
			error = inner_error;
			inner_error = NULL;
			{
				g_critical (error->message);
			}
		}
		__finally6:
		;
	}
	return obj;
}


static gboolean _dbus_power_current_energy (Power* self, gint* result, GError** error) {
	*result = power_current_energy (self);
	return !error || !*error;
}


static gboolean _dbus_power_get_max_energy (Power* self, gint* result, GError** error) {
	*result = power_get_max_energy (self);
	return !error || !*error;
}


static gboolean _dbus_power_energy_full_design (Power* self, gint* result, GError** error) {
	*result = power_energy_full_design (self);
	return !error || !*error;
}


static gboolean _dbus_power_battery_status (Power* self, char** result, GError** error) {
	*result = power_battery_status (self);
	return !error || !*error;
}


static gboolean _dbus_power_type (Power* self, char** result, GError** error) {
	*result = power_type (self);
	return !error || !*error;
}


static gboolean _dbus_power_model_name (Power* self, char** result, GError** error) {
	*result = power_model_name (self);
	return !error || !*error;
}


static gboolean _dbus_power_manufacturer (Power* self, char** result, GError** error) {
	*result = power_manufacturer (self);
	return !error || !*error;
}


static gboolean _dbus_power_technology (Power* self, char** result, GError** error) {
	*result = power_technology (self);
	return !error || !*error;
}


static void power_class_init (PowerClass * klass) {
	power_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (PowerPrivate));
	G_OBJECT_CLASS (klass)->constructor = power_constructor;
	G_OBJECT_CLASS (klass)->dispose = power_dispose;
	static const DBusGMethodInfo power_dbus_methods[] = {
{ (GCallback) _dbus_power_current_energy, g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER, 0 },
{ (GCallback) _dbus_power_get_max_energy, g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER, 73 },
{ (GCallback) _dbus_power_energy_full_design, g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER, 146 },
{ (GCallback) _dbus_power_battery_status, g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER, 223 },
{ (GCallback) _dbus_power_type, g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER, 296 },
{ (GCallback) _dbus_power_model_name, g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER, 359 },
{ (GCallback) _dbus_power_manufacturer, g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER, 428 },
{ (GCallback) _dbus_power_technology, g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER, 499 },
}
;
	static const DBusGObjectInfo power_dbus_object_info = { 0, power_dbus_methods, 8, "org.freesmartphone.Device.Plugins.Power\0current_energy\0S\0result\0O\0F\0N\0i\0\0org.freesmartphone.Device.Plugins.Power\0get_max_energy\0S\0result\0O\0F\0N\0i\0\0org.freesmartphone.Device.Plugins.Power\0energy_full_design\0S\0result\0O\0F\0N\0i\0\0org.freesmartphone.Device.Plugins.Power\0battery_status\0S\0result\0O\0F\0N\0s\0\0org.freesmartphone.Device.Plugins.Power\0type\0S\0result\0O\0F\0N\0s\0\0org.freesmartphone.Device.Plugins.Power\0model_name\0S\0result\0O\0F\0N\0s\0\0org.freesmartphone.Device.Plugins.Power\0manufacturer\0S\0result\0O\0F\0N\0s\0\0org.freesmartphone.Device.Plugins.Power\0technology\0S\0result\0O\0F\0N\0s\0\0", "", "" };
	dbus_g_object_type_install_info (TYPE_POWER, &power_dbus_object_info);
}


static void power_instance_init (Power * self) {
	self->priv = POWER_GET_PRIVATE (self);
	self->priv->power_supply_node = g_new0 (char, 1);
	self->priv->status = g_new0 (char, 1);
	self->priv->conf = g_key_file_new ();
	self->priv->max_energy = ((gint) (g_new0 (char, 1)));
}


static void power_dispose (GObject * obj) {
	Power * self;
	self = POWER (obj);
	self->priv->power_supply_node = (g_free (self->priv->power_supply_node), NULL);
	self->priv->status = (g_free (self->priv->status), NULL);
	(self->priv->conf == NULL ? NULL : (self->priv->conf = (g_key_file_free (self->priv->conf), NULL)));
	G_OBJECT_CLASS (power_parent_class)->dispose (obj);
}


GType power_get_type (void) {
	static GType power_type_id = 0;
	if (G_UNLIKELY (power_type_id == 0)) {
		static const GTypeInfo g_define_type_info = { sizeof (PowerClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) power_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (Power), 0, (GInstanceInitFunc) power_instance_init };
		power_type_id = g_type_register_static (G_TYPE_OBJECT, "Power", &g_define_type_info, 0);
	}
	return power_type_id;
}

G_MODULE_EXPORT gboolean power_init (ODevicedPlugin *plugin) {
	Power *powerobj;
	powerobj = power_new();
	odeviced_register_dbus_object (plugin, G_OBJECT(powerobj));
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



