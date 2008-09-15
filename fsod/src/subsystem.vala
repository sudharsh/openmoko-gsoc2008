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
		
		public Module library;
		private string _dbus_iface;
		public string dbus_iface {
			get { return _dbus_iface; }
		}

		public List<string> dbus_object_paths = new List<string> ();

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
		
		construct {
			var _conf_path  = "/usr/share/fsod/subsystems/Device" + "/" + this.name + ".plugin";
			try {
				this._conf.load_from_file(_conf_path, KeyFileFlags.NONE);
				this._conf.set_list_separator(',');
				this._dbus_iface = this._conf.get_string (this.name, "dbus_interface");
			}
			catch (GLib.Error error) {
				critical("Plugin configuration file for %s malformed/not found : %s", this.name, error.message);
			}
		}

		public abstract bool init_subsystem();

	}
}
