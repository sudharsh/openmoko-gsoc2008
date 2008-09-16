/* 
 * accelerometer.vala
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
using AccelHelpers;
using Device;
using FSO;


[DBus (name = "org.freesmartphone.Device.Accelerometer") ]
public class Accelerometer: Object {

	private string[] dev_node;
	private string device = new string();
	private int[] sample_rates;
	private string sample_rate_node = new string();
	private List<IOChannel> channels = new List<IOChannel> ();
	   
	[DBus (visible=false)]
	public Device plugin {
		get;
		construct;
	}

	Accelerometer (Device plugin) {
		this.plugin = plugin;
	}


	construct {
		this.device = ODeviced.get_device();
		this.sample_rates = plugin.conf.get_integer_list (device, "allowed_sample_rates");
		this.sample_rate_node = plugin.conf.get_string (device, "sysfs_node");
		this.dev_node = plugin.conf.get_string_list (device, "dev_node");

		foreach (string node in this.dev_node) {
			IOChannel channel = new IOChannel.file ("/dev/input/" + node, "r");
			this.channels.append (channel);
		}
		
	}


	/* DBus methods */
	public int GetSampleRate() {
		return ODeviced.read_integer (this.sample_rate_node);
	}

	public void SetSamplerate(int rate) {
		/* FIXME: any better way of doing this? */
		foreach (int allowed in this.sample_rates) {
			if (rate == allowed) {
				ODeviced.write_integer (this.sample_rate_node, rate);
				return;
			}
		}
	}	

	
	public int[] Value() {
		int[] values = new int[3];
		var _temp = this.channels.nth_data(0);
		int fd = _temp.unix_get_fd();
		int i = 0;
		while (i<3) {
			int val = AccelHelpers.retrieve_xyz (fd);			
			if (val == -1) /* read() failed or EV_SYN */
				continue;
			values[i++] = val;
		}
		return values;
	}

}



namespace accelerometer {

	public static Accelerometer obj;

	public bool init (Device plugin) {		
		obj = new Accelerometer(plugin);
		if(obj == null)
			return false;

		ODeviced.register_dbus_object (plugin, obj);
		return true;
	}
}
