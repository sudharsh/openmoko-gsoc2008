
AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(GMODULE_CFLAGS) \
	$(PYTHON_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-ggdb


pkgdir = $(libdir)/fsod/subsystems
pkg_LTLIBRARIES	= PythonManager.la

PythonManager_VALASOURCES = PythonManager.vala

PythonManager_la_SOURCES = \
	$(PythonManager_VALASOURCES:.vala=.c) \
	$(PythonManager_VALASOURCES:.vala=.h) \
	fsod-python-plugin.c \
	fsod-python-plugin.h

BUILT_SOURCES = PythonManager.vala.stamp


PythonManager.vala.stamp: $(PythonManager_VALASOURCES)
	$(VALAC) -C --basedir $(top_srcdir) --vapidir $(top_srcdir)/vapi --pkg fsod --pkg dbus-glib-1 --pkg gmodule-2.0 --pkg gio-2.0  \
	$^
	touch $@


PythonManager_la_LIBADD = \
	$(GLIB_LIBS) \
	$(DBUS_LIBS) \
	$(PYTHON_LIBS) \
	$(GMODULE_LIBS)

PythonManager_la_LDFLAGS      = -module -avoid-version
PythonManager_la_DEPENDENCIES = $(top_builddir)/config.h

EXTRA_DIST = \
	$(PythonManager_VALASOURCES) \
	PythonManager.vala.stamp

CLEANFILES = \
	PythonManager.c \
	PythonManager.h \
	PythonManager.vala.stamp