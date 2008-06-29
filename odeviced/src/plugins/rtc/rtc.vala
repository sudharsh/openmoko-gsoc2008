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

/* RTC plugin for odeviced */

/*
#include <linux/rtc.h>
#include <sys/ioctl.h>
*/

using DBus;
using GLib;
using ODeviced;

[DBus (name = "org.freesmartphone.Device.RealTimeClock")]
public class RealTimeClock: GLib.Object {
	
	private string name = new string();
	private string dev = new string();
	private KeyFile conf = new KeyFile();

	public string node {
		get;
		construct;
	}

	public string dbus_path {
		get;
		construct;
	}

	RealTimeClock (string node, string dbus_path) {
		this.node = node;
		this.dbus_path = dbus_path;
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
		string ret = ODeviced.read_string (this.node + "/wakealarm");
		if(ret!=null)
			return ret;

		/*	

			int fd = 0;
			char *ret;
			struct rtc_wkalrm alarm;
			g_return_val_if_fail (IS_REAL_TIME_CLOCK (self), NULL);
			
			fd = open("/dev/rtc", O_RDONLY);
			if (fd == -1) {
			g_message("Couldn't open rtc device");
			return "0";
			}
			
			g_message ("bleh");
			if(ioctl(fd, RTC_WKALM_RD, &alarm) == 0)
			ret = g_strdup_printf("%d", alarm.time.tm_sec);
			fclose (fd);
			
			return ret;
		*/
		return ret;
	}

/*
G_MODULE_EXPORT gboolean rtc_init (ODevicedPlugin *plugin) {
	GType type;
	GList *list = NULL;
	RealTimeClock *obj;
	type = real_time_clock_get_type();
	list = odeviced_compute_objects (plugin, type);
	if(!list)
	return FALSE;
	g_list_foreach(list, (GFunc)register_dbus, NULL);
	
	return TRUE;
}
*/

}


void register_dbus (RealTimeClock obj) {
	GLib.message("Registering DBus object at %s", obj.dbus_path);
	ODeviced.connection.register_object(obj.dbus_path, obj);
}
	
		
