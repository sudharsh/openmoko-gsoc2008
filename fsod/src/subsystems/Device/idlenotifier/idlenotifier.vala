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
using FSOD;
using Device;


errordomain InvalidState {
	INVALID_STATE;
}


[DBus (name = "org.freesmartphone.Device.IdleNotifier") ]
public class IdleNotifier: GLib.Object {

	public signal void state (string curr_state);

	private void emit_signal (string _state) {
		this.state (_state);
		log ("Device.IdleNotifier", LogLevelFlags.LEVEL_INFO, "Switching to %s state", _state);
		this._current_state = _state;
	}

	private string _current_state = "awake";
	[DBus (visible = false)]
	public string current_state {
		get { return _current_state; }
  	}

	private string device = new string();
	private string[] watches;
	private List<string> states = new List<string> ();
		   
	private HashTable<string, int> timeouts = new HashTable<string, int>((HashFunc)str_hash, (EqualFunc)str_equal);

	[DBus (visible=false)]
	public uint tag;
		
	[DBus (visible=false)]
	public Plugin plugin {
		get;
		construct;
	}


	IdleNotifier (Plugin plugin) {
		this.plugin = plugin;
	}


	construct {
		this.device = ODeviced.get_device();
		this.timeouts.insert ("idle", plugin.conf.get_integer (this.device, "idle"));
		this.timeouts.insert ("idle_dim", plugin.conf.get_integer (this.device, "idle_dim"));
		this.timeouts.insert ("idle_prelock", plugin.conf.get_integer (this.device, "idle_prelock"));
		this.timeouts.insert ("suspend", plugin.conf.get_integer (this.device, "suspend"));
		this.timeouts.insert ("lock", plugin.conf.get_integer (this.device, "lock"));
		this.timeouts.insert ("awake", plugin.conf.get_integer (this.device, "awake"));
		this.timeouts.insert ("busy", plugin.conf.get_integer (this.device, "busy"));
		this.states = this.timeouts.get_keys();
		try {
			this.watches  = plugin.conf.get_string_list (device, "watchfor");
			foreach (string node in this.watches) {
				var channel = new IOChannel.file ("/dev/input/" + node, "r");
				/* See http://bugzilla.gnome.org/show_bug.cgi?id=546898 */
				if(channel != null)
					channel.add_watch (IOCondition.IN, (IOFunc)IdleHelpers.on_activity);
			}
			tag = Timeout.add_seconds (2, this.onIdle);
		}
		catch (GLib.KeyFileError error) {
			log ("Device.IdleNotifier", LogLevelFlags.LEVEL_WARNING, error.message);
		}
			
	}

	
	private bool onBusy() {
		this.emit_signal ("busy");
		int secs = this.timeouts.lookup("idle");
		if (secs > 0)
			this.tag = Timeout.add_seconds (secs, this.onIdle);
		return false;
	}
		
	private bool onIdle() {
		this.emit_signal ("idle");
		int secs = this.timeouts.lookup("idle_dim");
		if (secs > 0)
			this.tag = Timeout.add_seconds (secs, this.onIdleDim);
		return false;
	}

	private bool onIdleDim() {
		this.emit_signal ("idle_dim");
		int secs = this.timeouts.lookup("idle_prelock");
		if (secs > 0)
			this.tag = Timeout.add_seconds (secs, this.onIdlePrelock);
		return false;
	}

	private bool onIdlePrelock() {
		this.emit_signal ("idle_prelock");
		int secs = this.timeouts.lookup("idle_lock");
		if (secs > 0)
			this.tag = Timeout.add_seconds (secs, this.onLock);
		return false;
	}

	private bool onLock() {
		this.emit_signal ("lock");
		int secs = this.timeouts.lookup("suspend");
		if (secs > 0)
			this.tag = Timeout.add_seconds (secs, this.onSuspend);
		return false;
	}

	private bool onSuspend() {
		this.emit_signal ("suspend");
		return false;
	}

	
	private bool lookup_state (string key) {
		foreach (string str in this.states) {
			if (key == str)
				return true;
		}
		return false;
	}

	
	/* DBus methods */
	public string GetState () {
		return this._current_state;
	}

	public void SetState (string _state) {
		if (this._current_state == _state) 
			log ("Device.IdleNotifier", LogLevelFlags.LEVEL_WARNING, "Already in %s state", _state);
					
		if (this.tag > 0)
			Source.remove(this.tag);
			
		switch (_state) {
		case "busy":
			onBusy();
			break;
		case "idle":
			onIdle();
			break;
		case "idle_dim":
			onIdleDim();
			break;
		case "idle_prelock":
			onIdlePrelock();
			break;
		case "lock":
			onLock();
			break;
		case "suspend":
			onSuspend();
			break;
		default:
			log ("Device.IdleNotifier", LogLevelFlags.LEVEL_WARNING, "No such state exists");
			throw new InvalidState.INVALID_STATE ("org.freesmartphone.IdleNotifier.InvalidState");
		}
	}

	public HashTable<string, int> GetTimeouts() {
		return this.timeouts;
	}

	public void SetTimeout(string _state, int timeout) throws InvalidState {
		
		if (!this.lookup_state(_state)) {
			log ("Device.IdleNotifier", LogLevelFlags.LEVEL_WARNING, "No such state exists");
			throw new InvalidState.INVALID_STATE ("org.freesmartphone.IdleNotifier.InvalidState");
		}
		
		this.timeouts.insert(_state, timeout);
		try {
			string _keyfile = "";
			size_t out_size = 0;
			plugin.conf.set_integer (this.device, _state, timeout);
			log ("Device.IdleNotifier", LogLevelFlags.LEVEL_INFO, "Timeout for %s set to %d successfully", 
				 _state, timeout);
			_keyfile = plugin.conf.to_data(out out_size);
			ODeviced.write_string (Path.build_filename(Config.DATADIR, "fsod/subsystems/Device/idlenotifier.plugin"), _keyfile);
		}

		catch (GLib.KeyFileError error) {
			log ("Device.IdleNotifier", LogLevelFlags.LEVEL_WARNING, error.message);
		}
	}

}


namespace idlenotifier {

	public static IdleNotifier obj;

	public bool init (Plugin plugin) {		
		obj = new IdleNotifier(plugin);
		if(obj == null)
			return false;

		plugin.register_dbus_object (obj);
		return true;
	}
}
