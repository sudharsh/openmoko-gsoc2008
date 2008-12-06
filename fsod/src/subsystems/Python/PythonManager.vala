/* 
 * pyfsod.vala
 * Written by Sudharshan "Sup3rkiddo" S <sudharsh@gmail.com>
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

/* Python subsystem for loading python modules */

using GLib;
using FSOD;
using Subsystem;

public class PythonManager : Subsystem.Manager {
	
	private string modules_path = Path.build_filename(Config.LIBDIR, "fsod/subsystems/Python");
	private List<FSOD.PythonPlugin> plugins = new List<FSOD.PythonPlugin>();

	public FSOD.Service service {
		get;
		construct;
	}

	public PythonManager (FSOD.Service service) {
		this.service = service;
	}

	construct {
		/* Initialize the python interpreter and such */
		if(FSOD.init_python()) {
			try {
				Dir dir = Dir.open(modules_path, 0);
				string module_name;
				while ((module_name = dir.read_name()) != null) {
					if (!module_name.has_suffix(".pyc")) {
						FSOD.PythonPlugin plugin = new FSOD.PythonPlugin(module_name.split(".")[0], service);
						if (plugin == null) {
							continue;
						}
						if (plugin.call_factory())
							plugins.append(plugin);
					}
					
				}
				
			}
			catch (GLib.Error error) {
				log ("PythonManager", LogLevelFlags.LEVEL_WARNING, error.message);
			}
		}
		else {
			log ("PythonManager", LogLevelFlags.LEVEL_WARNING, "Couldn't initialize python interpreter");
		}
						
	}

	~PythonManager() {
		FSOD.finalize_python();
	}
	
	public override string[] ListObjectsByInterface(string iface) {
		string[] ret = { "Not", "Implemented", "Yet" };
		return ret;
	}

}


public Subsystem.Manager InitPythonManager(FSOD.Service service) {
	PythonManager obj = new PythonManager(service);
	return obj;
}
