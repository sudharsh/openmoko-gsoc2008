/* 
 * info.vala
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
using ODeviced;
using FSOD;
using Device;

/* Info plugin that returns basic CPU info */
[DBus (name = "org.freesmartphone.Device.Info")]
public class Info : GLib.Object {
	
	private const string cpuinfo = "/proc/cpuinfo";
	
	[DBus (visible = false)]
	public Plugin plugin {
		get;
		construct;
	}

	Info (Plugin plugin) {
		this.plugin = plugin;
	}
	
	public HashTable<string, string> GetCpuInfo() {

		File node_file = File.new_for_path(cpuinfo);
		DataInputStream stream;
		string line = new string();
		HashTable<string, string> _ret = new HashTable<string, string> ((HashFunc)str_hash,
																		(EqualFunc)str_equal);
		stream = new DataInputStream(node_file.read(null));
		try {
			line = stream.read_line(null, null);
			while ((line = stream.read_line(null, null)) != null) {
				if (line == "\n" || line == "")
					continue;
				
				string[] _list = line.split(":");
				if ((_list[1] != "") && (_list[0] != "")) {
					_ret.insert (_list[0].strip(), _list[1].strip());
				}
			}
		}
		catch (GLib.Error error) {
			log ("Device.Info", LogLevelFlags.LEVEL_WARNING, error.message);
		}
		return _ret;
	}

}
	

namespace info {

	public static Info obj;

	public bool init (Plugin plugin) {		
		obj = new Info(plugin);
		if(obj == null)
			return false;

		plugin.register_dbus_object (obj);
		return true;
	}
}


		
