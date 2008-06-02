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
		public HashTable<string, Plugin> loadedTable = new HashTable<string, Plugin>((HashFunc)str_hash, (EqualFunc)str_equal);
		private string dev_name = new string();		
		private KeyFile conf_file = new KeyFile();

		public static DBus.Connection conn {
			get;
			set construct;
		}

		
		construct {

			Idle.add(this.idle);
			string[] plugins;
			Timeout.add_seconds(50, this.timeout);
			
			conf_file.set_list_separator(',');
			try {
				/* Hard coded, change later */
				conf_file.load_from_file("/etc/odeviced.conf", KeyFileFlags.NONE);
				if (conf_file.has_group("odeviced"))
					this.dev_name = conf_file.get_string("odeviced", "device_name");
				plugins = conf_file.get_string_list("odeviced", "autoload");
				load_multiple(plugins);
			}
			catch (Error error) {
				print( "Oops %s\n", error.message);
			}
			
		}
		

		public Service(DBus.Connection conn) {
			this.conn = conn;
		}
			

		public bool load(string plugin_name) {

			var plugin_path = Module.build_path("/usr/lib/odeviced/plugins", plugin_name);
			File _file = File.new_for_path(plugin_path);

			debug("No of plugins loaded: %d", this.loadedTable.size());

			message("Trying to load %s", plugin_name);
			if(!_file.query_exists(null)) {
				critical("Module file %s, not found", plugin_path);
				return false;
			}
			
			/* Check the hash table if the plugin exists */
			if(this.is_loaded(plugin_name)) {
				message("Plugin already loaded");
				return true;
			}

			/* Get dependencies of the plugin and try to load them */
			if(conf_file.has_group(plugin_name) && conf_file.has_key(plugin_name, "depends")) {
				string[] _deps = conf_file.get_string_list(plugin_name, "depends");
				print("\t%s has dependencies\n", plugin_name);
				load_multiple(_deps);
				print("\tDone handling dependencies\n", plugin_name);
			}			
		
			Plugin plugin = new ODeviced.Plugin(plugin_name, plugin_path);
						
			if(plugin.register(this.conn)) {
				/* This throws up an error in valac atm
				plugin.depends = _deps; */
				this.loadedTable.insert(plugin_name, plugin);
				message("Successfully loaded %s\n", plugin_name);
				return true;
			}
			critical("Unknown Error: Couldn't load %s", plugin_name);
			return false;	
			
		}


		public bool unload(string plugin_name) {
			
			message("Trying to unload %s", plugin_name);
			if(!is_loaded(plugin_name)) {
				message("Plugin %s already unloaded\n", plugin_name);
				return true;
			}

			Plugin plugin = this.loadedTable.lookup(plugin_name);
			
			if(plugin.unregister()) {
				plugin.unref();
				this.loadedTable.remove(plugin_name);
				message("Plugin %s unloaded\n", plugin_name);
				return true;
			}

			message("Plugin couldn't be unloaded\n");
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
 

		public string get_device_name() {
			return this.dev_name;
		}

		 
		public bool is_loaded(string plugin_name) {

			if(this.loadedTable.lookup(plugin_name)==null) 
				return false;
			return true;

		}

		/* Not implemented in vala yet
		public weak List get_loaded_plugins() {
			return this.loaded;
		}
		*/
		
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

			
		[DBus (visible="false")]
		public void run() {
			loop.run();
		}

				
	}

       
}
