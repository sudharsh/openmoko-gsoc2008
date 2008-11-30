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
using FSOD;

namespace FSOD {

	[DBus (name = "org.freesmartphone.Objects")]
	public class Service : GLib.Object {
	
		private KeyFile conf_file = new KeyFile();

		/* hashmap of the objects */
		public HashTable<string, Subsystem.Manager> fso_objects  = new HashTable<string, Subsystem.Manager>((HashFunc)str_hash,
																											(EqualFunc)str_equal);

		private delegate Subsystem.Manager FactoryFunc(Service service);
		protected static string dev_name = new string();
		private Module library;
		private string[] enableList;

		[DBus (visible = false)];
		public DBus.Connection connection {
			get;
			construct;
		}

		private static Service instance = null;

		private Service (DBus.Connection connection) {
			this.connection = connection;
		}

		public static void create_service(DBus.Connection connection) {
			if (instance == null)
				instance = new Service(connection);
		}

		/* FIXME: Do we really need this
 		public static Service? get_service() {
 			if (instance != null)
 				return instance;
 			return null;
 		}
		*/
								
		construct {
			this.connection.register_object ("/org/freesmartphone/Framework", this);
			conf_file.set_list_separator (',');
			Idle.add(this.idle);
			Timeout.add_seconds(50, this.timeout);
			try {
				conf_file.load_from_file (Path.build_filename(Config.SYSCONFDIR, "fsod.conf"), KeyFileFlags.NONE);
				if (conf_file.has_group("fsod")) 
					dev_name = conf_file.get_string ("fsod", "device_name");

				/* Try to load all subsystems if there is no enable_subsystems key */
				if (conf_file.has_key ("fsod", "enable_subsystems")) {
					this.enableList = conf_file.get_string_list ("fsod", "enable_subsystems");
					foreach (string subsystem in this.enableList) {
						load(Path.build_filename(Config.LIBDIR, "fsod/subsystems", subsystem), subsystem);
					}
				}
				else
					probe_subsystems();
			}
				
			catch (GLib.Error error) {
				log ("FSOD Service", LogLevelFlags.LEVEL_WARNING, error.message);
			}
		}
 		
		
		private void probe_subsystems () {
			var path = Path.build_filename(Config.LIBDIR, "fsod/subsystems");
			try {
				string _subsys;
				Dir subsys_dir = Dir.open(path, 0);
				while((_subsys = subsys_dir.read_name()) != null && _subsys.has_suffix(".so")) {
					load(path + "/" +_subsys, _subsys.split(".")[0]);
				}
			}
			catch (GLib.Error error) {
				log ("FSOD Service", LogLevelFlags.LEVEL_WARNING, error.message);
			}
		}

		
		/* Make this static, kinda dumb but vala generates the same code for bus.RequestName in main.vala
		   So giving rise to a multiple definition error for the generated code. Use this from main.vala as 
		   well */
		[DBus (visible = false)]
		public static uint request_name (DBus.Connection connection, string name) {
			uint result;
			try {
				dynamic DBus.Object bus = connection.get_object ("org.freedesktop.DBus", "/org/freedesktop/DBus",
																	 "org.freedesktop.DBus");
				result = bus.RequestName ("org.freesmartphone." + name, (uint) 0);
				
			}
			catch (DBus.Error error) {
				log ("FSOD Service", LogLevelFlags.LEVEL_WARNING,
					 "Check your DBus policy if %s exists: %s", name, error.message);
				result = 100;
			}
			return result;
		}


		/* No array of dbus object paths in vala yet. Just return them as a string array */
		public string[]? ListObjectsByInterface (string iface) {
			Subsystem.Manager manager = this.fso_objects.lookup (iface.split(".")[2]);
			if (manager == null) {
				return null;
			}
			return manager.ListObjectsByInterface (iface);
		}
	

		/* Private methods ... */
		private bool load(string path, string name) {
			
			library = Module.open (path, ModuleFlags.BIND_LAZY);
			if (this.library == null) {
				log ("FSOD Service", LogLevelFlags.LEVEL_WARNING, 
					 "Module.open returned null when loading %s: %s", name,Module.error());
				return false;
			}
			var _init = null;
			if (!this.library.symbol("Init" + name, out _init)) {
				log ("FSOD Service", LogLevelFlags.LEVEL_WARNING,
					 "factory function not found");
				return false;
			}
			FactoryFunc factory_func = (FactoryFunc)_init;
			Subsystem.Manager subsystem = factory_func(this);
			if (subsystem != null) {
				log ("FSOD Service", LogLevelFlags.LEVEL_INFO,
					 "%s loaded", path);
				this.fso_objects.insert (name, subsystem);
				return true;
			}

			log ("FSOD Service", LogLevelFlags.LEVEL_WARNING,
				 "Couldn't load %s", name);
			return false;
		}


		private bool timeout() {
			log ("FSOD Service", LogLevelFlags.LEVEL_INFO, "Timeout");
			return true;
		}


		private bool idle() { 
			log ("FSOD Service", LogLevelFlags.LEVEL_INFO, "idle");
			return false;
		}	
	  
	}

}
