#
#   Openmoko PIM Daemon
#   Contacts Domain Plugin
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

"""pypimd Contacts Domain Plugin

Establishes the contact PIM domain and handles all related requests
Exports:
 - register_backend .......... Registers a backend we can use
"""

from dbus import SystemBus
from dbus.service import FallbackObject as DBusFBObject
from dbus.service import signal as dbus_signal
from dbus.service import method as dbus_method

from difflib import SequenceMatcher

from shlex import shlex

from syslog import syslog, LOG_ERR, LOG_WARNING, LOG_INFO, LOG_DEBUG

from domain_manager import DomainManager
from helpers import *
from settings_manager import *


_DOMAIN_NAME = "Contacts"

_MIN_MATCH_TRESHOLD = 0.75

_QUERY_TOKENS = [
	"FIND", "FROM", "WHERE", "AND", "LIMIT"
]


# D-Bus constant initialization, *must* be done before any D-Bus method decorators are declared
try:
	import framework.config
	
	_DBUS_PATH_CONTACTS = DBUS_PATH_BASE_FSO + '/' + _DOMAIN_NAME
	_DIN_CONTACTS_BASE = DIN_BASE_FSO
	ENV_MODE = 'FSO'

except ImportError:
	_DBUS_PATH_CONTACTS = DBUS_PATH_BASE_PYNEO + '/' + _DOMAIN_NAME
	_DIN_CONTACTS_BASE = DIN_BASE_PYNEO
	ENV_MODE = 'pyneo'


_DBUS_PATH_QUERIES = _DBUS_PATH_CONTACTS + '/Queries'

_DIN_CONTACTS = _DIN_CONTACTS_BASE + '.' + 'Contacts'
_DIN_ENTRY = _DIN_CONTACTS_BASE + '.' + 'Contact'
_DIN_QUERY = _DIN_CONTACTS_BASE + '.' + 'ContactQuery'


#----------------------------------------------------------------------------#
class ContactQueryMatcher(object):
#----------------------------------------------------------------------------#
	query_obj = None
#----------------------------------------------------------------------------#

	def __init__(self, query):
		"""Evaluate a query
		
		@param query The query to evaluate, can be either a string or a dict query"""
		if isinstance(query, str) or isinstance(query, unicode):
			self.query_obj = self.query_to_object_query(query)
		else:
			self.query_obj = query


	def match(self, contacts):
		"""Tries to match a given set of contacts to the current query
		
		@param contacts A list of ContactEntry objects
		@return List of contact IDs that match"""
		assert(self.query_obj, "Query object is empty, cannot match!")
		
		result_count = 0
		matches = {}
		results = []
		
		# Match all contacts
		for (contact_id, contact) in enumerate(contacts):
			match = contact.match_query(self.query_obj)
			if match > _MIN_MATCH_TRESHOLD:
				matches[contact_id] = match
				result_count += 1
		
		# Sort matches by relevance and return the best hits
		if result_count > 0:
			# Transform the dict into a list of key/value tuples so we can sort it
			kv_list = [(v, k) for k, v in matches.items()]
			kv_list.sort()
			
			try:
				limit = self.query_obj["_limit"]
			except KeyError:
				limit = len(kv_list)
			
			# Append the contact IDs to the result list in the order of the sorted list
			for i in range(limit):
				results.append(kv_list[i][1])
		
		return results


	def query_to_object_query(self, query):
		query_obj = {}
		
		handlers = {
			"FIND": self.parse_FIND_to_object,
			"FROM": self.parse_FROM_to_object,
			"WHERE": self.parse_WHERE_to_object
		}
		
		#TODO shlex is not unicode compatible yet
		tokenizer = shlex(str(query), posix=True)
		tokenizer.whitespace_split = True
		tokenizer.quotes = True
		
		while True:
			try:
				token = tokenizer.get_token()
				if token == tokenizer.eof: break
				handler = handlers[token]
			
			except KeyError:
				msg = "Error parsing query at %s: %s" % (token, query)
				syslog(LOG_ERR, msg)
				return None
		
		return query_obj


	def parse_FIND_to_object(self, tokenizer, query_obj):
		"""Parses the FIND query compound and converts it into an object query
		
		@param tokenizer Tokenizer to get the next token(s) from
		@param query_obj The query object we can modify"""
		return


	def parse_FROM_to_object(self, tokenizer, query_obj):
		"""Parses the FIND query compound and converts it into an object query
		
		@param tokenizer Tokenizer to get the next token(s) from
		@param query_obj The query object we can modify"""
		return


	def parse_WHERE_to_object(self, tokenizer, query_obj):
		"""Parses the FIND query compound and converts it into an object query
		
		@param tokenizer Tokenizer to get the next token(s) from
		@param query_obj The query object we can modify"""
		return



#----------------------------------------------------------------------------#
class ContactEntry():
#----------------------------------------------------------------------------#
	"""Represents one single contact with all the data fields it consists of.
	
	_fields[n] = [field_name, field_value, value_used_for_comparison, source]
	
	Best way to explain the usage of _fields and _field_idx is by example:
	_fields[3] = ["EMail", "foo@bar.com", "", "CSV-Contacts"]
	_fields[4] = ["EMail", "moo@cow.com", "", "LDAP-Contacts"]
	_field_idx["EMail"] = [3, 4]"""

	_fields = None
	_field_idx = None
	_used_backends = None
#----------------------------------------------------------------------------#

	def __init__(self, uri):
		self._fields = []
		self._field_idx = {}
		self._used_backends = []
		
		# Add URI field
		self._fields.append( ['URI', uri, ''] )
		self.rebuild_index()


	def __getitem__(self, field_name):
		"""Finds all field values for field_name
		
		@param field_name The name of the field whose data we return
		@return The field value if there's just one result, a list of values otherwise; None if field_name is unknown"""
		try:
			field_ids = self._field_idx[field_name]
		except KeyError:
			return None
		
		if len(field_ids) == 1:
			# Return single result
			field = self._fields[field_ids[0]]
			return field[1]
			
		else:
			# Return multiple results
			result = []
			for n in field_ids:
				field = self._fields[n]
				result.append(field[1])
			
			return result


	def __repr__(self):
		return str(get_content())


	def rebuild_index(self):
		"""Rebuilds the field index, thereby ensuring consistency"""
		self._field_idx = {}
		for (field_idx, field) in enumerate(self._fields):
			(field_name, field_data, field_source) = field
			
			try:
				self._field_idx[field_name].append(field_idx)
			except KeyError:
				self._field_idx[field_name] = [field_idx]


	def import_fields(self, contact_fields, backend_name):
		"""Adds an array of contact data fields to this contact
		
		@param contact_fields Contact data; format: ((Key,Val), (Key,Val), ...)
		@param backend_name Name of the backend to which those fields belong"""
		
		# We add all fields as they come, not checking for duplicate data
		self._used_backends.append(backend_name)
		
		for field in contact_fields:
			(field_name, field_value) = field
			
			# We only generate compare values for specific fields
			compare_value = ""
			
			# TODO Do this in a more extensible way
			if ("phone" in field_name) or (field_name == "Phone"): compare_value = get_compare_for_tel(field_value)
			
			our_field = [field_name, field_value, compare_value, backend_name]
			
			self._fields.append(our_field)
			field_idx = len(self._fields) - 1
			
			# Keep the index happy, too
			if not field_name in self._field_idx.keys(): self._field_idx[field_name] = []
			self._field_idx[field_name].append(field_idx)
			
#		for (field_idx, field) in enumerate(self._fields):
#			print "%s: %s" % (field_idx, field)
#		print self._field_idx


	def export_fields(self, backend_name):
		entry = []
		for field in self._fields:
			(field_name, field_data, field_source) = field
			if field_source == backend_name:
				entry.append((field_name, field_data))
		return entry


	def get_fields(self, fields):
		"""Returns a dict containing the fields whose names are listed in the fields parameter
		Backend information is omitted.
		Fields that have more than one occurence are concatenated using a separation character of ','.
		
		@param fields List of field names to include in the resulting dict
		@return Dict containing the field_name:field_value pairs that were requested"""
		result = {}
		separator = ','
		
		for field_name in fields:
			field_ids = self._field_idx[field_name]
			
			# Do we need to concatenate multiple values?
			if len(field_ids) > 1:
				
				field_values = []
				for field_id in field_ids:
					field_value = (self._fields[field_id])[1]
					field_values.append(field_value)
				
				value = ','.join(field_values)
				result[field_name] = field_values
				
			else:
				field_value = (self._fields[field_ids[0]])[1]
				result[field_name] = field_value
		
		return result


	def get_content(self):
		"""Creates and returns a complete representation of the contact
		Backend information is omitted.
		Fields that have more than one occurence are concatenated using a separation character of ','.
		
		@return Contact data, in {Field_name:Field_value} notation"""
		return self.get_fields(self._field_idx)


	def attempt_merge(self, contact_fields, backend_name):
		"""Attempts to merge the given contact into the contact list and returns its ID
		
		@param contact_fields Contact data; format: ((Key,Val), (Key,Val), ...)
		@param backend_name The backend that owns the contact data
		@return True on successful merge, False otherwise"""
		
		# Don't merge if we already have data from $backend_name as one backend can't contain two mergeable contacts
		return False


	def incorporates_data_from(self, backend_name):
		"""Determines whether this contact entry has data from a specific backend saved
		
		@param backend_name The backend to look for
		@return True if we have data belonging to that backend, False otherwise"""
		return backend_name in self._used_backends


	def match_query(self, query_obj):
		"""Checks whether this contact matches the given query
		
		@param query_obj A dict containing key/value pairs of the required matches
		@return The accuracy of the match, ranging from 0.0 (no match) to 1.0 (complete match)"""
		overall_match = 1.0
		matcher = SequenceMatcher()
		
		for field_name in query_obj.keys():
			# Skip fields only meaningful to the parser
			if field_name[:1] == "_": continue
			
			field_value = query_obj[field_name]
			best_field_match = 0.0
			
			# The matcher internally caches details about seq2, so let's make use of that
			matcher.set_seq2(field_value)
			seq2_len = len(field_value)
			
			# Check if field value(s) of this contact match(es) the query field
			try:
				field_ids = self._field_idx[field_name]
				
				for field_id in field_ids:
					
					# A field is (Key,Value,Comp_Val,Source), so [2] is the value we usually use for comparison
					comp_val = self._fields[field_id][2]
					if not comp_val:
						comp_val = self._fields[field_id][1]   # We use the true value if no comparison value given
					
					# Compare and determine the best match ratio
					matcher.set_seq1(comp_val)
					match = matcher.find_longest_match(0, len(comp_val), 0, seq2_len)
					match_len = match[2]
					field_match = float(match_len) / seq2_len
					
					if field_match > best_field_match: best_field_match = field_match
					syslog(LOG_DEBUG, "Contacts: Field match for %s / %s: %f" % (comp_val, field_value, field_match))
			
			except KeyError:
				# Contact has no data for this field contained in the query, so this entry cannot match
				return 0.0
			
			# Aggregate the field match value into the overall match
			# We don't use the average of all field matches as one
			# non-match *must* result in a final value of 0.0
			overall_match *= best_field_match
			
			# Stop comparing if there is too little similarity
			if overall_match < _MIN_MATCH_TRESHOLD: break
		
		return overall_match



#----------------------------------------------------------------------------#
class SingleQueryHandler(object):
#----------------------------------------------------------------------------#
	_contacts = None
	query = None      # The query this handler is processing
	entries = None
	cursors = None    # The next entry we'll serve, depending on the client calling us
#----------------------------------------------------------------------------#

	def __init__(self, query, contacts, dbus_sender):
		"""Instantiate a new query handler
		
		@param query The query to evaluate
		@param contacts The set of contacts we can use
		@param dbus_sender The unique name of the query sender on the bus"""
		matcher = ContactQueryMatcher(query)
		
		self._contacts = contacts
		self.query = query
		self.entries = matcher.match(self._contacts)
		self.cursors = {}
		
		self.sanitize_query()
		
		# TODO Register with all contacts to receive updates


	def dispose(self):
		"""Unregister from all contact entries to allow this instance to be eaten by GC"""
		# TODO Unregister from all contacts
		pass


	def sanitize_query(self):
		"""Make sure the query meets the criteria that related code uses to omit wasteful sanity checks"""
		
		# For get_result_and_advance():
		# Make sure the _result_fields list has no whitespaces, e.g. "a, b, c" should be "a,b,c"
		# Reasoning: Contact.get_fields() has no fuzzy matching for performance reasons
		# Also, we remove any empty list elements created by e.g. "a, b, c,"
		try:
			field_list = self.query['_result_fields']
			fields = field_list.split(',')
			new_field_list = []
			
			for field_name in fields:
				field_name = field_name.strip()
				if field_name: new_field_list.append(field_name)
			
			self.query['_result_fields'] = ','.join(new_field_list)
		except KeyError:
			# There's no _result_fields entry to sanitize
			pass


	def get_result_count(self):
		return len(self.entries)


	def rewind(self, dbus_sender):
		self.cursors[dbus_sender] = 0


	def skip(self, dbus_sender, num_entries):
		if not self.cursors.has_key(dbus_sender): self.cursors[dbus_sender] = 0
		self.cursors[dbus_sender] += num_entries


	def get_contact_uri(self, dbus_sender):
		# If the sender is not in the list of cursors it just means that it is starting to iterate
		if not self.cursors.has_key(dbus_sender): self.cursors[dbus_sender] = 0
		
		# Check whether we've reached the end of the entry list
		try:
			result = self.entries[self.cursors[dbus_sender]]
		except IndexError:
			raise NoMoreContactsError()
		
		contact_id = self.entries[self.cursors[dbus_sender]]
		contact = self._contacts[contact_id]
		self.cursors[dbus_sender] += 1
		
		return contact['URI']


	def get_result(self, dbus_sender):
		# If the sender is not in the list of cursors it just means that it is starting to iterate
		if not self.cursors.has_key(dbus_sender): self.cursors[dbus_sender] = 0
		
		# Check whether we've reached the end of the entry list
		try:
			result = self.entries[self.cursors[dbus_sender]]
		except IndexError:
			raise NoMoreContactsError()
		
		contact_id = self.entries[self.cursors[dbus_sender]]
		contact = self._contacts[contact_id]
		self.cursors[dbus_sender] += 1
		
		try:
			fields = self.query['_result_fields']
			field_list = fields.split(',')
			result = contact.get_fields(field_list)
		except KeyError:
			result = contact.get_content()
		
		return result


	def get_multiple_results(self, dbus_sender, num_entries):
		result = {}
		
		for i in range(0, num_entries):
			try:
				entry = self.get_result(dbus_sender)
				result[i] = entry
			except NoMoreContactsError:
				break
		
		return result


#----------------------------------------------------------------------------#
class QueryManager(DBusFBObject):
#----------------------------------------------------------------------------#
	_queries = None
	_contacts = None
	_next_query_id = None
#----------------------------------------------------------------------------#

	def __init__(self, contacts):
		self._contacts = contacts
		self._queries = {}
		self._next_query_id = 0
		
		# Initialize the D-Bus-Interface
		DBusFBObject.__init__(
			self,
			conn=SystemBus(),
			object_path=_DBUS_PATH_QUERIES
			)
		
		# Keep frameworkd happy
		self.interface = _DIN_CONTACTS
		self.path = _DBUS_PATH_QUERIES


	def process_query(self, query, dbus_sender):
		"""Handles a query and returns the URI of the newly created query result
		
		@param query The query to evaluate
		@param dbus_sender The unique name of the query sender on the bus
		@return The URI of the query result"""
		query_handler = SingleQueryHandler(query, self._contacts, dbus_sender)
		
		query_id = self._next_query_id
		self._next_query_id += 1
		
		self._queries[query_id] = query_handler
		
		return 'dbus://' + _DBUS_PATH_QUERIES + '/' + str(query_id)


	@dbus_method(_DIN_QUERY, "", "i", rel_path_keyword="rel_path")
	def GetResultCount(self, rel_path):
		num_id = int(rel_path[1:])
		
		# Make sure the requested query exists
		if not self._queries.has_key(num_id): raise InvalidQueryIDError()
		
		return self._queries[num_id].get_result_count()


	@dbus_method(_DIN_QUERY, "", "", rel_path_keyword="rel_path", sender_keyword="sender")
	def Rewind(self, rel_path, sender):
		num_id = int(rel_path[1:])
		
		# Make sure the requested query exists
		if not self._queries.has_key(num_id): raise InvalidQueryIDError()
		
		self._queries[num_id].rewind(sender)


	@dbus_method(_DIN_QUERY, "i", "", rel_path_keyword="rel_path", sender_keyword="sender")
	def Skip(self, num_entries, rel_path, sender):
		num_id = int(rel_path[1:])
		
		# Make sure the requested query exists
		if not self._queries.has_key(num_id): raise InvalidQueryIDError()
		
		self._queries[num_id].skip(sender, num_entries)


	@dbus_method(_DIN_QUERY, "", "s", rel_path_keyword="rel_path", sender_keyword="sender")
	def GetContactURI(self, rel_path, sender):
		num_id = int(rel_path[1:])
		
		# Make sure the requested query exists
		if not self._queries.has_key(num_id): raise InvalidQueryIDError()
		
		return self._queries[num_id].get_contact_uri(sender)


	@dbus_method(_DIN_QUERY, "", "a{sv}", rel_path_keyword="rel_path", sender_keyword="sender")
	def GetResult(self, rel_path, sender):
		num_id = int(rel_path[1:])
		
		# Make sure the requested query exists
		if not self._queries.has_key(num_id): raise InvalidQueryIDError()
		
		return self._queries[num_id].get_result(sender)


	@dbus_method(_DIN_QUERY, "i", "a{ia{sv}}", rel_path_keyword="rel_path", sender_keyword="sender")
	def GetMultipleResults(self, num_entries, rel_path, sender):
		num_id = int(rel_path[1:])
		
		# Make sure the requested query exists
		if not self._queries.has_key(num_id): raise InvalidQueryIDError()
		
		return self._queries[num_id].get_multiple_results(sender, num_entries)


	@dbus_method(_DIN_QUERY, "", "", rel_path_keyword="rel_path")
	def Dispose(self, rel_path):
		num_id = int(rel_path[1:])
		
		# Make sure the requested query exists
		if not self._queries.has_key(num_id): raise InvalidQueryIDError()
		
		# Make sure no one else references the query handler before we remove our reference to it
		# Otherwise, garbage collection won't actually free its memory
		self._queries[num_id].dispose()
		self._queries.__delitem__(num_id)



#----------------------------------------------------------------------------#
class ContactDomain(DBusFBObject):
#----------------------------------------------------------------------------#
	name = _DOMAIN_NAME

	_backends = None
	_contacts = None
	query_manager = None
#----------------------------------------------------------------------------#

	def __init__(self):
		self._backends = {}
		self._contacts = []
		self.query_manager = QueryManager(self._contacts)
		
		# Initialize the D-Bus-Interface
		DBusFBObject.__init__(
			self,
			conn=SystemBus(),
			object_path=_DBUS_PATH_CONTACTS
			)
		
		# Keep frameworkd happy
		self.interface = _DIN_CONTACTS
		self.path = _DBUS_PATH_CONTACTS


	def get_dbus_objects(self):
		"""Returns a list of all d-bus objects we manage
		
		@return A list of d-bus objects"""
		return (self, self.query_manager)


	def register_backend(self, backend):
		"""Registers a backend for usage with this domain
		
		@param backend Backend plugin object to register"""
		self._backends[backend.name] = backend


	def register_contact(self, backend, contact_fields):
		"""Merges/inserts the given contact into the contact list and returns its ID
		
		@param backend The backend that requests the registration
		@param contact_fields Contact data; format: ((Key,Val), (Key,Val), ...)"""
		contact_id = -1
		merge_success = False

		# Check if the contact can be merged with one we already know of
		for entry in self._contacts:
			if entry.attempt_merge(contact_fields, backend.name):
				
				# Find that entry's ID
				for (contact_idx, contact) in enumerate(self._contacts):
					if contact == entry: contact_id = contact_idx
					break
				
				# Stop trying to merge
				break
		else:
			# Merging failed, so create a new contact entry and append it to the list
			contact_id = len(self._contacts)
			
			uri = 'dbus://' + _DBUS_PATH_CONTACTS+ '/' + str(contact_id)
			contact = ContactEntry(uri)
			contact.import_fields(contact_fields, backend.name)
			
			self._contacts.append(contact)
		
		return contact_id


	def enumerate_contacts(self, backend):
		"""Enumerates all contact data belonging to a specific backend
		
		@param backend The backend whose contacts to enumerate"""
		for contact in self._contacts:
			if contact.incorporates_data_from(backend.name):
				yield contact.export_fields(backend.name)


	@dbus_method(_DIN_CONTACTS, "a{sv}", "s")
	def Add(self, contact_info):
		"""Adds a contact to the list, assigning it to the default backend and saving it
		
		@param contact_info A list of fields; format either [(Key,Val), (Key,Val), ...] or [Key: Val, Key: Val, ...]
		@return The ID of the newly created contact"""
		raise NotImplementedError()


	@dbus_method(_DIN_CONTACTS, "a{sv}s", "s")
	def GetSingleContactSingleField(self, query, field_name):
		"""Returns the first contact found for a query, making it real easy to query simple things
		
		@param query The query object
		@param field_name The name of the field to return
		@return The requested data"""
		result = ""
		
		query['_limit'] = 1
		matcher = ContactQueryMatcher(query)
		res = matcher.match(self._contacts)
		
		# Copy all requested fields if we got a result
		if len(res) > 0:
			contact = self._contacts[res[0]]
			result = contact[field_name]
			
			# Merge results if we received multiple results
			if isinstance(result, list):
				result = ",".join(map(str, result))
		
		return result


	@dbus_method(_DIN_CONTACTS, "a{sv}", "s", sender_keyword="sender")
	def Query(self, query, sender):
		"""Processes a query and returns the URI of the resulting query object
		
		@param query The query in the form of a dict
		@param sender The unique name of the query sender on the bus
		@return The URI of the query object, e.g. dbus://org.pyneo.PIM/Contacts/Queries/4"""
		return self.query_manager.process_query(query, sender)


	@dbus_method(_DIN_ENTRY, "", "a{sv}", rel_path_keyword="rel_path")
	def GetContent(self, rel_path):
		num_id = int(rel_path[1:])
		
		# Make sure the requested contact exists
		if num_id >= len(self._contacts): raise InvalidContactIDError()
		
		return self._contacts[num_id].get_content()


	@dbus_method(_DIN_ENTRY, "s", "a{sv}", rel_path_keyword="rel_path")
	def GetMultipleFields(self, field_list, rel_path):
		num_id = int(rel_path[1:])
		
		# Make sure the requested contact exists
		if num_id >= len(self._contacts): raise InvalidContactIDError()
		
		# Break the string up into a list
		fields = field_list.split(',')
		new_field_list = []
			
		for field_name in fields:
			# Make sure the field list entries contain no spaces and aren't empty
			field_name = field_name.strip()
			if field_name: new_field_list.append(field_name)
			
		return self._contacts[num_id].get_fields(new_field_list)



###  Initalization  ###

DomainManager.register_domain(ContactDomain())
