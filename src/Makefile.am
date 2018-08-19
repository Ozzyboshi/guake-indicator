NULL =

bin_PROGRAMS = guake-indicator
guake_indicator_SOURCES =  					\
                           guake-indicator.c			\
                           guake-indicator-ayatana.c		\
                           guake-indicator-read-json.c		\
                           guake-indicator-notify.c		\
                           guake-indicator-dbus.c		\
                           guake-indicator-write-json.c		\
                           guake-indicator-edit-menu.c		\
                           guake-indicator-write-xml.c		\
                           guake-indicator-read-xml.c		\
                           $(NULL)

guake_indicator_CPPFLAGS = 					\
                           -Wunused-variable			\
                           $(GTK_CFLAGS)			\
                           $(AYATANA_APPINDICATOR_CFLAGS)	\
                           $(JSONC_CFLAGS)			\
                           $(XML2_CFLAGS)			\
                           $(DBUSGLIB_CFLAGS)			\
                           $(GLIB_CFLAGS) 			\
                           $(NULL)

LIBS =                      					\
                           $(GTK_LIBS)				\
                           $(AYATANA_APPINDICATOR_LIBS)		\
                           $(JSONC_LIBS)			\
                           $(XML2_LIBS)				\
                           $(DBUSGLIB_LIBS)			\
                           $(GLIB_LIBS)				\
                           -ldl					\
                           $(NULL)

AM_CFLAGS =                					\
                           -DDATADIR=\"$(datadir)\"		\
                           -DPYTHONDATADIR=\"$(pythondir)\"	\
                           $(NULL)
