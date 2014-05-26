VERSION = 20140525
DEFINES += APPV=\"\\\"$${VERSION}\\\"\"

isEmpty(PREFIX) {
    PREFIX                           = /usr/local
}

isEmpty(LIBDIR) {
	LIBDIR				= $${PREFIX}/lib$${LIBSUFFIX}
}

isEmpty(DBUS_CONF) {
    DBUS_CONF                        = /etc
}

isEmpty(DBUS_SERVICE) {
    DBUS_SERVICE                     = /usr/share
}

isEmpty(INCLUDEDIR) {
    INCLUDEDIR                   = $${PREFIX}/include
}

isEmpty(DOCDIR) {
    DOCDIR                           = $$PREFIX/share/doc
}

isEmpty(ADMIN_GROUP) {
    ADMIN_GROUP                      = wheel
}

message("PREFIX: $$PREFIX")
message("LIBDIR: $$LIBDIR")
message("DBUS_CONF: $${DBUS_CONF}")
message("DBUS_SERVICE: $${DBUS_SERVICE}")
message("LIBSUFFIX: $${LIBSUFFIX}")
message("INCLUDEDIR: $${INCLUDEDIR}")
message("DOCDIR: $${DOCDIR}")
message("ADMIN_GROUP: $${ADMIN_GROUP}")
