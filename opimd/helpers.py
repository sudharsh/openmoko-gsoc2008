#
#   Openmoko PIM Daemon
#   Various globally used helpers
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

"""pypimd Helpers"""


def phone_number_to_tel_uri(phone_num):
	"""Transforms a regular phone number into a tel URI"""
	
	uri = "tel:"
	
	uri += phone_num
	return uri


def get_compare_for_tel(tel_value):
	"""Determines and returns a representation of a tel URI that is comparable to human input"""
	
	res = tel_value[4:]    # Remove tel:
	res = res.translate({ord(u'+'):None, ord(u'-'):None, ord(u'/'):None})   # Remove +, - and /
	
	return res


#----------------------------------------------------------------------------#
class PIMException(Exception):
#----------------------------------------------------------------------------#
	"""Base class for PIM-specific exceptions"""
	pass

#----------------------------------------------------------------------------#
class InvalidQueryIDError(PIMException):
#----------------------------------------------------------------------------#
	"""Raised when a submitted query ID is invalid / out of range"""
	pass

#----------------------------------------------------------------------------#
class InvalidBackendIDError(PIMException):
#----------------------------------------------------------------------------#
	"""Raised when a submitted backend ID is invalid / out of range"""
	pass

#----------------------------------------------------------------------------#
class InvalidBackendError(PIMException):
#----------------------------------------------------------------------------#
	"""Raised when a backend is either invalid or unsuited for a certain function call"""
	pass

