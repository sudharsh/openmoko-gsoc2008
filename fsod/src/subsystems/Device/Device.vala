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
	
	construct {
		var _conf_path  = "/usr/share/fsod/subsystems/Device" + "/" + this.name + ".plugin";
		try {
			this._conf.load_from_file(_conf_path, KeyFileFlags.NONE);
			this._conf.set_list_separator(',');
			this.dbus_iface = this._conf.get_string (this.name, "dbus_interface");
		}
		catch (GLib.Error error) {
			critical("Plugin configuration file for %s malformed/not found : %s", this.name, error.message);
		}
	}

	delegate bool InitFunc(Device dev);
	
	Device (string name, string path) {
		this.name = name;
		this.path = path;
	}

	public override string[] ListObjectsByInterface(string iface) {
		uint length = this.dbus_object_paths.length();
		string[] ret = new string[length];
	   	int i = 0;  
		foreach (string path in this.dbus_object_paths) {
			ret[i] = path;
			i++;
		}
		return ret;		
	}

	
	public bool init_system() {		
		/* Resolve symbols only when necessary and don't pollute the global namespace */
		log("Device", LogLevelFlags.LEVEL_INFO, "Loading plugin at %s", this.path);
		this.library = Module.open(this.path, ModuleFlags.BIND_LAZY | ModuleFlags.BIND_LOCAL);				
		if(this.library == null) {
			warning ("library is null, possibly some symbol error");
			return false;
		}		

		/* Don't load the plugin is disable key is present */
		if(this.conf.has_key(name, "disable"))
			return true;

		/* If the plugin uses sysfs, check if the device class exists */
		if(this.conf.has_key (name, "device_class")) {
			var _dev = this.conf.get_string (name, "device_class");
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

 

public bool factory(FSO.Service service) {
	
	uint result = service.request_name("odeviced");
	if (result == DBus.RequestNameReply.PRIMARY_OWNER) {
		try {
			Dir dir = Dir.open("/usr/lib/fsod/subsystems/Device", 0);
			string plugin = dir.read_name();
			while (plugin!=null) {
				var path = "/usr/lib/fsod/subsystems/Device/" + plugin;
				if(plugin.has_suffix (".so")) {
					Device dev = new Device(plugin.split(".")[0], path);
					dev.init_system();						
					service.fso_objects.append((Subsystem.Manager)dev);
				}
				plugin = dir.read_name();
			}			
			return true;
		}
		catch (GLib.Error error) {
			message (error.message);
		}
	}
	return false;
}
			
					
				
				
