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
*/

#include <guake-indicator-notify.h>
#include <guake-indicator.h>

int guake_notify(gchar* title,gchar* message )
{
	void *handle, *n;

	typedef void  (*notify_init_t)(char *);
	typedef void *(*notify_notification_new_t)( char *, char *, char *, char *);
	typedef void  (*notify_notification_set_timeout_t)( void *, int );
	typedef void (*notify_notification_show_t)(void *, char *);

	handle= dlopen(LIBNOTIFY_SHARED_OBJECT, RTLD_LAZY);
	if (handle==NULL)
		return -1;

	notify_init_t init = (notify_init_t)dlsym(handle, "notify_init");
	if (init==NULL)
	{
		dlclose(handle);
		return -1;
	}
	init("Basics");
	notify_notification_new_t notification_function_ptr = (notify_notification_new_t)dlsym(handle, "notify_notification_new");
	if (notification_function_ptr==NULL)
	{
		dlclose(handle);
		return -1;
	}

	n = notification_function_ptr(title, message, DATADIR"/"GUAKE_INDICATOR_ICON_DIR"/guake-indicator.png", NULL);

	notify_notification_show_t show = (notify_notification_show_t)dlsym(handle, "notify_notification_show");
	if (show==NULL)
	{
		dlclose( handle );
		return 1;
	}
	show(n, NULL );
	return 0;
}
