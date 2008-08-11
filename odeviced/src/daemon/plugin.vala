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


	public class PluginManager: GLib.Object {

		private Module _library;
		public Module library {
			get { return _library; }
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

		private delegate bool InitFunc(PluginManager plugin);
		
	    public string path {
			public get;
			construct;
		}
		
		public string name {
			public get;
			construct;
		}
		
		public PluginManager(string name, string path) {
			this.name = name;		
			this.path = path;
		}
		
		construct {
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

			this._library = Module.open(this.path, ModuleFlags.BIND_LOCAL);				
			if(this._library == null) {
				warning ("library is null, possibly some symbol error");
				throw new PluginError.LOAD_ERROR("_library is null");
			}
			
			/* If the plugin uses sysfs, check if the device class exists */
			if(this.conf.has_key (name, "device_class")) {
				var _dev = this.conf.get_string (name, "device_class");
				if (!FileUtils.test ("/sys/class/" + _dev, FileTest.IS_DIR)) 
					throw new PluginError.LOAD_ERROR("Device class %s doesn't exist".printf(name));
			}				

			var _symbol = null;
			if(!this._library.symbol(name + "_init", out _symbol)) {
				throw new PluginError.LOAD_ERROR("Malformed odeviced plugin");
			}

			InitFunc func = (InitFunc)_symbol;
			var success = func(this);
			if (!success) {
				message (Module.error());
			}
			else
				this._library.make_resident();
			
			return success;
									
		}
						
	}

}
