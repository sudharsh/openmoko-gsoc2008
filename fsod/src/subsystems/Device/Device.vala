/* 
 * device.vala
 * Written by Sudharshan "Sup3rkiddo" S <sudharsh@gmail.com>
 * All Rights Reserved
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */ 

using FSO;
using GLib;
using DBus;
using ODeviced;
using Subsystem;


public class Device: Subsystem.Manager {

	private List<Plugin> plugins = new List<Plugin>();

	construct {
		try {
			Dir dir = Dir.open("/usr/lib/fsod/subsystems/Device", 0);
			string plugin_name = dir.read_name();
			while (plugin_name!=null) {
				var path = "/usr/lib/fsod/subsystems/Device/" + plugin_name;
				if(plugin_name.has_suffix (".so")) {
					Plugin plugin = new Plugin(plugin_name.split(".")[0], path);
					plugin.load_plugin();
					this.plugins.append(plugin);
				}
				plugin_name = dir.read_name();
			}			
			
		}
		catch (GLib.Error error) {
			message (error.message);
		}
	}


	public override string[] ListObjectsByInterface(string iface) {
		string[] ret;
		foreach (Plugin plugin in this.plugins) {
			uint length = plugin.dbus_object_paths.length();
			ret = new string[length];
			int i = 0;  
			foreach (string path in plugin.dbus_object_paths) {
				ret[i] = path;
				i++;
			}
			
		}
		return ret;		
	}
}


public class Plugin: GLib.Object {

	public List<string> dbus_object_paths = new List<string> ();
	private string _dbus_iface = new string();
	public string dbus_iface {
		get { return _dbus_iface; }
	}

	private Module library;
   	private KeyFile _conf = new KeyFile();
	public KeyFile conf {
		get { return _conf; }
	}

	public string path {
		get; 
		construct;
	}

	public string name {
		get;
		construct;
	}

	delegate bool InitFunc(Plugin dev);

	Plugin (string name, string path) {
		this.name = name;
		this.path = path;
	}

	
	construct {
		var _conf_path  = "/usr/share/fsod/subsystems/Device" + "/" + this.name + ".plugin";
		try {
			this._conf.load_from_file(_conf_path, KeyFileFlags.NONE);
			this._conf.set_list_separator(',');
			this._dbus_iface = this._conf.get_string (this.name, "dbus_interface");
		}
		catch (GLib.Error error) {
			critical("Plugin configuration file for %s malformed/not found : %s", this.name, error.message);
		}
	}


	public bool load_plugin() {		

		log("Device", LogLevelFlags.LEVEL_INFO, "Loading plugin at %s", this.path);
		this.library = Module.open(this.path, ModuleFlags.BIND_LAZY|ModuleFlags.BIND_LOCAL);				
		if(this.library == null) {
			warning ("library is null, possibly some symbol error");
			return false;
		}		

		/* Don't load the plugin is disable key is present */
		if(this._conf.has_key(name, "disable"))
			return true;

		/* If the plugin uses sysfs, check if the device class exists */
		if(this._conf.has_key (name, "device_class")) {
			var _dev = this._conf.get_string (name, "device_class");
			if (!FileUtils.test ("/sys/class/" + _dev, FileTest.IS_DIR)) 
				return false;	
		}	
		var _symbol = null;
		if(!this.library.symbol(name + "_init", out _symbol)) {
			log("Device", LogLevelFlags.LEVEL_WARNING, "Malformed odeviced plugin");
			return false;
		}	
		InitFunc func = (InitFunc)_symbol;

		/* This calls the foo_init functions of the Device plugins */
		var success = func(this);
		if (!success) {
			message ("Weird error %s", Module.error());
		 }
		else
			this.library.make_resident(); /* Prevent unloading */
				
		return success;
	}
}

 

public bool DeviceFactory(FSO.Service service) {
	
	uint result = service.request_name("odeviced");
	if (result == DBus.RequestNameReply.PRIMARY_OWNER) {
		Device dev = new Device();
		if(dev != null)
			return true;
	}
	return false;
}
			
					
				
				
