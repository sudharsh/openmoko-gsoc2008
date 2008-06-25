/* odeviced.vala
 *
 * Copyright (C) 2008  Sudharshan S
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
 * 	Sudharshan S <sudharsh@gmail.com>
 */

using GLib;

namespace ODeviced {

	public static DBus.Connection connection;
	
	void main(string[] args) {

		try {
			connection = DBus.Bus.get(DBus.BusType.SYSTEM);		
			dynamic DBus.Object bus =
				connection.get_object ("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");
			uint result = bus.RequestName ("org.freesmartphone.odeviced", (uint) 0);

			if (result == DBus.RequestNameReply.PRIMARY_OWNER) {

				print("Starting ODeviced Server....\n");
				var service = new Service();
				ODeviced.connection.register_object ("/org/freesmartphone/Device", service);
			
				if(!GLib.Module.supported()) {
					critical("Modules are not supported in the current system");
				}		
       				
				service.run();
				
			}
			else {
				/* If odeviced is already running */
				print("ODeviced already running!\n");			       
			}
			
		}
		catch (Error error) {
			stderr.printf("%s\n", error.message);
		}
		
	}

}

