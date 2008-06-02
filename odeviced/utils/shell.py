#!/usr/bin/env python

# Python shell for the odeviced dbus service

import dbus
import sys

DBUS_INTERFACE = "org.freesmartphone.Device"
DBUS_OBJECT_PATH = "/org/freesmartphone/Device"


def main():
    bus = dbus.SystemBus()
    proxy = bus.get_object(DBUS_INTERFACE, DBUS_OBJECT_PATH)
    while True:
        readval = raw_input("> ")
        if(readval.startswith("unload")):
            proxy.unload(readval.split()[1], dbus_interface=DBUS_INTERFACE)
            continue
        proxy.load(readval, dbus_interface=DBUS_INTERFACE)

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print "Exiting"
        sys.exit(0)
        
    

