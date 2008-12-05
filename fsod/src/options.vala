/* 
 * options.vala
 * Written by Cliff Brake <cliff.brake@gmail.com>
 *            Sudharshan "Sup3rkiddo" S <sudharsh@gmail.com>
 * All Rights Reserved
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */ 

using GLib;

public class FSOD.Options : Object
{
    public static bool run_from_build_dir = false;
    public static bool session = false;
    public static bool verbose = false;
    public static string log_file = null;
	
	[NoArrayLength]
	public static string[] log_domains;
	
	[NoArrayLength]
	public static string[] log_levels;
    
	private const OptionEntry[] entries = {
        { "run_from_build_dir", 'r', 0, OptionArg.NONE, ref run_from_build_dir, "Run from build directory, all transient files created there", null},
        { "session", 's', 0, OptionArg.NONE, ref session, "use dbus SESSION bus instead of SYSTEM bus (for development)", null},
        { "verbose", 'v', 0, OptionArg.NONE, ref verbose, "verbose", null},
		{ "log-file",'O', 0, OptionArg.FILENAME, ref log_file, "Write log to file", "Filename of the logfile (use '-' for stdout"},x
		{ "log-domains",'D', 0, OptionArg.STRING_ARRAY, ref log_domains, "Set domains for logging","Name of the Domains to log. Depends on loaded modules."},
		{ "log-levels", 'L', 0, OptionArg.STRING_ARRAY, ref log_levels, "Set levels for logging", "Possible values: ERROR,CRITICAL,WARNIN,INFO,DEBUG (default: all)"},
		{ null }
    };
	
	
    public static bool parse_args(ref weak string[] args) 
    {
        /* handle command line options */
        try {
            OptionContext options = new OptionContext ("");
			options.set_summary ("fsod FSO Framework Daemon");
			options.add_main_entries(entries, null);
            options.parse(ref args);
        } catch (OptionError err) {
            print ("Error while parsing commandline arguments: %s\n", err.message);
			return false;
        }
		
		return true;
    }
}

