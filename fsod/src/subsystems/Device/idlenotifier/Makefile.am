AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(GMODULE_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-DDATADIR=\"$(datadir)\"           \
	-DSYSCONFDIR=\""$(sysconfdir)"\"   \
	-DLIBDIR=\""$(libdir)"\"	   \
	-ggdb


pkgdir		= $(libdir)/fsod/subsystems/Device
idlenotifierconfdir	= $(datadir)/fsod/subsystems/Device
idlenotifierconf_DATA   = idlenotifier.plugin	

pkg_LTLIBRARIES	= idlenotifier.la

idlenotifier_VALASOURCES = idlenotifier.vala

idlenotifier_la_SOURCES = \
	idlenotifier-helpers.c \
	$(idlenotifier_VALASOURCES:.vala=.c) \
	$(idlenotifier_VALASOURCES:.vala=.h)

BUILT_SOURCES = idlenotifier.vala.stamp


idlenotifier.vala.stamp: $(idlenotifier_VALASOURCES)
	$(VALAC) --ccode --vapidir $(top_srcdir)/vapi --pkg fsod --pkg config --vapidir ../ --pkg Device --vapidir ./ --pkg idlenotifier-helpers --pkg dbus-glib-1 --pkg gmodule-2.0 --pkg gio-2.0  \
	$^
	touch $@


idlenotifier_la_LIBADD = \
	$(GLIB_LIBS) \
	$(DBUS_LIBS) \
	$(GMODULE_LIBS)

idlenotifier_la_LDFLAGS      = -module -avoid-version
idlenotifier_la_DEPENDENCIES = $(top_builddir)/config.h

EXTRA_DIST = \
	$(idlenotifier_VALASOURCES) \
	idlenotifier.vala.stamp

CLEANFILES = \
	idlenotifier.c \
	idlenotifier.h \
	idlenotifier.vala.stamp
