
bin_PROGRAMS = guake-indicator
guake_indicator_SOURCES = \
	guake-indicator.c guake-indicator.h \
	guake-indicator-read-json.c guake-indicator-read-json.h \
	guake-indicator-notify.c guake-indicator-notify.h \
	guake-indicator-dbus.c guake-indicator-dbus.h \
	guake-indicator-write-json.c guake-indicator-read-json.h \
	guake-indicator-new-entry.c guake-indicator-new-group.c guake-indicator-new-group.h \
	guake-indicator-delete-entry-group.c guake-indicator-delete-entry-group.h
	
guake_indicator_CPPFLAGS = `pkg-config --cflags gtk+-2.0` `pkg-config --cflags appindicator-0.1` `pkg-config --cflags json-c` -I/usr/include/dbus-1.0 -I/usr/lib/x86_64-linux-gnu/dbus-1.0/include -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include 
LIBS=-lappindicator -lgtk-x11-2.0 -lgdk_pixbuf-2.0 -lgobject-2.0 `pkg-config --libs json-c` -lglib-2.0 -ldl -ldbus-glib-1

