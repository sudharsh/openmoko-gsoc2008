namespace IdleHelpers {
	[CCode (cname = "start_timers")]
	public void start_timers(GLib.IOChannel channel, IdleNotifier obj);
}
