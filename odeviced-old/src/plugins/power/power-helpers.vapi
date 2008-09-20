namespace PowerHelpers {
	[CCode (name = "get_netlink_fd", cheader_filename="power-helpers.h")]
	public int get_netlink_fd();

	[CCode (name = "start_watch", cheader_filename="power-helpers.h")]
	public void start_watch (GLib.IOChannel channel, Power self);
}