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

	BacklightPlugin(string node) {
		this.node = node;
	}
		
	construct {
		try {
			GLib.KeyFile _file = new GLib.KeyFile();
			_file.load_from_file("/usr/share/odeviced/plugins/backlight.plugin", GLib.KeyFileFlags.NONE);
		
			var dev = ODeviced.get_device();
			this.max_brightness = ODeviced.read_integer("/sys/class/backlight/" + this.node + "/max_brightness");
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

/*
G_MODULE_EXPORT gboolean backlight_init (ODevicedPlugin *plugin) {
	GType type;
	type = backlight_plugin_get_type();
	odeviced_compute_dbus_paths (plugin, type);
	return TRUE;
}


G_MODULE_EXPORT gboolean backlight_init (ODevicedPlugin *plugin) {
	GType type;
	Backlight *obj;
	gchar **paths;
	type = backlight_plugin_get_type();
	paths = odeviced_compute_dbus_paths (plugin, type);
	for(; *paths!=NULL; **paths++) {
		obj = g_object_new()
	}
	
	return TRUE;
}

*/

}
	

bool backlight_init (ODeviced.Plugin plugin) {

	string[] nodes;
	nodes = ODeviced.compute_dbus_paths(plugin);
	foreach (string node in nodes) {
		var obj = new BacklightPlugin("/sys/class/backlight/" + node);
		plugin.conn.register_object("/org/freesmartphone/Device/Backlight/" + node, obj);
	}

	return true;
}