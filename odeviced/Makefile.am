NULL = 

AUTOMAKE_OPTIONS = subdir-objects


SUBDIRS = \
	po \
	src \
	$(NULL)

AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(GMODULE_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-ggdb \
	$(NULL)

BUILT_SOURCES = src/daemon/odeviced.vala.stamp 

bin_PROGRAMS = odeviced

odeviced_VALASOURCES = \
	src/daemon/helpers.vala \
	src/daemon/plugin.vala \
	src/daemon/odeviced.vala \
	src/daemon/service.vala \
	$(NULL)

odeviced_SOURCES = \
	$(odeviced_VALASOURCES:.vala=.c) \
	$(odeviced_VALASOURCES:.vala=.h) \
	$(NULL)


src/daemon/odeviced.vala.stamp: $(odeviced_VALASOURCES)
	$(VALAC) -C --pkg dbus-glib-1 --pkg gmodule-2.0 --pkg gio-2.0 --basedir $(top_srcdir) $^
	touch $@

odeviced_LDADD = \
	$(GLIB_LIBS) \
	$(DBUS_LIBS) \
	$(GMODULE_LIBS) \
	$(NULL)

dbussystemdir = $(DBUS_SYSTEM_DIR)
dbussystem_DATA = etc/dbus-1/system.d/Device.conf

odevicedconfdir = /etc
odevicedconf_DATA = etc/odeviced.conf

EXTRA_DIST = \
	intltool-extract.in \
	intltool-update.in \
	intltool-merge.in \
	$(odeviced_VALASOURCES) \
	src/daemon/odeviced.vala.stamp \
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
	src/daemon/*.c \
	src/daemon/*.h \
	src/daemon/*.stamp \
	$(NULL)

