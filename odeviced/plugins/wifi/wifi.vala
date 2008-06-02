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

/* This file exists to generate wifi.[c/h], being the lazy bum I am
   $ valac wifi.vala --pkg dbus-glib-1 -C */

using GLib;


[DBus (name = "org.freesmartphone.Device.Plugins.Wifi") ]
public class WifiPlugin: Object {
	public bool is_on(string iface) {
		return true;
	}
	
	public bool control(string iface, bool enable) {
		return true;
	}
}

public int init() {
	try {
		DBus.Connection conn = DBus.Bus.get(DBus.BusType.SYSTEM);
		WifiPlugin wifiobj = new WifiPlugin();
		conn.register_object ("/org/freesmartphone/Device/Plugins/Wifi", wifiobj);
		
		
	}
	catch (GLib.Error error) {
		stderr.printf("%s\n", error.message);
	}
	return 0;
	
}


		