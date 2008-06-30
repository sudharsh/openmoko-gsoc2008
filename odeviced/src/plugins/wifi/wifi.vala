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
using WifiHelpers;
using ODeviced;


[DBus (name = "org.freesmartphone.Device.Wifi") ]
public class WifiPlugin: Object {
	public bool GetStatus(string iface) {
		message("%b", WifiHelpers.get_status (iface));
		return true;
	}
	
	public bool SetControl(string iface, bool enable) {
		return WifiHelpers.set_status (iface, enable);
	}
}


namespace wifi {
	
	public static WifiPlugin obj;
	public bool init(ODeviced.Plugin plugin) {
	
		obj = new WifiPlugin();
		if (obj == null)
			return false;
		
		ODeviced.register_dbus_object(plugin, obj);
		return true;
	}
}


		