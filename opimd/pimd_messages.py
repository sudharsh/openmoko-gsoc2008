#
#   Openmoko PIM Daemon
#   Messages Domain Plugin
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

"""pypimd Messages Domain Plugin
Establishes the 'messages' PIM domain and handles all related requests"""

from dbus import SystemBus
from dbus.service import FallbackObject as DBusFBObject
from dbus.service import signal as dbus_signal
from dbus.service import method as dbus_method

from difflib import SequenceMatcher

from syslog import syslog, LOG_ERR, LOG_WARNING, LOG_INFO, LOG_DEBUG

from backend_manager import BackendManager
from backend_manager import PIMB_CAN_ADD_ENTRY

from domain_manager import DomainManager
from helpers import *
from settings_manager import *


_DOMAIN_NAME = "Messages"


# D-Bus constant initialization, *must* be done before any D-Bus method decorators are declared
try:
	import framework.config
	
	_DBUS_PATH_MESSAGES = DBUS_PATH_BASE_FSO + '/' + _DOMAIN_NAME
	_DIN_MESSAGES_BASE = DIN_BASE_FSO
	ENV_MODE = 'FSO'

except ImportError:
	_DBUS_PATH_MESSAGES = DBUS_PATH_BASE_PYNEO + '/' + _DOMAIN_NAME
	_DIN_MESSAGES_BASE = DIN_BASE_PYNEO
	ENV_MODE = 'pyneo'


_DBUS_PATH_QUERIES = _DBUS_PATH_MESSAGES + '/Queries'

_DIN_MESSAGES = _DIN_MESSAGES_BASE + '.' + 'Messages'
_DIN_ENTRY = _DIN_MESSAGES_BASE + '.' + 'Message'
_DIN_QUERY = _DIN_MESSAGES_BASE + '.' + 'MessageQuery'



#----------------------------------------------------------------------------#
class InvalidMessageIDError(PIMException):
#----------------------------------------------------------------------------#
	"""Raised when a submitted contact ID is invalid / out of range"""
	pass



#----------------------------------------------------------------------------#
class NoMoreMessagesError(PIMException):
#----------------------------------------------------------------------------#
	"""Raised when there are no more messages to be listed"""
	pass



#----------------------------------------------------------------------------#
class MessageQueryMatcher(object):
#----------------------------------------------------------------------------#
	query_obj = None
#----------------------------------------------------------------------------#

	def __init__(self, query):
		"""Evaluates a query
		
		@param query Query to evaluate, must be a dict"""
		
		self.query_obj = query


	def match(self, messages):
		"""Tries to match a given set of messages to the current query
		
		@param messages List of Message objects
		@return List of message IDs that match"""
		
		assert(self.query_obj, "Query object is empty, cannot match!")
		
		result_count = 0
		matches = {}
		results = []
		
		# Match all messages
		for (message_id, message) in enumerate(messages):
			match = message.match_query(self.query_obj)
			if match > _MIN_MATCH_TRESHOLD:
				matches[message_id] = match
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
			
			# Append the message IDs to the result list in the order of the sorted list
			for i in range(limit):
				results.append(kv_list[i][1])
		
		return results



#----------------------------------------------------------------------------#
class Message():
#----------------------------------------------------------------------------#
	"""Represents one single message with all the data fields it consists of.
	
	_fields[n] = [field_name, field_value, value_used_for_comparison, source]
	
	Best way to explain the usage of _fields and _field_idx is by example:
	_fields[3] = ["Recipient", "foo@bar.com", "", "EMail-Messages"]
	_fields[4] = ["Recipient", "moo@cow.com", "", "EMail-Messages"]
	_field_idx["Recipient"] = [3, 4]"""

	_fields = None
	_field_idx = None
	_used_backends = None
#----------------------------------------------------------------------------#

	def __init__(self, uri):
		"""Creates a new Message instance
		
		@param uri URI of the message itself"""
		
		self._fields = []
		self._field_idx = {}
		self._used_backends = []
		
		# Add URI field
		self._fields.append( ['URI', uri, '', ''] )
		self.rebuild_index()


	def __getitem__(self, field_name):
		"""Finds all field values for field_name
		
		@param field_name Name of the field whose data we return
		@return Value of the field if there's just one result, a list of values otherwise; None if field_name is unknown"""
		
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
		"""Rebuilds the field index, thereby ensuring consistency
		@note Should only be performed when absolutely necessary"""
		
		self._field_idx = {}
		for (field_idx, field) in enumerate(self._fields):
			(field_name, field_data, comp_value, field_source) = field
			
			try:
				self._field_idx[field_name].append(field_idx)
			except KeyError:
				self._field_idx[field_name] = [field_idx]


	def import_fields(self, message_data, backend_name):
		"""Adds an array of message data fields to this message
		
		@param message_data Message data; format: ((Key,Value), (Key,Value), ...)
		@param backend_name Name of the backend to which those fields belong"""
		
		# We add all fields as they come, not checking for duplicate data
		
		self._used_backends.append(backend_name)
		
		for field_name in message_data:
			field_value = message_data[field_name]
			
			# We only generate compare values for specific fields
			compare_value = ""
			
			# TODO Do this in a more extensible way
			# TODO Set contact ID as compare value for senders and recipients
#			if (field_name == "Sender") or (field_name == "Recipient"): compare_value = get_compare_for_contact(field_value)
			
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
		"""Extracts all fields belonging to a certain backend
		
		@param backend_name Name of the backend whose data we want to extract from this contract
		@return List of (field_name, field_data) tuples"""
		
		entry = []
		
		for field in self._fields:
			(field_name, field_data, comp_value, field_source) = field
			
			if field_source == backend_name:
				entry.append((field_name, field_data))
		
		return entry


	def get_fields(self, fields):
		"""Returns a dict containing the fields whose names are listed in the fields parameter
		@note Backend information is omitted.
		@note Fields that have more than one occurence are concatenated using a separation character of ','.
		
		@param fields List of field names to include in the resulting dict
		@return Dict containing the field_name/field_value pairs that were requested"""
		
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
		"""Creates and returns a complete representation of the message
		@note Backend information is omitted.
		@note Fields that have more than one occurence are concatenated using a separation character of ','.
		
		@return Message data, in {Field_name:Field_value} notation"""
		
		# TODO Do not return private fields, such as the internal ID
		return self.get_fields(self._field_idx)


	def incorporates_data_from(self, backend_name):
		"""Determines whether this message entry has data from a specific backend saved
		
		@param backend_name Name of backend to look for
		@return True if we have data belonging to that backend, False otherwise"""
		
		return backend_name in self._used_backends


	def match_query(self, query_obj):
		"""Checks whether this message matches the given query
		
		@param query_obj Dict containing key/value pairs of the required matches
		@return Accuracy of the match, ranging from 0.0 (no match) to 1.0 (complete match)"""
		
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
			
			# Check if field value(s) of this message match(es) the query field
			try:
				field_ids = self._field_idx[field_name]
				
				for field_id in field_ids:
					
					# A field is (Key,Value,Comp_Value,Source), so [2] is the value we usually use for comparison
					comp_value = self._fields[field_id][2]
					if not comp_value:
						comp_value = self._fields[field_id][1]   # We use the true value if no comparison value given
					
					# Compare and determine the best match ratio
					matcher.set_seq1(comp_value)
					match = matcher.find_longest_match(0, len(comp_value), 0, seq2_len)
					match_len = match[2]
					field_match = float(match_len) / seq2_len
					
					if field_match > best_field_match: best_field_match = field_match
					syslog(LOG_DEBUG, "Messages: Field match for %s / %s: %f" % (comp_value, field_value, field_match))
			
			except KeyError:
				# Message has no data for this field contained in the query, so this entry cannot match
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
	_messages = None
	query = None      # The query this handler is processing
	entries = None
	cursors = None    # The next entry we'll serve, depending on the client calling us
#----------------------------------------------------------------------------#

	def __init__(self, query, messages, dbus_sender):
		"""Creates a new SingleQueryHandler instance
		
		@param query Query to evaluate
		@param messages Set of Message objects to use
		@param dbus_sender Sender's unique name on the bus"""
		
		self.query = query
		self.sanitize_query()
		
		matcher = MessageQueryMatcher(self.query)
		
		self._messages = messages
		self.entries = matcher.match(self._messages)
		self.cursors = {}
		
		# TODO Register with all messages to receive updates


	def dispose(self):
		"""Unregisters from all message entries to allow this instance to be eaten by GC"""
		# TODO Unregister from all messages
		pass


	def sanitize_query(self):
		"""Makes sure the query meets the criteria that related code uses to omit wasteful sanity checks"""
		
		# For get_result_and_advance():
		# Make sure the _result_fields list has no whitespaces, e.g. "a, b, c" should be "a,b,c"
		# Reasoning: Message.get_fields() has no fuzzy matching for performance reasons
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
		"""Determines the number of results for this query
		
		@return Number of result entries"""
		
		return len(self.entries)


	def rewind(self, dbus_sender):
		"""Resets the cursor for a given d-bus sender to the first result entry
		
		@param dbus_sender Sender's unique name on the bus"""
		
		self.cursors[dbus_sender] = 0


	def skip(self, dbus_sender, num_entries):
		"""Skips n result entries of the result set
		
		@param dbus_sender Sender's unique name on the bus
		@param num_entries Number of result entries to skip"""
		
		if not self.cursors.has_key(dbus_sender): self.cursors[dbus_sender] = 0
		self.cursors[dbus_sender] += num_entries


	def get_message_uri(self, dbus_sender):
		"""Determines the URI of the next message that the cursor points at and advances to the next result entry
		
		@param dbus_sender Sender's unique name on the bus
		@return URI of the message"""
		
		# If the sender is not in the list of cursors it just means that it is starting to iterate
		if not self.cursors.has_key(dbus_sender): self.cursors[dbus_sender] = 0
		
		# Check whether we've reached the end of the entry list
		try:
			result = self.entries[self.cursors[dbus_sender]]
		except IndexError:
			raise NoMoreMessagesError()
		
		message_id = self.entries[self.cursors[dbus_sender]]
		message = self._messages[message_id]
		self.cursors[dbus_sender] += 1
		
		return message['URI']


	def get_result(self, dbus_sender):
		"""Extracts the requested fields from the next message entry in the result set and advances the cursor
		
		@param dbus_sender Sender's unique name on the bus
		@return Dict containing field_name/field_value pairs"""
		
		# If the sender is not in the list of cursors it just means that it is starting to iterate
		if not self.cursors.has_key(dbus_sender): self.cursors[dbus_sender] = 0
		
		# Check whether we've reached the end of the entry list
		try:
			result = self.entries[self.cursors[dbus_sender]]
		except IndexError:
			raise NoMoreMessagesError()
		
		message_id = self.entries[self.cursors[dbus_sender]]
		message = self._messages[message_id]
		self.cursors[dbus_sender] += 1
		
		try:
			fields = self.query['_result_fields']
			field_list = fields.split(',')
			result = message.get_fields(field_list)
		except KeyError:
			result = message.get_content()
		
		return result


	def get_multiple_results(self, dbus_sender, num_entries):
		"""Creates a list containing n dicts which represent the corresponding entries from the result set
		@note If there are less entries than num_entries, only the available entries will be returned
		
		@param dbus_sender Sender's unique name on the bus
		@param num_entries Number of result set entries to return
		@return List of dicts with field_name/field_value pairs"""
		
		result = {}
		
		for i in range(0, num_entries):
			try:
				entry = self.get_result(dbus_sender)
				result[i] = entry
			except NoMoreMessagesError:
				break
		
		return result


		def check_new_message(self, message_id):
			"""Checks whether a newly added message matches this so it can signal clients
			
			@param message_id Message ID of the message that was added
			@return True if message matches this query, False otherwise
			
			@todo Currently this messes up the order of the result set if a specific order was desired"""
			
			result = False
			
			matcher = MessageQueryMatcher(self.query)
			if matcher.single_message_matches():
				self.entries = matcher.match(self._messages)
				
				# TODO Register with the new message to receive changes
				
				# We *should* reset all cursors *if* the result set is ordered, however
				# in order to prevent confusion, this is left for the client to do.
				# Rationale: clients with unordered queries can just use get_result()
				# and be done with it. For those, theres's no need to re-read all results.
				
				# Let clients know that this result set changed
				result = True
			
			return result



#----------------------------------------------------------------------------#
class QueryManager(DBusFBObject):
#----------------------------------------------------------------------------#
	_queries = None
	_messages = None
	_next_query_id = None
#----------------------------------------------------------------------------#

	def __init__(self, messages):
		"""Creates a new QueryManager instance
		
		@param messages Set of Message objects to use"""
		
		self._messages = messages
		self._queries = {}    # Must be a dict so we can remove queries without messing up query IDs
		self._next_query_id = 0
		
		# Initialize the D-Bus-Interface
		DBusFBObject.__init__(
			self,
			conn=SystemBus(),
			object_path=_DBUS_PATH_QUERIES
			)
		
		# Keep frameworkd happy
		self.interface = _DIN_MESSAGES
		self.path = _DBUS_PATH_QUERIES


	def process_query(self, query, dbus_sender):
		"""Handles a query and returns the URI of the newly created query result
		
		@param query Query to evaluate
		@param dbus_sender Sender's unique name on the bus
		@return URI of the query result"""
		
		query_handler = SingleQueryHandler(query, self._messages, dbus_sender)
		
		query_id = self._next_query_id
		self._next_query_id += 1
		
		self._queries[query_id] = query_handler
		
		return 'dbus://' + _DBUS_PATH_QUERIES + '/' + str(query_id)


	def check_new_message(self, message_id):
		"""Checks whether a newly added message matches one or more queries so they can signal clients
		
		@param message_id Message ID of the message that was added"""
		
		for (query_id, query_handler) in self._queries.items():
			if query_handler.check_new_message(message_id):
				message = self._messages[message_id]
				message_uri = message['URI']
				# TODO Figure out how relative signals really work
				# self.MessageAdded(query_id, message_uri)


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
	def GetMessageURI(self, rel_path, sender):
		num_id = int(rel_path[1:])
		
		# Make sure the requested query exists
		if not self._queries.has_key(num_id): raise InvalidQueryIDError()
		
		return self._queries[num_id].get_message_uri(sender)


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
class MessageDomain(DBusFBObject):
#----------------------------------------------------------------------------#
	name = _DOMAIN_NAME

	_backends = None
	_messages = None
	query_manager = None
#----------------------------------------------------------------------------#

	def __init__(self):
		"""Creates a new MessageDomain instance"""
		
		self._backends = {}
		self._messages = []
		self.query_manager = QueryManager(self._messages)
		
		# Initialize the D-Bus-Interface
		DBusFBObject.__init__(
			self,
			conn=SystemBus(),
			object_path=_DBUS_PATH_MESSAGES
			)
		
		# Keep frameworkd happy, pyneo won't care
		self.interface = _DIN_MESSAGES
		self.path = _DBUS_PATH_MESSAGES


	def get_dbus_objects(self):
		"""Returns a list of all d-bus objects we manage
		
		@return List of d-bus objects"""
		
		return (self, self.query_manager)


	def register_backend(self, backend):
		"""Registers a backend for usage with this domain
		
		@param backend Backend plugin object to register"""
		
		self._backends[backend.name] = backend


	def register_message(self, backend, message_fields):
		"""Merges/inserts the given message into the message list and returns its ID
		
		@param backend Backend objects that requests the registration
		@param message_fields Message data; format: [Key:Value, Key:Value, ...]"""
		
		# Create a new message entry and append it to the list
		message_id = len(self._messages)
		
		uri = 'dbus://' + _DBUS_PATH_MESSAGES+ '/' + str(message_id)
		message = Message(uri)
		message.import_fields(message_fields, backend.name)
		
		self._messages.append(message)
		
		# Notify clients that a new message arrived
		self.NewMessage(uri)
		
		return message_id


	def enumerate_messages(self, backend):
		"""Enumerates all message data belonging to a specific backend
		
		@param backend Backend object whose messages should be enumerated
		@return Lists of (field_name,field_value) tuples of all messages that have data from this particular backend"""
		
		for message in self._messages:
			if message.incorporates_data_from(backend.name):
				yield message.export_fields(backend.name)


	@dbus_method(_DIN_MESSAGES, "a{sv}", "s")
	def Add(self, message_data):
		"""Adds a message to the list, assigning it to the default backend and saving it
		
		@param message_data List of fields; format is [Key:Value, Key:Value, ...]
		@return URI of the newly created d-bus message object"""
		
		# We use the default backend for now
		backend = BackendManager.get_default_backend(_DOMAIN_NAME)
		result = ""
		
		if not PIMB_CAN_ADD_ENTRY in backend.properties:
			raise InvalidBackendError()
		
		try:
			message_id = backend.add_message(message_data)
		except AttributeError:
			raise InvalidBackendError()
		
		message = self._messages[message_id]
		result = message['URI']
		
		# As we just added a new message, we check it against all queries to see if it matches
		self.query_manager.check_new_message(message_id)
		
		return result


	@dbus_method(_DIN_MESSAGES, "a{sv}s", "s")
	def GetSingleMessageSingleField(self, query, field_name):
		"""Returns the first message found for a query, making it real easy to query simple things
		
		@param query The query object
		@param field_name The name of the field to return
		@return The requested data"""
		
		result = ""
		
		# Only return one message
		query['_limit'] = 1
		matcher = MessageQueryMatcher(query)
		res = matcher.match(self._messages)
		
		# Copy all requested fields if we got a result
		if len(res) > 0:
			message = self._messages[res[0]]
			result = message[field_name]
			
			# Merge results if we received multiple results
			if isinstance(result, list):
				result = ",".join(map(str, result))
		
		return result


	@dbus_method(_DIN_MESSAGES, "a{sv}", "s", sender_keyword="sender")
	def Query(self, query, sender):
		"""Processes a query and returns the URI of the resulting query object
		
		@param query Query
		@param sender Unique name of the query sender on the bus
		@return URI of the query object, e.g. dbus://org.pyneo.PIM/Messages/Queries/4"""
		
		return self.query_manager.process_query(query, sender)


	@dbus_signal(_DIN_MESSAGES, "s")
	def NewMessage(self, message_URI):
		pass


	@dbus_method(_DIN_ENTRY, "", "a{sv}", rel_path_keyword="rel_path")
	def GetContent(self, rel_path):
		num_id = int(rel_path[1:])
		
		# Make sure the requested message exists
		if num_id >= len(self._messages): raise InvalidMessageIDError()
		
		return self._messages[num_id].get_content()


	@dbus_method(_DIN_ENTRY, "s", "a{sv}", rel_path_keyword="rel_path")
	def GetMultipleFields(self, field_list, rel_path):
		num_id = int(rel_path[1:])
		
		# Make sure the requested message exists
		if num_id >= len(self._messages): raise InvalidMessageIDError()
		
		# Break the string up into a list
		fields = field_list.split(',')
		new_field_list = []
			
		for field_name in fields:
			# Make sure the field list entries contain no spaces and aren't empty
			field_name = field_name.strip()
			if field_name: new_field_list.append(field_name)
			
		return self._messages[num_id].get_fields(new_field_list)



###  Initalization  ###

DomainManager.register_domain(MessageDomain())
