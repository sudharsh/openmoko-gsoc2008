INCLUDES =	\
	$(PYTHON_CFLAGS)	\
	$(PYGOBJECT_CFLAGS)     \
	$(DBUS_CFLAGS)	\
	$(GLIB_CFLAGS)

noinst_LTLIBRARIES = libpyfsod.la

nodist_libpyfsod_la_SOURCES = fsod.c

fsod.c: fsod.defs fsod.override

CLEANFILES = fsod.c
EXTRA_DIST = fsod.defs fsod.override

.defs.c:
	($(PYGOBJECT_CODEGEN) \
	    --override $*.override \
	    --prefix py$* $*.defs) > gen-$*.c \
	&& cp gen-$*.c $*.c \
	&& rm -f gen-$*.c
