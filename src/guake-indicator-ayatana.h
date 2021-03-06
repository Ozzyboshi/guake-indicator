/*
Copyright (C) 2013-2019 Alessio Garzi <gun101@email.it>
Copyright (C) 2013-2019 Francesco Minà <mina.francesco@gmail.com>

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

#ifndef _GUAKE_INDICATOR_AYATANA_H_
#define _GUAKE_INDICATOR_AYATANA_H_

#include <libayatana-appindicator/app-indicator.h>

#define DEFAULT_ICON "guake-indicator"

static void scroll_event_cb (AppIndicator * , gint , guint , gpointer );


static void append_submenu (GtkWidget *,Host*);

static void activate_clicked_cb (GtkWidget *, gpointer );

static void label_toggle_cb (GtkWidget * , gpointer );

static void apply_css();
static void gtk3_build_menu(GtkInfo*);

static void gtk3_close_guake_indicator (GtkAction *,gpointer);
static void gtk3_reload (GtkAction*,gpointer);
static void gtk3_about (GtkAction*,gpointer );

gboolean gtk3_detect_clickbutton(GtkWidget *, GdkEventButton *, gpointer );


static GMainLoop * mainloop = NULL;
static gboolean active = TRUE;

static const gchar CSS[] =
 ".titlebar { "
	 " border-radius: 0px 0px 0px 0px; "
	 " background-color: gray;"
	 " border: 0px; "
	 " padding: 0px; "
	 " margin: 0px; "
 "}";

static GUAKE_INDICATOR_DEFAULT_ACTION GUAKE_INDICATOR_DEFAULT_MENUITEMS_ARRAY_GTK3[]=
{
        {GUAKE_INDICATOR_DEFAULT_MENUITEMS_ARRAY[0],print_edit_menu_form},
        {GUAKE_INDICATOR_DEFAULT_MENUITEMS_ARRAY[1],gtk3_reload},
        {GUAKE_INDICATOR_DEFAULT_MENUITEMS_ARRAY[2],gtk3_close_guake_indicator},
        {GUAKE_INDICATOR_DEFAULT_MENUITEMS_ARRAY[3],gtk3_about}
};

#endif //_GUAKE_INDICATOR_AYATANA_H_