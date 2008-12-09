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
using FSOD;
using Device;


errordomain UnsupportedTrigger {
	UNSUPPORTED_TRIGGER;
}
	
errordomain InvalidParameter {
	INVALID_PARAMETER;
}


[DBus (name = "org.freesmartphone.Device.LED")]
public class LED:GLib.Object {
	
	private string name = new string();
	private string[] triggers = null;
	private const string SYSFS_NET = "/sys/class/net/";

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
	public Plugin plugin {
		get;
		construct;
	}


	LED(string node, string dbus_path, Plugin plugin) {
		this.node = node;
		this.dbus_path = dbus_path;
		this.plugin = plugin;
	}

		
	construct {
		var dev = ODeviced.get_device();
		this.name = ODeviced.compute_name(dbus_path);
		this.dbus_path = ODeviced.cleanup_dbus_path (this.dbus_path);
		this.triggers = ODeviced.read_string (this.node + "/trigger").split(" ");
	}
		

	private bool _contains_trigger (string trigger) {
		foreach (string key in this.triggers) {
			if (key == trigger)
				return true;
		}
		return false;
	}



	public void SetBrightness(int brightness) {
		ODeviced.write_integer(this.node + "/brightness", brightness);
	}


	public void SetBlinking(int delay_on, int delay_off) throws UnsupportedTrigger {
		if (!this._contains_trigger("timer"))
			throw new UnsupportedTrigger.UNSUPPORTED_TRIGGER ("timer trigger not present");
		ODeviced.write_string(this.node + "/trigger", "timer");
		ODeviced.write_integer(this.node + "/delay_on", delay_on);
		ODeviced.write_integer(this.node + "/delay_off", delay_off);
	}


	public void SetNetworking (string net_iface, string mode) throws UnsupportedTrigger, InvalidParameter {
		if (!this._contains_trigger("netdev"))
			throw new UnsupportedTrigger.UNSUPPORTED_TRIGGER ("netdev trigger not present");

		if (!FileUtils.test(SYSFS_NET + net_iface, FileTest.EXISTS))
			throw new InvalidParameter.INVALID_PARAMETER ("Interface %s not known".printf(net_iface));
		
		if (!(mode == "link" || mode == "rx" || mode == "tx"))
			throw new InvalidParameter.INVALID_PARAMETER ("Mode %s not allowed. Available modes are link, rx, and tx".printf(net_iface));
		ODeviced.write_string(this.node + "/trigger", "netdev");
		ODeviced.write_string(this.node + "/device_name", net_iface);
		ODeviced.write_string(this.node + "/mode", mode);
	}


	public string GetName() {
		return this.name;
	}


}


namespace led {

	public static List<LED> list;

	public bool init (Plugin plugin) {
		Type type;
		type = typeof (LED);
		list = ODeviced.compute_objects (plugin, type);
		if (list == null)
			return false;

		foreach (LED _obj in list) {
			plugin.connection.register_object (_obj.dbus_path, _obj);
		}

		return true;
	}
}
