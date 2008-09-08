#
#   Openmoko PIM Daemon
#   SIM-Messages Backend Plugin for FSO
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

"""pypimd SIM-Messages Backend Plugin for FSO"""

from dbus import SystemBus
from dbus.proxies import Interface
from dbus.exceptions import DBusException
from gobject import timeout_add
from logging import getLogger as get_logger

from backend_manager import BackendManager
from backend_manager import PIMB_CAN_ADD_ENTRY, PIMB_CAN_DEL_ENTRY, PIMB_CAN_UPD_ENTRY
from domain_manager import DomainManager
from helpers import *


_DOMAINS = ('Messages', )
_OGSMD_POLL_INTERVAL = 7500



#----------------------------------------------------------------------------#
class SIMMessageBackendFSO(object):
#----------------------------------------------------------------------------#
	name = 'SIM-Messages-FSO'
	properties = []

	# Dict containing the domain handler objects we support
	_domain_handlers = None
	
	# List of all entry IDs that have data from us
	_entry_ids = None
	
	_gsm_sim_iface = None
#----------------------------------------------------------------------------#

	def __init__(self):
		self._domain_handlers = {}
		self._entry_ids = []
		
		for domain in _DOMAINS:
		 self._domain_handlers[domain] = DomainManager.get_domain_handler(domain)

		self.load_entries()
		self.install_signal_handlers()


	def get_supported_domains(self):
		"""Returns a list of PIM domains that this plugin supports"""
		return _DOMAINS


	def handle_sim_error(self, error):
		log = get_logger('opimd')
		log.error("%s hit an error, scheduling retry. Reason: %s" % (self.name, error))
		timeout_add(_OGSMD_POLL_INTERVAL, self.load_entries)


	def process_single_entry(self, status, number, text):
		entry = {}
		
		entry['Direction'] = 'in' if status in ('read', 'unread') else 'out'
		
		if status == 'read': entry['MessageRead'] = 1
		if status == 'sent': entry['MessageSent'] = 1
		
		if entry['Direction'] == 'in':
			entry['Sender'] = phone_number_to_tel_uri(number)
		else:
			entry['Recipient'] = phone_number_to_tel_uri(number)
		
		# TODO Handle text properly, i.e. make it on-demand if >1KiB
		entry['Text'] = text
		
		entry['Folder'] = settings['sim_messages_default_folder']
		
		entry_id = self._domain_handlers['Messages'].register_message(self, entry)
		self._entry_ids.append(entry_id)


	def process_all_entries(self, entries):
		for (sim_entry_id, status, number, text) in entries:
			if len(text) == 0: continue
			self.process_single_entry(status, number, text)


	def process_incoming_entry(self, entry):
		(number, text) = entry
		self.process_single_entry('unread', number, text)


	def load_entries(self):
		bus = SystemBus()
		
		try:
			gsm = bus.get_object('org.freesmartphone.ogsmd', '/org/freesmartphone/GSM/Device')
			self.gsm_sim_iface = Interface(gsm, 'org.freesmartphone.GSM.SIM')
			
			self.gsm_sim_iface.RetrieveMessagebook(
				'all',
				reply_handler=self.process_all_entries,
				error_handler=self.handle_sim_error)
				
		except DBusException, e:
			syslog(LOG_WARNING, "%s: Could not request SIM messagebook from ogsmd, scheduling retry (%s)" % (self.name, e))
			return True
		
		return False


	def handle_incoming_message(self, message_id):
		self.gsm_sim_iface.RetrieveMessage(
			message_id,
			reply_handler=self.process_incoming_entry
			# TODO We ignore errors for now
			)


	def install_signal_handlers(self):
		"""Hooks to some d-bus signals that are of interest to us"""
		
		bus = SystemBus()
		
		bus.add_signal_receiver(
			self.handle_incoming_message,
			'IncomingMessage',
			'org.freesmartphone.GSM.SIM'
			)



###  Initalization  ###

BackendManager.register_backend(SIMMessageBackendFSO())
