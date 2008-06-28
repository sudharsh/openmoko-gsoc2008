AC_INIT([odeviced], [0.1.0], [sudharsh@gmail.com], [odeviced])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS(config.h)
AM_INIT_AUTOMAKE([dist-bzip2])
AM_MAINTAINER_MODE

AC_PROG_CC
AM_PROG_CC_C_O
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

AC_PATH_PROG(VALAC, valac, valac)
AC_SUBST(VALAC)

AH_TEMPLATE([GETTEXT_PACKAGE], [Package name for gettext])
GETTEXT_PACKAGE=odeviced
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
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

AC_SUBST(DBUS_CFLAGS)
AC_SUBST(DBUS_LIBS)

AC_SUBST(GMODULE_CFLAGS)
AC_SUBST(GMODULE_LIBS)


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

AC_CONFIG_FILES([po/Makefile.in])

AC_OUTPUT([
Makefile
src/Makefile
src/plugins/Makefile
src/plugins/wifi/Makefile
src/plugins/rtc/Makefile
src/plugins/display/Makefile
src/plugins/power/Makefile
src/plugins/input/Makefile
])
