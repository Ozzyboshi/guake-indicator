/*
Copyright (C) 2013-2015 Alessio Garzi <gun101@email.it>
Copyright (C) 2013-2015 Francesco Minà <mina.francesco@gmail.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public
License along with this program; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.
* */

#include <glib.h>
#include <dbus/dbus-glib.h>


#define GUAKE_URI "org.guake.RemoteControl"
#define GUAKE_PATH "/org/guake/RemoteControl"
#define GUAKE_IFACE "org.guake.RemoteControl"

DBusGProxy* guake_dbus_init();
gboolean guake_dbus_send(const gchar*,const gchar*);
gboolean guake_dbus_send_intparam_with_string_return(const gchar*,gint32,gchar**);
gboolean guake_dbus_send_noparams_with_integer_return(const gchar*,gint32*);
gboolean guake_dbus_send_noparam(const gchar*);

gboolean guake_show();
gboolean guake_newtab();
gboolean guake_gettabcount(gint32*);
gboolean guake_getgtktabname(guint,gchar**);
gboolean guake_selecttab(const gchar*);
gboolean guake_renamecurrenttab(const gchar*);
gboolean guake_executecommand(const char*);

