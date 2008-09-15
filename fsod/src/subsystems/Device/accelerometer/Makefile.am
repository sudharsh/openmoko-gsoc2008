AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(GMODULE_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-ggdb


pkgdir		= $(libdir)/fsod/subsystems/Device
accelerometerconfdir	= $(datadir)/fsod/subsystems/Device
accelerometerconf_DATA   = accelerometer.plugin	

pkg_LTLIBRARIES	= accelerometer.la

accelerometer_VALASOURCES = accelerometer.vala

accelerometer_la_SOURCES = \
	accelerometer-helpers.c \
	$(accelerometer_VALASOURCES:.vala=.c) \
	$(accelerometer_VALASOURCES:.vala=.h)

BUILT_SOURCES = accelerometer.vala.stamp


accelerometer.vala.stamp: $(accelerometer_VALASOURCES)
	$(VALAC) --ccode --vapidir $(top_srcdir)/vapi --pkg fsod --vapidir ../ --pkg Device --vapidir ./ --pkg accelerometer-helpers --pkg dbus-glib-1 --pkg gmodule-2.0 --pkg gio-2.0  \
	$^
	touch $@


accelerometer_la_LIBADD = \
	$(GLIB_LIBS) \
	$(DBUS_LIBS) \
	$(GMODULE_LIBS)

accelerometer_la_LDFLAGS      = -module -avoid-version
accelerometer_la_DEPENDENCIES = $(top_builddir)/config.h

EXTRA_DIST = \
	$(accelerometer_VALASOURCES) \
	accelerometer.vala.stamp

CLEANFILES = \
	accelerometer.c \
	accelerometer.h \
	accelerometer.vala.stamp
