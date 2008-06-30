namespace RTCHelpers {
	[CCode (cname = "rtc_get_wakeup")]
	public string get_wakeup_time();
	[CCode (cname = "rtc_set_currtime")]
	public void set_curr_time (string seconds);
	[CCode (cname = "rtc_disable_alarm")]
	public void disable_alarm ();
}
