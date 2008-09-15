AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(GMODULE_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-ggdb


pkgdir		= $(libdir)/fsod/subsystems/Device
powerconfdir	= $(datadir)/fsod/subsystems/Device
powerconf_DATA   = power.plugin	

pkg_LTLIBRARIES	= power.la

power_VALASOURCES = power.vala

power_la_SOURCES = \
	power-helpers.c \
	$(power_VALASOURCES:.vala=.c) \
	$(power_VALASOURCES:.vala=.h)

BUILT_SOURCES = power.vala.stamp


power.vala.stamp: $(power_VALASOURCES)
	$(VALAC) --ccode --vapidir $(top_srcdir)/vapi --pkg fsod --vapidir ../ --pkg Device --vapidir ./ --pkg power-helpers --pkg dbus-glib-1 --pkg gmodule-2.0 --pkg gio-2.0  \
	$^
	touch $@


power_la_LIBADD = \
	$(GLIB_LIBS) \
	$(DBUS_LIBS) \
	$(GMODULE_LIBS)

power_la_LDFLAGS      = -module -avoid-version
power_la_DEPENDENCIES = $(top_builddir)/config.h

EXTRA_DIST = \
	$(power_VALASOURCES) \
	power.vala.stamp

CLEANFILES = \
	power.c \
	power.h \
	power.vala.stamp
