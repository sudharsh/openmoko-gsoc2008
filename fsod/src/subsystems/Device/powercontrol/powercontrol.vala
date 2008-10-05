/* 
 * wifi.vala
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
using DBus; 
using WifiHelpers;
using FSO;
using Device;

[DBus (name = "org.freesmartphone.Plugin.PowerControl")]
public abstract class GenericPowerControl: GLib.Object {
	
	[DBus (visible = false)]
	public string name = new string();

	[DBus (visible = false)]
	public string node = new string();

	[DBus (visible = false)]
	public string powernode = new string();

	[DBus (visible = false)]
	public string resetnode = new string();

	public virtual signal void power (string device, bool enable);
	 
	[DBus (visible = false)]
	public virtual bool get_power () {
		var val =  ODeviced.read_integer (this.powernode);
		if (val == 1)
			return true;
		return false;
	}
	
	[DBus (visible = false)]
	public virtual void set_power (bool enable) {
		if(enable)
			ODeviced.write_integer (this.powernode, 1);
		else
			ODeviced.write_integer (this.powernode, 0);
	}

	[DBus (visible = false)]
	public virtual void reset () {
		ODeviced.write_integer (this.resetnode, 1);
	}
		
	public string GetName () {
		return this.name;
	}

	public bool GetPower() {
		return this.get_power();
	}

	public void SetPower(bool enable) {
		if (enable!=this.get_power())
			this.set_power (enable);
	}

	public void Reset() {
		message ("Not implemented yet");
	}
		
}


public class Bluetooth: GenericPowerControl {
	
	private string _bluetooth_node = new string();

	private string _dbus_path = "/org/freesmartphone/Plugin/PowerControl/Bluetooth";
	public string dbus_path {
		get { return _dbus_path; }
	}

	public Plugin plugin {
		get;
		construct;
	}

	Bluetooth (Plugin plugin) {
		this.plugin = plugin;
	}

	construct {
		var device = ODeviced.get_device();
		this.name = "Bluetooth";
		this._bluetooth_node = plugin.conf.get_string (device, "bluetooth_node");
		this.powernode = "/sys/class/platform/" + this._bluetooth_node + "/power_on";
		this.resetnode = "/sys/class/platform/" + this._bluetooth_node + "/reset";
	}

}


public class GSM: GenericPowerControl {
	
	private string _gsm_node = new string();

	private string _dbus_path = "/org/freesmartphone/Plugin/PowerControl/GSM";
	public string dbus_path {
		get { return _dbus_path; }
	}

	public Plugin plugin {
		get;
		construct;
	}

	GSM (Plugin plugin) {
		this.plugin = plugin;
	}

	construct {
		var device = ODeviced.get_device();
		this.name = "GSM";
		this._gsm_node = plugin.conf.get_string (device, "gsm_node");
		this.powernode = "/sys/class/platform/" + this._gsm_node + "/power_on";
		this.resetnode = "/sys/class/platform/" + this._gsm_node + "/reset";
	}

}


public class UsbHost: GenericPowerControl {
	
	private string modenode = new string();
	private string _dbus_path = "/org/freesmartphone/Plugin/PowerControl/UsbHost";
	public string dbus_path {
		get { return _dbus_path; }
	}

	public Plugin plugin {
		get;
		construct;
	}

	UsbHost (Plugin plugin) {
		this.plugin = plugin;
	}

	construct {
		this.name = "UsbHost";
		this.powernode = "/sys/devices/platform/neo1973-pm-host.0/hostmode";
		this.modenode = "/sys/devices/platform/s3c2410-ohci/usb-mode";
		this.resetnode = "";
	}

	public override void set_power (bool power) {
		if (power)
			ODeviced.write_string (this.modenode, "host");
		else
			ODeviced.write_string (this.modenode, "device");
	}
				

}


public class GPS: GenericPowerControl {
	
	private string _gps_node = new string();

	private string _dbus_path = "/org/freesmartphone/Plugin/PowerControl/GPS";
	public string dbus_path {
		get { return _dbus_path; }
	}

	public Plugin plugin {
		get;
		construct;
	}

	GPS (Plugin plugin) {
		this.plugin = plugin;
	}

	construct {
		var device = ODeviced.get_device();
		this.name = "GPS";
		this._gps_node = plugin.conf.get_string (device, "gps_node");
		this.powernode = "/sys/class/platform/" + this._gps_node + "/pwron";
		this.resetnode = "";
	}

}



public class Wifi: GenericPowerControl {

	private string default_iface = new string();
	private string _dbus_path = "/org/freesmartphone/Plugin/PowerControl/WiFi";
		
   	public string dbus_path {
		get { return _dbus_path; }
	}
	
	public Plugin plugin {
		get;
		construct;
	}

	Wifi (Plugin plugin) {
		this.plugin = plugin;
	}

	construct {
		string device = ODeviced.get_device();
		this.default_iface = plugin.conf.get_string (device, "default_iface");
		this.name = "WiFi";
	}
		
	public override bool get_power() {
		return WifiHelpers.get_status (this.default_iface);
	}
	
	public override void set_power(bool enable) {
		WifiHelpers.set_control (this.default_iface, enable);
	}

}



namespace powercontrol {
	
	public static Wifi wifi_obj;
	public static Bluetooth blt_obj;
	public static GSM gsm_obj;
	public static GPS gps_obj;
	public static UsbHost usbhost_obj;
	
	public bool init (Plugin plugin) {

		bool success = true;
		string device = ODeviced.get_device();
		var enable = plugin.conf.get_string_list (device, "enable");
		
		/* FIXME: refactor this */
		foreach (string klass in enable) {
			switch (klass){

			case "WiFi":
				wifi_obj = new Wifi (plugin);
				if (wifi_obj == null)
					success = false;
				FSO.connection.register_object (wifi_obj.dbus_path, (GLib.Object)wifi_obj);
				plugin.dbus_object_paths.append (wifi_obj.dbus_path);
				print ("\tPowerControl: INFO: Registered %s\n", klass);
				break;

			case "Bluetooth":
				blt_obj = new Bluetooth (plugin);
				if (blt_obj == null)
					success = false;
				FSO.connection.register_object (blt_obj.dbus_path, (GLib.Object)blt_obj);
				plugin.dbus_object_paths.append (blt_obj.dbus_path);
				print ("\tPowerControl: INFO: Registered %s\n", klass);	
				break;

			case "GSM":
				gsm_obj = new GSM (plugin);
				if (gsm_obj == null)
					success = false;
				FSO.connection.register_object (gsm_obj.dbus_path, (GLib.Object)gsm_obj);
				plugin.dbus_object_paths.append (gsm_obj.dbus_path);
				print ("\tPowerControl: INFO: Registered %s\n", klass);	
				break;

			case "GPS":
				gps_obj = new GPS (plugin);
				if (gps_obj == null)
					success = false;
				FSO.connection.register_object (gps_obj.dbus_path, (GLib.Object)gps_obj);
				plugin.dbus_object_paths.append (gps_obj.dbus_path);
				print ("\tPowerControl: INFO: Registered %s\n", klass);	
				break;

			case "UsbHost":
				usbhost_obj = new UsbHost (plugin);
				if (usbhost_obj == null)
					success = false;
				FSO.connection.register_object (usbhost_obj.dbus_path, (GLib.Object)usbhost_obj);
				plugin.dbus_object_paths.append (usbhost_obj.dbus_path);
				print ("\tPowerControl: INFO: Registered %s\n", klass);	
				break;

			default:
				print ("\tNo definition for %s\n", klass);
				break;
			}
		}		
		return success;
	}


}
				

	
	

