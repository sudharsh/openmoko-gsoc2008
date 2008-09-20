
AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(GMODULE_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-ggdb


pkgdir = $(libdir)/fsod/subsystems
pkg_LTLIBRARIES	= Time.la


Time_VALASOURCES = Time.vala

Time_la_SOURCES = \
	$(Time_VALASOURCES:.vala=.c) \
	$(Time_VALASOURCES:.vala=.h)

BUILT_SOURCES = Time.vala.stamp


Time.vala.stamp: $(Time_VALASOURCES)
	$(VALAC) -C --basedir $(top_srcdir) --vapidir $(top_srcdir)/vapi --pkg fsod --pkg dbus-glib-1 --pkg gmodule-2.0 --pkg gio-2.0 \
	$^
	touch $@


Time_la_LIBADD = \
	$(GLIB_LIBS) \
	$(DBUS_LIBS) \
	$(GMODULE_LIBS)

Time_la_LDFLAGS      = -module -avoid-version
Time_la_DEPENDENCIES = $(top_builddir)/config.h

EXTRA_DIST = \
	$(Time_VALASOURCES) \
	Time.vala.stamp

CLEANFILES = \
	Time.c \
	Time.h \
	Time.vala.stamp


