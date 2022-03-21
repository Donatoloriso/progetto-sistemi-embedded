DESCRIPTION = "esercizio sulla concorrenza du più thread. Il problema di SANTA CLAUS."
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://SantaClaus.c"

S = "${WORKDIR}"

do_compile() {
	${CC} SantaClaus.c ${LDFLAGS} -o santaclaus
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 santaclaus ${D}${bindir}
}
