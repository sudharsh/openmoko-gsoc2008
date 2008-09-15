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

using Subsystem;
using GLib;
using DBus;

public class Device: Subsystem.Manager {

   	 public bool init_subsystem() {
		 this._library = Module.open(this.path, ModuleFlags.BIND_LAZY | ModuleFlags.BIND_LOCAL);				
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
 
		 