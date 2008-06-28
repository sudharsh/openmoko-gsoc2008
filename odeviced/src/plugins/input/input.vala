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

/*
#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
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

   	public signal void @event(string name, string action, int seconds);

	public static PollFD[] pollfd;

	private static bool prepare (Source source, ref int timeout) {
		return false;
	}
	
	private static bool check (Source source) {
		foreach (PollFD _fd in pollfd) {
			if ((_fd.revents & IOCondition.IN) != 0)
				return true;
		}
		return false;
	}
	
	private static bool dispatch (Source source, SourceFunc cback) {
		return true;
	}

	construct {

		
		KeyFile _conf = new KeyFile();
		Dir dir = Dir.open(dev_node, 0);

		SourceFuncs funcs = new SourceFuncs ();
		/* This is crappy, Must look for a way to do this in vala */
		/*
		  static GSourceFuncs funcs = {
		      input_prepare,
			  input_check,
			  input_dispatch,
			  NULL,
		  };

		  watcher = g_source_new (&funcs, ((guint) (sizeof (GSource))));
		*/   
		  
		Source watcher = new Source(funcs, (uint)sizeof(Source));
		try {
			
			_conf.load_from_file("/usr/share/odeviced/plugins/input.plugin", KeyFileFlags.NONE);
			this.device = ODeviced.get_device();
			this.auxbutton_keycode = _conf.get_integer (device, "auxbutton");
			this.powerbutton_keycode = _conf.get_integer (device, "powerbutton");
			this.touchscreen_keycode = _conf.get_integer (device, "touchscreen");
			dev_node = dir.read_name();
			int i = 0;
			while (dev_node!=null) {
				if (dev_node.has_prefix ("event")) {
					pollfd[i].fd = 0; /* input_pollfd[i].fd = open(g_strdup_printf("/dev/input/event%d", i), O_RDONLY); */
					pollfd[i].revents = 0;
					pollfd[i].events = IOCondition.IN | IOCondition.HUP | IOCondition.ERR;
					var _fd = pollfd[i];
					watcher.add_poll (ref _fd);
					i++;
				}
				dev_node = dir.read_name();
			}
						
		}
		catch (GLib.Error error) {
			message (error.message);
		}
	}

/*
G_MODULE_EXPORT gboolean input_init (ODevicedPlugin *plugin) {

	Input* inputobj;
	inputobj = input_new ();
	if(inputobj) 
		odeviced_register_dbus_object (plugin, G_OBJECT(inputobj));
	else
		return FALSE;
	
	return TRUE;
  
}
*/

}
