namespace WifiHelpers {
	[CCode (cname = "wifi_get_status")]
	public bool get_status (string iface);
	[CCode (cname = "wifi_set_control")]
	public bool set_control (string iface, bool control);
}
