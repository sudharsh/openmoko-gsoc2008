namespace InputHelpers {
	[CCode (cname = "process_watch", cheader_filename = "input-helpers.h")]
	public void process_watch (GLib.IOChannel channel, Input self);
}

