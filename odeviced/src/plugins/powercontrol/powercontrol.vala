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
using DBus; 
using WifiHelpers;


[DBus (name = "org.freesmartphone.Device.PowerControl")]
public abstract class GenericPowerControl: GLib.Object {
	
	[DBus (visible = false)]
	public string name = new string();

	[DBus (visible = false)]
	public string node = new string();

	[DBus (visible = false)]
	public string powernode = new string();

	[DBus (visible = false)]
	public string resetnode = new string();

	public virtual signal void power (string device, bool enable);
	 
	[DBus (visible = false)]
	public int get_power () {
		return ODeviced.read_integer (this.powernode);
	}
	
	[DBus (visible = false)]
	public void set_power (bool enable) {
		if(enable)
			ODeviced.write_integer (this.powernode, 1);
		else
			ODeviced.write_integer (this.powernode, 0);
	}
	
		
	public abstract string GetName ();		
	public abstract bool GetPower();
	public abstract bool SetPower(bool enable);
	public abstract void Reset();
		
}


public class Wifi: GenericPowerControl {

	private string default_iface = new string();
	private string _dbus_path = "/org/freesmartphone/Device/PowerControl/WiFi";
		
	[DBus (visible = false)]
	public string dbus_path {
		get { return _dbus_path; }
	}
	
	[DBus (visible = false)]
	public ODeviced.PluginManager plugin {
		get;
		construct;
	}

	Wifi (ODeviced.PluginManager plugin) {
		this.plugin = plugin;
	}

	construct {
		string device = ODeviced.get_device();
		this.default_iface = plugin.conf.get_string (device, "default_iface");
		this.name = "WiFi";
	}
		
	public override bool GetPower() {
		return WifiHelpers.get_status (this.default_iface);
	}
	
	public override bool SetPower(bool enable) {
		return WifiHelpers.set_control (this.default_iface, enable);
	}

	public override string GetName () {
		return this.name;
	}

	public override void Reset () {
		print ("\tWiFi: INFO: No Reset for Wifi\n");
	}
}



namespace powercontrol {
	
	public static Wifi wifi_obj;
	
	public bool init (ODeviced.PluginManager plugin) {

		bool success = true;
		string device = ODeviced.get_device();
		var enable = plugin.conf.get_string_list (device, "enable");
		
		foreach (string klass in enable) {
			switch (klass){
			case "WiFi":
				wifi_obj = new Wifi (plugin);
				if (wifi_obj == null)
					success = false;
				ODeviced.connection.register_object (wifi_obj.dbus_path, (GLib.Object)wifi_obj);
				plugin.dbus_object_paths.append (wifi_obj.dbus_path);
				break;
			default:
				print ("\tNo definition for %s\n", klass);
				break;
			}
		}
		return success;
	}
}
				

	
	

