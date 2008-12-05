NULL = 
AUTOMAKE_OPTIONS = subdir-objects

BUILT_SOURCES = src/fsod.vala.stamp

SUBDIRS = \
	po \
	src \
	$(NULL)


AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(GMODULE_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-DDATADIR=\"$(datadir)\"           \
	-DSYSCONFDIR=\""$(sysconfdir)"\"   \
	-DLIBDIR=\""$(libdir)"\"	\
	-ggdb \
	$(NULL)


##-----------------------##
# fsod - main executable  #
##-----------------------##
bin_PROGRAMS = fsod

fsodconfdir = $(sysconfdir)
fsodconf_DATA = etc/fsod.conf

dbussystemdir = /etc/dbus-1/system.d
dbussystem_DATA = etc/dbus-1/system.d/fsod.conf

fsod_VALASOURCES = \
	src/main.vala \
	src/options.vala \
	src/logger.vala \
	src/fsod.vala \
	src/subsystem.vala \
	src/resource.vala \
	$(NULL)


fsod_SOURCES = \
	$(fsod_VALASOURCES:.vala=.c) \
	$(fsod_VALASOURCES:.vala=.h) \
	$(NULL)


src/fsod.vala.stamp: $(fsod_VALASOURCES)
	$(VALAC) -C --basedir $(top_srcdir) --vapidir $(top_srcdir)/vapi --library $(top_srcdir)/vapi/fsod --pkg config --pkg dbus-glib-1 --pkg gmodule-2.0 --pkg gio-2.0 $^
	touch $@


fsod_LDADD = \
	$(GLIB_LIBS) \
	$(DBUS_LIBS) \
	$(GMODULE_LIBS) \
	$(NULL)

fsod_LDFLAGS = -export-dynamic -no-undefined -export-symbols-regex "^[[^_]].*"



EXTRA_DIST = \
	intltool-extract.in \
	intltool-update.in \
	intltool-merge.in \
	$(fsod_VALASOURCES) \
	src/fsod.vala.stamp \
	fsodconf_DATA \
	dbussystem_DATA \
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

