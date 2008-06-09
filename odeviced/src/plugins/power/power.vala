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
	private string status = new string();
	private KeyFile conf = new KeyFile();
	private int max_energy = new string();
	private int low_energy_threshold;
	private int status_poll_interval;

	public signal void battery_status_changed(string status);

	private string _curr_status;
	
	construct {
		
		Timeout.add_seconds(300, poll_energy);
		try {
			var dev = ODeviced.get_device();
			conf.load_from_file("/usr/share/odeviced/plugins/power.plugin", KeyFileFlags.NONE);
			this.power_supply_node = conf.get_string(dev, "power_supply_node");
			var _min = conf.get_integer(dev, "low_energy_threshold");
			this.status_poll_interval = conf.get_integer(dev, "status_poll_interval");
			this.max_energy = ODeviced.read_integer (this.power_supply_node + "/energy_full");
			/* Prolly use this for warning during low battery */
			this.low_energy_threshold = this.max_energy * (_min/100);

			Timeout.add_seconds(this.status_poll_interval, poll_status);
			this._curr_status = battery_status();
		}
		catch (GLib.Error error) {
			critical(error.message);
		}
	}

	public int current_energy() {
		return ODeviced.read_integer(this.power_supply_node + "/energy_now");
	}

	public int get_max_energy() {
		return this.max_energy;
	}

	public int energy_full_design() {
		return ODeviced.read_integer(this.power_supply_node + "/energy_full_design");
	}

	public string battery_status() {
		return ODeviced.read_string(this.power_supply_node + "/status");
	}

	public string type() {
		return ODeviced.read_string(this.power_supply_node + "/type");
	}

	public string model_name() {
		return ODeviced.read_string(this.power_supply_node + "/model_name");
	}

	public string manufacturer() {
		return ODeviced.read_string(this.power_supply_node + "/manufacturer");
	}

	public string technology() {
		return ODeviced.read_string(this.power_supply_node + "/technology");
	}

	private bool poll_energy() {
		var _curr = current_energy();
		message("Current energy, %d", _curr);
		return true;
	}

	private bool poll_status() {
		var _curr = battery_status();
		if(_curr != this._curr_status) {
			message("\tStatus changed, %s", _curr);
			this.battery_status_changed(_curr);
			this._curr_status = _curr;
		}
		return true;
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

}
