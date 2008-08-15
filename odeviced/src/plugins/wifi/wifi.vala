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

using GLib;
using ODeviced;
using WifiHelpers;

[DBus (name = "org.freesmartphone.Device.PowerControl.Wifi") ]
public class WifiPlugin: Object {

	ODeviced.PluginManager plugin;

	public bool GetStatus(string iface) {
		return WifiHelpers.get_status (iface);
	}
	
	public bool SetControl(string iface, bool enable) {
		return WifiHelpers.set_control (iface, enable);
	}
}


public bool wifi_init(ODeviced.PluginManager plugin) { 
	WifiPlugin obj;

	obj = new WifiPlugin();
	if(obj == null)
		return false;
	
	ODeviced.register_dbus_object (plugin, obj);
	return true;
}
