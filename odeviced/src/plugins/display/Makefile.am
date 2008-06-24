AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(GMODULE_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-I$(top_srcdir)/src/daemon

pkgdir		= $(libdir)/odeviced/plugins
displayconfdir	= $(datadir)/odeviced/plugins
displayconf_DATA   = display.plugin

pkg_LTLIBRARIES	= display.la
display_la_SOURCES	= display.c \
		  display.h

display_la_LIBADD = \
	$(GLIB_LIBS) \
	$(DBUS_LIBS) \
	$(GMODULE_LIBS)

display_la_LDFLAGS      = -module -avoid-version
display_la_DEPENDENCIES = $(top_builddir)/config.h
