INCLUDES        = 	 -I$(includedir) \
			-I../../src/

AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(GMODULE_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-ggdb

pkgdir		= $(libdir)/odeviced/plugins
wificonfdir	= $(datadir)/odeviced/plugins
wificonf_DATA   = wifi.conf	

pkg_LTLIBRARIES	= wifi.la
wifi_la_SOURCES	= wifi.c \
		  wifi.h

wifi_la_LIBADD = \
	$(GLIB_LIBS) \
	$(DBUS_LIBS) \
	$(GMODULE_LIBS)

wifi_la_LDFLAGS      = -module -avoid-version
wifi_la_DEPENDENCIES = $(top_builddir)/config.h
