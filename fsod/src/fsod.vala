/* fsod.vala
 *
 * Copyright (C) 2008  Sudharshan 'Sup3rkiddo' S
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author:
 * 	Sudharshan 'Sup3rkiddo' S <sudharsh@gmail.com>
 */

using GLib;
using DBus;
using Subsystem;

namespace FSO {

	protected static DBus.Connection connection;

	[DBus (name = "org.freesmartphone.Objects")]
	public class Service : GLib.Object {

		
		private KeyFile conf_file = new KeyFile();
		HashTable<string, Subsystem.Manager> loadedTable = new HashTable<string, Subsystem.Manager>((HashFunc)str_hash,
																							(EqualFunc)str_equal);

		private delegate bool InitFunc();
		protected static string dev_name = new string();
		Module library;
		private string[] enableList;

		construct {
			conf_file.set_list_separator (',');
			
			try {
				conf_file.load_from_file ("/etc/fsod.conf", KeyFileFlags.NONE);
				if (conf_file.has_group("fsod")) 
					dev_name = conf_file.get_string ("fsod", "device_name");
				if (conf_file.has_key ("fsod", "enable_subsystems")) {
					this.enableList = conf_file.get_string_list ("fsod", "enable_subsystems");
					probe_subsystems();
				}
				
			}
			catch (GLib.Error error) {
				print( "FSO Service - %s\n", error.message);
			}
		}
 		
		
		private void probe_subsystems () {
			var path = "/usr/lib/fsod/subsystems";
			try {
				Dir subsys_dir = Dir.open(path, 0);
				string _subsys = subsys_dir.read_name();
				while((_subsys=subsys_dir.read_name()) != null) {
					if(_subsys.has_suffix(".so"))
						load(path + "/" +_subsys);
				}
			}
			catch (GLib.Error error) {
				message (error.message);
			}
		}
						

		/* Private methods ... */
		private bool load(string path) {
			bool success = false;
			library = Module.open (path, ModuleFlags.BIND_LAZY);
			if (this.library == null) {
				log ("FSO Service", LogLevelFlags.LEVEL_WARNING, 
					 "Library NULL");
				return success;
			}
			var _init = null;
			if (!this.library.symbol("factory", out _init)) {
				log ("FSO Service", LogLevelFlags.LEVEL_WARNING,
					 "factory function not found");
				return success;
			}
			InitFunc _func = (InitFunc)_init;
			success = _func();
			if (success) {
				log ("FSO Service", LogLevelFlags.LEVEL_INFO,
					 "%s loaded", path);
				return success;
			}

			return false;
		}
		  

		static void main(string[] args) {
			MainLoop loop = new MainLoop (null, false);	
			try {
				connection = DBus.Bus.get(DBus.BusType.SYSTEM);		
				dynamic DBus.Object bus = connection.get_object ("org.freedesktop.DBus", "/org/freedesktop/DBus",
																 "org.freedesktop.DBus");
				uint result = bus.RequestName ("org.freesmartphone.frameworkd", (uint) 0);				
				if (result == DBus.RequestNameReply.PRIMARY_OWNER) {
					print("Starting fsod....\n");
					Service service = new Service();
					connection.register_object ("/org/freesmartphone/Framework", service);
					
					if(!GLib.Module.supported()) {
						log("FSO Service", LogLevelFlags.LEVEL_ERROR, 
							"Modules are not supported in the current system");
					}		
       			
					loop.run();					
				}
				else {
					/* If odeviced is already running */
					print("fsod already running!\n");			       
				}
				
			}
			catch (GLib.Error error) {
				stderr.printf("%s\n", error.message);
			}
		
		}
		
	}

}
