DESCRIPTION = "Open Device Daemon from freesmartphone.org"
HOMEPAGE = "http://freesmartphone.org"
AUTHOR = "Sudharshan S"
LICENSE = "LGPL"
PV = "0.0+gitr${SRCPV}"
PR = "r0"

DEPENDS = "dbus-glib glib-2.0 dbus"
SRC_URI = "${FREESMARTPHONE_GIT}/openmoko-gsoc2008.git;protocol=git;branch=master"
S = "${WORKDIR}/git/odeviced"

inherit autotools pkgconfig

EXTRA_OECONF = "--prefix=/usr"

PACKAGES =+ "odeviced-plugins"

FILES_${PN} = "${sysconfdir} \
	      "${bindir}/* \
	      "

FILES_odeviced-plugins = "${libdir}/odeviced/plugins/* \
		          ${sysconfdir}/odeviced/plugins/*.plugin \
			  "

