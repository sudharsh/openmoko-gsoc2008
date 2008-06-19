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

#include "odeviced.h"
#include "power.h"
#include "helpers.h"
#include <dbus/dbus-glib.h>




struct _PowerPrivate {
	char* status;
	GKeyFile* conf;
	gint max_energy;
	gint low_energy_threshold;
	gint status_poll_interval;
	char* curr_status;
	char* _node;
	char* _dbus_path;
};

#define POWER_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TYPE_POWER, PowerPrivate))
enum  {
	POWER_DUMMY_PROPERTY,
	POWER_NODE,
	POWER_DBUS_PATH
};
static Power* power_new (const char* node, const char* dbus_path);
static gboolean power_poll_energy (Power* self);
static gboolean power_poll_status (Power* self);
static void power_set_node (Power* self, const char* value);
static void power_set_dbus_path (Power* self, const char* value);
static gboolean _power_poll_energy_gsource_func (gpointer self);
static gboolean _power_poll_status_gsource_func (gpointer self);
static GObject * power_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_properties);
static gpointer power_parent_class = NULL;
static gboolean _dbus_power_GetCurrentEnergy (Power* self, gint* result, GError** error);
static gboolean _dbus_power_GetMaxEnergy (Power* self, gint* result, GError** error);
static gboolean _dbus_power_GetEnergyFullDesign (Power* self, gint* result, GError** error);
static gboolean _dbus_power_GetBatteryStatus (Power* self, char** result, GError** error);
static gboolean _dbus_power_GetType (Power* self, char** result, GError** error);
static gboolean _dbus_power_GetModel (Power* self, char** result, GError** error);
static gboolean _dbus_power_GetManufacturer (Power* self, char** result, GError** error);
static gboolean _dbus_power_GetTechnology (Power* self, char** result, GError** error);
static gboolean _dbus_power_GetEnergyPercentage (Power* self, double* result, GError** error);
static void power_dispose (GObject * obj);
static int _vala_strcmp0 (const char * str1, const char * str2);


static void g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER (GClosure * closure, GValue * return_value, guint n_param_values, const GValue * param_values, gpointer invocation_hint, gpointer marshal_data);

static Power* power_new (const char* node, const char* dbus_path) {
	GParameter * __params;
	GParameter * __params_it;
	Power * self;
	g_return_val_if_fail (node != NULL, NULL);
	g_return_val_if_fail (dbus_path != NULL, NULL);
	__params = g_new0 (GParameter, 2);
	__params_it = __params;
	__params_it->name = "node";
	g_value_init (&__params_it->value, G_TYPE_STRING);
	g_value_set_string (&__params_it->value, node);
	__params_it++;
	__params_it->name = "dbus-path";
	g_value_init (&__params_it->value, G_TYPE_STRING);
	g_value_set_string (&__params_it->value, dbus_path);
	__params_it++;
	self = g_object_newv (TYPE_POWER, __params_it - __params, __params);
	while (__params_it > __params) {
		--__params_it;
		g_value_unset (&__params_it->value);
	}
	g_free (__params);
	return self;
}


gint power_GetCurrentEnergy (Power* self) {
	char* _tmp0;
	gint _tmp1;
	g_return_val_if_fail (IS_POWER (self), 0);
	_tmp0 = NULL;
	return (_tmp1 = odeviced_read_integer ((_tmp0 = g_strconcat (self->priv->_node, "/energy_now", NULL))), (_tmp0 = (g_free (_tmp0), NULL)), _tmp1);
}


gint power_GetMaxEnergy (Power* self) {
	g_return_val_if_fail (IS_POWER (self), 0);
	return self->priv->max_energy;
}


gint power_GetEnergyFullDesign (Power* self) {
	char* _tmp0;
	gint _tmp1;
	g_return_val_if_fail (IS_POWER (self), 0);
	_tmp0 = NULL;
	return (_tmp1 = odeviced_read_integer ((_tmp0 = g_strconcat (self->priv->_node, "/energy_full_design", NULL))), (_tmp0 = (g_free (_tmp0), NULL)), _tmp1);
}


char* power_GetBatteryStatus (Power* self) {
	char* _tmp0;
	char* _tmp1;
	g_return_val_if_fail (IS_POWER (self), NULL);
	_tmp0 = NULL;
	_tmp1 = NULL;
	return (_tmp1 = odeviced_read_string ((_tmp0 = g_strconcat (self->priv->_node, "/status", NULL))), (_tmp0 = (g_free (_tmp0), NULL)), _tmp1);
}


char* power_GetType (Power* self) {
	char* _tmp0;
	char* _tmp1;
	g_return_val_if_fail (IS_POWER (self), NULL);
	_tmp0 = NULL;
	_tmp1 = NULL;
	return (_tmp1 = odeviced_read_string ((_tmp0 = g_strconcat (self->priv->_node, "/type", NULL))), (_tmp0 = (g_free (_tmp0), NULL)), _tmp1);
}


char* power_GetModel (Power* self) {
	char* _tmp0;
	char* _tmp1;
	g_return_val_if_fail (IS_POWER (self), NULL);
	_tmp0 = NULL;
	_tmp1 = NULL;
	return (_tmp1 = odeviced_read_string ((_tmp0 = g_strconcat (self->priv->_node, "/model_name", NULL))), (_tmp0 = (g_free (_tmp0), NULL)), _tmp1);
}


char* power_GetManufacturer (Power* self) {
	char* _tmp0;
	char* _tmp1;
	g_return_val_if_fail (IS_POWER (self), NULL);
	_tmp0 = NULL;
	_tmp1 = NULL;
	return (_tmp1 = odeviced_read_string ((_tmp0 = g_strconcat (self->priv->_node, "/manufacturer", NULL))), (_tmp0 = (g_free (_tmp0), NULL)), _tmp1);
}


char* power_GetTechnology (Power* self) {
	char* _tmp0;
	char* _tmp1;
	g_return_val_if_fail (IS_POWER (self), NULL);
	_tmp0 = NULL;
	_tmp1 = NULL;
	return (_tmp1 = odeviced_read_string ((_tmp0 = g_strconcat (self->priv->_node, "/technology", NULL))), (_tmp0 = (g_free (_tmp0), NULL)), _tmp1);
}


double power_GetEnergyPercentage (Power* self) {
	gint _curr;
	g_return_val_if_fail (IS_POWER (self), 0.0);
	_curr = power_GetCurrentEnergy (self);
	return 100.0 * (((float) (_curr)) / ((float) (self->priv->max_energy)));
}


static gboolean power_poll_energy (Power* self) {
	gint _curr;
	g_return_val_if_fail (IS_POWER (self), FALSE);
	_curr = power_GetCurrentEnergy (self);
	if (_curr == -1) {
		return FALSE;
	}
	g_message ("power.vala:122: Current energy, %d", _curr);
	if (_curr < self->priv->low_energy_threshold) {
		g_message ("power.vala:124: \tLow energy warning");
		g_signal_emit_by_name (G_OBJECT (self), "low-battery", _curr);
	}
	return TRUE;
}


static gboolean power_poll_status (Power* self) {
	char* stat;
	gboolean _tmp3;
	g_return_val_if_fail (IS_POWER (self), FALSE);
	stat = power_GetBatteryStatus (self);
	if (stat == NULL) {
		gboolean _tmp0;
		return (_tmp0 = FALSE, (stat = (g_free (stat), NULL)), _tmp0);
	}
	if (_vala_strcmp0 (stat, self->priv->curr_status) != 0) {
		char* _tmp2;
		const char* _tmp1;
		g_message ("power.vala:136: \tStatus changed, %s", stat);
		g_signal_emit_by_name (G_OBJECT (self), "battery-status-changed", stat);
		_tmp2 = NULL;
		_tmp1 = NULL;
		self->priv->curr_status = (_tmp2 = (_tmp1 = stat, (_tmp1 == NULL ? NULL : g_strdup (_tmp1))), (self->priv->curr_status = (g_free (self->priv->curr_status), NULL)), _tmp2);
	}
	return (_tmp3 = TRUE, (stat = (g_free (stat), NULL)), _tmp3);
}


const char* power_get_node (Power* self) {
	g_return_val_if_fail (IS_POWER (self), NULL);
	return self->priv->_node;
}


static void power_set_node (Power* self, const char* value) {
	char* _tmp2;
	const char* _tmp1;
	g_return_if_fail (IS_POWER (self));
	_tmp2 = NULL;
	_tmp1 = NULL;
	self->priv->_node = (_tmp2 = (_tmp1 = value, (_tmp1 == NULL ? NULL : g_strdup (_tmp1))), (self->priv->_node = (g_free (self->priv->_node), NULL)), _tmp2);
}


const char* power_get_dbus_path (Power* self) {
	g_return_val_if_fail (IS_POWER (self), NULL);
	return self->priv->_dbus_path;
}


static void power_set_dbus_path (Power* self, const char* value) {
	char* _tmp2;
	const char* _tmp1;
	g_return_if_fail (IS_POWER (self));
	_tmp2 = NULL;
	_tmp1 = NULL;
	self->priv->_dbus_path = (_tmp2 = (_tmp1 = value, (_tmp1 == NULL ? NULL : g_strdup (_tmp1))), (self->priv->_dbus_path = (g_free (self->priv->_dbus_path), NULL)), _tmp2);
}


static gboolean _power_poll_energy_gsource_func (gpointer self) {
	return power_poll_energy (self);
}


static gboolean _power_poll_status_gsource_func (gpointer self) {
	return power_poll_status (self);
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
			gint _min;
			char* _tmp0;
			char* _tmp1;
			dev = odeviced_get_device ();
			g_key_file_load_from_file (self->priv->conf, "/usr/share/odeviced/plugins/power.plugin", G_KEY_FILE_NONE, &inner_error);
			if (inner_error != NULL) {
				goto __catch7_g_error;
			}
			_min = g_key_file_get_integer (self->priv->conf, dev, "low_energy_threshold", &inner_error);
			if (inner_error != NULL) {
				goto __catch7_g_error;
			}
			self->priv->status_poll_interval = g_key_file_get_integer (self->priv->conf, dev, "status_poll_interval", &inner_error);
			if (inner_error != NULL) {
				goto __catch7_g_error;
			}
			/*this.power_supply_node = conf.get_string(dev, "power_supply_node");*/
			_tmp0 = NULL;
			self->priv->max_energy = odeviced_read_integer ((_tmp0 = g_strconcat (self->priv->_node, "/energy_full", NULL)));
			_tmp0 = (g_free (_tmp0), NULL);
			/* Prolly use this for warning during low battery */
			self->priv->low_energy_threshold = self->priv->max_energy * (_min / 100);
			g_timeout_add_seconds (((guint) (self->priv->status_poll_interval)), _power_poll_status_gsource_func, self);
			_tmp1 = NULL;
			self->priv->curr_status = (_tmp1 = power_GetBatteryStatus (self), (self->priv->curr_status = (g_free (self->priv->curr_status), NULL)), _tmp1);
			dev = (g_free (dev), NULL);
		}
		goto __finally7;
		__catch7_g_error:
		{
			GError * error;
			error = inner_error;
			inner_error = NULL;
			{
				g_critical (error->message);
			}
		}
		__finally7:
		;
	}
	return obj;
}


static void power_get_property (GObject * object, guint property_id, GValue * value, GParamSpec * pspec) {
	Power * self;
	self = POWER (object);
	switch (property_id) {
		case POWER_NODE:
		g_value_set_string (value, power_get_node (self));
		break;
		case POWER_DBUS_PATH:
		g_value_set_string (value, power_get_dbus_path (self));
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}


static void power_set_property (GObject * object, guint property_id, const GValue * value, GParamSpec * pspec) {
	Power * self;
	self = POWER (object);
	switch (property_id) {
		case POWER_NODE:
		power_set_node (self, g_value_get_string (value));
		break;
		case POWER_DBUS_PATH:
		power_set_dbus_path (self, g_value_get_string (value));
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}


static gboolean _dbus_power_GetCurrentEnergy (Power* self, gint* result, GError** error) {
	*result = power_GetCurrentEnergy (self);
	return !error || !*error;
}


static gboolean _dbus_power_GetMaxEnergy (Power* self, gint* result, GError** error) {
	*result = power_GetMaxEnergy (self);
	return !error || !*error;
}


static gboolean _dbus_power_GetEnergyFullDesign (Power* self, gint* result, GError** error) {
	*result = power_GetEnergyFullDesign (self);
	return !error || !*error;
}


static gboolean _dbus_power_GetBatteryStatus (Power* self, char** result, GError** error) {
	*result = power_GetBatteryStatus (self);
	return !error || !*error;
}


static gboolean _dbus_power_GetType (Power* self, char** result, GError** error) {
	*result = power_GetType (self);
	return !error || !*error;
}


static gboolean _dbus_power_GetModel (Power* self, char** result, GError** error) {
	*result = power_GetModel (self);
	return !error || !*error;
}


static gboolean _dbus_power_GetManufacturer (Power* self, char** result, GError** error) {
	*result = power_GetManufacturer (self);
	return !error || !*error;
}


static gboolean _dbus_power_GetTechnology (Power* self, char** result, GError** error) {
	*result = power_GetTechnology (self);
	return !error || !*error;
}


static gboolean _dbus_power_GetEnergyPercentage (Power* self, double* result, GError** error) {
	*result = power_GetEnergyPercentage (self);
	return !error || !*error;
}


static void power_class_init (PowerClass * klass) {
	power_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (PowerPrivate));
	G_OBJECT_CLASS (klass)->get_property = power_get_property;
	G_OBJECT_CLASS (klass)->set_property = power_set_property;
	G_OBJECT_CLASS (klass)->constructor = power_constructor;
	G_OBJECT_CLASS (klass)->dispose = power_dispose;
	g_object_class_install_property (G_OBJECT_CLASS (klass), POWER_NODE, g_param_spec_string ("node", "node", "node", NULL, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
	g_object_class_install_property (G_OBJECT_CLASS (klass), POWER_DBUS_PATH, g_param_spec_string ("dbus-path", "dbus-path", "dbus-path", NULL, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
	g_signal_new ("battery_status_changed", TYPE_POWER, G_SIGNAL_RUN_LAST, 0, NULL, NULL, g_cclosure_marshal_VOID__STRING, G_TYPE_NONE, 1, G_TYPE_STRING);
	g_signal_new ("low_battery", TYPE_POWER, G_SIGNAL_RUN_LAST, 0, NULL, NULL, g_cclosure_marshal_VOID__INT, G_TYPE_NONE, 1, G_TYPE_INT);
	static const DBusGMethodInfo power_dbus_methods[] = {
{ (GCallback) _dbus_power_GetCurrentEnergy, g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER, 0 },
{ (GCallback) _dbus_power_GetMaxEnergy, g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER, 73 },
{ (GCallback) _dbus_power_GetEnergyFullDesign, g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER, 142 },
{ (GCallback) _dbus_power_GetBatteryStatus, g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER, 218 },
{ (GCallback) _dbus_power_GetType, g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER, 291 },
{ (GCallback) _dbus_power_GetModel, g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER, 355 },
{ (GCallback) _dbus_power_GetManufacturer, g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER, 420 },
{ (GCallback) _dbus_power_GetTechnology, g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER, 492 },
{ (GCallback) _dbus_power_GetEnergyPercentage, g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER, 562 },
}
;
	static const DBusGObjectInfo power_dbus_object_info = { 0, power_dbus_methods, 9, "org.freesmartphone.Device.PowerSupply\0GetCurrentEnergy\0S\0result\0O\0F\0N\0i\0\0org.freesmartphone.Device.PowerSupply\0GetMaxEnergy\0S\0result\0O\0F\0N\0i\0\0org.freesmartphone.Device.PowerSupply\0GetEnergyFullDesign\0S\0result\0O\0F\0N\0i\0\0org.freesmartphone.Device.PowerSupply\0GetBatteryStatus\0S\0result\0O\0F\0N\0s\0\0org.freesmartphone.Device.PowerSupply\0GetType\0S\0result\0O\0F\0N\0s\0\0org.freesmartphone.Device.PowerSupply\0GetModel\0S\0result\0O\0F\0N\0s\0\0org.freesmartphone.Device.PowerSupply\0GetManufacturer\0S\0result\0O\0F\0N\0s\0\0org.freesmartphone.Device.PowerSupply\0GetTechnology\0S\0result\0O\0F\0N\0s\0\0org.freesmartphone.Device.PowerSupply\0GetEnergyPercentage\0S\0result\0O\0F\0N\0d\0\0", "org.freesmartphone.Device.PowerSupply\0battery_status_changed\0org.freesmartphone.Device.PowerSupply\0low_battery\0", "" };
	dbus_g_object_type_install_info (TYPE_POWER, &power_dbus_object_info);
}


static void power_instance_init (Power * self) {
	self->priv = POWER_GET_PRIVATE (self);
	self->priv->status = g_new0 (char, 1);
	self->priv->conf = g_key_file_new ();
}


static void power_dispose (GObject * obj) {
	Power * self;
	self = POWER (obj);
	self->priv->status = (g_free (self->priv->status), NULL);
	(self->priv->conf == NULL ? NULL : (self->priv->conf = (g_key_file_free (self->priv->conf), NULL)));
	self->priv->curr_status = (g_free (self->priv->curr_status), NULL);
	self->priv->_node = (g_free (self->priv->_node), NULL);
	self->priv->_dbus_path = (g_free (self->priv->_dbus_path), NULL);
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

static void register_dbus (Power* obj) {
	g_return_if_fail (IS_POWER (obj));
	g_message ("power.vala:158: Registering DBus object at %s", power_get_dbus_path (obj));
	dbus_g_connection_register_g_object (odeviced_connection, power_get_dbus_path (obj), G_OBJECT (obj));
}

G_MODULE_EXPORT gboolean power_init (ODevicedPlugin *plugin) {
	GType type;
	GList *list = NULL;
	Power *obj;
	type = power_get_type();
	list = odeviced_compute_objects (plugin, type);

	if (!list)
		return FALSE;

	g_list_foreach(list, (GFunc)register_dbus, NULL);	
	return TRUE;
}


static int _vala_strcmp0 (const char * str1, const char * str2) {
	if (str1 == NULL) {
		return -(str1 != str2);
	}
	if (str2 == NULL) {
		return (str1 != str2);
	}
	return strcmp (str1, str2);
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



