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

BUILT_SOURCES = src/fsod.vala.stamp


##-----------------------##
# fsod - main executable  #
##-----------------------##
bin_PROGRAMS = fsod

fsodconfdir = $(sysconfdir)
fsodconf_DATA = etc/fsod.conf

dbussystemdir = $(DBUS_SYSTEM_DIR)
dbussystem_DATA = etc/dbus-1/system.d/fsod.conf

fsod_VALASOURCES = \
	src/main.vala \
	src/options.vala

fsod_SOURCES = \
	$(fsod_VALASOURCES:.vala=.c) \
	$(fsod_VALASOURCES:.vala=.h) \
	$(NULL)


src/fsod.vala.stamp: $(fsod_VALASOURCES)
	$(VALAC) -C --basedir $(top_srcdir) --vapidir $(top_srcdir)/vapi --pkg fsod --pkg dbus-glib-1 --pkg gmodule-2.0 --pkg gio-2.0 $^
	touch $@


fsod_LDADD = \
	$(GLIB_LIBS) \
	$(DBUS_LIBS) \
	$(GMODULE_LIBS) \
	src/libfsod.la \
	$(NULL)

fsod_LDFLAGS = -export-dynamic -no-undefined -export-symbols-regex "^[[^_]].*"



EXTRA_DIST = \
	intltool-extract.in \
	intltool-update.in \
	intltool-merge.in \
	$(fsod_VALASOURCES) \
	$(libfsod_la_VALASOURCES) \
	src/libfsod.vala.stamp \
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

