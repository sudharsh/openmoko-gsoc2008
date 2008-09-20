/* 
 * Time.vala
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
using DBus;
using GLib;
using Subsystem;

namespace Time {

	[DBus (name = "org.freesmartphone.Time")]
	public class DBusIface: GLib.Object {
		
		public string GetLocalTime() {
		return "Not Implemented Yet";
		}
		
	}
	
	/* Time Subsystem */
	public class Manager: Subsystem.Manager {

		private DBusIface _iface = new Time.DBusIface();	
		public DBusIface iface {
			get { return _iface; }
		}
		
		construct {
			this.dbus_iface = "org.freesmartphone.Time";
		}
		
		public override string[] ListObjectsByInterface(string iface) {
			string[] paths = new string[1];
			paths[0] = "/org/freesmartphone/Time";
			return paths;
		}
		
	}
	
}
	

public bool TimeFactory(FSO.Service service) {

	int result = service.request_name("otimed");
	if (result == DBus.RequestNameReply.PRIMARY_OWNER) {
		Time.Manager manager = new Time.Manager();
		FSO.connection.register_object ("/org/freesmartphone/Time", manager.iface);
		service.fso_objects.append((Subsystem.Manager) manager);
		return true;
	}
	return false;
	
}

	

	
		