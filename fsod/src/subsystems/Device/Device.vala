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
using Subsystem;
using GLib;
using DBus;
using ODeviced;

public class Device: Subsystem.Manager {

	delegate bool InitFunc(Device dev);

	Device (string name, string path) {
			this.name = name;
			this.path = path;
	}

	public override bool init_subsystem() {		
		/* Resolve symbols only when necessary and don't pollute the global namespace */
		this.library = Module.open(this.path, ModuleFlags.BIND_LAZY | ModuleFlags.BIND_LOCAL);				
		if(this.library == null) {
			warning ("library is null, possibly some symbol error");
		}		 

		/* If the plugin uses sysfs, check if the device class exists */
		if(this.conf.has_key (name, "device_class")) {
			var _dev = this.conf.get_string (name, "device_class");
			if (!FileUtils.test ("/sys/class/" + _dev, FileTest.IS_DIR)) 
				return false;	
		}	
		var _symbol = null;
		if(!this.library.symbol(name + "_init", out _symbol)) {
			log("Device", LogLevelFlags.LEVEL_WARNING, "Malformed odeviced plugin");
		}	
		InitFunc func = (InitFunc)_symbol;

		/* This calls the foo_init functions of the Device plugins */
		var success = func(this);
		if (!success) {
			message (Module.error());
		 }
		else
			this.library.make_resident();
		
		
		return success;
	}
}
 

public bool factory() {
	
	dynamic DBus.Object bus = FSO.connection.get_object ("org.freedesktop.DBus", 
														 "/org/freedesktop/DBus", "org.freedesktop.DBus");
	uint result = bus.RequestName ("org.freesmartphone.odeviced", (uint) 0);
	if (result == DBus.RequestNameReply.PRIMARY_OWNER) {
		try {
			Dir dir = Dir.open("/usr/lib/fsod/subsystems/Device", 0);
			var plugin = dir.read_name();

			while ((plugin = dir.read_name())!=null) {
				var path = "/usr/lib/fsod/subsystems/Device/" + plugin;
				if(plugin.has_suffix (".so")) {
					message ("loading " + plugin);
					Device dev = new Device(plugin.split(".")[0], path);
					dev.init_subsystem();			
				}
			}
			return true;
		}
		catch (GLib.Error error) {
			message (error.message);
		}
	}
	return false;
}
					
					
				
				
