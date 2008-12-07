/* 
 * logger.vala
 * Written by Sudharshan "Sup3rkiddo" S <sudharsh@gmail.com>
 *            Frederik "playya" Sdun <Frederik.Sdun@googlemail.com>
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
using FSOD;


namespace FSOD {
	
	public class Logger : GLib.Object   {

		private FileStream log_stream;
		private LogLevelFlags levels;

		construct  {
			log_stream = FileStream.fdopen (2, "w");
			KeyFile conf_file = FSOD.get_fsod_conf();
						
			try {
				if (Options.log_domains == null)
					Options.log_domains = conf_file.get_string_list ("debug", "log_domains");				
				if (Options.log_file == null)
					Options.log_file = conf_file.get_string ("debug", "log_file");
				if (Options.log_levels == null)
					Options.log_levels = conf_file.get_string_list ("debug", "log_flags");
			}						
			catch (KeyFileError e) {
				warning(e.message);
			}

			if (Options.log_file == "-") {
				GLib.message ("Logging set to stdout\n");
				this.log_stream = FileStream.fdopen (1, "w");
			}
			else if (Options.log_file != null) {
				GLib.message ("Logging set to %s\n", Options.log_file);
				this.log_stream = GLib.FileStream.open (Options.log_file, "w+");
			}
			else {
				GLib.message ("Logging set to stderr\n");
				this.log_stream = FileStream.fdopen (2, "w");
			}

			if (Options.log_levels != null) {
				foreach (string _level in Options.log_levels) {
					switch(_level) {
					case "ERROR":
						this.levels |= LogLevelFlags.LEVEL_ERROR;
						break;
					case "CRITICAL":
						this.levels |= LogLevelFlags.LEVEL_CRITICAL;
						break;
					case "WARNING":
						this.levels |= LogLevelFlags.LEVEL_WARNING;
						break;
					case "MESSAGE":
						this.levels |= LogLevelFlags.LEVEL_MESSAGE;
						break;
					case "INFO":
						this.levels |= LogLevelFlags.LEVEL_INFO;
						break;
					case "DEBUG":
						this.levels |= LogLevelFlags.LEVEL_DEBUG;
						break;
					}

				}

			}
			
		}


		public void log (string? log_domain, LogLevelFlags flags,
						 string message) {
		
			bool log_this = false;

			/* If log_domain is empty log everything */
			if (strv_length(Options.log_domains) == 0) {
				log_this = true;
			}
			
			foreach (string key in Options.log_domains) {
				if (key == log_domain) {
					log_this = true;
					break;
				}
			}

			if (log_this && ((this.levels & flags) != 0)) {
				this.log_stream.printf ("%s: %s\n", log_domain, message);
				this.log_stream.flush();
			}

		}
	
	}

}
