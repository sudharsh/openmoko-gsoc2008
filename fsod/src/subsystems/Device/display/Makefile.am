AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(GMODULE_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-ggdb


pkgdir		= $(libdir)/fsod/subsystems/Device
displayconfdir	= $(datadir)/fsod/subsystems/Device
displayconf_DATA   = display.plugin	

pkg_LTLIBRARIES	= display.la

display_VALASOURCES = display.vala

display_la_SOURCES = \
	display-helpers.c \
	$(display_VALASOURCES:.vala=.c) \
	$(display_VALASOURCES:.vala=.h)

BUILT_SOURCES = display.vala.stamp


display.vala.stamp: $(display_VALASOURCES)
	$(VALAC) --basedir $(top_srcdir) --ccode --vapidir $(top_srcdir)/vapi --pkg fsod --vapidir $(top_srcdir)/ --pkg Device --vapidir ./ --pkg display-helpers --pkg dbus-glib-1 --pkg gmodule-2.0 --pkg gio-2.0  \
	$^
	touch $@


display_la_LIBADD = \
	$(GLIB_LIBS) \
	$(DBUS_LIBS) \
	$(GMODULE_LIBS)

display_la_LDFLAGS      = -module -avoid-version
display_la_DEPENDENCIES = $(top_builddir)/config.h

EXTRA_DIST = \
	$(display_VALASOURCES) \
	display.vala.stamp

CLEANFILES = \
	display.c \
	display.h \
	display.vala.stamp
