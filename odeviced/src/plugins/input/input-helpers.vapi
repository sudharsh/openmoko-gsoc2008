namespace InputHelpers {
	[CCode (cname = "process_event", cheader_filename = "input-helpers.h")]
	public bool process_event ();

	[CCode (cname = "on_activity", cheader_filename = "input-helpers.h")]
	public bool on_activity (GLib.IOChannel channel, GLib.IOCondition condition);
}

