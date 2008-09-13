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

namespace FSO {
	[DBus (name = "org.freesmartphone.Objects")]
	public class Service : GLib.Object {
	
		private MainLoop loop = new MainLoop (null, false);
		private DBus.Connection _connection;
		public DBus.Connection connection {
			get { return connection; }
		}

		[DBus (visible = false)]
		public void run() {
			loop.run();
		}
		
		static void main(string[] args) {
			
			try {
				DBus.Connection connection = DBus.Bus.get(DBus.BusType.SYSTEM);		
				dynamic DBus.Object bus =
				connection.get_object ("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");
				uint result = bus.RequestName ("org.freesmartphone.frameworkd", (uint) 0);
				
				if (result == DBus.RequestNameReply.PRIMARY_OWNER) {
					
					print("Starting fsod....\n");
					var service = new Service();
					connection.register_object ("/org/freesmartphone/Device", service);
					
					if(GLib.Module.supported()) {
						log("FSO Service", LogLevelFlags.LEVEL_ERROR, 
							"Modules are not supported in the current system");
					}		
       			
					service.run();
					
				}
				else {
					/* If odeviced is already running */
					print("ODeviced already running!\n");			       
				}
				
			}
			catch (GLib.Error error) {
				stderr.printf("%s\n", error.message);
			}
		
		}
		
	}

}
