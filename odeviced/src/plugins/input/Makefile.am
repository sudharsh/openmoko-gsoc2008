AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(GMODULE_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-I$(top_srcdir)/src/daemon \
	-ggdb


pkgdir          = $(libdir)/odeviced/plugins
inputconfdir    = $(datadir)/odeviced/plugins
inputconf_DATA  = input.plugin

pkg_LTLIBRARIES   = input.la
input_la_SOURCES  = input.c

input_la_LIBADD = \
	$(GLIB_LIBS) \
	$(DBUS_LIBS) \
	$(GMODULE_LIBS)


input_la_LDFLAGS      = -module -avoid-version
input_la_DEPENDENCIES = $(top_builddir)/config.h
