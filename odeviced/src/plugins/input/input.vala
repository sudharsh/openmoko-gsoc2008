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
using InputHelpers;

[DBus (name = "org.freesmartphone.Device.Input")]
public class Input: GLib.Object {

	private string device = new string();
	private string dev_node = "/dev/input";
	private HashTable<string, int> watches = new HashTable<string, int> ((HashFunc)str_hash, (EqualFunc)str_equal);
	private Queue<int> event_q = new Queue<int> ();

	private string[] ignore_list;
	private List<IOChannel> channels = new List<IOChannel> ();
	private string[] reportheld;

   	public signal void @event(string name, string action, int seconds);

	[DBus (visible = false)]
	public ODeviced.PluginManager plugin {
		get;
		construct;
	}

	Input (ODeviced.PluginManager plugin) {
		this.plugin = plugin;
	}


   	construct {
		
		Dir dir = Dir.open(dev_node, 0);

		try {
			
			this.device = ODeviced.get_device();
			this.ignore_list = plugin.conf.get_string_list (device, "ignore_input");
			this.reportheld = plugin.conf.get_string_list (device, "reportheld");
			var _watchfor = plugin.conf.get_string_list (device, "watchfor");
			compute_watches (_watchfor);
			
			var name = dir.read_name ();
			while (name!=null) {
				/* Wait till vala has support for "in" operator in if clauses*/
				if (name.has_prefix ("event") && !(name[5] == '1' || name[5] == '2' || name[5] == '3')) {
					var channel = new IOChannel.file (dev_node+"/"+name, "r");
					/* See http://bugzilla.gnome.org/show_bug.cgi?id=546898 */
					InputHelpers.process_watch (channel, this);;
				}
				name = dir.read_name();

			}
			
		}

		catch (GLib.Error error) {
			message (error.message);
		}

	}

	
	private void compute_watches (string[] watchfor) {
		foreach (string key in watchfor) {
			try {
				int keycode = plugin.conf.get_integer (device, key);
				this.watches.insert (key, keycode);
			}
			catch (GLib.Error error) {
				message (error.message);
			}
		}
	}		

}


namespace input {

	public static Input obj;

	public bool init (ODeviced.PluginManager plugin) {		
		obj = new Input(plugin);
		if(obj == null)
			return false;
		
		ODeviced.register_dbus_object (plugin, obj);
		return true;
	}
}


