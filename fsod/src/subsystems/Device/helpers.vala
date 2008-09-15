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
using Subsystem;
using FSO;

namespace ODeviced {

	public static void register_dbus_object(Manager plugin, GLib.Object interface_obj) {
		
		try {
			if(plugin.conf.has_group(plugin.name)) {
				var at_path = plugin.conf.get_string(plugin.name, "dbus_object_path");
				FSO.connection.register_object (at_path, interface_obj);
				plugin.dbus_object_paths.append (at_path);
			}
			else
				critical("Malformed plugin configuration file");
		}
		catch (GLib.Error error) {
			critical(error.message);
		}
	}
	

	public static string get_device () {
		return "Freerunner";
	}


	public static List? compute_objects(Manager plugin, GLib.Type klass) {
	
		List<GLib.Object> objList = new List<GLib.Object>();
		string dev_class;
		string dev_node;

		if(plugin.conf.has_group(plugin.name)) {
			try {

				dev_class = plugin.conf.get_string(plugin.name, "device_class");
				dev_node = "/sys/class/" + dev_class;
				var dbus_path = plugin.conf.get_string(plugin.name, "dbus_object_path");
			
				Dir dir = Dir.open(dev_node, 0);
				string node = dir.read_name();
				var i = 0;
				while(node!=null) {
					string cleansed_node = ODeviced.cleanup_dbus_path (node);
					var obj = GLib.Object.new(klass, "node", dev_node + "/" + node,
											  "dbus_path", dbus_path + "/" + cleansed_node,
						                      "plugin", plugin);
					plugin.dbus_object_paths.append(dbus_path + "/" + cleansed_node);
					objList.append(obj);
					node = dir.read_name();
					
				}
			}
			catch (GLib.FileError e) {
				message ("Device class: %s doesn't exist", dev_class);
			}
			catch (GLib.KeyFileError e) {
				message ("%s %s", plugin.name, e.message);
			}
		}
		return objList;
	}


	/* plugins using sysfs could use this to create DBus object paths automatically */
	public static string compute_name (string path) {
		string[] split_list = path.split("/", 0);
		var _length = strv_length(split_list);
		return split_list[_length - 1];
	}



	/* DBus doesn't like ':' or '-' in object paths */
	public static string cleanup_dbus_path (string dbus_path) {
		try {
			Regex regex = new Regex ("-|:", RegexCompileFlags.CASELESS, RegexMatchFlags.NOTEMPTY);
			return regex.replace_literal (dbus_path, -1, 0, "_", RegexMatchFlags.NOTEMPTY);
		}
		catch (GLib.RegexError error) {
			message ("Couldn't cleanup dbus_path: %s", error.message);
		}
		return dbus_path;
	}		
	

	/* I know =(, Ideally these should have been a single function */
	public static int read_integer(string node) {
		int val;
		FileStream node_file = FileStream.open(node, "r");
		if(node_file == null) 
			return -1;
		node_file.scanf("%d", ref val);
		return val;
	}
	

	public static string? read_string(string node) {
		File node_file = File.new_for_path(node);
		DataInputStream stream;
		string line;

		try {
			stream = new DataInputStream(node_file.read(null));
			line = stream.read_line(null, null);
		}

		catch (GLib.Error e) {
			message (e.message);
			return null;
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

	
	public static bool write_string (string node, string data) {
		FileStream node_file = FileStream.open (node, "w");
		if (node_file == null)
			return false;
		node_file.printf ("%s", data);		
		return true;
	}
		
}


