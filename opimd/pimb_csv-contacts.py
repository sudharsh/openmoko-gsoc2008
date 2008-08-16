#
#   Openmoko PIM Daemon
#   CSV-Contacts Backend Plugin
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

"""pypimd CSV-Contacts Backend Plugin

Exports:
 - init ...................... initializes plugin and registers it
"""

from syslog import syslog, LOG_WARNING, LOG_INFO, LOG_DEBUG

from domain_manager import DomainManager
from backend_manager import BackendManager
from backend_manager import PIMB_CAN_ADD_ENTRY, PIMB_CAN_DEL_ENTRY, PIMB_CAN_UPD_ENTRY


_DOMAINS = ('Contacts', )
_CSV_FILE_NAME = 'csv-contacts.txt'



#----------------------------------------------------------------------------#
class CSVContactBackend(object):
#----------------------------------------------------------------------------#
	name = 'CSV-Contacts'
	properties = [PIMB_CAN_ADD_ENTRY, PIMB_CAN_DEL_ENTRY, PIMB_CAN_UPD_ENTRY]

	_domain_handlers = None           # Map of the domain handler objects we support
	_entry_ids = None                 # List of all entry IDs that have data from us
#----------------------------------------------------------------------------#

	def __init__(self):
		self._domain_handlers = {}
		self._entry_ids = []
		
		for domain in _DOMAINS:
		 self._domain_handlers[domain] = DomainManager.get_domain_handler(domain)
		self.load_entries_from_file()


	def __del__(self):
		self.save_entries_to_file()


	def get_supported_domains(self):
		"""Returns a list of PIM domains that this plugin supports"""
		
		return _DOMAINS


	def load_entries_from_file(self):
		"""Loads all entries from disk"""
		
		try:
			file = open(_CSV_FILE_NAME, 'r')
			
			for line in file:
				if line.find('=') == -1: continue
				
				# Break CSV line up into key/value pairs, then assign them to the new entry
				entry = {}
				pairs = line.split(',')
				for pair in pairs:
					(key, value) = pair.split('=')
					entry[key] = value
				
				entry_id = self._domain_handlers['Contacts'].register_contact(self, entry)
				self._entry_ids.append(entry_id)
				
		except IOError:
			syslog("Error opening " + _CSV_FILE_NAME)


	def save_entries_to_file(self):
		"""Saves all entries to disk"""
		
		file = open(_CSV_FILE_NAME, 'w')
		
		for entry in self._domain_handlers['Contacts'].enumerate_contacts(self):
			line = ""
			for field in entry:
				(field_name, field_data) = field
				line += field_name + '=' + field_data + ','
			file.write(line[:-1] + "\n")
		
		file.close()


	def add_contact(self, contact_data):
		contact_id = self._domain_handlers['Contacts'].register_contact(self, contact_data)
		
		# TODO Delayed writing to prevent performance issues when adding lots of contacts
		self.save_entries_to_file()
		
		return contact_id



###  Initalization  ###

BackendManager.register_backend(CSVContactBackend())
