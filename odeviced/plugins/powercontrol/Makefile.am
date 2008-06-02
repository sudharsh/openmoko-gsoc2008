INCLUDES        = 	 -I$(includedir) \
			-I../../src/

AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(GMODULE_CFLAGS) \
	-include $(CONFIG_HEADER)
	-ggdb



pkgdir                 = $(libdir)/odeviced/plugins


pkg_LTLIBRARIES        = libpowercontrol.la
libpowercontrol_la_SOURCES  = powercontrol.c

libpowercontrol_la_LIBADD = \
	$(GLIB_LIBS) \
	$(DBUS_LIBS) \
	$(GMODULE_LIBS)


libpowercontrol_la_LDFLAGS      = -module -avoid-version
libpowercontrol_la_DEPENDENCIES = $(top_builddir)/config.h
