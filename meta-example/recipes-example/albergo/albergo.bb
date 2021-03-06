DESCRIPTION = "Applicazione che usa la libreria pthread. Prenotazione camere in un albergo."
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://albergo.c"

S = "${WORKDIR}"


do_compile() {
	${CC} albergo.c ${LDFLAGS} -o albergo
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 albergo ${D}${bindir}
}
