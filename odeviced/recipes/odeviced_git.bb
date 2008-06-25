DESCRIPTION = "Open Device Daemon - C implementation"
HOMEPAGE = "http://www.freesmartphone.org/"
AUTHOR = "Sudharshan S"
SECTION = "console/network"
DEPENDS = "dbus dbus-glib"
LICENSE = "LGPL"
#PV = "0.1+gitr${AUTOREV}"
PV = "0.1"
PR = "r0"

#SRC_URI = "${FREESMARTPHONE_GIT}/openmoko-gsoc2008.git;protocol=git;branch=master \
#	   "file://odeviced"
#S = "${WORKDIR}/git/odeviced"

SRC_URI = "file:///home/sudharsh/Projects/openmoko-gsoc2008/ \
	   file://odeviced"
	  
S = "${WORKDIR}/openmoko-gsoc2008/odeviced"

inherit autotools pkgconfig update-rc.d

INITSCRIPT_NAME = "odeviced"
INITSCRIPT_PARAMS = "defaults 21"

# install init script for people who want to manually
# start/stop it, but don't add runlevels.
do_install_append() {
	install -d ${D}${sysconfdir}/init.d
	install -m 0755 ${WORKDIR}/odeviced ${D}${sysconfdir}/init.d/
}

FILES_${PN} += "${sysconfdir}"
