# This is just a dummy python script
#!/usr/bin/env python
"""
The Time Deamon - Python Implementation

(C) 2008 Guillaume 'Charlie' Chereau
(C) 2008 Openmoko, Inc.
GPLv2 or later

Package: otimed
Module: otime
"""

__version__ = "0.1.0"

import time

# All the dbus modules
import dbus
import dbus.service
import dbus.mainloop.glib
import gobject

import fsod

from dbus.mainloop.glib import DBusGMainLoop
DBusGMainLoop(set_as_default =True)

# #import logging
# #logger = logging.getLogger('otimed')


#============================================================================#
class Time(dbus.service.Object):
#============================================================================#
    def __init__(self, bus):
        self.path = "/org/freesmartphone/Time"
        super(Time, self).__init__(bus, self.path)
        self.interface = "org.freesmartphone.Time"
        self.last_emitted = None
        gobject.timeout_add_seconds(1, self.time_changed)
        self.bus = bus

    @dbus.service.method("org.freesmartphone.Time", in_signature='i', out_signature='iiiiiiiii')
    def GetLocalTime(self, seconds = None):
        """seconds -> (year, mon, day, hour, min, sec, wday, yday, isdst)
           
           Convert seconds since Epoch to a time tuple expressing local time.   
           When `seconds` is not passed in, convert the current time instead.
        """ 
        #logger.debug("GetLocalTime")
        return time.localtime(seconds)
        
    @dbus.service.signal('org.freesmartphone.Time', signature='iiiiiiiii')
    def Minute(self, year, mon, day, hour, min, sec, wday, yday, isdst):
        """signal used to notify a minute change in the local time"""
        #logger.debug("Minute %d:%d", hour, min)
        
    def time_changed(self):
        local_time = time.localtime()
        if local_time[:5] != self.last_emitted:
            self.last_emitted = local_time[:5]
            self.Minute(*local_time)
        return True


def factory():
    print "The Hovercraft is full of eels"
    bus = dbus.SystemBus()
    bus.request_name("org.freesmartphone.otimed")
    time_obj = Time(bus)
    return [time_obj]


