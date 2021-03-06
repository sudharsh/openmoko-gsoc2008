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

using FSOD; 
using DBus;
using GLib;
using Subsystem;
using TimeHelpers;

namespace Time {

	[DBus (name = "org.freesmartphone.Time")]
	public class DBusIface: GLib.Object {

		public signal void minute(int sec, int min, int hour, int mday,
								  int mon, int year, int wday, int yday, int isdst);
		construct {
			Timeout.add_seconds (60, this.emit_signal);
		}

		/* FIXME: In the worst case the signal may get emitted with payload that is a minute old */
		private bool emit_signal() {
			TimeHelpers.emit_signal(this);
			return true;
		}

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
	

public Subsystem.Manager? InitTime(FSOD.Service service) {

	uint result = service.request_name(service.connection, "otimed");
	if (result == DBus.RequestNameReply.PRIMARY_OWNER) {
		Time.Manager manager = new Time.Manager();
		return manager;
	}
	return null;
	
}

	

	
		