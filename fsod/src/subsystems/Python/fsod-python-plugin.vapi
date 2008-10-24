[CCode (cprefix = "FSOD", lower_case_prefix = "fsod_")]
namespace FSOD {
	[CCode (cheader_filename = "fsod-python-plugin.h")]
	public class PythonPlugin : GLib.Object {
		public GLib.List<string> dbus_object_paths;
	}	
	
	[CCode (cname = "fsod_init_python", cheader_filename = "fsod-python-plugin.h")]
	public void init_python();
}
