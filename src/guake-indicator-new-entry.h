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

void print_new_entry_form(GtkAction*,gpointer);
static void crunch_new_entry_form_data(gpointer);
static void  host_changed(GtkWidget *,gpointer);

void label_check_toggled (GtkToggleButton*,gpointer);

typedef struct Host2GtkTree {
	gpointer user_data;
	Host* host;
	HostGroup* hostgroup;
	GtkAction* action;
	gpointer gtk_user_data;
}Host2GtkTree;

