/* odeviced.vala
 *
 * Copyright (C) 2008  Sudharshan S
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author:
 * 	Sudharshan S <sudharsh@gmail.com>
 */

#include "odeviced.h"
#include <stdlib.h>
#include <string.h>
#include <gmodule.h>
#include <stdio.h>
#include "service.h"




DBusGConnection* odeviced_connection = NULL;
guint _dynamic_RequestName0 (DBusGProxy* obj, const char* param1, guint param2, GError** error);
static void odeviced_main (char** args, int args_length1);



guint _dynamic_RequestName0 (DBusGProxy* obj, const char* param1, guint param2, GError** error) {
	guint result;
	dbus_g_proxy_call (obj, "RequestName", error, G_TYPE_STRING, param1, G_TYPE_UINT, param2, G_TYPE_INVALID, G_TYPE_UINT, &result, G_TYPE_INVALID);
	if ((*error)) {
		return 0U;
	}
	return result;
}


static void odeviced_main (char** args, int args_length1) {
	GError * inner_error;
	inner_error = NULL;
	{
		DBusGConnection* _tmp0;
		DBusGProxy* bus;
		guint result;
		_tmp0 = NULL;
		odeviced_connection = (_tmp0 = dbus_g_bus_get (DBUS_BUS_SYSTEM, &inner_error), (odeviced_connection == NULL ? NULL : (odeviced_connection = (dbus_g_connection_unref (odeviced_connection), NULL))), _tmp0);
		if (inner_error != NULL) {
			goto __catch3_g_error;
		}
		bus = dbus_g_proxy_new_for_name (odeviced_connection, "org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");
		result = _dynamic_RequestName0 (bus, "org.freesmartphone.Device", ((guint) (0)), &inner_error);
		if (inner_error != NULL) {
			goto __catch3_g_error;
		}
		if (result == DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
			ODevicedService* service;
			g_print ("Starting ODeviced Server....\n");
			service = odeviced_service_new ();
			if (!g_module_supported ()) {
				g_critical ("odeviced.vala:42: Modules are not supported in the current system");
			}
			odeviced_service_run (service);
			(service == NULL ? NULL : (service = (g_object_unref (service), NULL)));
		} else {
			/* If odeviced is already running */
			g_print ("ODeviced already running!\n");
		}
		(bus == NULL ? NULL : (bus = (g_object_unref (bus), NULL)));
	}
	goto __finally3;
	__catch3_g_error:
	{
		GError * error;
		error = inner_error;
		inner_error = NULL;
		{
			fprintf (stderr, "%s\n", error->message);
		}
	}
	__finally3:
	;
}


int main (int argc, char ** argv) {
	g_type_init ();
	odeviced_main (argv, argc);
	return 0;
}




