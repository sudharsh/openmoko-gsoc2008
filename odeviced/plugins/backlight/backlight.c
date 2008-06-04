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
#include <dbus/dbus-glib.h>




struct _BacklightPluginPrivate {
	gint max_brightness;
};

#define BACKLIGHT_PLUGIN_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TYPE_BACKLIGHT_PLUGIN, BacklightPluginPrivate))
enum  {
	BACKLIGHT_PLUGIN_DUMMY_PROPERTY
};
static gpointer backlight_plugin_parent_class = NULL;
static gboolean _dbus_backlight_plugin_get_max_brightness (BacklightPlugin* self, gint* result, GError** error);
static gboolean _dbus_backlight_plugin_set_brightness (BacklightPlugin* self, gboolean* result, GError** error);
static void backlight_plugin_dispose (GObject * obj);


static void g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER (GClosure * closure, GValue * return_value, guint n_param_values, const GValue * param_values, gpointer invocation_hint, gpointer marshal_data);

gint backlight_plugin_get_max_brightness (BacklightPlugin* self) {
	g_return_val_if_fail (IS_BACKLIGHT_PLUGIN (self), 0);
	return 1;
}


gboolean backlight_plugin_set_brightness (BacklightPlugin* self) {
	g_return_val_if_fail (IS_BACKLIGHT_PLUGIN (self), FALSE);
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


static gboolean _dbus_backlight_plugin_get_max_brightness (BacklightPlugin* self, gint* result, GError** error) {
	*result = backlight_plugin_get_max_brightness (self);
	return !error || !*error;
}


static gboolean _dbus_backlight_plugin_set_brightness (BacklightPlugin* self, gboolean* result, GError** error) {
	*result = backlight_plugin_set_brightness (self);
	return !error || !*error;
}


static void backlight_plugin_class_init (BacklightPluginClass * klass) {
	backlight_plugin_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (BacklightPluginPrivate));
	G_OBJECT_CLASS (klass)->dispose = backlight_plugin_dispose;
	static const DBusGMethodInfo backlight_plugin_dbus_methods[] = {
{ (GCallback) _dbus_backlight_plugin_get_max_brightness, g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER, 0 },
{ (GCallback) _dbus_backlight_plugin_set_brightness, g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER, 81 },
}
;
	static const DBusGObjectInfo backlight_plugin_dbus_object_info = { 0, backlight_plugin_dbus_methods, 2, "org.freesmartphone.Device.Plugins.Backlight\0get_max_brightness\0S\0result\0O\0F\0N\0i\0\0org.freesmartphone.Device.Plugins.Backlight\0set_brightness\0S\0result\0O\0F\0N\0b\0\0", "", "" };
	dbus_g_object_type_install_info (TYPE_BACKLIGHT_PLUGIN, &backlight_plugin_dbus_object_info);
}


static void backlight_plugin_instance_init (BacklightPlugin * self) {
	self->priv = BACKLIGHT_PLUGIN_GET_PRIVATE (self);
}


static void backlight_plugin_dispose (GObject * obj) {
	BacklightPlugin * self;
	self = BACKLIGHT_PLUGIN (obj);
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



