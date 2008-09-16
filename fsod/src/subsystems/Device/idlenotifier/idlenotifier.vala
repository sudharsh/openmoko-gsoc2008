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
using IdleHelpers;
using FSO;
using Device;


[DBus (name = "org.freesmartphone.Device.IdleNotifier") ]
public class IdleNotifier: GLib.Object {

	public signal void state (string curr_state);

	private void emit_signal (string _state) {
		this.state (_state);
		message ("Switching to %s state", _state);
		this._current_state = _state;
	}

	private string _current_state = "AWAKE";
	[DBus (visible = false)]
	public string current_state {
		get { return _current_state; }
  	}

	private string dev_node = "/dev/input";
	private string device = new string();
	private string[] watches;
	private PollFD[] fds;
	   
	private HashTable<string, int> timeouts = new HashTable<string, int>((HashFunc)str_hash, (EqualFunc)str_equal);

	[DBus (visible=false)]
	public uint tag;
		
	[DBus (visible=false)]
	public Device plugin {
		get;
		construct;
	}


	IdleNotifier (Device plugin) {
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

		this.watches  = plugin.conf.get_string_list (device, "watchfor");
		
		try {		
			foreach (string node in this.watches) {
				message (dev_node+"/"+node);
				IOChannel channel = new IOChannel.file ("/dev/input/event0", "r");
				/* See http://bugzilla.gnome.org/show_bug.cgi?id=546898 */
				channel.add_watch (IOCondition.IN, (IOFunc)IdleHelpers.on_activity);
			}
			tag = Timeout.add_seconds (2, this.onIdle);
		}
		catch (GLib.Error error) {
			print (">>>> %s\n", error.domain.to_string());
		}
	}

	
	private bool onBusy() {
		this.emit_signal ("BUSY");
		int secs = this.timeouts.lookup("IDLE");
		if (secs > 0)
			this.tag = Timeout.add_seconds (secs, this.onIdle);
		return false;
	}
		
	private bool onIdle() {
		this.emit_signal ("IDLE");
		int secs = this.timeouts.lookup("IDLE_DIM");
		if (secs > 0)
			this.tag = Timeout.add_seconds (secs, this.onIdleDim);
		return false;
	}

	private bool onIdleDim() {
		this.emit_signal ("IDLE_DIM");
		int secs = this.timeouts.lookup("IDLE_PRELOCK");
		if (secs > 0)
			this.tag = Timeout.add_seconds (secs, this.onIdlePrelock);
		return false;
	}

	private bool onIdlePrelock() {
		this.emit_signal ("IDLE_PRELOCK");
		int secs = this.timeouts.lookup("IDLE_LOCK");
		if (secs > 0)
			this.tag = Timeout.add_seconds (secs, this.onLock);
		return false;
	}

	private bool onLock() {
		this.emit_signal ("LOCK");
		int secs = this.timeouts.lookup("SUSPEND");
		if (secs > 0)
			this.tag = Timeout.add_seconds (secs, this.onSuspend);
		return false;
	}

	private bool onSuspend() {
		this.emit_signal ("SUSPEND");
		return false;
	}

	
	/* DBus methods */
	public string GetState () {
		return this._current_state;
	}

	public void SetState (string _state) {
		if (this._current_state == _state) 
			message ("Already in %s, ignoring..", _state);
		
		if (this.tag > 0)
			Source.remove(this.tag);
			
		switch (_state) {
		case "BUSY":
			onBusy();
			break;
		case "IDLE":
			onIdle();
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

	public bool init (Device plugin) {		
		obj = new IdleNotifier(plugin);
		if(obj == null)
			return false;

		ODeviced.register_dbus_object (plugin, obj);
		return true;
	}
}
