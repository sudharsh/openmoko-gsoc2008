AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(GMODULE_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-ggdb


pkgdir		= $(libdir)/odeviced/plugins
inputconfdir	= $(datadir)/odeviced/plugins
inputconf_DATA   = input.plugin	

pkg_LTLIBRARIES	= input.la

input_VALASOURCES = input.vala

input_la_SOURCES = \
	input-helpers.c \
	$(input_VALASOURCES:.vala=.c) \
	$(input_VALASOURCES:.vala=.h)

BUILT_SOURCES = input.vala.stamp


input.vala.stamp: $(input_VALASOURCES)
	$(VALAC) --ccode --vapidir $(top_srcdir)/vapi --pkg odeviced --vapidir ./ --pkg input-helpers --pkg dbus-glib-1 --pkg gmodule-2.0 --pkg gio-2.0  \
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
