
bin_PROGRAMS = guake-indicator
guake_indicator_SOURCES = \
	guake-indicator-ayatana.c guake-indicator-ayatana.h \
	guake-indicator.c guake-indicator.h \
	guake-indicator-read-json.c guake-indicator-read-json.h \
	guake-indicator-notify.c guake-indicator-notify.h \
	guake-indicator-dbus.c guake-indicator-dbus.h \
	guake-indicator-write-json.c guake-indicator-write-json.h \
	guake-indicator-write-xml.c guake-indicator-xml.h \
	guake-indicator-edit-menu.c guake-indicator-edit-menu.h \
	guake-indicator-read-xml.c guake3.h
	
guake_indicator_CPPFLAGS = -Wunused-variable `pkg-config --cflags ayatana-appindicator3-0.1` `pkg-config --cflags json-c` `pkg-config --cflags libxml-2.0` -I/usr/include/dbus-1.0 -I/usr/lib/x86_64-linux-gnu/dbus-1.0/include -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include 
LIBS=-layatana-appindicator3 `pkg-config --libs gtk+-3.0` `pkg-config --libs json-c` -lglib-2.0 -lgio-2.0 -ldl -ldbus-glib-1 -lxml2
AM_CFLAGS = -DDATADIR=\"$(datadir)\" -DPYTHONDATADIR=\"$(pythondir)\"

