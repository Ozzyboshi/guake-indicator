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
* */

#ifndef _GUAKE_INDICATOR_H
#define _GUAKE_INDICATOR_H

#include <glib.h>
#include <gtk/gtk.h>

#define GUAKE_INDICATOR_DEFAULT_DIR ".guake-indicator"
#define GUAKE_INDICATOR_PLUGIN_DIR "plugins"
#define GUAKE_INDICATOR_ICON_DIR "icons/hicolor/256x256/apps/"
#define GUAKE_INDICATOR_DCONF_SCHEMA_ROOT "org.guake-indicator.data"
#define GUAKE_INDICATOR_VERSION "1.4.3"

// Define default actions
#define GUAKE_INDICATOR_DEFAULT_MENUITEMS_ARRAY_SIZE 4
#define GUAKE_INDICATOR_DEFAULT_MENUITEMS_ARRAY_MAXLENGTH 20
static const gchar GUAKE_INDICATOR_DEFAULT_MENUITEMS_ARRAY[GUAKE_INDICATOR_DEFAULT_MENUITEMS_ARRAY_SIZE][GUAKE_INDICATOR_DEFAULT_MENUITEMS_ARRAY_MAXLENGTH]=
{
        "Edit Menu",
        "Reload",
        "Quit",
        "About"
};

typedef struct GUAKE_INDICATOR_DEFAULT_ACTION
{
        gchar const* label;
        void(* action_function)(GtkAction *,gpointer);
}GUAKE_INDICATOR_DEFAULT_ACTION;


typedef struct Host {
	gchar* id;
	gboolean label;
	gchar* hostname;
	gchar* login;
	gchar* menu_name;
	gchar* tab_name;
	gchar* command_after_login;
	gchar* remote_command;
	gchar* x_forwarded;
	gchar* dont_show_guake;
	gchar* open_in_tab;
	gboolean open_in_tab_named;
	gchar* lfcr;
	gchar* guakeindicatorscript;
	
	gboolean open_all;
	struct Host* next;
	struct Host* previous;
	struct Host* group_head;
	struct HostGroup* parent;

	gboolean force_current_tab;
	gboolean vertical_split_current_tab;
	gboolean horizontal_split_current_tab;
	gboolean force_current_split;	
	void (*right_click_funct_ptr)(GtkAction*,gpointer);
} Host;

typedef struct HostGroup {
	gchar* id;
	gchar* title;
	Host* hostarray;
	gboolean label;
}HostGroup;

typedef struct GtkInfo {
	GtkActionGroup* action_group;
	GtkUIManager * uim;
	GArray* grouphostlist;
} GtkInfo;

//void reload(GtkAction*,gpointer);
void reload(GtkInfo* gtkinfo);
static void activate_action (GtkAction* );
void guake_open(GtkAction*,gpointer);
void guake_open_with_show(GtkAction*,gpointer);
void group_guake_open(GtkAction*,gpointer);
static void close_guake ( GtkWidget *, gpointer);
static void update_json(gpointer);

void grouphostlist_free(GArray*);
void host_free(Host*);
void hostgroup_free(HostGroup*);
void error_modal_box (const char*);
void build_menu_ayatana(int , char **,GtkInfo* );
void refresh_indicator_ayatana(gpointer);
void print_edit_menu_form(GtkAction*,gpointer);
void refresh_indicator(gpointer);

#endif //_GUAKE_INDICATOR_H