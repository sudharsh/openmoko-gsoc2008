#!/usr/bin/env python
"""
Network

(C) 2008 Michael 'Mickey' Lauer <mlauer@vanille-media.de>
(C) 2008 Openmoko, Inc.
GPLv2 or later

Package: onetworkd
Module: network
"""

MODULE_NAME = "onetworkd"
__version__ = "0.0.0"

import gobject

import os
import socket
import fcntl
import struct

import logging
logger = logging.getLogger( MODULE_NAME )

#============================================================================#
class Network( dict ):
#============================================================================#
    def __init__( self ):
        gobject.idle_add( self._sync )

    def _sync( self ):
        # FIXME add listener so that this gets called whenever a change in
        # interfaces occurs
        interfaces = os.listdir( "/sys/class/net" )
        # first pass: remove
        for interface in self:
            if interface not in interfaces:
                logger.debug( "interface %s no longer present -- removing" % interface )
                del self[interface]
        # second pass: add
        for interface in os.listdir( "/sys/class/net" ):
            if interface not in self:
                logger.debug( "new interface %s -- adding" % interface )
                self[interface] = Interface( interface )
        return False

#============================================================================#
class Interface( object ):
#============================================================================#
    def __init__( self, name ):
        self._name = name

    def name( self ):
        return self.name

    def ipAddress4( self ):
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        return socket.inet_ntoa(fcntl.ioctl(
            s.fileno(),
            0x8915,  # SIOCGIFADDR
            struct.pack('256s', self._name[:15])
        )[20:24])

#============================================================================#
theNetwork = Network()
#============================================================================#

if __name__ == "__main__":
    pass
