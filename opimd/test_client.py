#!/usr/bin/env python2.5

#
#   Openmoko PIM Daemon
#   Test Client
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

from dbus.mainloop.glib import DBusGMainLoop
DBusGMainLoop(set_as_default=True)


#----------------------------------------------------------------------------#
# Test domain manager
#----------------------------------------------------------------------------#
from domain_manager import DomainManager

class DummyDomain(object):
	name = 'DummyDomain'
	def get_dbus_objects(self): yield self
	def register_backend(self, backend): pass

dd = DummyDomain()

# Try adding a dummy domain and make sure it gets listed
DomainManager.init('')
DomainManager.register_domain(dd)

if DomainManager.get_domain_handler('DummyDomain') == dd:
	print "Domain registration OK"
else:
	print "Domain registration FAILED"

# Make sure the D-Bus object listing works, too
obj_ok = False
for obj in DomainManager.enumerate_dbus_objects():
	if obj == dd: obj_ok = True

if obj_ok:
	print "D-Bus domain object listing OK"
else:
	print "D-Bus domain object listing FAILED"


#----------------------------------------------------------------------------#
# Test backend manager
#----------------------------------------------------------------------------#
from backend_manager import BackendManager

class DummyBackend(object):
	name = 'DummyBackend'
	status = 'Happy'
	def get_supported_domains(self): return ('DummyDomain')

db = DummyBackend()

# Try adding a dummy backend and make sure it gets listed
bm = BackendManager('')
BackendManager.register_backend(db)

if bm.GetName('/0') == db.name:
	print "Backend registration OK"
else:
	print "Backend registration FAILED"

# See if number of backends is correct
if bm.GetEntryCount() == 1:
	print "Backend count OK"
else:
	print "Backend count FAILED"

if bm.GetSupportedPIMDomains('/0') == db.get_supported_domains():
	print "Backend domain support check OK"
else:
	print "Backend domain support check FAILED"

if bm.GetStatus('/0') == db.status:
	print "Backend status check OK"
else:
	print "Backend status check FAILED"

