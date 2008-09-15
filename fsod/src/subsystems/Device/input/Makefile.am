AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(GMODULE_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-ggdb


pkgdir		= $(libdir)/fsod/subsystems/Device
inputconfdir	= $(datadir)/fsod/subsystems/Device
inputconf_DATA   = input.plugin	

pkg_LTLIBRARIES	= input.la

input_VALASOURCES = input.vala

input_la_SOURCES = \
	input-helpers.c \
	$(input_VALASOURCES:.vala=.c) \
	$(input_VALASOURCES:.vala=.h)

BUILT_SOURCES = input.vala.stamp


input.vala.stamp: $(input_VALASOURCES)
	$(VALAC) --basedir $(top_srcdir) --ccode --vapidir $(top_srcdir)/vapi --pkg fsod --vapidir $(top_srcdir)/ --pkg Device --vapidir ./ --pkg input-helpers --pkg dbus-glib-1 --pkg gmodule-2.0 --pkg gio-2.0  \
	$^
	touch $@


input_la_LIBADD = \
	$(GLIB_LIBS) \
	$(DBUS_LIBS) \
	$(GMODULE_LIBS)

input_la_LDFLAGS      = -module -avoid-version
input_la_DEPENDENCIES = $(top_builddir)/config.h

EXTRA_DIST = \
	$(input_VALASOURCES) \
	input.vala.stamp

CLEANFILES = \
	input.c \
	input.h \
	input.vala.stamp
