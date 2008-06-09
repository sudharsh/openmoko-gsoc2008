INCLUDES        = 	 -I$(includedir) \
			-I$(top_srcdir)/src/daemon

AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(GMODULE_CFLAGS) \
	-include $(CONFIG_HEADER)
	-ggdb



pkgdir                 = $(libdir)/odeviced/plugins
powerconfdir    = $(datadir)/odeviced/plugins
powerconf_DATA      = power.plugin

pkg_LTLIBRARIES        = power.la
power_la_SOURCES  = power.c

power_la_LIBADD = \
	$(GLIB_LIBS) \
	$(DBUS_LIBS) \
	$(GMODULE_LIBS)


power_la_LDFLAGS      = -module -avoid-version
power_la_DEPENDENCIES = $(top_builddir)/config.h
