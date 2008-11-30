
AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(GMODULE_CFLAGS) \
	$(PYTHON_CFLAGS) \
	$(PYGOBJECT_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-DDATADIR=\"$(datadir)\"           \
	-DSYSCONFDIR=\""$(sysconfdir)"\"   \
	-DLIBDIR=\""$(libdir)"\"                      
	-ggdb


pythonmodulesdir = $(libdir)/fsod/subsystems/Python
pythonmodules_DATA = test.py


pkgdir = $(libdir)/fsod/subsystems
pkg_LTLIBRARIES	= PythonManager.la

PythonManager_VALASOURCES = PythonManager.vala

PythonManager_la_SOURCES = \
	$(PythonManager_VALASOURCES:.vala=.c) \
	$(PythonManager_VALASOURCES:.vala=.h) \
	fsod-python-plugin.c \
	fsod-python-plugin.h

BUILT_SOURCES = PythonManager.vala.stamp

VALA_FLAGS = \
	--vapidir $(top_srcdir)/vapi \
   	--pkg fsod \
   	--pkg dbus-glib-1 \
   	--pkg gmodule-2.0 \
	--pkg gio-2.0 \
	--pkg config \
	--vapidir ./ --pkg fsod-python-plugin

PythonManager.vala.stamp: $(PythonManager_VALASOURCES)
	$(VALAC) -C --basedir $(top_srcdir)  $(VALA_FLAGS) $^
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
	pythonmodules_DATA \
	PythonManager.vala.stamp 


CLEANFILES = \
	PythonManager.c \
	PythonManager.h \
	PythonManager.vala.stamp

