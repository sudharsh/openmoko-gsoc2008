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

#include "backlight.h"
#include <dbus/dbus-glib-lowlevel.h>
#include <dbus/dbus-glib.h>
#include "helpers.h"
#include "odeviced.h"
#include <dbus/dbus-glib.h>




struct _BacklightPluginPrivate {
	gint max_brightness;
	char* _node;
	char* _dbus_path;
};

#define BACKLIGHT_PLUGIN_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TYPE_BACKLIGHT_PLUGIN, BacklightPluginPrivate))
enum  {
	BACKLIGHT_PLUGIN_DUMMY_PROPERTY,
	BACKLIGHT_PLUGIN_NODE,
	BACKLIGHT_PLUGIN_DBUS_PATH
};
static BacklightPlugin* backlight_plugin_new (const char* node, const char* dbus_path);
static void backlight_plugin_set_node (BacklightPlugin* self, const char* value);
static void backlight_plugin_set_dbus_path (BacklightPlugin* self, const char* value);
static GObject * backlight_plugin_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_properties);
static gpointer backlight_plugin_parent_class = NULL;
static gboolean _dbus_backlight_plugin_GetMaximumBrightness (BacklightPlugin* self, gint* result, GError** error);
static gboolean _dbus_backlight_plugin_SetBrightness (BacklightPlugin* self, gint brightness, gboolean* result, GError** error);
static gboolean _dbus_backlight_plugin_GetCurrentBrightness (BacklightPlugin* self, gint* result, GError** error);
static void backlight_plugin_dispose (GObject * obj);
static void register_dbus (BacklightPlugin* obj);


static void g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER (GClosure * closure, GValue * return_value, guint n_param_values, const GValue * param_values, gpointer invocation_hint, gpointer marshal_data);
static void g_cclosure_user_marshal_BOOLEAN__INT_POINTER_POINTER (GClosure * closure, GValue * return_value, guint n_param_values, const GValue * param_values, gpointer invocation_hint, gpointer marshal_data);

static BacklightPlugin* backlight_plugin_new (const char* node, const char* dbus_path) {
	GParameter * __params;
	GParameter * __params_it;
	BacklightPlugin * self;
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
	self = g_object_newv (TYPE_BACKLIGHT_PLUGIN, __params_it - __params, __params);
	while (__params_it > __params) {
		--__params_it;
		g_value_unset (&__params_it->value);
	}
	g_free (__params);
	return self;
}


gint backlight_plugin_GetMaximumBrightness (BacklightPlugin* self) {
	g_return_val_if_fail (IS_BACKLIGHT_PLUGIN (self), 0);
	return self->priv->max_brightness;
}


gboolean backlight_plugin_SetBrightness (BacklightPlugin* self, gint brightness) {
	char* _tmp1;
	g_return_val_if_fail (IS_BACKLIGHT_PLUGIN (self), FALSE);
	if (brightness > self->priv->max_brightness) {
		return FALSE;
	}
	_tmp1 = NULL;
	odeviced_write_integer ((_tmp1 = g_strconcat (self->priv->_node, "/brightness", NULL)), brightness);
	_tmp1 = (g_free (_tmp1), NULL);
	return TRUE;
}


gint backlight_plugin_GetCurrentBrightness (BacklightPlugin* self) {
	char* _tmp0;
	gint _tmp1;
	g_return_val_if_fail (IS_BACKLIGHT_PLUGIN (self), 0);
	_tmp0 = NULL;
	return (_tmp1 = odeviced_read_integer ((_tmp0 = g_strconcat (self->priv->_node, "/actual_brightness", NULL))), (_tmp0 = (g_free (_tmp0), NULL)), _tmp1);
}


const char* backlight_plugin_get_node (BacklightPlugin* self) {
	g_return_val_if_fail (IS_BACKLIGHT_PLUGIN (self), NULL);
	return self->priv->_node;
}


static void backlight_plugin_set_node (BacklightPlugin* self, const char* value) {
	char* _tmp2;
	const char* _tmp1;
	g_return_if_fail (IS_BACKLIGHT_PLUGIN (self));
	_tmp2 = NULL;
	_tmp1 = NULL;
	self->priv->_node = (_tmp2 = (_tmp1 = value, (_tmp1 == NULL ? NULL : g_strdup (_tmp1))), (self->priv->_node = (g_free (self->priv->_node), NULL)), _tmp2);
}


const char* backlight_plugin_get_dbus_path (BacklightPlugin* self) {
	g_return_val_if_fail (IS_BACKLIGHT_PLUGIN (self), NULL);
	return self->priv->_dbus_path;
}


static void backlight_plugin_set_dbus_path (BacklightPlugin* self, const char* value) {
	char* _tmp2;
	const char* _tmp1;
	g_return_if_fail (IS_BACKLIGHT_PLUGIN (self));
	_tmp2 = NULL;
	_tmp1 = NULL;
	self->priv->_dbus_path = (_tmp2 = (_tmp1 = value, (_tmp1 == NULL ? NULL : g_strdup (_tmp1))), (self->priv->_dbus_path = (g_free (self->priv->_dbus_path), NULL)), _tmp2);
}


static GObject * backlight_plugin_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_properties) {
	GObject * obj;
	BacklightPluginClass * klass;
	GObjectClass * parent_class;
	BacklightPlugin * self;
	GError * inner_error;
	klass = BACKLIGHT_PLUGIN_CLASS (g_type_class_peek (TYPE_BACKLIGHT_PLUGIN));
	parent_class = G_OBJECT_CLASS (g_type_class_peek_parent (klass));
	obj = parent_class->constructor (type, n_construct_properties, construct_properties);
	self = BACKLIGHT_PLUGIN (obj);
	inner_error = NULL;
	{
		{
			GKeyFile* _file;
			char* dev;
			char* _tmp0;
			_file = g_key_file_new ();
			g_key_file_load_from_file (_file, "/usr/share/odeviced/plugins/backlight.plugin", G_KEY_FILE_NONE, &inner_error);
			if (inner_error != NULL) {
				if (inner_error->domain == DBUS_GERROR) {
					goto __catch7_dbus_gerror;
				}
				g_critical ("file %s: line %d: uncaught error: %s", __FILE__, __LINE__, inner_error->message);
				g_clear_error (&inner_error);
			}
			dev = odeviced_get_device ();
			_tmp0 = NULL;
			self->priv->max_brightness = odeviced_read_integer ((_tmp0 = g_strconcat (self->priv->_node, "/max_brightness", NULL)));
			_tmp0 = (g_free (_tmp0), NULL);
			(_file == NULL ? NULL : (_file = (g_key_file_free (_file), NULL)));
			dev = (g_free (dev), NULL);
		}
		goto __finally7;
		__catch7_dbus_gerror:
		{
			GError * e;
			e = inner_error;
			inner_error = NULL;
			{
				g_critical (e->message);
			}
		}
		__finally7:
		;
	}
	return obj;
}


static void backlight_plugin_get_property (GObject * object, guint property_id, GValue * value, GParamSpec * pspec) {
	BacklightPlugin * self;
	self = BACKLIGHT_PLUGIN (object);
	switch (property_id) {
		case BACKLIGHT_PLUGIN_NODE:
		g_value_set_string (value, backlight_plugin_get_node (self));
		break;
		case BACKLIGHT_PLUGIN_DBUS_PATH:
		g_value_set_string (value, backlight_plugin_get_dbus_path (self));
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}


static void backlight_plugin_set_property (GObject * object, guint property_id, const GValue * value, GParamSpec * pspec) {
	BacklightPlugin * self;
	self = BACKLIGHT_PLUGIN (object);
	switch (property_id) {
		case BACKLIGHT_PLUGIN_NODE:
		backlight_plugin_set_node (self, g_value_get_string (value));
		break;
		case BACKLIGHT_PLUGIN_DBUS_PATH:
		backlight_plugin_set_dbus_path (self, g_value_get_string (value));
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}


static gboolean _dbus_backlight_plugin_GetMaximumBrightness (BacklightPlugin* self, gint* result, GError** error) {
	*result = backlight_plugin_GetMaximumBrightness (self);
	return !error || !*error;
}


static gboolean _dbus_backlight_plugin_SetBrightness (BacklightPlugin* self, gint brightness, gboolean* result, GError** error) {
	*result = backlight_plugin_SetBrightness (self, brightness);
	return !error || !*error;
}


static gboolean _dbus_backlight_plugin_GetCurrentBrightness (BacklightPlugin* self, gint* result, GError** error) {
	*result = backlight_plugin_GetCurrentBrightness (self);
	return !error || !*error;
}


static void backlight_plugin_class_init (BacklightPluginClass * klass) {
	backlight_plugin_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (BacklightPluginPrivate));
	G_OBJECT_CLASS (klass)->get_property = backlight_plugin_get_property;
	G_OBJECT_CLASS (klass)->set_property = backlight_plugin_set_property;
	G_OBJECT_CLASS (klass)->constructor = backlight_plugin_constructor;
	G_OBJECT_CLASS (klass)->dispose = backlight_plugin_dispose;
	g_object_class_install_property (G_OBJECT_CLASS (klass), BACKLIGHT_PLUGIN_NODE, g_param_spec_string ("node", "node", "node", NULL, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
	g_object_class_install_property (G_OBJECT_CLASS (klass), BACKLIGHT_PLUGIN_DBUS_PATH, g_param_spec_string ("dbus-path", "dbus-path", "dbus-path", NULL, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
	static const DBusGMethodInfo backlight_plugin_dbus_methods[] = {
{ (GCallback) _dbus_backlight_plugin_GetMaximumBrightness, g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER, 0 },
{ (GCallback) _dbus_backlight_plugin_SetBrightness, g_cclosure_user_marshal_BOOLEAN__INT_POINTER_POINTER, 75 },
{ (GCallback) _dbus_backlight_plugin_GetCurrentBrightness, g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER, 158 },
}
;
	static const DBusGObjectInfo backlight_plugin_dbus_object_info = { 0, backlight_plugin_dbus_methods, 3, "org.freesmartphone.Device.Backlight\0GetMaximumBrightness\0S\0result\0O\0F\0N\0i\0\0org.freesmartphone.Device.Backlight\0SetBrightness\0S\0brightness\0I\0i\0result\0O\0F\0N\0b\0\0org.freesmartphone.Device.Backlight\0GetCurrentBrightness\0S\0result\0O\0F\0N\0i\0\0", "", "org.freesmartphone.Device.Backlight\0node\0org.freesmartphone.Device.Backlight\0dbus_path\0" };
	dbus_g_object_type_install_info (TYPE_BACKLIGHT_PLUGIN, &backlight_plugin_dbus_object_info);
}


static void backlight_plugin_instance_init (BacklightPlugin * self) {
	self->priv = BACKLIGHT_PLUGIN_GET_PRIVATE (self);
}


static void backlight_plugin_dispose (GObject * obj) {
	BacklightPlugin * self;
	self = BACKLIGHT_PLUGIN (obj);
	self->priv->_node = (g_free (self->priv->_node), NULL);
	self->priv->_dbus_path = (g_free (self->priv->_dbus_path), NULL);
	G_OBJECT_CLASS (backlight_plugin_parent_class)->dispose (obj);
}


GType backlight_plugin_get_type (void) {
	static GType backlight_plugin_type_id = 0;
	if (G_UNLIKELY (backlight_plugin_type_id == 0)) {
		static const GTypeInfo g_define_type_info = { sizeof (BacklightPluginClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) backlight_plugin_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (BacklightPlugin), 0, (GInstanceInitFunc) backlight_plugin_instance_init };
		backlight_plugin_type_id = g_type_register_static (G_TYPE_OBJECT, "BacklightPlugin", &g_define_type_info, 0);
	}
	return backlight_plugin_type_id;
}


static void register_dbus (BacklightPlugin* obj) {
	g_return_if_fail (IS_BACKLIGHT_PLUGIN (obj));
	g_message ("backlight.vala:100: Registering DBus object at %s", backlight_plugin_get_dbus_path (obj));
	dbus_g_connection_register_g_object (odeviced_connection, backlight_plugin_get_dbus_path (obj), G_OBJECT (obj));
}


G_MODULE_EXPORT gboolean backlight_init (ODevicedPlugin *plugin) {
	GType type;
	GList *list = NULL;
	BacklightPlugin *obj;
	type = backlight_plugin_get_type();
	list = odeviced_compute_objects (plugin, type);
	g_list_foreach(list, (GFunc)register_dbus, NULL);
	
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


static void g_cclosure_user_marshal_BOOLEAN__INT_POINTER_POINTER (GClosure * closure, GValue * return_value, guint n_param_values, const GValue * param_values, gpointer invocation_hint, gpointer marshal_data) {
	typedef gboolean (*GMarshalFunc_BOOLEAN__INT_POINTER_POINTER) (gpointer data1, gint arg_1, gpointer arg_2, gpointer arg_3, gpointer data2);
	register GMarshalFunc_BOOLEAN__INT_POINTER_POINTER callback;
	register GCClosure * cc;
	register gpointer data1, data2;
	gboolean v_return;
	cc = ((GCClosure *) (closure));
	g_return_if_fail (return_value != NULL);
	g_return_if_fail (n_param_values == 4);
	if (G_CCLOSURE_SWAP_DATA (closure)) {
		data1 = closure->data;
		data2 = param_values->data[0].v_pointer;
	} else {
		data1 = param_values->data[0].v_pointer;
		data2 = closure->data;
	}
	callback = ((GMarshalFunc_BOOLEAN__INT_POINTER_POINTER) ((marshal_data ? marshal_data : cc->callback)));
	v_return = callback (data1, g_value_get_int (param_values + 1), g_value_get_pointer (param_values + 2), g_value_get_pointer (param_values + 3), data2);
	g_value_set_boolean (return_value, v_return);
}



