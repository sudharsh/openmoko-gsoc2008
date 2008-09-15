AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(GMODULE_CFLAGS) \
	$(GSTREAMER_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-ggdb


pkgdir		= $(libdir)fsod/subsystems/Device
audioconfdir	= $(datadir)fsod/subsystems/Device
audioconf_DATA   = audio.plugin	

pkg_LTLIBRARIES	= audio.la

audio_VALASOURCES = audio.vala

audio_la_SOURCES = \
	$(audio_VALASOURCES:.vala=.c) \
	$(audio_VALASOURCES:.vala=.h)

BUILT_SOURCES = audio.vala.stamp


audio.vala.stamp: $(audio_VALASOURCES)
	$(VALAC) --ccode --vapidir $(top_srcdir)/vapi --pkg odeviced --pkg dbus-glib-1 --pkg gmodule-2.0 --pkg gio-2.0 --pkg gstreamer-0.10 \
	$^
	touch $@


audio_la_LIBADD = \
	$(GLIB_LIBS) \
	$(DBUS_LIBS) \
	$(GMODULE_LIBS) \
	$(GSTREAMER_LIBS)

audio_la_LDFLAGS      = -module -avoid-version
audio_la_DEPENDENCIES = $(top_builddir)/config.h

EXTRA_DIST = \
	$(audio_VALASOURCES) \
	audio.vala.stamp

CLEANFILES = \
	audio.c \
	audio.h \
	audio.vala.stamp
