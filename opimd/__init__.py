#!/usr/bin/env python2.5

#
#   Openmoko PIM Daemon
#   Main Program
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

"""PIM Daemon, main program"""

import os
import sys

from dbus import SystemBus
from dbus.mainloop.glib import DBusGMainLoop

from gobject import MainLoop

from syslog import openlog as log_open
from syslog import closelog as log_close
from syslog import LOG_ERR, LOG_WARNING, LOG_INFO, LOG_DEBUG, LOG_DAEMON, LOG_NDELAY, LOG_PID, LOG_PERROR

from settings_manager import *


###  Main Program  ###

def main_pyneo():
	"""This is the function that will be used to launch pypimd for pyneo"""
	log_open('pypimd', LOG_NDELAY|LOG_PID|LOG_PERROR, LOG_DAEMON)
	
	DBusGMainLoop(set_as_default=True)
	
	# Claim the bus name
	# TODO Check for exceptions
	SystemBus().request_name(DBUS_BUS_NAME_PYNEO)
	
	# Workaround for relative imports of pyneod stuff, see
	# http://mail.python.org/pipermail/python-list/2007-May/438250.html
	sys.path.append('/usr/share/pyneod')
	
	from backend_manager import BackendManager
	from domain_manager import DomainManager

	# Load plugins
	DomainManager.init(os.getcwdu())
	BackendManager(os.getcwdu())
	
	# 3-2-1-Go!
	main_loop = MainLoop()
	main_loop.run()
	
	log_close()
	return 0


def factory(prefix, controller):
	"""This is the function that FSO's frameworkd will call to start this subsystem"""
	from logging import getLogger as get_logger
	log = get_logger('opimd')
	
	# Claim the bus name
	# TODO Check for exceptions
	SystemBus().request_name(DBUS_BUS_NAME_FSO)
	
	from backend_manager import BackendManager
	from domain_manager import DomainManager
	
	# Load plugins
	DomainManager.init(os.getcwdu())
	backend_manager = BackendManager(os.getcwdu())
	
	dbus_objects = []
	
	# Create a list of all d-bus objects to make frameworkd happy
	for dbus_obj in DomainManager.enumerate_dbus_objects():
		dbus_objects.append(dbus_obj)

	dbus_objects.append(backend_manager)
	
	log.info('opimd subsystem loaded')
	
	return dbus_objects


if __name__ == '__main__':
	result = main_pyneo()
	sys.exit(result)
