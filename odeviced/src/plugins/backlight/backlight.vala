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

/*
 * This file exists to generate the sources.
 * $ valac -C backlight.vala --pkg dbus-glib-1
 */

using DBus;
using ODeviced;

[DBus (name = "org.freesmartphone.Device.Backlight")]
public class BacklightPlugin: GLib.Object {
	
	private int max_brightness;
	public string node {
		get;
		construct;
	}

	public string dbus_path {
		get;
		construct;
	}

	BacklightPlugin(string node, string dbus_path) {
		this.node = node;
		this.dbus_path = dbus_path;
	}
		
	construct {
		try {
			GLib.KeyFile _file = new GLib.KeyFile();
			_file.load_from_file("/usr/share/odeviced/plugins/backlight.plugin", GLib.KeyFileFlags.NONE);
		
			var dev = ODeviced.get_device();
			this.max_brightness = ODeviced.read_integer(this.node + "/max_brightness");
		}

		catch (Error e) {
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

	public int GetCurrentBrightness() {
		return ODeviced.read_integer(this.node + "/actual_brightness");
	}

	public bool GetBacklightPower() {
		return ODeviced.read_integer(this.node + "/bl_power")==0;
	}

	public void SetBacklightPower(bool power) {
		int _val;
		if (power)
			_val = 1;
		else
			_val = 0;
		ODeviced.write_integer(this.node + "/bl_power", _val);
	}

/* Using auto-detected sysfs nodes


G_MODULE_EXPORT gboolean backlight_init (ODevicedPlugin *plugin) {
	GType type;
	GList *list = NULL;
	BacklightPlugin *obj;
	type = backlight_plugin_get_type();
	list = odeviced_compute_objects (plugin, type);
	if(!list)
	return FALSE;
	g_list_foreach(list, (GFunc)register_dbus, NULL);
		
	return TRUE;
}

*/

}


void register_dbus (BacklightPlugin obj) {
	GLib.message("Registering DBus object at %s", obj.dbus_path);
	ODeviced.connection.register_object(obj.dbus_path, obj);
}