namespace IdleHelpers {
	[CCode (cname = "on_activity", cheader_filename = "idlenotifier-helpers.h")]
	public bool on_activity(GLib.IOChannel channel, GLib.IOCondition condition);
}
