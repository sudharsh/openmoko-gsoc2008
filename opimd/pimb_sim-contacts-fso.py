#
#   Openmoko PIM Daemon
#   SIM-Contacts Backend Plugin for FSO
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

"""pypimd SIM-Contacts Backend Plugin for FSO"""

from dbus import SystemBus
from dbus.proxies import Interface
from dbus.exceptions import DBusException
from gobject import timeout_add
from logging import getLogger as get_logger

from backend_manager import BackendManager
from backend_manager import PIMB_CAN_ADD_ENTRY, PIMB_CAN_DEL_ENTRY, PIMB_CAN_UPD_ENTRY
from domain_manager import DomainManager
from helpers import *


_DOMAINS = ('Contacts', )
_OGSMD_POLL_INTERVAL = 7500



#----------------------------------------------------------------------------#
class SIMContactBackendFSO(object):
#----------------------------------------------------------------------------#
	name = 'SIM-Contacts-FSO'
	properties = []

	# Dict containing the domain handler objects we support
	_domain_handlers = None
	
	# List of all entry IDs that have data from us
	_entry_ids = None
#----------------------------------------------------------------------------#

	def __init__(self):
		self._domain_handlers = {}
		self._entry_ids = []
		
		for domain in _DOMAINS:
		 self._domain_handlers[domain] = DomainManager.get_domain_handler(domain)

		self.load_entries()


	def get_supported_domains(self):
		"""Returns a list of PIM domains that this plugin supports"""
		return _DOMAINS


	def log_error(self, error):
		log = get_logger('opimd')
		log.error("%s hit an error and schedules retry. Reason: %s" % (self.name, error))
		timeout_add(_OGSMD_POLL_INTERVAL, self.load_entries)


	def process_entries(self, entries):
		for (sim_entry_id, name, number) in entries:
			
			if len(name) == 0: continue
			
			# Remove special characters that indicate groups
			
			# TODO Do this in a non-unicode-destructing manner
			name = name.encode('ascii', 'ignore')
#			name.translate({"\xbf":None, "$":None})
			
			entry = {}
			entry['Phone'] = phone_number_to_tel_uri(number)
			entry['Name'] = name
			
			entry_id = self._domain_handlers['Contacts'].register_contact(self, entry)
			self._entry_ids.append(entry_id)


	def load_entries(self):
		bus = SystemBus()
		
		try:
			gsm = bus.get_object('org.freesmartphone.ogsmd', '/org/freesmartphone/GSM/Device')
			gsm_sim_iface = Interface(gsm, 'org.freesmartphone.GSM.SIM')
			
			gsm_sim_iface.RetrievePhonebook(
				reply_handler=process_entries,
				error_handler=log_error)
				
		except DBusException, e:
			syslog(LOG_WARNING, "%s: Could not request SIM phonebook from ogsmd, scheduling retry (%s)" % (self.name, e))
			return True
		
		return False



###  Initalization  ###

BackendManager.register_backend(SIMContactBackendFSO())
