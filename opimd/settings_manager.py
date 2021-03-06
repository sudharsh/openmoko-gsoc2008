#
#   Openmoko PIM Daemon
#   Settings storage
#
#   http://openmoko.org/
#   http://pyneo.org/
#
#   Copyright (C) 2008 by Soeren Apel (abraxa@dar-clan.de)
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#

"""pypimd Settings storage

The settings are accessed by using them as properties of the class instance:
settings["startup_cmd"] = "moo"
"""

DBUS_BUS_NAME_PYNEO = "org.pyneo.pim"
DBUS_PATH_BASE_PYNEO = "/org/pyneo/pim"
DIN_BASE_PYNEO = "org.pyneo.pim"

DBUS_BUS_NAME_FSO = "org.freesmartphone.PIM"
DBUS_PATH_BASE_FSO = "/org/freesmartphone/PIM"
DIN_BASE_FSO = "org.freesmartphone.PIM"

DBUS_PATH_BASE = None
DIN_BASE = None

# ENV_MODE is either "pyneo" or "FSO", depending on the runtime environment
ENV_MODE = None


_SETTINGS_DEFAULTS = {
	"contacts_default_backend": "CSV-Contacts",

	"messages_default_folder": "Unfiled",
	"messages_trash_folder": "Trash",
	"sim_messages_default_folder": "SMS"
}



#----------------------------------------------------------------------------#
class SettingsManager():
#----------------------------------------------------------------------------#
	_settings = {}
	_file_name = "settings.txt"
#----------------------------------------------------------------------------#

	def __init__(self):
		self.load_settings()


	def __del__(self):
		self.save_settings()


	def __getitem__(self, key):
		
		result = None
		
		try:
			result = self._settings[key]
		except KeyError:
			result = _SETTINGS_DEFAULTS[key]
		
		return result


	def __setitem__(self, key, value):
		self._settings[key] = value


	def load_defaults(self):
		self._settings = _SETTINGS_DEFAULTS


	def load_settings(self):
		"""Loads all entries from disk"""
		
		try:
			file = open(self._file_name, "r")
			
			for line in file:
				# One setting per line, key=value format
				if line.find('=') > 0:
					line = line.rstrip()
					(key, value) = line.split("=")
					self._settings[key] = value
			
			file.close()
			
		except IOError:
			self.load_defaults()


	def save_settings(self):
		"""Saves all entries to disk"""
		
		file = open(self._file_name, "w")
		
		for key in self._settings.keys():
			file.write("%s=%s\n" % (key, self._settings[key]))
		
		file.close()



###  Initialization  ###

settings = SettingsManager()
