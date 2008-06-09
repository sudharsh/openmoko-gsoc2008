/* 
 * powercontrol.vala
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

/* This vala source exists to generate the boilerplate code
   $ valac -C powercontrol.vala --pkg dbus-glib-1*/

using DBus;
using GLib;
using ODeviced;


[DBus (name = "org.freesmartphone.Device.Plugins.Power")]
public class Power: GLib.Object {
	
	private string power_supply_node = new string();
	private KeyFile conf = new KeyFile();

	construct {
		try {
			var dev = ODeviced.get_device();
			conf.load_from_file("/usr/share/odeviced/plugins/power.plugin", KeyFileFlags.NONE);
			this.power_supply_node = conf.get_string(dev, "power_supply_node");
		}
		catch (GLib.Error error) {
			critical(error.message);
		}
	}

	public int current_energy() {
		return ODeviced.read_integer(this.power_supply_node + "/energy_now");
	}

}


/*
 * Uncomment this in the generated file
G_MODULE_EXPORT gboolean power_init (ODevicedPlugin *plugin) {
	Power *powerobj;
	powerobj = power_new();
	odeviced_register_dbus_object (plugin, G_OBJECT(powerobj));
	return TRUE;
}
*/