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
using FSO;
using Device;

[DBus (name = "org.freesmartphone.Plugin.Display")]
public class Display: GLib.Object {
	
	private int max_brightness;
	private int curr_brightness;
	private string name = new string();
	private int fb_fd;

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

	Display(string node, string dbus_path, Plugin plugin) {
		this.node = node;
		this.dbus_path = dbus_path;
		this.plugin = plugin;
	}
		
	construct {
		var dev = ODeviced.get_device();
		this.name = ODeviced.compute_name(dbus_path);
		this.max_brightness = ODeviced.read_integer(this.node + "/max_brightness");
		this.curr_brightness = this.GetBrightness();
		try {
			var _fb = new IOChannel.file ("/dev/fb0", "r");
			this.fb_fd = _fb.unix_get_fd();
		}
		catch (GLib.Error error) {
			warning (error.message);
		}
	}
		
	public int GetMaximumBrightness() {
		return this.max_brightness;
	}

	public bool SetBrightness(int brightness) {
		int value = GetBrightness();
		if(brightness > this.max_brightness)
			return false;
		if(this.curr_brightness!=value) {
			ODeviced.write_integer(this.node + "/brightness", brightness);
			if (this.curr_brightness == 0) 
				DisplayHelpers.set_fb(true, this.fb_fd);
			else if(value == 0)
				DisplayHelpers.set_fb(false, this.fb_fd);
			this.curr_brightness = value;
		}
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

	public bool init (Plugin plugin) {
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
	FSO.connection.register_object(obj.dbus_path, obj);
}
