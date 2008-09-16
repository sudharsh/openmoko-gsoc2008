/* 
 * rtc.vala
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
using RTCHelpers;
using FSO;
using Device;

[DBus (name = "org.freesmartphone.Device.RealTimeClock")]
public class RealTimeClock: GLib.Object {
	
	private string name = new string();
	private string dev = new string();
	private KeyFile conf = new KeyFile();

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

	[DBus (visible=false)]
	public Device plugin {
		get;
		construct;
	}


	RealTimeClock (string node, string dbus_path, Device plugin) {
		this.node = node;
		this.dbus_path = dbus_path;
		this.plugin = plugin;
	}


	construct {
		this.name = ODeviced.compute_name (this.dbus_path);
	}


	public string GetName() {
		return this.name;
	}


	public string GetCurrentTime() {
		return ODeviced.read_string (this.node + "/since_epoch");
	}


	public string GetWakeupTime() {
		if (FileUtils.test (this.node + "/wakealarm", FileTest.EXISTS))
			return ODeviced.read_string (this.node + "/wakealarm");
		
		return RTCHelpers.get_wakeup_time ();
	}

	
	public void SetWakeupTime(string seconds) {
		if (FileUtils.test (this.node + "/wakealarm", FileTest.EXISTS))
			ODeviced.write_string (this.node + "/wakealarm", seconds);
		else
			RTCHelpers.set_wakeup_time(seconds);
	}

		
	public void SetCurrentTime(string seconds) {
		RTCHelpers.set_curr_time (seconds);	   
	}


	/* Move this to Power plugin? */
	public void Suspend() {
		ODeviced.write_string ("/sys/power/state", "mem");
	}

}


void register_dbus (RealTimeClock obj) {
	GLib.message("Registering DBus object at %s", obj.dbus_path);
	FSO.connection.register_object(obj.dbus_path, obj);
}
	
		
namespace rtc {

	public static List<RealTimeClock> list;

	public bool init (Device plugin) {
		Type type;
		list = new List<RealTimeClock>();
		type = typeof (RealTimeClock);
		list = ODeviced.compute_objects (plugin, type);
		if(list == null)
			return false;
		
		foreach (RealTimeClock _obj in list) {
			register_dbus (_obj);
		}
		return true;
	}
}
