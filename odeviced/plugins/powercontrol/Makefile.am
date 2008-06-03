INCLUDES        = 	 -I$(includedir) \
			-I../../src/

AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(GMODULE_CFLAGS) \
	-include $(CONFIG_HEADER)
	-ggdb



pkgdir                 = $(libdir)/odeviced/plugins


pkg_LTLIBRARIES        = powercontrol.la
powercontrol_la_SOURCES  = powercontrol.c

powercontrol_la_LIBADD = \
	$(GLIB_LIBS) \
	$(DBUS_LIBS) \
	$(GMODULE_LIBS)


powercontrol_la_LDFLAGS      = -module -avoid-version
powercontrol_la_DEPENDENCIES = $(top_builddir)/config.h
