AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(GMODULE_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-ggdb


pkgdir		= $(libdir)/fsod/subsystems/Device
rtcconfdir	= $(datadir)/fsod/subsystems/Device
rtcconf_DATA   = rtc.plugin	

pkg_LTLIBRARIES	= rtc.la

rtc_VALASOURCES = rtc.vala

rtc_la_SOURCES = \
	rtc-helpers.c \
	$(rtc_VALASOURCES:.vala=.c) \
	$(rtc_VALASOURCES:.vala=.h)

BUILT_SOURCES = rtc.vala.stamp


rtc.vala.stamp: $(rtc_VALASOURCES)
	$(VALAC) --ccode --vapidir $(top_srcdir)/vapi --pkg fsod --vapidir ../ --pkg Device --vapidir ./ --pkg rtc-helpers --pkg dbus-glib-1 --pkg gmodule-2.0 --pkg gio-2.0  \
	$^
	touch $@


rtc_la_LIBADD = \
	$(GLIB_LIBS) \
	$(DBUS_LIBS) \
	$(GMODULE_LIBS)

rtc_la_LDFLAGS      = -module -avoid-version
rtc_la_DEPENDENCIES = $(top_builddir)/config.h

EXTRA_DIST = \
	$(rtc_VALASOURCES) \
	rtc.vala.stamp

CLEANFILES = \
	rtc.c \
	rtc.h \
	rtc.vala.stamp
