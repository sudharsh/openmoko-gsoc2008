NULL = 

AUTOMAKE_OPTIONS = subdir-objects

SUBDIRS = \
	po \
	$(NULL)

AM_CPPFLAGS = \
	$(FRAMEWORKD_CFLAGS) \
	-include $(CONFIG_HEADER) \
	$(NULL)

BUILT_SOURCES = src/frameworkd.vala.stamp

bin_PROGRAMS = frameworkd

frameworkd_VALASOURCES = \
	src/main.vala \
	$(NULL)

frameworkd_SOURCES = \
	$(frameworkd_VALASOURCES:.vala=.c) \
	$(frameworkd_VALASOURCES:.vala=.h) \
	$(NULL)

src/frameworkd.vala.stamp: $(frameworkd_VALASOURCES)
	$(VALAC) -C --basedir $(top_srcdir) $^
	touch $@

frameworkd_LDADD = \
	$(FRAMEWORKD_LIBS) \
	$(NULL)

EXTRA_DIST = \
	intltool-extract.in \
	intltool-update.in \
	intltool-merge.in \
	$(frameworkd_VALASOURCES) \
	src/frameworkd.vala.stamp \
	$(NULL)

DISTCLEANFILES = \
	intltool-extract \
	intltool-update \
	intltool-merge \
	po/.intltool-merge-cache \
	$(NULL)

