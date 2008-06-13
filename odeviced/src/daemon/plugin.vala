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
		protected KeyFile conf = new KeyFile();
		
		delegate bool PluginFunc(Plugin plugin);
		
		public string path {
			public get;
			construct;
		}


		public string name {
			public get;
			construct;
		}

	       		
		public Plugin(string name, string path) {
			this.name = name;		
			this.path = path;
		}
		
				
		construct {
			Rand rand = new Rand();
			this.handle = rand.int_range(10, 99);
			var _conf  = ODeviced.Service.conf_dir_plugins + "/" + this.name + ".plugin";
			try {
				this.conf.load_from_file(_conf, KeyFileFlags.NONE);
				this.conf.set_list_separator(',');
			}
			catch (GLib.Error error) {
				critical("Plugin configuration file for %s malformed/not found : %s", this.name, error.message);
			}
		}


		public bool register() {

			this.library = Module.open(this.path, ModuleFlags.BIND_MASK);
				
			if(this.library == null) {
				critical("plugin.library null!");
				return false;
			}
			
			this.library.make_resident();
			
			var _symbol = null;
			if(!this.library.symbol(name + "_init", out _symbol)) {
				critical("Malformed odeviced plugin");			
				return false;
			}

			PluginFunc func = (PluginFunc)_symbol;
			return func(this);
									
		}
		
	
	}


}
