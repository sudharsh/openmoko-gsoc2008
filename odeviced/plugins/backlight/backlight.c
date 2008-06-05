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
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus-glib.h>




struct _BacklightPluginPrivate {
	gint max_brightness;
	char* max_brightness_node;
	char* set_brightness_node;
};

#define BACKLIGHT_PLUGIN_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TYPE_BACKLIGHT_PLUGIN, BacklightPluginPrivate))
enum  {
	BACKLIGHT_PLUGIN_DUMMY_PROPERTY
};
static GObject * backlight_plugin_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_properties);
static gpointer backlight_plugin_parent_class = NULL;
static gboolean _dbus_backlight_plugin_get_max_brightness (BacklightPlugin* self, gint* result, GError** error);
static gboolean _dbus_backlight_plugin_set_brightness (BacklightPlugin* self, gint brightness, gboolean* result, GError** error);
static void backlight_plugin_dispose (GObject * obj);


static void g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER (GClosure * closure, GValue * return_value, guint n_param_values, const GValue * param_values, gpointer invocation_hint, gpointer marshal_data);
static void g_cclosure_user_marshal_BOOLEAN__INT_POINTER_POINTER (GClosure * closure, GValue * return_value, guint n_param_values, const GValue * param_values, gpointer invocation_hint, gpointer marshal_data);

gint backlight_plugin_get_max_brightness (BacklightPlugin* self) {
	g_return_val_if_fail (IS_BACKLIGHT_PLUGIN (self), 0);
 	return self->priv->max_brightness;
}


gboolean backlight_plugin_set_brightness (BacklightPlugin* self, gint brightness) {

	FILE *set_node;
	g_return_val_if_fail (IS_BACKLIGHT_PLUGIN (self), FALSE);
	if(brightness > self->priv->max_brightness) 
		return FALSE;

	set_node = fopen( (char *)self->priv->set_brightness_node, "w");
	fprintf(set_node, "%d", brightness);
	fclose(set_node);
	
	return TRUE;
}


/*
 * This file exists to generate the sources.
 * $ valac -C backlight.vala --pkg dbus-glib-1
 */
BacklightPlugin* backlight_plugin_new (void) {
	BacklightPlugin * self;
	self = g_object_newv (TYPE_BACKLIGHT_PLUGIN, 0, NULL);
	return self;
}


static GObject * backlight_plugin_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_properties) {
	GObject * obj;
	BacklightPluginClass * klass;
	GObjectClass * parent_class;
	BacklightPlugin * self;
	GError * inner_error;
	FILE *max_node;
	char *dev_name = odeviced_helpers_get_device();
	klass = BACKLIGHT_PLUGIN_CLASS (g_type_class_peek (TYPE_BACKLIGHT_PLUGIN));
	parent_class = G_OBJECT_CLASS (g_type_class_peek_parent (klass));
	obj = parent_class->constructor (type, n_construct_properties, construct_properties);
	self = BACKLIGHT_PLUGIN (obj);
	inner_error = NULL;
	{
		GKeyFile* _file;
		char* _tmp0;
		char* _tmp1;
		_file = g_key_file_new ();
		g_key_file_load_from_file (_file, "/usr/share/odeviced/plugins/backlight.conf", G_KEY_FILE_NONE, &inner_error);
		if (inner_error != NULL) {
			g_critical ("file %s: line %d: uncaught error: %s", __FILE__, __LINE__, inner_error->message);
			g_clear_error (&inner_error);
		}
		_tmp0 = NULL;
		self->priv->max_brightness_node = (_tmp0 = g_key_file_get_string (_file, dev_name, "max_brightness_node", &inner_error), (self->priv->max_brightness_node = (g_free (self->priv->max_brightness_node), NULL)), _tmp0);
		if (inner_error != NULL) {
			g_critical ("file %s: line %d: uncaught error: %s", __FILE__, __LINE__, inner_error->message);
			g_clear_error (&inner_error);
		}
		_tmp1 = NULL;
		self->priv->set_brightness_node = (_tmp1 = g_key_file_get_string (_file, dev_name, "set_brightness_node", &inner_error), (self->priv->set_brightness_node = (g_free (self->priv->set_brightness_node), NULL)), _tmp1);
		if (inner_error != NULL) {
			g_critical ("file %s: line %d: uncaught error: %s", __FILE__, __LINE__, inner_error->message);
			g_clear_error (&inner_error);
		}

		/**************************************************/
		max_node = fopen((char *)self->priv->max_brightness_node, "r");
		fscanf(max_node, "%d", &self->priv->max_brightness);
		fclose(max_node);

		(_file == NULL ? NULL : (_file = (g_key_file_free (_file), NULL)));
	}
	return obj;
}


static gboolean _dbus_backlight_plugin_get_max_brightness (BacklightPlugin* self, gint* result, GError** error) {
	*result = backlight_plugin_get_max_brightness (self);
	return !error || !*error;
}


static gboolean _dbus_backlight_plugin_set_brightness (BacklightPlugin* self, gint brightness, gboolean* result, GError** error) {
	*result = backlight_plugin_set_brightness (self, brightness);
	return !error || !*error;
}


static void backlight_plugin_class_init (BacklightPluginClass * klass) {
	backlight_plugin_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (BacklightPluginPrivate));
	G_OBJECT_CLASS (klass)->constructor = backlight_plugin_constructor;
	G_OBJECT_CLASS (klass)->dispose = backlight_plugin_dispose;
	static const DBusGMethodInfo backlight_plugin_dbus_methods[] = {
{ (GCallback) _dbus_backlight_plugin_get_max_brightness, g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER, 0 },
{ (GCallback) _dbus_backlight_plugin_set_brightness, g_cclosure_user_marshal_BOOLEAN__INT_POINTER_POINTER, 81 },
}
;
	static const DBusGObjectInfo backlight_plugin_dbus_object_info = { 0, backlight_plugin_dbus_methods, 2, "org.freesmartphone.Device.Plugins.Backlight\0get_max_brightness\0S\0result\0O\0F\0N\0i\0\0org.freesmartphone.Device.Plugins.Backlight\0set_brightness\0S\0brightness\0I\0i\0result\0O\0F\0N\0b\0\0", "", "" };
	dbus_g_object_type_install_info (TYPE_BACKLIGHT_PLUGIN, &backlight_plugin_dbus_object_info);
}


static void backlight_plugin_instance_init (BacklightPlugin * self) {
	self->priv = BACKLIGHT_PLUGIN_GET_PRIVATE (self);
}


static void backlight_plugin_dispose (GObject * obj) {
	BacklightPlugin * self;
	self = BACKLIGHT_PLUGIN (obj);
	self->priv->max_brightness_node = (g_free (self->priv->max_brightness_node), NULL);
	self->priv->set_brightness_node = (g_free (self->priv->set_brightness_node), NULL);
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


G_MODULE_EXPORT gboolean backlight_init (ODevicedPlugin *plugin) {
	BacklightPlugin *backlightobj;
	backlightobj = backlight_plugin_new();
	odeviced_helpers_register_dbus_object (plugin, G_OBJECT(backlightobj));
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



