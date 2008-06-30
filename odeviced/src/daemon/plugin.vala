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


	public errordomain PluginError {
		LOAD_ERROR;
	}


	public class Plugin: GLib.Object {

		private Module _library;
		public Module library {
			get { return _library; }
		}
		
		private int _handle;
		public int handle {
			get { return _handle; }
		}

		private string _dbus_iface;
		public string dbus_iface {
			get { return _dbus_iface; }
		}
		
		/* valac doesnt support duplicating string array intances */
		/*private string[] _dbus_object_paths;*/
		public List<string> dbus_object_paths = new List<string> ();
		/*{
		  get { return _dbus_object_paths; }
		  set { _dbus_object_paths = value; }
		  }
		*/
		
		private string[] depends;
		/*public string[] depends {
		  get { return _depends; }
		  set { _depends = value; }
		  }
		*/
		
		private KeyFile _conf = new KeyFile();
		public KeyFile conf {
			get { return _conf; }
		}

		private delegate bool InitFunc(Plugin plugin);
		
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
			this._handle = rand.int_range(10, 99);
			var _conf_path  = ODeviced.Service.conf_dir_plugins + "/" + this.name + ".plugin";
			try {
				this._conf.load_from_file(_conf_path, KeyFileFlags.NONE);
				this._conf.set_list_separator(',');
				this._dbus_iface = this._conf.get_string (this.name, "dbus_interface");
			}
			catch (GLib.Error error) {
				critical("Plugin configuration file for %s malformed/not found : %s", this.name, error.message);
			}
		}


		public bool register() throws PluginError {

			this._library = Module.open(this.path, ModuleFlags.BIND_LAZY);
				
			if(this._library == null) {
				throw new PluginError.LOAD_ERROR("_library is null");
			}
			
			this._library.make_resident();
			
			var _symbol = null;
			if(!this._library.symbol(name + "_init", out _symbol)) {
				throw new PluginError.LOAD_ERROR("Malformed odeviced plugin");
			}

			InitFunc func = (InitFunc)_symbol;
			return func(this);
									
		}
		
				
	}


}
