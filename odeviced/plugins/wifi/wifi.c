/* 
 * wifi.vala
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

#include "wifi.h"

#include <dbus/dbus-glib-lowlevel.h>
#include <dbus/dbus-glib.h>
#include <stdio.h>
#include <dbus/dbus-glib.h>

enum  {
	WIFI_PLUGIN_DUMMY_PROPERTY
};
static gpointer wifi_plugin_parent_class = NULL;
static gboolean _dbus_wifi_plugin_is_on (WifiPlugin* self, const char* iface, gboolean* result, GError** error);
static gboolean _dbus_wifi_plugin_control (WifiPlugin* self, const char* iface, gboolean enable, gboolean* result, GError** error);


static void g_cclosure_user_marshal_BOOLEAN__STRING_POINTER_POINTER (GClosure * closure, GValue * return_value, guint n_param_values, const GValue * param_values, gpointer invocation_hint, gpointer marshal_data);
static void g_cclosure_user_marshal_BOOLEAN__STRING_BOOLEAN_POINTER_POINTER (GClosure * closure, GValue * return_value, guint n_param_values, const GValue * param_values, gpointer invocation_hint, gpointer marshal_data);

/* The code within the dbus methods taken from python-odeviced and neod*/

gboolean wifi_plugin_is_on (WifiPlugin* self, const char* iface) {
	g_return_val_if_fail (IS_WIFI_PLUGIN (self), FALSE);
	g_return_val_if_fail (iface != NULL, FALSE);
	struct iwreq wrq;
	int sock = socket (AF_INET, SOCK_DGRAM, 0);
	if (!sock)
	{
		perror( "Unable to open socket" );
		return 0;
	}

	memset (&wrq, 0, sizeof (struct iwreq));
	strncpy ((char *)&wrq.ifr_name, iface, IFNAMSIZ);

	if (ioctl (sock, SIOCGIWTXPOW, &wrq) != 0)
	{
		perror( "Error performing ioctl" );
		close (sock);
		return 0;
	}
	
	close (sock);
	
	return !wrq.u.txpower.disabled;

}


gboolean wifi_plugin_control (WifiPlugin* self, const char* iface, gboolean enable) {
	g_return_val_if_fail (IS_WIFI_PLUGIN (self), FALSE);
	g_return_val_if_fail (iface != NULL, FALSE);
	struct iwreq wrq;
	int sock = socket (AF_INET, SOCK_DGRAM, 0);
	if (!sock)
	{
		perror( "Unable to open socket" );
		return 0;
	}

	memset (&wrq, 0, sizeof (struct iwreq));
	strncpy ((char *)&wrq.ifr_name, iface, IFNAMSIZ);

	if (ioctl (sock, SIOCGIWTXPOW, &wrq) != 0)
	{
		perror( "Error performing ioctl" );
		close (sock);
		return 0;
	}

	if ( wrq.u.txpower.disabled != !enable )
	{
		wrq.u.txpower.disabled = !enable;

		if (ioctl (sock, SIOCSIWTXPOW, &wrq) != 0)
		{
			perror( "Error performing ioctl" );
			close (sock);
			return 0;
		}
	}

	close (sock);
	return 1;

}


/* This file exists to generate the wifi.[c/h] for modifications */
WifiPlugin* wifi_plugin_new (void) {
	WifiPlugin * self;
	self = g_object_newv (TYPE_WIFI_PLUGIN, 0, NULL);
	return self;
}


static gboolean _dbus_wifi_plugin_is_on (WifiPlugin* self, const char* iface, gboolean* result, GError** error) {
	*result = wifi_plugin_is_on (self, iface);
	return !error || !*error;
}


static gboolean _dbus_wifi_plugin_control (WifiPlugin* self, const char* iface, gboolean enable, gboolean* result, GError** error) {
	*result = wifi_plugin_control (self, iface, enable);
	return !error || !*error;
}


static void wifi_plugin_class_init (WifiPluginClass * klass) {
	wifi_plugin_parent_class = g_type_class_peek_parent (klass);
	static const DBusGMethodInfo wifi_plugin_dbus_methods[] = {
{ (GCallback) _dbus_wifi_plugin_is_on, g_cclosure_user_marshal_BOOLEAN__STRING_POINTER_POINTER, 0 },
{ (GCallback) _dbus_wifi_plugin_control, g_cclosure_user_marshal_BOOLEAN__STRING_BOOLEAN_POINTER_POINTER, 73 },
}
;
	static const DBusGObjectInfo wifi_plugin_dbus_object_info = { 0, wifi_plugin_dbus_methods, 2, "org.freesmartphone.Device.Plugins.Wifi\0is_on\0S\0iface\0I\0s\0result\0O\0F\0N\0b\0\0org.freesmartphone.Device.Plugins.Wifi\0control\0S\0iface\0I\0s\0enable\0I\0b\0result\0O\0F\0N\0b\0\0", "", "" };
	dbus_g_object_type_install_info (TYPE_WIFI_PLUGIN, &wifi_plugin_dbus_object_info);
}


static void wifi_plugin_init (WifiPlugin * self) {
}


static void wifi_plugin_dispose (GObject *obj) {
	WifiPlugin *self;
	self = WIFI_PLUGIN(obj);
	G_OBJECT_CLASS (wifi_plugin_parent_class)-> dispose(obj);
}



GType wifi_plugin_get_type (void) {
	static GType wifi_plugin_type_id = 0;
	if (G_UNLIKELY (wifi_plugin_type_id == 0)) {
		static const GTypeInfo g_define_type_info = { sizeof (WifiPluginClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) wifi_plugin_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (WifiPlugin), 0, (GInstanceInitFunc) wifi_plugin_init };
		wifi_plugin_type_id = g_type_register_static (G_TYPE_OBJECT, "WifiPlugin", &g_define_type_info, 0);
	}
	return wifi_plugin_type_id;
}


G_MODULE_EXPORT gboolean wifi_init (ODevicedPlugin *plugin) {
	GError * inner_error;
	{
		WifiPlugin* wifiobj;
		if (inner_error != NULL) {
			goto __catch0_g_error;
		}
		wifiobj = wifi_plugin_new ();
		if(wifiobj) {
			odeviced_register_dbus_object (plugin, G_OBJECT(wifiobj));
		}
	}
	goto __finally0;
	__catch0_g_error:
	{
		GError * error;
		error = inner_error;
		inner_error = NULL;
		{ 
			fprintf (stderr, "%s\n", error->message);
		}
	}
	__finally0:
	;

	return TRUE;
}


static void g_cclosure_user_marshal_BOOLEAN__STRING_POINTER_POINTER (GClosure * closure, GValue * return_value, guint n_param_values, const GValue * param_values, gpointer invocation_hint, gpointer marshal_data) {
	typedef gboolean (*GMarshalFunc_BOOLEAN__STRING_POINTER_POINTER) (gpointer data1, const char* arg_1, gpointer arg_2, gpointer arg_3, gpointer data2);
	register GMarshalFunc_BOOLEAN__STRING_POINTER_POINTER callback;
	register GCClosure * cc;
	register gpointer data1, data2;
	gboolean v_return;
	cc = ((GCClosure *) closure);
	g_return_if_fail (return_value != NULL);
	g_return_if_fail (n_param_values == 4);
	if (G_CCLOSURE_SWAP_DATA (closure)) {
		data1 = closure->data;
		data2 = param_values->data[0].v_pointer;
	} else {
		data1 = param_values->data[0].v_pointer;
		data2 = closure->data;
	}
	callback = ((GMarshalFunc_BOOLEAN__STRING_POINTER_POINTER) (marshal_data ? marshal_data : cc->callback));
	v_return = callback (data1, g_value_get_string (param_values + 1), g_value_get_pointer (param_values + 2), g_value_get_pointer (param_values + 3), data2);
	g_value_set_boolean (return_value, v_return);
}


static void g_cclosure_user_marshal_BOOLEAN__STRING_BOOLEAN_POINTER_POINTER (GClosure * closure, GValue * return_value, guint n_param_values, const GValue * param_values, gpointer invocation_hint, gpointer marshal_data) {
	typedef gboolean (*GMarshalFunc_BOOLEAN__STRING_BOOLEAN_POINTER_POINTER) (gpointer data1, const char* arg_1, gboolean arg_2, gpointer arg_3, gpointer arg_4, gpointer data2);
	register GMarshalFunc_BOOLEAN__STRING_BOOLEAN_POINTER_POINTER callback;
	register GCClosure * cc;
	register gpointer data1, data2;
	gboolean v_return;
	cc = ((GCClosure *) closure);
	g_return_if_fail (return_value != NULL);
	g_return_if_fail (n_param_values == 5);
	if (G_CCLOSURE_SWAP_DATA (closure)) {
		data1 = closure->data;
		data2 = param_values->data[0].v_pointer;
	} else {
		data1 = param_values->data[0].v_pointer;
		data2 = closure->data;
	}
	callback = ((GMarshalFunc_BOOLEAN__STRING_BOOLEAN_POINTER_POINTER) (marshal_data ? marshal_data : cc->callback));
	v_return = callback (data1, g_value_get_string (param_values + 1), g_value_get_boolean (param_values + 2), g_value_get_pointer (param_values + 3), g_value_get_pointer (param_values + 4), data2);
	g_value_set_boolean (return_value, v_return);
}



