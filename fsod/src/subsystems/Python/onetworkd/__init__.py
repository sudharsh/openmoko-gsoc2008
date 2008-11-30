import dbus

from dbus.mainloop.glib import DBusGMainLoop
DBusGMainLoop(set_as_default =True)

import sharing

#============================================================================#
def factory():
#============================================================================#
    """This is the magic function that will be called by the framework module manager"""
    print "Initialising onetworkd"
    bus = dbus.SystemBus()
    bus.request_name ("org.freesmartphone.onetworkd")
    return [ sharing.ConnectionSharing( bus ) ]
