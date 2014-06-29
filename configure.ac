AC_INIT([guake-indicator], [0.4], [gun101@email.it])

# safety check , make sure that config is not run from outer space
AC_CONFIG_SRCDIR([src/guake-indicator.c])
AC_CONFIG_SRCDIR([src/guake-indicator-read-json.c])
AM_INIT_AUTOMAKE([-Wall -Werror])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
#AC_CHECK_PROGS([GUAKE], [guake guake-prefs], [:])
#if test "$GUAKE" = :; then
#AC_MSG_ERROR([This package needs guake.])
#fi

# Checks for libraries.
AC_CHECK_FUNCS([bzero]) 
AC_CHECK_FUNCS([mkdir]) 
AC_CHECK_FUNCS([strdup])
AC_CHECK_FUNCS([strerror])

AC_CHECK_LIB([appindicator], [app_indicator_new], [APPINDICATORLIB=-lindicator], [AC_MSG_ERROR([This package needs libappindicator.])])
AC_SUBST([APPINDICATORLIB])
AC_CHECK_LIB([gtk-x11-2.0], [gtk_action_group_new], [GTKLIB=-lgtk-x11-2.0], [AC_MSG_ERROR([This package needs gtk2.0.])])
AC_SUBST([GTKLIB])

# Checks for header files.
AC_CHECK_HEADER([gtk/gtk.h])
AC_CHECK_HEADER([libappindicator/app-indicator.h])
AC_CHECK_HEADER([json-c/json.h])
AC_CHECK_HEADER([glib.h])
AC_CHECK_HEADER([dbus/dbus-glib.h])

# Checks for typedefs, structures, and compiler characteristics.
# Checks for library functions.
# Output files.

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile])

AC_OUTPUT
