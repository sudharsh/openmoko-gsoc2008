/* 
 * resource.vala
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

/* A Big FIXME: Make the DBus methods asynchronous */

[DBus (name = "org.freesmartphone.Resource")]
public abstract class FSOD.Resource : GLib.Object {

	private enum status {
		ENABLED,
		DISABLED,
		SUSPEND,
		RESUME
	}
	private int current_status;
	protected string resource_name{get;set;}

	[DBus (visible = false)]
	public abstract void _enable();

	[DBus (visible = false)]
	public abstract void _disable();

	[DBus (visible = false)]
	public abstract void _suspend();

	[DBus (visible = false)]
	public abstract void _resume();

	public Resource (string resource_name) {
		this.resource_name = resource_name;
	}

	public void Enable() {
		this.current_status = status.ENABLED;
		this._enable();
	}

	public void Disable() {
		this.current_status = status.DISABLED;
		this._disable();
	}

	public void Suspend() {
		this.current_status = status.SUSPEND;
		this._suspend();
	}

	public void Resume() {
		this.current_status = status.RESUME;
		this._resume();
	}
	
}
