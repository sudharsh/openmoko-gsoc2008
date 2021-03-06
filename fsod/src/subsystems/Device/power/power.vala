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


using DBus;
using GLib;
using ODeviced;
using PowerHelpers;
using FSOD;
using Device;

[DBus (name = "org.freesmartphone.Device.PowerSupply")]
public class Power: GLib.Object {

	private string status = new string();
	private int max_energy = 0;
	private int low_energy_threshold;
	private uint _id;
	private string name = new string();
	private int curr_capacity;
	private bool ispresent;

	public signal void power_status(string status);
	public signal void capacity(int charge);

	private void emit_signal(int current_capacity) {
		log ("Device.Power", LogLevelFlags.LEVEL_INFO, "Capacity changed");
		this.curr_capacity = current_capacity;
	}

	private string _curr_status = new string();
	[DBus (visible = false)]
	public string curr_status {
		get { return _curr_status; }
		set { _curr_status = value; }
	}

	[DBus (visible = false)]
	public string node {
		get;
		construct;
	}

	[DBus (visible = false)]
	public string dbus_path {
		get;
		construct;
	}

	[DBus (visible = false)]
	public Plugin plugin {
		get;
		construct;
	}


	Power(string node, string dbus_path, Plugin plugin) {
		this.node = node;
		this.dbus_path = dbus_path;
		this.plugin = plugin;
	}
	 

	construct {
		
		this._id = Timeout.add_seconds(30, poll_energy);
		try {
			var dev = ODeviced.get_device();
			this.name = ODeviced.compute_name (this.dbus_path);
			this._curr_status = GetPowerStatus();
			if (this._curr_status != null)
				PowerHelpers.start_watch (this);

			this.curr_capacity = GetCapacity();
			this.ispresent = FileUtils.test (this.node + "/present", FileTest.EXISTS);

			if (!FileUtils.test (this.node + "/capacity", FileTest.EXISTS))
				this.max_energy = ODeviced.read_integer (this.node + "/energy_full");
		}
		catch (GLib.KeyFileError error) {
			log ("Device.Power", LogLevelFlags.LEVEL_WARNING, error.message);
		}
	}


	public int GetCurrentEnergy() {
		return ODeviced.read_integer(this.node + "/energy_now");
	}


	public HashTable<string, string> GetInfo() {
		string _leaf;
		HashTable<string, string> info_table = new HashTable<string, string>((HashFunc)str_hash,
																			 (EqualFunc)str_equal);
		/* Just read all the files in the sysfs path and return it as a{ss} */
		try {
			Dir dir = Dir.open (this.node, 0);
			while ((_leaf = dir.read_name()) != null) {
				if(FileUtils.test (this.node + "/" + _leaf, FileTest.IS_REGULAR) && _leaf != "uevent") 
					info_table.insert (_leaf, ODeviced.read_string (this.node + "/" + _leaf).strip());
			}
		}
		catch (GLib.Error error) {
			log ("Device.Power", LogLevelFlags.LEVEL_WARNING, error.message);
		}
		return info_table;;
	}
				

	public string GetPowerStatus() {
		return ODeviced.read_string(this.node + "/status");
	}


	public string GetName() {
		return this.name;
	}


	public bool IsPresent() {
		return this.ispresent;
	}


	public int GetCapacity() {
		/* Some devices have capacity, just return that */
		var curr_capacity = ODeviced.read_integer (this.node + "/capacity");
		if(curr_capacity != -1)
			return curr_capacity;

		var _curr = GetCurrentEnergy();
		return (int)(100.0 * ((float)_curr / (float)this.max_energy));
	}


	private bool poll_energy() {
		var _curr = GetCapacity();
		if (_curr == -1 || ODeviced.read_string(this.node + "/type") == "Mains") { 
			Source.remove(_id);
			return false;
		}

		log ("Device.Power", LogLevelFlags.LEVEL_INFO, "Current capacity, %d", _curr);
		if(_curr != this.curr_capacity) {
			this.emit_signal(_curr);
		}
		return true;
	}

}


/* PluginManager init function */
namespace power {

	public static List<Power> list;

	public bool init (Plugin plugin) {
		Type type;
		list = new List<Power>();
		type = typeof (Power);
		list = ODeviced.compute_objects (plugin, type);
		if(list == null)
			return false;
		
		foreach (Power _obj in list) {
			plugin.connection.register_object (_obj.dbus_path, _obj);
		}
		return true;
	}
}
