AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(GMODULE_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-ggdb


pkgdir		= $(libdir)/fsod/subsystems/Device
infoconfdir	= $(datadir)/fsod/subsystems/Device
infoconf_DATA   = info.plugin	

pkg_LTLIBRARIES	= info.la

info_VALASOURCES = info.vala

info_la_SOURCES = \
	$(info_VALASOURCES:.vala=.c) \
	$(info_VALASOURCES:.vala=.h)

BUILT_SOURCES = info.vala.stamp


info.vala.stamp: $(info_VALASOURCES)
	$(VALAC) --ccode --vapidir $(top_srcdir)/vapi --pkg fsod --vapidir ../ --pkg Device --pkg dbus-glib-1 --pkg gmodule-2.0 --pkg gio-2.0  \
	$^
	touch $@


info_la_LIBADD = \
	$(GLIB_LIBS) \
	$(DBUS_LIBS) \
	$(GMODULE_LIBS)

info_la_LDFLAGS      = -module -avoid-version
info_la_DEPENDENCIES = $(top_builddir)/config.h

EXTRA_DIST = \
	$(info_VALASOURCES) \
	info.vala.stamp

CLEANFILES = \
	info.c \
	info.h \
	info.vala.stamp
