#!/usr/bin/env python

# Python shell for the odeviced dbus service

import dbus
import sys

DBUS_INTERFACE_PREFIX = "org.freesmartphone.Device"
DBUS_OBJECT_PREFIX = "/org/freesmartphone/Device"

bus = dbus.SystemBus()

def main():
        
    while True:
        command = raw_input("> ")
        if command == '':
            continue

        argv = command.split()
        
        if argv[0] == '?':
            odeviced_shell_help()
        else:
            execute (argv)


def execute (argv):
    proxy = bus.get_object(DBUS_INTERFACE_PREFIX,
                           DBUS_OBJECT_PREFIX + "/Plugins/" + argv[0])
    _func = "proxy."+argv[1]+"("+argv[1]+", dbus_interface=\""+DBUS_INTERFACE_PREFIX+".Plugins."+argv[0]+"\")"
    exec(_func)
    
    



if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print "Exiting"
        sys.exit(0)
        
    

