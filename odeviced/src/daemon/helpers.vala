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

		if(plugin.conf.has_group(plugin.name)) {
				plugin.dbus_object_path = plugin.conf.get_string(plugin.name, "dbus_object_path");
				plugin.conn.register_object (plugin.dbus_object_path, interface_obj);
				plugin.plugin_instance = interface_obj;
		}
		else 
			critical("Malformed plugin configuration file");
	}
	
	
	public static string get_device () {
		return ODeviced.Service.dev_name;
	}

	[NoArrayLength]
	public static string[]? compute_dbus_paths(Plugin plugin) {
	
		string[] ret = new string[5];

		if(plugin.conf.has_group(plugin.name)) {
			var dev_class = plugin.conf.get_string(plugin.name, "device_class");
			var dev_node = "/sys/class/" + dev_class;
			Dir dir = Dir.open(dev_node, 0);
			string node = dir.read_name();
			var i = 0;
			while(node!=null) {
				message(dev_node + "/" + node);
				ret[i] = node;
				node = dir.read_name();
				i++;
			}
		}
		return ret;
	}
	

	/* I know =(, Ideally these should have been a single function */
	public static int read_integer(string node) {
		FileStream node_file = FileStream.open(node, "r");
		if(node_file == null) 
			return -1;
		int val;
		node_file.scanf("%d", ref val);
		return val;		
	}

	public static string read_string(string node) {
		File node_file = File.new_for_path(node);
		DataInputStream stream;
		string line;

		try {
			stream = new DataInputStream(node_file.read(null));
			line = stream.read_line(null, null);
		}

		catch (GLib.Error e) {
			critical (e.message);
		}

		finally {
			stream.close(null);
		}
		return line;
	}

  	public static bool write_integer(string node, int val) {
		FileStream node_file = FileStream.open(node, "w");
		if(node_file == null) 
			return false;
		node_file.printf("%d", val);
		return true;
	}
 
}


