#!/usr/bin/env python
"""
Network

(C) 2008 Michael 'Mickey' Lauer <mlauer@vanille-media.de>
(C) 2008 Openmoko, Inc.
GPLv2 or later

Package: onetworkd
Module: dhcp

Support for the Dynamic Host Configuration Protocol
"""

MODULE_NAME = "onetworkd"
__version__ = "0.0.0"

from helpers import readFromFile, writeToFile

import subprocess

import logging
logger = logging.getLogger( MODULE_NAME )

ETC_RESOLV_CONF = "/etc/resolv.conf"

#============================================================================#
def launchDaemon():
#============================================================================#
    subprocess.call( "killall udhcpd".split() )
    subprocess.call( "udhcpd" )

#============================================================================#
def prepareDaemonConfigurationForInterface( iface ):
#============================================================================#
    name = iface.name()
    address = iface.ipAddress4()

    nameservers = ""
    resolv_conf = readFromFile( ETC_RESOLV_CONF ).split( '\n' )
    for line in resolv.conf:
        if line.startswith( "nameserver" ):
            nameservers += ( line.strip().split( ' ' ) )
            nameservers += " "

    conf_file = daemon_conf_file_template % ( name, nameservers, address )

    writeToFile( ETC_RESOLV_CONF, conf_file )


#============================================================================#
daemon_conf_file_name = "/etc/udhcpd.conf"
#============================================================================#

#============================================================================#
daemon_conf_file_template = """# freesmartphone.org /etc/udhcpd.conf
start           192.168.0.20  # lease range
end             192.168.0.199 # lease range
interface       %s            # listen on interface
option dns      %s            # grab from resolv.conf
option  subnet  255.255.255.0
opt     router  %s            # address of interface
option  lease   864000        # 10 days of seconds
"""
#============================================================================#
