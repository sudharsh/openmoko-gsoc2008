DESCRIPTION = "Open Device Daemon - C implementation"
HOMEPAGE = "http://www.freesmartphone.org/"
AUTHOR = "Sudharshan S"
SECTION = "console/network"
DEPENDS = "dbus dbus-glib"
RDEPENDS = "dbus dbus-glib"
LICENSE = "LGPL"
PV = "0.1+gitr${AUTOREV}"
PR = "r0"

SRC_URI = "${FREESMARTPHONE_GIT}/openmoko-gsoc2008.git;protocol=git;branch=master"
S = "${WORKDIR}/git/odeviced"

inherit autotools pkgconfig

# install init script for people who want to manually
# start/stop it, but don't add runlevels.
#do_append_install() {
#	install -d ${D}${sysconfdir}/init.d
#	install -m 0755 etc/init.d/odeviced ${D}${sysconfdir}/init.d/
#}

#FILES_${PN} += "${datadir} ${sysconfdir}"
