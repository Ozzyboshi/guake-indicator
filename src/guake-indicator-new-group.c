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

#include <string.h>
#include <gtk/gtk.h>
#include "guake-indicator.h"
#include "guake-indicator-new-entry.h"
#include "guake-indicator-read-json.h"
#include "guake-indicator-write-json.h"
#include "guake-indicator-new-group.h"

static Host2GtkTree* SELECTED_HOST = NULL;


void print_new_group_form(GtkAction* action,gpointer user_data)
{
	GtkWidget *window, *vbox, *hbox, *hbox1, *question, *label,*labellogin, *hostname,*login;
	GtkWidget * tree_view;
	GtkCellRenderer *cell_renderer;
	GtkTreeViewColumn* column;
	
	const gint HBOX_SPACING = 5;
	
	SELECTED_HOST=NULL;
	
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), "New group");
	gtk_container_set_border_width (GTK_CONTAINER (window), 10);

	// Start of menu_name creation
	GtkWidget* hbox_menu_name,*menu_name,*labelmenu_name;
	menu_name = gtk_entry_new ();
	labelmenu_name = gtk_label_new ("Menu name :");
	hbox_menu_name = gtk_hbox_new (TRUE, HBOX_SPACING);
	gtk_box_pack_start_defaults (GTK_BOX (hbox_menu_name), labelmenu_name);
	gtk_box_pack_start_defaults (GTK_BOX (hbox_menu_name), menu_name);
	// End of menu_name creation
	
	// Start of label creation
	GtkWidget* hbox_label,*labelobj,*labellabel;
	labelobj = gtk_entry_new ();
	labellabel = gtk_label_new ("Label value :");
	hbox_label = gtk_hbox_new (TRUE, HBOX_SPACING);
	gtk_box_pack_start_defaults (GTK_BOX (hbox_label), labellabel);
	gtk_box_pack_start_defaults (GTK_BOX (hbox_label), labelobj);
	// End of label creation
	
	// Start of treeview
	SELECTED_HOST=NULL;
	GtkTreeStore* tree_store = gtk_tree_store_new(1, G_TYPE_STRING);
	tree_view=gtk_tree_view_new ();
	
	cell_renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes("Ref", cell_renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view), column);

	GtkTreeIter iter;
	GtkTreeIter iter2;
	GArray* host2gtktreelist= g_array_new (TRUE, FALSE, sizeof (Host2GtkTree*));
	
	// Fetch data from the cfg file
	GArray* grouphostlist=read_json_cfg_file(NULL);
	
	gtk_tree_store_append(tree_store, &iter, NULL);
	gtk_tree_store_set(tree_store, &iter,0,"Insert on top",-1);
	Host2GtkTree* rel=(Host2GtkTree*)malloc(sizeof(Host2GtkTree));
	bzero(rel,sizeof(Host2GtkTree));
	rel->user_data=iter.user_data;
	rel->action=action;
	rel->gtk_user_data=user_data;
	g_array_append_val (host2gtktreelist, rel);
	
	int j=0;
	for (j=0;grouphostlist!=NULL && j<grouphostlist->len;j++)
	{
		HostGroup* hostgroup = g_array_index (grouphostlist, HostGroup* , j);
		if (hostgroup->label==TRUE) continue;
	
		gtk_tree_store_append(tree_store, &iter, NULL);
		gtk_tree_store_set(tree_store, &iter,0,hostgroup->title!=NULL?hostgroup->title:"Root node",-1);
		
		Host2GtkTree* rel=(Host2GtkTree*)malloc(sizeof(Host2GtkTree));
		bzero(rel,sizeof(Host2GtkTree));
		rel->user_data=iter.user_data;
		rel->hostgroup=hostgroup;
		rel->action=action;
		rel->gtk_user_data=user_data;
		g_array_append_val (host2gtktreelist, rel);
		
	}
	gtk_tree_view_set_model (GTK_TREE_VIEW (tree_view), GTK_TREE_MODEL (tree_store));
	GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
	g_signal_connect(selection, "changed", G_CALLBACK(host_changed), (gpointer)host2gtktreelist);
	
	GtkWidget * scrollbar = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollbar),
		GTK_POLICY_AUTOMATIC,
		GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scrollbar), tree_view);

        
        
	GtkWidget* hbox_treeview,*labeltreeview;
	labeltreeview = gtk_label_new ("Select where to place new group :");
	hbox_treeview = gtk_hbox_new (TRUE, HBOX_SPACING);
	gtk_widget_set_size_request (scrollbar,400,250);
	gtk_box_pack_start_defaults (GTK_BOX (hbox_treeview), labeltreeview);
	gtk_box_pack_start_defaults (GTK_BOX (hbox_treeview), scrollbar);
	// End of treeview
	
	vbox = gtk_vbox_new (FALSE, 10);

	GtkWidget* saveButton = gtk_button_new_from_stock (GTK_STOCK_SAVE);
	g_signal_connect_swapped (G_OBJECT (saveButton), "clicked",G_CALLBACK (crunch_new_group_form_data),(gpointer) window);
	GtkWidget* hbox_buttons = gtk_hbox_new (TRUE, HBOX_SPACING);
	gtk_box_pack_start_defaults (GTK_BOX (hbox_buttons), saveButton);

	// Add to vbox
	gtk_box_pack_start_defaults (GTK_BOX (vbox), hbox_menu_name);
	gtk_box_pack_start_defaults (GTK_BOX (vbox), hbox_label);
	gtk_box_pack_start_defaults (GTK_BOX (vbox), hbox_treeview);
	gtk_box_pack_start_defaults (GTK_BOX (vbox), hbox_buttons);

	gtk_container_add (GTK_CONTAINER (window), vbox);
	gtk_window_set_position (GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_widget_show_all (window);
	
	return;
}
static void  host_changed(GtkWidget *widget,gpointer data) 
{
	GArray* host2gtktreelist = (GArray*)data;
	GtkTreeIter iter;
	GtkTreeModel *model;
	char *value;

	if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(widget), &model, &iter))
	{
		gtk_tree_model_get(model, &iter, 0, &value,  -1);
	
		int i=0;
		for (i=0;host2gtktreelist!=NULL && i<host2gtktreelist->len;i++)
		{
			Host2GtkTree* ref = g_array_index (host2gtktreelist, Host2GtkTree* , i);
			if (ref->user_data==iter.user_data)
				SELECTED_HOST=ref;
		}
    
		g_free(value);
	}
}
static void crunch_new_group_form_data(gpointer window_pointer)
{
	if (SELECTED_HOST==NULL)
	{
		error_modal_box("Select where to place the new group");
		return ;
	}

	GtkWidget *window = (GtkWidget *)window_pointer;
	GtkWidget *vbox = gtk_bin_get_child(GTK_BIN(window));
	
	GList *vbox_children = gtk_container_get_children(GTK_CONTAINER(vbox));
	
	// Start fetching menu_name
	GtkWidget* hbox_menuname = (GtkWidget*) vbox_children->data;
	GList* menuname_reference = gtk_container_get_children(GTK_CONTAINER(hbox_menuname));
	menuname_reference = g_list_next(menuname_reference);
	GtkEntry* menuname_entry = (GtkEntry*) menuname_reference->data;
	const gchar* testomenuname=gtk_entry_get_text(menuname_entry);
	// End fetching menu_name
	
	vbox_children = g_list_next(vbox_children);
	
	// Start fetching label
	GtkWidget* hbox_label = (GtkWidget*) vbox_children->data;
	GList* label_reference = gtk_container_get_children(GTK_CONTAINER(hbox_label));
	label_reference = g_list_next(label_reference);
	GtkEntry* label_entry = (GtkEntry*) label_reference->data;
	const gchar* testolabel=gtk_entry_get_text(label_entry);
	// End fetching label
	
	// Fetch data from the cfg file
	GArray* grouphostlist=read_json_cfg_file(NULL);
	
	int j=-1;
	if (SELECTED_HOST->hostgroup!=NULL)
	{
		for (j=0;grouphostlist!=NULL && j<grouphostlist->len;j++)
		{
			HostGroup* hostgroup = g_array_index (grouphostlist, HostGroup* , j);
			if (!g_strcmp0(hostgroup->id,SELECTED_HOST->hostgroup->id))
			{
				break;
			}
		}
	}
	
	if (strlen((char*)testolabel)>0)
	{
		HostGroup* newhostgroup=malloc(sizeof(HostGroup));
		bzero((void*)newhostgroup,sizeof(HostGroup));
		newhostgroup->title=g_strdup(testolabel);
		newhostgroup->label=TRUE;
		g_array_insert_val(grouphostlist,++j,newhostgroup);
	}
	
	HostGroup* newhostgroup=malloc(sizeof(HostGroup));
	bzero((void*)newhostgroup,sizeof(HostGroup));
	newhostgroup->title=g_strdup(testomenuname);
	g_array_insert_val(grouphostlist,++j,newhostgroup);
	
	//printf("%s\n",(char*)write_json_cfg_file_to_str(grouphostlist));
	char* filedir,*filecfg;
	filedir=checkandcreatedefaultdir();
	asprintf(&filecfg,"%s/%s",filedir,GUAKE_INDICATOR_DEFAULT_FILEJSON);
	free((void*)filedir);
	FILE * fd=fopen(filecfg,"w");
	if (fd==NULL)
	{
			error_modal_box ("Error saving cfg file");
			gtk_widget_destroy(window);
			free((void*)filecfg);
			return ;
	}
	char* data = (char*)write_json_cfg_file_to_str(grouphostlist);
	fwrite((const void*)data,strlen(data),1,fd);
	fclose(fd);
	free((void*)filecfg);
	
	gtk_widget_destroy(window);
	fflush(stdout);
	error_modal_box ("Group saved successfully");
	reload(SELECTED_HOST->action, SELECTED_HOST->gtk_user_data);
}

