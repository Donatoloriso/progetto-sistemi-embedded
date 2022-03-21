DESCRIPTION = "Vector Normalization"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://normalize.c"

S = "${WORKDIR}"

do_compile() {
	${CC} normalize.c ${LDFLAGS} -o normalize -lm
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 normalize ${D}${bindir}
}
