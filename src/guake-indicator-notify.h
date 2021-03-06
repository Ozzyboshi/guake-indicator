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

#ifndef _GUAKE_INDICATOR_NOTIFY_H
#define _GUAKE_INDICATOR_NOTIFY_H

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <glib.h>

#define LIBNOTIFY_SHARED_OBJECT "libnotify.so.4"

int guake_notify(gchar*,gchar*);

#endif //_GUAKE_INDICATOR_NOTIFY_H