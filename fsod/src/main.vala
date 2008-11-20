/* 
 * main.vala
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

using GLib;
using FSOD;
using DBus;

namespace FSOD {

	static void main(string[] args) {

        if (Options.parse_args(ref args)) {
		
			MainLoop loop = new MainLoop (null, false);	
			try {
				DBus.Connection connection;
				if (Options.session) {
					connection = DBus.Bus.get(DBus.BusType.SESSION);		
				} else {
					connection = DBus.Bus.get(DBus.BusType.SYSTEM);		
				}

				dynamic DBus.Object bus = connection.get_object ("org.freedesktop.DBus", "/org/freedesktop/DBus",
																 "org.freedesktop.DBus");
				uint result = FSOD.Service.request_name (connection, "frameworkd");				
				if (result == DBus.RequestNameReply.PRIMARY_OWNER) {
					print("Starting fsod....\n");
					FSOD.Service.create_service(connection);
					if(!GLib.Module.supported()) {
					log("FSOD Service", LogLevelFlags.LEVEL_ERROR, 
						"Modules are not supported in the current system");
					}		
					
					loop.run();					
				}
				else {
					/* If odeviced is already running */
					print("Error starting fsod. Check if fsod is already running\n");			       
				}
			
			}
			catch (GLib.Error error) {
				stderr.printf("%s\n", error.message);
			}
		}
	}
	
}
