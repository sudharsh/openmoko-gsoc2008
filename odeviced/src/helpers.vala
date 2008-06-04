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
	
	namespace Helpers {
		
		public static void register_dbus_object(Plugin plugin, GLib.Object interface_obj) {
			KeyFile plugin_conf;
			plugin_conf.set_list_separator(',');
			try {
				plugin_conf.load_from_file("/usr/share/odeviced/plugins/" + plugin.name + ".conf", KeyFileFlags.NONE);
				if(plugin_conf.has_group(plugin.name)) {
					plugin.dbus_object_path = plugin_conf.get_string(plugin.name, "dbus_object_path");				
					plugin.conn.register_object (plugin.dbus_object_path, interface_obj);
					plugin.plugin_instance = obj;
				}
				else 
					critical("Malformed plugin configuration file");
			}
			catch (GLib.Error error) {
				message("Plugin configuration not found, %s", error.message);
			}

		}

	}

}
