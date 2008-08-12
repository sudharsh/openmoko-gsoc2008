namespace InputHelpers {
	[CCode (cname = "process_event")]
	public bool process_event (int fd, ref ushort type, ref ushort code, ref int value);
}

