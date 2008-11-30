#!/usr/bin/env python
"""
Network Connection Sharing

(C) 2008 Michael 'Mickey' Lauer <mlauer@vanille-media.de>
(C) 2008 Openmoko, Inc.
GPLv2 or later

Package: onetworkd
Module: sharing
"""

MODULE_NAME = "onetworkd"
__version__ = "0.0.0"

from network import theNetwork
from helpers import isValidAddress, writeToFile
import dhcp

import dbus
import dbus.service
import gobject

import os, subprocess

import logging
logger = logging.getLogger( MODULE_NAME )

DBUS_INTERFACE_NETWORK = "org.freesmartphone.Network"
DBUS_OBJECT_PATH = "/org/freesmartphone/Network"

#============================================================================#
class NoInterface( dbus.DBusException ):
#============================================================================#
    _dbus_error_name = "org.freesmartphone.Network.NoInterface"

#============================================================================#
class NoAddress( dbus.DBusException ):
#============================================================================#
    _dbus_error_name = "org.freesmartphone.Network.NoAddress"

#============================================================================#
class InternalError( dbus.DBusException ):
#============================================================================#
    _dbus_error_name = "org.freesmartphone.Network.InternalError"

#============================================================================#
class ConnectionSharing( dbus.service.Object ):
#============================================================================#
    def __init__( self, bus ):
        self.path = DBUS_OBJECT_PATH
        dbus.service.Object.__init__( self, bus, self.path )
        self.bus = bus

    #
    # dbus org.freesmartphone.Network
    #
    @dbus.service.method( DBUS_INTERFACE_NETWORK, "s", "",
                          async_callbacks=( "dbus_ok", "dbus_error" ) )
    def StartConnectionSharingWithInterface( self, interface, dbus_ok, dbus_error ):
        """

        This should be roughly equivalent to:

        #!/bin/sh
        iptables -I INPUT 1 -s 192.168.0.200 -j ACCEPT
        iptables -I OUTPUT 1 -s 192.168.0.202 -j ACCEPT
        iptables -A POSTROUTING -t nat -j MASQUERADE -s 192.168.0.0/24
        echo 1 > /proc/sys/net/ipv4/ip_forward

        <prepare udhcpd configuration>
        <launch udhcpd>
        """

        try:
            iface = theNetwork[ str(interface) ]
        except KeyError:
            dbus_error( NoInterface( "%s is not a valid interface. Known interfaces are %s" % ( interface, theNetwork.keys() ) ) )
            return

        source_address = iface.ipAddress4()

        if not isValidAddress( address ):
            dbus_error( NoAddress( "%s is not a valid IPv4 address." % address ) )
            return

        target_address = address

        # FIXME use dhcp daemon

        commands = []
        commands.append( "iptables -I INPUT 1 -s 192.168.0.0/24 -j ACCEPT" )
        commands.append( "iptables -I OUTPUT 1 -s %s -j ACCEPT" % source_address )
        commands.append( "iptables -A POSTROUTING -t nat -j MASQUERADE -s 192.168.0.0/24" )

        for command in commands:
            logging.debug( "issuing command '%s'" % command )
            result = subprocess.call( command.split( ' ' ) )
            logging.debug( "command result = %d" % result )
            if result != 0:
                dbus_error( InternalError( "%s gave returncode %d" % ( command, result ) ) )
                return
        writeToFile( "/proc/sys/net/ipv4/ip_forward", "1" )


        dhcp.prepareDaemonConfigurationForInterface( iface )
        dhcp.launchDaemon()

        dbus_ok()

    def StopConnectionSharing( self, interface, dbus_ok, dbus_error ):

        """
        stop dhcpd daemons
        clear iptables
        """

#============================================================================#
if __name__ == "__main__":
#============================================================================#
    pass

