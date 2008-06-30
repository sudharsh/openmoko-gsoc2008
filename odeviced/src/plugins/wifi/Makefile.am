AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(GMODULE_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-ggdb


pkgdir		= $(libdir)/odeviced/plugins
wificonfdir	= $(datadir)/odeviced/plugins
wificonf_DATA   = wifi.plugin	

pkg_LTLIBRARIES	= wifi.la

wifi_VALASOURCES = wifi.vala

wifi_la_SOURCES = \
	wifi-helpers.c \
	$(wifi_VALASOURCES:.vala=.c) \
	$(wifi_VALASOURCES:.vala=.h)

BUILT_SOURCES = wifi.vala.stamp


wifi.vala.stamp: $(wifi_VALASOURCES)
	$(VALAC) --ccode --vapidir $(top_srcdir)/vapi --pkg odeviced --vapidir ./ --pkg wifi-helpers --pkg dbus-glib-1 --pkg gmodule-2.0 --pkg gio-2.0  \
	$^
	touch $@


wifi_la_LIBADD = \
	$(GLIB_LIBS) \
	$(DBUS_LIBS) \
	$(GMODULE_LIBS)

wifi_la_LDFLAGS      = -module -avoid-version
wifi_la_DEPENDENCIES = $(top_builddir)/config.h

EXTRA_DIST = \
	$(wifi_VALASOURCES) \
	wifi.vala.stamp