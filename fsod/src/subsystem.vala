/* 
 * subsystem.vala
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
using FSO;
using DBus;

/* abstract manager for fsod subsystems, override this in your subsystems */
namespace Subsystem {

	public abstract class Manager: GLib.Object {

		public Module library; /* Subsystem shared object */

		public string dbus_iface;
		public List<string> dbus_object_paths = new List<string> ();

		public abstract string[] ListObjectsByInterface(string iface);
		
	}
}


