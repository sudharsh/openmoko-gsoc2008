AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(GMODULE_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-I$(top_srcdir)/src/daemon \
	-ggdb

pkgdir		= $(libdir)/odeviced/plugins
rtcconfdir	= $(datadir)/odeviced/plugins
rtcconf_DATA   = rtc.plugin	

pkg_LTLIBRARIES	= rtc.la
rtc_la_SOURCES	= rtc.c \
		  rtc.h

rtc_la_LIBADD = \
	$(GLIB_LIBS) \
	$(DBUS_LIBS) \
	$(GMODULE_LIBS)

rtc_la_LDFLAGS      = -module -avoid-version
rtc_la_DEPENDENCIES = $(top_builddir)/config.h
