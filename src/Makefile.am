
bin_PROGRAMS = guake-indicator
guake_indicator_SOURCES = guake-indicator.c guake-indicator.h guake-indicator-read-json.c guake-indicator-read-json.h guake-indicator-notify.c guake-indicator-notify.h guake-indicator-dbus.c guake-indicator-dbus.h
#guake_LIBRARIES = `pkg-config --cflags gtk+-2.0`
guake_indicator_CPPFLAGS = `pkg-config --cflags gtk+-2.0` `pkg-config --cflags appindicator-0.1` `pkg-config --cflags json-c` -I/usr/include/dbus-1.0 -I/usr/lib/x86_64-linux-gnu/dbus-1.0/include -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include 
LIBS=`pkg-config --libs gtk+-2.0` `pkg-config --libs appindicator-0.1` `pkg-config --libs json-c` `pkg-config --libs glib-2.0` -ldl -ldbus-1 -ldbus-glib-1
#guake_indicator_LDADD =""
#AM_LDFLAGS           =  `pkg-config --libs gtk+-2.0` `pkg-config --libs appindicator-0.1` `pkg-config --libs json-c`
#guake_indicator_LDADD = `pkg-config --libs gtk+-2.0` `pkg-config --libs appindicator-0.1` `pkg-config --libs json-c` `pkg-config --libs glib-2.0`
