INCLUDES        = 	 -I$(includedir) \
			-I../../src/

AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(GMODULE_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-ggdb

pkgdir                 = $(libdir)/odeviced/plugins


pkg_LTLIBRARIES    	= libwifi.la
libwifi_la_SOURCES	= wifi.c \
			wifi.h

libwifi_la_LIBADD = \
	$(GLIB_LIBS) \
	$(DBUS_LIBS) \
	$(GMODULE_LIBS)

libwifi_la_LDFLAGS      = -module -avoid-version
libwifi_la_DEPENDENCIES = $(top_builddir)/config.h
