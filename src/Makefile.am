
bin_PROGRAMS = guake-indicator
guake_indicator_SOURCES = \
	guake-indicator.c guake-indicator.h \
	guake-indicator-read-json.c guake-indicator-read-json.h \
	guake-indicator-notify.c guake-indicator-notify.h \
	guake-indicator-dbus.c guake-indicator-dbus.h \
	guake-indicator-write-json.c guake-indicator-read-json.h \
	guake-indicator-edit-menu.c guake-indicator-edit-menu.h guake-indicator-write-xml.c guake-indicator-xml.h \
	guake-indicator-read-xml.c
	
guake_indicator_CPPFLAGS = -Wunused-variable `pkg-config --cflags gtk+-2.0` `pkg-config --cflags appindicator-0.1` `pkg-config --cflags json-c` `pkg-config --cflags libxml-2.0` -I/usr/include/dbus-1.0 -I/usr/lib/x86_64-linux-gnu/dbus-1.0/include -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/gconf/2
LIBS=-lappindicator -lgtk-x11-2.0 -lgdk_pixbuf-2.0 -lgobject-2.0 `pkg-config --libs json-c` -lglib-2.0 -ldl -ldbus-glib-1 -lxml2 -lgconf-2
AM_CFLAGS = -DDATADIR=\"$(datadir)\" -DPYTHONDATADIR=\"$(pythondir)\"

