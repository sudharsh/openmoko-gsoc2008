/* odeviced.vapi generated by valac, do not modify. */

[CCode (cprefix = "ODeviced", lower_case_cprefix = "odeviced_")]
namespace ODeviced {
	[CCode (cprefix = "ODEVICED_PLUGIN_ERROR_", cheader_filename = "src/daemon/plugin.h")]
	public errordomain PluginError {
		LOAD_ERROR,
	}
	[CCode (cheader_filename = "src/daemon/plugin.h")]
	public class Plugin : GLib.Object {
		public GLib.List<string> dbus_object_paths;
		public Plugin (string name, string path);
		public bool register () throws ODeviced.PluginError;
		public GLib.Module library { get; }
		public string dbus_iface { get; }
		public GLib.KeyFile conf { get; }
		public string path { get; construct; }
		public string name { get; construct; }
	}
	[CCode (cheader_filename = "src/daemon/service.h")]
	public class Service : GLib.Object {
		protected string dev_name;
		protected string conf_dir_plugins;
		public string[] ListObjectsByInterface (string iface);
		public void run ();
		public Service ();
	}
	[CCode (cheader_filename = "src/daemon/odeviced.h")]
	protected DBus.Connection connection;
	[CCode (cheader_filename = "src/daemon/helpers.h")]
	public static void register_dbus_object (ODeviced.Plugin plugin, GLib.Object interface_obj);
	[CCode (cheader_filename = "src/daemon/helpers.h")]
	public static string get_device ();
	[CCode (cheader_filename = "src/daemon/helpers.h")]
	public static GLib.List? compute_objects (ODeviced.Plugin plugin, GLib.Type klass);
	[CCode (cheader_filename = "src/daemon/helpers.h")]
	public static string compute_name (string path);
	[CCode (cheader_filename = "src/daemon/helpers.h")]
	public static string cleanup_dbus_path (string dbus_path);
	[CCode (cheader_filename = "src/daemon/helpers.h")]
	public static int read_integer (string node);
	[CCode (cheader_filename = "src/daemon/helpers.h")]
	public static string? read_string (string node);
	[CCode (cheader_filename = "src/daemon/helpers.h")]
	public static bool write_integer (string node, int val);
	[CCode (cheader_filename = "src/daemon/helpers.h")]
	public static bool write_string (string node, string data);
}
