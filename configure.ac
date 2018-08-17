AC_INIT([guake-indicator], [1.4], [gun101@email.it])

# safety check , make sure that config is not run from outer space
AC_CONFIG_SRCDIR([src/guake-indicator.c])
AC_CONFIG_SRCDIR([src/guake-indicator-read-json.c])
AM_INIT_AUTOMAKE([-Wall -Werror])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AM_PATH_PYTHON(2.5.0)


# Checks for libraries.
AC_CHECK_FUNCS([bzero]) 
AC_CHECK_FUNCS([mkdir]) 
AC_CHECK_FUNCS([strdup])
AC_CHECK_FUNCS([strerror])

AC_CHECK_LIB([ayatana-appindicator3], [app_indicator_new], [APPINDICATORLIB=-layatana-appindicator3], [AC_MSG_ERROR([This package needs Ayatana appindicator libraries.])])
AC_SUBST([APPINDICATORLIB])

###########################
# Ayatana dependencies
###########################
GTK3_REQUIRED_VERSION=2.91
LIBXML_REQUIRED=2.6.27

PKG_CHECK_MODULES(LIBRARY,  [gtk+-3.0 >= $GTK3_REQUIRED_VERSION,libxml-2.0 >= $LIBXML_REQUIRED])  

# Checks for header files.
my_CPPFLAGS=-I/usr/include/libxml2
AC_SUBST([my_CPPFLAGS])

AC_CHECK_HEADER([gtk/gtk.h])
AC_CHECK_HEADER([libayatana-appindicator/app-indicator.h])
AC_CHECK_HEADER([json-c/json.h],[],[AC_MSG_ERROR([libjson-c-dev not installed)])])
AC_CHECK_HEADER([glib.h])
AC_CHECK_HEADER([dbus/dbus-glib.h])

# Checks for typedefs, structures, and compiler characteristics.
# Checks for library functions.
# Output files.

# needed to install gconf schema
GLIB_GSETTINGS

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile data/Makefile plugins/Makefile data/icons/Makefile])

AC_OUTPUT
