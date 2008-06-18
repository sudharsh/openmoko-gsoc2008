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


[DBus (name = "org.freesmartphone.Device.PowerSupply")]
public class Power: GLib.Object {
	
	private string status = new string();
	private KeyFile conf = new KeyFile();
	private int max_energy;
	private int low_energy_threshold;
	private int status_poll_interval;

	public signal void battery_status_changed(string status);
	public signal void low_battery(int charge);

	private string curr_status;

	public string node {
		get;
		construct;
	}

	public string dbus_path {
		get;
		construct;
	}

	Power(string node, string dbus_path) {
		this.node = node;
		this.dbus_path = dbus_path;
	}
	
	construct {
		
		Timeout.add_seconds(300, poll_energy);
		try {
			var dev = ODeviced.get_device();
			conf.load_from_file("/usr/share/odeviced/plugins/power.plugin", KeyFileFlags.NONE);
			var _min = conf.get_integer(dev, "low_energy_threshold");
			this.status_poll_interval = conf.get_integer(dev, "status_poll_interval");
			/*this.power_supply_node = conf.get_string(dev, "power_supply_node");*/
			this.max_energy = ODeviced.read_integer (this.node + "/energy_full");
			/* Prolly use this for warning during low battery */
			this.low_energy_threshold = this.max_energy * (_min/100);

			Timeout.add_seconds(this.status_poll_interval, poll_status);
			this.curr_status = GetBatteryStatus();
		}
		catch (GLib.Error error) {
			critical(error.message);
		}
	}

	public int GetCurrentEnergy() {
		return ODeviced.read_integer(this.node + "/energy_now");
	}

	public int GetMaxEnergy() {
		return this.max_energy;
	}

	public int GetEnergyFullDesign() {
		return ODeviced.read_integer(this.node + "/energy_full_design");
	}

	public string GetBatteryStatus() {
		return ODeviced.read_string(this.node + "/status");
	}

	public string GetType() {
		return ODeviced.read_string(this.node + "/type");
	}

	public string GetModel() {
		return ODeviced.read_string(this.node + "/model_name");
	}

	public string GetManufacturer() {
		return ODeviced.read_string(this.node + "/manufacturer");
	}

	public string GetTechnology() {
		return ODeviced.read_string(this.node + "/technology");
	}

	public double GetEnergyPercentage() {
		var _curr = GetCurrentEnergy();
		return 100.0 * ((float)_curr / (float)this.max_energy);
	}

	private bool poll_energy() {
		var _curr = GetCurrentEnergy();
		if (_curr == -1) 
			return false;

		message("Current energy, %d", _curr);
		if(_curr < this.low_energy_threshold) {
			message("\tLow energy warning");
			this.low_battery(_curr);
		}
		return true;
	}

	private bool poll_status() {
		var stat = GetBatteryStatus();
		if (stat == null)
			return false;

		if(stat != this.curr_status) {
			message("\tStatus changed, %s", stat);
			this.battery_status_changed(stat);
			this.curr_status = stat;
		}
		return true;
	}

/*
 * Uncomment this in the generated file

G_MODULE_EXPORT gboolean power_init (ODevicedPlugin *plugin) {
	GType type;
	GList *list = NULL;
	Power *obj;
	type = power_get_type();
	list = odeviced_compute_objects (plugin, type);
	g_list_foreach(list, (GFunc)register_dbus, NULL);
	
	return TRUE;
}

*/

}


void register_dbus (Power obj) {
	GLib.message("Registering DBus object at %s", obj.dbus_path);
	ODeviced.connection.register_object(obj.dbus_path, obj);
}
