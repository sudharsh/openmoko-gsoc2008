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

[DBus (name = "org.freesmartphone.Device.Plugins.Backlight")]
public class BacklightPlugin: GLib.Object {
	
	private int max_brightness;
	private string max_brightness_node;
	
	construct {
		GLib.KeyFile _file = new GLib.KeyFile();
		_file.load_from_file("/usr/share/odeviced/plugins/backlight.conf", GLib.KeyFileFlags.NONE);
		this.max_brightness_node = _file.get_string("backlight", "max_brightness_node");
		
	}
		

	public int get_max_brightness() {
		return 1;
	}

	public bool set_brightness() {
		return true;
	}

}
	