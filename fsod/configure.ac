AC_INIT([fsod], [0.1.0], [sudharsh@gmail.com], [fsod])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS(config.h)
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([dist-bzip2])
AM_MAINTAINER_MODE

AC_PROG_CC
AM_PROG_CC_C_O
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

AC_PATH_PROG(VALAC, valac, valac)
AC_SUBST(VALAC)

AH_TEMPLATE([GETTEXT_PACKAGE], [Package name for gettext])
GETTEXT_PACKAGE=fsod
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE")
AC_SUBST(GETTEXT_PACKAGE)
AM_GLIB_GNU_GETTEXT
IT_PROG_INTLTOOL([0.35.0])

AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)

GLIB_REQUIRED=2.12.0

PKG_CHECK_MODULES(GLIB, glib-2.0 >= $GLIB_REQUIRED gobject-2.0 >= $GLIB_REQUIRED gio-2.0)
PKG_CHECK_MODULES(DBUS, dbus-1 dbus-glib-1)
PKG_CHECK_MODULES(GMODULE, gmodule-2.0)
PKG_CHECK_MODULES(GSTREAMER, gstreamer-0.10) 
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

AC_SUBST(DBUS_CFLAGS)
AC_SUBST(DBUS_LIBS)

AC_SUBST(GMODULE_CFLAGS)
AC_SUBST(GMODULE_LIBS)

AC_SUBST(GSTREAMER_CFLAGS)
AC_SUBST(GSTREAMER_LIBS)

dnl Some variables
AS_AC_EXPAND(DATADIR, $datadir)
AS_AC_EXPAND(LIBDIR, $libdir)


dnl dbus
DBUS_GLIB_BIN="`$PKG_CONFIG --variable=exec_prefix dbus-glib-1`/bin"
AC_ARG_WITH(dbusbindir,[  --with-dbusbindir=DIR  dbus tools in DIR], [DBUS_GLIB_BIN="$withval"])
echo "DBus tools location ${DBUS_GLIB_BIN}"
AC_SUBST(DBUS_GLIB_BIN)

DBUS_SERVICES_DIR="$DATADIR/dbus-1/services"
AC_SUBST(DBUS_SERVICES_DIR)
AC_DEFINE_UNQUOTED(DBUS_SERVICES_DIR, "$DBUS_SERVICES_DIR", [Where services dir for dbus is])

DBUS_SYSTEM_DIR="/etc/dbus-1/system.d"
AC_SUBST(DBUS_SYSTEM_DIR)
AC_DEFINE_UNQUOTED(DBUS_SYSTEM_DIR, "$DBUS_SYSTEM_DIR", [Where system.d dir is])


dnl Python Plugin support, copied rather shamelessy from gedit
AC_MSG_CHECKING([whether python support is requested])
AC_ARG_ENABLE([python],
	AS_HELP_STRING([--enable-python],[Enable python plugin support]),
	[enable_python=$enableval have_python=$enableval],
	[enable_python=autodetect have_python=no])
AC_MSG_RESULT([$enable_python])

if test "x$have_python" != "xno"; then
	AM_PATH_PYTHON([2.5],[],[no])
	if test "x$PYTHON" = "x:"; then
		have_python=no
	fi
fi

if test "x$have_python" != "xno"; then
	AM_CHECK_PYTHON_HEADERS([],[have_python=no])
fi

if test "x$have_python" != "xno"; then
	PY_PREFIX=`$PYTHON -c 'import sys ; print sys.prefix'`
	PY_EXEC_PREFIX=`$PYTHON -c 'import sys ; print sys.exec_prefix'`
	PYTHON_LIBS="-lpython$PYTHON_VERSION"
	PYTHON_LIB_LOC="-L$PY_EXEC_PREFIX/lib/python$PYTHON_VERSION/config"
	PYTHON_CFLAGS="-I$PY_PREFIX/include/python$PYTHON_VERSION"
	PYTHON_MAKEFILE="$PY_EXEC_PREFIX/lib/python$PYTHON_VERSION/config/Makefile"
	PYTHON_LOCALMODLIBS=`sed -n -e 's/^LOCALMODLIBS=\(.*\)/\1/p' $PYTHON_MAKEFILE`
	PYTHON_BASEMODLIBS=`sed -n -e 's/^BASEMODLIBS=\(.*\)/\1/p' $PYTHON_MAKEFILE`
	PYTHON_OTHER_LIBS=`sed -n -e 's/^LIBS=\(.*\)/\1/p' $PYTHON_MAKEFILE`
	PYTHON_EXTRA_LIBS="$PYTHON_LOCALMODLIBS $PYTHON_BASEMODLIBS $PYTHON_OTHER_LIBS"
	AC_SUBST([PYTHON_LIBS])
	AC_SUBST([PYTHON_LIB_LOC])
	AC_SUBST([PYTHON_CFLAGS])
	AC_SUBST([PYTHON_EXTRA_LIBS])
	FSOD_PYTHON_PLUGINS_PATH="$LIBDIR/fsod/subsystems/Python"
	AC_SUBST(FSOD_PYTHON_PLUGINS_PATH)	
	echo "FSOD python scripts location ${FSOD_PYTHON_PLUGINS_PATH}"
	AC_DEFINE_UNQUOTED(FSOD_PYTHON_PLUGINS_PATH, "$FSOD_PYTHON_PLUGINS_PATH", [Where fsod python scripts are located])
fi	

if test "x$have_python" != "xyes"; then
	if test "x$enable_python" = "xyes"; then
		AC_MSG_ERROR([Python not found])
	elif test "x$enable_python" = "xautodetect"; then
		enable_python=no
		AC_MSG_WARN([Python support not requested])
	fi
fi

if test "x$have_python" != "xno" -a "x$enable_python" != "xno"; then
	enable_python=yes
	AC_DEFINE([ENABLE_PYTHON],[1],[Define to compile with python support])
fi

AM_CONDITIONAL([ENABLE_PYTHON],[test "x$enable_python" = "xyes"])

dnl This allows the bug-report script to know whether python has been enabled
#AC_SUBST(enable_python)


AC_CONFIG_FILES([Makefile
	po/Makefile.in])

AC_OUTPUT([
	src/Makefile
	src/subsystems/Makefile
        src/subsystems/Device/Makefile
	src/subsystems/Device/input/Makefile
	src/subsystems/Device/idlenotifier/Makefile
	src/subsystems/Device/power/Makefile
	src/subsystems/Device/powercontrol/Makefile
	src/subsystems/Device/accelerometer/Makefile
	src/subsystems/Device/audio/Makefile
	src/subsystems/Device/rtc/Makefile
	src/subsystems/Device/led/Makefile
	src/subsystems/Device/display/Makefile
	src/subsystems/Python/Makefile
	])
