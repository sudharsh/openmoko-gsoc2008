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

using DBus;
using ODeviced;


[DBus (name = "org.freesmartphone.Device.Display")]
public class Display: GLib.Object {
	
	private int max_brightness;
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

	Display(string node, string dbus_path) {
		this.node = node;
		this.dbus_path = dbus_path;
	}
		
	construct {
		try {
			GLib.KeyFile _file = new GLib.KeyFile();
			_file.load_from_file("/usr/share/odeviced/plugins/display.plugin", GLib.KeyFileFlags.NONE);
		
			var dev = ODeviced.get_device();
			this.name = ODeviced.compute_name(dbus_path);
			this.max_brightness = ODeviced.read_integer(this.node + "/max_brightness");
		}

		catch (GLib.Error e) {
			GLib.critical(e.message);
		}
	}
		
	public int GetMaximumBrightness() {
		return this.max_brightness;
	}

	public bool SetBrightness(int brightness) {
		if(brightness > this.max_brightness)
			return false;
		ODeviced.write_integer(this.node + "/brightness", brightness);
		return true;
	}

	public int GetBrightness() {
		return ODeviced.read_integer(this.node + "/actual_brightness");
	}

	public bool GetBacklightPower() {
		return ODeviced.read_integer(this.node + "/bl_power")==0;
	}

	public void SetBacklightPower(bool power) {
		int _val;
		if (power)
			_val = 0;
		else
			_val = 1;
		ODeviced.write_integer(this.node + "/bl_power", _val);
	}

	public string GetName() {
		return this.name;
	}


}


namespace display {

	public static List<Display> list;

	public bool init (ODeviced.Plugin plugin) {
		Type type;
		type = typeof (Display);
		list = ODeviced.compute_objects (plugin, type);
		if (list == null)
			return false;

		foreach (Display _obj in list) {
			register_dbus (_obj);
		}

		return true;
	}
}


static void register_dbus (Display obj) {
	GLib.message("Registering DBus object at %s", obj.dbus_path);
	ODeviced.connection.register_object(obj.dbus_path, obj);
}