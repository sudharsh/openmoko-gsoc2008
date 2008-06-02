/* 
 *  plugins.vala
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
using DBus;

namespace ODeviced {

	public class Plugin: GLib.Object {

		public Module library;
		public Type type;
		public int handle;
		public string[] depends;
		
		public static DBus.Connection conn;
		public GLib.Object plugin_instance;
		public Type plugin_type;

		delegate bool PluginFunc(Plugin plugin);
		
		public string path {
			public get;
			set construct;
		}


		public string name {
			public get;
			set construct;
		}

	       		
		public Plugin(string name, string path) {
			this.name = name;		
			this.path = path;
		}
		
				
		construct {
			Rand rand = new Rand();
			this.handle = rand.int_range(10, 99);			
		}

		public bool register(DBus.Connection _conn) {
					
			this.conn = _conn;
			this.library = Module.open(this.path, ModuleFlags.BIND_LOCAL);
				
			if(this.library == null) {
				critical("plugin.library null!");
				return false;
			}
			
			var _symbol = null;
			if(!this.library.symbol(name + "_init", out _symbol)) {
				critical("Malformed odeviced plugin");			
				return false;
			}

			PluginFunc func = (PluginFunc)_symbol;
			return func(this);
									
		}

		
		public bool unregister() {
			var _symbol = null;
			if(!this.library.symbol(name + "_close", out _symbol)) {
				critical("Couldn't close the plugin");
				return false;
			}
			
			PluginFunc func = (PluginFunc)_symbol;
			return func(this);

		}

		
	}


}
