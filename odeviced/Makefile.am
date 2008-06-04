NULL = 

AUTOMAKE_OPTIONS = subdir-objects


SUBDIRS = \
	po \
	plugins \
	$(NULL)

AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(GMODULE_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-ggdb \
	$(NULL)

BUILT_SOURCES = src/odeviced.vala.stamp 

bin_PROGRAMS = odeviced

odeviced_VALASOURCES = \
	src/helpers.vala \
	src/plugin.vala \
	src/odeviced.vala \
	src/service.vala \
	$(NULL)

odeviced_SOURCES = \
	$(odeviced_VALASOURCES:.vala=.c) \
	$(odeviced_VALASOURCES:.vala=.h) \
	$(NULL)


src/odeviced.vala.stamp: $(odeviced_VALASOURCES)
	$(VALAC) -C --pkg dbus-glib-1 --pkg gmodule-2.0 --pkg gio-2.0 --basedir $(top_srcdir) $^
	touch $@

odeviced_LDADD = \
	$(GLIB_LIBS) \
	$(DBUS_LIBS) \
	$(GMODULE_LIBS) \
	$(NULL)

dbussystemdir = $(DBUS_SYSTEM_DIR)
dbussystem_DATA = Device.conf

odevicedconfdir = /etc
odevicedconf_DATA = conf/odeviced.conf

EXTRA_DIST = \
	intltool-extract.in \
	intltool-update.in \
	intltool-merge.in \
	$(odeviced_VALASOURCES) \
	src/odeviced.vala.stamp \
	dbussystem_DATA \
	odevicedconf_DATA \
	$(NULL)

DISTCLEANFILES = \
	intltool-extract \
	intltool-update \
	intltool-merge \
	po/.intltool-merge-cache \
	$(NULL)

CLEANFILES = \
	src/*.c \
	src/*.h \
	src/*.stamp \
	$(NULL)

