/* 
 *  service.vala
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

using GLib;

namespace ODeviced {	

	[DBus (name = "org.freesmartphone.Device")]
	public class Service: Object {

		MainLoop loop = new MainLoop (null, false);		
		HashTable<string, Plugin> loadedTable = new HashTable<string, Plugin>((HashFunc)str_hash, (EqualFunc)str_equal);
		List<Plugin> obj_list = new List<Plugin> ();
			   
		protected static string dev_name = new string();
		protected static string conf_dir_plugins = new string();
		
		private KeyFile conf_file = new KeyFile();
		private string plugins_location = new string();
		

	   	construct {
			
			Idle.add(this.idle);
			string[] plugins;
			Timeout.add_seconds(50, this.timeout);
			
			conf_file.set_list_separator(',');
			try {
				/* Hard coded, change later */
				conf_file.load_from_file("/etc/odeviced.conf", KeyFileFlags.NONE);
				if (conf_file.has_group("odeviced")) {
					dev_name = conf_file.get_string("odeviced", "device_name");
					this.plugins_location = conf_file.get_string("odeviced", "plugins_path");
				    conf_dir_plugins = conf_file.get_string("odeviced", "plugins_conf");
				}

				if (conf_file.has_key("odeviced", "enable")) {
					var plugins = conf_file.get_string_list("odeviced", "enable");
					load_multiple(plugins);
				}
				else	
					probe_plugins(this.plugins_location);

				this.obj_list = this.loadedTable.get_values();
			}
			catch (Error error) {
				print( "Oops %s\n", error.message);
			}
			
		}
		
		public void ListObjectsByInterface (string iface) {
		
			foreach (ODeviced.Plugin plugin in this.obj_list) {
				uint length = plugin.dbus_object_paths.length();
				
				if(iface == plugin.dbus_iface) {
					uint i = 0;
					foreach (string _path in plugin.dbus_object_paths) {
						message (_path);
					}
				}
			}
		}
		
		

		/* Private methods... */
		private bool load(string plugin_name) {
			
			var plugin_path = this.plugins_location + "/" + plugin_name + ".so";
			string[] _deps;
			
			debug("No of plugins already loaded: %d", this.loadedTable.size());
			
			/* Check the hash table if the plugin exists */
			if(this.is_loaded(plugin_name)) {
				message("Plugin already loaded");
				return true;
			}
			
			KeyFile _plugin_conf = new KeyFile();
			try {
				_plugin_conf.load_from_file(this.conf_dir_plugins + "/" + plugin_name + ".plugin", KeyFileFlags.NONE);
				
				/* Get dependencies of the plugin and try to load them */
				if(_plugin_conf.has_group(plugin_name) && _plugin_conf.has_key(plugin_name, "depends")) {
					_deps = conf_file.get_string_list(plugin_name, "depends");
					print("\t%s has dependencies\n", plugin_name);
					load_multiple(_deps);
					print("\tDone handling dependencies\n", plugin_name);
				}

				Plugin plugin = new ODeviced.Plugin(plugin_name, plugin_path);
			
				if(plugin.register()) {
					/* This throws up an error in valac atm 
					   plugin.depends = _deps; */
					this.loadedTable.insert(plugin_name, plugin);
					message("Successfully loaded %s\n", plugin_name);
					return true;
				}
			
			}
			catch (ODeviced.PluginError error) {
				message(error.message);
			}
						
			catch (GLib.Error error) {
				warning("No configuration file for %s", plugin_name);
			}
			
			return false;	
			
		}
		

		private void load_multiple(string[] plugins) {
			
			foreach (string plugin in plugins) {
					if(this.is_loaded(plugin)) {
						print("\tDependency %s already loaded\n", plugin);
						continue;
					}
					else
						load(plugin);
					
			}
			
		}
		
		
		private bool is_loaded(string plugin_name) {
			
			if(this.loadedTable.lookup(plugin_name)==null) 
				return false;
			return true;
			
		}
		

		private void probe_plugins(string plugins_path) {
				
			message("Probing plugins at %s\n", plugins_path);
			
			string[] _temp;

			try {
				var dir = Dir.open(plugins_path, 0);
				string plugin = dir.read_name();
				while (plugin!=null) {
					if(plugin.has_suffix(".so")) {
						_temp = plugin.split(".", 0);
						message("Probing " + _temp[0]);
						load(_temp[0]);
					}
					plugin = dir.read_name();
				}
			}

			catch(GLib.FileError error) {
				warning(error.message);
			}
			
		}

		
		private bool timeout() {
			//log("", LogLevelFLags.LEVEL_INFO, "Timeout");
			message("timeout");
			return true;
		}


		private bool idle() { 
			//log("", LogLevelFlags.LEVEL_INFO, "idle");
			message("idle");
			return false;
		}

		[DBus (visible = false)]	
		public void run() {
			loop.run();
			
		}
		
	}

}
