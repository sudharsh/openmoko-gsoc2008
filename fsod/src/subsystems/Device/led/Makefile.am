AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(GMODULE_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-ggdb


pkgdir		= $(libdir)fsod/subsystems/Device
ledconfdir	= $(datadir)fsod/subsystems/Device
ledconf_DATA   = led.plugin	

pkg_LTLIBRARIES	= led.la

led_VALASOURCES = led.vala

led_la_SOURCES = \
	$(led_VALASOURCES:.vala=.c) \
	$(led_VALASOURCES:.vala=.h)

BUILT_SOURCES = led.vala.stamp


led.vala.stamp: $(led_VALASOURCES)
	$(VALAC) --ccode --vapidir $(top_srcdir)/vapi --pkg odeviced --pkg dbus-glib-1 --pkg gmodule-2.0 --pkg gio-2.0  \
	$^
	touch $@


led_la_LIBADD = \
	$(GLIB_LIBS) \
	$(DBUS_LIBS) \
	$(GMODULE_LIBS)

led_la_LDFLAGS      = -module -avoid-version
led_la_DEPENDENCIES = $(top_builddir)/config.h

EXTRA_DIST = \
	$(led_VALASOURCES) \
	led.vala.stamp

CLEANFILES = \
	led.c \
	led.h \
	led.vala.stamp
