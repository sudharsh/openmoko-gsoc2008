/* 
 * idlenotifier.vala
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

[DBus (name = "org.freesmartphone.Device.IdleNotifier") ]
public class IdleNotifier: Object {

	public signal void state (string curr_state);

	private virtual void emit_signal (string _state) {
		this.state (_state);
		this.current_state = _state;
	}
	private string current_state = "AWAKE";
	private string dev_node = "/dev/input";
	private string device = new string();
	   
	private HashTable<string, int> timeouts = new HashTable<string, int>((HashFunc)str_hash, (EqualFunc)str_equal);
	private uint tag;
	
	[DBus (visible=false)]
	public ODeviced.PluginManager plugin {
		get;
		construct;
	}


	IdleNotifier (ODeviced.PluginManager plugin) {
		this.plugin = plugin;
	}


	construct {
		this.device = ODeviced.get_device();
		this.timeouts.insert ("IDLE", plugin.conf.get_integer (this.device, "IDLE"));
		this.timeouts.insert ("IDLE_DIM", plugin.conf.get_integer (this.device, "IDLE_DIM"));
		this.timeouts.insert ("IDLE_PRELOCK", plugin.conf.get_integer (this.device, "IDLE_PRELOCK"));
		this.timeouts.insert ("SUSPEND", plugin.conf.get_integer (this.device, "SUSPEND"));
		this.timeouts.insert ("LOCK", plugin.conf.get_integer (this.device, "LOCK"));
		this.timeouts.insert ("AWAKE", plugin.conf.get_integer (this.device, "AWAKE"));
		this.timeouts.insert ("BUSY", plugin.conf.get_integer (this.device, "BUSY"));

		Dir dir = Dir.open(dev_node, 0);

		try {			
			var name = dir.read_name ();
			while (name!=null) {
				/* Wait till vala has support for "in" operator in if clauses
				 /dev/input/event2 and event3 are accelerometers in the FreeRunner */
				if (name.has_prefix ("event") && !(name[5] == '2' || name[5] == '3')) {
					var _temp = new IOChannel.file (dev_node+"/"+name, "r");
					_temp.add_watch (IOCondition.IN, (IOFunc) this.onActivity);
				}
				name = dir.read_name();
			}
		}
		catch (GLib.Error error) {
			message (error.message);
		}
	}


	private bool onActivity (IOChannel source, IOCondition condition) {
		message ("IdleNotifier, Activity");
		if (current_state != "BUSY") {
			if (tag > 0) /* add_timeout_seconds returns a uint on success */
				GLib.Source.remove (tag);
			this.onBusy();
		}
		return true;
	}

	private bool onBusy() {
		this.emit_signal ("BUSY");
		tag = Timeout.add_seconds (this.timeouts.lookup("IDLE"), this.onIdle);
		return false;
	}
		
	private bool onIdle() {
		this.emit_signal ("IDLE");
		tag = Timeout.add_seconds (this.timeouts.lookup("IDLE_DIM"), this.onIdleDim);
		return false;
	}

	private bool onIdleDim() {
		this.emit_signal ("IDLE_DIM");
		tag = Timeout.add_seconds (this.timeouts.lookup("IDLE_PRELOCK"), this.onIdlePrelock);
		return false;
	}

	private bool onIdlePrelock() {
		this.emit_signal ("IDLE_PRELOCK");
		tag = Timeout.add_seconds (this.timeouts.lookup("IDLE_LOCK"), this.onLock);
		return false;
	}

	private bool onLock() {
		this.emit_signal ("LOCK");
		tag = Timeout.add_seconds (this.timeouts.lookup("SUSPEND"), this.onSuspend);
		return false;
	}

	private bool onSuspend() {
		this.emit_signal ("SUSPEND");
		return false;
	}

	
	/* DBus methods */
	public string GetState () {
		return this.current_state;
	}

	public void SetState (string _state) {
		if (this.current_state == _state) {
			message ("Already in %s, ignoring..", _state);
			return;
		}
		
		switch (_state) {
		case "IDLE":
			onIdle();
			break;
		case "BUSY":
			onBusy();
			break;
		case "IDLE_DIM":
			onIdleDim();
			break;
		case "IDLE_PRELOCK":
			onIdlePrelock();
			break;
		case "LOCK":
			onLock();
			break;
		case "SUSPEND":
			onSuspend();
			break;
		default:
			message ("No such state exists");
			break;
		}
	}

}


namespace idlenotifier {

	public static IdleNotifier obj;

	public bool init (ODeviced.PluginManager plugin) {		
		obj = new IdleNotifier(plugin);
		if(obj == null)
			return false;

		ODeviced.register_dbus_object (plugin, obj);
		return true;
	}
}
