INCLUDES        = 	 -I$(includedir) \
			-I$(top_srcdir)/src

AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(GMODULE_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-ggdb

pkgdir		= $(libdir)/odeviced/plugins
backlightconfdir	= $(datadir)/odeviced/plugins
backlightconf_DATA   = backlight.plugin

pkg_LTLIBRARIES	= backlight.la
backlight_la_SOURCES	= backlight.c \
		  backlight.h

backlight_la_LIBADD = \
	$(GLIB_LIBS) \
	$(DBUS_LIBS) \
	$(GMODULE_LIBS)

backlight_la_LDFLAGS      = -module -avoid-version
backlight_la_DEPENDENCIES = $(top_builddir)/config.h
