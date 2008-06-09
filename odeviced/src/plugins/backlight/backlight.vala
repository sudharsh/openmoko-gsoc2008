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

[DBus (name = "org.freesmartphone.Device.Plugins.Backlight")]
public class BacklightPlugin: GLib.Object {
	
	private int max_brightness;
	private string max_brightness_node;
	private string set_brightness_node;
	private string curr_brightness_node;
	
	construct {
		try {
			GLib.KeyFile _file = new GLib.KeyFile();
			_file.load_from_file("/usr/share/odeviced/plugins/backlight.plugin", GLib.KeyFileFlags.NONE);
		
			var dev = ODeviced.get_device();
			this.max_brightness_node = _file.get_string(dev, "max_brightness_node");
			this.set_brightness_node = _file.get_string(dev, "set_brightness_node");
			this.curr_brightness_node = _file.get_string(dev, "curr_brightness_node");
			this.max_brightness = ODeviced.read_integer(this.max_brightness_node);
		}

		catch (Error e) {
			GLib.critical(e.message);
		}
	}
		

	public int get_max_brightness() {
		return this.max_brightness;
	}

	public bool set_brightness(int brightness) {
		if(brightness > this.max_brightness)
			return false;
		ODeviced.write_integer(this.curr_brightness_node, brightness);
		return true;
	}

	public int get_curr_brightness() {
		return ODeviced.read_integer(this.curr_brightness_node);
	}

}
	