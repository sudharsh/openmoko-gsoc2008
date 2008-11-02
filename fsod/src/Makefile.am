AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(GMODULE_CFLAGS) \
	$(DBUS_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-ggdb \
	$(NULL)

BUILT_SOURCES = libfsod.vala.stamp


##-------------------------------##
# libfsod convenience library     #
##-------------------------------##
noinst_LTLIBRARIES	= libfsod.la

libfsod_la_VALASOURCES = \
	fsod.vala \
	subsystem.vala \
	$(NULL)


nodist_libfsod_la_SOURCES = \
	$(libfsod_la_VALASOURCES:.vala=.c) \
	$(libfsod_la_VALASOURCES:.vala=.h) \
	$(NULL)

libfsod.vala.stamp: $(libfsod_la_VALASOURCES)
	$(VALAC) -C --library $(top_srcdir)/vapi/fsod --basedir $(top_srcdir) --pkg dbus-glib-1 --pkg gmodule-2.0 --pkg gio-2.0 $^
	touch $@

libfsod_la_LIBADD = \
	$(GLIB_LIBS) \
	$(DBUS_LIBS) \
	$(GMODULE_LIBS) \
	$(NULL)

#if ENABLE_PYTHON
#libfsod_la_LIBADD += subsystems/Python/bindings/libpyfsod.la \
#	$(PYTHON_LIBS)
#
#endif

libfsod_la_LDFLAGS      =  -export-dynamic -no-undefined -export-symbols-regex "^[[^_]].*"
libfsod_la_DEPENDENCIES = $(top_builddir)/config.h


SUBDIRS = \
	subsystems
