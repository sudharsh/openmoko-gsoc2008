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

[DBus (name = "org.freesmartphone.Device.Input")]
public class Input: GLib.Object {

	private string device = new string();
	private string dev_node = "/dev/input";
	private int auxbutton_keycode;
	private int powerbutton_keycode;
	private int touchscreen_keycode;
	private string[] ignore_list;
	private List<IOChannel> channels = new List<IOChannel> ();

   	public signal void @event(string name, string action, int seconds);


   	construct {
		
		KeyFile _conf = new KeyFile();
		Dir dir = Dir.open(dev_node, 0);

		try {
			
			_conf.load_from_file("/usr/share/odeviced/plugins/input.plugin", KeyFileFlags.NONE);
			_conf.set_list_separator (',');
			this.device = ODeviced.get_device();
			this.auxbutton_keycode = _conf.get_integer (device, "auxbutton");
			this.powerbutton_keycode = _conf.get_integer (device, "powerbutton");
			this.touchscreen_keycode = _conf.get_integer (device, "touchscreen");
			this.ignore_list = _conf.get_string_list (device, "ignore_input");
			
			var name = dir.read_name ();
			while (name!=null) {
				/* Wait till vala has support for "in" operator in if clauses*/
				if (name.has_prefix ("event") && !(name[5] == '2' || name[5] == '3')) {
					var _temp = new IOChannel.file (dev_node+"/"+name, "r");
					channels.append (_temp);
					_temp.add_watch (IOCondition.IN, (IOFunc) this.onActivity);
				}
				name = dir.read_name();

			}
		}

		catch (GLib.Error error) {
			message (error.message);
		}

	}

	
	static bool onActivity (IOChannel source, IOCondition condition) {
		message ("Activity in %s", name);
		return false;
	}

}


namespace input {

	public static Input obj;

	public bool init (ODeviced.Plugin plugin) {		
		obj = new Input();
		if(obj == null)
			return false;
		
		ODeviced.register_dbus_object (plugin, obj);
		return true;
	}
}


