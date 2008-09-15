/* 
 * led.vala
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

using DBus;
using ODeviced;
using FSO;
using Subsystem;

[DBus (name = "org.freesmartphone.Device.LED")]
public class LED:GLib.Object {
	
	private string name = new string();

	[DBus (visible=false)]
	public string node {
		get;
		construct;
	}

	[DBus (visible=false)]
	public string dbus_path {
		get;
		construct;
	}

	[DBus (visible=false)]
	public Subsystem.Manager plugin {
		get;
		construct;
	}


	LED(string node, string dbus_path, Subsystem.Manager plugin) {
		this.node = node;
		this.dbus_path = dbus_path;
		this.plugin = plugin;
	}

		
	construct {
		var dev = ODeviced.get_device();
		this.name = ODeviced.compute_name(dbus_path);
		this.dbus_path = ODeviced.cleanup_dbus_path (this.dbus_path);
	}
		

	public void SetBrightness(int brightness) {
		ODeviced.write_integer(this.node + "/brightness", brightness);
	}


	public void SetBlinking(int delay_on, int delay_off) {
		ODeviced.write_string(this.node + "/trigger", "timer");
		ODeviced.write_integer(this.node + "/delay_on", delay_on);
		ODeviced.write_integer(this.node + "/delay_off", delay_off);
	}


	public string GetName() {
		return this.name;
	}


}


namespace led {

	public static List<LED> list;

	public bool init (Subsystem.Manager plugin) {
		Type type;
		type = typeof (LED);
		list = ODeviced.compute_objects (plugin, type);
		if (list == null)
			return false;

		foreach (LED _obj in list) {
			register_dbus (_obj);
		}

		return true;
	}
}


static void register_dbus (LED obj) {
	GLib.message("Registering DBus object at %s", obj.dbus_path);
	FSO.connection.register_object(obj.dbus_path, obj);
}
