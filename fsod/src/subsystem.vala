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

using FSO;
using GLib;

namespace Subsystem {
	
	public abstract class Manager: GLib.Object {
		
		private Module library;
		private string _dbus_iface;
		public string dbus_iface {
			get { return _dbus_iface; }
		}
		
		private KeyFile _conf = new KeyFile();
		public KeyFile conf {
			get { return _conf; }
		}
		
		public string path {
			get;
			construct;
		}

		public string name {
			get;
			construct;
		}

		Manager (string name, string path) {
			this.name = name;
			this.path = path;
		}
		
		public abstract bool init_subsystem();

	}
}
