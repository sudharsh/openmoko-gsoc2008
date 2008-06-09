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
		
	public static void register_dbus_object(Plugin plugin, GLib.Object interface_obj) {
		KeyFile plugin_conf = new KeyFile();
		plugin_conf.set_list_separator(',');
		try {
			plugin_conf.load_from_file(ODeviced.Service.conf_dir_plugins + "/" + plugin.name + ".plugin", KeyFileFlags.NONE);
			if(plugin_conf.has_group(plugin.name)) {
				plugin.dbus_object_path = plugin_conf.get_string(plugin.name, "dbus_object_path");				
				plugin.conn.register_object (plugin.dbus_object_path, interface_obj);
				plugin.plugin_instance = interface_obj;
			}
			else 
				critical("Malformed plugin configuration file");
		}
		catch (GLib.Error error) {
			message("Plugin configuration not found, %s", error.message);
		}
		
	}
	
	public static string get_device () {
		return ODeviced.Service.dev_name;
	}

	public static string get_conf(Plugin plugin) {
		return plugin.conf;
	}

	/* I know =( */
	public static int read_integer(string node) {
		FileStream node_file = FileStream.open(node, "r");
		int val;
		node_file.scanf("%d", ref val);
		return val;		
	}

	public static string read_string(string node) {
		FileStream node_file = FileStream.open(node, "r");
		string val;
		node_file.scanf("%s", val);
		return val;
	}

	public static bool write_integer(string node, int val) {
		FileStream node_file = FileStream.open(node, "w");
		node_file.printf("%d", val);
		return true;
	}
 
}


