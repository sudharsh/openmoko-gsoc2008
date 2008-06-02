/* 
 *  helpers.vala
 *  Author: Sudharshan "Sup3rkiddo" sudharsh@gmail.com>
 *  All Rights Reserved
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */


/* static helper class to be used by the plugins */

using GLib;
using DBus;

namespace ODeviced {
	
	public class Helpers: GLib.Object {
		
		public static void register_dbus_object(Plugin plugin, GLib.Object obj) {
			plugin.conn.register_object ("/org/freesmartphone/Device/plugins/" + plugin.name, obj);
			plugin.plugin_instance = obj;
		}


		public static void register_type(Plugin plugin, Type type) {
			plugin.plugin_type = type;
		}
	}
}