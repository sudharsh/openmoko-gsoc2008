#
#   Openmoko PIM Daemon
#   Backend Plugin Manager
#
#   http://openmoko.org/
#   http://neo1973-germany.de/
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

"""pypimd Backend Plugin Manager

Loads and manages the various PIM backend plugins (csv-contacts, mail-messages, etc.)
Exports a DomainManager singleton with the following class methods:
  - init ..................... Loads backend plugins and initializes them
  - register_backend ......... Takes a PIM backend class and registers it with
                               the given domain handler
"""

import os

from dbus import SystemBus
from dbus.service import FallbackObject as DBusFBObject
from dbus.service import signal as dbus_signal
from dbus.service import method as dbus_method

from syslog import syslog, LOG_INFO, LOG_DEBUG

from domain_manager import DomainManager
from settings_manager import *


PIMB_CAP_ADD_ENTRY = 'add_entry'
PIMB_CAP_DEL_ENTRY = 'del_entry'
PIMB_CAP_UPD_ENTRY = 'upd_entry'
PIMB_CAP_NEEDS_LOGIN = 'needs_login'

PIMB_STATUS_DISCONNECTED = 0
PIMB_STATUS_CONNECTED = 1


# D-Bus constant initialization, *must* be done before any D-Bus method decorators are declared
try:
	import framework.config
	
	_DBUS_PATH_BACKENDMNG = DBUS_PATH_BASE_FSO + '/Sources'
	_DIN_BACKENDMNG = DIN_BASE_FSO + '.Sources'
	ENV_MODE = 'FSO'
	
except ImportError:
	_DBUS_PATH_BACKENDMNG = DBUS_PATH_BASE_PYNEO + '/Sources'
	_DIN_BACKENDMNG = DIN_BASE_PYNEO + '.Sources'
	ENV_MODE = 'pyneo'


#----------------------------------------------------------------------------#
class BackendManager(DBusFBObject):
#----------------------------------------------------------------------------#
	_backends = []                  # List containing all backend objects
#----------------------------------------------------------------------------#

	def __init__(self, plugin_path):
		"""Initializes the backend manager
		
		@param plugin_path The directory where we'll look for backend plugins"""

		# Initialize the D-Bus-Interface
		DBusFBObject.__init__(
			self,
			conn=SystemBus(),
			object_path=_DBUS_PATH_BACKENDMNG
			)
		
		# Keep frameworkd happy
		self.interface = _DIN_BACKENDMNG
		self.path = _DBUS_PATH_BACKENDMNG
		
		# Load all backend plugins (pimb = PIM Backend)
		files = os.listdir(plugin_path)
		
		for plugin in filter(
			lambda s: (s[-3:] == '.py' and s[:5] == 'pimb_'),
				files):
				
				# Don't load unsuited modules
				if (ENV_MODE == 'pyneo' and 'FSO' in plugin): continue
				if (ENV_MODE == 'FSO' and 'pyneo' in plugin): continue
				
				syslog(LOG_DEBUG, "Loading " + plugin)
				
				(file_name, file_ext) = os.path.splitext(plugin)
				__import__(file_name, globals(), locals(), [])


	@classmethod
	def register_backend(class_, backend):
		"""Register a backend and register it with all supported PIM domains
		
		@param backend The backend object to register"""
		class_._backends.append(backend)
		syslog(LOG_INFO, "Registered backend " + backend.name)
		
		for domain in backend.get_supported_domains():
			domain_handler = DomainManager.get_domain_handler(domain)
			if domain_handler: domain_handler.register_backend(backend)


	@classmethod
	def get_default_backend(class_, domain):
		"""Returns the default backend for a specific domain
		
		@param domain The name of the domain to get the default backend for
		@return The backend to use"""
		try:
			key = domain.lower() + "_default_handler"
			backend_name = settings[key]
			backend = self._backends[backend_name]
		except KeyError:
			backend = None
			
		return backend


	@dbus_method(_DIN_BACKENDMNG, "", "i")
	def GetEntryCount(self):
		"""Return the number of backends we know of"""
		return len(self._backends)


	@dbus_method(_DIN_BACKENDMNG, "", "s", rel_path_keyword="rel_path")
	def GetName(self, rel_path):
		num_id = int(rel_path[1:])
		backend = None
		
		if (num_id < len(self._backends)): backend = self._backends[num_id]
		return backend.name if backend else ""


	@dbus_method(_DIN_BACKENDMNG, "", "as", rel_path_keyword="rel_path")
	def GetSupportedPIMDomains(self, rel_path):
		num_id = int(rel_path[1:])
		backend = None
		
		if (num_id < len(self._backends)): backend = self._backends[num_id]
		return backend.get_supported_domains() if backend else ()


	@dbus_method(_DIN_BACKENDMNG, "", "s", rel_path_keyword="rel_path")
	def GetStatus(self, rel_path):
		num_id = int(rel_path[1:])
		backend = None
		
		if (num_id < len(self._backends)): backend = self._backends[num_id]
		return backend.status if backend else ""



