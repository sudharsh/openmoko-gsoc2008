/* 
 * input.vala
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


using DBus;
using GLib;
using ODeviced;
using FSOD;
using Device;
using InputHelpers;

[DBus (name = "org.freesmartphone.Device.Input")]
public class Input: GLib.Object {

	private string device = new string();
	private const string dev_node = "/dev/input";
	public uint tag;

	private HashTable<uint, string> _watches = new HashTable<uint, string> ((HashFunc)direct_hash, (EqualFunc)direct_equal);
	[DBus (visible = false)]
	public HashTable<uint, string> watches {
		get { return _watches; }
	}	[DBus (visible = false)]

	public Queue<void> event_q = new Queue<void>();

	private List<string> _reportheld = new List<string> ();
	[DBus (visible = false)]
	public List<string> reportheld {
		get { return _reportheld; }
	}

	private string[] watchfor;
	private List<string> ignoreList = new List<string> ();

	public signal void @event(string name, string action, int seconds);

	[DBus (visible = false)]
	public Plugin plugin {
		get;
		construct;
	}

	Input (Plugin plugin) {
		this.plugin = plugin;
	}


   	construct {
		
		try {
			this.device = "Freerunner";
			compute_watches ();
		   
			string[] input_nodes = plugin.conf.get_string_list (this.device, "input_nodes");

			foreach (string input_node in input_nodes) {
					string device_node = this.dev_node + "/" + input_node;
					var channel = new IOChannel.file (device_node, "r");
					/* See http://bugzilla.gnome.org/show_bug.cgi?id=546898 */
					//message ("Adding watch for %s", device_node);
					channel.add_watch (IOCondition.IN, (IOFunc)InputHelpers.on_activity);		  
			}
			
		}
		catch (GLib.KeyFileError error) {
			log ("Device.Input", LogLevelFlags.LEVEL_WARNING, error.message);
		}

	}

	
	private void compute_watches () {
		try {
			this.watchfor = plugin.conf.get_string_list(this.device, "watchfor");
		
			foreach (string key in watchfor) {
			
				string[] settings = plugin.conf.get_string_list (device, key);
				this._watches.insert (settings[0].to_int(), settings[1]);
				if(settings[3] == "true") {
					//message ("\tConfig wants to report held seconds for %s", settings[1]);
					this._reportheld.append(settings[1]);
				}
			}
		}
		catch (GLib.KeyFileError error) {
			log ("Device.Input", LogLevelFlags.LEVEL_WARNING, error.message);
		}
		
	}

}


namespace input {

	public static Input obj;

	public bool init (Plugin plugin) {		
		obj = new Input(plugin);
		if(obj == null)
			return false;
		
		plugin.register_dbus_object (obj);
		return true;
	}
}


