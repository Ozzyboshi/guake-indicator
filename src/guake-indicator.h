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
* */

#include <glib.h>
#include <gtk/gtk.h>

typedef struct Host {
	gchar* id;
	gboolean label;
	gchar* protocol;
	gchar* hostname;
	gchar* login;
	gchar* menu_name;
	gchar* tab_name;
	gchar* command_after_login;
	gchar* remote_command;
	gchar* x_forwarded;
	struct Host* next;
	struct Host* group_head;
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
} GtkInfo;

static void reload_action(GtkAction*,gpointer);
static void activate_action (GtkAction* );
static void guake_open(GtkAction*,gpointer);
static void guake_open_with_show(GtkAction*,gpointer);
static void group_guake_open(GtkAction*,gpointer);
static void about(GtkAction*);

gchar* add_host_to_menu(Host*,GtkActionGroup *);
gchar* add_lable_to_menu(HostGroup*,GtkActionGroup *);
void create_default_actions(GtkActionGroup*,GtkInfo*);
gchar* create_actionlists(GArray*,GtkUIManager*,GtkActionGroup*);
int findguakepid();
