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

#include "display.h"
#include <src/daemon/helpers.h>
#include <src/daemon/odeviced.h>
#include <dbus/dbus-glib-lowlevel.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib.h>




struct _DisplayPrivate {
	gint max_brightness;
	char* name;
	char* _node;
	char* _dbus_path;
};

#define DISPLAY_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TYPE_DISPLAY, DisplayPrivate))
enum  {
	DISPLAY_DUMMY_PROPERTY,
	DISPLAY_NODE,
	DISPLAY_DBUS_PATH
};
static Display* display_new (const char* node, const char* dbus_path);
static void display_set_node (Display* self, const char* value);
static void display_set_dbus_path (Display* self, const char* value);
static GObject * display_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_properties);
static gpointer display_parent_class = NULL;
static gboolean _dbus_display_GetMaximumBrightness (Display* self, gint* result, GError** error);
static gboolean _dbus_display_SetBrightness (Display* self, gint brightness, gboolean* result, GError** error);
static gboolean _dbus_display_GetBrightness (Display* self, gint* result, GError** error);
static gboolean _dbus_display_GetBacklightPower (Display* self, gboolean* result, GError** error);
static gboolean _dbus_display_SetBacklightPower (Display* self, gboolean power, GError** error);
static gboolean _dbus_display_GetName (Display* self, char** result, GError** error);
static void display_dispose (GObject * obj);
Display* display_obj = NULL;
GList* display_list = NULL;
static void _g_list_free_g_object_unref (GList* self);
static void register_dbus (Display* obj);


static void g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER (GClosure * closure, GValue * return_value, guint n_param_values, const GValue * param_values, gpointer invocation_hint, gpointer marshal_data);
static void g_cclosure_user_marshal_BOOLEAN__INT_POINTER_POINTER (GClosure * closure, GValue * return_value, guint n_param_values, const GValue * param_values, gpointer invocation_hint, gpointer marshal_data);
static void g_cclosure_user_marshal_BOOLEAN__BOOLEAN_POINTER (GClosure * closure, GValue * return_value, guint n_param_values, const GValue * param_values, gpointer invocation_hint, gpointer marshal_data);

static Display* display_new (const char* node, const char* dbus_path) {
	GParameter * __params;
	GParameter * __params_it;
	Display * self;
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
	self = g_object_newv (TYPE_DISPLAY, __params_it - __params, __params);
	while (__params_it > __params) {
		--__params_it;
		g_value_unset (&__params_it->value);
	}
	g_free (__params);
	return self;
}


gint display_GetMaximumBrightness (Display* self) {
	g_return_val_if_fail (IS_DISPLAY (self), 0);
	return self->priv->max_brightness;
}


gboolean display_SetBrightness (Display* self, gint brightness) {
	char* _tmp1;
	g_return_val_if_fail (IS_DISPLAY (self), FALSE);
	if (brightness > self->priv->max_brightness) {
		return FALSE;
	}
	_tmp1 = NULL;
	odeviced_write_integer ((_tmp1 = g_strconcat (self->priv->_node, "/brightness", NULL)), brightness);
	_tmp1 = (g_free (_tmp1), NULL);
	return TRUE;
}


gint display_GetBrightness (Display* self) {
	char* _tmp0;
	gint _tmp1;
	g_return_val_if_fail (IS_DISPLAY (self), 0);
	_tmp0 = NULL;
	return (_tmp1 = odeviced_read_integer ((_tmp0 = g_strconcat (self->priv->_node, "/actual_brightness", NULL))), (_tmp0 = (g_free (_tmp0), NULL)), _tmp1);
}


gboolean display_GetBacklightPower (Display* self) {
	char* _tmp0;
	gboolean _tmp1;
	g_return_val_if_fail (IS_DISPLAY (self), FALSE);
	_tmp0 = NULL;
	return (_tmp1 = odeviced_read_integer ((_tmp0 = g_strconcat (self->priv->_node, "/bl_power", NULL))) == 0, (_tmp0 = (g_free (_tmp0), NULL)), _tmp1);
}


void display_SetBacklightPower (Display* self, gboolean power) {
	gint _val;
	char* _tmp0;
	g_return_if_fail (IS_DISPLAY (self));
	_val = 0;
	if (power) {
		_val = 1;
	} else {
		_val = 0;
	}
	_tmp0 = NULL;
	odeviced_write_integer ((_tmp0 = g_strconcat (self->priv->_node, "/bl_power", NULL)), _val);
	_tmp0 = (g_free (_tmp0), NULL);
}


char* display_GetName (Display* self) {
	const char* _tmp0;
	g_return_val_if_fail (IS_DISPLAY (self), NULL);
	_tmp0 = NULL;
	return (_tmp0 = self->priv->name, (_tmp0 == NULL ? NULL : g_strdup (_tmp0)));
}


const char* display_get_node (Display* self) {
	g_return_val_if_fail (IS_DISPLAY (self), NULL);
	return self->priv->_node;
}


static void display_set_node (Display* self, const char* value) {
	char* _tmp2;
	const char* _tmp1;
	g_return_if_fail (IS_DISPLAY (self));
	_tmp2 = NULL;
	_tmp1 = NULL;
	self->priv->_node = (_tmp2 = (_tmp1 = value, (_tmp1 == NULL ? NULL : g_strdup (_tmp1))), (self->priv->_node = (g_free (self->priv->_node), NULL)), _tmp2);
}


const char* display_get_dbus_path (Display* self) {
	g_return_val_if_fail (IS_DISPLAY (self), NULL);
	return self->priv->_dbus_path;
}


static void display_set_dbus_path (Display* self, const char* value) {
	char* _tmp2;
	const char* _tmp1;
	g_return_if_fail (IS_DISPLAY (self));
	_tmp2 = NULL;
	_tmp1 = NULL;
	self->priv->_dbus_path = (_tmp2 = (_tmp1 = value, (_tmp1 == NULL ? NULL : g_strdup (_tmp1))), (self->priv->_dbus_path = (g_free (self->priv->_dbus_path), NULL)), _tmp2);
}


static GObject * display_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_properties) {
	GObject * obj;
	DisplayClass * klass;
	GObjectClass * parent_class;
	Display * self;
	GError * inner_error;
	klass = DISPLAY_CLASS (g_type_class_peek (TYPE_DISPLAY));
	parent_class = G_OBJECT_CLASS (g_type_class_peek_parent (klass));
	obj = parent_class->constructor (type, n_construct_properties, construct_properties);
	self = DISPLAY (obj);
	inner_error = NULL;
	{
		{
			GKeyFile* _file;
			char* dev;
			char* _tmp0;
			char* _tmp1;
			_file = g_key_file_new ();
			g_key_file_load_from_file (_file, "/usr/share/odeviced/plugins/display.plugin", G_KEY_FILE_NONE, &inner_error);
			if (inner_error != NULL) {
				goto __catch0_g_error;
			}
			dev = odeviced_get_device ();
			_tmp0 = NULL;
			self->priv->name = (_tmp0 = odeviced_compute_name (self->priv->_dbus_path), (self->priv->name = (g_free (self->priv->name), NULL)), _tmp0);
			_tmp1 = NULL;
			self->priv->max_brightness = odeviced_read_integer ((_tmp1 = g_strconcat (self->priv->_node, "/max_brightness", NULL)));
			_tmp1 = (g_free (_tmp1), NULL);
			(_file == NULL ? NULL : (_file = (g_key_file_free (_file), NULL)));
			dev = (g_free (dev), NULL);
		}
		goto __finally0;
		__catch0_g_error:
		{
			GError * e;
			e = inner_error;
			inner_error = NULL;
			{
				g_critical (e->message);
				(e == NULL ? NULL : (e = (g_error_free (e), NULL)));
			}
		}
		__finally0:
		;
	}
	return obj;
}


static void display_get_property (GObject * object, guint property_id, GValue * value, GParamSpec * pspec) {
	Display * self;
	self = DISPLAY (object);
	switch (property_id) {
		case DISPLAY_NODE:
		g_value_set_string (value, display_get_node (self));
		break;
		case DISPLAY_DBUS_PATH:
		g_value_set_string (value, display_get_dbus_path (self));
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}


static void display_set_property (GObject * object, guint property_id, const GValue * value, GParamSpec * pspec) {
	Display * self;
	self = DISPLAY (object);
	switch (property_id) {
		case DISPLAY_NODE:
		display_set_node (self, g_value_get_string (value));
		break;
		case DISPLAY_DBUS_PATH:
		display_set_dbus_path (self, g_value_get_string (value));
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}


static gboolean _dbus_display_GetMaximumBrightness (Display* self, gint* result, GError** error) {
	*result = display_GetMaximumBrightness (self);
	return !error || !*error;
}


static gboolean _dbus_display_SetBrightness (Display* self, gint brightness, gboolean* result, GError** error) {
	*result = display_SetBrightness (self, brightness);
	return !error || !*error;
}


static gboolean _dbus_display_GetBrightness (Display* self, gint* result, GError** error) {
	*result = display_GetBrightness (self);
	return !error || !*error;
}


static gboolean _dbus_display_GetBacklightPower (Display* self, gboolean* result, GError** error) {
	*result = display_GetBacklightPower (self);
	return !error || !*error;
}


static gboolean _dbus_display_SetBacklightPower (Display* self, gboolean power, GError** error) {
	display_SetBacklightPower (self, power);
	return !error || !*error;
}


static gboolean _dbus_display_GetName (Display* self, char** result, GError** error) {
	*result = display_GetName (self);
	return !error || !*error;
}


static void display_class_init (DisplayClass * klass) {
	display_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (DisplayPrivate));
	G_OBJECT_CLASS (klass)->get_property = display_get_property;
	G_OBJECT_CLASS (klass)->set_property = display_set_property;
	G_OBJECT_CLASS (klass)->constructor = display_constructor;
	G_OBJECT_CLASS (klass)->dispose = display_dispose;
	g_object_class_install_property (G_OBJECT_CLASS (klass), DISPLAY_NODE, g_param_spec_string ("node", "node", "node", NULL, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
	g_object_class_install_property (G_OBJECT_CLASS (klass), DISPLAY_DBUS_PATH, g_param_spec_string ("dbus-path", "dbus-path", "dbus-path", NULL, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
	static const DBusGMethodInfo display_dbus_methods[] = {
{ (GCallback) _dbus_display_GetMaximumBrightness, g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER, 0 },
{ (GCallback) _dbus_display_SetBrightness, g_cclosure_user_marshal_BOOLEAN__INT_POINTER_POINTER, 73 },
{ (GCallback) _dbus_display_GetBrightness, g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER, 154 },
{ (GCallback) _dbus_display_GetBacklightPower, g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER, 220 },
{ (GCallback) _dbus_display_SetBacklightPower, g_cclosure_user_marshal_BOOLEAN__BOOLEAN_POINTER, 290 },
{ (GCallback) _dbus_display_GetName, g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER, 355 },
}
;
	static const DBusGObjectInfo display_dbus_object_info = { 0, display_dbus_methods, 6, "org.freesmartphone.Device.Display\0GetMaximumBrightness\0S\0result\0O\0F\0N\0i\0\0org.freesmartphone.Device.Display\0SetBrightness\0S\0brightness\0I\0i\0result\0O\0F\0N\0b\0\0org.freesmartphone.Device.Display\0GetBrightness\0S\0result\0O\0F\0N\0i\0\0org.freesmartphone.Device.Display\0GetBacklightPower\0S\0result\0O\0F\0N\0b\0\0org.freesmartphone.Device.Display\0SetBacklightPower\0S\0power\0I\0b\0\0org.freesmartphone.Device.Display\0GetName\0S\0result\0O\0F\0N\0s\0\0", "", "" };
	dbus_g_object_type_install_info (TYPE_DISPLAY, &display_dbus_object_info);
}


static void display_instance_init (Display * self) {
	self->priv = DISPLAY_GET_PRIVATE (self);
	self->priv->name = g_new0 (char, 1);
}


static void display_dispose (GObject * obj) {
	Display * self;
	self = DISPLAY (obj);
	self->priv->name = (g_free (self->priv->name), NULL);
	self->priv->_node = (g_free (self->priv->_node), NULL);
	self->priv->_dbus_path = (g_free (self->priv->_dbus_path), NULL);
	G_OBJECT_CLASS (display_parent_class)->dispose (obj);
}


GType display_get_type (void) {
	static GType display_type_id = 0;
	if (G_UNLIKELY (display_type_id == 0)) {
		static const GTypeInfo g_define_type_info = { sizeof (DisplayClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) display_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (Display), 0, (GInstanceInitFunc) display_instance_init };
		display_type_id = g_type_register_static (G_TYPE_OBJECT, "Display", &g_define_type_info, 0);
	}
	return display_type_id;
}


static void _g_list_free_g_object_unref (GList* self) {
	g_list_foreach (self, ((GFunc) (g_object_unref)), NULL);
	g_list_free (self);
}


gboolean display_init (ODevicedPlugin* plugin) {
	GType type;
	GList* _tmp0;
	g_return_val_if_fail (ODEVICED_IS_PLUGIN (plugin), FALSE);
	type = 0UL;
	type = TYPE_DISPLAY;
	_tmp0 = NULL;
	display_list = (_tmp0 = odeviced_compute_objects (plugin, type), (display_list == NULL ? NULL : (display_list = (_g_list_free_g_object_unref (display_list), NULL))), _tmp0);
	if (display_list == NULL) {
		return FALSE;
	}
	{
		GList* _obj_collection;
		GList* _obj_it;
		_obj_collection = display_list;
		for (_obj_it = _obj_collection; _obj_it != NULL; _obj_it = _obj_it->next) {
			Display* _tmp2;
			Display* _obj;
			_tmp2 = NULL;
			_obj = (_tmp2 = ((Display*) (_obj_it->data)), (_tmp2 == NULL ? NULL : g_object_ref (_tmp2)));
			{
				register_dbus (_obj);
				(_obj == NULL ? NULL : (_obj = (g_object_unref (_obj), NULL)));
			}
		}
	}
	return TRUE;
}


static void register_dbus (Display* obj) {
	g_return_if_fail (IS_DISPLAY (obj));
	g_message ("display.vala:122: Registering DBus object at %s", display_get_dbus_path (obj));
	dbus_g_connection_register_g_object (odeviced_connection, display_get_dbus_path (obj), G_OBJECT (obj));
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



