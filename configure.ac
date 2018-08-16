AC_INIT([guake-indicator], [1.4], [gun101@email.it])

# safety check , make sure that config is not run from outer space
AC_CONFIG_SRCDIR([src/guake-indicator.c])
AC_CONFIG_SRCDIR([src/guake-indicator-read-json.c])
AM_INIT_AUTOMAKE([-Wall -Werror])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AM_PATH_PYTHON(2.5.0)

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

###########################
# Ayatana dependencies
###########################
GTK3_REQUIRED_VERSION=2.91
GLIB_REQUIRED_VERSION=2.35.4
INDICATOR_REQUIRED_VERSION=0.6.0
GIO_REQUIRED_VERSION=2.26
DBUSMENUGTK_REQUIRED_VERSION=0.5.90


PKG_CHECK_MODULES(LIBRARY,  gtk+-3.0 >= $GTK3_REQUIRED_VERSION
                            ayatana-indicator3-0.4 >= $INDICATOR_REQUIRED_VERSION)  

# Checks for header files.
AC_CHECK_HEADER([gtk/gtk.h])
AC_CHECK_HEADER([libappindicator/app-indicator.h])
AC_CHECK_HEADER([json-c/json.h],[],[AC_MSG_ERROR([libjson-c-dev not installed)])])
AC_CHECK_HEADER([glib.h])
AC_CHECK_HEADER([dbus/dbus-glib.h])

# python checks
AC_MSG_CHECKING([for the python-urllib package])
ac_urllib2_result=`$PYTHON -c 'try:
    import urllib2
except Exception, e:
    print str(e)' 2> /dev/null`

if test -z "$ac_urllib2_result"; then
    AC_MSG_RESULT([yes])
else
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([cannot import Python module "urllib2".
    Please check if you have python-urllib2 installed. The error was:
    $ac_urllib2_results])
fi

AC_MSG_CHECKING([for the python-BeautifulSoup package])
ac_BeautifulSoup_result=`$PYTHON -c 'try:
    import BeautifulSoup
except Exception, e:
    print str(e)' 2> /dev/null`

if test -z "$ac_BeautifulSoup_result"; then
    AC_MSG_RESULT([yes])
else
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([cannot import Python module "BeautifulSoup".
    Please check if you have python-beautifulsoup installed. The error was:
    $ac_BeautifulSoup_result])
fi

AC_MSG_CHECKING([for the python-BeautifulSoup package])

# Checks for typedefs, structures, and compiler characteristics.
# Checks for library functions.
# Output files.

# needed to install gconf schema
GLIB_GSETTINGS

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile data/Makefile plugins/Makefile data/icons/Makefile])

AC_OUTPUT
