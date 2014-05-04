/*
Copyright (C) 2013-2014 Alessio Garzi <gun101@email.it>

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
*/

#include "guake-indicator-dbus.h"

gboolean guake_dbus_send(const gchar* functname,const gchar* param)
{
	DBusGConnection* bus;
	GError* error = NULL;
	DBusGProxy* remoteValue;
	gboolean returnValue;
	
	bus = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
	
	if (error != NULL)
	{
		g_printf("Couldn't connect to the Session bus %s", error->message);
		return FALSE;
	}
		
	remoteValue =
    dbus_g_proxy_new_for_name(bus,GUAKE_URI,GUAKE_PATH,GUAKE_IFACE);
                              
    if (remoteValue == NULL) {
		g_printf("Couldn't create the proxy object");
		return FALSE;
	}
	
	if (param==NULL)
		returnValue = dbus_g_proxy_call (remoteValue, functname,&error,G_TYPE_INVALID,G_TYPE_INVALID);
	else
		returnValue = dbus_g_proxy_call (remoteValue, functname,&error,G_TYPE_STRING,param,G_TYPE_INVALID,G_TYPE_INVALID);
	
	if (!returnValue)
	{
		g_printf("function call failed : '%s'",error->message);
		return FALSE;
	}
	return TRUE;
}

gboolean guake_dbus_send_noparam(const gchar* funcname)
{
	return guake_dbus_send(funcname,NULL);
}

// It shows Guake
gboolean guake_show()
{
	// Show is not supported on old guake versions, so i set a fallback on show_hide
	if (guake_dbus_send_noparam("show")==FALSE)
		return guake_dbus_send_noparam("show_hide");
	return TRUE;
}

// It opens a new Guake tab
gboolean guake_newtab()
{
	return guake_dbus_send_noparam("add_tab");
}

// It renames the current Guake tab
gboolean guake_renamecurrenttab(const gchar* param)
{
	return guake_dbus_send("rename_current_tab",param);
}

// It execute a command on a Guake shell
gboolean guake_executecommand(const char* command)
{
	return guake_dbus_send("execute_command",command);
}
