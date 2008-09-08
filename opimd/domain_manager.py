#
#   Openmoko PIM Daemon
#   Domain Plugin Manager
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

"""pypimd Domain Plugin Manager"""

import os

from syslog import syslog, LOG_WARNING, LOG_INFO, LOG_DEBUG

from settings_manager import *



#----------------------------------------------------------------------------#
class DomainManager(object):
#----------------------------------------------------------------------------#
	_domains = {}        # List containing the domain objects
#----------------------------------------------------------------------------#

	@classmethod
	def init(class_, plugin_path):

		# Load all domain plugins (pimd = PIM Domain)
		try:
			files = os.listdir(plugin_path)
			
			for plugin in filter(
				lambda s: (s[-3:] == '.py' and s[:5] == 'pimd_'),
					files):
					
					# Don't load unsuited modules
					if (ENV_MODE == 'pyneo' and 'fso' in plugin): continue
					if (ENV_MODE == 'fso' and 'pyneo' in plugin): continue
					
					syslog(LOG_DEBUG, "Loading " + plugin)
					
					(file_name, file_ext) = os.path.splitext(plugin)
					__import__(file_name, globals(), locals(), [])
		
		except OSError:
			syslog(LOG_WARNING, "Could not open domain plugin directory: %s" % plugin_path)


	@classmethod
	def register_domain(class_, domain):
		class_._domains[domain.name] = domain
		syslog(LOG_INFO, "Registered domain " + domain.name)


	@classmethod
	def get_domain_handler(class_, domain):
		return class_._domains[domain] if (domain in class_._domains) else None


	@classmethod
	def enumerate_dbus_objects(class_):
		for handler in class_._domains.values():
			for obj in handler.get_dbus_objects():
				yield obj

