/*
Copyright (C) 2013-2018 Alessio Garzi <gun101@email.it>
Copyright (C) 2013-2018 Francesco Minà <mina.francesco@gmail.com>

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

DBusGProxy* guake_dbus_init()
{
	DBusGConnection* bus;
	GError* error = NULL;
	DBusGProxy* remoteValue;
	
	bus = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
	if (error != NULL)
	{
		g_printf("Couldn't connect to the Session bus %s", error->message);
		g_error_free (error);
		return NULL;
	}
		
	remoteValue =dbus_g_proxy_new_for_name(bus,GUAKE3?GUAKE3_URI:GUAKE_URI,GUAKE3?GUAKE3_PATH:GUAKE_PATH,GUAKE3?GUAKE3_IFACE:GUAKE_IFACE);
	if (remoteValue == NULL) {
		g_printf("Couldn't create the proxy object");
		return NULL;
	}
	return remoteValue;
}

// Send a integer to the dbus system and get a string in return
gboolean guake_dbus_send_intparam_with_string_return(const gchar* functname,gint32 param,gchar** ret)
{
	GError* error = NULL;
	gchar* strret;
	
	DBusGProxy* remoteValue = guake_dbus_init();
	if (remoteValue==NULL)
		return FALSE;
	
	if (!dbus_g_proxy_call (remoteValue, functname,&error,G_TYPE_INT, param,G_TYPE_INVALID,G_TYPE_STRING,&strret,G_TYPE_INVALID))
	{
		g_printerr ("Error: %s\n", error->message);
		g_error_free (error);
		return FALSE;
	}
	*ret=strret;
	return TRUE;
}

// Get a string from the dbus system
gboolean guake_dbus_send_noparams_with_string_return(const gchar* functname,gchar** ret)
{
	GError* error = NULL;
	gchar* strret;
	
	DBusGProxy* remoteValue = guake_dbus_init();
	if (remoteValue==NULL)
		return FALSE;
	
	if (!dbus_g_proxy_call (remoteValue, functname,&error,G_TYPE_INVALID,G_TYPE_STRING,&strret,G_TYPE_INVALID))
	{
		g_printerr ("Error: %s\n", error->message);
		g_error_free (error);
		return FALSE;
	}
	*ret=strret;
	return TRUE;
}

// Get an integer from the dbus system
gboolean guake_dbus_send_noparams_with_integer_return(const gchar* functname,gint32* ret)
{
	GError* error = NULL;
	gboolean returnValue;
	
	DBusGProxy* remoteValue = guake_dbus_init();
	if (remoteValue==NULL)
		return FALSE;
	
	*ret=-1;
	
	returnValue = dbus_g_proxy_call (remoteValue, functname,&error,G_TYPE_INVALID,G_TYPE_INT, ret,G_TYPE_INVALID);
	if (!returnValue)
	{
		//g_printerr ("Error: %s\n", error->message);
		g_error_free (error);
		return FALSE;
	}
	return TRUE;
}

gboolean guake_dbus_send(const gchar* functname,const gchar* param)
{
	GError* error = NULL;
	gboolean returnValue;
	DBusGProxy* remoteValue = guake_dbus_init();
	if (remoteValue==NULL)
		return FALSE;

	if (param==NULL)
		returnValue = dbus_g_proxy_call (remoteValue, functname,&error,G_TYPE_INVALID,G_TYPE_INVALID);
	else
		returnValue = dbus_g_proxy_call (remoteValue, functname,&error,G_TYPE_STRING,param,G_TYPE_INVALID,G_TYPE_INVALID);
	
	if (!returnValue)
	{
		g_error_free (error);
		return FALSE;
	}
	return TRUE;
}

gboolean guake_dbus_send_2strings(const gchar* functname,const gchar* param,const gchar* param2)
{
	GError* error = NULL;
	gboolean returnValue;
	DBusGProxy* remoteValue = guake_dbus_init();
	if (remoteValue==NULL)
		return FALSE;

	returnValue = dbus_g_proxy_call (remoteValue, functname,&error,G_TYPE_STRING,param,G_TYPE_STRING,param2,G_TYPE_INVALID,G_TYPE_INVALID);
	
	if (!returnValue)
	{
		g_error_free (error);
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
gboolean guake_newtab(gchar** name)
{
	//return guake_dbus_send_noparam("add_tab");
	return guake_dbus_send_noparams_with_string_return("add_tab",name);
}

// It opens the nth Guake tab
gboolean guake_selecttab(const gchar* param)
{
	return guake_dbus_send("select_tab",param);
}

// Get number of Guake tabs
gboolean guake_gettabcount(gint32* numtabs)
{
	return guake_dbus_send_noparams_with_integer_return("get_tab_count",numtabs);
}

// Get number of the nth Guake tab
gboolean guake_getgtktabname(guint tabindex,gchar** name)
{
	return guake_dbus_send_intparam_with_string_return("get_gtktab_name",tabindex,name);
}

// Rename the current Guake tab
gboolean guake_renamecurrenttab(const gchar* param)
{
	return guake_dbus_send("rename_current_tab",param);
}

// Execute a command on a Guake shell
gboolean guake_executecommand(const char* command)
{
	return guake_dbus_send("execute_command",command);
}

// Execute a command on a Guake shell by uuid
gboolean guake_executecommand_by_uuid(gchar* uuid,const char* command)
{
	return guake_dbus_send_2strings("execute_command_by_uuid",uuid,command);
}

//Get current tab uuid
gboolean guake_getcurrenttab_uuid(gchar** uuid)
{
	return guake_dbus_send_noparams_with_string_return("get_selected_uuidtab",uuid);
}
