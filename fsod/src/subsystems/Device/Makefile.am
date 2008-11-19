
AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(GMODULE_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-DDATADIR=\"$(datadir)\"           \
	-DSYSCONFDIR=\""$(sysconfdir)"\"   \
	-DLIBDIR=\""$(libdir)"\"                      
	-ggdb


pkgdir = $(libdir)/fsod/subsystems
pkg_LTLIBRARIES	= Device.la

deviceconfdir	= $(datadir)/fsod/subsystems
deviceconf_DATA   = Device.conf	

Device_VALASOURCES = Device.vala \
		helpers.vala

Device_la_SOURCES = \
	$(Device_VALASOURCES:.vala=.c) \
	$(Device_VALASOURCES:.vala=.h)

BUILT_SOURCES = Device.vala.stamp

VALA_FLAGS = \
	--vapidir $(top_srcdir)/vapi \
   	--pkg fsod \
   	--pkg dbus-glib-1 \
   	--pkg gmodule-2.0 \
	--pkg gio-2.0 \
	--pkg config

Device.vala.stamp: $(Device_VALASOURCES)
	$(VALAC) -C --basedir $(top_srcdir) --library  $(top_srcdir)/vapi/Device $(VALA_FLAGS) $^
	touch $@


Device_la_LIBADD = \
	$(GLIB_LIBS) \
	$(DBUS_LIBS) \
	$(GMODULE_LIBS)

Device_la_LDFLAGS      = -module -avoid-version
Device_la_DEPENDENCIES = $(top_builddir)/config.h

EXTRA_DIST = \
	$(Device_VALASOURCES) \
	Device.vala.stamp

CLEANFILES = \
	Device.c \
	Device.h \
	Device.vala.stamp


SUBDIRS = \
	input \
	powercontrol \
	power \
	idlenotifier \
	accelerometer \
	rtc \
	led \
	display
