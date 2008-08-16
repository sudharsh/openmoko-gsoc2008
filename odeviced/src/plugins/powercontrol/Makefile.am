AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(GMODULE_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-ggdb


pkgdir		= $(libdir)/odeviced/plugins
powercontrolconfdir	= $(datadir)/odeviced/plugins
powercontrolconf_DATA   = powercontrol.plugin	

pkg_LTLIBRARIES	= powercontrol.la

powercontrol_VALASOURCES = powercontrol.vala 

powercontrol_la_SOURCES = \
	powercontrol-helpers.c \
	$(powercontrol_VALASOURCES:.vala=.c) \
	$(powercontrol_VALASOURCES:.vala=.h)

BUILT_SOURCES = powercontrol.vala.stamp


powercontrol.vala.stamp: $(powercontrol_VALASOURCES)
	$(VALAC) --ccode --vapidir $(top_srcdir)/vapi --pkg odeviced --vapidir ./ --pkg powercontrol-helpers --pkg dbus-glib-1 --pkg gmodule-2.0 --pkg gio-2.0  \
	$^
	touch $@


powercontrol_la_LIBADD = \
	$(GLIB_LIBS) \
	$(DBUS_LIBS) \
	$(GMODULE_LIBS)

powercontrol_la_LDFLAGS      = -module -avoid-version
powercontrol_la_DEPENDENCIES = $(top_builddir)/config.h

EXTRA_DIST = \
	$(powercontrol_VALASOURCES) \
	powercontrol.vala.stamp

CLEANFILES = \
	powercontrol.c \
	powercontrol.h \
	powercontrol.vala.stamp
