[CCode (cprefix = "FSOD", lower_case_prefix = "fsod_")]
namespace FSOD {
	[CCode (cheader_filename = "fsod-python-plugin.h")]
	public class PythonPlugin : GLib.Object {
		public GLib.List<string> dbus_object_paths;
		public PythonPlugin(string module_name, FSOD.Service service);
		public bool call_factory();
	}	
	
	[CCode (cname = "fsod_init_python", cheader_filename = "fsod-python-plugin.h")]
	public bool init_python();
	
	[CCode (cnmae = "fsod_finalize_python", cheader_filename = "fsod-python-plugin.h")]
	public void finalize_python();
}
