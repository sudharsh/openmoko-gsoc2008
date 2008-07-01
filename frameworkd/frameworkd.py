#!/usr/bin/env python
# Just a dummy frameworkd service to keep up with zhone

import dbus, dbus.service, dbus.glib, dbus.mainloop.glib
from gobject import MainLoop

dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)

BUS_NAME = "org.freesmartphone.frameworkd"
OBJ_PATH = "/org/freesmartphone/Framework"


class Objects (dbus.service.Object):

    def __init__(self):
        dbus.service.Object.__init__ (self, bus, OBJ_PATH)
        self.mainloop = MainLoop()
   
    @dbus.service.method (dbus_interface='org.freesmartphone.Objects',
                          in_signature = 's', out_signature = 'ao')
    def ListObjectsByInterface (self, iface):
        proxy = bus.get_object ("org.freesmartphone.odeviced", "/org/freesmartphone/Device")
        return proxy.ListObjectsByInterface (iface, dbus_interface="org.freesmartphone.Device")

    def run(self):
        self.mainloop.run()


if __name__ == "__main__":
    bus = dbus.SystemBus()
    bus.request_name (BUS_NAME)
    fwd = Objects()
    fwd.run()

